#ifndef NOTE_H
#define NOTE_H

#include <QDateTime>
#include <QSqlQuery>
#include <QFile>
#include <QUrl>
#include <QRegularExpression>
#include "notesubfolder.h"
#include "bookmark.h"

#define NOTE_TEXT_ENCRYPTION_PRE_STRING "<!-- BEGIN ENCRYPTED TEXT --"
#define NOTE_TEXT_ENCRYPTION_POST_STRING "-- END ENCRYPTED TEXT -->"
#define BOTAN_SALT "Gj3%36/SmPoe12$snNAs-A-_.),?faQ1@!f32"

class Note {
public:
    explicit Note();

    int getId() const;

    QString getName() const;

    QString getFileName() const;

    QString getNoteText() const;

    bool getHasDirtyData() const;

    void setHasDirtyData(const bool hasDirtyData);

    void setName(QString text);

    void setNoteText(QString text);

    qint64 getCryptoKey() const;

    QString getCryptoPassword() const;

    void setCryptoKey(const qint64 cryptoKey);

    static bool addNote(const QString& name, const QString& fileName, const QString& text);

    static Note fetch(int id);

    static Note fetchByName(const QRegularExpression& regExp,
                            int noteSubFolderId = -1);

    static Note fetchByFileName(const QString &fileName, int noteSubFolderId = -1);

    static Note fetchByName(const QString& name, int noteSubFolderId = -1);

    static QList<Note> fetchAll(int limit = -1);

    static QList<Note> fetchAllNotTagged(int activeNoteSubFolderId);

    static QStringList fetchAllNotTaggedNames();

    static int countAllNotTagged(int activeNoteSubFolderId=-1);

    static QList<Note> search(const QString& text);

    static QList<QString> searchAsNameListInCurrentNoteSubFolder(
            const QString& text, bool searchInNameOnly = false);

    static QList<QString> searchAsNameList(
            const QString& text, bool searchInNameOnly = false);

    static QStringList fetchNoteNamesInCurrentNoteSubFolder();

    static QStringList fetchNoteNames();

    static QStringList fetchNoteFileNames();

    static Note noteFromQuery(const QSqlQuery& query);

    bool store();

    bool storeNewText(QString text);

    bool storeNoteTextFileToDisk();

    static QString defaultNoteFileExtension();

    static QStringList customNoteFileExtensionList(const QString& prefix = "");

    static QString getFullFilePathForFile(const QString& fileName);

    QString getFilePathRelativeToNote(const Note &note) const;

    static int storeDirtyNotesToDisk(Note &currentNote,
                                     bool *currentNoteChanged = Q_NULLPTR,
                                     bool *noteWasRenamed = Q_NULLPTR);

    bool updateNoteTextFromDisk();

    friend QDebug operator<<(QDebug dbg, const Note &note);

    void createFromFile(QFile &file, int noteSubFolderId = 0,
                        bool withNoteNameHook = false);

    static bool deleteAll();

    bool fileExists();

    bool fileWriteable();

    bool exists();

    bool refetch();

    bool fillFromQuery(const QSqlQuery& query);

    bool fillByFileName(const QString &fileName, int noteSubFolderId = -1);

    bool removeNoteFile();

    bool remove(bool withFile = false);

    QString toMarkdownHtml(QString notesPath, int maxImageWidth = 980,
                           bool forExport = false, bool decrypt = true,
                           bool base64Images = false);

    bool isFetched();

    bool copyToPath(const QString& destinationPath, QString noteFolderPath = "");

    bool moveToPath(const QString &destinationPath, const QString &noteFolderPath = "");

    static QString generateTextForLink(QString text);

    static qint64 qint64Hash(const QString &str);

    QString encryptNoteText();

    QString getDecryptedNoteText() const;

    bool hasEncryptedNoteText();

    void setCryptoPassword(const QString& password);

    bool canDecryptNoteText();

    static bool expireCryptoKeys();

    QUrl fullNoteFileUrl() const;

    QString fullNoteFilePath() const;

    QString fullNoteFileDirPath() const;

    static QString encodeCssFont(const QFont& refFont);

    void setDecryptedNoteText(QString text);

    bool storeNewDecryptedText(QString text);

    void setDecryptedText(QString text);

    QDateTime getFileLastModified() const;

    QDateTime getFileCreated() const;

    QDateTime getModified() const;

    static int countAll();

    static bool allowDifferentFileName();

    bool renameNoteFile(QString newName);

    QString fileNameSuffix();

    bool modifyNoteTextFileNameFromQMLHook();

    static QList<int> searchInNotes(QString query,
                                    bool ignoreNoteSubFolder = false,
                                    int noteSubFolderId = -1);

    int countSearchTextInNote(QString &search);

    static QStringList buildQueryStringList(
            QString searchString, bool escapeForRegularExpression = false);

    QString fileBaseName(bool withFullName = false);

    NoteSubFolder getNoteSubFolder() const;

    void setNoteSubFolder(const NoteSubFolder &noteSubFolder);

    void setNoteSubFolderId(int id);

    static QList<Note> fetchAllByNoteSubFolderId(int noteSubFolderId);

    static QList<int> noteIdListFromNoteList(const QList<Note>& noteList);

    static int countByNoteSubFolderId(int noteSubFolderId = 0, bool recursive = false);

    int getNoteSubFolderId() const;

    bool isInCurrentNoteSubFolder() const;

    QString relativeNoteFilePath(QString separator = "") const;

    QString relativeNoteSubFolderPath() const;

    QString noteSubFolderPathData() const;

    bool isSameFile(const Note &note);

    QString getShareUrl() const;

    void setShareUrl(QString url);

    int getShareId() const;

    void setShareId(int id);

    unsigned int getSharePermissions() const;

    bool isShareEditAllowed() const;

    void setSharePermissions(unsigned int permissions);

    bool isShared() const;

    static Note fetchByShareId(int shareId);

    qint64 getFileSize() const;

    static Note updateOrCreateFromFile(QFile &file, NoteSubFolder noteSubFolder,
                                           bool withNoteNameHook = false);

    static QList<int> fetchAllIds(int limit = -1, int offset = -1);

    QList<int> findLinkedNoteIds() const;

    void handleNoteMoving(const Note &oldNote);

    static QString createNoteHeader(const QString& name);

    QString getInsertMediaMarkdown(QFile *file, bool addNewLine = true, bool returnUrlOnly = false, QString title = "");

    QString getInsertAttachmentMarkdown(QFile *file, QString fileName = "", bool returnUrlOnly= false);

    static bool scaleDownImageFileIfNeeded(QFile &file);

    QString downloadUrlToMedia(const QUrl& url, bool returnUrlOnly = false);

    QString importMediaFromBase64(QString &data, const QString& imageSuffix = "dat");

    bool canWriteToNoteFile();

    static QString generateNoteFileNameFromName(const QString& name);

    void generateFileNameFromName();

    QString textToMarkdownHtml(QString str, const QString& notesPath,
                               int maxImageWidth = 980, bool forExport = false,
                               bool base64Images = false);

    QStringList getMediaFileList();

    static Note fetchByUrlString(const QString& urlString);

    static QList<int> fetchAllIdsByNoteTextPart(const QString& textPart);

    QStringList getAttachmentsFileList() const;

    QString getNotePreviewText(bool asHtml = false, int lines = 3) const;

    static QString generateMultipleNotesPreviewText(const QList<Note> &notes);

    bool handleNoteTextFileName();

    QString getNoteIdURL() const;

    static QString cleanupFileName(QString name);

    static QString extendedCleanupFileName(QString name);

    QList<Bookmark> getParsedBookmarks() const;

    QString getParsedBookmarksWebServiceJsonText() const;

    void resetNoteTextHtmlConversionHash();

    QString getFileURLFromFileName(QString fileName) const;

    static bool fileUrlIsNoteInCurrentNoteFolder(const QUrl &url);

    static QString fileUrlInCurrentNoteFolderToRelativePath(const QUrl &url);

    QString relativeFilePath(const QString &path) const;

    static Note fetchByFileUrl(const QUrl& url);

    static Note fetchByRelativeFilePath(const QString& relativePath);

    QString getNoteUrlForLinkingTo(const Note &note) const;

    QString mediaUrlStringForFileName(const QString &fileName) const;

    QString attachmentUrlStringForFileName(const QString &fileName) const;

    Note fetchByRelativeFileName(const QString &fileName) const;

    bool updateRelativeMediaFileLinks();

    bool updateRelativeAttachmentFileLinks();

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
    unsigned int sharePermissions;
    QString _noteTextHtml;
    QString _noteTextHtmlConversionHash;

    QRegularExpression getEncryptedNoteTextRegularExpression() const;
    QString getEncryptedNoteText() const;
signals:

public slots:

    static const QString getNoteURL(const QString &baseName);

    static const QString getNoteURLFromFileName(const QString &fileName);
};

#endif // NOTE_H
