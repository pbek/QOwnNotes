#pragma once

#include "helpers/qownnotesmarkdownhighlighter.h"
#include "libraries/qmarkdowntextedit/qmarkdowntextedit.h"
class MainWindow;
class QOwnSpellChecker;

#define QOWNNOTESMARKDOWNTEXTEDIT_OVERRIDE_FONT_SIZE_STYLESHEET_PRE_STRING \
    "/* BEGIN FONT SIZE OVERRIDE STYLESHEET */"
#define QOWNNOTESMARKDOWNTEXTEDIT_OVERRIDE_FONT_SIZE_STYLESHEET_POST_STRING \
    "/* END FONT SIZE OVERRIDE STYLESHEET */"

class QOwnNotesMarkdownTextEdit : public QMarkdownTextEdit {
    Q_OBJECT

   public:
    enum EditorWidthMode { Narrow = 1, Medium, Wide, Full, Custom };
    Q_ENUMS(EditorWidthMode)

    enum FontModificationMode { Increase = 1, Decrease, Reset };
    Q_ENUMS(FontModificationMode)

    explicit QOwnNotesMarkdownTextEdit(QWidget *parent = nullptr);

    void setStyles();
    void openUrl(QString urlString);
    //    void setViewportMargins(int left, int top, int right, int bottom);
    void setPaperMargins(int width = -1);
    void setMainWindow(MainWindow *mainWindow);
    int modifyFontSize(FontModificationMode mode);
    void updateSettings();
    QMargins viewportMargins();
    void setText(const QString &text);
    static void setSpellcheckingEnabled(bool enabled);

   protected:
    // we must not override _highlighter or Windows will create a
    // QOwnNotesMarkdownHighlighter and MarkdownHighlighter instance
    //    QOwnNotesMarkdownHighlighter *_highlighter;
    bool canInsertFromMimeData(const QMimeData *source) const override;
    void insertFromMimeData(const QMimeData *source);
    void resizeEvent(QResizeEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

   private slots:
    void highlightCurrentLine();

   private:
    MainWindow *mainWindow;

    void setFormatStyle(MarkdownHighlighter::HighlighterState index);

    bool onContextMenuEvent(QContextMenuEvent *event);

    void overrideFontSizeStyle(int fontSize);
   Q_SIGNALS:
    void resize(QResizeEvent *event);
};
