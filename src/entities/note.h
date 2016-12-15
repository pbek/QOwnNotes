#ifndef NOTE_H
#define NOTE_H

#include <QDateTime>
#include <QSqlQuery>
#include <QFile>
#include "notesubfolder.h"

#define NOTE_TEXT_ENCRYPTION_PRE_STRING "<!-- BEGIN ENCRYPTED TEXT --"
#define NOTE_TEXT_ENCRYPTION_POST_STRING "-- END ENCRYPTED TEXT -->"
#define BOTAN_SALT "Gj3%36/SmPoe12$snNAs-A-_.),?faQ1@!f32"

class Note {
public:
    explicit Note();

    int getId();

    QString getName();

    QString getFileName();

    QString getNoteText();

    bool getHasDirtyData();

    void setHasDirtyData(bool hasDirtyData);

    void setName(QString text);

    void setNoteText(QString text);

    qint64 getCryptoKey();

    QString getCryptoPassword();

    void setCryptoKey(qint64 cryptoKey);

    static bool addNote(QString name, QString fileName, QString text);

    static Note fetch(int id);

    static Note fetchByFileName(QString fileName, int noteSubFolderId = -1);

    static Note fetchByName(QString name, int noteSubFolderId = -1);

    static QList<Note> fetchAll(int limit = -1);

    static QList<Note> fetchAllNotTagged();

    static QStringList fetchAllNotTaggedNames();

    static int countAllNotTagged();

    static QList<Note> search(QString text);

    static QList<QString> searchAsNameList(QString text,
                                           bool searchInNameOnly = false);

    static QStringList fetchNoteNames();

    static QStringList fetchNoteFileNames();

    static Note noteFromQuery(QSqlQuery query);

    bool store();

    bool storeNewText(QString text);

    bool storeNoteTextFileToDisk();

    static QString defaultNoteFileExtension();

    static QStringList customNoteFileExtensionList(QString prefix = "");

    static QString getFullNoteFilePathForFile(QString fileName);

    static int storeDirtyNotesToDisk(Note &currentNote,
                                     bool *currentNoteChanged = Q_NULLPTR,
                                     bool *noteWasRenamed = Q_NULLPTR);

    bool updateNoteTextFromDisk();

    friend QDebug operator<<(QDebug dbg, const Note &note);

    void createFromFile(QFile &file, int noteSubFolderId = 0);

    static bool deleteAll();

    bool fileExists();

    bool exists();

    bool refetch();

    bool fillFromQuery(QSqlQuery query);

    bool fillByFileName(QString fileName, int noteSubFolderId = -1);

    bool removeNoteFile();

    bool remove(bool withFile = false);

    QString toMarkdownHtml(QString notesPath, int maxImageWidth = 980,
                           bool forExport = false, bool decrypt = true);

    bool isFetched();

    bool copy(QString destinationPath);

    bool move(QString destinationPath);

    static QString generateTextForLink(QString text);

    static qint64 qint64Hash(const QString &str);

    QString encryptNoteText();

    QString getDecryptedNoteText();

    bool hasEncryptedNoteText();

    void setCryptoPassword(QString password);

    bool canDecryptNoteText();

    static bool expireCryptoKeys();

    QUrl fullNoteFileUrl();

    QString fullNoteFilePath();

    static QString encodeCssFont(const QFont& refFont);

    void setDecryptedNoteText(QString text);

    bool storeNewDecryptedText(QString text);

    QDateTime getFileLastModified();

    QDateTime getModified();

    static int countAll();

    static bool allowDifferentFileName();

    bool renameNoteFile(QString newName);

    QString fileNameSuffix();

    bool modifyNoteTextFileNameFromQMLHook();

    static QList<int> searchInNotes(QString query,
                                    bool ignoreNoteSubFolder = false,
                                    int noteSubFolderId = -1);

    static QStringList buildQueryStringList(QString searchString);

    QString fileBaseName(bool withFullName = false);

    NoteSubFolder getNoteSubFolder();

    void setNoteSubFolder(NoteSubFolder noteSubFolder);

    void setNoteSubFolderId(int id);

    static QList<Note> fetchAllByNoteSubFolderId(int noteSubFolderId);

    static QList<int> noteIdListFromNoteList(QList<Note> noteList);

    static int countByNoteSubFolderId(int noteSubFolderId = 0);

    int getNoteSubFolderId();

    QString relativeNoteFilePath(QString separator = "");

    bool isSameFile(Note note);

    QString getShareUrl();

    void setShareUrl(QString url);

    int getShareId();

    void setShareId(int id);

    bool isShared();

    static Note fetchByShareId(int shareId);

    qint64 getFileSize();

    static Note updateOrCreateFromFile(QFile &file,
                                       NoteSubFolder noteSubFolder);

    static QList<int> fetchAllIds();

    static QList<int> findLinkedNotes(QString fileName);

    static void handleNoteRenaming(QString oldFileName, QString newFileName);

    static QString createNoteHeader(QString name);

    static QString getInsertMediaMarkdown(QFile *file, bool addNewLine = true);

    static bool scaleDownImageFileIfNeeded(QFile &file);

protected:
    int id;
    QString name;
    QString fileName;
    qint64 fileSize;
    int noteSubFolderId;
    QString noteText;
    QString decryptedNoteText;
    bool hasDirtyData;
    QDateTime fileCreated;
    QDateTime fileLastModified;
    QDateTime created;
    QDateTime modified;
    qint64 cryptoKey;
    QString cryptoPassword;
    QString shareUrl;
    int shareId;
    void handleNoteTextFileName();
    QRegularExpression getEncryptedNoteTextRegularExpression();
    QString getEncryptedNoteText();
    static QString cleanupFileName(QString name);

signals:

public slots:

    static const QString getNoteURL(const QString &baseName);

    static const QString getNoteURLFromFileName(const QString &fileName);
};

#endif // NOTE_H
