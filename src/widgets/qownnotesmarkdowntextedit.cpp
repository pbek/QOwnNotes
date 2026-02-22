#include "qownnotesmarkdowntextedit.h"

#include <utils/gui.h>
#include <utils/misc.h>
#include <utils/schema.h>

#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QEvent>
#include <QFont>
#include <QFontDatabase>
#include <QHelpEvent>
#include <QJSEngine>
#include <QKeyEvent>
#include <QMenu>
#include <QMimeData>
#include <QRegularExpression>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocumentFragment>
#include <QTextEdit>
#include <QTextFormat>
#include <QTimer>
#include <QToolTip>

#include "entities/notefolder.h"
#include "helpers/qownspellchecker.h"
#include "libraries/qmarkdowntextedit/linenumberarea.h"
#include "mainwindow.h"
#include "services/markdownlspclient.h"
#include "services/nextclouddeckservice.h"
#include "services/openaiservice.h"
#include "services/scriptingservice.h"
#include "services/settingsservice.h"
#include "utils/urlhandler.h"
#include "version.h"

namespace {
constexpr int kMarkdownLspDiagnosticProperty = QTextFormat::UserProperty + 1;
}

// Initialize static member
QOwnNotesMarkdownTextEdit *QOwnNotesMarkdownTextEdit::_activeAutocompleteEditor = nullptr;

QOwnNotesMarkdownTextEdit::QOwnNotesMarkdownTextEdit(QWidget *parent)
    : QMarkdownTextEdit(parent, false) {
    // We need to set the internal variable to true, because we start with a highlighter
    _highlightingEnabled = true;
    _highlighter = nullptr;
    if (!parent || parent->objectName() != QStringLiteral("LogWidget")) {
        _highlighter = new QOwnNotesMarkdownHighlighter(document());

        setStyles();
        updateSettings();
    }

    // Initialize AI autocomplete timer (only for note editors, not log widget)
    _aiAutocompleteTimer = new QTimer(this);
    _aiAutocompleteTimer->setSingleShot(true);
    _aiAutocompleteTimer->setInterval(500);    // Wait 500ms after typing stops
    connect(_aiAutocompleteTimer, &QTimer::timeout, this,
            &QOwnNotesMarkdownTextEdit::requestAiAutocomplete);

    _markdownLspChangeTimer = new QTimer(this);
    _markdownLspChangeTimer->setSingleShot(true);
    _markdownLspChangeTimer->setInterval(200);
    connect(_markdownLspChangeTimer, &QTimer::timeout, this,
            &QOwnNotesMarkdownTextEdit::sendMarkdownLspChange);

    // NOTE: Callback registration is now done globally in OpenAiService constructor
    // We just need to register this editor as active if it's a note editor
    if (!parent || parent->objectName() != QStringLiteral("LogWidget")) {
        qDebug() << __func__ << " - Registering as active editor";
        registerAsActiveEditor();
    } else {
        qDebug() << __func__ << " - Skipping registration for non-editor widget:" << objectName();
    }

    // Use DirectConnection to ensure signal is delivered immediately before widget can be destroyed
    // This prevents the "0 receivers" problem when widgets are recreated
    bool conn1 = connect(
        OpenAiService::instance(), &OpenAiService::autocompleteCompleted, this,
        [this](const QString &result) {
            qDebug() << "*** LAMBDA RECEIVED autocompleteCompleted signal for widget:" << this
                     << objectName();
            this->onAiAutocompleteCompleted(result);
        },
        Qt::DirectConnection);
    bool conn2 = connect(OpenAiService::instance(), &OpenAiService::autocompleteErrorOccurred, this,
                         &QOwnNotesMarkdownTextEdit::onAiAutocompleteTimeout, Qt::DirectConnection);

    qDebug() << __func__
             << " - AI autocomplete signals connected (DirectConnection), conn1:" << conn1
             << "conn2:" << conn2;
    qDebug() << __func__ << " - Connected to OpenAiService instance:" << OpenAiService::instance();

    // Test: Call the slot directly to verify it exists
    qDebug() << __func__ << " - Testing slot by calling it directly...";
    onAiAutocompleteCompleted("TEST DIRECT CALL");
    qDebug() << __func__ << " - Direct call completed";

    // Connect to text changes to trigger autocomplete
    connect(this, &QOwnNotesMarkdownTextEdit::textChanged, this, [this]() {
        // Only trigger if autocomplete is enabled
        if (OpenAiService::getAutocompleteEnabled() && OpenAiService::getEnabled()) {
            // Skip for log widgets
            if (objectName() == QStringLiteral("logTextEdit")) {
                return;
            }
            // Don't clear or restart timer if we're currently inserting a suggestion
            if (_isInsertingAiSuggestion) {
                return;
            }
            clearAiAutocompleteSuggestion();
            _aiAutocompleteTimer->start();
        }

        if (_markdownLspEnabled) {
            scheduleMarkdownLspChange();
        }
    });

    SettingsService settings;
    MarkdownHighlighter::HighlightingOptions options;

    if (settings.value(QStringLiteral("fullyHighlightedBlockquotes")).toBool()) {
        options |= MarkdownHighlighter::HighlightingOption ::FullyHighlightedBlockQuote;
    }
    if (settings.value(QStringLiteral("MainWindow/noteTextView.underline")).toBool()) {
        options |= MarkdownHighlighter::HighlightingOption ::Underline;
    }

    // set the highlighting options
    if (_highlighter) {
        _highlighter->setHighlightingOptions(options);

        // re-initialize the highlighting rules if we are using some options
        if (options != MarkdownHighlighter::HighlightingOption::None) {
            _highlighter->initHighlightingRules();
        }
    }

    // ignores note clicks in QMarkdownTextEdit in the note text edit
    setIgnoredClickUrlSchemata(QStringList({"note", "task", "deck"}));

    connect(this, &QOwnNotesMarkdownTextEdit::zoomIn, this, [this]() { onZoom(/*in=*/true); });
    connect(this, &QOwnNotesMarkdownTextEdit::zoomOut, this, [this]() { onZoom(/*in=*/false); });

    connect(this, &QOwnNotesMarkdownTextEdit::urlClicked, this, [this](const QString &url) {
        if (!MainWindow::instance()) {
            qWarning() << "No MainWindow! shouldn't happen!";
            return;
        }
        // Use the openUrl method which properly handles the openInNewTab flag
        openUrl(url, _openLinkInNewTab);
    });

    connect(MainWindow::instance(), &MainWindow::settingsChanged, this,
            &QOwnNotesMarkdownTextEdit::updateSettings);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QOwnNotesMarkdownTextEdit::customContextMenuRequested, this,
            &QOwnNotesMarkdownTextEdit::onContextMenu);

    applyMarkdownLspSettings();
}

/*
 * Prevent infinite loops of QResizeEvents in conjunction with setPaperMargins()
 * when the widget is resized (and leave some space for line numbers too)
 * See: https://github.com/pbek/QOwnNotes/issues/2679
 */
QSize QOwnNotesMarkdownTextEdit::minimumSizeHint() const {
    int lineWidthLeftMargin =
        _lineNumArea->isLineNumAreaEnabled() ? _lineNumArea->lineNumAreaWidth() : 0;

    // Let the min size be the defaultMinSize + lineNumAreaWidth + paper margin
    auto sizeHint = QMarkdownTextEdit::minimumSizeHint();
    sizeHint.rwidth() += lineWidthLeftMargin + 10;

    return sizeHint;
}

void QOwnNotesMarkdownTextEdit::onZoom(bool in) {
    FontModificationMode mode = in ? Increase : Decrease;
    const int fontSize = modifyFontSize(mode);

    auto mainWindow = MainWindow::instance();
    if (mainWindow && MainWindow::isInDistractionFreeMode()) {
        setPaperMargins();
        if (in) {
            mainWindow->showStatusBarMessage(tr("Increased font size to %1 pt").arg(fontSize),
                                             QStringLiteral("🔤"), 3000);
        } else {
            mainWindow->showStatusBarMessage(tr("Decreased font size to %1 pt").arg(fontSize),
                                             QStringLiteral("🔤"), 3000);
        }
    }

    setPaperMargins();
}

/**
 * Sets the format style
 *
 * @param index
 * @param styles
 */
void QOwnNotesMarkdownTextEdit::setFormatStyle(MarkdownHighlighter::HighlighterState index) {
    QTextCharFormat format;
    Utils::Schema::schemaSettings->setFormatStyle(index, format);
    if (_highlighter) {
        _highlighter->setTextFormat(index, format);
    }
}

/**
 * Overrides the font size style if overrideInterfaceFontSize was set to prevent
 * Utils::Gui::updateInterfaceFontSize from overriding the default text size on
 * Windows 10
 *
 * @param fontSize
 */
void QOwnNotesMarkdownTextEdit::overrideFontSizeStyle(int fontSize) {
    bool overrideInterfaceFontSize =
        SettingsService().value(QStringLiteral("overrideInterfaceFontSize"), false).toBool();

    // remove old style
    QString stylesheet = styleSheet().remove(QRegularExpression(
        QRegularExpression::escape(
            QOWNNOTESMARKDOWNTEXTEDIT_OVERRIDE_FONT_SIZE_STYLESHEET_PRE_STRING) +
        ".*" +
        QRegularExpression::escape(
            QOWNNOTESMARKDOWNTEXTEDIT_OVERRIDE_FONT_SIZE_STYLESHEET_POST_STRING)));

    if (overrideInterfaceFontSize) {
        // using pt is important here, px didn't work properly
        stylesheet +=
            QStringLiteral(QOWNNOTESMARKDOWNTEXTEDIT_OVERRIDE_FONT_SIZE_STYLESHEET_PRE_STRING) +
            "QOwnNotesMarkdownTextEdit {font-size: " + QString::number(fontSize) + "pt;}" +
            QStringLiteral(QOWNNOTESMARKDOWNTEXTEDIT_OVERRIDE_FONT_SIZE_STYLESHEET_POST_STRING);
    }

    setStyleSheet(stylesheet);
}

/**
 * Sets the highlighting styles for the text edit
 */
void QOwnNotesMarkdownTextEdit::setStyles() {
    QFont font = Utils::Schema::schemaSettings->getEditorTextFont();
    setFont(font);

    // workaround for Windows 10 if overrideInterfaceFontSize was set
    overrideFontSizeStyle(font.pointSize());

    // set the tab stop to the width of 4 spaces in the editor
    const int tabStop = 4;
    QFontMetrics metrics(font);

#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
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
    setFormatStyle(MarkdownHighlighter::HighlighterState::CheckBoxChecked);
    setFormatStyle(MarkdownHighlighter::HighlighterState::CheckBoxUnChecked);
    setFormatStyle(MarkdownHighlighter::HighlighterState::Bold);
    setFormatStyle(MarkdownHighlighter::HighlighterState::Italic);
    setFormatStyle(MarkdownHighlighter::HighlighterState::StUnderline);
    setFormatStyle(MarkdownHighlighter::HighlighterState::BlockQuote);
    setFormatStyle(MarkdownHighlighter::HighlighterState::CodeBlock);
    setFormatStyle(MarkdownHighlighter::HighlighterState::Comment);
    setFormatStyle(MarkdownHighlighter::HighlighterState::MaskedSyntax);
    setFormatStyle(MarkdownHighlighter::HighlighterState::Image);
    setFormatStyle(MarkdownHighlighter::HighlighterState::InlineCodeBlock);
    setFormatStyle(MarkdownHighlighter::HighlighterState::Link);
    setFormatStyle(MarkdownHighlighter::HighlighterState::Table);
    setFormatStyle(MarkdownHighlighter::HighlighterState::BrokenLink);
    setFormatStyle(MarkdownHighlighter::HighlighterState::TrailingSpace);

    setFormatStyle(MarkdownHighlighter::HighlighterState::CodeType);
    setFormatStyle(MarkdownHighlighter::HighlighterState::CodeKeyWord);
    setFormatStyle(MarkdownHighlighter::HighlighterState::CodeComment);
    setFormatStyle(MarkdownHighlighter::HighlighterState::CodeString);
    setFormatStyle(MarkdownHighlighter::HighlighterState::CodeNumLiteral);
    setFormatStyle(MarkdownHighlighter::HighlighterState::CodeBuiltIn);
    setFormatStyle(MarkdownHighlighter::HighlighterState::CodeOther);

#ifdef Q_OS_WIN32
    // set the selection background color to a light blue if not in dark mode
    if (!SettingsService().value(QStringLiteral("darkMode")).toBool()) {
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
    SettingsService settings;
    QFont font = this->font();
    int fontSize = font.pointSize();
    bool doSetStyles = false;

    // modify the text edit default font
    QString fontString = settings.value(QStringLiteral("MainWindow/noteTextEdit.font")).toString();
    if (!fontString.isEmpty()) {
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
                if (fontSize != newFontSize) {
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
    if (!fontString.isEmpty()) {
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
                if (codeFontSize != newCodeFontSize) {
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
        if (_highlighter) {
            _highlighter->rehighlight();
        }
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
void QOwnNotesMarkdownTextEdit::openUrl(const QString &urlString, bool openInNewTab) {
    qDebug() << "QOwnNotesMarkdownTextEdit " << __func__ << " - 'urlString': " << urlString
             << " - 'openInNewTab': " << openInNewTab;

    QString notesPath = NoteFolder::currentLocalPath();
    QString windowsSlash = QString();

#ifdef Q_OS_WIN32
    // we need another slash for Windows
    windowsSlash = QStringLiteral("/");
#endif

    auto urlCopy = urlString;

    // parse for relative file urls and make them absolute
    urlCopy.replace(QRegularExpression(QStringLiteral("^file:[\\/]{2}([^\\/].+)$")),
                    QStringLiteral("file://") + windowsSlash + notesPath + QStringLiteral("/\\1"));

    // Check if this is a note URL that should be handled specially
    QUrl url(urlCopy);
    const QString scheme = url.scheme();

    // If it's a note URL, noteid URL, file URL in note folder, or has no scheme (relative link),
    // use UrlHandler which knows how to handle notes properly
    if (scheme == QStringLiteral("note") || scheme == QStringLiteral("noteid") ||
        scheme == QStringLiteral("file") || scheme.isEmpty() ||
        Note::fileUrlIsNoteInCurrentNoteFolder(url)) {
        // Use UrlHandler for note URLs with the openInNewTab flag
        UrlHandler().openUrl(urlCopy, openInNewTab);
    } else {
        // For other URLs (http, https, etc.), use the base class implementation
        QMarkdownTextEdit::openUrl(urlCopy, openInNewTab);
    }
}

// void QOwnNotesMarkdownTextEdit::setViewportMargins(
//        int left, int top, int right, int bottom) {
//    QMarkdownTextEdit::setViewportMargins(left, top, right, bottom);
//}

/**
 * Sets the viewport margins for the distraction free mode
 */
void QOwnNotesMarkdownTextEdit::setPaperMargins(int width) {
    SettingsService settings;
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
                case Custom:
                    characterAmount =
                        settings.value(QStringLiteral("DistractionFreeMode/editorWidthCustom"), 80)
                            .toInt();
                    break;
                default:
                case Narrow:
                    characterAmount = 60;
                    break;
            }

                // set the size of characterAmount times the size of "O"
                // characters
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
            int proposedEditorWidth = metrics.width(QStringLiteral("O").repeated(characterAmount));
#else
            int proposedEditorWidth =
                metrics.horizontalAdvance(QStringLiteral("O").repeated(characterAmount));
#endif

            // Apply a factor to correct the faulty calculated margin
            // Use a different factor for monospaced fonts
            // TODO(pbek): I don't know better way to get around this yet
            proposedEditorWidth /= usesMonospacedFont() ? 0.95 : 1.332;

            // calculate the margin to be applied
            margin = (width - proposedEditorWidth) / 2;

            if (margin < 0) {
                margin = 0;
            }
        }

        setViewportMargins(margin, 20, margin, 0);
    } else {
        int lineWidthLeftMargin =
            lineNumberArea()->isLineNumAreaEnabled() ? lineNumberArea()->lineNumAreaWidth() : 0;

        setLineNumberLeftMarginOffset(10);
        setViewportMargins(10 + lineWidthLeftMargin, 10, 10, 0);
    }
}

/**
 * Try to determine if the used font is monospaced
 *
 * @return
 */
bool QOwnNotesMarkdownTextEdit::usesMonospacedFont() {
    QFontMetrics metrics(font());

#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
    int widthNarrow = metrics.width(QStringLiteral("iiiii"));
    int widthWide = metrics.width(QStringLiteral("WWWWW"));
#else
    int widthNarrow = metrics.horizontalAdvance(QStringLiteral("iiiii"));
    int widthWide = metrics.horizontalAdvance(QStringLiteral("WWWWW"));
#endif

    return widthNarrow == widthWide;
}

void QOwnNotesMarkdownTextEdit::toggleCase() {
    QTextCursor c = textCursor();
    // Save positions to restore everything at the end
    const int selectionStart = c.selectionStart();
    const int selectionEnd = c.selectionEnd();
    const int cPos = c.position();

    QString selectedText = c.selectedText();
    const bool textWasSelected = !selectedText.isEmpty();

    // if no text is selected: automatically select the Word under the Cursor
    if (selectedText.isEmpty()) {
        c.select(QTextCursor::WordUnderCursor);
        selectedText = c.selectedText();
    }

    // cycle text through lowercase, uppercase, start case, and sentence case
    c.insertText(Utils::Misc::cycleTextCase(selectedText));

    if (textWasSelected) {
        // select the text again to maybe do another operation on it
        // keep the original cursor position
        if (cPos == selectionStart) {
            c.setPosition(selectionEnd, QTextCursor::MoveAnchor);
            c.setPosition(selectionStart, QTextCursor::KeepAnchor);
        } else {
            c.setPosition(selectionStart, QTextCursor::MoveAnchor);
            c.setPosition(selectionEnd, QTextCursor::KeepAnchor);
        }
    } else {
        // Just restore the Cursor Position if no text was selected
        c.setPosition(cPos, QTextCursor::MoveAnchor);
    }
    // Restore the visible cursor
    setTextCursor(c);
}

void QOwnNotesMarkdownTextEdit::insertCodeBlock() {
    QTextCursor c = this->textCursor();
    QString selectedText = c.selection().toPlainText();

    if (selectedText.isEmpty()) {
        // insert multi-line code block if cursor is in an empty line
        if (c.atBlockStart() && c.atBlockEnd()) {
            c.insertText(QStringLiteral("```\n\n```"));
            c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 3);
        } else {
            c.insertText(QStringLiteral("``"));
        }

        c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
        setTextCursor(c);
    } else {
        bool addNewline = false;

        // if the selected text has multiple lines add a multi-line code block
        if (selectedText.contains(QStringLiteral("\n"))) {
            // add another newline if there is no newline at the end of the
            // selected text
            const QString endNewline =
                selectedText.endsWith(QLatin1String("\n")) ? QString() : QStringLiteral("\n");

            selectedText =
                QStringLiteral("``\n") + selectedText + endNewline + QStringLiteral("``");
            addNewline = true;
        }

        c.insertText(QStringLiteral("`") + selectedText + QStringLiteral("`"));

        if (addNewline) {
            c.insertText(QStringLiteral("\n"));
        }
    }
}

void QOwnNotesMarkdownTextEdit::onAutoCompleteRequested() {
    // attempt to toggle a checkbox at the cursor position
    if (Utils::Gui::toggleCheckBoxAtCursor(this)) {
        return;
    }

    if (_markdownLspEnabled && _markdownLspClient && !_markdownLspUri.isEmpty()) {
        const QTextCursor cursor = textCursor();
        const int line = cursor.blockNumber();
        const int character = cursor.positionInBlock();
        _markdownLspCompletionRequestId =
            _markdownLspClient->requestCompletion(_markdownLspUri, line, character);
        if (_markdownLspCompletionRequestId >= 0) {
            return;
        }
    }

    // try to open a link at the cursor position
    if (openLinkAtCursorPosition()) {
        MainWindow::instance()->showStatusBarMessage(
            tr("An url was opened at the current cursor position"), QStringLiteral("📃"), 5000);
        return;
    }

    // attempt a Markdown table auto-format
    if (Utils::Gui::autoFormatTableAtCursor(this)) {
        return;
    }

    QMenu menu;

    double resultValue;
    if (solveEquation(resultValue)) {
        const QString text = QString::number(resultValue);
        auto *action = menu.addAction(QStringLiteral("= ") + text);
        action->setData(text);
        action->setWhatsThis(QStringLiteral("equation"));
    }

    QStringList resultList;
    if (autoComplete(resultList)) {
        for (const QString &text : Utils::asConst(resultList)) {
            auto *action = menu.addAction(text);
            action->setData(text);
            action->setWhatsThis(QStringLiteral("autocomplete"));
        }
    }

    // load texts from scripts to show in the autocompletion list
    const QStringList autocompletionList = ScriptingService::instance()->callAutocompletionHook();
    if (!autocompletionList.isEmpty()) {
        auto *action = menu.addAction(QString());
        action->setSeparator(true);

        for (const QString &text : autocompletionList) {
            auto *newAction = menu.addAction(text);
            newAction->setData(text);
            newAction->setWhatsThis(QStringLiteral("autocomplete"));
        }
    }

    QPoint globalPos = mapToGlobal(cursorRect().bottomRight());

    // compensate viewport margins
    globalPos.setY(globalPos.y() + viewportMargins().top());
    globalPos.setX(globalPos.x() + viewportMargins().left());

    if (menu.actions().count() > 0) {
        QAction *selectedItem = menu.exec(globalPos);
        if (selectedItem) {
            const QString text = selectedItem->data().toString();
            const QString type = selectedItem->whatsThis();

            if (text.isEmpty()) {
                return;
            }

            if (type == QStringLiteral("autocomplete")) {
                // overwrite the currently written word
                QTextCursor c = textCursor();
                c.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
                c.insertText(text + QStringLiteral(" "));
            } else {
                insertPlainText(text);
            }
        }
    }
}

/**
 * Returns the text from the current cursor to the start of the word in the
 * note text edit
 *
 * @param withPreviousCharacters also get more characters at the beginning
 *                               to get characters like "@" that are not
 *                               word-characters
 * @return
 */
QString QOwnNotesMarkdownTextEdit::currentWord(bool withPreviousCharacters) const {
    QTextCursor c = textCursor();

    // get the text from the current word
    c.movePosition(QTextCursor::EndOfWord);
    c.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);

    QString text = c.selectedText();

    if (withPreviousCharacters) {
        static const QRegularExpression re(QStringLiteral("^[\\s\\n][^\\s]*"));
        do {
            c.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
            text = c.selectedText();
        } while (!(re.match(text).hasMatch() || c.atBlockStart()));
    }

    return text.trimmed();
}

QString QOwnNotesMarkdownTextEdit::currentBlock() const {
    QTextCursor cursor = textCursor();
    QTextBlock currentBlock = cursor.block();
    return currentBlock.text();
}

/**
 * Tries to find words that start with the current word in the note text edit
 *
 * @param resultList
 * @return
 */
bool QOwnNotesMarkdownTextEdit::autoComplete(QStringList &resultList) const {
    // get the text from the current cursor to the start of the word
    const QString text = currentWord();
    qDebug() << __func__ << " - 'text': " << text;

    if (text.isEmpty()) {
        return false;
    }

    const QString noteText = toPlainText();

    // find all items that match our current word
    resultList =
        noteText
            .split(QRegularExpression(QStringLiteral("[^\\w\\d]"),
                                      QRegularExpression::UseUnicodePropertiesOption),
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
                   QString::SkipEmptyParts)
#else
                   Qt::SkipEmptyParts)
#endif
            .filter(QRegularExpression(QStringLiteral("^") + QRegularExpression::escape(text),
                                       QRegularExpression::CaseInsensitiveOption));

    // we only want each word once
    resultList.removeDuplicates();

    // remove the text we already entered
    resultList.removeOne(text);

    if (resultList.count() == 0) {
        return false;
    }

    qDebug() << __func__ << " - 'resultList': " << resultList;

    return true;
}

/**
 * Tries to find an equation in the current line and solves it
 *
 * @param returnValue
 * @return
 */
bool QOwnNotesMarkdownTextEdit::solveEquation(double &returnValue) {
    QTextCursor c = textCursor();

    // get the text from the current cursor to the start of the line
    c.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
    QString text = c.selectedText();
    qDebug() << __func__ << " - 'text': " << text;

    QString equation = text;

    // replace "," with "." to allow "," as coma
    equation.replace(QLatin1Char(','), QLatin1Char('.'));

    // remove leading list characters
    equation.remove(QRegularExpression(QStringLiteral(R"(^\s*[\-*+] )")));

    // match all numbers and basic operations like +, -, * and /
    QRegularExpressionMatch match =
        QRegularExpression(QStringLiteral(R"(([\d\.,+\-*\/\(\)\s]+)\s*=)")).match(equation);

    if (!match.hasMatch()) {
        if (equation.trimmed().endsWith(QChar('='))) {
            MainWindow::instance()->showStatusBarMessage(
                tr("No equation was found in front of the cursor"), QStringLiteral("🧮"), 5000);
        }

        return false;
    }

    equation = match.captured(1);
    qDebug() << __func__ << " - 'equation': " << equation;

    QJSEngine engine;
    // evaluate our equation
    QJSValue result = engine.evaluate(equation);
    double resultValue = result.toNumber();
    qDebug() << __func__ << " - 'resultValue': " << resultValue;

    // compensate for subtraction errors with 0
    if ((resultValue < 0.0001) && (resultValue > 0)) {
        resultValue = 0;
    }

    MainWindow::instance()->showStatusBarMessage(
        tr("Result for equation: %1 = %2").arg(equation, QString::number(resultValue)),
        QStringLiteral("🧮"), 10000);

    // check if cursor is after the "="
    match = QRegularExpression(QStringLiteral("=\\s*$")).match(text);
    if (!match.hasMatch()) {
        return false;
    }

    returnValue = resultValue;
    return true;
}

void QOwnNotesMarkdownTextEdit::insertBlockQuote() {
    QTextCursor c = textCursor();
    QString selectedText = c.selectedText();

    if (selectedText.isEmpty()) {
        c.insertText(QStringLiteral("> "));
        setTextCursor(c);
    } else {
        // this only applies to the start of the selected block
        selectedText.replace(QRegularExpression(QStringLiteral("^")), QStringLiteral("> "));

        // transform Unicode line endings
        // this newline character seems to be used in multi-line selections
        const QString newLine = QString::fromUtf8(QByteArray::fromHex("e280a9"));
        selectedText.replace(newLine, QStringLiteral("\n> "));

        // remove the block quote if it was placed at the end of the text
        selectedText.remove(QRegularExpression(QStringLiteral("> $")));

        c.insertText(selectedText);
    }
}

QMargins QOwnNotesMarkdownTextEdit::viewportMargins() {
    return QMarkdownTextEdit::viewportMargins();
}

void QOwnNotesMarkdownTextEdit::setText(const QString &text) {
    // set a search delay of 250ms for text with more than 200k characters
    setSearchWidgetDebounceDelay(text.size() > 200000 ? 250 : 0);

    QMarkdownTextEdit::setText(text);
}

/**
 * Since spell checking can only be enabled and disabled globally this allows to
 * disable it in one QOwnNotesMarkdownTextEdit
 */
void QOwnNotesMarkdownTextEdit::disableSpellChecking() { _isSpellCheckingDisabled = true; }

void QOwnNotesMarkdownTextEdit::setSpellCheckingEnabled(bool enabled) {
    QOwnSpellChecker::instance()->setActive(enabled);
}

bool QOwnNotesMarkdownTextEdit::isSpellCheckingEnabled() {
    return QOwnSpellChecker::instance()->isActive();
}

void QOwnNotesMarkdownTextEdit::resizeEvent(QResizeEvent *event) {
    setPaperMargins();
    QMarkdownTextEdit::resizeEvent(event);
}

bool QOwnNotesMarkdownTextEdit::canInsertFromMimeData(const QMimeData *source) const {
    return (!source->hasUrls());
}

/**
 * Handles pasting from clipboard
 */
void QOwnNotesMarkdownTextEdit::insertFromMimeData(const QMimeData *source) {
    // if there is text in the clipboard do the normal pasting process
    if (source->hasText()) {
        QMarkdownTextEdit::insertFromMimeData(source);
    } else if (auto mainWindow = MainWindow::instance()) {
        // to more complex pasting if there was no text (and a main window
        // was set)
        mainWindow->handleInsertingFromMimeData(source);
    }
}

/**
 * Handles the settings of the Markdown textedit
 */
void QOwnNotesMarkdownTextEdit::updateSettings() {
    // we need a blocker, otherwise the "change" events will fire
    const QSignalBlocker blocker(this);
    Q_UNUSED(blocker)

    SettingsService settings;
    QMarkdownTextEdit::AutoTextOptions options;

    if (settings.value(QStringLiteral("Editor/autoBracketClosing"), true).toBool()) {
        options |= QMarkdownTextEdit::AutoTextOption::BracketClosing;
    }

    if (settings.value(QStringLiteral("Editor/autoBracketRemoval"), true).toBool()) {
        options |= QMarkdownTextEdit::AutoTextOption::BracketRemoval;
    }

    setAutoTextOptions(options);

    auto spellchecker = QOwnSpellChecker::instance();
    if (spellchecker) {
        // spell check active/inactive
        bool spellcheckerActive = settings.value(QStringLiteral("checkSpelling"), true).toBool();
        spellchecker->setActive(spellcheckerActive);

        QString lang =
            settings.value(QStringLiteral("spellCheckLanguage"), QStringLiteral("auto")).toString();
        if (lang == QStringLiteral("auto")) {
            spellchecker->setAutoDetect(true);
        } else {
            spellchecker->setAutoDetect(false);
            spellchecker->setCurrentLanguage(lang);
        }
    }

    // highlighting is always disabled for logTextEdit
    if (objectName() != QStringLiteral("logTextEdit")) {
        // enable or disable Markdown highlighting
        bool highlightingEnabled =
            settings.value(QStringLiteral("markdownHighlightingEnabled"), true).toBool();

        setHighlightingEnabled(highlightingEnabled);

        if (highlightingEnabled) {
            // set the new highlighting styles
            setStyles();
            if (_highlighter) {
                _highlighter->rehighlight();
            }
        }
    }

    const bool hlCurrLine =
        settings.value(QStringLiteral("Editor/highlightCurrentLine"), true).toBool();
    setHighlightCurrentLine(hlCurrLine);
    const auto color = Utils::Schema::schemaSettings->getBackgroundColor(
        MarkdownHighlighter::HighlighterState::CurrentLineBackgroundColor);
    setCurrentLineHighlightColor(color);

    _centerCursor = settings.value(QStringLiteral("Editor/centerCursor")).toBool();
    QMarkdownTextEdit::updateSettings();

    applyMarkdownLspSettings();
}

void QOwnNotesMarkdownTextEdit::onContextMenu(QPoint pos) {
    auto *spellCheckMenu = spellCheckContextMenu(pos);

    const QPoint globalPos = this->viewport()->mapToGlobal(pos);
    QMenu *menu = this->createStandardContextMenu();
    if (spellCheckMenu) {
        // insert spell check at the top if available
        menu->insertMenu(menu->actions().constFirst(), spellCheckMenu);
    }

    const bool isAllowNoteEditing = Utils::Misc::isNoteEditingAllowed();
    const bool isTextSelected = textCursor().hasSelection();

    const QString linkTextActionName =
        isTextSelected ? tr("&Link selected text") : tr("Insert &link");
    QAction *linkTextAction = menu->addAction(linkTextActionName, this, []() {
        MainWindow::instance()->insertTextLinkAction()->trigger();
    });
    linkTextAction->setEnabled(isAllowNoteEditing);

    QString blockQuoteTextActionName = isTextSelected
                                           ? tr("Block &quote selected text",
                                                "Action to apply a block quote formatting to the "
                                                "selected text")
                                           : tr("Insert block &quote");
    QAction *blockQuoteTextAction = menu->addAction(blockQuoteTextActionName);
    connect(blockQuoteTextAction, &QAction::triggered, this,
            &QOwnNotesMarkdownTextEdit::insertBlockQuote);
    blockQuoteTextAction->setEnabled(isAllowNoteEditing);

    if (isTextSelected) {
        menu->addAction(MainWindow::instance()->searchTextOnWebAction());
        menu->addAction(MainWindow::instance()->findNoteAction());
    }

    //     searchAction->setEnabled(isTextSelected);
    //     QAction *searchAction =
    //         menu->addAction(ui->actionSearch_text_on_the_web->text());
    //     searchAction->setShortcut(ui->actionSearch_text_on_the_web->shortcut());

    QAction *copyCodeBlockAction = menu->addAction(tr("Copy code block"));
    copyCodeBlockAction->setIcon(
        QIcon::fromTheme(QStringLiteral("edit-copy"),
                         QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/edit-copy.svg"))));
    const QTextBlock currentTextBlock = cursorForPosition(pos).block();
    const int userState = currentTextBlock.userState();
    const bool isCodeSpan = highlighter()->isPosInACodeSpan(
        currentTextBlock.blockNumber(), cursorForPosition(pos).positionInBlock());
    copyCodeBlockAction->setEnabled(MarkdownHighlighter::isCodeBlock(userState) || isCodeSpan);
    connect(copyCodeBlockAction, &QAction::triggered, this,
            [this, isCodeSpan, currentTextBlock, pos]() {
                // copy the text from a copy block around currentTextBlock to the
                // clipboard
                if (isCodeSpan) {
                    const auto codeSpanRange = highlighter()->getSpanRange(
                        MarkdownHighlighter::RangeType::CodeSpan, currentTextBlock.blockNumber(),
                        cursorForPosition(pos).positionInBlock());
                    QApplication::clipboard()->setText(currentTextBlock.text().mid(
                        codeSpanRange.first + 1, codeSpanRange.second - codeSpanRange.first - 1));
                } else {
                    Utils::Gui::copyCodeBlockText(currentTextBlock);
                }
            });

    menu->addSeparator();

    // add table column insertion actions if cursor is in a table
    if (Utils::Gui::isTableAtCursor(this)) {
        QAction *addColumnLeftAction = menu->addAction(tr("Add table column left"));
        addColumnLeftAction->setEnabled(isAllowNoteEditing);
        connect(addColumnLeftAction, &QAction::triggered, this,
                [this]() { Utils::Gui::insertTableColumnLeft(this); });

        QAction *addColumnRightAction = menu->addAction(tr("Add table column right"));
        addColumnRightAction->setEnabled(isAllowNoteEditing);
        connect(addColumnRightAction, &QAction::triggered, this,
                [this]() { Utils::Gui::insertTableColumnRight(this); });

        QAction *addRowAboveAction = menu->addAction(tr("Add table row above"));
        addRowAboveAction->setEnabled(isAllowNoteEditing);
        connect(addRowAboveAction, &QAction::triggered, this,
                [this]() { Utils::Gui::insertTableRowAbove(this); });

        QAction *addRowBelowAction = menu->addAction(tr("Add table row below"));
        addRowBelowAction->setEnabled(isAllowNoteEditing);
        connect(addRowBelowAction, &QAction::triggered, this,
                [this]() { Utils::Gui::insertTableRowBelow(this); });

        menu->addSeparator();
    }

    // add the print menu
    QMenu *printMenu = menu->addMenu(tr("Print"));
    QIcon printIcon =
        QIcon::fromTheme(QStringLiteral("document-print"),
                         QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/document-print.svg")));
    printMenu->setIcon(printIcon);

    // add the print selected text action
    QAction *printTextAction = printMenu->addAction(tr("Print selected text"));
    printTextAction->setEnabled(isTextSelected);
    printTextAction->setIcon(printIcon);
    connect(printTextAction, &QAction::triggered, this, [this]() {
        // print the selected text
        auto mainWindow = MainWindow::instance();
        auto *textEdit = new QOwnNotesMarkdownTextEdit(this);
        textEdit->setPlainText(mainWindow->selectedNoteTextEditText());
        mainWindow->printTextDocument(textEdit->document());
    });

    // add the print selected text (preview) action
    QAction *printHTMLAction = printMenu->addAction(tr("Print selected text (preview)"));
    printHTMLAction->setEnabled(isTextSelected);
    printHTMLAction->setIcon(printIcon);
    connect(printHTMLAction, &QAction::triggered, this, [this]() {
        // print the selected text (preview)
        auto mainWindow = MainWindow::instance();
        auto note = mainWindow->getCurrentNote();
        QString html = note.textToMarkdownHtml(
            mainWindow->selectedNoteTextEditText(), NoteFolder::currentLocalPath(),
            mainWindow->getMaxImageWidth(), Utils::Misc::useInternalExportStylingForPreview());
        auto *textEdit = new QTextEdit(this);
        textEdit->setHtml(html);
        mainWindow->printTextDocument(textEdit->document());
    });

    // add the export menu
    QMenu *exportMenu = menu->addMenu(tr("Export"));
    exportMenu->setIcon(QIcon::fromTheme(
        QStringLiteral("document-export"),
        QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/document-export.svg"))));

    QIcon pdfIcon = QIcon::fromTheme(
        QStringLiteral("application-pdf"),
        QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/application-pdf.svg")));

    // add the export selected text action
    QAction *exportTextAction = exportMenu->addAction(tr("Export selected text as PDF"));
    exportTextAction->setEnabled(isTextSelected);
    exportTextAction->setIcon(pdfIcon);
    connect(exportTextAction, &QAction::triggered, this, [this]() {
        // export the selected text as PDF
        auto mainWindow = MainWindow::instance();
        auto *textEdit = new QOwnNotesMarkdownTextEdit(this);
        textEdit->setPlainText(mainWindow->selectedNoteTextEditText());
        mainWindow->exportNoteAsPDF(textEdit->document());
    });

    // add the export selected text (preview) action
    QAction *exportHTMLAction = exportMenu->addAction(tr("Export selected text as PDF (preview)"));
    exportHTMLAction->setEnabled(isTextSelected);
    exportHTMLAction->setIcon(pdfIcon);
    connect(exportHTMLAction, &QAction::triggered, this, []() {
        // export the selected text (preview) as PDF
        auto mainWindow = MainWindow::instance();
        auto note = mainWindow->getCurrentNote();
        QString html = note.textToMarkdownHtml(
            mainWindow->selectedNoteTextEditText(), NoteFolder::currentLocalPath(),
            mainWindow->getMaxImageWidth(), Utils::Misc::useInternalExportStylingForPreview());
        html = Utils::Misc::parseTaskList(html, false);
        QTextDocument doc;
        doc.setHtml(html);
        mainWindow->exportNoteAsPDF(&doc);
    });

    menu->addSeparator();

    // add some other existing menu entries
    auto mainWindow = MainWindow::instance();
    menu->addAction(mainWindow->pasteImageAction());
    menu->addAction(mainWindow->autocompleteAction());
    menu->addAction(mainWindow->splitNoteAtPosAction());

    // add the custom actions to the context menu
    const auto customActions = mainWindow->customTextEditActions();
    if (!customActions.isEmpty()) {
        // add the scripts menu
        QIcon scriptIcon = QIcon::fromTheme(
            QStringLiteral("story-editor"),
            QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/story-editor.svg")));
        menu->addSeparator();
        QMenu *scriptMenu = menu->addMenu(tr("Custom actions"));
        scriptMenu->setIcon(scriptIcon);
        scriptMenu->addActions(customActions);
    }

    menu->exec(globalPos);
}

QMenu *QOwnNotesMarkdownTextEdit::spellCheckContextMenu(QPoint pos) {
    auto spellchecker = QOwnSpellChecker::instance();
    if (!spellchecker || !spellchecker->isActive() || _isSpellCheckingDisabled) {
        return nullptr;
    }

    // obtain the cursor at current mouse position
    QTextCursor cursorAtMouse = cursorForPosition(pos);
    const int mousePos = cursorAtMouse.position();

    QTextCursor cursor = textCursor();
    if (MarkdownHighlighter::isCodeBlock(cursor.block().userState())) {
        return nullptr;
    }

    // Check if the user clicked a selected word
    const bool selectedWordClicked = cursor.hasSelection() && mousePos >= cursor.selectionStart() &&
                                     mousePos <= cursor.selectionEnd();

    // Get the word under the (mouse-)cursor and see if it is misspelled.
    // Don't include apostrophes at the start/end of the word in the selection.
    QTextCursor wordSelectCursor(cursorAtMouse);
    wordSelectCursor.clearSelection();
    wordSelectCursor.select(QTextCursor::WordUnderCursor);
    QString selectedWord = wordSelectCursor.selectedText();
    const int selectedWordBlock = wordSelectCursor.blockNumber();

    bool isMouseCursorInsideWord = true;
    if ((mousePos < wordSelectCursor.selectionStart() ||
         mousePos >= wordSelectCursor.selectionEnd()) &&
        (selectedWord.length() > 1)) {
        isMouseCursorInsideWord = false;
    }

    // Clear the selection again, we re-select it below (without the
    // apostrophes).
    wordSelectCursor.setPosition(wordSelectCursor.position() - selectedWord.size());
    if (selectedWord.startsWith(QLatin1Char('\'')) || selectedWord.startsWith(QLatin1Char('\"'))) {
        selectedWord = selectedWord.right(selectedWord.size() - 1);
        wordSelectCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);
    }
    if (selectedWord.endsWith(QLatin1Char('\'')) || selectedWord.endsWith(QLatin1Char('\"'))) {
        selectedWord.chop(1);
    }

    wordSelectCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor,
                                  selectedWord.size());

    const bool wordIsMisspelled = isMouseCursorInsideWord && spellchecker->isActive() &&
                                  !selectedWord.isEmpty() &&
                                  spellchecker->isWordMisspelled(selectedWord);

    if (!wordIsMisspelled) {
        return nullptr;
    }

    if (!selectedWordClicked) {
        // If the user clicked on a misspelled word, select that word.
        setTextCursor(wordSelectCursor);
        cursor = textCursor();
    }

    // Create the suggestion menu
    auto *menu = new QMenu(this);
    // Add the suggestions to the menu
    const QStringList reps = spellchecker->suggestionsForWord(selectedWord, cursor, 8);
    if (reps.isEmpty()) {
        QAction *suggestionsAction = menu->addAction(tr("No suggestions for %1").arg(selectedWord));
        suggestionsAction->setEnabled(false);
    } else {
        for (const QString &rep : reps) {
            menu->addAction(rep, this, [rep, this, cursor]() mutable {
                if (!cursor.isNull()) {
                    cursor.insertText(rep);
                    setTextCursor(cursor);
                }
            });
        }
    }

    menu->addSeparator();

    menu->addAction(tr("Ignore"), this,
                    [selectedWord]() { QOwnSpellChecker::instance()->ignoreWord(selectedWord); });
    menu->addAction(tr("Add to Dictionary"), this, [this, selectedWord, selectedWordBlock]() {
        QOwnSpellChecker::instance()->addWordToDictionary(selectedWord);
        if (highlighter() && document()) {
            const QTextBlock block = document()->findBlockByNumber(selectedWordBlock);
            if (block.isValid()) {
                highlighter()->rehighlightBlock(block);
            }
        }
    });

    menu->setTitle(tr("Spelling"));

    return menu;
}

bool QOwnNotesMarkdownTextEdit::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);

        if (objectName() == QStringLiteral("encryptedNoteTextEdit") ||
            objectName() == QStringLiteral("noteTextEdit")) {
            // deactivating the search widget has priority
            if ((keyEvent->key() == Qt::Key_Escape) && _searchWidget->isVisible()) {
                _searchWidget->deactivate();
                return true;
            } else if (!Utils::Misc::isNoteEditingAllowed()) {
                const auto noModifierKeys = QList<int>()
                                            << Qt::Key_Return << Qt::Key_Enter << Qt::Key_Space
                                            << Qt::Key_Backspace << Qt::Key_Delete << Qt::Key_Tab
                                            << Qt::Key_Backtab << Qt::Key_Minus << Qt::Key_ParenLeft
                                            << Qt::Key_BraceLeft << Qt::Key_BracketLeft
                                            << Qt::Key_Plus << Qt::Key_Comma << Qt::Key_Period;

                const auto controlModifierKeys = QList<int>() << Qt::Key_V << Qt::Key_Space;

                // show notification if user tries to edit a note while
                // note editing is turned off
                if (((keyEvent->key() < 128 || noModifierKeys.contains(keyEvent->key())) &&
                     keyEvent->modifiers().testFlag(Qt::NoModifier)) ||
                    (controlModifierKeys.contains(keyEvent->key()) &&
                     keyEvent->modifiers().testFlag(Qt::ControlModifier) && isReadOnly())) {
                    if (MainWindow::instance() && MainWindow::instance()->doNoteEditingCheck()) {
                        // If the answer is overridden to Yes ("Don't ask again" with "Yes"),
                        // what you type then only enables note editing, but is not typed in
                        // the editor. We need to re-send the event after enabling editing.
                        // BUT, we should do that only if the msgbox is overridden to Yes,
                        // not if manually answered.
                        // You may see: https://github.com/pbek/QOwnNotes/issues/2421
                        // This check is partially copied from utils/gui.cpp showMessage()
                        SettingsService settings;
                        const QString settingsKey =
                            QStringLiteral("MessageBoxOverride/readonly-mode-allow");
                        auto overrideButton = static_cast<QMessageBox::StandardButton>(
                            settings.value(settingsKey, QMessageBox::NoButton).toInt());
                        if (overrideButton == QMessageBox::Yes) {
                            // overridden to answer yes: re-send the event
                            return QMarkdownTextEdit::eventFilter(obj, event);
                        }
                    }

                    return true;
                }
            } else {
                // disable note editing if escape key was pressed
                if (keyEvent->key() == Qt::Key_Escape) {
                    MainWindow::instance()->disallowNoteEditing();

                    return true;
                } else if ((keyEvent->key() == Qt::Key_Tab) ||
                           (keyEvent->key() == Qt::Key_Backtab)) {
                    // handle entered tab and reverse tab keys
                    return handleTabEntered(keyEvent->key() == Qt::Key_Backtab,
                                            Utils::Misc::indentCharacters());
                }
            }
        }
    }

    return QMarkdownTextEdit::eventFilter(obj, event);
}

void QOwnNotesMarkdownTextEdit::updateIgnoredClickUrlRegexps() {
    NextcloudDeckService nextcloudDeckService(this);

    if (nextcloudDeckService.isEnabledAndValid()) {
        QList<QRegularExpression> ignoredClickUrlRegexps;
        ignoredClickUrlRegexps.append(QRegularExpression(
            QRegularExpression::escape(nextcloudDeckService.getCardUrlPattern())));
        setIgnoredClickUrlRegexps(ignoredClickUrlRegexps);
    }
}

/**
 * Requests AI autocomplete for the current text
 */
void QOwnNotesMarkdownTextEdit::requestAiAutocomplete() {
    qDebug() << __func__ << " - called";

    if (!OpenAiService::getAutocompleteEnabled() || !OpenAiService::getEnabled()) {
        qDebug() << __func__ << " - autocomplete not enabled, returning";
        return;
    }

    qDebug() << __func__ << " - autocomplete is enabled, proceeding";

    // Don't autocomplete if there's a selection
    QTextCursor cursor = textCursor();
    if (cursor.hasSelection()) {
        qDebug() << __func__ << " - cursor has selection, returning";
        return;
    }

    // Get the current text context (e.g., last 200 characters)
    cursor.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor);
    QString context = cursor.selectedText();

    // Limit context to last 500 characters to avoid too large prompts
    if (context.length() > 500) {
        context = context.right(500);
    }

    qDebug() << __func__ << " - context:" << context;

    // Don't request if context is too short
    if (context.trimmed().length() < 10) {
        qDebug() << __func__ << " - context too short, returning";
        return;
    }

    // Store the current cursor position
    _aiAutocompletePosition = textCursor().position();

    qDebug() << __func__ << " - calling completeAsync, position:" << _aiAutocompletePosition;

    // Request completion from OpenAI service
    OpenAiService::instance()->completeAsync(context);
}

/**
 * Shows the AI autocomplete suggestion
 */
void QOwnNotesMarkdownTextEdit::showAiAutocompleteSuggestion(const QString &suggestion) {
    qDebug() << "=== showAiAutocompleteSuggestion CALLED ===" << this;
    qDebug() << __func__ << " - Widget:" << objectName();

    SettingsService settings;
    bool enabled = settings.value(QStringLiteral("ai/autocompleteEnabled")).toBool();
    qDebug() << __func__ << " - ai/autocompleteEnabled setting:" << enabled;

    if (!enabled) {
        qDebug() << __func__ << " - autocomplete not enabled in settings, returning";
        return;
    }

    qDebug() << __func__ << " - 'suggestion': " << suggestion;

    if (suggestion.isEmpty()) {
        qDebug() << __func__ << " - suggestion is empty, returning";
        return;
    }

    // Check if cursor hasn't moved too far from the autocomplete position
    int currentPos = textCursor().position();
    qDebug() << __func__ << " - 'currentPos': " << currentPos
             << " '_aiAutocompletePosition': " << _aiAutocompletePosition;

    if (currentPos < _aiAutocompletePosition || currentPos > _aiAutocompletePosition + 50) {
        // Cursor has moved too much, don't show suggestion
        qDebug() << __func__ << " - cursor moved too much, returning";
        return;
    }

    _aiAutocompleteSuggestion = suggestion;
    _isInsertingAiSuggestion = true;

    qDebug() << __func__ << " - inserting suggestion text...";

    // Insert the suggestion with a gray color format
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();

    // Store the original format to restore it later
    QTextCharFormat originalFormat = cursor.charFormat();

    // Create the suggestion format
    QTextCharFormat format;
    format.setForeground(QColor(128, 128, 128));    // Gray color
    format.setFontItalic(true);

    cursor.insertText(_aiAutocompleteSuggestion, format);

    // Select the suggestion so it can be easily replaced
    cursor.setPosition(currentPos);
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor,
                        _aiAutocompleteSuggestion.length());

    // Reset the character format to the original to prevent the italic format
    // from affecting subsequent text
    cursor.setCharFormat(originalFormat);

    cursor.endEditBlock();
    setTextCursor(cursor);

    qDebug() << __func__ << " - suggestion inserted and selected successfully!";

    _isInsertingAiSuggestion = false;
}

/**
 * Clears the AI autocomplete suggestion
 */
void QOwnNotesMarkdownTextEdit::clearAiAutocompleteSuggestion() {
    if (_aiAutocompleteSuggestion.isEmpty()) {
        return;
    }

    _isInsertingAiSuggestion = true;

    QTextCursor cursor = textCursor();

    // If we have selected text that matches our suggestion, delete it
    if (cursor.hasSelection()) {
        QString selectedText = cursor.selectedText();
        if (selectedText == _aiAutocompleteSuggestion) {
            cursor.removeSelectedText();
            setTextCursor(cursor);
        }
    }

    _aiAutocompleteSuggestion.clear();
    _aiAutocompletePosition = -1;
    _isInsertingAiSuggestion = false;
}

/**
 * Accepts the current AI autocomplete suggestion
 */
void QOwnNotesMarkdownTextEdit::acceptAiAutocompleteSuggestion() {
    if (_aiAutocompleteSuggestion.isEmpty()) {
        return;
    }

    _isInsertingAiSuggestion = true;

    QTextCursor cursor = textCursor();

    // If we have the suggestion selected, replace it with normal formatted text
    if (cursor.hasSelection()) {
        QString selectedText = cursor.selectedText();
        if (selectedText == _aiAutocompleteSuggestion) {
            // Remove the formatted suggestion
            cursor.removeSelectedText();
            // Insert as normal text
            cursor.insertText(_aiAutocompleteSuggestion);
            setTextCursor(cursor);
        }
    }

    _aiAutocompleteSuggestion.clear();
    _aiAutocompletePosition = -1;
    _isInsertingAiSuggestion = false;
}

/**
 * Called when AI autocomplete is completed
 */
void QOwnNotesMarkdownTextEdit::onAiAutocompleteCompleted(const QString &result) {
    qDebug() << "=== onAiAutocompleteCompleted CALLED ===" << this;
    qDebug() << __func__ << " - Widget:" << objectName()
             << "Parent:" << (parent() ? parent()->objectName() : "null");
    qDebug() << __func__ << " - 'result': " << result;

    if (result.isEmpty()) {
        qDebug() << __func__ << " - result is empty, returning";
        return;
    }

    qDebug() << __func__ << " - Processing result, length:" << result.length();

    // Extract the first line or first sentence as suggestion
    QString suggestion = result.trimmed();

    // Take only the first line or up to 100 characters
    int newlinePos = suggestion.indexOf('\n');
    if (newlinePos > 0 && newlinePos < 100) {
        suggestion = suggestion.left(newlinePos);
    } else if (suggestion.length() > 100) {
        suggestion = suggestion.left(100);
        // Try to break at a word boundary
        int lastSpace = suggestion.lastIndexOf(' ');
        if (lastSpace > 50) {
            suggestion = suggestion.left(lastSpace);
        }
    }

    qDebug() << __func__ << " - 'suggestion': " << suggestion;
    qDebug() << __func__ << " - 'cursor position': " << textCursor().position();
    qDebug() << __func__ << " - '_aiAutocompletePosition': " << _aiAutocompletePosition;

    qDebug() << __func__ << " - Calling showAiAutocompleteSuggestion...";
    showAiAutocompleteSuggestion(suggestion);
    qDebug() << __func__ << " - showAiAutocompleteSuggestion returned";
}

/**
 * Called when AI autocomplete request times out or errors
 */
void QOwnNotesMarkdownTextEdit::onAiAutocompleteTimeout(const QString &errorString) {
    qDebug() << __func__ << " - error:" << errorString;
    // Just clear the state, don't show error to user
    _aiAutocompleteSuggestion.clear();
    _aiAutocompletePosition = -1;
}

/**
 * Override keyPressEvent to handle Tab and Escape for autocomplete
 */
void QOwnNotesMarkdownTextEdit::keyPressEvent(QKeyEvent *e) {
    // Handle Tab key to accept AI autocomplete suggestion
    if (e->key() == Qt::Key_Tab && !_aiAutocompleteSuggestion.isEmpty()) {
        acceptAiAutocompleteSuggestion();
        e->accept();
        return;
    }

    // Handle Escape key to dismiss AI autocomplete suggestion
    if (e->key() == Qt::Key_Escape && !_aiAutocompleteSuggestion.isEmpty()) {
        clearAiAutocompleteSuggestion();
        e->accept();
        return;
    }

    // Clear suggestion on any other key press
    if (!_aiAutocompleteSuggestion.isEmpty()) {
        clearAiAutocompleteSuggestion();
    }

    // Call parent implementation
    QMarkdownTextEdit::keyPressEvent(e);
}

/**
 * Override focusInEvent to register this editor as active when it receives focus
 */
void QOwnNotesMarkdownTextEdit::focusInEvent(QFocusEvent *e) {
    // Register as the active editor for autocomplete when receiving focus
    // Skip for log widgets
    if (objectName() != QStringLiteral("logTextEdit")) {
        qDebug() << __func__ << " - Registering as active editor:" << this << objectName();
        registerAsActiveEditor();
    }

    // Call parent implementation
    QMarkdownTextEdit::focusInEvent(e);
}

/**
 * Register this editor as the active one for AI autocomplete
 */
void QOwnNotesMarkdownTextEdit::registerAsActiveEditor() {
    qDebug() << __func__ << " - Registering editor:" << this << objectName();
    _activeAutocompleteEditor = this;
    // Also store in QApplication property for access from OpenAiService callback
    qApp->setProperty("activeAutocompleteEditor", QVariant::fromValue<QObject *>(this));
}

/**
 * Unregister this editor from receiving AI autocomplete
 */
void QOwnNotesMarkdownTextEdit::unregisterAsActiveEditor() {
    qDebug() << __func__ << " - Unregistering editor:" << this << objectName();
    if (_activeAutocompleteEditor == this) {
        _activeAutocompleteEditor = nullptr;
        // Also clear QApplication property
        qApp->setProperty("activeAutocompleteEditor", QVariant::fromValue<QObject *>(nullptr));
    }
}

/**
 * Get the currently active editor for AI autocomplete
 */
QOwnNotesMarkdownTextEdit *QOwnNotesMarkdownTextEdit::getActiveEditorForAutocomplete() {
    return _activeAutocompleteEditor;
}

QOwnNotesMarkdownTextEdit::~QOwnNotesMarkdownTextEdit() {
    qDebug() << "*** QOwnNotesMarkdownTextEdit DESTROYED ***" << this << objectName();
    closeMarkdownLspDocument();
    // Unregister if this was the active editor
    unregisterAsActiveEditor();
}

void QOwnNotesMarkdownTextEdit::setMarkdownLspDocumentPath(const QString &filePath,
                                                           const QString &text) {
    if (!_markdownLspEnabled) {
        return;
    }

    const QString uri = QUrl::fromLocalFile(filePath).toString();
    if (uri.isEmpty()) {
        return;
    }

    if (_markdownLspUri == uri) {
        return;
    }

    if (_markdownLspClient && !_markdownLspUri.isEmpty()) {
        _markdownLspClient->didClose(_markdownLspUri);
    }

    _markdownLspUri = uri;
    _markdownLspVersion = 1;

    if (_markdownLspClient) {
        _markdownLspClient->didOpen(_markdownLspUri, QStringLiteral("markdown"), text,
                                    _markdownLspVersion);
    }
}

void QOwnNotesMarkdownTextEdit::closeMarkdownLspDocument() {
    if (_markdownLspClient && !_markdownLspUri.isEmpty()) {
        _markdownLspClient->didClose(_markdownLspUri);
    }

    _markdownLspUri.clear();
    _markdownLspVersion = 0;
    _markdownLspDiagnosticsSelections.clear();
    applyMarkdownLspDiagnosticsSelections();
}

void QOwnNotesMarkdownTextEdit::applyMarkdownLspSettings() {
    SettingsService settings;
    const bool enabled =
        settings.value(QStringLiteral("Editor/markdownLspEnabled"), false).toBool();
    const QString command =
        settings.value(QStringLiteral("Editor/markdownLspCommand"), QStringLiteral("marksman"))
            .toString();
    const QStringList arguments =
        settings.value(QStringLiteral("Editor/markdownLspArguments")).toStringList();

    if (!enabled) {
        _markdownLspEnabled = false;
        if (_markdownLspClient) {
            closeMarkdownLspDocument();
            _markdownLspClient->shutdown();
        }
        return;
    }

    if (!_markdownLspClient) {
        _markdownLspClient = new MarkdownLspClient(this);
        connect(_markdownLspClient, &MarkdownLspClient::completionReceived, this,
                &QOwnNotesMarkdownTextEdit::showMarkdownLspCompletions);
        connect(_markdownLspClient, &MarkdownLspClient::diagnosticsReceived, this,
                &QOwnNotesMarkdownTextEdit::showMarkdownLspDiagnostics);
        connect(_markdownLspClient, &MarkdownLspClient::errorMessage, this,
                [this](const QString &message) {
                    if (!message.trimmed().isEmpty()) {
                        qWarning() << "Markdown LSP:" << message.trimmed();
                    }
                });
    }

    _markdownLspClient->setServerCommand(command, arguments);
    if (_markdownLspClient->start()) {
        const QString rootPath = NoteFolder::currentLocalPath();
        _markdownLspClient->initialize(rootPath, QStringLiteral("QOwnNotes"),
                                       QStringLiteral(VERSION));
    }

    _markdownLspEnabled = true;

    if (!_markdownLspUri.isEmpty()) {
        _markdownLspVersion = 1;
        _markdownLspClient->didOpen(_markdownLspUri, QStringLiteral("markdown"), toPlainText(),
                                    _markdownLspVersion);
    }
}

void QOwnNotesMarkdownTextEdit::scheduleMarkdownLspChange() {
    if (!_markdownLspEnabled || !_markdownLspClient || _markdownLspUri.isEmpty()) {
        return;
    }

    _markdownLspPendingText = toPlainText();
    _markdownLspChangeTimer->start();
}

void QOwnNotesMarkdownTextEdit::sendMarkdownLspChange() {
    if (!_markdownLspEnabled || !_markdownLspClient || _markdownLspUri.isEmpty()) {
        return;
    }

    _markdownLspVersion++;
    _markdownLspClient->didChange(_markdownLspUri, _markdownLspPendingText, _markdownLspVersion);
}

void QOwnNotesMarkdownTextEdit::showMarkdownLspCompletions(int requestId,
                                                           const QStringList &items) {
    if (requestId != _markdownLspCompletionRequestId || _markdownLspCompletionRequestId == -1) {
        return;
    }

    QMenu menu;
    for (const QString &text : items) {
        auto *action = menu.addAction(text);
        action->setData(text);
        action->setWhatsThis(QStringLiteral("autocomplete"));
    }

    _markdownLspCompletionRequestId = -1;

    if (menu.actions().isEmpty()) {
        return;
    }

    QPoint globalPos = mapToGlobal(cursorRect().bottomRight());
    globalPos.setY(globalPos.y() + viewportMargins().top());
    globalPos.setX(globalPos.x() + viewportMargins().left());

    QAction *selectedItem = menu.exec(globalPos);
    if (!selectedItem) {
        return;
    }

    const QString text = selectedItem->data().toString();
    if (text.isEmpty()) {
        return;
    }

    QTextCursor c = textCursor();
    c.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
    c.insertText(text + QStringLiteral(" "));
}

void QOwnNotesMarkdownTextEdit::showMarkdownLspDiagnostics(
    const QString &uri, const QVector<MarkdownLspClient::Diagnostic> &diagnostics) {
    if (uri != _markdownLspUri) {
        return;
    }

    _markdownLspDiagnosticsSelections.clear();
    for (const MarkdownLspClient::Diagnostic &diagnostic : diagnostics) {
        if (diagnostic.message.isEmpty()) {
            continue;
        }

        const QTextBlock startBlock = document()->findBlockByNumber(diagnostic.range.startLine);
        if (!startBlock.isValid()) {
            continue;
        }

        const QTextBlock endBlock = document()->findBlockByNumber(diagnostic.range.endLine);
        if (!endBlock.isValid()) {
            continue;
        }

        const int startPosition = startBlock.position() + diagnostic.range.startCharacter;
        const int endPosition = endBlock.position() + diagnostic.range.endCharacter;
        if (endPosition <= startPosition) {
            continue;
        }

        // Choose underline color based on LSP severity:
        // 1 = Error, 2 = Warning, 3 = Information, 4 = Hint
        QColor underlineColor;
        switch (diagnostic.severity) {
            case 1:
                underlineColor = QColor(214, 68, 68);    // Red for errors
                break;
            case 2:
                underlineColor = QColor(210, 140, 30);    // Orange for warnings
                break;
            case 3:
                underlineColor = QColor(80, 140, 210);    // Blue for information
                break;
            case 4:
                underlineColor = QColor(100, 170, 100);    // Green for hints
                break;
            default:
                underlineColor = QColor(214, 68, 68);    // Default to red
                break;
        }

        QTextCursor cursor(document());
        cursor.setPosition(startPosition);
        cursor.setPosition(endPosition, QTextCursor::KeepAnchor);

        QTextEdit::ExtraSelection selection;
        selection.cursor = cursor;
        selection.format.setUnderlineStyle(QTextCharFormat::WaveUnderline);
        selection.format.setUnderlineColor(underlineColor);
        selection.format.setToolTip(diagnostic.message);
        selection.format.setProperty(kMarkdownLspDiagnosticProperty, true);
        _markdownLspDiagnosticsSelections.append(selection);
    }

    applyMarkdownLspDiagnosticsSelections();
}

bool QOwnNotesMarkdownTextEdit::viewportEvent(QEvent *event) {
    if (event->type() == QEvent::ToolTip && !_markdownLspDiagnosticsSelections.isEmpty()) {
        auto *helpEvent = static_cast<QHelpEvent *>(event);
        const QPoint pos = helpEvent->pos();
        const int cursorPos = cursorForPosition(pos).position();

        // Find the first diagnostic selection that covers the cursor position
        for (const QTextEdit::ExtraSelection &selection :
             std::as_const(_markdownLspDiagnosticsSelections)) {
            const int selStart = selection.cursor.selectionStart();
            const int selEnd = selection.cursor.selectionEnd();
            if (cursorPos >= selStart && cursorPos <= selEnd) {
                const QString tip = selection.format.toolTip();
                if (!tip.isEmpty()) {
                    QToolTip::showText(helpEvent->globalPos(), tip, this);
                    event->accept();
                    return true;
                }
            }
        }

        QToolTip::hideText();
    }
    return QMarkdownTextEdit::viewportEvent(event);
}

void QOwnNotesMarkdownTextEdit::applyMarkdownLspDiagnosticsSelections() {
    const QList<QTextEdit::ExtraSelection> selections = extraSelections();
    QList<QTextEdit::ExtraSelection> filteredSelections;
    filteredSelections.reserve(selections.size());
    for (const QTextEdit::ExtraSelection &selection : selections) {
        if (selection.format.property(kMarkdownLspDiagnosticProperty).toBool()) {
            continue;
        }
        filteredSelections.append(selection);
    }

    filteredSelections += _markdownLspDiagnosticsSelections;
    setExtraSelections(filteredSelections);
}
