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
    ~QOwnNotesMarkdownTextEdit() override;

    void setStyles();
    void openUrl(const QString &urlString) override;
    //    void setViewportMargins(int left, int top, int right, int bottom);
    void setPaperMargins(int width = -1);
    int modifyFontSize(FontModificationMode mode);
    void updateSettings();
    QMargins viewportMargins();
    void setText(const QString &text);
    static void setSpellCheckingEnabled(bool enabled);
    bool isSpellCheckingEnabled();
    void disableSpellChecking();
    bool usesMonospacedFont();

    /**
     * Toggles the case of the word under the Cursor or the selected text
     */
    void toggleCase();

    /**
     * Register this editor as the active one for AI autocomplete
     */
    void registerAsActiveEditor();

    /**
     * Unregister this editor from receiving AI autocomplete
     */
    void unregisterAsActiveEditor();

    /**
     * Get the currently active editor for AI autocomplete
     */
    static QOwnNotesMarkdownTextEdit *getActiveEditorForAutocomplete();

    /**
     * Inserts an empty code block
     */
    void insertCodeBlock();

    /**
     * Handles auto completion
     */
    void onAutoCompleteRequested();

    /**
     * Tries to find an equation in the current line and solves it
     * @return true on success
     */
    bool solveEquation(double &returnValue);

    /**
     * Inserts a block quote character or formats the selected text as block quote
     */
    void insertBlockQuote();

    /**
     * Returns the text from the current cursor to the start of the word in the
     * note text edit
     *
     * @param withPreviousCharacters also get more characters at the beginning
     *                               to get characters like "@" that are not
     *                               word-characters
     * @return
     */
    QString currentWord(bool withPreviousCharacters = false) const;

    /**
     * Tries to find words that start with the current word in the note text edit
     *
     * @param resultList
     * @return
     */
    bool autoComplete(QStringList &resultList) const;

    QString currentBlock() const;

    QSize minimumSizeHint() const;

    void updateIgnoredClickUrlRegexps();

   public slots:
    /**
     * Called when AI autocomplete is completed
     */
    void onAiAutocompleteCompleted(const QString &result);

    /**
     * Called when AI autocomplete request times out or errors
     */
    void onAiAutocompleteTimeout(const QString &errorString);

   protected:
    // we must not override _highlighter or Windows will create a
    // QOwnNotesMarkdownHighlighter and MarkdownHighlighter instance
    //    QOwnNotesMarkdownHighlighter *_highlighter;
    bool canInsertFromMimeData(const QMimeData *source) const override;
    void insertFromMimeData(const QMimeData *source) override;
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;

   private:
    /// @param in is true if zoom-in, false otherwise
    void onZoom(bool in);

    void setFormatStyle(MarkdownHighlighter::HighlighterState index);

    void onContextMenu(QPoint pos);

    void overrideFontSizeStyle(int fontSize);

    QMenu *spellCheckContextMenu(QPoint pos);

    /**
     * Requests AI autocomplete for the current text
     */
    void requestAiAutocomplete();

    /**
     * Shows the AI autocomplete suggestion
     */
    void showAiAutocompleteSuggestion(const QString &suggestion);

    /**
     * Hides/clears the AI autocomplete suggestion
     */
    void clearAiAutocompleteSuggestion();

    /**
     * Accepts the current AI autocomplete suggestion
     */
    void acceptAiAutocompleteSuggestion();

    bool _isSpellCheckingDisabled = false;
    QString _aiAutocompleteSuggestion;
    int _aiAutocompletePosition = -1;
    QTimer *_aiAutocompleteTimer = nullptr;
    bool _isInsertingAiSuggestion = false;

    // Static pointer to the currently active editor for AI autocomplete
    static QOwnNotesMarkdownTextEdit *_activeAutocompleteEditor;
};
