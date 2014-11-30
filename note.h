#ifndef DATASTORAGE_H
#define DATASTORAGE_H

#include <QDateTime>
#include <QSqlQuery>


class Note
{
public:
    explicit Note();
    static bool createConnection();
    static bool addNote( QString name, QString fileName, QString text );
    static Note fetch( int id );
    static Note fetchByFileName( QString fileName );
    static QList<Note> fetchAll();
    static QStringList fetchNoteNames();
    static QStringList fetchNoteFileNames();
    static Note noteFromQuery( QSqlQuery query );

    int getId();
    QString getName();
    QString getFileName();
    QString getNoteText();

private:
    int id;
    QString name;
    QString fileName;
    QString noteText;
    QDateTime created;
    QDateTime modified;

signals:

public slots:
};

#endif // DATASTORAGE_H
