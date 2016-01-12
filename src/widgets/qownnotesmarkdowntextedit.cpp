#include <QFont>
#include <QSettings>
#include <QDebug>
#include <QRegularExpression>
#include "qownnotesmarkdowntextedit.h"

QOwnNotesMarkdownTextEdit::QOwnNotesMarkdownTextEdit(QWidget *parent)
 : QMarkdownTextEdit(parent)
{
    // load text edit font

    QSettings settings;
    QFont font = this->font();
    QString fontString = settings.value("MainWindow/noteTextEdit.font").toString();

    if ( fontString != "" )
    {
        // set the note text edit font
        font.fromString( fontString );
        setFont( font );

        // set the default size for the highlighter
        _highlighter->setDefaultStyles( font.pointSize() );
    }

    // set the tab stop to the width of 4 spaces in the editor
    const int tabStop = 4;
    QFontMetrics metrics( font );
    setTabStopWidth( tabStop * metrics.width( ' ' ) );
}

/**
 * Handles clicked urls (including relative urls)
 *
 * examples:
 * - <http://www.qownnotes.org> opens the webpage
 * - <file:///path/to/my/file/QOwnNotes.pdf> opens the file "/path/to/my/file/QOwnNotes.pdf" if the operating system supports that handler
 */
void QOwnNotesMarkdownTextEdit::openUrl( QUrl url )
{
    qDebug() << "QOwnNotesMarkdownTextEdit " << __func__ << " - 'url': " << url;

    QString urlString = url.toString();
    QSettings settings;
    QString notesPath = settings.value( "notesPath" ).toString();

    // parse for relative file urls and make them absolute
    urlString.replace( QRegularExpression( "^file:\\/\\/([^\\/].+)$" ), "file://"+ notesPath +"/\\1" );

    QMarkdownTextEdit::openUrl( QUrl( urlString ) );
}
