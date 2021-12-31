#include "htmlpreviewwidget.h"

#ifdef USE_QLITEHTML

#include <QEventLoop>
#include <QNetworkReply>
#include <QDesktopServices>

HtmlPreviewWidget::HtmlPreviewWidget(QWidget *parent)
    : QLiteHtmlWidget(parent)
{
    auto callback = [this](const QUrl& url){
        return resourceLoadCallBack(url);
    };
    setResourceHandler(callback);

    setZoomFactor(1.5);

    connect(this, &QLiteHtmlWidget::linkClicked, this, [](const QUrl &url){
        QDesktopServices::openUrl(url);
    });
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

#endif
