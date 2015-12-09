#include "note.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include <QApplication>
#include <QFile>
#include <QSettings>
#include <QDir>
#include <QSqlError>
#include <QRegularExpression>
#include "libraries/hoedown/html.h"


Note::Note()
{
    this->id = 0;
    this->hasDirtyData = false;
}

int Note::getId()
{
    return this->id;
}

QString Note::getName()
{
    return this->name;
}

QString Note::getFileName()
{
    return this->fileName;
}

QString Note::getNoteText()
{
    return this->noteText;
}

bool Note::getHasDirtyData()
{
    return this->hasDirtyData;
}

void Note::setName(QString text)
{
    this->name = text;
}

void Note::setNoteText(QString text)
{
    this->noteText = text;
}

bool Note::createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    if (!db.open())
    {
        QMessageBox::critical(0, qApp->tr("Cannot open database"),
            qApp->tr("Unable to establish a database connection.\n"
                      "This application needs SQLite support. Please read "
                      "the Qt SQL driver documentation for information how "
                      "to build it.\n\n"
                      "Click Cancel to exit."), QMessageBox::Cancel);
        return false;
    }

    return true;
}

bool Note::setupTables()
{
    QSqlQuery query;
    query.exec("create table note (id integer primary key, "
               "name varchar(255), file_name varchar(255), note_text text,"
               "has_dirty_data integer default 0,"
               "file_last_modified datetime,"
               "file_created datetime,"
               "created datetime default current_timestamp,"
               "modified datetime default current_timestamp)");

    return true;
}

bool Note::addNote( QString name, QString fileName, QString text )
{
    QSqlQuery query;
    query.prepare( "INSERT INTO note ( name, file_name, note_text ) VALUES ( :name, :file_name, :note_text )" );
    query.bindValue( ":name", name );
    query.bindValue( ":file_name", fileName );
    query.bindValue( ":note_text", text );
    return query.exec();
}

Note Note::fetch( int id )
{
    QSqlQuery query;
    Note note;

    query.prepare( "SELECT * FROM note WHERE id = :id" );
    query.bindValue( ":id", id );

    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
    }
    else
    {
        if ( query.first() )
        {
            note = noteFromQuery( query );
        }
    }

    return note;
}

Note Note::fetchByFileName( QString fileName )
{
    Note note;
    note.fillByFileName( fileName );
    return note;
}

bool Note::fillByFileName( QString fileName )
{
    QSqlQuery query;

    query.prepare( "SELECT * FROM note WHERE file_name = :file_name" );
    query.bindValue( ":file_name", fileName );

    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
    }
    else
    {
        if ( query.first() )
        {
            this->fillFromQuery( query );
            return true;
        }
    }

    return false;
}

bool Note::remove( bool withFile )
{
    QSqlQuery query;

    query.prepare( "DELETE FROM note WHERE id = :id" );
    query.bindValue( ":id", this->id );

    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
        return false;
    }
    else
    {
        if ( withFile )
        {
            this->removeNoteFile();
        }

        return true;
    }
}

/**
 * @brief Copies a note to an other path
 *
 * @param destinationPath
 * @return bool
 */
bool Note::copy( QString destinationPath )
{
    QDir d;
    if ( this->fileExists() && ( d.exists( destinationPath ) ) )
    {
        QFile file( fullNoteFilePath( this->fileName ) );
        QString destinationFileName = destinationPath + QDir::separator() + this->fileName;

        if ( d.exists( destinationFileName ) )
        {
            qDebug() << destinationFileName << "already exists!";

            // find a new filename for the note
            QDateTime currentDateTime = QDateTime::currentDateTime();
            destinationFileName = destinationPath + QDir::separator() + this->name + " " + currentDateTime.toString( Qt::ISODate ).replace( ":", "_" ) + ".txt";

            qDebug() << "New file name:" << destinationFileName;
        }

        return file.copy( destinationFileName );
    }

    return false;
}

/**
 * @brief Moves a note to an other path
 *
 * @param destinationPath
 * @return bool
 */
bool Note::move( QString destinationPath )
{
    bool result = copy( destinationPath );

    if ( result )
    {
        return remove( true );
    }

    return false;
}

Note Note::fetchByName( QString name )
{
    QSqlQuery query;
    Note note;

    query.prepare( "SELECT * FROM note WHERE name = :name" );
    query.bindValue( ":name", name );

    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
    }
    else
    {
        if ( query.first() )
        {
            note = noteFromQuery( query );
        }
    }

    return note;
}

Note Note::noteFromQuery( QSqlQuery query )
{
    Note note;
    note.fillFromQuery( query );
    return note;
}

bool Note::fillFromQuery( QSqlQuery query )
{
    int id = query.value("id").toInt();
    QString name = query.value("name").toString();
    QString fileName = query.value("file_name").toString();
    QString noteText = query.value("note_text").toString();
    bool hasDirtyData = query.value("has_dirty_data").toInt() == 1;
    QDateTime fileCreated = query.value("file_created").toDateTime();
    QDateTime fileLastModified = query.value("file_last_modified").toDateTime();
    QDateTime created = query.value("created").toDateTime();
    QDateTime modified = query.value("modified").toDateTime();

    this->id = id;
    this->name = name;
    this->fileName = fileName;
    this->noteText = noteText;
    this->hasDirtyData = hasDirtyData;
    this->fileCreated = fileCreated;
    this->fileLastModified = fileLastModified;
    this->created = created;
    this->modified = modified;

    return true;
}

QList<Note> Note::fetchAll()
{
    QSqlQuery query;
    QList<Note> noteList;

    query.prepare( "SELECT * FROM note ORDER BY file_last_modified DESC" );
    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
    }
    else
    {
        for( int r=0; query.next(); r++ )
        {
            Note note = noteFromQuery( query );
            noteList.append( note );
        }
    }

    return noteList;
}

QList<Note> Note::search( QString text )
{
    QSqlQuery query;
    QList<Note> noteList;

    query.prepare( "SELECT * FROM note WHERE note_text LIKE :text ORDER BY file_last_modified DESC" );
    query.bindValue( ":text", "%" + text + "%"  );

    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
    }
    else
    {
        for( int r=0; query.next(); r++ )
        {
            Note note = noteFromQuery( query );
            noteList.append( note );
        }
    }

    return noteList;
}

QList<QString> Note::searchAsNameList( QString text, bool searchInName )
{
    QSqlQuery query;
    QList<QString> nameList;
    QString searchField = searchInName ? "name" : "note_text";

    query.prepare( "SELECT name FROM note WHERE " + searchField + " LIKE :text ORDER BY file_last_modified DESC" );
    query.bindValue( ":text", "%" + text + "%"  );

    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
    }
    else
    {
        for( int r=0; query.next(); r++ )
        {
            nameList.append( query.value("name").toString() );
        }
    }

    return nameList;
}

QStringList Note::fetchNoteNames()
{
    QSqlQuery query;
    QStringList list;

    query.prepare( "SELECT name FROM note ORDER BY file_last_modified DESC" );
    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
    }
    else
    {
        for( int r=0; query.next(); r++ )
        {
            list.append( query.value("name").toString() );
        }
    }

    return list;
}

QStringList Note::fetchNoteFileNames()
{
    QSqlQuery query;
    QStringList list;

    query.prepare( "SELECT file_name FROM note ORDER BY file_last_modified DESC" );
    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
    }
    else
    {
        for( int r=0; query.next(); r++ )
        {
            list.append( query.value("file_name").toString() );
        }
    }

    return list;
}

bool Note::storeNewText( QString text ) {
    this->noteText = text;
    this->hasDirtyData = true;

    return this->store();
}

//
// inserts or updates a note object in the database
//
bool Note::store() {
    QSqlQuery query;

    if ( this->fileName == "" )
    {
        this->fileName = this->name + ".txt";
    }

    if ( this->id > 0 )
    {
        query.prepare( "UPDATE note SET "
                       "name = :name, file_name = :file_name, note_text = :note_text, has_dirty_data = :has_dirty_data, "
                       "file_last_modified = :file_last_modified, file_created = :file_created, modified = :modified "
                       "WHERE id = :id" );
        query.bindValue( ":id", this->id );
    }
    else
    {
        query.prepare( "INSERT INTO note"
                       "( name, file_name, note_text, has_dirty_data, file_last_modified, file_created, modified ) "
                       "VALUES ( :name, :file_name, :note_text, :has_dirty_data, :file_last_modified, :file_created, :modified )");
    }

    QDateTime modified = QDateTime::currentDateTime();

    query.bindValue( ":name", this->name );
    query.bindValue( ":file_name", this->fileName );
    query.bindValue( ":note_text", this->noteText );
    query.bindValue( ":has_dirty_data", this->hasDirtyData ? 1 : 0 );
    query.bindValue( ":file_created", this->fileCreated );
    query.bindValue( ":file_last_modified", this->fileLastModified );
    query.bindValue( ":modified", modified );

    // on error
    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
        return false;
    }
    // on insert
    else if ( this->id == 0 )
    {
        this->id = query.lastInsertId().toInt();
    }

    this->modified = modified;
    return true;
}

bool Note::storeNoteTextFileToDisk() {

    // checks if filename has to be changed (and change it if needed)
    this->handleNoteTextFileName();

    QFile file( fullNoteFilePath( this->fileName ) );
    bool fileExists = this->fileExists();

    qDebug() << "storing note file: "  << this->fileName;

    if ( !file.open( QIODevice::WriteOnly | QIODevice::Text ) )
    {
//        QMessageBox::critical( 0, "Can not store note!", "Can not store note <strong>" + this->name + "</strong><br /><br />" + file.errorString() );
        qDebug() << file.errorString();
        return false;
    }

    // transform all types of newline to \n (maybe the ownCloud-sync works better then)
    QString text = this->noteText;
    text.replace( QRegExp( "(\\r\\n)|(\\n\\r)|\\r|\\n" ), "\n" );

//    diff_match_patch *diff = new diff_match_patch();
//    QList<Diff> diffList = diff->diff_main( this->noteText, text );

//    QString html = diff->diff_prettyHtml( diffList );
//    diff->diff_cleanupSemantic( diffList );
//    qDebug() << __func__ << " - 'diffList': " << diffList[0].toString();
//    qDebug() << __func__ << " - 'html': " << html;

    QTextStream out( &file );
    out.setCodec("UTF-8");
    out << text;
    file.flush();
    file.close();

    this->hasDirtyData = false;
    this->fileLastModified = QDateTime::currentDateTime();

    if ( !fileExists ) {
        this->fileCreated = this->fileLastModified;
    }

    return this->store();
}

//
// checks if filename has to be changed
// generates a new name and filename and removes the old file (the new file is not stored to a note text file!)
//
void Note::handleNoteTextFileName() {
    // split the text into a string list
    QStringList noteTextLines = this->noteText.split( QRegExp( "(\\r\\n)|(\\n\\r)|\\r|\\n" ) );

    // do nothing if there is no text
    if ( noteTextLines.count() == 0 ) return;

    QString name = noteTextLines[0];
    // do nothing if the first line is empty
    if ( name == "" ) return;

    // check if name has changed
    if ( name != this->name )
    {
        qDebug() << __func__ << " - 'name' was changed: " << name;
        QString fileName = name + ".txt";

        // check if note with this filename already exists
        Note note = Note::fetchByFileName( fileName );
        if ( note.id > 0 )
        {
            // find new filename for the note (not very safe yet)
            QDateTime currentDateTime = QDateTime::currentDateTime();
            name = name + " " + currentDateTime.toString( Qt::ISODate ).replace( ":", "_" );
        }

        // remove old note file
        // TODO: note doesn't seem to be removed very often
        this->removeNoteFile();

        // update first line of note text
        // TODO: UI has to be updated too then!
        noteTextLines[0] = name;
        this->noteText = noteTextLines.join( "\n" );

        // store new name and filename
        this->name = name;
        this->fileName = name + ".txt";
        this->store();
    }

    qDebug() << __func__ << " - 'name': " << name;
    qDebug() << __func__ << " - 'this->id': " << this->id;
}

bool Note::updateNoteTextFromDisk() {
    QFile file( fullNoteFilePath( this->fileName ) );

    if ( !file.open( QIODevice::ReadOnly ) )
    {
        qDebug() << file.errorString();
        return false;
    }

    QTextStream in( &file );
    in.setCodec("UTF-8");
    this->noteText = in.readAll();
    file.close();

    // strangly it sometimes gets null
    if ( this->noteText.isNull() ) this->noteText = "";

    return true;
}

QString Note::fullNoteFilePath( QString fileName )
{
    QSettings settings;
    QString notesPath = settings.value( "notesPath" ).toString();

    return notesPath + QDir::separator() + fileName;
}

int Note::storeDirtyNotesToDisk( Note &currentNote ) {
    QSqlQuery query;
    Note note;
//    qDebug() << "storeDirtyNotesToDisk";

    query.prepare( "SELECT * FROM note WHERE has_dirty_data = 1" );
    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
        return 0;
    }
    else
    {
        int count = 0;
        for( int r=0; query.next(); r++ )
        {
            note = noteFromQuery( query );
            QString oldFileName = note.getFileName();
            note.storeNoteTextFileToDisk();
            QString newFileName = note.getFileName();

            // reassign currentNote if filename of currentNote has changed
            if ( ( oldFileName == currentNote.getFileName() ) && ( oldFileName != newFileName ) )
            {
                currentNote = note;
                qDebug() << "filename of currentNote has changed to: " << newFileName;
            }

            qDebug() << "stored note: " << note;
            count++;
        }

        return count;
    }
}

void Note::createFromFile( QFile &file ) {
    if ( file.open( QIODevice::ReadOnly ) )
    {
        QTextStream in( &file );
        in.setCodec("UTF-8");

        // qDebug() << file.size() << in.readAll();
        QString noteText = in.readAll();
        file.close();

        QFileInfo fileInfo;
        fileInfo.setFile( file );

        // create a nicer name by removing ".txt"
        // TODO: make sure name is ownCloud Notes conform
        QString name = fileInfo.fileName();
        name.chop( 4 );

        this->name = name;
        this->fileName = fileInfo.fileName();
        this->noteText = noteText;
        this->fileCreated = fileInfo.created();
        this->fileLastModified = fileInfo.lastModified();
        this->store();
    }

}

//
// deletes all notes in the database
//
bool Note::deleteAll() {
    QSqlQuery query;

    // no truncate in sqlite
    query.prepare( "DELETE FROM note" );
    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
        return false;
    }
    else
    {
        return true;
    }
}

//
// checks if file of note exists in the filesystem
//
bool Note::fileExists() {
    QFile file( fullNoteFilePath( this->fileName ) );
    return file.exists();
}

//
// checks if the current note still exists in the database
//
bool Note::exists() {
    Note note = Note::fetch( this->id );
    return note.id > 0;
}

//
// reloads the current Note (by fileName)
//
bool Note::refetch() {
    return this->fillByFileName( this->fileName );
}

//
// remove the file of the note
//
bool Note::removeNoteFile() {
    if ( this->fileExists() )
    {
        QFile file( fullNoteFilePath( this->fileName ) );
        qDebug() << __func__ << " - 'this->fileName': " << this->fileName;
        qDebug() << __func__ << " - 'file': " << file.fileName();
        return file.remove();
    }

    return false;
}

//
// return html rendered markdown of note text
//
QString Note::toMarkdownHtml() {
    hoedown_renderer *renderer = hoedown_html_renderer_new( HOEDOWN_HTML_USE_XHTML, 16 );
    hoedown_extensions extensions = (hoedown_extensions) ( HOEDOWN_EXT_BLOCK | HOEDOWN_EXT_SPAN );
    hoedown_document *document = hoedown_document_new(renderer, extensions, 16);

    QString str = this->noteText;

    unsigned char *sequence = (unsigned char*)qstrdup( str.toUtf8().constData() );
    int length = strlen( (char*) sequence );

    // return an empty string if the note is empty
    if ( length == 0 )
    {
        return "";
    }

    hoedown_buffer *html = hoedown_buffer_new( length );

    // render markdown html
    hoedown_document_render( document, html, sequence, length );

    // get markdown html
    QString result = QString::fromUtf8( (char*) html->data, html->size );

    /* Cleanup */
    free(sequence);
    hoedown_buffer_free(html);

    hoedown_document_free(document);
    hoedown_html_renderer_free(renderer);

    result = "<html><head><style>h1, h2, h3 { margin: 5pt 0 10pt 0; } a { color: #FF9137; text-decoration: none; }</style></head><body>" + result + "</body></html>";

    return result;
}

bool Note::isFetched() {
    return ( this->id > 0 );
}

/**
 * @brief Generates a text that can be used in a link
 * @param text
 * @return
 */
QString Note::generateTextForLink( QString text )
{
    // replace everything but characters and numbers with "_"
    QRegularExpression re( "[^\\d\\w]", QRegularExpression::CaseInsensitiveOption );
    text.replace( re, "_" );
    return text;
}

/**
 * @brief Returns a map of parsed markdown urls with their link texts as key
 *
 * @param text
 * @return parsed urls
 */
QMap<QString, QString> Note::parseMarkdownUrlsFromText( QString text )
{
    QMap<QString, QString> urlMap;

    // match urls like this: [this url](http://mylink)
    QRegularExpression re("(\\[.*?\\]\\((.+?://.+?)\\))");
    QRegularExpressionMatchIterator i = re.globalMatch( text );
    while ( i.hasNext() )
    {
        QRegularExpressionMatch match = i.next();
        QString linkText = match.captured(1);
        QString url = match.captured(2);
        urlMap[linkText] = url;
    }

    // match urls like this: <http://mylink>
    re = QRegularExpression("(<(.+?://.+?)>)");
    i = re.globalMatch( text );
    while ( i.hasNext() )
    {
        QRegularExpressionMatch match = i.next();
        QString linkText = match.captured(1);
        QString url = match.captured(2);
        urlMap[linkText] = url;
    }

    return urlMap;
}

/**
 * @brief Returns the markdown url at position
 * @param text
 * @param position
 * @return url string
 */
QString Note::getMarkdownUrlAtPosition( QString text, int position )
{
    // get a map of parsed markdown urls with their link texts as key
    QMap<QString, QString> urlMap = parseMarkdownUrlsFromText( text );

    QMapIterator<QString, QString> i( urlMap );
    while ( i.hasNext() )
    {
        i.next();
        QString linkText = i.key();
        QString url = i.value();

        int foundPositionStart = text.indexOf( linkText );

        if ( foundPositionStart >= 0 )
        {
            // calculate end position of found linkText
            int foundPositionEnd = foundPositionStart + linkText.size();

            // check if position is in found string range
            if ( ( position >= foundPositionStart ) && ( position <= foundPositionEnd ) )
            {
                return url;
            }
        }
    }

    return "";
}

QDebug operator<<(QDebug dbg, const Note &note)
{
    dbg.nospace() << "Note: <id>" << note.id << " <name>" << note.name << " <fileName>" << note.fileName << " <hasDityData>" << note.hasDirtyData;
    return dbg.space();
}
