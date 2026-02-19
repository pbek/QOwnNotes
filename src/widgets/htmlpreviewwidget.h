#pragma once

#ifdef USE_QLITEHTML

#include <libraries/qlitehtml/src/qlitehtmlwidget.h>

#include <QNetworkAccessManager>
#include <QWidget>

class QLiteHtmlSearchWidget;
class QScrollBar;

// Internal widget that handles the actual HTML rendering
class HtmlPreviewWidgetInternal final : public QLiteHtmlWidget {
    Q_OBJECT
   public:
    explicit HtmlPreviewWidgetInternal(QWidget *parent);

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

// Container widget that holds the HTML preview and search bar
class HtmlPreviewWidget final : public QWidget {
    Q_OBJECT
   public:
    HtmlPreviewWidget(QWidget *parent);
    void setHtml(const QString &text);
    QLiteHtmlSearchWidget *searchWidget();

    // Expose QLiteHtmlWidget methods
    void setZoomFactor(qreal scale);
    qreal zoomFactor() const;
    QString selectedText() const;
    void setDefaultFont(const QFont &font);
    QFont defaultFont() const;
    bool findText(const QString &text, QTextDocument::FindFlags flags, bool incremental,
                  bool *wrapped = nullptr);

    // Expose QAbstractScrollArea methods
    QWidget *viewport() const;
    QScrollBar *verticalScrollBar() const;
    QScrollBar *horizontalScrollBar() const;

   Q_SIGNALS:
    void anchorClicked(const QUrl &url);

   protected:
    void keyPressEvent(QKeyEvent *event) override;

   private:
    HtmlPreviewWidgetInternal *_htmlWidget;
    QLiteHtmlSearchWidget *_searchWidget;
};

#endif
