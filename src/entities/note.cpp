#include "entities/note.h"

#include <botanwrapper.h>
#include <services/owncloudservice.h>
#include <services/scriptingservice.h>
#include <utils/gui.h>
#include <utils/misc.h>
#include <utils/schema.h>

#include <QApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QMimeDatabase>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QSqlError>
#include <QSqlRecord>
#include <QTemporaryFile>
#include <QUrl>
#include <utility>

#include "api/noteapi.h"
#include "cloudconnection.h"
#include "dialogs/textdiffdialog.h"
#include "entities/bookmark.h"
#include "entities/commandsnippet.h"
#include "helpers/codetohtmlconverter.h"
#include "libraries/md4c/src/md4c-html.h"
#include "libraries/md4c/src/md4c.h"
#include "libraries/simplecrypt/simplecrypt.h"
#include "notefolder.h"
#include "notesubfolder.h"
#include "services/settingsservice.h"
#include "tag.h"
#include "trashitem.h"

#ifdef Q_OS_WIN32
// for restoreCreatedDate()
#include <windows.h>
#endif

Note::Note()
    : _id{0},
      _noteSubFolderId{0},
      _cryptoKey{0},
      _fileSize{0},
      _shareId{0},
      _sharePermissions{0},
      _hasDirtyData{false} {}

int Note::getId() const { return this->_id; }

QString Note::getName() const { return this->_name; }

QDateTime Note::getFileLastModified() const { return this->_fileLastModified; }

QDateTime Note::getFileCreated() const { return this->_fileCreated; }

QDateTime Note::getModified() const { return this->_modified; }

qint64 Note::getCryptoKey() const { return this->_cryptoKey; }

QString Note::getCryptoPassword() const { return this->_cryptoPassword; }

QString Note::getFileChecksum() const { return this->_fileChecksum; }

void Note::setFileChecksum(const QString &checksum) { this->_fileChecksum = checksum; }

QString Note::getShareUrl() const { return this->_shareUrl; }

int Note::getShareId() const { return this->_shareId; }

unsigned int Note::getSharePermissions() const { return this->_sharePermissions; }

/**
 * Check 2nd bit for edit permissions
 *
 * @return
 */
bool Note::isShareEditAllowed() const { return _sharePermissions & 2; }

int Note::getFileSize() const { return this->_fileSize; }

bool Note::isShared() const { return this->_shareId > 0; }

QString Note::getFileName() const { return this->_fileName; }

NoteSubFolder Note::getNoteSubFolder(const QString &connectionName) const {
    return NoteSubFolder::fetch(this->_noteSubFolderId, connectionName);
}

int Note::getNoteSubFolderId() const { return this->_noteSubFolderId; }

bool Note::isInCurrentNoteSubFolder() const {
    const int currentNoteSubFolderId = NoteSubFolder::activeNoteSubFolderId();

    // beware: the special "All notes" note subfolder also uses the id 0
    if (currentNoteSubFolderId < 0) {
        return true;
    }

    return this->_noteSubFolderId == currentNoteSubFolderId;
}

void Note::setNoteSubFolder(const NoteSubFolder &noteSubFolder) {
    setNoteSubFolderId(noteSubFolder.getId());
}

void Note::setNoteSubFolderId(int id) { this->_noteSubFolderId = id; }

QString Note::getNoteText() const { return this->_noteText; }

void Note::setHasDirtyData(const bool hasDirtyData) { this->_hasDirtyData = hasDirtyData; }

bool Note::getHasDirtyData() const { return this->_hasDirtyData; }

void Note::setName(QString text) { this->_name = std::move(text); }

void Note::setShareUrl(QString url) { this->_shareUrl = std::move(url); }

void Note::setShareId(int id) { this->_shareId = id; }

void Note::setSharePermissions(unsigned int permissions) { this->_sharePermissions = permissions; }

/**
 * Gets the Nextcloud file link for this note
 *
 * @return the Nextcloud file link URL, empty if not available
 */
QString Note::getNextcloudFileLink() const {
    if (!OwnCloudService::isOwnCloudSupportEnabled()) {
        return {};
    }

    // Get the OwnCloud service instance
    OwnCloudService *ownCloudService = OwnCloudService::instance();
    if (ownCloudService == nullptr) {
        return {};
    }

    // Fetch the file ID from Nextcloud
    QString fileId = ownCloudService->fetchNoteFileId(*this);
    if (fileId.isEmpty()) {
        return {};
    }

    // Get the server URL and construct the file link
    auto cloudConnection = CloudConnection::currentCloudConnection();
    QString serverUrl = cloudConnection.getServerUrlWithoutPath();

    if (serverUrl.isEmpty()) {
        return {};
    }

    // Get the directory path for the "dir" parameter
    QString noteRelativePath = relativeNoteFilePath(QStringLiteral("/"));
    QString notesPath = NoteFolder::currentRemotePath();
    QString fullPath = notesPath + QStringLiteral("/") + noteRelativePath;

    // Extract the directory part
    QFileInfo fileInfo(fullPath);
    QString dirPath = fileInfo.dir().path();

    // Construct the Nextcloud file link
    // Format: https://server/apps/files/files/FILEID?dir=/path&opendetails=true
    const QString fileLink = serverUrl + QStringLiteral("/apps/files/files/") + fileId +
                             QStringLiteral("?dir=") + QUrl::toPercentEncoding(dirPath) +
                             QStringLiteral("&opendetails=true");

    return fileLink;
}

/**
 * Gets the Nextcloud Notes link for this note
 *
 * @return the Nextcloud Notes link URL, empty if not available
 */
QString Note::getNextcloudNotesLink() const {
    if (!OwnCloudService::isOwnCloudSupportEnabled()) {
        return {};
    }

    // Get the OwnCloud service instance
    OwnCloudService *ownCloudService = OwnCloudService::instance();
    if (ownCloudService == nullptr) {
        return {};
    }

    // Fetch the file ID from Nextcloud
    QString fileId = ownCloudService->fetchNoteFileId(*this);
    if (fileId.isEmpty()) {
        return {};
    }

    // Get the server URL and construct the file link
    auto cloudConnection = CloudConnection::currentCloudConnection();
    QString serverUrl = cloudConnection.getServerUrlWithoutPath();

    if (serverUrl.isEmpty()) {
        return {};
    }

    // Construct the Nextcloud Notes link
    // Format: https://server/apps/notes/note/FILEID
    const QString link = serverUrl + QStringLiteral("/apps/notes/note/") + fileId;

    return link;
}

void Note::setCryptoKey(const qint64 cryptoKey) { this->_cryptoKey = cryptoKey; }

void Note::setNoteText(QString text) { this->_noteText = std::move(text); }

void Note::setDecryptedNoteText(QString text) { this->_decryptedNoteText = std::move(text); }

bool Note::addNote(const QString &name, const QString &fileName, const QString &text) {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    query.prepare(
        QStringLiteral("INSERT INTO note ( name, file_name, note_text ) "
                       "VALUES ( :name, :file_name, :note_text )"));
    query.bindValue(QStringLiteral(":name"), name);
    query.bindValue(QStringLiteral(":file_name"), fileName);
    query.bindValue(QStringLiteral(":note_text"), text);
    return query.exec();
}

Note Note::fetch(int id) {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    query.prepare(QStringLiteral("SELECT * FROM note WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        if (query.first()) {
            return noteFromQuery(query);
        }
    }

    return Note();
}

/**
 * Fetches a note by note name with a regular expression
 *
 * @param regExp
 * @param noteSubFolderId if not set all notes will be searched
 * @return
 */
Note Note::fetchByName(const QRegularExpression &regExp, int noteSubFolderId) {
    const QVector<Note> noteList =
        noteSubFolderId == -1 ? fetchAll() : fetchAllByNoteSubFolderId(noteSubFolderId);

    // since there is no regular expression search in Qt's sqlite
    // implementation we have to iterate
    for (const Note &note : noteList) {
        const bool match = regExp.match(note.getName()).hasMatch();
        if (match) {
            return note;
        }
    }

    return Note();
}

Note Note::fetchByFileName(const QString &fileName, int noteSubFolderId) {
    Note note;
    // get the active note subfolder id if none was set
    if (noteSubFolderId == -1) {
        noteSubFolderId = NoteSubFolder::activeNoteSubFolderId();
    }

    note.fillByFileName(fileName, noteSubFolderId);
    return note;
}

Note Note::fetchByFileName(const QString &fileName, const QString &noteSubFolderPathData) {
    auto noteSubFolder = NoteSubFolder::fetchByPathData(noteSubFolderPathData, QStringLiteral("/"));
    return fetchByFileName(fileName, noteSubFolder.getId());
}

bool Note::fillByFileName(const QString &fileName, int noteSubFolderId) {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    // get the active note subfolder id if none was set
    if (noteSubFolderId == -1) {
        noteSubFolderId = NoteSubFolder::activeNoteSubFolderId();
    }

    query.prepare(
        QStringLiteral("SELECT * FROM note WHERE file_name = :file_name AND "
                       "note_sub_folder_id = :note_sub_folder_id"));
    query.bindValue(QStringLiteral(":file_name"), fileName);
    query.bindValue(QStringLiteral(":note_sub_folder_id"), noteSubFolderId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        if (query.first()) {
            this->fillFromQuery(query);
            return true;
        }
    }

    return false;
}

/**
 * Fetches a note by its file path relative to the note folder it is in
 *
 * @brief Note::fetchByRelativeFilePath
 * @param relativePath
 * @return
 */
Note Note::fetchByRelativeFilePath(const QString &relativePath) {
    const QFileInfo fileInfo(relativePath);

    // load note subfolder and note from the relative path
    // be aware that there must not be a ".." in the path, a canonical path must
    // be presented!
    const auto noteSubFolder = NoteSubFolder::fetchByPathData(fileInfo.path(), QStringLiteral("/"));

    if ((fileInfo.path() != ".") && !noteSubFolder.isFetched()) {
        return Note();
    }

    return Note::fetchByFileName(fileInfo.fileName(), noteSubFolder.getId());
}

/**
 * Fetches a note by its full file url
 *
 * @brief Note::fetchByFileUrl
 * @param url
 * @return
 */
Note Note::fetchByFileUrl(const QUrl &url) {
    const QString &relativePath = Note::fileUrlInCurrentNoteFolderToRelativePath(url);

    const Note note = Note::fetchByRelativeFilePath(relativePath);
    return note;
}

bool Note::remove(bool withFile) {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    query.prepare(QStringLiteral("DELETE FROM note WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), this->_id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else {
        if (withFile) {
            this->removeNoteFile();

            // remove all links to tags
            Tag::removeAllLinksToNote(*this);
        }

        return true;
    }
}

/**
 * @brief Copies a note to another path
 *
 * @param destinationPath
 * @return bool
 */
bool Note::copyToPath(const QString &destinationPath, QString noteFolderPath) {
    QDir d;
    if (this->fileExists() && (d.exists(destinationPath))) {
        QFile file(fullNoteFilePath());
        QString destinationFileName = destinationPath + QDir::separator() + this->_fileName;

        if (d.exists(destinationFileName)) {
            qDebug() << destinationFileName << "already exists!";

            // find a new filename for the note
            const QDateTime currentDateTime = QDateTime::currentDateTime();
            destinationFileName =
                destinationPath + QDir::separator() + this->_name + QChar(' ') +
                currentDateTime.toString(Qt::ISODate).replace(QChar(':'), QChar('_')) + QChar('.') +
                defaultNoteFileExtension();

            qDebug() << "New file name:" << destinationFileName;
        }

        // copy the note file to the destination
        const bool isFileCopied = file.copy(destinationFileName);
        if (!isFileCopied) {
            return false;
        }

        // check if there are media files in the note
        const QStringList mediaFileList = getMediaFileList();
        if (mediaFileList.empty()) {
            return true;
        }

        if (noteFolderPath.isEmpty()) {
            noteFolderPath = destinationPath;
        }

        if (!NoteFolder::isPathNoteFolder(noteFolderPath)) {
            return true;
        }

        const QDir mediaDir(noteFolderPath + QDir::separator() + QStringLiteral("media"));

        // created the media folder if it doesn't exist
        if (!mediaDir.exists()) {
            if (!mediaDir.mkpath(mediaDir.path())) {
                return true;
            }
        }

        // copy all images to the media folder inside
        // destinationPath
        for (const QString &fileName : mediaFileList) {
            QFile mediaFile(NoteFolder::currentMediaPath() + QDir::separator() + fileName);

            if (mediaFile.exists()) {
                mediaFile.copy(mediaDir.path() + QDir::separator() + fileName);
            }
        }
    }

    return false;
}

/**
 * Exports a note to destinationPath as Markdown file
 *
 * @param destinationPath of the note file (with note name and extension)
 * @param withAttachedFiles if true media files and attachments will be exported too
 * @return
 */
bool Note::exportToPath(const QString &destinationPath, bool withAttachedFiles) {
    auto noteText = getNoteText();
    QFile file(destinationPath);
    QFileInfo fileInfo(destinationPath);
    auto absolutePath = fileInfo.absolutePath();

    if (withAttachedFiles) {
        // check if there are media files in the note
        const QStringList mediaFileList = getMediaFileList();

        if (!mediaFileList.empty()) {
            qDebug() << __func__ << " - 'mediaFileList': " << mediaFileList;

            // copy all images to the destination folder
            for (const QString &fileName : mediaFileList) {
                QFile mediaFile(NoteFolder::currentMediaPath() + QDir::separator() + fileName);

                if (mediaFile.exists()) {
                    mediaFile.copy(absolutePath + QDir::separator() + fileName);
                }
            }

            static const QRegularExpression re(QStringLiteral(R"((!\[.*?\])\(.*media/(.+?)\))"));
            QRegularExpressionMatchIterator i = re.globalMatch(noteText);

            // remove the "media/" part from the file names in the note text
            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                const QString wholeLinkText = match.captured(0);
                const QString titlePart = match.captured(1);
                const QString fileName = match.captured(2);

                noteText.replace(wholeLinkText,
                                 titlePart + QStringLiteral("(./") + fileName + QChar(')'));
            }
        }

        // check if there are attachment files in the note
        const QStringList attachmentFileList = getAttachmentsFileList();

        if (!attachmentFileList.empty()) {
            qDebug() << __func__ << " - 'attachmentFileList': " << attachmentFileList;

            // copy all attachment to the destination folder
            for (const QString &fileName : attachmentFileList) {
                QFile attachmentFile(NoteFolder::currentAttachmentsPath() + QDir::separator() +
                                     fileName);

                if (attachmentFile.exists()) {
                    attachmentFile.copy(absolutePath + QDir::separator() + fileName);
                }
            }

            static const QRegularExpression re(
                QStringLiteral(R"((\[.*?\])\(.*attachments/(.+?)\))"));
            QRegularExpressionMatchIterator i = re.globalMatch(noteText);

            // remove the "attachments/" part from the file names in the note text
            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                const QString wholeLinkText = match.captured(0);
                const QString titlePart = match.captured(1);
                const QString fileName = match.captured(2);

                noteText.replace(wholeLinkText,
                                 titlePart + QStringLiteral("(./") + fileName + QChar(')'));
            }
        }
    }

    qDebug() << "exporting note file: " << destinationPath;

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCritical() << file.errorString();

        return false;
    }

    QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");
#endif
    out << noteText;

    file.flush();
    file.close();
    Utils::Misc::openFolderSelect(destinationPath);

    return true;
}

/**
 * @brief Moves a note to another path
 *
 * @param destinationPath
 * @return bool
 */
bool Note::moveToPath(const QString &destinationPath, const QString &noteFolderPath) {
    const bool result = copyToPath(destinationPath, noteFolderPath);
    if (result) {
        return remove(true);
    }

    return false;
}

/**
 * Returns a list of all linked image files of the media folder of the current
 * note
 * @return
 */
QStringList Note::getMediaFileList() const {
    QStringList fileList;

    // match image links like ![media-qV920](file://media/608766373.gif)
    // or  ![media-qV920](media/608766373.gif)
    static const QRegularExpression re(QStringLiteral(R"(!\[.*?\]\(.*media/(.+?)\))"));
    QRegularExpressionMatchIterator i = re.globalMatch(_noteText);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        const QString fileName = match.captured(1);
        // We want to decode the file name parsed from the note, because it can be encoded
        // For example the image dialog will automatically encode the file name
        fileList << QUrl::fromPercentEncoding(fileName.toUtf8());
    }

    return fileList;
}

bool Note::hasMediaFiles() { return !getMediaFileList().empty(); }

bool Note::updateRelativeMediaFileLinks() {
    static const QRegularExpression re(QStringLiteral(R"((!\[.*?\])\((.*media/(.+?))\))"));
    QRegularExpressionMatchIterator i = re.globalMatch(_noteText);
    bool textWasUpdated = false;
    QString newText = getNoteText();

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString filePath = match.captured(2);

        if (filePath.startsWith(QLatin1String("file://"))) {
            continue;
        }

        const QString wholeLinkText = match.captured(0);
        const QString titlePart = match.captured(1);
        const QString fileName = match.captured(3);

        filePath = mediaUrlStringForFileName(fileName);
        newText.replace(wholeLinkText, titlePart + QChar('(') + filePath + QChar(')'));
        textWasUpdated = true;
    }

    if (textWasUpdated) {
        storeNewText(std::move(newText));
    }

    return textWasUpdated;
}

/**
 * Returns a list of all linked attachments of the current note
 * @return
 */
QStringList Note::getAttachmentsFileList() const {
    const QString text = getNoteText();
    QStringList fileList;

    // match attachment links like [956321614](file://attachments/956321614.pdf)
    // or [956321614](attachments/956321614.pdf)
    static const QRegularExpression re(QStringLiteral(R"(\[.*?\]\(.*attachments/(.+?)\))"));
    QRegularExpressionMatchIterator i = re.globalMatch(text);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        const QString fileName = match.captured(1);

        // We want to decode the file name parsed from the note, because it can be encoded
        // For example the attachment dialog will automatically encode the file name
        fileList << QUrl::fromPercentEncoding(fileName.toUtf8());
    }

    return fileList;
}

bool Note::hasAttachments() { return !getAttachmentsFileList().empty(); }

bool Note::updateRelativeAttachmentFileLinks() {
    static const QRegularExpression re(QStringLiteral(R"((\[.*?\])\((.*attachments/(.+?))\))"));
    QRegularExpressionMatchIterator i = re.globalMatch(_noteText);
    bool textWasUpdated = false;
    QString newText = getNoteText();

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString filePath = match.captured(2);

        if (filePath.startsWith(QLatin1String("file://"))) {
            continue;
        }

        const QString wholeLinkText = match.captured(0);
        const QString titlePart = match.captured(1);
        const QString fileName = match.captured(3);

        filePath = attachmentUrlStringForFileName(fileName);
        newText.replace(wholeLinkText, titlePart + QChar('(') + filePath + QChar(')'));
        textWasUpdated = true;
    }

    if (textWasUpdated) {
        storeNewText(std::move(newText));
    }

    return textWasUpdated;
}

/**
 * Fetches a note by its share id
 *
 * @param shareId
 * @return
 */
Note Note::fetchByShareId(int shareId) {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    query.prepare(QStringLiteral("SELECT * FROM note WHERE share_id = :share_id"));
    query.bindValue(QStringLiteral(":share_id"), shareId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        if (query.first()) {
            return noteFromQuery(query);
        }
    }

    return Note();
}

Note Note::fetchByName(const QString &name, const QString &noteSubFolderPathData,
                       const QString &pathDataSeparator) {
    int noteSubFolderId =
        NoteSubFolder::fetchByPathData(noteSubFolderPathData, pathDataSeparator).getId();

    return fetchByName(name, noteSubFolderId);
}

int Note::fetchNoteIdByName(const QString &name, int noteSubFolderId) {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    // get the active note subfolder id if none was set
    if (noteSubFolderId == -1) {
        noteSubFolderId = NoteSubFolder::activeNoteSubFolderId();
    }

    query.prepare(
        QStringLiteral("SELECT id FROM note WHERE name = :name AND "
                       "note_sub_folder_id = :note_sub_folder_id"));
    query.bindValue(QStringLiteral(":name"), name);
    query.bindValue(QStringLiteral(":note_sub_folder_id"), noteSubFolderId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        if (query.first()) {
            return query.value(QStringLiteral("id")).toInt();
        }
    }
    return -1;
}

Note Note::fetchByName(const QString &name, int noteSubFolderId) {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    // get the active note subfolder id if none was set
    if (noteSubFolderId == -1) {
        noteSubFolderId = NoteSubFolder::activeNoteSubFolderId();
    }

    query.prepare(
        QStringLiteral("SELECT * FROM note WHERE name = :name AND "
                       "note_sub_folder_id = :note_sub_folder_id"));
    query.bindValue(QStringLiteral(":name"), name);
    query.bindValue(QStringLiteral(":note_sub_folder_id"), noteSubFolderId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        if (query.first()) {
            return noteFromQuery(query);
        }
    }
    return Note();
}

Note Note::noteFromQuery(const QSqlQuery &query) { return Note().fillFromQuery(query); }

Note Note::fillFromQuery(const QSqlQuery &query) {
    _id = query.value(QStringLiteral("id")).toInt();
    _name = query.value(QStringLiteral("name")).toString();
    _fileName = query.value(QStringLiteral("file_name")).toString();
    _shareUrl = query.value(QStringLiteral("share_url")).toString();
    _shareId = query.value(QStringLiteral("share_id")).toInt();
    _sharePermissions = query.value(QStringLiteral("share_permissions")).toInt();
    _noteSubFolderId = query.value(QStringLiteral("note_sub_folder_id")).toInt();
    _noteText = query.value(QStringLiteral("note_text")).toString();
    _decryptedNoteText = query.value(QStringLiteral("decrypted_note_text")).toString();
    _cryptoKey = query.value(QStringLiteral("crypto_key")).toLongLong();
    _fileSize = query.value(QStringLiteral("file_size")).toLongLong();
    _cryptoPassword = query.value(QStringLiteral("crypto_password")).toString();
    _fileChecksum = query.value(QStringLiteral("file_checksum")).toString();
    _hasDirtyData = query.value(QStringLiteral("has_dirty_data")).toInt() == 1;
    _fileCreated = query.value(QStringLiteral("file_created")).toDateTime();
    _fileLastModified = query.value(QStringLiteral("file_last_modified")).toDateTime();
    _created = query.value(QStringLiteral("created")).toDateTime();
    _modified = query.value(QStringLiteral("modified")).toDateTime();
    return *this;
}

QVector<Note> Note::fetchAll(int limit) {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    QVector<Note> noteList;

    const QString sql = limit >= 0
                            ? QStringLiteral(
                                  "SELECT * FROM note ORDER BY file_last_modified DESC "
                                  "LIMIT :limit")
                            : QStringLiteral("SELECT * FROM note ORDER BY file_last_modified DESC");

    query.prepare(sql);

    if (limit >= 0) {
        noteList.reserve(limit);
        query.bindValue(QStringLiteral(":limit"), limit);
    }

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        // there is no way to get the num of rows returned by the query,
        // so we use static int to save the size after first query to
        // prevent extra allocations
        static int r = 0;
        noteList.reserve(r);
        for (r = 0; query.next(); r++) {
            noteList.append(noteFromQuery(query));
        }
    }

    return noteList;
}

QVector<int> Note::fetchAllIds(int limit, int offset) {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    QVector<int> noteIdList;
    QString sql = QStringLiteral("SELECT id FROM note ORDER BY id");

    if (limit >= 0) {
        sql += QStringLiteral(" LIMIT :limit");
    }

    if (offset >= 0) {
        sql += QStringLiteral(" OFFSET :offset");
    }

    query.prepare(sql);

    if (limit >= 0) {
        noteIdList.reserve(limit);
        query.bindValue(QStringLiteral(":limit"), limit);
    }

    if (offset >= 0) {
        query.bindValue(QStringLiteral(":offset"), offset);
    }

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            int id = query.value(QStringLiteral("id")).toInt();
            noteIdList.append(id);
        }
    }

    return noteIdList;
}

QVector<Note> Note::fetchAllByNoteSubFolderId(int noteSubFolderId) {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    QVector<Note> noteList;
    const QString sql = QStringLiteral(
        "SELECT * FROM note WHERE note_sub_folder_id = "
        ":note_sub_folder_id ORDER BY file_last_modified DESC");

    query.prepare(sql);
    query.bindValue(QStringLiteral(":note_sub_folder_id"), noteSubFolderId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            noteList.append(noteFromQuery(query));
        }
    }

    return noteList;
}

QVector<int> Note::fetchAllIdsByNoteSubFolderId(int noteSubFolderId) {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    QVector<int> noteList;
    const QString sql = QStringLiteral(
        "SELECT id FROM note WHERE note_sub_folder_id = "
        ":note_sub_folder_id ORDER BY file_last_modified DESC");

    query.prepare(sql);
    query.bindValue(QStringLiteral(":note_sub_folder_id"), noteSubFolderId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            int id = query.value(QStringLiteral("id")).toInt();
            noteList.append(id);
        }
    }

    return noteList;
}

/**
 * Gets a list of note ids from a note list
 */
QVector<int> Note::noteIdListFromNoteList(const QVector<Note> &noteList) {
    QVector<int> idList;
    idList.reserve(noteList.size());

    QVector<Note>::const_iterator i;
    for (i = noteList.constBegin(); i != noteList.constEnd(); ++i) {
        idList.append((*i).getId());
    }
    return idList;
}

/**
 * Returns all notes names that are not tagged
 */
QVector<int> Note::fetchAllNotTaggedIds() {
    QVector<Note> noteList = Note::fetchAll();
    QVector<int> untaggedNoteIdList;
    untaggedNoteIdList.reserve(noteList.size());

    QVector<Note>::const_iterator it = noteList.constBegin();
    for (; it != noteList.constEnd(); ++it) {
        if (!Tag::noteHasTags(*it, QString())) {
            untaggedNoteIdList << it->getId();
        }
    }

    return untaggedNoteIdList;
}

/**
 * Counts all notes that are not tagged
 */
int Note::countAllNotTagged(int activeNoteSubFolderId) {
    QVector<Note> noteList;
    QString path;
    if (activeNoteSubFolderId < 0) {
        noteList = Note::fetchAll();
    } else {
        noteList = Note::fetchAllByNoteSubFolderId(activeNoteSubFolderId);
        path = NoteSubFolder::fetch(activeNoteSubFolderId).relativePath();
    }

    QVector<Note>::const_iterator i;
    int count = 0;
    for (i = noteList.constBegin(); i != noteList.constEnd(); ++i) {
        if (!Tag::noteHasTags(*i, path)) {
            count++;
        }
    }
    return count;
}

QVector<Note> Note::search(const QString &text) {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    QVector<Note> noteList;

    query.prepare(
        QStringLiteral("SELECT * FROM note WHERE note_text LIKE :text "
                       "ORDER BY file_last_modified DESC"));
    query.bindValue(QStringLiteral(":text"), QStringLiteral("%") + text + QStringLiteral("%"));

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            noteList.append(noteFromQuery(query));
        }
    }

    return noteList;
}

QVector<QString> Note::searchAsNameListInCurrentNoteSubFolder(const QString &text,
                                                              bool searchInNameOnly) {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    QVector<QString> nameList;
    const QString textSearchSql =
        !searchInNameOnly ? QStringLiteral("OR note_text LIKE :text ") : QLatin1String("");
    const int noteSubFolderId = NoteSubFolder::activeNoteSubFolderId();

    query.prepare(QStringLiteral("SELECT name FROM note WHERE (name LIKE :text ") + textSearchSql +
                  QStringLiteral(") AND note_sub_folder_id = :note_sub_folder_id "
                                 "ORDER BY file_last_modified DESC"));
    query.bindValue(QStringLiteral(":text"), QStringLiteral("%") + text + QStringLiteral("%"));
    query.bindValue(QStringLiteral(":note_sub_folder_id"), noteSubFolderId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            nameList.append(query.value(QStringLiteral("name")).toString());
        }
    }

    return nameList;
}

QVector<QString> Note::searchAsNameList(const QString &text, bool searchInNameOnly) {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    QVector<QString> nameList;
    const QString textSearchSql =
        !searchInNameOnly ? QStringLiteral("OR note_text LIKE :text ") : QLatin1String("");

    query.prepare(QStringLiteral("SELECT name FROM note WHERE (name LIKE :text ") + textSearchSql +
                  QStringLiteral(") ORDER BY file_last_modified DESC"));
    query.bindValue(QStringLiteral(":text"), QStringLiteral("%") + text + QStringLiteral("%"));

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            nameList.append(query.value(QStringLiteral("name")).toString());
        }
    }

    return nameList;
}

bool Note::isNameSearch(const QString &searchTerm) {
    return searchTerm.startsWith(QStringLiteral("name:")) ||
           searchTerm.startsWith(QStringLiteral("n:"));
}

QString Note::removeNameSearchPrefix(QString searchTerm) {
    static const QRegularExpression re("^(name:|n:)");
    return searchTerm.remove(re);
}

/**
 * Searches for text in notes and returns the note ids
 *
 * By default notes that contain every single word will be found, `word1
 * word2` will find all notes that are containing `word1` and `word2`
 *
 * You can search for longer texts by using quotes, `"this word1" word2`
 * will find all notes that are containing `this word1` and `word2`
 */
QVector<int> Note::searchInNotes(QString search, bool ignoreNoteSubFolder, int noteSubFolderId) {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);
    auto noteIdList = QVector<int>();
    QStringList sqlList;

    // get the active note subfolder id if none was set
    if ((noteSubFolderId == -1) && !ignoreNoteSubFolder) {
        noteSubFolderId = NoteSubFolder::activeNoteSubFolderId();
    }

    // build the string list of the search string
    const QStringList queryStrings = buildQueryStringList(std::move(search));

    sqlList.reserve(queryStrings.count());

    // we want to search for the text in the note text and the filename
    for (int i = 0; i < queryStrings.count(); i++) {
        const QString &queryString = queryStrings[i];

        // if we just want to search in the name we use different columns
        // skip encrypted notes if search term is not found in (file) name of note
        sqlList.append(
            isNameSearch(queryString)
                ? QStringLiteral("(name LIKE ? OR file_name LIKE ?)")
                : QStringLiteral(
                      "((note_text LIKE ? AND note_text NOT LIKE '%\n%1\n%') OR name LIKE ?)")
                      .arg(NOTE_TEXT_ENCRYPTION_PRE_STRING));
    }

    QString sql;

    // build the query
    if (ignoreNoteSubFolder) {
        sql = QStringLiteral("SELECT id FROM note WHERE ") + sqlList.join(QStringLiteral(" AND "));
        query.prepare(sql);
    } else {
        sql = QStringLiteral(
                  "SELECT id FROM note WHERE note_sub_folder_id = "
                  ":note_sub_folder_id AND ") +
              sqlList.join(QStringLiteral(" AND "));
        query.prepare(sql);
        query.bindValue(0, noteSubFolderId);
    }

    // add the values to the query
    for (int i = 0; i < queryStrings.count(); i++) {
        QString queryString = queryStrings[i];

        // remove the search prefix if we searched for names only
        if (isNameSearch(queryString)) {
            queryString = removeNameSearchPrefix(queryString);
        }

        int pos = i * 2;
        pos = ignoreNoteSubFolder ? pos : pos + 1;

        // bind the values for the note text (or name) and the filename
        query.bindValue(pos, QStringLiteral("%") + queryString + QStringLiteral("%"));
        query.bindValue(pos + 1, QStringLiteral("%") + queryString + QStringLiteral("%"));
    }

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            noteIdList.append(query.value(QStringLiteral("id")).toInt());
        }
    }

    return noteIdList;
}

int Note::countSearchTextInNote(const QString &search) const {
    return _noteText.count(search, Qt::CaseInsensitive);
}

/**
 * Builds a string list of a search string
 */
QStringList Note::buildQueryStringList(QString searchString, bool escapeForRegularExpression,
                                       bool removeSearchPrefix) {
    auto queryStrings = QStringList();

    // check for strings in ""
    static const QRegularExpression re(QStringLiteral("\"([^\"]+)\""));
    QRegularExpressionMatchIterator i = re.globalMatch(searchString);
    while (i.hasNext()) {
        const QRegularExpressionMatch match = i.next();
        QString text = match.captured(1);

        if (escapeForRegularExpression) {
            text = QRegularExpression::escape(text);
        }

        queryStrings.append(text);
        searchString.remove(match.captured(0));
    }

    // remove a possible remaining "
    searchString.remove(QChar('\"'));
    // remove multiple spaces and spaces in front and at the end
    searchString = searchString.simplified();

    const QStringList searchStringList = searchString.split(QChar(' '));
    queryStrings.reserve(searchStringList.size());
    // add the remaining strings
    for (QString text : searchStringList) {
        if (removeSearchPrefix) {
            if (isNameSearch(text)) {
                text = removeNameSearchPrefix(text);
            }
        }

        // escape the text so strings like `^ ` don't cause an
        // infinite loop
        queryStrings.append(escapeForRegularExpression ? QRegularExpression::escape(text) : text);
    }

    // remove empty items, so the search will not run amok
    queryStrings.removeAll(QLatin1String(""));

    // remove duplicate query items
    queryStrings.removeDuplicates();

    return queryStrings;
}

QStringList Note::fetchNoteNamesInCurrentNoteSubFolder() {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    QStringList list;
    const int noteSubFolderId = NoteSubFolder::activeNoteSubFolderId();

    query.prepare(
        QStringLiteral("SELECT DISTINCT(name) FROM note WHERE "
                       "note_sub_folder_id = :note_sub_folder_id "
                       "ORDER BY file_last_modified DESC"));
    query.bindValue(QStringLiteral(":note_sub_folder_id"), noteSubFolderId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            QString name = query.value(QStringLiteral("name")).toString();
            if (!name.isEmpty()) {
                list.append(name);
            }
        }
    }

    return list;
}

QStringList Note::fetchNoteNames() {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    QStringList list;

    query.prepare(
        QStringLiteral("SELECT DISTINCT(name) FROM note "
                       "ORDER BY file_last_modified DESC"));

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            const QString name = query.value(QStringLiteral("name")).toString();
            if (!name.isEmpty()) {
                list.append(name);
            }
        }
    }

    return list;
}

QStringList Note::fetchNoteFileNames() {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    QStringList list;

    query.prepare(QStringLiteral("SELECT file_name FROM note ORDER BY file_last_modified DESC"));
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            list.append(query.value(QStringLiteral("file_name")).toString());
        }
    }

    return list;
}

QVector<int> Note::fetchAllIdsByNoteTextPart(const QString &textPart) {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    QVector<int> list;

    query.prepare(
        QStringLiteral("SELECT id FROM note WHERE note_text LIKE :text "
                       "ORDER BY file_last_modified DESC"));
    query.bindValue(QStringLiteral(":text"), QStringLiteral("%") + textPart + QStringLiteral("%"));

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            list.append(query.value(QStringLiteral("id")).toInt());
        }
    }

    return list;
}

bool Note::storeNewText(QString text) {
    if (!this->fileWriteable() || !Utils::Misc::isNoteEditingAllowed()) {
        return false;
    }

    this->_noteText = std::move(text);
    this->_hasDirtyData = true;

    return this->store();
}

void Note::setDecryptedText(QString text) { this->_decryptedNoteText = std::move(text); }

bool Note::storeNewDecryptedText(QString text) {
    if (text == this->_decryptedNoteText || !Utils::Misc::isNoteEditingAllowed()) {
        return false;
    }

    this->_decryptedNoteText = std::move(text);
    this->_hasDirtyData = true;

    return this->store();
}

/**
 * Returns the default note file extension (`md`, `txt` or custom extensions)
 */
QString Note::defaultNoteFileExtension() {
    return SettingsService()
        .value(QStringLiteral("defaultNoteFileExtension"), QStringLiteral("md"))
        .toString();
}

/**
 * Returns the a list of all note file extensions
 */
QStringList Note::noteFileExtensionList(const QString &prefix) {
    const SettingsService settings;
    QStringList list = settings.value(QStringLiteral("noteFileExtensionList")).toStringList();
    list.removeDuplicates();

    if (list.isEmpty()) {
        list << defaultNoteFileExtension();
    }

    if (!prefix.isEmpty()) {
        list.replaceInStrings(QRegularExpression(QStringLiteral("^")), prefix);
    }

    return list;
}

/**
 * Checks if it is allowed to have a different note file name than the headline
 */
bool Note::allowDifferentFileName() {
    return NoteFolder::currentNoteFolder()
        .settingsValue(QStringLiteral("allowDifferentNoteFileName"))
        .toBool();
}

//
// inserts or updates a note object in the database
//
bool Note::store() {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    if (_fileName.isEmpty()) {
        // don't store notes with empty filename and empty name
        if (_name.isEmpty()) {
            return false;
        }

        generateFileNameFromName();
    }

    if (_id > 0) {
        query.prepare(
            QStringLiteral("UPDATE note SET "
                           "name = :name,"
                           "share_url = :share_url,"
                           "share_id = :share_id,"
                           "share_permissions = :share_permissions,"
                           "file_name = :file_name,"
                           "file_size = :file_size,"
                           "note_sub_folder_id = :note_sub_folder_id,"
                           "note_text = :note_text,"
                           "decrypted_note_text = :decrypted_note_text,"
                           "has_dirty_data = :has_dirty_data, "
                           "file_last_modified = :file_last_modified,"
                           "file_created = :file_created,"
                           "crypto_key = :crypto_key,"
                           "crypto_password = :crypto_password,"
                           "file_checksum = :file_checksum,"
                           "modified = :modified "
                           "WHERE id = :id"));
        query.bindValue(QStringLiteral(":id"), _id);
    } else {
        query.prepare(
            QStringLiteral("INSERT INTO note"
                           "(name, share_url, share_id, share_permissions, file_name, "
                           "file_size, note_text, has_dirty_data, "
                           "file_last_modified, file_created, crypto_key,"
                           "modified, crypto_password, decrypted_note_text, "
                           "note_sub_folder_id, file_checksum) "
                           "VALUES (:name, :share_url, :share_id, :share_permissions, "
                           ":file_name, :file_size, :note_text,"
                           ":has_dirty_data, :file_last_modified,"
                           ":file_created, :crypto_key, :modified,"
                           ":crypto_password, :decrypted_note_text,"
                           ":note_sub_folder_id, :file_checksum)"));
    }

    const QDateTime modified = QDateTime::currentDateTime();

    // get the size of the note text
    const QByteArray &bytes = _noteText.toUtf8();
    _fileSize = bytes.size();

    query.bindValue(QStringLiteral(":name"), _name);
    query.bindValue(QStringLiteral(":share_url"), _shareUrl);
    query.bindValue(QStringLiteral(":share_id"), _shareId);
    query.bindValue(QStringLiteral(":share_permissions"), _sharePermissions);
    query.bindValue(QStringLiteral(":file_name"), _fileName);
    query.bindValue(QStringLiteral(":file_size"), _fileSize);
    query.bindValue(QStringLiteral(":note_sub_folder_id"), _noteSubFolderId);
    query.bindValue(QStringLiteral(":note_text"), _noteText);
    query.bindValue(QStringLiteral(":decrypted_note_text"), _decryptedNoteText);
    query.bindValue(QStringLiteral(":has_dirty_data"), _hasDirtyData ? 1 : 0);
    query.bindValue(QStringLiteral(":file_created"), _fileCreated);
    query.bindValue(QStringLiteral(":file_last_modified"), _fileLastModified);
    query.bindValue(QStringLiteral(":crypto_key"), _cryptoKey);
    query.bindValue(QStringLiteral(":crypto_password"), _cryptoPassword);
    query.bindValue(QStringLiteral(":file_checksum"), _fileChecksum);
    query.bindValue(QStringLiteral(":modified"), modified);

    // on error
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else if (_id == 0) {    // on insert
        _id = query.lastInsertId().toInt();
    }

    this->_modified = modified;
    return true;
}

/**
 * Stores a note text file to disk
 * The file name will be changed if needed
 *
 * @return true if note was stored
 */
bool Note::storeNoteTextFileToDisk() {
    bool currentNoteTextChanged = false;

    // We couldn't set a default parameter for currentNoteTextChanged, because that
    // would only work with a pointer and then the app would crash when
    // currentNoteTextChanged would get assigned a value
    return storeNoteTextFileToDisk(currentNoteTextChanged);
}

/**
 * Stores a note text file to disk
 * The file name will be changed if needed
 *
 * @param currentNoteTextChanged true if the note text was changed during a rename
 * @param wasCancelledDueToExternalModification true if the save was cancelled because file was
 * modified externally
 * @return true if note was stored
 */
bool Note::storeNoteTextFileToDisk(bool &currentNoteTextChanged,
                                   bool *wasCancelledDueToExternalModification) {
    const Note oldNote = *this;
    const QString oldName = _name;
    const QString oldNoteFilePath = fullNoteFilePath();
    TrashItem trashItem = TrashItem::prepare(this);

    if (allowDifferentFileName()) {
        // check if a QML function wants to set another note file name and
        // modify it accordingly
        modifyNoteTextFileNameFromQMLHook();
    } else {
        // checks if filename has to be changed (and change it if needed)
        handleNoteTextFileName();
    }

    const QString newName = _name;
    bool noteFileWasRenamed = false;

    // rename the current note file if the file name changed
    if (oldName != newName) {
        QFile oldFile(oldNoteFilePath);

        // rename the note file
        if (oldFile.exists()) {
            noteFileWasRenamed = oldFile.rename(fullNoteFilePath());
            qDebug() << __func__ << " - 'noteFileWasRenamed': " << noteFileWasRenamed;

            // Restore the created date of the current note under Windows,
            // because it gets set to the current date when note is renamed
            restoreCreatedDate();
        }
    }

    QFile file(fullNoteFilePath());
    QFile::OpenMode flags = QIODevice::WriteOnly;
    const SettingsService settings;
    const bool useUNIXNewline = settings.value(QStringLiteral("useUNIXNewline")).toBool();

    if (!useUNIXNewline) {
        flags |= QIODevice::Text;
    }

    // Check if the file was modified externally by comparing checksums
    if (fileExists() && !_fileChecksum.isEmpty()) {
        // Read the current file content
        QFile checkFile(fullNoteFilePath());
        if (checkFile.open(QIODevice::ReadOnly)) {
            QTextStream checkIn(&checkFile);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            checkIn.setCodec("UTF-8");
#endif
            QString currentFileText = checkIn.readAll();
            checkFile.close();

            // Calculate checksum of current file content
            QString currentChecksum = calculateChecksum(currentFileText);

            // If checksums don't match, the file was modified externally
            if (currentChecksum != _fileChecksum) {
                qWarning() << "Note file was modified externally, showing diff dialog";
#ifndef INTEGRATION_TESTS
                // Show diff dialog to let user decide what to do
                TextDiffDialog dialog(
                    nullptr, QObject::tr("Note file was modified externally"),
                    QObject::tr(
                        "The note file '%1' was modified externally while you had unsaved "
                        "changes.\n\n"
                        "The editor below shows YOUR version. The differences view shows changes "
                        "between "
                        "the external file (in red) and your version (in green).\n\n"
                        "Click 'OK' to save YOUR version and overwrite the external changes.\n"
                        "Click 'Cancel' to discard your changes and keep the external version.")
                        .arg(_fileName),
                    currentFileText, _noteText);

                if (dialog.exec() == QDialog::Accepted) {
                    if (dialog.resultAccepted()) {
                        // User chose to use their edited text from the dialog
                        _noteText = dialog.resultText();
                    } else {
                        // User chose to cancel, keep the external version
                        _noteText = currentFileText;
                        _fileChecksum = currentChecksum;

                        // Mark that we cancelled due to external modification
                        if (wasCancelledDueToExternalModification != nullptr) {
                            *wasCancelledDueToExternalModification = true;
                        }

                        return false;
                    }
                } else {
                    // Dialog was closed without accepting, keep the external version
                    _noteText = currentFileText;
                    _fileChecksum = currentChecksum;

                    // Mark that we cancelled due to external modification
                    if (wasCancelledDueToExternalModification != nullptr) {
                        *wasCancelledDueToExternalModification = true;
                    }

                    return false;
                }
#endif
            }
        }
    }

    qDebug() << "storing note file: " << this->_fileName;

    if (!file.open(flags)) {
        qCritical() << QObject::tr(
                           "Could not store note file: %1 - Error "
                           "message: %2")
                           .arg(file.fileName(), file.errorString());
        return false;
    }

    const bool fileExists = this->fileExists();

    // assign the tags to the new name if the name has changed
    if (oldName != newName) {
        if (!noteFileWasRenamed && TrashItem::isLocalTrashEnabled()) {
            qDebug() << __func__ << " - 'trashItem': " << trashItem;

            // trash the old note
            trashItem.doTrashing();
        }

        // rename the note file names of note tag links
        Tag::renameNoteFileNamesOfLinks(oldName, newName, this->getNoteSubFolder());

        // handle the replacing of all note urls if a note was renamed
        // (we couldn't make currentNoteTextChanged a pointer or the app would crash)
        currentNoteTextChanged = handleNoteMoving(oldNote);
    }

    // if we find a decrypted text to encrypt, then we attempt to encrypt it
    if (!_decryptedNoteText.isEmpty()) {
        _noteText = _decryptedNoteText;
        encryptNoteText();
        _decryptedNoteText = QLatin1String("");
    }

    // transform all types of newline to \n
    // (maybe the ownCloud-sync works better then)
    const QString text = Utils::Misc::transformLineFeeds(this->_noteText);

    //    diff_match_patch *diff = new diff_match_patch();
    //    QList<Diff> diffList = diff->diff_main( this->noteText, text );

    //    QString html = diff->diff_prettyHtml( diffList );
    //    diff->diff_cleanupSemantic( diffList );
    //    qDebug() << __func__ << " - 'diffList': " << diffList[0].toString();
    //    qDebug() << __func__ << " - 'html': " << html;

    QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");
#endif
    out << text;
    out.flush();
    file.flush();
    file.close();

    // Update the checksum after writing to disk
    this->_fileChecksum = calculateChecksum(text);

    this->_hasDirtyData = false;
    this->_fileLastModified = QDateTime::currentDateTime();

    if (!fileExists || !this->_fileCreated.isValid()) {
        this->_fileCreated = this->_fileLastModified;
    }

    const bool noteStored = this->store();

    // if note was stored but the note file wasn't renamed do some more checks
    // whether we need to remove the old note file
    if (noteStored && !noteFileWasRenamed) {
        QFile oldFile(oldNoteFilePath);
        const QFileInfo oldFileInfo(oldFile);
        const QFile newFile(fullNoteFilePath());
        const QFileInfo newFileInfo(newFile);

        // in the end we want to remove the old note file if note was stored and
        // filename has changed
        // #1190: we also need to check if the files are the same even if the
        // name is not the same for NTFS
        if ((fullNoteFilePath() != oldNoteFilePath) && (oldFileInfo != newFileInfo)) {
            // remove the old note file
            if (oldFile.exists() && oldFileInfo.isFile() && oldFileInfo.isReadable() &&
                oldFile.remove()) {
                qInfo() << QObject::tr("Renamed note-file was removed: %1").arg(oldFile.fileName());

            } else {
                qWarning() << QObject::tr(
                                  "Could not remove renamed note-file: %1"
                                  " - Error message: %2")
                                  .arg(oldFile.fileName(), oldFile.errorString());
            }
        }
    }

    return noteStored;
}

/**
 * Restores the created date of the current note under Windows,
 * because it gets set to the current date when note is renamed
 *
 * See: https://github.com/pbek/QOwnNotes/issues/1743
 */
void Note::restoreCreatedDate() {
#ifdef Q_OS_WIN32
    // QDateTime to FILETIME conversion
    // see: https://stackoverflow.com/questions/19704817/qdatetime-to-filetime
    QDateTime origin(QDate(1601, 1, 1), QTime(0, 0, 0, 0), Qt::UTC);
    const qint64 _100nanosecs = 10000 * origin.msecsTo(_fileCreated);
    FILETIME fileTime;
    fileTime.dwLowDateTime = _100nanosecs;
    fileTime.dwHighDateTime = (_100nanosecs >> 32);

    LPCWSTR filePath = (const wchar_t *)QDir::toNativeSeparators(fullNoteFilePath()).utf16();
    HANDLE fileHandle =
        CreateFile(filePath, FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                   OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    // set the created date to the old created date before the rename
    // see:
    // https://stackoverflow.com/questions/10041651/changing-the-file-creation-date-in-c-using-windows-h-in-windows-7
    SetFileTime(fileHandle, &fileTime, (LPFILETIME) nullptr, (LPFILETIME) nullptr);
    CloseHandle(fileHandle);
#endif
}

/**
 * Does a file name cleanup
 */
QString Note::cleanupFileName(QString name) {
    // remove characters from the name that are problematic
    static const QRegularExpression re(QStringLiteral(R"([\/\\:])"));
    name.remove(re);

    // remove multiple whitespaces from the name
    static const QRegularExpression re1(QStringLiteral("\\s+"));
    name.replace(re1, QStringLiteral(" "));

    return name;
}

/**
 * Does the extended filename cleanup
 * Will be triggered mainly on FAT and NTFS filesystems
 *
 * @param name
 * @return
 */
QString Note::extendedCleanupFileName(QString name) {
    // replace characters that cause problems on certain filesystems when
    // present in filenames with underscores
    static const QRegularExpression re(QStringLiteral(R"([\/\\:<>\"\|\?\*])"));
    name.replace(re, QStringLiteral(" "));

    return name;
}

/**
 * Checks if a QML function wants to set another note file name and
 * modifies it accordingly
 */
bool Note::modifyNoteTextFileNameFromQMLHook() {
    // check if a QML function wants to set another note name
    const QString newName = ScriptingService::instance()->callHandleNoteTextFileNameHook(this);

    // set the file name from the QML hook
    if (!newName.isEmpty() && (newName != _name)) {
        qDebug() << __func__ << " - 'newName': " << newName;

        // store new name and filename
        _name = newName;
        _fileName = newName + QStringLiteral(".") + fileNameSuffix();
        return store();
    }

    return false;
}

/**
 * Checks if the filename has to be changed
 * Generates a new name and filename and removes the old file
 * (the new file is not stored to a note text file!)
 *
 * @return (bool) true if filename was changed
 */
bool Note::handleNoteTextFileName() {
    QString noteText = _noteText;

    // remove frontmatter from start of Markdown text
    if (noteText.startsWith(QLatin1String("---"))) {
        static const QRegularExpression re(
            QStringLiteral(
                R"(^---((\r\n)|(\n\r)|\r|\n).+?((\r\n)|(\n\r)|\r|\n)---((\r\n)|(\n\r)|\r|\n))"),
            QRegularExpression::DotMatchesEverythingOption);
        noteText.remove(re);
    }

    // remove html comment from start of Markdown text
    if (noteText.startsWith(QLatin1String("<!--"))) {
        static const QRegularExpression re(QStringLiteral(R"(^<!--.+?-->((\r\n)|(\n\r)|\r|\n))"),
                                           QRegularExpression::DotMatchesEverythingOption);
        noteText.remove(re);
    }

    // split the text into a string list
    static const QRegularExpression re(QStringLiteral(R"((\r\n)|(\n\r)|\r|\n)"));
    const QStringList noteTextLines = noteText.trimmed().split(re);
    const int noteTextLinesCount = noteTextLines.count();

    // do nothing if there is no text
    if (noteTextLinesCount == 0) {
        return false;
    }

    QString name = noteTextLines.at(0).trimmed();
    // do nothing if the first line is empty
    if (name.isEmpty()) {
        return false;
    }

    // remove a leading "# " for Markdown headlines
    static const QRegularExpression headlinesRE(QStringLiteral("^#\\s"));
    name.remove(headlinesRE);

    // cleanup additional characters
    name = cleanupFileName(name);

    if (name.isEmpty()) {
        name = QObject::tr("Note");
    }

    // check if name has changed
    if (name != this->_name) {
        qDebug() << __func__ << " - 'name' was changed: " << name;
        QString fileName = generateNoteFileNameFromName(name);

        // only try to find a new name if the filename in lowercase has changed
        // to prevent troubles on case-insensitive filesystems like NTFS
        if (fileName.toLower() != this->_fileName.toLower()) {
            int nameCount = 0;
            const QString nameBase = name;

            // check if note with this filename already exists
            while (Note::fetchByFileName(fileName).isFetched()) {
                // find new filename for the note
                name = nameBase + QStringLiteral(" ") + QString::number(++nameCount);
                fileName = generateNoteFileNameFromName(name);
                qDebug() << __func__ << " - 'override fileName': " << fileName;

                if (nameCount > 1000) {
                    break;
                }
            }
        }

        // update the first line of the note text
        // TODO(pbek): UI has to be updated too then!
        // update: we now try not to change the first line of the note,
        //         this doesn't seem to trouble ownCloud / Nextcloud notes
        //         a lot, but it renames the notes to its own liking
        //        noteTextLines[0] = name;
        //        this->noteText = noteTextLines.join("\n");

        // set the new name and filename
        this->_name = name;
        generateFileNameFromName();

        // let's check if we would be able to write to the file
        if (!canWriteToNoteFile()) {
            qDebug() << __func__ << " - cannot write to file " << this->_fileName
                     << " - we will try another filename";

            // we try to replace some more characters (mostly for Windows
            // filesystems)
            name = extendedCleanupFileName(name);

            this->_name = name;
            generateFileNameFromName();
        }

        return this->store();
    }

    return false;
}

/**
 * Generates a note filename from a name
 *
 * @param name
 * @return
 */
QString Note::generateNoteFileNameFromName(const QString &name) {
    return name + QStringLiteral(".") + defaultNoteFileExtension();
}

/**
 * Generates filename of the note from it's name
 */
void Note::generateFileNameFromName() { _fileName = generateNoteFileNameFromName(_name); }

/**
 * Checks if we can write to the note file
 *
 * @return
 */
bool Note::canWriteToNoteFile() {
    QFile file(fullNoteFilePath());
    const bool canWrite = file.open(QIODevice::WriteOnly);
    const bool fileExists = file.exists();

    if (file.isOpen()) {
        file.close();

        if (!fileExists) {
            file.remove();
        }
    }

    return canWrite;
}

bool Note::updateNoteTextFromDisk() {
    if (!isFetched()) {
        return false;
    }

    QFile file(fullNoteFilePath());

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << __func__ << " - 'file': " << file.fileName();
        qDebug() << __func__ << " - " << file.errorString();
        return false;
    }

    // Update the last modified date
    QFileInfo fileInfo;
    fileInfo.setFile(file);
    this->_fileLastModified = fileInfo.lastModified();

    QTextStream in(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    in.setCodec("UTF-8");
#endif
    this->_noteText = in.readAll();
    file.close();

    // strangely it sometimes gets null
    if (this->_noteText.isNull()) this->_noteText = QLatin1String("");

    // Calculate and store the checksum of the loaded text
    this->_fileChecksum = calculateChecksum(this->_noteText);

    return true;
}

QString Note::getFullFilePathForFile(const QString &fileName) {
    const SettingsService settings;

    // prepend the portable data path if we are in portable mode
    const QString notesPath = Utils::Misc::prependPortableDataPathIfNeeded(
        settings.value(QStringLiteral("notesPath")).toString());

    const QString path = Utils::Misc::removeIfEndsWith(std::move(notesPath), QStringLiteral("/")) +
                         Utils::Misc::dirSeparator() + fileName;
    const QFileInfo fileInfo(path);

    // we can't get a canonical path if the path doesn't exist
    if (!fileInfo.exists()) {
        return path;
    }

    // we need that for links to notes in subfolders in portable mode if
    // note folder lies outside of the application directory
    // Warning: This also resolves symbolic links! If a folder that lies outside
    //          of the note folder is linked into it as subfolder
    //          "canonicalFilePath" will show the original file path!
#ifdef Q_OS_WIN32
    // Let's stay with "canonicalFilePath" on Windows in case there is any issue
    // in portable mode
    const QString canonicalFilePath = fileInfo.canonicalFilePath();
#else
    // Don't resolve symbolic links
    const QString canonicalFilePath = fileInfo.absoluteFilePath();
#endif

    return canonicalFilePath;
}

QString Note::getFilePathRelativeToNote(const Note &note, const QString &connectionName) const {
    const QDir dir(fullNoteFilePath(connectionName));

    // for some reason there is a leading "../" too much
    static const QRegularExpression re(QStringLiteral(R"(^\.\.\/)"));
    QString path = dir.relativeFilePath(note.fullNoteFilePath(connectionName)).remove(re);

    // if "note" is the current note we want to use the real filename
    if (path == QChar('.')) {
        path = note.getFileName();
    }

    return path;
}

QString Note::getNoteUrlForLinkingTo(const Note &note, bool forceLegacy) const {
    const SettingsService settings;
    QString noteUrl;

    if (forceLegacy || settings.value(QStringLiteral("legacyLinking")).toBool()) {
        const QString noteNameForLink = Note::generateTextForLink(note.getName());
        noteUrl = QStringLiteral("note://") + noteNameForLink;
    } else {
        noteUrl = urlEncodeNoteUrl(getFilePathRelativeToNote(note));

        // if one of the link characters `<>()` were found in the note url use
        // the legacy way of linking because otherwise the "url" would break the
        // Markdown link
        static const QRegularExpression re(QRegularExpression(R"([<>()])"));
        if (noteUrl.contains(re)) {
            noteUrl = getNoteUrlForLinkingTo(note, true);
        }
    }

    return noteUrl;
}

/**
 * Returns an url string that is fit to be placed in a note link
 *
 * Example:
 * "Note with one bracket].md" will get "Note%20with%20one%20bracket%5D.md"
 */
QString Note::urlEncodeNoteUrl(const QString &url, bool escapeSlashes) {
    auto encoded = QString(QUrl::toPercentEncoding(url));

    // We don't really need to escape slashes in the note url
    // https://github.com/pbek/QOwnNotes/issues/1717#issuecomment-2336767704
    if (!escapeSlashes) {
        encoded.replace(QStringLiteral("%2F"), QStringLiteral("/"));
    }

    return encoded;
}

/**
 * Returns the url decoded representation of a string to e.g. fetch a note from
 * the note database if url came from a note link
 *
 * Example:
 * "Note%20with%20one%20bracket%5D.md" will get "Note with one bracket].md"
 * "Note%20with&#32;one bracket].md" will also get "Note with one bracket].md"
 */
QString Note::urlDecodeNoteUrl(QString url) {
    return QUrl::fromPercentEncoding(
        url.replace(QStringLiteral("&#32;"), QStringLiteral(" ")).toUtf8());
}

/**
 * Returns the full path of the note file
 */
QString Note::fullNoteFilePath(const QString &connectionName) const {
    return getFullFilePathForFile(relativeNoteFilePath(QString(), connectionName));
}

/**
 * Returns the full path of directory of the note file
 */
QString Note::fullNoteFileDirPath() const {
    QFileInfo fileInfo;
    fileInfo.setFile(fullNoteFilePath());
    return fileInfo.dir().path();
}

/**
 * Returns the relative path of the note file
 */
QString Note::relativeNoteFilePath(QString separator, const QString &connectionName) const {
    QString fullFileName = _fileName;

    if (separator.isEmpty()) {
        separator = Utils::Misc::dirSeparator();
    }

    if (_noteSubFolderId > 0) {
        const NoteSubFolder noteSubFolder = getNoteSubFolder(connectionName);
        if (noteSubFolder.isFetched()) {
            fullFileName.prepend(noteSubFolder.relativePath('/', connectionName) + separator);
        }
    }

    return fullFileName;
}

/**
 * Returns the relative path of the note subfolder
 */
QString Note::relativeNoteSubFolderPath() const {
    QString path = QLatin1String("");

    if (_noteSubFolderId > 0) {
        const NoteSubFolder noteSubFolder = getNoteSubFolder();
        if (noteSubFolder.isFetched()) {
            path = noteSubFolder.relativePath();
        }
    }

    return path;
}

/**
 * Returns the path-data of the note subfolder file
 */
QString Note::noteSubFolderPathData() const {
    QString path = QLatin1String("");

    if (_noteSubFolderId > 0) {
        const NoteSubFolder noteSubFolder = getNoteSubFolder();
        if (noteSubFolder.isFetched()) {
            path = noteSubFolder.pathData();
        }
    }

    return path;
}

/**
 * Returns the full url of the note file
 */
QUrl Note::fullNoteFileUrl() const {
    QString windowsSlash = QLatin1String("");

#ifdef Q_OS_WIN32
    // we need another slash for Windows
    windowsSlash = QStringLiteral("/");
#endif

    return QUrl(QStringLiteral("file://") + windowsSlash +
                QUrl::toPercentEncoding(fullNoteFilePath(), QByteArrayLiteral(":/")));
}

/**
 * Stores all notes that were changed to disk
 *
 * @param currentNote will be set by this method if the filename of the current
 * note has changed
 * @param currentNoteChanged true if current note was changed
 * @param noteWasRenamed true if a note was renamed
 * @param currentNoteTextChanged true if the current note text was changed during a rename
 * @return amount of notes that were saved
 */
int Note::storeDirtyNotesToDisk(Note &currentNote, bool *currentNoteChanged, bool *noteWasRenamed,
                                bool *currentNoteTextChanged) {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);
    ScriptingService *scriptingService = ScriptingService::instance();
    //    qDebug() << "storeDirtyNotesToDisk";

    query.prepare(QStringLiteral("SELECT * FROM note WHERE has_dirty_data = 1"));
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return 0;
    }
    int count = 0;
    for (int r = 0; query.next(); r++) {
        Note note = noteFromQuery(query);
        const QString &oldName = note.getName();
        bool wasCancelledDueToExternalModification = false;
        const bool noteWasStored = note.storeNoteTextFileToDisk(
            *currentNoteTextChanged, &wasCancelledDueToExternalModification);

        // If the save was cancelled due to external modification and this is the current note,
        // we need to reload the note text to show the external version
        if (wasCancelledDueToExternalModification && note.isSameFile(currentNote)) {
            // Update the current note with the external version
            currentNote = note;
            *currentNoteTextChanged = true;

            // Store the note to persist the new checksum and text
            note.setHasDirtyData(false);
            note.store();

            qDebug() << "Note save cancelled due to external modification, reloading current note";
        }

        // continue if note couldn't be stored
        if (!noteWasStored) {
            continue;
        }

        const QString &newName = note.getName();

        // check if the file name has changed
        if (oldName != newName) {
            // rename the note file names of note tag links
            Tag::renameNoteFileNamesOfLinks(oldName, newName, note.getNoteSubFolder());
            *noteWasRenamed = true;

            // override the current note because the file name has changed
            if (note.isSameFile(currentNote)) {
                currentNote = note;
            }

            // handle the replacing of all note urls if a note was renamed
            // we don't need to do that here, it would be called two
            // times this way
            //                Note::handleNoteRenaming(oldName, newName);
        }

        // emit the signal for the QML that the note was stored
        emit scriptingService->noteStored(
            QVariant::fromValue(static_cast<QObject *>(NoteApi::fromNote(note))));

        // reassign currentNote if filename of currentNote has changed
        if (note.isSameFile(currentNote)) {
            *currentNoteChanged = true;
        }

        qDebug() << "stored note: " << note;
        count++;
    }

    return count;
}

/**
 * Strips trailing whitespaces off the note text
 *
 * @param skipLine skip that line (e.g. the current line)
 * @return
 */
bool Note::stripTrailingSpaces(int skipLine) {
    QStringList noteTextLines = getNoteTextLines();
    const int lineCount = noteTextLines.count();
    bool wasStripped = false;

    for (int l = 0; l < lineCount; l++) {
        if (l == skipLine) {
            continue;
        }

        const auto lineText = noteTextLines.at(l);
        if (lineText.endsWith(QChar(' '))) {
            noteTextLines[l] = Utils::Misc::rstrip(Utils::Misc::rstrip(lineText));
            wasStripped = true;
        }
    }

    if (wasStripped) {
        _noteText = noteTextLines.join(detectNewlineCharacters());
        store();
    }

    return wasStripped;
}

QString Note::detectNewlineCharacters() {
    if (_noteText.contains(QStringLiteral("\r\n"))) {
        return QStringLiteral("\r\n");
    } else if (_noteText.contains(QStringLiteral("\n\r"))) {
        return QStringLiteral("\n\r");
    } else if (_noteText.contains(QStringLiteral("\r"))) {
        return QStringLiteral("\r");
    }

    return QStringLiteral("\n");
}

void Note::createFromFile(QFile &file, int noteSubFolderId, bool withNoteNameHook) {
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        in.setCodec("UTF-8");
#endif

        // qDebug() << file.size() << in.readAll();
        const QString noteText = in.readAll();
        file.close();

        QFileInfo fileInfo;
        fileInfo.setFile(file);

        // create a nicer name by removing the extension
        // TODO(pbek): make sure name is ownCloud Notes conform
        QString name = fileInfo.fileName();

        const int lastPoint = name.lastIndexOf(QLatin1Char('.'));
        name = name.left(lastPoint);

        this->_name = std::move(name);
        this->_fileName = fileInfo.fileName();
        this->_noteSubFolderId = noteSubFolderId;
        this->_noteText = noteText;

#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
        this->_fileCreated = fileInfo.created();
#else
        this->_fileCreated = fileInfo.birthTime();
#endif

        this->_fileLastModified = fileInfo.lastModified();
        this->store();

        if (withNoteNameHook) {
            // check if a name was set in a script
            const QString hookName = ScriptingService::instance()->callHandleNoteNameHook(this);

            if (!hookName.isEmpty()) {
                this->_name = std::move(hookName);
                this->store();
            }
        }
    }
}

/**
 * Updates or creates a note from a file
 *
 * @param file
 * @param noteSubFolder
 * @return
 */
Note Note::updateOrCreateFromFile(QFile &file, const NoteSubFolder &noteSubFolder,
                                  bool withNoteNameHook) {
    const QFileInfo fileInfo(file);
    Note note = fetchByFileName(fileInfo.fileName(), noteSubFolder.getId());

    // regardless if the file was found or not, if the size differs or the
    // file was modified after the internal note was modified we want to load
    // the note content again
    if ((fileInfo.size() != note.getFileSize()) || (fileInfo.lastModified() > note.getModified())) {
        // load file data and store note
        note.createFromFile(file, noteSubFolder.getId(), withNoteNameHook);

        //        qDebug() << __func__ << " - 'file modified': " <<
        //        file.fileName();
    }

    return note;
}

//
// deletes all notes in the database
//
bool Note::deleteAll() {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    // no truncate in sqlite
    query.prepare(QStringLiteral("DELETE FROM note"));
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else {
        return true;
    }
}

/**
 * Checks if file of note exists in the filesystem and is readable
 *
 * @return bool
 */
bool Note::fileExists() const { return Utils::Misc::fileExists(fullNoteFilePath()); }

/**
 * Checks if file of note exists in the filesystem and is writable
 *
 * @return bool
 */
bool Note::fileWriteable() const {
    const QFile file(fullNoteFilePath());
    const QFileInfo fileInfo(file);
    return file.exists() && fileInfo.isFile() && fileInfo.isWritable();
}

//
// checks if the current note still exists in the database
//
bool Note::exists() const { return noteIdExists(this->_id); }

bool Note::noteIdExists(int id) { return fetch(id)._id > 0; }

//
// reloads the current Note (by fileName)
//
bool Note::refetch() { return this->fillByFileName(_fileName, _noteSubFolderId); }

/**
 * Returns the suffix of the note file name
 */
QString Note::fileNameSuffix() const { return QFileInfo(_fileName).suffix(); }

/**
 * Returns the base name of the note file name
 */
QString Note::fileBaseName(bool withFullName) {
    if (withFullName) {
        QStringList parts = _fileName.split(QChar('.'));
        parts.removeLast();
        return parts.join(QChar('.'));
    }
    QFileInfo fileInfo;
    fileInfo.setFile(_fileName);
    return fileInfo.baseName();
}

/**
 * Renames a note file
 *
 * @param newName new file name (without file-extension)
 * @return
 */
bool Note::renameNoteFile(QString newName) {
    // cleanup not allowed characters
    newName = cleanupFileName(std::move(newName));

    // add the old file suffix to the name
    const QString newFileName = newName + QChar('.') + fileNameSuffix();

    // check if name has really changed
    if (_name == newName) {
        return false;
    }

    // check if name already exists
    const Note existingNote = Note::fetchByName(newName);
    if (existingNote.isFetched() && (existingNote.getId() != _id)) {
        return false;
    }

    if (TrashItem::isLocalTrashEnabled()) {
        // add note to trash
        bool trashResult = TrashItem::add(this);
        qDebug() << __func__ << " - 'trashResult': " << trashResult;
    }

    // get the note file to rename it
    QFile file(fullNoteFilePath());

    // store the new note file name
    _fileName = std::move(newFileName);
    _name = std::move(newName);
    store();

    // rename the note file name
    return file.rename(fullNoteFilePath());
}

/**
 * Removes the file of the note
 *
 * @return
 */
bool Note::removeNoteFile() {
    if (this->fileExists()) {
        if (TrashItem::isLocalTrashEnabled()) {
            // add note to trash
            bool trashResult = TrashItem::add(this);
            qDebug() << __func__ << " - 'trashResult': " << trashResult;
        }

        QFile file(fullNoteFilePath());
        qDebug() << __func__ << " - 'this->fileName': " << this->_fileName;
        qDebug() << __func__ << " - 'file': " << file.fileName();
        return file.remove();
    }

    return false;
}

/**
 * @brief Returns html rendered Markdown of the note text
 * @param notesPath for transforming relative local urls to absolute ones
 * @param maxImageWidth defined maximum image width (ignored if forExport is
 * true)
 * @param forExport defines whether the export or preview stylesheet
 * @return
 */
QString Note::toMarkdownHtml(const QString &notesPath, int maxImageWidth, bool forExport,
                             bool decrypt, bool base64Images) {
    // get the decrypted note text (or the normal note text if there isn't any)
    const QString str = decrypt ? fetchDecryptedNoteText() : getNoteText();

    // create a hash of the note text and the parameters
    const QString toHash =
        str + QString::number(maxImageWidth) + (forExport ? QChar('1') : QChar('0')) +
        (decrypt ? QChar('1') : QChar('0')) + (base64Images ? QChar('1') : QChar('0'));
    const QString hash =
        QString(QCryptographicHash::hash(toHash.toLocal8Bit(), QCryptographicHash::Sha1).toHex());

    // check if the hash changed, if not return the old note text html
    if (hash == _noteTextHtmlConversionHash) {
        return _noteTextHtml;
    }

    const QString result =
        textToMarkdownHtml(std::move(str), notesPath, maxImageWidth, forExport, base64Images);

    // cache the html output and conversion hash
    _noteTextHtmlConversionHash = std::move(hash);
    _noteTextHtml = std::move(result);

    return _noteTextHtml;
}

static void captureHtmlFragment(const MD_CHAR *data, MD_SIZE data_size, void *userData) {
    QByteArray *array = static_cast<QByteArray *>(userData);

    if (data_size > 0) {
        array->append(data, int(data_size));
    }
}

/**
 * @brief Converts code blocks to highlighted code
 */
static void highlightCode(QString &str, const QString &type, int cbCount) {
    if (cbCount >= 1) {
        const int firstBlock = str.indexOf(type, 0);
        int currentCbPos = firstBlock;
        for (int i = 0; i < cbCount; ++i) {
            // find endline
            const int endline = str.indexOf(QChar('\n'), currentCbPos);
            // something invalid? => just skip it
            if (endline == -1) {
                break;
            }

            if (currentCbPos >= 4) {
                bool fourSpaces =
                    std::all_of(str.cbegin() + (currentCbPos - 4), str.cbegin() + currentCbPos,
                                [](QChar c) { return c == QChar(' '); });
                if (fourSpaces) {
                    continue;
                }
            }

            const QString lang = str.mid(currentCbPos + 3, endline - (currentCbPos + 3));
            // we skip it because it is inline code and not codeBlock
            if (lang.contains(type)) {
                int nextEnd = str.indexOf(type, currentCbPos + 3);
                nextEnd += 3;
                currentCbPos = str.indexOf(type, nextEnd);
                continue;
            }
            // move start pos to after the endline
            currentCbPos = endline + 1;
            // find the codeBlock end
            int next = str.indexOf(type, currentCbPos);
            if (next == -1) {
                break;
            }
            // extract the codeBlock
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 2)
            const QStringRef codeBlock = str.midRef(currentCbPos, next - currentCbPos);
#else
            QStringView str_view = str;
            QStringView codeBlock = str_view.mid(currentCbPos, next - currentCbPos);
#endif
            QString highlightedCodeBlock;
            if (!(codeBlock.isEmpty() && lang.isEmpty())) {
                const CodeToHtmlConverter c(lang);
                highlightedCodeBlock = c.process(codeBlock);
                // take care of the null char
                highlightedCodeBlock.replace(QChar('\u0000'), QLatin1String(""));
                str.replace(currentCbPos, next - currentCbPos, highlightedCodeBlock);
                // recalculate next because string has now changed
                next = str.indexOf(type, currentCbPos);
            }
            // move next pos to after the backticks
            next += 3;
            // find the start of the next code block
            currentCbPos = str.indexOf(type, next);
        }
    }
}

static inline int nonOverlapCount(const QString &str, const QChar c = '`') {
    const auto len = str.length();
    int count = 0;
    for (int i = 0; i < len; ++i) {
        if (str[i] == c && i + 2 < len && str[i + 1] == c && str[i + 2] == c) {
            ++count;
            i += 2;
        }
    }
    return count;
}

struct ImageSize {
    QString fileName;
    int size;
};
static std::vector<ImageSize> *getImageSizeCache() {
    static std::vector<ImageSize> _imageSizesCache;
    if (_imageSizesCache.size() > 100) _imageSizesCache.erase(_imageSizesCache.begin());
    return &_imageSizesCache;
}

/**
 * Converts a Markdown string for a note to html
 *
 * @param str
 * @param notesPath
 * @param maxImageWidth
 * @param forExport
 * @param base64Images
 * @return
 */
QString Note::textToMarkdownHtml(QString str, const QString &notesPath, int maxImageWidth,
                                 bool forExport, bool base64Images) {
    // MD4C flags
    unsigned flags =
        MD_DIALECT_GITHUB | MD_FLAG_WIKILINKS | MD_FLAG_LATEXMATHSPANS | MD_FLAG_UNDERLINE;
    // we parse the task lists ourselves

    // we render checkboxes when using qlitehtml
#ifndef USE_QLITEHTML
    flags &= ~MD_FLAG_TASKLISTS;
#endif

    const SettingsService settings;
    if (!settings.value(QStringLiteral("MainWindow/noteTextView.underline"), true).toBool()) {
        flags &= ~MD_FLAG_UNDERLINE;
    }

    QString windowsSlash = QLatin1String("");

#ifdef Q_OS_WIN32
    // we need another slash for Windows
    windowsSlash = "/";
#endif

    // remove frontmatter from Markdown text
    if (str.startsWith(QLatin1String("---"))) {
        static const QRegularExpression re(
            QStringLiteral(
                R"(^---((\r\n)|(\n\r)|\r|\n).+?((\r\n)|(\n\r)|\r|\n)---((\r\n)|(\n\r)|\r|\n))"),
            QRegularExpression::DotMatchesEverythingOption);
        str.remove(re);
    }

    // parse for relative file urls and make them absolute
    // (for example to show images under the note path)
    static const QRegularExpression re(QStringLiteral(R"(([\(<])file:\/\/([^\/].+?)([\)>]))"));
    str.replace(re, QStringLiteral("\\1file://") + windowsSlash +
                        QRegularExpression::escape(notesPath) + QStringLiteral("/\\2\\3"));

    // transform images without "file://" urls to file-urls (but we better do
    // that in the html, not the Markdown!)
    //    str.replace(
    //            QRegularExpression(R"((\!\[.*\]\()((?!file:\/\/).+)(\)))"),
    //            "\\1file://" + windowsSlash +
    //            QRegularExpression::escape(notesPath)
    //            + "/\\2\\3");

    QRegularExpressionMatchIterator i;

    // Try to replace links like <my-note.md> or <file.pdf> with proper file
    // links We need to do that in the Markdown because Hoedown would not create
    // a link tag This is a "has not '\w+:\/\/' in it" regular expression see:
    // http://stackoverflow.com/questions/406230/regular-expression-to-match-line-that-doesnt-contain-a-word
    // TODO: maybe we could do that per QTextBlock to check if it's done in comment blocks?
    // Important: The `\n` is needed to not crash under Windows if there is just
    // an opening `<` and a lot of other text after it
    // Note: If we find an `@` in the link we don't replace it because it's an email address
    static const QRegularExpression linkRE(
        QStringLiteral("<(((?!\\w+:\\/\\/)[^\\*<>@\n])+\\.[\\w\\d]+)>"));
    i = linkRE.globalMatch(str);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        const QString fileLink = match.captured(1);
        const QString url = Note::getFileURLFromFileName(fileLink, true, true);

        str.replace(match.captured(0), QStringLiteral("[") + fileLink + QStringLiteral("](") + url +
                                           QStringLiteral(")"));
    }

    // Try to replace links like [my note](my-note.md) or [file](file.md) with
    // proper file links This is currently also is handling relative image and
    // attachment links! We are using `{1,500}` instead of `+` because there
    // were crashes with regular expressions running wild
    // TODO: In theory we could convert relative note links in the html (and not
    // in the Markdown) to prevent troubles with code blocks
    i = QRegularExpression(QStringLiteral(R"(\[(.*?)\]\((((?!\w+:\/\/)[^<>]){1,500}?)\))"))
            .globalMatch(str);

    while (i.hasNext()) {
        const QRegularExpressionMatch match = i.next();
        const QString fileText = match.captured(1);
        const QString fileLink = match.captured(2);

        // ignore links to remote Nextcloud images
        if (fileLink.startsWith(QLatin1String("/core/preview"))) {
            continue;
        }

        const QString url = Note::getFileURLFromFileName(fileLink, true, true);

        str.replace(match.captured(0), QStringLiteral("[") + fileText + QStringLiteral("](") + url +
                                           QStringLiteral(")"));
    }

    // check if there is a script that wants to modify the Markdown
    const QString preScriptResult =
        ScriptingService::instance()->callPreNoteToMarkdownHtmlHook(this, str, forExport);

    if (!preScriptResult.isEmpty()) {
        str = std::move(preScriptResult);
    }

    /*CODE HIGHLIGHTING*/
    int cbCount = nonOverlapCount(str, '`');
    if (cbCount % 2 != 0) --cbCount;

    int cbTildeCount = nonOverlapCount(str, '~');
    if (cbTildeCount % 2 != 0) --cbTildeCount;

    // divide by two to get actual number of code blocks
    cbCount /= 2;
    cbTildeCount /= 2;

    // this will also add html in the code blocks, so we will do this at the very end
    highlightCode(str, QStringLiteral("```"), cbCount);
    highlightCode(str, QStringLiteral("~~~"), cbTildeCount);

    const auto data = str.toUtf8();
    if (data.size() == 0) {
        return QLatin1String("");
    }

    QByteArray array;
    const int renderResult =
        md_html(data.data(), MD_SIZE(data.size()), &captureHtmlFragment, &array, flags, 0);

    QString result;
    if (renderResult == 0) {
        result = QString::fromUtf8(array);
    } else {
        qWarning() << "MD4C Failure!";
        return QString();
    }

    // transform remote preview image tags
    Utils::Misc::transformRemotePreviewImages(result, maxImageWidth, externalImageHash());

    if (OwnCloudService::isOwnCloudSupportEnabled()) {
        // transform Nextcloud preview image tags
        Utils::Misc::transformNextcloudPreviewImages(result, maxImageWidth, externalImageHash());
    }

    // transform images without "file://" urls to file-urls
    // Note: this is currently handled above in Markdown
    //       if we want to activate this code again we need to take care of
    //       remote http(s) links to images! see:
    //       https://github.com/pbek/QOwnNotes/issues/1286
    /*
        const QString subFolderPath = getNoteSubFolder().relativePath("/");
        const QString notePath = notesPath + (subFolderPath.isEmpty() ? "" : "/"
       + subFolderPath); result.replace( QRegularExpression(R"((<img
       src=\")((?!file:\/\/).+)\")"),
                "\\1file://" + windowsSlash + notePath + "/\\2\"");
    */

    const QString fontString = Utils::Misc::previewCodeFontString();

    // set the stylesheet for the <code> blocks
    QString codeStyleSheet = QLatin1String("");
    if (!fontString.isEmpty()) {
        // set the note text view font
        QFont font;
        font.fromString(fontString);

        // add the font for the code block
        codeStyleSheet =
            QStringLiteral("pre, code { %1; }").arg(Utils::Schema::encodeCssFont(font));

        // ignore code font size to allow zooming (#1202)
        if (settings.value(QStringLiteral("MainWindow/noteTextView.ignoreCodeFontSize"), true)
                .toBool()) {
            codeStyleSheet.remove(QRegularExpression(QStringLiteral(R"(font-size: \d+\w+;)")));
        }
    }

    const bool darkModeColors =
        !forExport && settings.value(QStringLiteral("darkModeColors")).toBool();

    const QString codeForegroundColor =
        darkModeColors ? QStringLiteral("#ffffff") : QStringLiteral("#000000");
    const QString codeBackgroundColor =
        darkModeColors ? QStringLiteral("#444444") : QStringLiteral("#f1f1f1");

    // do some more code formatting
    // the "pre" styles are for the full-width code block background color
    codeStyleSheet += QString(
                          "pre { display: block; background-color: %1;"
                          " white-space: pre-wrap } "
                          "code { padding: 3px; overflow: auto;"
                          " line-height: 1.45em; background-color: %1;"
                          " border-radius: 5px; color: %2; }")
                          .arg(codeBackgroundColor, codeForegroundColor);

    // TODO: We should probably make a stylesheet for this
    codeStyleSheet += QStringLiteral(" .code-comment { color: #75715E;}");
    codeStyleSheet += QStringLiteral(" .code-string { color: #E6DB74;}");
    codeStyleSheet += QStringLiteral(" .code-literal { color: #AE81FF;}");
    codeStyleSheet += QStringLiteral(" .code-type { color: #66D9EF;}");
    codeStyleSheet += QStringLiteral(" .code-builtin { color: #A6E22E;}");
    codeStyleSheet += QStringLiteral(" .code-keyword { color: #F92672;}");
    codeStyleSheet += QStringLiteral(" .code-other { color: #F92672;}");

    // correct the strikeout tag
    result.replace(QRegularExpression(QStringLiteral("<del>([^<]+)<\\/del>")),
                   QStringLiteral("<s>\\1</s>"));
    const bool rtl = settings.value(QStringLiteral("MainWindow/noteTextView.rtl")).toBool();
    const QString rtlStyle =
        rtl ? QStringLiteral("body {text-align: right; direction: rtl;}") : QLatin1String("");

    if (forExport) {
        // get defined body font from settings
        const QString bodyFontString = Utils::Misc::previewFontString();

        // create export stylesheet
        QString exportStyleSheet = QLatin1String("");
        if (!bodyFontString.isEmpty()) {
            QFont bodyFont;
            bodyFont.fromString(bodyFontString);

            exportStyleSheet =
                QStringLiteral("body { %1; }").arg(Utils::Schema::encodeCssFont(bodyFont));
        }

        result = QString(
                     "<html><head><meta charset=\"utf-8\"/><style>"
                     "h1 { margin: 5px 0 20px 0; }"
                     "h2, h3 { margin: 10px 0 15px 0; }"
                     "img { max-width: 100%; }"
                     "pre { background-color: %5; border-radius: 5px; padding: "
                     "10px; }"
                     "pre > code { padding: 0; }"
                     "table {border-spacing: 0; border-style: solid; "
                     "border-width: 1px; "
                     "border-collapse: collapse; margin-top: 0.5em;}"
                     "th, td {padding: 2px 5px;}"
                     "a { color: #FF9137; text-decoration: none; } %1 %2 %4"
                     "</style></head><body class=\"export\">%3</body></html>")
                     .arg(codeStyleSheet, exportStyleSheet, result, rtlStyle, codeBackgroundColor);

        // remove trailing newline in code blocks
        result.replace(QStringLiteral("\n</code>"), QStringLiteral("</code>"));
    } else {
        const QString schemaStyles = Utils::Misc::isPreviewUseEditorStyles()
                                         ? Utils::Schema::getSchemaStyles()
                                         : QLatin1String("");

        // for preview
        result = QStringLiteral(
                     "<html><head><style>"
                     "h1 { margin: 5px 0 20px 0; }"
                     "h2, h3 { margin: 10px 0 15px 0; }"
                     "table {border-spacing: 0; border-style: solid; border-width: "
                     "1px; border-collapse: collapse; margin-top: 0.5em;}"
                     "th, td {padding: 2px 5px;}"
                     "a { color: #FF9137; text-decoration: none; } %1 %3 %4"
                     "</style></head><body class=\"preview\">%2</body></html>")
                     .arg(codeStyleSheet, result, rtlStyle, schemaStyles);
        // remove trailing newline in code blocks
        result.replace(QStringLiteral("\n</code>"), QStringLiteral("</code>"));
    }

    // check if width of embedded local images is too high
    static const QRegularExpression imgRE(QStringLiteral("<img src=\"(file:\\/\\/[^\"]+)\""));
    i = imgRE.globalMatch(result);

    auto getImageSizeFromCache = [](const QString &image) {
        auto cache = getImageSizeCache();
        auto it = std::find_if(cache->begin(), cache->end(),
                               [image](const ImageSize &i) { return i.fileName == image; });
        return it == cache->end() ? -1 : it->size;
    };

    while (i.hasNext()) {
        const QRegularExpressionMatch match = i.next();
        const QString fileUrl = match.captured(1);
        const QString fileName = QUrl(fileUrl).toLocalFile();

        int imageWidth = 0;

        // If file is greater than 1MB just limit its width already
        constexpr int OneMB = 1000 * 1000;
        if (QFileInfo(fileName).size() > OneMB) {
            imageWidth = maxImageWidth;
        }

        // try the cache
        if (imageWidth == 0) {
            imageWidth = getImageSizeFromCache(fileName);
        }

        // get image size using QImage and cache it
        if (imageWidth == -1) {
            const QImage image(fileName);
            imageWidth = image.width();
            getImageSizeCache()->push_back({fileName, imageWidth});
        }

        auto fileNameWithPercentSpaces = fileName;
        fileNameWithPercentSpaces.replace(QChar(' '), QStringLiteral("%20"));

        if (forExport) {
            result.replace(QRegularExpression(QStringLiteral(R"(<img src="file:\/\/)") +
                                              QRegularExpression::escape(
                                                  windowsSlash + fileNameWithPercentSpaces) +
                                              QStringLiteral("\"")),
                           QStringLiteral("<img src=\"file://%2\"").arg(windowsSlash + fileName));
        } else {
            // for preview
            // cap the image width at maxImageWidth (note text view width)
            const int originalWidth = imageWidth;
            const int displayWidth =
                (originalWidth > maxImageWidth) ? maxImageWidth : originalWidth;
            const QString filePattern = QStringLiteral(R"(<img src="file://)") + windowsSlash +
                                        fileNameWithPercentSpaces + QChar('"');

            result.replace(filePattern,
                           QStringLiteral(R"(<img width="%1" src="file://%2")")
                               .arg(QString::number(displayWidth), windowsSlash + fileName));
        }

        // encode the image base64
        if (base64Images) {
            QFile file(fileName);

            if (!file.open(QIODevice::ReadOnly)) {
                qWarning() << QObject::tr("Could not read image file: %1").arg(fileName);

                continue;
            }

            QMimeDatabase db;
            const QMimeType type = db.mimeTypeForFile(file.fileName());
            const QByteArray ba = file.readAll();

            result.replace(QRegularExpression(QStringLiteral("<img(.+?)src=\"") +
                                              QRegularExpression::escape(fileUrl) + QChar('"')),
                           QStringLiteral(R"(<img\1src="data:%1;base64,%2")")
                               .arg(type.name(), QString(ba.toBase64())));
        }
    }

    // check if there is a script that wants to modify the content
    const QString scriptResult =
        ScriptingService::instance()->callNoteToMarkdownHtmlHook(this, result, forExport);

    if (!scriptResult.isEmpty()) {
        result = scriptResult;
    }

    //    qDebug() << __func__ << " - 'result': " << result;
    return result;
}

/**
 * Returns the global external image hash instance
 */
Utils::Misc::ExternalImageHash *Note::externalImageHash() {
    auto *instance = qApp->property("externalImageHash").value<Utils::Misc::ExternalImageHash *>();

    if (instance == nullptr) {
        instance = new Utils::Misc::ExternalImageHash;

        qApp->setProperty("externalImageHash",
                          QVariant::fromValue<Utils::Misc::ExternalImageHash *>(instance));
    }

    return instance;
}

bool Note::isFetched() const { return (this->_id > 0); }

/**
 * @brief Generates a text that can be used in a link
 * @param text
 * @return
 */
QString Note::generateTextForLink(QString text) {
    // replace everything but characters and numbers with "_"
    // we want to treat unicode characters as normal characters
    // to support links to notes with unicode characters in their names
    static const QRegularExpression re(
        QStringLiteral("[^\\d\\w]"),
        QRegularExpression::CaseInsensitiveOption | QRegularExpression::UseUnicodePropertiesOption);
    text.replace(re, QStringLiteral("_"));

    // if there are only numbers we also want the "@" added, because
    // otherwise the text will get interpreted as ip address
    static const QRegularExpression re1 = QRegularExpression(QStringLiteral(R"(^(\d+)$)"));
    const QRegularExpressionMatch match = re1.match(text);
    bool addAtSign = match.hasMatch();

    if (!addAtSign) {
        // add a "@" if the text contains numbers and utf8 characters
        // because the url will be invalid then
        static const QRegularExpression re(QStringLiteral("\\d"));
        addAtSign = text.contains(re) && text.toLocal8Bit().size() != text.length();
    }

    // if the hostname of the url will get to long QUrl will not
    // recognize it because of STD 3 rules, so we will use the
    // username for the note name instead of the hostname
    // the limit is 63 characters, but special characters use up
    // far more space
    if (addAtSign || text.length() > 46) {
        text += QChar('@');
    }

    return text;
}

/**
 * Generates a qint64 hash from a QString
 */
qint64 Note::qint64Hash(const QString &str) {
    const QByteArray hash = QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Md5);
    Q_ASSERT(hash.size() == 16);
    QDataStream stream(hash);
    qint64 a, b;
    stream >> a >> b;
    return a ^ b;
}

/**
 * Calculates SHA256 checksum of text
 */
QString Note::calculateChecksum(const QString &text) {
    const QByteArray hash = QCryptographicHash::hash(text.toUtf8(), QCryptographicHash::Sha256);
    return QString::fromLatin1(hash.toHex());
}

/**
 * Encrypts the note text with the note's crypto key
 */
QString Note::encryptNoteText() {
    if (_noteText.isEmpty()) {
        return _noteText;
    }

    // split the text into a string list
    QStringList noteTextLines = getNoteTextLines();
    const int noteTextLinesCount = noteTextLines.count();

    // keep the first two lines unencrypted
    _noteText = noteTextLines.at(0) + QStringLiteral("\n");

    if (noteTextLinesCount > 1) {
        _noteText += noteTextLines.at(1) + QStringLiteral("\n");
    }

    _noteText += QStringLiteral("\n") + QStringLiteral(NOTE_TEXT_ENCRYPTION_PRE_STRING) +
                 QStringLiteral("\n");

    // remove the first two lines for encryption
    noteTextLines.removeFirst();

    if (noteTextLinesCount > 1) {
        noteTextLines.removeFirst();
    }

    // remove the 3rd line too if it is empty
    if (noteTextLinesCount > 2 && noteTextLines.at(0).isEmpty()) {
        noteTextLines.removeFirst();
    }

    // join the remaining lines
    QString text = noteTextLines.join(QStringLiteral("\n"));

    // empty notes will be detected as "can't be decrypted",
    // so we will add a space
    if (text.isEmpty()) {
        text = QStringLiteral(" ");
    }

    // check if we have an external encryption method
    QString encryptedText =
        ScriptingService::instance()->callEncryptionHook(text, _cryptoPassword, false);

    // check if a hook changed the text
    if (encryptedText.isEmpty()) {
        // fallback to Botan
        // encrypt the text
        BotanWrapper botanWrapper;
        botanWrapper.setPassword(_cryptoPassword);
        botanWrapper.setSalt(QStringLiteral(BOTAN_SALT));
        encryptedText = botanWrapper.Encrypt(text);

        //    SimpleCrypt *crypto = new
        //    SimpleCrypt(static_cast<quint64>(cryptoKey)); QString
        //    encryptedText = crypto->encryptToString(text);
    }

    // add the encrypted text to the new note text
    _noteText +=
        encryptedText + QStringLiteral("\n") + QStringLiteral(NOTE_TEXT_ENCRYPTION_POST_STRING);

    // store note
    store();

    return _noteText;
}

/**
 * Splits the text into a string list
 *
 * @return
 */
QStringList Note::getNoteTextLines() const {
    static const QRegularExpression re(QStringLiteral(R"((\r\n)|(\n\r)|\r|\n)"));
    return _noteText.split(re);
}

/**
 * Returns the regular expression to match encrypted text
 */
QRegularExpression Note::getEncryptedNoteTextRegularExpression() {
    // match the encrypted string
    QRegularExpression re(
        QRegularExpression::escape(QStringLiteral(NOTE_TEXT_ENCRYPTION_PRE_STRING)) +
        QStringLiteral("\\s+(.+)\\s+") +
        QRegularExpression::escape(QStringLiteral(NOTE_TEXT_ENCRYPTION_POST_STRING)));

    re.setPatternOptions(QRegularExpression::MultilineOption |
                         QRegularExpression::DotMatchesEverythingOption);

    return re;
}

/**
 * Returns encrypted note text if it is encrypted
 */
QString Note::getEncryptedNoteText() const { return parseEncryptedNoteText(getNoteText()); }

QString Note::parseEncryptedNoteText(const QString &noteText) {
    // Get regular expression for the encrypted string
    static const QRegularExpression re = getEncryptedNoteTextRegularExpression();

    // Check if we have an encrypted note text and return it if so
    const QRegularExpressionMatch match = re.match(noteText);
    return match.hasMatch() ? match.captured(1) : QLatin1String("");
}

/**
 * Returns encrypted note text if it is encrypted
 */
bool Note::hasEncryptedNoteText() const { return !getEncryptedNoteText().isEmpty(); }

/**
 * Checks if note text can be decrypted
 */
bool Note::canDecryptNoteText() const {
    const QString encryptedNoteText = getEncryptedNoteText();

    if (encryptedNoteText.isEmpty()) {
        return false;
    }

    // check if we have an external decryption method
    QString decryptedNoteText =
        ScriptingService::instance()->callEncryptionHook(encryptedNoteText, _cryptoPassword, true);

    // check if a hook changed the text
    if (decryptedNoteText.isEmpty()) {
        // decrypt the note text with Botan
        BotanWrapper botanWrapper;
        botanWrapper.setPassword(_cryptoPassword);
        botanWrapper.setSalt(QStringLiteral(BOTAN_SALT));
        decryptedNoteText = botanWrapper.Decrypt(encryptedNoteText);

        // fallback to SimpleCrypt
        if (decryptedNoteText.isEmpty()) {
            auto *crypto = new SimpleCrypt(static_cast<quint64>(_cryptoKey));
            decryptedNoteText = crypto->decryptToString(encryptedNoteText);
            delete crypto;
        }
    }

    return !decryptedNoteText.isEmpty();
}

/**
 * Sets the password to generate the cryptoKey
 */
void Note::setCryptoPassword(const QString &password) {
    _cryptoKey = qint64Hash(password);
    _cryptoPassword = password;
}

/**
 * Returns the decrypted note text
 */
QString Note::getDecryptedNoteText() const { return _decryptedNoteText; }

/**
 * Fetches the decrypted note text if it is encrypted
 * The crypto key has to be set in the object
 */
QString Note::fetchDecryptedNoteText() const {
    // If there is "dirty data" it means that the encrypted note was recently
    // changed, but not stored yet
    // In that case we want to return the already decrypted text, because that
    // text is the most current one
    if (_hasDirtyData && !_decryptedNoteText.isEmpty()) {
        return _decryptedNoteText;
    }

    const QString encryptedNoteText = getEncryptedNoteText();

    if (encryptedNoteText.isEmpty()) {
        return getNoteText();
    }

    return getDecryptedNoteText(encryptedNoteText);
}

QString Note::getDecryptedNoteText(
    const QString &encryptedNoteText) const {    // check if we have an external decryption method
    QString noteText = getNoteText();

    QString decryptedNoteText =
        ScriptingService::instance()->callEncryptionHook(encryptedNoteText, _cryptoPassword, true);

    // Check if a hook changed the text
    if (decryptedNoteText.isEmpty()) {
        // Decrypt the note text
        BotanWrapper botanWrapper;
        botanWrapper.setPassword(_cryptoPassword);
        botanWrapper.setSalt(QStringLiteral(BOTAN_SALT));
        decryptedNoteText = botanWrapper.Decrypt(encryptedNoteText);

        // Fallback to SimpleCrypt
        if (decryptedNoteText.isEmpty()) {
            auto *crypto = new SimpleCrypt(static_cast<quint64>(_cryptoKey));
            decryptedNoteText = crypto->decryptToString(encryptedNoteText);
            delete crypto;
        }
    }

    if (decryptedNoteText.isEmpty()) {
        return noteText;
    }

    // Get regular expression for the encrypted string
    static const QRegularExpression re = getEncryptedNoteTextRegularExpression();

    // Replace the encrypted text with the decrypted text
    noteText.replace(re, decryptedNoteText);
    return noteText;
}

/**
 * Expire crypto keys in the database after 10min
 */
bool Note::expireCryptoKeys() {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    // 10min ago
    QDateTime expiryDate = QDateTime::currentDateTime();
    expiryDate = expiryDate.addSecs(-600);

    // reset expired crypto keys
    query.prepare(
        QStringLiteral("UPDATE note SET crypto_key = 0, crypto_password = '' WHERE "
                       "modified < :expiryDate AND crypto_key != 0"));
    query.bindValue(QStringLiteral(":expiryDate"), expiryDate);

    // on error
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    }

    return true;
}

/**
 * Counts all notes
 */
int Note::countAll() {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    query.prepare(QStringLiteral("SELECT COUNT(*) AS cnt FROM note"));

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        return query.value(QStringLiteral("cnt")).toInt();
    }

    return 0;
}

/**
 * Counts all notes by note sub folder id
 */
int Note::countByNoteSubFolderId(int noteSubFolderId, bool recursive) {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);
    QVector<int> noteSubFolderIdList;

    if (recursive) {
        noteSubFolderIdList = NoteSubFolder::fetchIdsRecursivelyByParentId(noteSubFolderId);
    } else {
        noteSubFolderIdList << noteSubFolderId;
    }

    QStringList idStringList;
    idStringList.reserve(noteSubFolderIdList.size());
    for (const int id : Utils::asConst(noteSubFolderIdList)) {
        idStringList << QString::number(id);
    }

    query.prepare(QStringLiteral("SELECT COUNT(*) AS cnt FROM note WHERE note_sub_folder_id "
                                 "IN (") +
                  idStringList.join(QChar(',')) + QChar(')'));

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        return query.value(QStringLiteral("cnt")).toInt();
    }

    return 0;
}

/**
 * Checks if the notes are the same (by file)
 *
 * @param note
 * @return
 */
bool Note::isSameFile(const Note &note) const {
    return (_id == note.getId()) && (_noteSubFolderId == note.getNoteSubFolderId());
}

/**
 * Finds other notes that link to the note with fileName via legacy note:// links or the
 * relative file links
 *
 * @param fileName
 * @return list of note ids
 */
QVector<int> Note::findBacklinkedNoteIds() const {
    QVector<int> noteIdList;

    // search for legacy links
    const QString linkText = getNoteURL(_name);
    noteIdList << searchInNotes(QChar('<') + linkText + QChar('>'), true);
    noteIdList << searchInNotes(QStringLiteral("](") + linkText + QStringLiteral(")"), true);

    // search for legacy links ending with "@"
    const QString altLinkText = Utils::Misc::appendIfDoesNotEndWith(linkText, QStringLiteral("@"));
    if (altLinkText != linkText) {
        noteIdList << searchInNotes(QChar('<') + altLinkText + QChar('>'), true);
        noteIdList << searchInNotes(QStringLiteral("](") + altLinkText + QChar(')'), true);
    }

    const auto noteList = Note::fetchAll();
    noteIdList.reserve(noteList.size());
    // search for links to the relative file path in all notes
    for (const Note &note : noteList) {
        const int noteId = note.getId();

        // we also want to search in the current note, but we want to skip the
        // search if we already have found it
        if (noteIdList.contains(noteId)) {
            continue;
        }

        // We now don't escape slashes in the relative file path, but previously we did,
        // so we need to search for both
        for (bool escapeSlashes : {true, false}) {
            const QString relativeFilePath =
                Note::urlEncodeNoteUrl(note.getFilePathRelativeToNote(*this), escapeSlashes);
            const QString noteText = note.getNoteText();

            // Search for links to the relative file path in note
            // The "#" is for notes with a fragment (link to heading in note)
            if (noteText.contains(QStringLiteral("<") + relativeFilePath + QStringLiteral(">")) ||
                noteText.contains(QStringLiteral("](") + relativeFilePath + QStringLiteral(")")) ||
                noteText.contains(QStringLiteral("](") + relativeFilePath + QStringLiteral("#"))) {
                noteIdList.append(note.getId());
            }
        }
    }

    // remove duplicates and return list
    // return noteIdList.toSet().toList();
    // QSet<int>(noteIdList.constBegin(), noteIdList.constEnd());
    std::sort(noteIdList.begin(), noteIdList.end());
    noteIdList.erase(std::unique(noteIdList.begin(), noteIdList.end()), noteIdList.end());
    return noteIdList;
}

LinkHit Note::findAndReturnLinkHit(const QString &text, const QString &pattern) {
    return text.contains(pattern) ? LinkHit(pattern, {}) : LinkHit();
}

QSet<LinkHit> Note::findAndReturnLinkHits(const QString &text, const QRegularExpression &regex) {
    QSet<LinkHit> linkHits;
    QRegularExpressionMatchIterator iterator = regex.globalMatch(text);

    while (iterator.hasNext()) {
        QRegularExpressionMatch match = iterator.next();
        linkHits.insert(LinkHit(match.captured(0), match.captured(1)));
    }

    return linkHits;
}

void Note::addTextToBacklinkNoteHashIfFound(const Note &note, const QString &pattern) {
    const LinkHit linkHit = findAndReturnLinkHit(note.getNoteText(), pattern);

    if (!linkHit.isEmpty()) {
        if (!_backlinkNoteHash.contains(note)) {
            _backlinkNoteHash.insert(note, {linkHit});
        } else {
            _backlinkNoteHash[note] << linkHit;
        }
    }
}

void Note::addTextToLinkedNoteHashIfFound(const Note &note, const QString &noteText,
                                          const QString &pattern) {
    const LinkHit linkHit = findAndReturnLinkHit(noteText, pattern);

    if (!linkHit.isEmpty()) {
        if (!_linkedNoteHash.contains(note)) {
            _linkedNoteHash.insert(note, {linkHit});
        } else {
            _linkedNoteHash[note] << linkHit;
        }
    }
}

void Note::addTextToBacklinkNoteHashIfFound(const Note &note, const QRegularExpression &pattern) {
    const auto linkHits = findAndReturnLinkHits(note.getNoteText(), pattern);

    if (!linkHits.isEmpty()) {
        if (!_backlinkNoteHash.contains(note)) {
            _backlinkNoteHash.insert(note, linkHits);
        } else {
            _linkedNoteHash[note].unite(linkHits);
        }
    }
}

void Note::addTextToLinkedNoteHashIfFound(const Note &note, const QString &noteText,
                                          const QRegularExpression &pattern) {
    const auto linkHits = findAndReturnLinkHits(noteText, pattern);

    if (!linkHits.isEmpty()) {
        if (!_linkedNoteHash.contains(note)) {
            _linkedNoteHash.insert(note, linkHits);
        } else {
            _linkedNoteHash[note].unite(linkHits);
        }
    }
}

/**
 * Finds notes that the current note is linking to
 *
 * @return Hash of notes and the link hits
 */
QHash<Note, QSet<LinkHit>> Note::findLinkedNotes(QVector<Note> noteList,
                                                 const QString &connectionName) {
    const auto noteText = getNoteText();
    _linkedNoteHash.clear();

    if (noteList.isEmpty()) {
        noteList = Note::fetchAll();
    }

    // Check all notes and look if the current note contains a link to those notes
    // We don't need to care about legacy links, because they don't know subfolders
    for (const Note &note : noteList) {
        const QString &relativePathToNote = getFilePathRelativeToNote(note, connectionName);

        // We now don't escape slashes in the relative file path, but previously we did,
        // so we need to search for both
        for (bool escapeSlashes : {true, false}) {
            const QString relativeFilePath =
                Note::urlEncodeNoteUrl(relativePathToNote, escapeSlashes);

            // Search for links to the relative file path in note
            // The "#" is for notes with a fragment (link to heading in note)
            addTextToLinkedNoteHashIfFound(
                note, noteText, QStringLiteral("<") + relativeFilePath + QStringLiteral(">"));
            addTextToLinkedNoteHashIfFound(
                note, noteText,
                QRegularExpression(QStringLiteral(R"(\[([^\[\]]+?)\]\()") +
                                       QRegularExpression::escape(relativeFilePath) +
                                       QStringLiteral(R"(\))"),
                                   QRegularExpression::MultilineOption));
            addTextToLinkedNoteHashIfFound(
                note, noteText,
                QRegularExpression(QStringLiteral(R"(\[([^\[\]]+?)\]\()") +
                                   QRegularExpression::escape(relativeFilePath) +
                                   QStringLiteral(R"(#.+\))")));
        }
    }

    return _linkedNoteHash;
}

QHash<Note, QSet<LinkHit>> Note::findReverseLinkNotes() {
    const QVector<int> noteIdList = this->findBacklinkedNoteIds();
    const int noteCount = noteIdList.count();

    if (noteCount == 0) {
        return {};
    }

    _backlinkNoteHash.clear();
    const QString noteUrl = getNoteURL(this->getName());

    for (const int noteId : noteIdList) {
        Note note = Note::fetch(noteId);

        if (!note.isFetched()) {
            continue;
        }

        // search legacy links
        addTextToBacklinkNoteHashIfFound(note, QStringLiteral("<") + noteUrl + QStringLiteral(">"));
        addTextToBacklinkNoteHashIfFound(
            note,
            QRegularExpression(QStringLiteral(R"(\[([^\[\]]+?)\]\()") +
                               QRegularExpression::escape(noteUrl) + QStringLiteral(R"(\))")));

        // search for legacy links ending with "@"
        const QString altLinkText =
            Utils::Misc::appendIfDoesNotEndWith(noteUrl, QStringLiteral("@"));
        if (altLinkText != noteUrl) {
            addTextToBacklinkNoteHashIfFound(
                note, QStringLiteral("<") + altLinkText + QStringLiteral(">"));
            addTextToBacklinkNoteHashIfFound(
                note, QStringLiteral("](") + altLinkText + QStringLiteral(")"));
        }

        // We now don't escape slashes in the relative file path, but previously we did,
        // so we need to search for both
        for (bool escapeSlashes : {true, false}) {
            const QString relativeFilePath =
                Note::urlEncodeNoteUrl(note.getFilePathRelativeToNote(*this), escapeSlashes);

            // Search for links to the relative file path in note
            // The "#" is for notes with a fragment (link to heading in note)
            addTextToBacklinkNoteHashIfFound(
                note, QStringLiteral("<") + relativeFilePath + QStringLiteral(">"));
            addTextToBacklinkNoteHashIfFound(
                note, QRegularExpression(QStringLiteral(R"(\[([^\[\]]+?)\]\()") +
                                             QRegularExpression::escape(relativeFilePath) +
                                             QStringLiteral(R"(\))"),
                                         QRegularExpression::MultilineOption));
            addTextToBacklinkNoteHashIfFound(
                note, QRegularExpression(QStringLiteral(R"(\[([^\[\]]+?)\]\()") +
                                         QRegularExpression::escape(relativeFilePath) +
                                         QStringLiteral(R"(#.+\))")));
        }
    }

    return _backlinkNoteHash;
}

/**
 * Returns a (legacy) url to a note
 *
 * @param baseName
 * @return
 */
QString Note::getNoteURL(const QString &baseName) {
    return QStringLiteral("note://") + generateTextForLink(baseName);
}

/**
 * Returns the note-id url to a note
 *
 * @return
 */
QString Note::getNoteIdURL() const {
    return QStringLiteral("noteid://note-") + QString::number(getId());
}

/**
 * Returns the url to a note from a file name
 *
 * @param fileName
 * @return
 */
const QString Note::getNoteURLFromFileName(const QString &fileName) {
    const QFileInfo info(fileName);
    // TODO: baseName() will cut names like "Note 2018-07-26T18.24.22.md" down
    // to "Note 2018-07-26T18"!
    return Note::getNoteURL(info.baseName());
}

/**
 * Returns the absolute file url from a relative file name
 *
 * @param fileName
 * @return
 */
QString Note::getFileURLFromFileName(QString fileName, bool urlDecodeFileName,
                                     bool withFragment) const {
    // Remove the url fragment from the filename
    const auto splitList = fileName.split(QChar('#'));
    fileName = splitList.at(0);
    const QString fragment = splitList.count() > 1 ? splitList.at(1) : "";

    if (urlDecodeFileName) {
        fileName = urlDecodeNoteUrl(fileName);
    }

    if (_noteSubFolderId > 0) {
        const NoteSubFolder noteSubFolder = getNoteSubFolder();
        if (noteSubFolder.isFetched()) {
            fileName.prepend(noteSubFolder.relativePath() + QStringLiteral("/"));
        }
    }

    const QString path = this->getFullFilePathForFile(fileName);
    QString url = QUrl::fromLocalFile(path).toEncoded();

    if (withFragment && !fragment.isEmpty()) {
        url += QStringLiteral("#") + fragment;
    }

    return url;
}

QString Note::getURLFragmentFromFileName(const QString &fileName) {
    const auto splitList = fileName.split(QChar('#'));
    const QString fragment = splitList.count() > 1 ? splitList.at(1) : "";

    return QUrl::fromPercentEncoding(fragment.toLocal8Bit());
}

/**
 * @brief Note::fetchByRelativeFileName fetches a note by a relative filename by
 * current note
 * @param fileName
 * @return
 */
Note Note::fetchByRelativeFileName(const QString &fileName) const {
    const QString url = getFileURLFromFileName(fileName);
    return fetchByFileUrl(QUrl(url));
}

bool Note::fileUrlIsNoteInCurrentNoteFolder(const QUrl &url) {
    if (url.scheme() != QStringLiteral("file")) {
        return false;
    }

    const QString path = url.toLocalFile();

    if (!path.startsWith(NoteFolder::currentLocalPath())) {
        return false;
    }

    QListIterator<QString> itr(noteFileExtensionList(QStringLiteral(".")));

    while (itr.hasNext()) {
        const auto fileExtension = itr.next();

        if (path.endsWith(fileExtension, Qt::CaseInsensitive)) {
            return true;
        }
    }

    return false;
}

bool Note::fileUrlIsExistingNoteInCurrentNoteFolder(const QUrl &url) {
    if (url.scheme() != QStringLiteral("file")) {
        return false;
    }

    const QString path = url.toLocalFile();
    if (!QFile(path).exists()) {
        return false;
    }

    return path.startsWith(NoteFolder::currentLocalPath()) &&
           path.endsWith(QLatin1String(".md"), Qt::CaseInsensitive);
}

QString Note::fileUrlInCurrentNoteFolderToRelativePath(const QUrl &url) {
    QString path = url.toLocalFile();
    qDebug() << __func__ << " - 'path': " << path;

    // translates the "a path/../another path" to "another path"
    // needed for Note::fetchByRelativeFilePath!
    const QFileInfo fileInfo(path);
#ifdef Q_OS_WIN32
    // Let's stay with "canonicalFilePath" on Windows in case there is any issue
    // in portable mode
    // path = fileInfo.canonicalFilePath();

    // fileInfo.canonicalFilePath() didn't work when creating new notes when clicking
    // on links to note that were not found (at least in non-portable mode)
    // See: https://github.com/pbek/QOwnNotes/issues/2899
    path = fileInfo.absoluteFilePath();
#else
    // Don't resolve symbolic links
    path = fileInfo.absoluteFilePath();
#endif

    qDebug() << __func__ << " - 'canonicalFilePath': " << path;

    return path.remove(
        Utils::Misc::appendIfDoesNotEndWith(NoteFolder::currentLocalPath(), QStringLiteral("/")));
}

/**
 * @brief Note::relativeFilePath returns the relative path of "path" in regard
 * to the path of the note
 * @param path
 * @return
 */
QString Note::relativeFilePath(const QString &path) const {
    const QDir dir(fullNoteFilePath());
    // for some reason there is a leading "../" too much
    static const QRegularExpression re(QStringLiteral(R"(^\.\.\/)"));
    return dir.relativeFilePath(path).remove(re);
}

/**
 * Handles the replacing of all note urls if the note was renamed or moved
 * (subfolder)
 *
 * @param oldNote
 * @return true if we had to change the current note
 */
bool Note::handleNoteMoving(Note oldNote) {
    const QVector<int> noteIdList = oldNote.findBacklinkedNoteIds();
    const int noteCount = noteIdList.count();
    bool result = false;

    // Handle incoming note links
    if (noteCount > 0) {
        result = handleBacklinkedNotesAfterMoving(oldNote, noteIdList);
    }

    // Handle outgoing note links (only needed if subfolder was changed)
    if (oldNote.getNoteSubFolderId() != getNoteSubFolderId()) {
        const auto linkedNoteHits = oldNote.findLinkedNotes();
        const int linkedNotesCount = linkedNoteHits.count();
        qDebug() << __func__ << " - 'linkedNoteHits': " << linkedNoteHits;

        if (linkedNotesCount > 0) {
            result |= handleLinkedNotesAfterMoving(linkedNoteHits);
        }
    }

    return result;
}

bool Note::handleBacklinkedNotesAfterMoving(const Note &oldNote, const QVector<int> &noteIdList) {
    const int noteCount = noteIdList.count();
    const QString oldUrl = getNoteURL(oldNote.getName());
    const QString newUrl = getNoteURL(_name);

    if (Utils::Gui::questionNoSkipOverride(
            nullptr, QObject::tr("Note file path changed"),
            QObject::tr("A change of the note path was detected. Would you "
                        "like to replace all occurrences of "
                        "<strong>%1</strong> links with "
                        "<strong>%2</strong> and links with filename "
                        "<strong>%3</strong> with <strong>%4</strong>"
                        " in <strong>%n</strong> note file(s)?",
                        "", noteCount)
                .arg(oldUrl, newUrl, oldNote.getFileName(), _fileName),
            QStringLiteral("note-replace-links")) == QMessageBox::Yes) {
        // replace the urls in all found notes
        for (const int noteId : noteIdList) {
            Note note = fetch(noteId);

            if (!note.isFetched()) {
                continue;
            }

            QString text = note.getNoteText();

            // replace legacy links with note://
            text.replace(QStringLiteral("<") + oldUrl + QStringLiteral(">"),
                         QStringLiteral("<") + newUrl + QStringLiteral(">"));
            text.replace(QStringLiteral("](") + oldUrl + QStringLiteral(")"),
                         QStringLiteral("](") + newUrl + QStringLiteral(")"));

            //
            // replace legacy links with note:// and ending @
            //
            if (!oldUrl.contains(QLatin1String("@"))) {
                text.replace(QStringLiteral("<") + oldUrl + QStringLiteral("@>"),
                             QStringLiteral("<") + newUrl + QStringLiteral(">"));
                text.replace(QStringLiteral("](") + oldUrl + QStringLiteral("@)"),
                             QStringLiteral("](") + newUrl + QStringLiteral(")"));
            }

            QString oldNoteRelativeFilePath = note.getFilePathRelativeToNote(oldNote);
            const QString relativeFilePath =
                urlEncodeNoteUrl(note.getFilePathRelativeToNote(*this));

            //
            // replace non-urlencoded relative file links to the note
            //
            text.replace(QStringLiteral("<") + oldNoteRelativeFilePath + QStringLiteral(">"),
                         QStringLiteral("<") + relativeFilePath + QStringLiteral(">"));
            text.replace(QStringLiteral("](") + oldNoteRelativeFilePath + QStringLiteral(")"),
                         QStringLiteral("](") + relativeFilePath + QStringLiteral(")"));
            text.replace(QStringLiteral("](") + oldNoteRelativeFilePath + QStringLiteral("#"),
                         QStringLiteral("](") + relativeFilePath + QStringLiteral("#"));

            //
            // replace url encoded relative file links to the note
            //
            oldNoteRelativeFilePath = urlEncodeNoteUrl(oldNoteRelativeFilePath);
            text.replace(QStringLiteral("<") + oldNoteRelativeFilePath + QStringLiteral(">"),
                         QStringLiteral("<") + relativeFilePath + QStringLiteral(">"));
            text.replace(QStringLiteral("](") + oldNoteRelativeFilePath + QStringLiteral(")"),
                         QStringLiteral("](") + relativeFilePath + QStringLiteral(")"));
            text.replace(QStringLiteral("](") + oldNoteRelativeFilePath + QStringLiteral("#"),
                         QStringLiteral("](") + relativeFilePath + QStringLiteral("#"));

            // if the current note was changed we need to make sure the
            // _noteText is updated
            if (note.getId() == _id) {
                _noteText = text;
            }

            note.storeNewText(std::move(text));
        }
    }

    // return true if we had to change the current note
    return noteIdList.contains(_id);
}

bool Note::handleLinkedNotesAfterMoving(const QHash<Note, QSet<LinkHit>> &linkedNoteHits) {
    const int noteCount = linkedNoteHits.count();
    if (Utils::Gui::questionNoSkipOverride(
            nullptr, QObject::tr("Note file path changed"),
            QObject::tr("A change of the note path was detected. Would you "
                        "like to replace all outgoing links to <strong>%n</strong> note file(s)?",
                        "", noteCount),
            QStringLiteral("note-replace-outgoing-links")) != QMessageBox::Yes) {
        return false;
    }

    QString noteText = getNoteText();
    bool changed = false;

    // Iterate over linkedNoteHits and update the links to the containing notes
    for (auto it = linkedNoteHits.begin(); it != linkedNoteHits.end(); ++it) {
        const Note &linkedNote = it.key();
        const QSet<LinkHit> &linkHits = it.value();

        for (const LinkHit &linkHit : linkHits) {
            const QString oldMarkdown = linkHit.markdown;
            const QString linkText = linkHit.text;
            const QString relativeFilePath =
                urlEncodeNoteUrl(getFilePathRelativeToNote(linkedNote));
            const QString newMarkdown = linkText.isEmpty()
                                            ? "<" + relativeFilePath + ">"
                                            : "[" + linkText + "](" + relativeFilePath + ")";

            if (noteText.contains(oldMarkdown)) {
                noteText.replace(oldMarkdown, newMarkdown);
                changed = true;
            }
        }
    }

    if (changed) {
        // At this time the note is not existing anymore in the database, so we need to store a new
        // note
        _id = 0;
        this->_noteText = std::move(noteText);
        this->_hasDirtyData = true;
        store();
        storeNoteTextFileToDisk();
    }

    return changed;
}

QSet<Note> Note::findBacklinks() const {
    const QVector<int> noteIdList = this->findBacklinkedNoteIds();
    const int noteCount = noteIdList.count();

    if (noteCount == 0) {
        return {};
    }

    QSet<Note> notes;

    for (const int noteId : noteIdList) {
        const Note linkedNote = Note::fetch(noteId);

        if (!linkedNote.isFetched()) {
            continue;
        }

        notes << linkedNote;
    }

    return notes;
}

/**
 * Creates a note headline from a name
 *
 * @param name
 * @return
 */
QString Note::createNoteHeader(const QString &name) {
    QString header = name.trimmed() + QStringLiteral("\n");
    const auto len = std::min<int>(name.length(), 40);
    header.reserve(len);
    header.append(QString(QChar('=')).repeated(len));
    header.append(QStringLiteral("\n\n"));
    return header;
}

/**
 * Returns the Markdown of the inserted media file into a note
 */
QString Note::getInsertMediaMarkdown(QFile *file, bool addNewLine, bool returnUrlOnly,
                                     QString title) const {
    // file->exists() is false on Arch Linux for QTemporaryFile!
    if (file->size() == 0) {
        return QLatin1String("");
    }

    QDir mediaDir(NoteFolder::currentMediaPath());

    // create the media folder if it doesn't exist
    if (!mediaDir.exists()) {
        mediaDir.mkpath(mediaDir.path());
    }

    const QFileInfo fileInfo(file->fileName());
    QString suffix = fileInfo.suffix();
    QMimeDatabase db;
    const QMimeType type = db.mimeTypeForFile(file->fileName());

    // try to detect the mime type of the file and use a proper file suffix
    if (type.isValid()) {
        const QStringList suffixes = type.suffixes();
        if (suffixes.count() > 0) {
            suffix = suffixes.at(0);
        }
    }

    bool useExistingFile = false;
    // check if image with the same name already exists in media folder
    if (Utils::Misc::fileNameExists(file->fileName(), mediaDir.path())) {
        // file->fileName() wields a path!
        auto fileHash = Utils::Misc::generateFileSha1Signature(file->fileName());
        auto newFileHash = Utils::Misc::generateFileSha1Signature(
            mediaDir.path() + QDir::separator() + Utils::Misc::fileNameForPath(file->fileName()));

        // check if files are binary identical and ask if we want to use the existing file
        if (fileHash == newFileHash &&
            Utils::Gui::question(
                nullptr, QObject::tr("Image file exists"),
                QObject::tr("Image file already exists in the media folder, "
                            "do you want to use the existing one instead of creating a new file?"),
                QStringLiteral("insert-media-use-existing-image")) == QMessageBox::Yes) {
            useExistingFile = true;
        }
    }

    // find a name for the new file
    const QString newFileName = useExistingFile ? Utils::Misc::fileNameForPath(file->fileName())
                                                : Utils::Misc::findAvailableFileName(
                                                      file->fileName(), mediaDir.path(), suffix);

    const QString newFilePath = mediaDir.path() + QDir::separator() + newFileName;

    if (!useExistingFile) {
        // copy the file to the media folder
        file->copy(newFilePath);

        QFile newFile(newFilePath);
        scaleDownImageFileIfNeeded(newFile);
    }

    auto mediaUrlString = mediaUrlStringForFileName(newFileName);

    // check if we only want to return the media url string
    if (returnUrlOnly) {
        return mediaUrlString;
    }

    if (title.isEmpty()) {
        title = fileInfo.baseName();
    }

    // return the image link
    // we add a "\n" in the end so that hoedown recognizes multiple images
    return QStringLiteral("![") + title + QStringLiteral("](") + mediaUrlString +
           QStringLiteral(")") + (addNewLine ? QStringLiteral("\n") : QLatin1String(""));
}

QString Note::mediaUrlStringForFileName(const QString &fileName) const {
    QString urlString = QLatin1String("");
    const SettingsService settings;

    if (settings.value(QStringLiteral("legacyLinking")).toBool()) {
        urlString = QStringLiteral("file://media/") + fileName;
    } else {
        const int depth = getNoteSubFolder().depth();

        for (int i = 0; i < depth; ++i) {
            urlString += QStringLiteral("../");
        }

        urlString += QStringLiteral("media/") + fileName;
    }

    // Make sure all spaces and brackets are escaped
    // For example this is important in file names like "image (1).png"
    urlString = Utils::Misc::encodeFilePath(urlString);

    return urlString;
}

QString Note::attachmentUrlStringForFileName(const QString &fileName) const {
    QString urlString = QLatin1String("");
    const SettingsService settings;

    if (settings.value(QStringLiteral("legacyLinking")).toBool()) {
        urlString = QStringLiteral("file://attachments/") + fileName;
    } else {
        const int depth = getNoteSubFolder().depth();

        for (int i = 0; i < depth; ++i) {
            urlString += QStringLiteral("../");
        }

        urlString += QStringLiteral("attachments/") + fileName;
    }

    // Make sure all spaces and brackets are escaped
    // For example this is important in file names like "text (1).txt"
    urlString = Utils::Misc::encodeFilePath(urlString);

    return urlString;
}

/**
 * Returns the Markdown of the inserted attachment file into a note
 */
QString Note::getInsertAttachmentMarkdown(QFile *file, QString title, bool returnUrlOnly,
                                          QString fileName) const {
    // file->exists() was false for QTemporaryFile, so we are using file->size() only
    if (file->size() > 0) {
        const QDir dir(NoteFolder::currentAttachmentsPath());

        // created the attachments folder if it doesn't exist
        if (!dir.exists()) {
            dir.mkpath(dir.path());
        }

        const QFileInfo fileInfo(file->fileName());
        const QString suffix = fileInfo.suffix();

        if (fileName.isEmpty()) {
            fileName = file->fileName();
        }

        // find an available filename for the new file
        const QString newFileName =
            Utils::Misc::findAvailableFileName(fileName, dir.path(), suffix);

        const QString newFilePath = dir.path() + QDir::separator() + newFileName;

        // copy the file to the attachments folder
        file->copy(newFilePath);

        QString attachmentUrlString = attachmentUrlStringForFileName(newFileName);

        // check if we only want to return the attachment url string
        if (returnUrlOnly) {
            return attachmentUrlString;
        }

        if (title.isEmpty()) {
            title = fileInfo.fileName();
        }

        // return the attachment link
        return QStringLiteral("[") + title + QStringLiteral("](") + attachmentUrlString +
               QStringLiteral(")");
    }

    return QLatin1String("");
}

/**
 * Downloads an url to the media folder and returns the Markdown code or the
 * url for it relative to the note
 *
 * @param url
 * @param returnUrlOnly
 * @return
 */
QString Note::downloadUrlToMedia(const QUrl &url, bool returnUrlOnly) {
    // try to get the suffix from the url
    QString suffix = url.toString()
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
                         .split(QStringLiteral("."), QString::SkipEmptyParts)
#else
                         .split(QStringLiteral("."), Qt::SkipEmptyParts)
#endif
                         .last();

    if (suffix.isEmpty()) {
        suffix = QStringLiteral("image");
    }

    // remove strings like "?b=16068071000" and non-characters from the suffix
    static const QRegularExpression re(QStringLiteral("\\?.+$"));
    static const QRegularExpression re1(QStringLiteral("[^a-zA-Z0-9]"));
    suffix.remove(re).remove(re1);

    QString text;
    QTemporaryFile *tempFile = new QTemporaryFile(QDir::tempPath() + QDir::separator() +
                                                  QStringLiteral("media-XXXXXX.") + suffix);

    if (tempFile->open()) {
        // download the image to the temporary file
        if (Utils::Misc::downloadUrlToFile(url, tempFile)) {
            // copy image to media folder and generate Markdown code for
            // the image
            text = getInsertMediaMarkdown(tempFile, true, returnUrlOnly);
        }
    }

    delete tempFile;

    return text;
}

/**
 * Imports an image from a base64 string and returns the Markdown code
 *
 * @param data
 * @param imageSuffix
 * @return
 */
QString Note::importMediaFromBase64(QString &data, QString imageSuffix) const {
    // if data still starts with base64 prefix remove it
    if (data.startsWith(QLatin1String("base64,"), Qt::CaseInsensitive)) {
        data = data.mid(6);
    }

    if (imageSuffix.isEmpty()) {
        imageSuffix = QLatin1String("dat");
    }

    // create a temporary file for the image
    auto *tempFile = new QTemporaryFile(QDir::tempPath() + QDir::separator() +
                                        QStringLiteral("media-XXXXXX.") + imageSuffix);

    if (!tempFile->open()) {
        delete tempFile;
        return QLatin1String("");
    }

    // write image to the temporary file
    tempFile->write(QByteArray::fromBase64(data.toLatin1()));

    // store the temporary image in the media folder and return the Markdown
    // code
    QString markdownCode = getInsertMediaMarkdown(tempFile);

    delete tempFile;

    return markdownCode;
}

/**
 * Tries to import a media file into the note and returns the code for the
 * Markdown image tag
 */
QString Note::importMediaFromDataUrl(const QString &dataUrl) {
    if (dataUrl.contains(QLatin1String("data:image/"), Qt::CaseInsensitive)) {
        QStringList parts = dataUrl.split(QLatin1String("data:image/"));

        if (parts.count() != 2) {
            return {};
        }

        parts = parts[1].split(QStringLiteral(";base64,"));
        if (parts.count() == 2) {
            auto mimeType = QLatin1String("image/") + parts[0];
            QString fileExtension = Utils::Misc::fileExtensionForMimeType(mimeType);

            return importMediaFromBase64(parts[1], fileExtension);
        }
    }

    return "";
}

/**
 * Scales down an image file if needed
 * The image file will be overwritten in the process
 *
 * @param file
 * @return
 */
bool Note::scaleDownImageFileIfNeeded(QFile &file) {
    const SettingsService settings;

    // load image scaling settings
    const bool scaleImageDown = settings.value(QStringLiteral("imageScaleDown"), false).toBool();

    if (!scaleImageDown) {
        return true;
    }

    QMimeDatabase db;
    QMimeType type = db.mimeTypeForFile(file.fileName());

    // we don't want to resize SVGs because Qt can't store them
    if (type.name().contains("image/svg")) {
        return true;
    }

    QImage image;

    if (!image.load(file.fileName())) {
        return false;
    }

    const int width = settings.value(QStringLiteral("imageScaleDownMaximumWidth"), 1024).toInt();
    const int height = settings.value(QStringLiteral("imageScaleDownMaximumHeight"), 1024).toInt();

    // don't scale if image is already small enough
    if (image.width() <= width && image.height() <= height) {
        return true;
    }

    const QPixmap &pixmap = QPixmap::fromImage(
        image.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << file.fileName();
        return false;
    }

    pixmap.save(&file);
    file.close();

    return true;
}

/**
 * Tries to fetch a note from an url string
 *
 * @param urlString
 * @return
 */
Note Note::fetchByUrlString(const QString &urlString) {
    const QUrl url{urlString};

    // if the name of the linked note only consists of numbers we cannot use
    // host() to get the filename, it would get converted to an ip-address
    const QRegularExpressionMatch match =
        QRegularExpression(QStringLiteral(R"(^\w+:\/\/(\d+)$)")).match(urlString);
    QString fileName = match.hasMatch() ? match.captured(1) : url.host();

    // we are using the user name as fallback if the hostname was too long
    if (!url.userName().isEmpty()) {
        fileName = url.userName();
    } else {
        if (fileName.isEmpty()) {
            return Note();
        }

        // add a ".com" to the filename to simulate a valid domain
        fileName += QStringLiteral(".com");

        // convert the ACE to IDN (internationalized domain names) to support
        // links to notes with unicode characters in their names
        // then remove the ".com" again
        fileName = Utils::Misc::removeIfEndsWith(QUrl::fromAce(fileName.toLatin1()),
                                                 QStringLiteral(".com"));

        // if it seem we have unicode characters in our filename let us use
        // wildcards for each number, because full width numbers get somehow
        // translated to normal numbers by the QTextEdit
        if (fileName != url.host()) {
            fileName.replace(QLatin1Char('1'), QStringLiteral("[1１]"))
                .replace(QLatin1Char('2'), QStringLiteral("[2２]"))
                .replace(QLatin1Char('3'), QStringLiteral("[3３]"))
                .replace(QLatin1Char('4'), QStringLiteral("[4４]"))
                .replace(QLatin1Char('5'), QStringLiteral("[5５]"))
                .replace(QLatin1Char('6'), QStringLiteral("[6６]"))
                .replace(QLatin1Char('7'), QStringLiteral("[7７]"))
                .replace(QLatin1Char('8'), QStringLiteral("[8８]"))
                .replace(QLatin1Char('9'), QStringLiteral("[9９]"))
                .replace(QLatin1Char('0'), QStringLiteral("[0０]"));
        }
    }

    // this makes it possible to search for file names containing spaces
    // instead of spaces a "-" has to be used in the note link
    // example: note://my-note-with-spaces-in-the-name
    fileName.replace(QChar('-'), QChar('?')).replace(QChar('_'), QChar('?'));

    // create a regular expression to search in sqlite note table
    QString escapedFileName = QRegularExpression::escape(fileName);
    escapedFileName.replace(QStringLiteral("\\?"), QStringLiteral("."));
    const QRegularExpression regExp =
        QRegularExpression(QLatin1Char('^') + escapedFileName + QLatin1Char('$'),
                           QRegularExpression::CaseInsensitiveOption);

    qDebug() << __func__ << " - 'regExp': " << regExp;

    const int noteSubFolderId = NoteSubFolder::activeNoteSubFolderId();
    Note note;

    qDebug() << __func__ << " - 'noteSubFolderId': " << noteSubFolderId;

    if (noteSubFolderId > 0) {
        note = Note::fetchByName(regExp, noteSubFolderId);
        qDebug() << __func__ << " - 'note in sub folder': " << note;
    }

    // if we haven't found a note we try searching in all note subfolders
    if (!note.isFetched()) {
        note = Note::fetchByName(regExp);
        qDebug() << __func__ << " - 'note in all sub folders': " << note;
    }

    return note;
}

/**
 * Generates the preview text of the note
 *
 * @return
 */
QString Note::getNotePreviewText(bool asHtml, int lines) const {
    QString noteText = getNoteText();

    // remove Windows line breaks
    static const QRegularExpression leRE(QStringLiteral("\r\n"));
    noteText.replace(leRE, QStringLiteral("\n"));

    if (!allowDifferentFileName()) {
        // remove headlines
        static const QRegularExpression re1(QStringLiteral("^.+\n=+\n+"));
        static const QRegularExpression re2(QStringLiteral("^# .+\n+"));
        noteText.remove(re1);
        noteText.remove(re2);
    }

    // remove multiple line breaks
    static const QRegularExpression re(QStringLiteral("\n\n+"));
    noteText.replace(re, QStringLiteral("\n"));

    const QStringList &lineList = noteText.split(QStringLiteral("\n"));

    if (lineList.isEmpty()) {
        return QLatin1String("");
    }

    noteText = QLatin1String("");

    // first line
    QString line = lineList.at(0).trimmed();
    line.truncate(80);
    noteText += line;

    const auto min = std::min<int>(lines, lineList.count());
    for (int i = 1; i < min; i++) {
        noteText += QStringLiteral("\n");

        QString line = lineList.at(i).trimmed();
        line.truncate(80);

        noteText += line;
    }

    if (asHtml) {
        noteText = Utils::Misc::htmlspecialchars(std::move(noteText));
        noteText.replace(QStringLiteral("\n"), QStringLiteral("<br>"));
    }

    return noteText;
}

/**
 * Generate the preview text if multiple notes are selected
 *
 * @param notes
 * @return
 */
QString Note::generateMultipleNotesPreviewText(const QVector<Note> &notes) {
    const SettingsService settings;
    const bool darkModeColors = settings.value(QStringLiteral("darkModeColors")).toBool();
    const QString oddBackgroundColor =
        darkModeColors ? QStringLiteral("#444444") : QStringLiteral("#f1f1f1");
    const QString linkColor =
        darkModeColors ? QStringLiteral("#eeeeee") : QStringLiteral("#222222");

    QString previewHtml = QStringLiteral(
                              "<html><head><style>"
                              "table, body {width: 100%;}"
                              "table td {padding: 10px}"
                              "table td.odd {background-color: ") +
                          oddBackgroundColor +
                          QStringLiteral(
                              ";}"
                              "p {margin: 0.5em 0 0 0;}"
                              "small {font-size: 0.8em;}"
                              "h2 {margin-bottom: 0.5em;}"
                              "h2 a {text-decoration: none; color: ") +
                          linkColor +
                          QStringLiteral(
                              "}"
                              "</style></head>"
                              "<body>"
                              "   <table>");

    const int notesCount = notes.count();
    const int displayedNotesCount = notesCount > 40 ? 40 : notesCount;

    bool isOdd = false;
    for (int i = 0; i < displayedNotesCount; i++) {
        const Note &note = notes.at(i);
        const QString oddStyle = isOdd ? QStringLiteral(" class='odd'") : QLatin1String("");
        const QDateTime modified = note.getFileLastModified();
        const QString noteText = note.getNotePreviewText(true, 5);
        const QString noteLink = note.getNoteIdURL();

        previewHtml += QStringLiteral("<tr><td") + oddStyle +
                       QStringLiteral(
                           ">"
                           "<h2><a href='") +
                       noteLink + QStringLiteral("'>") + note.getName() +
                       QStringLiteral(
                           "</a></h2>"
                           "<small>") +
                       modified.toString() +
                       QStringLiteral(
                           "</small>"
                           "<p>") +
                       noteText +
                       QStringLiteral(
                           "</p>"
                           "</td></tr>");
        isOdd = !isOdd;
    }

    if (displayedNotesCount < notesCount) {
        previewHtml += QStringLiteral("<tr><td>") +
                       QObject::tr("…and %n more note(s)", "", notesCount - displayedNotesCount) +
                       QStringLiteral("</td></tr>");
    }

    previewHtml += QStringLiteral(
        "   </table>"
        "</body>"
        "</html>");

    return previewHtml;
}

/**
 * Returns the parsed bookmarks of the note for the WebSocketServerService
 *
 * @return
 */
QString Note::getParsedBookmarksWebServiceJsonText() const {
    return Bookmark::bookmarksWebServiceJsonText(getParsedBookmarks());
}

/**
 * Returns the parsed bookmarks of the note
 *
 * @return
 */
QVector<Bookmark> Note::getParsedBookmarks() const {
    const QString text = _decryptedNoteText.isEmpty() ? _noteText : _decryptedNoteText;
    return Bookmark::parseBookmarks(text);
}

/**
 * Returns the parsed command snippets of the note
 *
 * @return
 */
QVector<CommandSnippet> Note::getParsedCommandSnippets() const {
    const QString text = _decryptedNoteText.isEmpty() ? _noteText : _decryptedNoteText;
    return CommandSnippet::parseCommandSnippets(text);
}

void Note::resetNoteTextHtmlConversionHash() { _noteTextHtmlConversionHash = QLatin1String(""); }

bool Note::applyIgnoredNotesSetting(QStringList &fileNames) {
    const SettingsService settings;
    const QStringList ignoredFileRegExpList =
        settings.value(QStringLiteral("ignoredNoteFiles")).toString().split(QLatin1Char(';'));

    if (ignoredFileRegExpList.isEmpty()) {
        return false;
    }

    auto newFileNames = QStringList();

    for (const QString &fileName : fileNames) {
        if (!Utils::Misc::regExpInListMatches(fileName, ignoredFileRegExpList)) {
            newFileNames.append(fileName);
        }
    }

    fileNames = newFileNames;

    return true;
}

/**
 * Fetches all tags of the note
 */
// QList<Tag> Note::tags() {
//    return Tag::fetchAllOfNote(this);
//}

QDebug operator<<(QDebug dbg, const Note &note) {
    const NoteSubFolder noteSubFolder = NoteSubFolder::fetch(note._noteSubFolderId);
    dbg.nospace() << "Note: <id>" << note._id << " <name>" << note._name << " <fileName>"
                  << note._fileName << " <noteSubFolderId>" << note._noteSubFolderId
                  << " <relativePath>" << noteSubFolder.relativePath() << " <hasDirtyData>"
                  << note._hasDirtyData;
    return dbg.space();
}

bool Note::operator==(const Note &note) const {
    return _id == note.getId() && _fileName == note.getFileName() &&
           _noteSubFolderId == note.getNoteSubFolderId();
}

QDebug operator<<(QDebug dbg, const LinkHit &hit) {
    dbg.nospace() << "LinkHit(markdown: " << hit.markdown << ", text: " << hit.text << ')';
    return dbg.space();
}
