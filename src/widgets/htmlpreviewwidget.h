#pragma once

#ifdef USE_QLITEHTML

#include <libraries/qlitehtml/src/qlitehtmlwidget.h>
#include <QNetworkAccessManager>

class HtmlPreviewWidget final : public QLiteHtmlWidget
{
public:
    HtmlPreviewWidget(QWidget *parent);

private:
    QByteArray resourceLoadCallBack(const QUrl&);
    void onContextMenuRequested(QPoint pos, const QUrl &url);

    void wheelEvent(QWheelEvent *) override;
    bool eventFilter(QObject *src, QEvent *e) override;

    QNetworkAccessManager m_nam;
};

#endif
