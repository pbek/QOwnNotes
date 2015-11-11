#ifndef NOTE_H
#define NOTE_H

#include <QDateTime>
#include <QSqlQuery>
#include <QFile>


class Note
{
public:
    explicit Note();

    int getId();
    QString getName();
    QString getFileName();
    QString getNoteText();
    bool getHasDirtyData();
    void setName( QString text );
    void setNoteText(QString text);

    static bool createConnection();
    static bool addNote( QString name, QString fileName, QString text );
    static Note fetch( int id );
    static Note fetchByFileName( QString fileName );
    static Note fetchByName(QString name);
    static QList<Note> fetchAll();
    static QList<Note> search(QString text);
    static QList<QString> searchAsNameList(QString text, bool searchInName = false);
    static QStringList fetchNoteNames();
    static QStringList fetchNoteFileNames();
    static Note noteFromQuery( QSqlQuery query );
    bool store();
    bool storeNewText(QString text);
    bool storeNoteTextFileToDisk();
    static QString fullNoteFilePath(QString fileName);
    static int storeDirtyNotesToDisk( Note &currentNote );
    bool updateNoteTextFromDisk();
    friend QDebug operator<<(QDebug dbg, const Note &note);
    void createFromFile(QFile &file);
    static bool deleteAll();
    bool fileExists();
    bool exists();
    bool refetch();
    bool fillFromQuery(QSqlQuery query);
    bool fillByFileName(QString fileName);
    bool removeNoteFile();
    bool remove(bool withFile);
    QString toMarkdownHtml();
    bool isFetched();
    bool copy(QString destinationPath);
    bool move(QString destinationPath);
    static QString generateTextForLink(QString text);
    static QMap<QString, QString> parseMarkdownUrlsFromText(QString text);
    static QString getMarkdownUrlAtPosition(QString text, int position);
private:
    int id;
    QString name;
    QString fileName;
    QString noteText;
    bool hasDirtyData;
    QDateTime fileCreated;
    QDateTime fileLastModified;
    QDateTime created;
    QDateTime modified;
    void handleNoteTextFileName();
signals:

public slots:
};

#endif // NOTE_H
