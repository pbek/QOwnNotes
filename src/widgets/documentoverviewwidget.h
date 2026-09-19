#pragma once

#include <QPixmap>
#include <QPointer>
#include <QSet>
#include <QVector>
#include <QWidget>

class QEvent;
class QHideEvent;
class QMouseEvent;
class QPainter;
class QPaintEvent;
class QPlainTextEdit;
class QResizeEvent;
class QTextCursor;
class QTimer;
class QWheelEvent;
class DocumentOverviewPreview;

class DocumentOverviewWidget : public QWidget {
    Q_OBJECT

   public:
    explicit DocumentOverviewWidget(QPlainTextEdit *noteTextEdit,
                                    QPlainTextEdit *encryptedNoteTextEdit,
                                    QWidget *parent = nullptr);

    QSize sizeHint() const override;

   protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

   private:
    friend class DocumentOverviewPreview;

    struct VisualLine {
        int blockNumber;
        int textStart;
        int textLength;
    };

    QPlainTextEdit *activeTextEdit() const;
    void connectTextEdit(QPlainTextEdit *textEdit);
    void invalidateRepresentation();
    void hidePreview();
    bool highlightPreviewBlocks(QPlainTextEdit *textEdit, int previewHeight);
    void paintPreview(QPainter &painter, const QRect &rect) const;
    void rebuildRepresentation(QPlainTextEdit *textEdit, const QSize &size);
    void scheduleRepresentationUpdate();
    void showPreview();
    void scrollToPosition(int y);
    int visualLineAtPosition(int y) const;
    int visualLineForCursor(const QTextCursor &cursor) const;

    QPointer<QPlainTextEdit> _noteTextEdit;
    QPointer<QPlainTextEdit> _encryptedNoteTextEdit;
    QPointer<QPlainTextEdit> _previewHighlightedTextEdit;
    QPointer<QPlainTextEdit> _representedTextEdit;
    QPixmap _representation;
    QVector<VisualLine> _visualLines;
    QVector<int> _blockFirstVisualLines;
    QVector<int> _blockVisualLineCounts;
    QSet<int> _previewHighlightedBlocks;
    DocumentOverviewPreview *_preview;
    QTimer *_previewTimer;
    QTimer *_updateTimer;
    int _previewAnchorY = -1;
    int _previewVisualLine = -1;
    int _documentHeight = 0;
    bool _representationDirty = true;
    bool _updatingPreview = false;
    int _wheelAngleDelta = 0;
};
