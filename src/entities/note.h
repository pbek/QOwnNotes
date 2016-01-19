#ifndef NOTE_H
#define NOTE_H

#include <QDateTime>
#include <QSqlQuery>
#include <QFile>


#define NOTE_TEXT_ENCRYPTION_PRE_STRING "<!-- BEGIN ENCRYPTED TEXT --"
#define NOTE_TEXT_ENCRYPTION_POST_STRING "-- END ENCRYPTED TEXT -->"

class Note {
public:
    explicit Note();

    int getId();

    QString getName();

    QString getFileName();

    QString getNoteText();

    bool getHasDirtyData();

    void setName(QString text);

    void setNoteText(QString text);

    qint64 getCryptoKey();

    void setCryptoKey(qint64 cryptoKey);

    static bool addNote(QString name, QString fileName, QString text);

    static Note fetch(int id);

    static Note fetchByFileName(QString fileName);

    static Note fetchByName(QString name);

    static QList<Note> fetchAll();

    static QList<Note> search(QString text);

    static QList<QString> searchAsNameList(QString text, bool searchInName = false);

    static QStringList fetchNoteNames();

    static QStringList fetchNoteFileNames();

    static Note noteFromQuery(QSqlQuery query);

    bool store();

    bool storeNewText(QString text);

    bool storeNoteTextFileToDisk();

    static QString fullNoteFilePath(QString fileName);

    static int storeDirtyNotesToDisk(Note &currentNote);

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

    QString toMarkdownHtml(QString notesPath = "");

    bool isFetched();

    bool copy(QString destinationPath);

    bool move(QString destinationPath);

    static QString generateTextForLink(QString text);

    static qint64 qint64Hash(const QString &str);

    QString encryptNote(QString password);

    QString getDecryptedNoteText();

    bool hasEncryptedNoteText();

    void setCryptoPassword(QString password);

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
    qint64 cryptoKey;
    void handleNoteTextFileName();
    QRegularExpression getEncryptedNoteTextRegularExpression();
    QString getEncryptedNoteText();

signals:

public slots:
};

#endif // NOTE_H
