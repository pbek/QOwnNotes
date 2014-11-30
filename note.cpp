#include "note.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include <QApplication>



Note::Note()
{
    this->name = name;
    this->fileName = fileName;
    this->noteText = noteText;
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

bool Note::createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    if (!db.open())
    {
        QMessageBox::critical(0, qApp->tr("Cannot open database"),
            qApp->tr("Unable to establish a database connection.\n"
                      "This example needs SQLite support. Please read "
                      "the Qt SQL driver documentation for information how "
                      "to build it.\n\n"
                      "Click Cancel to exit."), QMessageBox::Cancel);
        return false;
    }

    QSqlQuery query;
    query.exec("create table note (id integer primary key, "
            "name varchar(255), file_name varchar(255), note_text text,"
            "created datetime default current_timestamp,"
            "modified datetime default current_timestamp)");
//    qDebug() << query.exec("insert into note (id, name) VALUES (NULL, 'test')");
//    qDebug() << query.exec("insert into note (name) VALUES ('test2')");
//    qDebug() << query.exec("insert into note (name) VALUES ('test3')");

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
    query.prepare( "SELECT * FROM note WHERE id = :id" );
    query.bindValue( ":id", id );
    qDebug() << query.exec();
    query.first();

    Note note = noteFromQuery( query );
    return note;
}

Note Note::fetchByFileName( QString fileName )
{
    QSqlQuery query;
    query.prepare( "SELECT * FROM note WHERE file_name = :file_name" );
    query.bindValue( ":file_name", fileName );
    qDebug() << query.exec();
    query.first();

    Note note = noteFromQuery( query );
    return note;
}

Note Note::noteFromQuery( QSqlQuery query )
{
    int id = query.value("id").toInt();
    QString name = query.value("name").toString();
    QString fileName = query.value("file_name").toString();
    QString noteText = query.value("note_text").toString();

    Note note;
    note.id = id;
    note.name = name;
    note.fileName = fileName;
    note.noteText = noteText;

    return note;
}

QList<Note> Note::fetchAll()
{
    QSqlQuery query;
    QList<Note> noteList;

    int id;
    QString name;
    QString fileName;
    QString noteText;

    query.prepare( "SELECT * FROM note" );
    if( !query.exec() )
    {
//        qDebug() << query.lastError().text();
        qDebug() << "error executing query";
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

QStringList Note::fetchNoteNames()
{
    QSqlQuery query;
    QStringList list;

    query.prepare( "SELECT * FROM note" );
    if( !query.exec() )
    {
//        qDebug() << query.lastError().text();
        qDebug() << "error fetching notes";
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

    query.prepare( "SELECT * FROM note" );
    if( !query.exec() )
    {
//        qDebug() << query.lastError().text();
        qDebug() << "error fetching notes";
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

