#include "htmlpreviewwidget.h"

#ifdef USE_QLITEHTML

#include <QEventLoop>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QAction>
#include <QMenu>
#include <QApplication>
#include <QClipboard>

HtmlPreviewWidget::HtmlPreviewWidget(QWidget *parent)
    : QLiteHtmlWidget(parent)
{
    auto callback = [this](const QUrl& url){
        return resourceLoadCallBack(url);
    };
    setResourceHandler(callback);

    connect(this, &QLiteHtmlWidget::linkClicked, this, [](const QUrl &url){
        QDesktopServices::openUrl(url);
    });

    connect(this, &QLiteHtmlWidget::contextMenuRequested, this, &HtmlPreviewWidget::onContextMenuRequested);
}

QByteArray HtmlPreviewWidget::resourceLoadCallBack(const QUrl &url)
{
    if (!url.isValid())
        return {};
    m_nam.get(QNetworkRequest(url));

    QEventLoop loop;
    QByteArray data;

    QNetworkReply *reply = m_nam.get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, [&data, &loop, reply] {
        if (reply->error() == QNetworkReply::NoError)
            data = reply->readAll();
        reply->deleteLater();
        loop.exit();
    });
    loop.exec(QEventLoop::ExcludeUserInputEvents);

    return data;
}

void HtmlPreviewWidget::onContextMenuRequested(QPoint pos, const QUrl &linkUrl)
{
    QMenu menu;

    QAction *act = new QAction(tr("Copy"), this);
    act->setDisabled(selectedText().isEmpty());
    connect(act, &QAction::triggered, this, [this]{
        qApp->clipboard()->setText(selectedText());
    });
    menu.addAction(act);

    if (!linkUrl.isEmpty() && linkUrl.isValid()) {
        QAction *act = new QAction(tr("Copy Link"), this);
        act->setEnabled(selectedText().isEmpty());
        connect(act, &QAction::triggered, this, [linkUrl]{
            qApp->clipboard()->setText(linkUrl.toString());
        });
        menu.addAction(act);
    }

    menu.exec(mapToGlobal(pos));
}

#endif
