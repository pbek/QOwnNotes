#include <QFont>
#include <QSettings>
#include <QDebug>
#include <QRegularExpression>
#include <QMimeData>
#include <QFontDatabase>
#include <utils/schema.h>
#include <utils/gui.h>
#include <utils/misc.h>
#include "qownnotesmarkdowntextedit.h"

QOwnNotesMarkdownTextEdit::QOwnNotesMarkdownTextEdit(QWidget *parent)
        : QMarkdownTextEdit(parent, false) {
    mainWindow = Q_NULLPTR;
    spellchecker = new QOwnSpellChecker();
    _highlighter = new QOwnNotesMarkdownHighlighter(document(), spellchecker);


    setStyles();
    updateSettings();

    connect(this, SIGNAL(cursorPositionChanged()),
            this, SLOT(highlightCurrentLine()));
    highlightCurrentLine();

    QSettings settings;
    MarkdownHighlighter::HighlightingOptions options;

    if (settings.value(QStringLiteral("fullyHighlightedBlockquotes")).toBool()) {
        options |= MarkdownHighlighter::HighlightingOption
        ::FullyHighlightedBlockQuote;
    }

    // set the highlighting options
    _highlighter->setHighlightingOptions(options);

    // re-initialize the highlighting rules if we are using some options
    if (options != MarkdownHighlighter::HighlightingOption::None) {
        _highlighter->initHighlightingRules();
    }
}

QOwnNotesMarkdownTextEdit::~QOwnNotesMarkdownTextEdit()
{
}

/**
 * Sets the format style
 *
 * @param index
 * @param styles
 */
void QOwnNotesMarkdownTextEdit::setFormatStyle(
        MarkdownHighlighter::HighlighterState index) {
    QTextCharFormat format;
    Utils::Schema::schemaSettings->setFormatStyle(index, format);
    _highlighter->setTextFormat(index, format);
}

/**
 * Sets the highlighting styles for the text edit
 */
void QOwnNotesMarkdownTextEdit::setStyles() {
    QFont font = Utils::Schema::schemaSettings->getEditorTextFont();
    setFont(font);

    // set the tab stop to the width of 4 spaces in the editor
    const int tabStop = 4;
    QFontMetrics metrics(font);

#if QT_VERSION < QT_VERSION_CHECK(5,11,0)
    setTabStopWidth(tabStop * metrics.width(' '));
#else
    setTabStopDistance(tabStop * metrics.horizontalAdvance(' '));
#endif

    setFormatStyle(MarkdownHighlighter::HighlighterState::H1);
    setFormatStyle(MarkdownHighlighter::HighlighterState::H2);
    setFormatStyle(MarkdownHighlighter::HighlighterState::H3);
    setFormatStyle(MarkdownHighlighter::HighlighterState::H4);
    setFormatStyle(MarkdownHighlighter::HighlighterState::H5);
    setFormatStyle(MarkdownHighlighter::HighlighterState::H6);
    setFormatStyle(MarkdownHighlighter::HighlighterState::HorizontalRuler);
    setFormatStyle(MarkdownHighlighter::HighlighterState::List);
    setFormatStyle(MarkdownHighlighter::HighlighterState::Bold);
    setFormatStyle(MarkdownHighlighter::HighlighterState::Italic);
    setFormatStyle(MarkdownHighlighter::HighlighterState::BlockQuote);
    setFormatStyle(MarkdownHighlighter::HighlighterState::CodeBlock);
    setFormatStyle(MarkdownHighlighter::HighlighterState::Comment);
    setFormatStyle(MarkdownHighlighter::HighlighterState::MaskedSyntax);
    setFormatStyle(MarkdownHighlighter::HighlighterState::Image);
    setFormatStyle(MarkdownHighlighter::HighlighterState::InlineCodeBlock);
    setFormatStyle(MarkdownHighlighter::HighlighterState::Link);
    setFormatStyle(MarkdownHighlighter::HighlighterState::Table);
    setFormatStyle(MarkdownHighlighter::HighlighterState::BrokenLink);

#ifdef Q_OS_WIN32
    QSettings settings;

    // set the selection background color to a light blue if not in dark mode
    if (!settings.value(QStringLiteral("darkMode")).toBool()) {
        // light green (#9be29b) could be another choice, but be aware that
        // this color will be used for mouse and keyboard selections too
        setStyleSheet(styleSheet() +
                      "QWidget {selection-color: #ffffff;"
                              "selection-background-color: #3399ff}");
    }
#endif
}

/**
 * Modifies the font size of the text edit
 */
int QOwnNotesMarkdownTextEdit::modifyFontSize(FontModificationMode mode) {
    QSettings settings;
    QFont font = this->font();
    int fontSize = font.pointSize();
    bool doSetStyles = false;

    // modify the text edit default font
    QString fontString = settings.value(
            QStringLiteral("MainWindow/noteTextEdit.font")).toString();
    if (fontString != "") {
        font.fromString(fontString);

        fontSize = font.pointSize();

        switch (mode) {
            case FontModificationMode::Increase:
                fontSize++;
                doSetStyles = true;
                break;
            case FontModificationMode::Decrease:
                fontSize--;

                if (fontSize < 5) {
                    fontSize = 5;
                } else {
                    doSetStyles = true;
                }
                break;
            default:
                QPlainTextEdit textEdit;
                int newFontSize = textEdit.font().pointSize();
                if ( fontSize != newFontSize ) {
                    fontSize = newFontSize;
                    doSetStyles = true;
                }
        }

        if (fontSize > 0) {
            font.setPointSize(fontSize);
        }

        // store the font settings
        settings.setValue(QStringLiteral("MainWindow/noteTextEdit.font"), font.toString());
    }

    // modify the text edit code font
    fontString = settings.value(QStringLiteral("MainWindow/noteTextEdit.code.font")).toString();
    if (fontString != "") {
        font.fromString(fontString);

        int codeFontSize = font.pointSize();

        switch (mode) {
            case FontModificationMode::Increase:
                codeFontSize++;
                doSetStyles = true;
                break;
            case FontModificationMode::Decrease:
                codeFontSize--;

                if (codeFontSize < 5) {
                    codeFontSize = 5;
                } else {
                    doSetStyles = true;
                }
                break;
            default:
                QPlainTextEdit textEdit;
                int newCodeFontSize = textEdit.font().pointSize();
                if ( codeFontSize != newCodeFontSize ) {
                    codeFontSize = newCodeFontSize;
                    doSetStyles = true;
                }
        }

        if (codeFontSize > 0) {
            font.setPointSize(codeFontSize);
        }

        // store the font settings
        settings.setValue(QStringLiteral("MainWindow/noteTextEdit.code.font"), font.toString());
    }

    if (doSetStyles) {
        this->setStyles();
        _highlighter->rehighlight();
    }

    return fontSize;
}

/**
 * Handles clicked urls (including relative urls)
 *
 * examples:
 * - <https://www.qownnotes.org> opens the webpage
 * - <file:///path/to/my/file/QOwnNotes.pdf> opens the file
 * "/path/to/my/file/QOwnNotes.pdf" if the operating system
 * supports that handler
 */
void QOwnNotesMarkdownTextEdit::openUrl(QString urlString) {
    qDebug() << "QOwnNotesMarkdownTextEdit " << __func__ << " - 'urlString': "
        << urlString;

    QString notesPath = NoteFolder::currentLocalPath();
    QString windowsSlash = QStringLiteral("");

#ifdef Q_OS_WIN32
    // we need another slash for Windows
    windowsSlash = QStringLiteral("/");
#endif

    // parse for relative file urls and make them absolute
    urlString.replace(QRegularExpression(QStringLiteral("^file:[\\/]{2}([^\\/].+)$")),
                      QStringLiteral("file://") + windowsSlash +
                      notesPath + QStringLiteral("/\\1"));

    QMarkdownTextEdit::openUrl(urlString);
}

//void QOwnNotesMarkdownTextEdit::setViewportMargins(
//        int left, int top, int right, int bottom) {
//    QMarkdownTextEdit::setViewportMargins(left, top, right, bottom);
//}

/**
 * Sets the viewport margins for the distraction free mode
 */
void QOwnNotesMarkdownTextEdit::setPaperMargins(int width) {
    QSettings settings;
    bool isInDistractionFreeMode =
            settings.value(QStringLiteral("DistractionFreeMode/isEnabled")).toBool();
    bool editorWidthInDFMOnly =
            settings.value(QStringLiteral("Editor/editorWidthInDFMOnly"), true).toBool();

    if (isInDistractionFreeMode || !editorWidthInDFMOnly) {
        int margin = 0;

        if (width == -1) {
            width = this->width();
        }

        int editorWidthMode =
                settings.value(QStringLiteral("DistractionFreeMode/editorWidthMode")).toInt();

        if (editorWidthMode != Full) {
            QFontMetrics metrics(font());

            int characterAmount = 0;
            switch (editorWidthMode) {
                case Medium:
                    characterAmount = 80;
                    break;
                case Wide:
                    characterAmount = 100;
                    break;
                default:
                case Narrow:
                    characterAmount = 60;
                    break;
            }

            // set the size of characterAmount times the size of "O" characters
#if QT_VERSION < QT_VERSION_CHECK(5,11,0)
            int proposedEditorWidth = metrics.width(
                            QString("O").repeated(characterAmount));
#else
            int proposedEditorWidth = metrics.horizontalAdvance(
                            QStringLiteral("O").repeated(characterAmount));
#endif

            // apply a factor to correct the faulty calculated margin
            // TODO(pbek): I don't know better way to get around this yet
            proposedEditorWidth /= 1.332;

            // calculate the margin to be applied
            margin = (width - proposedEditorWidth) / 2;

            if (margin < 0) {
                margin = 0;
            }
        }

        setViewportMargins(margin, 20, margin, 0);
    } else {
        setViewportMargins(10, 10, 10, 0);
    }
}

QMargins QOwnNotesMarkdownTextEdit::viewportMargins() {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
    return QMarkdownTextEdit::viewportMargins();
#else
    return QMargins();
#endif
}

void QOwnNotesMarkdownTextEdit::setText(const QString &text) {
    QSettings settings;
    bool highlightingEnabled = settings.value(QStringLiteral("markdownHighlightingEnabled"),
                                              true).toBool();
    if (!highlightingEnabled) {
        QMarkdownTextEdit::setText(text);
        return;
    }
    QOwnNotesMarkdownHighlighter *h = dynamic_cast<QOwnNotesMarkdownHighlighter*>(_highlighter);

    //check for comment block
    if (!text.contains(QStringLiteral("<!--"))) {
        h->setCommentHighlighting(false);
    }

    //check for code blocks
    if (!text.contains(QStringLiteral("```"))) {
        h->setCodeHighlighting(false);
    }

    //check for front matter
    if (h->document()->firstBlock().text() != "---") {
        h->setFrontmatterHighlighting(false);
    }

    //check for broken links
    h->sethighlightBrokenNotesLink(false);
    // check legacy note:// links
    QRegularExpression regex(R"(note:\/\/[^\s\)>]+)");
    QRegularExpressionMatch match = regex.match(text);

    if (match.hasMatch()) {
        h->sethighlightBrokenNotesLink(true);
    }

    // else we check for <note file.md> links
    regex = QRegularExpression(QStringLiteral("<([^\\s`][^`]*?\\.[^`]*?[^\\s`]\\.md)>"));
    match = regex.match(text);

    if (match.hasMatch()) {
        h->sethighlightBrokenNotesLink(true);
    }

    // else we check for [note](note file.md) links
    regex = QRegularExpression(R"(\[[^\[\]]+\]\((\S+\.md|.+?\.md)\)\B)");
    match = regex.match(text);

    if (match.hasMatch()) {
        h->sethighlightBrokenNotesLink(true);
    }

    QMarkdownTextEdit::setText(text);

    //after we are done we turn everything back on
    h->setCodeHighlighting(true);
    h->setCommentHighlighting(true);
    h->setFrontmatterHighlighting(true);
    h->sethighlightBrokenNotesLink(true);
}

void QOwnNotesMarkdownTextEdit::resizeEvent(QResizeEvent* event) {
    emit resize(event);
    QMarkdownTextEdit::resizeEvent(event);
}

/**
 * Sets the main window for insertFromMimeData
 */
void QOwnNotesMarkdownTextEdit::setMainWindow(MainWindow *mainWindow) {
    this->mainWindow = mainWindow;
}

/**
 * Handles pasting from clipboard
 */
void QOwnNotesMarkdownTextEdit::insertFromMimeData(const QMimeData * source) {
    // if there is text in the clipboard do the normal pasting process
    if (source->hasText()) {
        QMarkdownTextEdit::insertFromMimeData(source);
    } else if (mainWindow != Q_NULLPTR) {
        // to more complex pasting if there was no text (and a main window
        // was set)
        mainWindow->handleInsertingFromMimeData(source);
    }
}

/**
 * Handles the settings of the markdown textedit
 */
void QOwnNotesMarkdownTextEdit::updateSettings() {
    // we need a blocker, otherwise the "change" events will fire
    const QSignalBlocker blocker(this);
    Q_UNUSED(blocker)

    QSettings settings;
    QMarkdownTextEdit::AutoTextOptions options;

    if (settings.value(QStringLiteral("Editor/autoBracketClosing"), true).toBool()) {
        options |= QMarkdownTextEdit::AutoTextOption::BracketClosing;
    }

    if (settings.value(QStringLiteral("Editor/autoBracketRemoval"), true).toBool()) {
        options |= QMarkdownTextEdit::AutoTextOption::BracketRemoval;
    }

    //spell check active/inactive
    bool spellcheckerActive = settings.value(QStringLiteral("checkSpelling"), true).toBool();
    spellchecker->setActive(spellcheckerActive);


    QString lang = settings.value(QStringLiteral("spellCheckLanguage"), QStringLiteral("auto")).toString();
    if (lang == QStringLiteral("auto")) {
        spellchecker->setAutoDetect(true);
    } else {
        spellchecker->setAutoDetect(false);
        spellchecker->setCurrentLanguage(lang);
    }

    setAutoTextOptions(options);

    // highlighting is always disabled for logTextEdit
    if (objectName() != QStringLiteral("logTextEdit")) {
        // enable or disable markdown highlighting
        bool highlightingEnabled = settings.value(QStringLiteral("markdownHighlightingEnabled"),
                                                  true).toBool();

        setHighlightingEnabled(highlightingEnabled);

        if (highlightingEnabled) {
            // set the new highlighting styles
            setStyles();
            _highlighter->rehighlight();
        }
    }

    _centerCursor = settings.value(QStringLiteral("Editor/centerCursor")).toBool();
    QMarkdownTextEdit::updateSettings();
}

/**
 * Highlights the current line if enabled in the settings
 */
void QOwnNotesMarkdownTextEdit::highlightCurrentLine()
{
    QSettings settings;
    if (!settings.value(QStringLiteral("Editor/highlightCurrentLine"), true).toBool()) {
        return;
    }

    QList<QTextEdit::ExtraSelection> extraSelections;

    ensureCursorVisible();
    QTextEdit::ExtraSelection selection = QTextEdit::ExtraSelection();

    QColor lineColor = Utils::Schema::schemaSettings->getBackgroundColor(
            MarkdownHighlighter::HighlighterState::
            CurrentLineBackgroundColor);

    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
//        selection.cursor.clearSelection();
//        selection.cursor.select(QTextCursor::BlockUnderCursor);
    extraSelections.append(selection);

    // be aware that extra selections, like for global searching, gets
    // removed when the current line gets highlighted
    setExtraSelections(extraSelections);
}

bool QOwnNotesMarkdownTextEdit::onContextMenuEvent(QContextMenuEvent *event) {
    //obtain the cursor at current mouse position
    QTextCursor cursorAtMouse = cursorForPosition(event->pos());
    const int mousePos = cursorAtMouse.position();

    QTextCursor cursor = textCursor();
    // Check if the user clicked a selected word
    const bool selectedWordClicked = cursor.hasSelection()
                                     && mousePos >= cursor.selectionStart()
                                     && mousePos <= cursor.selectionEnd();

    // Get the word under the (mouse-)cursor and see if it is misspelled.
    // Don't include apostrophes at the start/end of the word in the selection.
    QTextCursor wordSelectCursor(cursorAtMouse);
    wordSelectCursor.clearSelection();
    wordSelectCursor.select(QTextCursor::WordUnderCursor);
    QString selectedWord = wordSelectCursor.selectedText();

    bool isMouseCursorInsideWord = true;
    if ( (mousePos < wordSelectCursor.selectionStart()
         || mousePos >= wordSelectCursor.selectionEnd())
         && (selectedWord.length() > 1) ) {
        isMouseCursorInsideWord = false;
    }

    // Clear the selection again, we re-select it below (without the apostrophes).
    wordSelectCursor.setPosition(wordSelectCursor.position() - selectedWord.size());
    if (selectedWord.startsWith(QLatin1Char('\'')) || selectedWord.startsWith(QLatin1Char('\"'))) {
        selectedWord = selectedWord.right(selectedWord.size() - 1);
        wordSelectCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);
    }
    if (selectedWord.endsWith(QLatin1Char('\'')) || selectedWord.endsWith(QLatin1Char('\"'))) {
        selectedWord.chop(1);
    }

    wordSelectCursor.movePosition(QTextCursor::NextCharacter,
                                  QTextCursor::KeepAnchor, selectedWord.size());

    const bool wordIsMisspelled = isMouseCursorInsideWord
                                  && spellchecker
                                  && spellchecker->isActive()
                                  && !selectedWord.isEmpty()
                                  && spellchecker->isWordMisspelled(selectedWord);

    if (!selectedWordClicked) {
        // If the user clicked on a misspelled word, select that word.
        if (wordIsMisspelled) {
                setTextCursor(wordSelectCursor);
        }
        // If the user clicked somewhere else, move the cursor there.
        else {
            setTextCursor(cursorAtMouse);
        }
        cursor = textCursor();
    }

    // Use standard context menu for already selected words, correctly spelled
    // words and words inside quotes.
    if (!wordIsMisspelled || selectedWordClicked) {
        return false;
    }

    //create the suggesstion menu
    QMenu menu;
    //Add the suggestions to the menu
    const QStringList reps = spellchecker->suggestionsForWord(selectedWord, cursor, 8);
    if (reps.isEmpty()) {
        QAction *suggestionsAction = menu.addAction(tr("No suggestions for %1").arg(selectedWord));
        suggestionsAction->setEnabled(false);
    } else {
        QStringList::const_iterator end(reps.constEnd());
        for (QStringList::const_iterator it = reps.constBegin(); it != end; ++it) {
            menu.addAction(*it);
        }
    }

    menu.addSeparator();
    const QPoint &pos = event->globalPos();
    QAction *ignoreAction = menu.addAction(tr("Ignore"));
    QAction *addToDictAction = menu.addAction(tr("Add to Dictionary"));
    //Execute the popup inline
    const QAction *selectedAction = menu.exec(pos);

    if (selectedAction) {
        Q_ASSERT(cursor.selectedText() == selectedWord);

        if (selectedAction == ignoreAction) {
            spellchecker->ignoreWord(selectedWord);
            _highlighter->rehighlight();
        } else if (selectedAction == addToDictAction) {
            spellchecker->addWordToDictionary(selectedWord);
            _highlighter->rehighlight();
        }
        // Other actions can only be one of the suggested words
        else {
            const QString replacement = selectedAction->text();
            Q_ASSERT(reps.contains(replacement));
            cursor.insertText(replacement);
            setTextCursor(cursor);
        }
    }

    return true;
}


bool QOwnNotesMarkdownTextEdit::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::ContextMenu) {
        if (spellchecker->isActive())
            return onContextMenuEvent(static_cast<QContextMenuEvent *>(event));
    }
    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);

        if (objectName() == QStringLiteral("encryptedNoteTextEdit")
            || objectName() == QStringLiteral("noteTextEdit") ) {
            // deactivating the search widget has priority
            if ((keyEvent->key() == Qt::Key_Escape) && _searchWidget->isVisible()) {
                _searchWidget->deactivate();
                return true;
            } else if (!Utils::Misc::isNoteEditingAllowed()) {
                auto keys = QList<int>() << Qt::Key_Return << Qt::Key_Enter <<
                        Qt::Key_Space << Qt::Key_Backspace << Qt::Key_Delete <<
                        Qt::Key_Tab << Qt::Key_Backtab << Qt::Key_Minus <<
                        Qt::Key_ParenLeft << Qt::Key_BraceLeft <<
                        Qt::Key_BracketLeft << Qt::Key_Plus << Qt::Key_Comma <<
                        Qt::Key_Period;

                // show notification if user tries to edit a note while
                // note editing is turned off
                if ((keyEvent->key() < 128 || keys.contains(keyEvent->key())) &&
                        keyEvent->modifiers().testFlag(Qt::NoModifier) &&
                        isReadOnly()) {
                    if (Utils::Gui::question(
                            this,
                            tr("Note editing disabled"),
                            tr("Note editing is currently disabled, do you want to "
                               "allow again?"), QStringLiteral("readonly-mode-allow")) ==
                        QMessageBox::Yes) {
                        if (mainWindow != Q_NULLPTR) {
                            mainWindow->allowNoteEditing();
                        }
                    }

                    return true;
                }
            } else {
                // disable note editing if escape key was pressed
                if (keyEvent->key() == Qt::Key_Escape && mainWindow != Q_NULLPTR) {
                    mainWindow->disallowNoteEditing();

                    return true;
                }
            }
        }
    }

    return QMarkdownTextEdit::eventFilter(obj, event);
}
