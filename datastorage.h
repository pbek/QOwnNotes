#ifndef DATASTORAGE_H
#define DATASTORAGE_H

#include <QObject>

class DataStorage : public QObject
{
    Q_OBJECT
public:
    explicit DataStorage(QObject *parent = 0);
    static bool createConnection();
    static bool addNote( QString name, QString fileName, QString text );
    static void fetchNotes();
    static QStringList fetchNoteNames();
    static QStringList fetchNoteFileNames();

signals:

public slots:

};

#endif // DATASTORAGE_H
