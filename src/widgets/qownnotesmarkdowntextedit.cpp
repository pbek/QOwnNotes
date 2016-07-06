#include <QFont>
#include <QSettings>
#include <QDebug>
#include <QRegularExpression>
#include <QMimeData>
#include <QFontDatabase>
#include "qownnotesmarkdowntextedit.h"

QOwnNotesMarkdownTextEdit::QOwnNotesMarkdownTextEdit(QWidget *parent)
        : QMarkdownTextEdit(parent) {
    setStyles();
}

#define STY(type, format) styles->append((HighlightingStyle){type, format})

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
    bool darkModeColors = settings.value("darkModeColors").toBool();

    // set the tab stop to the width of 4 spaces in the editor
    const int tabStop = 4;
    QFontMetrics metrics(font);
    setTabStopWidth(tabStop * metrics.width(' '));

    QVector<HighlightingStyle> *styles = new QVector<HighlightingStyle>();

    QTextCharFormat headers;
    headers.setForeground(QBrush(darkModeColors ?
                                 QColor(158, 182, 255) : QColor(0, 49, 110)));

    if (!darkModeColors) {
        headers.setBackground(QBrush(QColor(235, 235, 240)));
    }
    headers.setFontWeight(QFont::Bold);
    headers.setFontPointSize(defaultFontSize * 1.2);
    STY(pmh_H1, headers);

    headers.setFontPointSize(defaultFontSize * 1.1);
    STY(pmh_H2, headers);

    headers.setFontPointSize(defaultFontSize);
    STY(pmh_H3, headers);
    STY(pmh_H4, headers);
    STY(pmh_H5, headers);
    STY(pmh_H6, headers);

    QTextCharFormat hrule;
    hrule.setForeground(QBrush(Qt::darkGray));
    hrule.setBackground(QBrush(Qt::lightGray));
    STY(pmh_HRULE, hrule);

    /* <ul> */
    QTextCharFormat list;
    list.setForeground(QBrush(darkModeColors ? Qt::yellow : QColor(163, 0, 123)));
    STY(pmh_LIST_BULLET, list);
    STY(pmh_LIST_ENUMERATOR, list);

    /* <a href> */
    QTextCharFormat link;
    link.setForeground(QBrush(QColor(252, 126, 0)));
    link.setBackground(QBrush(darkModeColors ?
                              QColor(51, 51, 51) : QColor(255, 242, 228)));
    STY(pmh_LINK, link);
    STY(pmh_AUTO_LINK_URL, link);
    STY(pmh_AUTO_LINK_EMAIL, link);

    /* <img> */
    QTextCharFormat image;
    image.setForeground(QBrush(QColor(0, 191, 0)));
    image.setBackground(QBrush(QColor(228, 255, 228)));
    STY(pmh_IMAGE, image);

    QTextCharFormat ref;
    ref.setForeground(QBrush(QColor(213, 178, 178)));
    STY(pmh_REFERENCE, ref);

    /* <pre> */
    QTextCharFormat code;
    code.setForeground(QBrush(darkModeColors ? Qt::green : Qt::darkGreen));
    code.setBackground(QBrush(darkModeColors ?
                              QColor(51, 51, 51) : QColor(226, 231, 226)));

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

    STY(pmh_CODE, code);
    STY(pmh_VERBATIM, code);

    /* <em> */
    QTextCharFormat emph;
    emph.setForeground(QBrush(darkModeColors ?
                              QColor(158, 182, 255) : QColor(0, 87, 174)));
    emph.setFontItalic(true);
    STY(pmh_EMPH, emph);

    /* <strong> */
    QTextCharFormat strong;
    strong.setForeground(QBrush(darkModeColors ?
                              QColor(158, 182, 255) : QColor(0, 66, 138)));
    strong.setFontWeight(QFont::Bold);
    STY(pmh_STRONG, strong);

    QTextCharFormat comment;
    comment.setForeground(QBrush(Qt::gray));
    STY(pmh_COMMENT, comment);

    QTextCharFormat blockquote;
    blockquote.setForeground(QBrush(Qt::darkRed));
    STY(pmh_BLOCKQUOTE, blockquote);

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
