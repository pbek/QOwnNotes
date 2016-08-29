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
}

// The initial define causes an error with Visual Studio 2015:
// "error C4576: a parenthesized type followed by an initializer list is
// a non-standard explicit type conversion syntax"
// The replacement works, probably also on other platforms (to be tested)
//#define STY(type, format) styles->append((HighlightingStyle){type, format})
#define STY(type, format) styles->append({type, format})

/**
 * Sets the highlighting styles for the text edit
 */
void QOwnNotesMarkdownTextEdit::setStyles() {
    QSettings settings;
    QFont font = this->font();
    QString fontString = settings.value(
            "MainWindow/noteTextEdit.font").toString();

    if (fontString != "") {
        // set the note text edit font
        font.fromString(fontString);
        setFont(font);
    } else {
        // store the default settings
        fontString = this->font().toString();
        settings.setValue("MainWindow/noteTextEdit.font", fontString);
    }

    int defaultFontSize = font.pointSize();

    // set the tab stop to the width of 4 spaces in the editor
    const int tabStop = 4;
    QFontMetrics metrics(font);
    setTabStopWidth(tabStop * metrics.width(' '));

    QVector<HighlightingStyle> *styles = new QVector<HighlightingStyle>();

    QTextCharFormat headers;
    headers.setFontWeight(QFont::Bold);
    headers.setFontPointSize(defaultFontSize * 1.2);
    Utils::Schema::setFormatColors(pmh_H1, headers);
    STY(pmh_H1, headers);

    headers.setFontPointSize(defaultFontSize * 1.1);
    Utils::Schema::setFormatColors(pmh_H2, headers);
    STY(pmh_H2, headers);

    headers.setFontPointSize(defaultFontSize);
    Utils::Schema::setFormatColors(pmh_H3, headers);
    STY(pmh_H3, headers);
    Utils::Schema::setFormatColors(pmh_H4, headers);
    STY(pmh_H4, headers);
    Utils::Schema::setFormatColors(pmh_H5, headers);
    STY(pmh_H5, headers);
    Utils::Schema::setFormatColors(pmh_H6, headers);
    STY(pmh_H6, headers);

    QTextCharFormat hrule;
    Utils::Schema::setFormatColors(pmh_HRULE, hrule);
    STY(pmh_HRULE, hrule);

    /* <ul> */
    QTextCharFormat list;
    Utils::Schema::setFormatColors(pmh_LIST_BULLET, list);
    STY(pmh_LIST_BULLET, list);
    Utils::Schema::setFormatColors(pmh_LIST_ENUMERATOR, list);
    STY(pmh_LIST_ENUMERATOR, list);

    /* <a href> */
    QTextCharFormat link;
    Utils::Schema::setFormatColors(pmh_LINK, link);
    STY(pmh_LINK, link);
    Utils::Schema::setFormatColors(pmh_AUTO_LINK_URL, link);
    STY(pmh_AUTO_LINK_URL, link);
    Utils::Schema::setFormatColors(pmh_AUTO_LINK_EMAIL, link);
    STY(pmh_AUTO_LINK_EMAIL, link);

    /* <img> */
    QTextCharFormat image;
    Utils::Schema::setFormatColors(pmh_IMAGE, image);
    STY(pmh_IMAGE, image);

    QTextCharFormat ref;
    Utils::Schema::setFormatColors(pmh_REFERENCE, ref);
    STY(pmh_REFERENCE, ref);

    /* <pre> */
    QTextCharFormat code;
    QFont codeFont = this->font();

    fontString = settings.value(
            "MainWindow/noteTextEdit.code.font").toString();

    if (fontString != "") {
        // set the code font
        codeFont.fromString(fontString);
    } else {
        codeFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);

        // store the default settings
        settings.setValue(
                "MainWindow/noteTextEdit.code.font", codeFont.toString());
    }

    code.setFont(codeFont);

    Utils::Schema::setFormatColors(pmh_CODE, code);
    STY(pmh_CODE, code);
    Utils::Schema::setFormatColors(pmh_VERBATIM, code);
    STY(pmh_VERBATIM, code);

    /* <em> */
    QTextCharFormat emph;
    emph.setFontItalic(true);
    Utils::Schema::setFormatColors(pmh_EMPH, emph);
    STY(pmh_EMPH, emph);

    /* <strong> */
    QTextCharFormat strong;
    strong.setFontWeight(QFont::Bold);
    Utils::Schema::setFormatColors(pmh_STRONG, strong);
    STY(pmh_STRONG, strong);

    QTextCharFormat comment;
    Utils::Schema::setFormatColors(pmh_COMMENT, comment);
    STY(pmh_COMMENT, comment);

    QTextCharFormat blockquote;
    Utils::Schema::setFormatColors(pmh_BLOCKQUOTE, blockquote);
    STY(pmh_BLOCKQUOTE, blockquote);

    QTextCharFormat html;
    Utils::Schema::setFormatColors(pmh_HTML, html);
    STY(pmh_HTML, html);
    Utils::Schema::setFormatColors(pmh_HTML_ENTITY, html);
    STY(pmh_HTML_ENTITY, html);
    Utils::Schema::setFormatColors(pmh_HTMLBLOCK, html);
    STY(pmh_HTMLBLOCK, html);

    QTextCharFormat note;
    Utils::Schema::setFormatColors(pmh_NOTE, note);
    STY(pmh_NOTE, note);

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
