#pragma once

#ifdef USE_QLITEHTML

#include <libraries/qlitehtml/src/qlitehtmlwidget.h>
#include <QNetworkAccessManager>

class HtmlPreviewWidget : public QLiteHtmlWidget
{
public:
    HtmlPreviewWidget(QWidget *parent);

private:
    QByteArray resourceLoadCallBack(const QUrl&);
    void onContextMenuRequested(QPoint pos, const QUrl &url);

    QNetworkAccessManager m_nam;
};

#endif
