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
    _highlighter = new QOwnNotesMarkdownHighlighter(document());
    setStyles();
    updateSettings();

    connect(this, SIGNAL(cursorPositionChanged()),
            this, SLOT(highlightCurrentLine()));
    highlightCurrentLine();

    QSettings settings;
    MarkdownHighlighter::HighlightingOptions options;

    if (settings.value("fullyHighlightedBlockquotes").toBool()) {
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
    setTabStopWidth(tabStop * metrics.width(' '));

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
    if (!settings.value("darkMode").toBool()) {
        // light green (#9be29b) could be an other choice, but be aware that
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
            "MainWindow/noteTextEdit.font").toString();
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
        settings.setValue("MainWindow/noteTextEdit.font", font.toString());
    }

    // modify the text edit code font
    fontString = settings.value("MainWindow/noteTextEdit.code.font").toString();
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
        settings.setValue("MainWindow/noteTextEdit.code.font", font.toString());
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
    QString windowsSlash = "";

#ifdef Q_OS_WIN32
    // we need an other slash for Windows
    windowsSlash = "/";
#endif

    // parse for relative file urls and make them absolute
    urlString.replace(QRegularExpression("^file:[\\/]{2}([^\\/].+)$"),
                      "file://" + windowsSlash + notesPath + "/\\1");

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
            settings.value("DistractionFreeMode/isEnabled").toBool();
    bool editorWidthInDFMOnly =
            settings.value("Editor/editorWidthInDFMOnly", true).toBool();

    if (isInDistractionFreeMode || !editorWidthInDFMOnly) {
        int margin = 0;

        if (width == -1) {
            width = this->width();
        }

        int editorWidthMode =
                settings.value("DistractionFreeMode/editorWidthMode").toInt();

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
            int proposedEditorWidth = metrics.width(
                            QString("O").repeated(characterAmount));

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
    } else if (mainWindow != NULL) {
        // to more complex pasting if there was no text (and a main window
        // was set)
        mainWindow->handleInsertingFromMimeData(source);
    }
}

/**
 * Handles the settings of the markdown textedit
 */
void QOwnNotesMarkdownTextEdit::updateSettings() {
    QSettings settings;
    QMarkdownTextEdit::AutoTextOptions options;

    if (settings.value("Editor/autoBracketClosing", true).toBool()) {
        options |= QMarkdownTextEdit::AutoTextOption::BracketClosing;
    }

    if (settings.value("Editor/autoBracketRemoval", true).toBool()) {
        options |= QMarkdownTextEdit::AutoTextOption::BracketRemoval;
    }

    setAutoTextOptions(options);

    // highlighting is always disabled for logTextEdit
    if (objectName() != "logTextEdit") {
        // enable or disable markdown highlighting
        bool highlightingEnabled = settings.value("markdownHighlightingEnabled",
                                                  true).toBool();

        setHighlightingEnabled(highlightingEnabled);

        if (highlightingEnabled) {
            // set the new highlighting styles
            setStyles();
            _highlighter->rehighlight();
        }
    }
}

/**
 * Highlights the current line if enabled in the settings
 */
void QOwnNotesMarkdownTextEdit::highlightCurrentLine()
{
    QSettings settings;
    if (!settings.value("Editor/highlightCurrentLine", true).toBool()) {
        return;
    }

    QList<QTextEdit::ExtraSelection> extraSelections;

    // check if current line is really visible!
    if (!isReadOnly()) {
        ensureCursorVisible();
        QTextEdit::ExtraSelection selection;

        QColor lineColor = Utils::Schema::schemaSettings->getBackgroundColor(
                MarkdownHighlighter::HighlighterState::
                CurrentLineBackgroundColor);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
//        selection.cursor.clearSelection();
//        selection.cursor.select(QTextCursor::BlockUnderCursor);
        extraSelections.append(selection);
    }

    // be aware that extra selections, like for global searching, gets
    // removed when the current line gets highlighted
    setExtraSelections(extraSelections);
}

bool QOwnNotesMarkdownTextEdit::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        // show notification if user tries to edit a note while note editing
        // is turned off
        if ((objectName() == "encryptedNoteTextEdit" ||
             objectName() == "noteTextEdit") && isReadOnly() &&
             (keyEvent->key() < 128) &&
            keyEvent->modifiers().testFlag(Qt::NoModifier) &&
                !Utils::Misc::isNoteEditingAllowed()) {
            Utils::Gui::information(this, tr("Note editing disabled"),
                                    tr("Note editing is currently disabled, "
                                       "please allow it again in the "
                                       "<i>Note-menu</i>."),
                                    "readonly-mode");
        }
    }

    return QMarkdownTextEdit::eventFilter(obj, event);
}
