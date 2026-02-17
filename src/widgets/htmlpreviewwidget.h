#pragma once

#ifdef USE_QLITEHTML

#include <libraries/qlitehtml/src/qlitehtmlwidget.h>

#include <QNetworkAccessManager>

class HtmlPreviewWidget final : public QLiteHtmlWidget {
    Q_OBJECT
   public:
    HtmlPreviewWidget(QWidget *parent);
    void setHtml(const QString &text);

   Q_SIGNALS:
    void anchorClicked(const QUrl &url);

   private:
    QByteArray resourceLoadCallBack(const QUrl &);
    void onContextMenuRequested(QPoint pos, const QUrl &url);

    void wheelEvent(QWheelEvent *) override;
    bool eventFilter(QObject *src, QEvent *e) override;

    QNetworkAccessManager m_nam;

    void exportAsHTMLFile();
};

#endif
