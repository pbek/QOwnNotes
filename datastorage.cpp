#include "datastorage.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include <QApplication>



DataStorage::DataStorage(QObject *parent) :
    QObject(parent)
{
}

bool DataStorage::createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    if (!db.open()) {
        QMessageBox::critical(0, qApp->tr("Cannot open database"),
            qApp->tr("Unable to establish a database connection.\n"
                      "This example needs SQLite support. Please read "
                      "the Qt SQL driver documentation for information how "
                      "to build it.\n\n"
                      "Click Cancel to exit."), QMessageBox::Cancel);
        return false;
    }

    QSqlQuery query;
    query.exec("create table note (id int primary key, "
            "name varchar(255), filename varchar(255), note_text text,"
            "created datetime default current_timestamp,"
            "modified datetime default current_timestamp)");
//    qDebug() << query.exec("insert into note (id, name) VALUES (NULL, 'test')");
//    qDebug() << query.exec("insert into note (name) VALUES ('test2')");
//    qDebug() << query.exec("insert into note (name) VALUES ('test3')");

    return true;
}

bool DataStorage::addNote( QString name, QString fileName, QString text )
{
    QSqlQuery query;
    query.prepare( "INSERT INTO note ( name, filename, note_text ) VALUES ( :name, :filename, :note_text )" );
    query.bindValue( ":name", name );
    query.bindValue( ":filename", fileName );
    query.bindValue( ":note_text", text );
    return query.exec();
}

void DataStorage::fetchNotes()
{
    QSqlQuery query;

    query.prepare( "SELECT * FROM note" );
    if( !query.exec() )
    {
//        qDebug() << query.lastError().text();
        qDebug() << "error executing query";
    }
    else
    {
        qDebug( "Selected!" );

//        query.first();
//        qDebug() << query.value("name").toString();

        QSqlRecord rec = query.record();


        int cols = rec.count();
        qDebug() << cols;

        for( int c=0; c<cols; c++ )
            qDebug() << QString( "Column %1: %2" ).arg( c ).arg( rec.fieldName(c) );

        for( int r=0; query.next(); r++ )
            for( int c=0; c<cols; c++ )
                qDebug() << QString( "Row %1, %2: %3" ).arg( r ).arg( rec.fieldName(c) ).arg( query.value(c).toString() );
    }
}

QStringList DataStorage::fetchNoteNames()
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

QStringList DataStorage::fetchNoteFileNames()
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
            list.append( query.value("filename").toString() );
        }
    }

    return list;
}
