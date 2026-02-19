#include "htmlpreviewwidget.h"

#ifdef USE_QLITEHTML

#include <dialogs/filedialog.h>
#include <utils/misc.h>
#include <utils/schema.h>

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QDialog>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QMenu>
#include <QNetworkReply>
#include <QScrollBar>
#include <QTextStream>
#include <QVBoxLayout>
#include <QWheelEvent>

#include "libraries/qlitehtml/src/qlitehtmlsearchwidget.h"

// HtmlPreviewWidgetInternal implementation
HtmlPreviewWidgetInternal::HtmlPreviewWidgetInternal(QWidget *parent) : QLiteHtmlWidget(parent) {
    setStyleSheet(QStringLiteral("background-color: %1;")
                      .arg(Utils::Schema::schemaSettings
                               ->getBackgroundColor(MarkdownHighlighter::HighlighterState::NoState)
                               .name()));

    auto callback = [this](const QUrl &url) { return resourceLoadCallBack(url); };
    setResourceHandler(callback);

    connect(this, &QLiteHtmlWidget::linkClicked, this, &HtmlPreviewWidgetInternal::anchorClicked);

    connect(this, &QLiteHtmlWidget::contextMenuRequested, this,
            &HtmlPreviewWidgetInternal::onContextMenuRequested);
}

QByteArray HtmlPreviewWidgetInternal::resourceLoadCallBack(const QUrl &url) {
    if (!url.isValid()) return {};
    m_nam.get(QNetworkRequest(url));

    QEventLoop loop;
    QByteArray data;

    QNetworkReply *reply = m_nam.get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, [&data, &loop, reply] {
        if (reply->error() == QNetworkReply::NoError) data = reply->readAll();
        reply->deleteLater();
        loop.exit();
    });
    loop.exec(QEventLoop::ExcludeUserInputEvents);

    return data;
}

void HtmlPreviewWidgetInternal::onContextMenuRequested(QPoint pos, const QUrl &linkUrl) {
    QMenu menu;

    QAction *act = new QAction(tr("Copy"), this);
    act->setDisabled(selectedText().isEmpty());
    connect(act, &QAction::triggered, this, [this] { qApp->clipboard()->setText(selectedText()); });
    menu.addAction(act);

    if (!linkUrl.isEmpty() && linkUrl.isValid()) {
        QAction *copyLinkAction = new QAction(tr("Copy link location"), this);
        copyLinkAction->setEnabled(selectedText().isEmpty());
        connect(copyLinkAction, &QAction::triggered, this,
                [linkUrl] { qApp->clipboard()->setText(linkUrl.toString()); });
        menu.addAction(copyLinkAction);
    }

    menu.addSeparator();
    QAction *htmlFileExportAction = menu.addAction(tr("Export generated raw HTML"));

    menu.addAction(tr("Reset zoom"), this, [this] { setZoomFactor(1.0); });

    QAction *selectedItem = menu.exec(mapToGlobal(pos));
    if (selectedItem == htmlFileExportAction) {
        exportAsHTMLFile();
    }
}

bool HtmlPreviewWidgetInternal::eventFilter(QObject *src, QEvent *e) {
    if (e->type() == QEvent::Wheel) {
        auto we = static_cast<QWheelEvent *>(e);
        if (we->modifiers() == Qt::ControlModifier) {
            e->ignore();
            return true;
        }
    }

    return QLiteHtmlWidget::eventFilter(src, e);
}

void HtmlPreviewWidgetInternal::wheelEvent(QWheelEvent *event) {
    if (event->modifiers() != Qt::ControlModifier) return QLiteHtmlWidget::wheelEvent(event);

    event->accept();
    const int deltaY = event->angleDelta().y();
    if (deltaY > 0) {
        setZoomFactor(zoomFactor() + .1);
    } else {
        setZoomFactor(zoomFactor() - .1);
    }
}

void HtmlPreviewWidgetInternal::exportAsHTMLFile() {
    FileDialog dialog(QStringLiteral("PreviewHTMLFileExport"));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("HTML files") + " (*.html)");
    dialog.setWindowTitle(
        tr("Export preview as raw HTML file",
           "\"Raw\" means that actually the html that was fed to the preview will be stored (the "
           "QTextBrowser modifies the html that it is showing)"));
    dialog.selectFile(QStringLiteral("preview.html"));
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QString fileName = dialog.selectedFile();

        if (!fileName.isEmpty()) {
            if (QFileInfo(fileName).suffix().isEmpty()) {
                fileName.append(".html");
            }

            QFile file(fileName);

            qDebug() << "exporting raw preview html file: " << fileName;

            if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                qCritical() << file.errorString();
                return;
            }
            QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            out.setCodec("UTF-8");
#endif
            out << html();
            file.flush();
            file.close();
            Utils::Misc::openFolderSelect(fileName);
        }
    }
}

// HtmlPreviewWidget implementation
HtmlPreviewWidget::HtmlPreviewWidget(QWidget *parent)
    : QWidget(parent),
      _htmlWidget(new HtmlPreviewWidgetInternal(this)),
      _searchWidget(new QLiteHtmlSearchWidget(_htmlWidget)) {
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(_htmlWidget);
    layout->addWidget(_searchWidget);

    // Forward signals from internal widget
    connect(_htmlWidget, &HtmlPreviewWidgetInternal::anchorClicked, this,
            &HtmlPreviewWidget::anchorClicked);
}

void HtmlPreviewWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape && _searchWidget->isVisible()) {
        _searchWidget->deactivate();
        event->accept();
    } else if ((event->key() == Qt::Key_F) && event->modifiers().testFlag(Qt::ControlModifier)) {
        _searchWidget->activate();
        event->accept();
    } else if (event->key() == Qt::Key_F3) {
        _searchWidget->doSearch(!event->modifiers().testFlag(Qt::ShiftModifier));
        event->accept();
    } else {
        QWidget::keyPressEvent(event);
    }
}

QLiteHtmlSearchWidget *HtmlPreviewWidget::searchWidget() { return _searchWidget; }

void HtmlPreviewWidget::setHtml(const QString &text) {
    _htmlWidget->setHtml(Utils::Misc::parseTaskList(text, true));
}

void HtmlPreviewWidget::setZoomFactor(qreal scale) { _htmlWidget->setZoomFactor(scale); }

qreal HtmlPreviewWidget::zoomFactor() const { return _htmlWidget->zoomFactor(); }

QString HtmlPreviewWidget::selectedText() const { return _htmlWidget->selectedText(); }

void HtmlPreviewWidget::setDefaultFont(const QFont &font) { _htmlWidget->setDefaultFont(font); }

QFont HtmlPreviewWidget::defaultFont() const { return _htmlWidget->defaultFont(); }

bool HtmlPreviewWidget::findText(const QString &text, QTextDocument::FindFlags flags,
                                 bool incremental, bool *wrapped) {
    return _htmlWidget->findText(text, flags, incremental, wrapped);
}

QWidget *HtmlPreviewWidget::viewport() const { return _htmlWidget->viewport(); }

QScrollBar *HtmlPreviewWidget::verticalScrollBar() const {
    return _htmlWidget->verticalScrollBar();
}

QScrollBar *HtmlPreviewWidget::horizontalScrollBar() const {
    return _htmlWidget->horizontalScrollBar();
}

#endif
