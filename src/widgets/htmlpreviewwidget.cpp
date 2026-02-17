#include "htmlpreviewwidget.h"

#ifdef USE_QLITEHTML

#include <dialogs/filedialog.h>
#include <utils/misc.h>

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDialog>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QMenu>
#include <QNetworkReply>
#include <QTextStream>
#include <QWheelEvent>

HtmlPreviewWidget::HtmlPreviewWidget(QWidget *parent) : QLiteHtmlWidget(parent) {
    auto callback = [this](const QUrl &url) { return resourceLoadCallBack(url); };
    setResourceHandler(callback);

    connect(this, &QLiteHtmlWidget::linkClicked, this, &HtmlPreviewWidget::anchorClicked);

    connect(this, &QLiteHtmlWidget::contextMenuRequested, this,
            &HtmlPreviewWidget::onContextMenuRequested);
}

QByteArray HtmlPreviewWidget::resourceLoadCallBack(const QUrl &url) {
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

void HtmlPreviewWidget::onContextMenuRequested(QPoint pos, const QUrl &linkUrl) {
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

bool HtmlPreviewWidget::eventFilter(QObject *src, QEvent *e) {
    if (e->type() == QEvent::Wheel) {
        auto we = static_cast<QWheelEvent *>(e);
        if (we->modifiers() == Qt::ControlModifier) {
            e->ignore();
            return true;
        }
    }

    return QLiteHtmlWidget::eventFilter(src, e);
}

void HtmlPreviewWidget::wheelEvent(QWheelEvent *event) {
    if (event->modifiers() != Qt::ControlModifier) return QLiteHtmlWidget::wheelEvent(event);

    event->accept();
    const int deltaY = event->angleDelta().y();
    if (deltaY > 0) {
        setZoomFactor(zoomFactor() + .1);
    } else {
        setZoomFactor(zoomFactor() - .1);
    }
}

void HtmlPreviewWidget::setHtml(const QString &text) {
    QLiteHtmlWidget::setHtml(Utils::Misc::parseTaskList(text, true));
}

void HtmlPreviewWidget::exportAsHTMLFile() {
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

#endif
