#include <QFont>
#include <QSettings>
#include <QDebug>
#include <QRegularExpression>
#include <QMimeData>
#include <QFontDatabase>
#include <utils/schema.h>
#include "qownnotesmarkdowntextedit.h"

QOwnNotesMarkdownTextEdit::QOwnNotesMarkdownTextEdit(QWidget *parent)
        : QMarkdownTextEdit(parent) {
    setStyles();
    updateSettings();
}

/**
 * Sets the format style
 *
 * @param index
 * @param styles
 */
void QOwnNotesMarkdownTextEdit::setFormatStyle(
        pmh_element_type index, QVector<HighlightingStyle> *styles) {
    QTextCharFormat format;
    Utils::Schema::setFormatStyle(index, format);
    styles->append({index, format});

// The initial define causes an error with Visual Studio 2015:
// "error C4576: a parenthesized type followed by an initializer list is
// a non-standard explicit type conversion syntax"
// The replacement works, probably also on other platforms (to be tested)
// styles->append((HighlightingStyle){index, format});
}

/**
 * Sets the highlighting styles for the text edit
 */
void QOwnNotesMarkdownTextEdit::setStyles() {
    QFont font = Utils::Schema::getEditorTextFont();
    setFont(font);

    // set the tab stop to the width of 4 spaces in the editor
    const int tabStop = 4;
    QFontMetrics metrics(font);
    setTabStopWidth(tabStop * metrics.width(' '));

    QVector<HighlightingStyle> *styles = new QVector<HighlightingStyle>();

    setFormatStyle(pmh_H1, styles);
    setFormatStyle(pmh_H2, styles);
    setFormatStyle(pmh_H3, styles);
    setFormatStyle(pmh_H4, styles);
    setFormatStyle(pmh_H5, styles);
    setFormatStyle(pmh_H6, styles);

    setFormatStyle(pmh_HRULE, styles);

    /* <ul> */
    setFormatStyle(pmh_LIST_BULLET, styles);
    setFormatStyle(pmh_LIST_ENUMERATOR, styles);

    /* <a href> */
    setFormatStyle(pmh_LINK, styles);
    setFormatStyle(pmh_AUTO_LINK_URL, styles);
    setFormatStyle(pmh_AUTO_LINK_EMAIL, styles);

    /* <img> */
    setFormatStyle(pmh_IMAGE, styles);

    setFormatStyle(pmh_REFERENCE, styles);

    /* <pre> */
    setFormatStyle(pmh_CODE, styles);
    setFormatStyle(pmh_VERBATIM, styles);

    /* <em> */
    setFormatStyle(pmh_EMPH, styles);

    /* <strong> */
    setFormatStyle(pmh_STRONG, styles);

    setFormatStyle(pmh_COMMENT, styles);
    setFormatStyle(pmh_BLOCKQUOTE, styles);

    setFormatStyle(pmh_HTML, styles);
    setFormatStyle(pmh_HTML_ENTITY, styles);
    setFormatStyle(pmh_HTMLBLOCK, styles);

    setFormatStyle(pmh_NOTE, styles);

    _highlighter->setStyles(*styles);
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
                QTextEdit textEdit;
                int newFontSize = textEdit.font().pointSize();
                if ( fontSize != newFontSize ) {
                    fontSize = newFontSize;
                    doSetStyles = true;
                }
        }

        font.setPointSize(fontSize);

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
                QTextEdit textEdit;
                int newFontSize = textEdit.font().pointSize();
                if ( fontSize != newFontSize ) {
                    fontSize = newFontSize;
                    doSetStyles = true;
                }
        }

        font.setPointSize(codeFontSize);

        // store the font settings
        settings.setValue("MainWindow/noteTextEdit.code.font", font.toString());
    }

    if (doSetStyles) {
        this->setStyles();
        highlighter()->parse();
    }

    return fontSize;
}

/**
 * Handles clicked urls (including relative urls)
 *
 * examples:
 * - <http://www.qownnotes.org> opens the webpage
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

    if (isInDistractionFreeMode) {
        int margin = 0;
        int editorWidthMode =
                settings.value("DistractionFreeMode/editorWidthMode").toInt();

        if (editorWidthMode != Full) {
            QFontMetrics metrics(font());
            // take the size of an "O" character
            metrics.width('O');

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

            // set the size of characterAmount times the "O" characters
            int proposedEditorWidth = characterAmount * metrics.width('O');

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
}
