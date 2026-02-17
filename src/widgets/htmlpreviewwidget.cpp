#include "htmlpreviewwidget.h"

#ifdef USE_QLITEHTML

#include <utils/misc.h>

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QEventLoop>
#include <QMenu>
#include <QNetworkReply>
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
        QAction *act = new QAction(tr("Copy link"), this);
        act->setEnabled(selectedText().isEmpty());
        connect(act, &QAction::triggered, this,
                [linkUrl] { qApp->clipboard()->setText(linkUrl.toString()); });
        menu.addAction(act);
    }

    menu.addAction(tr("Reset zoom"), this, [this] { setZoomFactor(1.0); });

    menu.exec(mapToGlobal(pos));
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

#endif
