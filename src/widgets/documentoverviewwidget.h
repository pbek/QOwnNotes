#pragma once

#include <QPixmap>
#include <QPointer>
#include <QVector>
#include <QWidget>

class QEvent;
class QMouseEvent;
class QPaintEvent;
class QPlainTextEdit;
class QResizeEvent;
class QTextCursor;
class QTimer;
class QWheelEvent;

class DocumentOverviewWidget : public QWidget {
    Q_OBJECT

   public:
    explicit DocumentOverviewWidget(QPlainTextEdit *noteTextEdit,
                                    QPlainTextEdit *encryptedNoteTextEdit,
                                    QWidget *parent = nullptr);

    QSize sizeHint() const override;

   protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

   private:
    struct VisualLine {
        int blockNumber;
        int textStart;
        int textLength;
    };

    QPlainTextEdit *activeTextEdit() const;
    void connectTextEdit(QPlainTextEdit *textEdit);
    void invalidateRepresentation();
    void rebuildRepresentation(QPlainTextEdit *textEdit, const QSize &size);
    void scheduleRepresentationUpdate();
    void scrollToPosition(int y);
    int visualLineForCursor(const QTextCursor &cursor) const;

    QPointer<QPlainTextEdit> _noteTextEdit;
    QPointer<QPlainTextEdit> _encryptedNoteTextEdit;
    QPointer<QPlainTextEdit> _representedTextEdit;
    QPixmap _representation;
    QVector<VisualLine> _visualLines;
    QVector<int> _blockFirstVisualLines;
    QVector<int> _blockVisualLineCounts;
    QTimer *_updateTimer;
    int _documentHeight = 0;
    bool _representationDirty = true;
    int _wheelAngleDelta = 0;
};
