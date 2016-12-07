#include "entities/note.h"
#include <QDebug>
#include <QSqlRecord>
#include <QMessageBox>
#include <QApplication>
#include <QSettings>
#include <QDir>
#include <QSqlError>
#include <QRegularExpression>
#include <QUrl>
#include <QCryptographicHash>
#include "libraries/simplecrypt/simplecrypt.h"
#include "libraries/hoedown/html.h"
#include "libraries/botan/botanwrapper.h"
#include "libraries/botan/botan.h"
#include "tag.h"
#include "notesubfolder.h"
#include <utils/misc.h>
#include <services/scriptingservice.h>


Note::Note() {
    this->id = 0;
    this->noteSubFolderId = 0;
    this->shareId = 0;
    this->hasDirtyData = false;
    this->fileSize = 0;
}

int Note::getId() {
    return this->id;
}

QString Note::getName() {
    return this->name;
}

QDateTime Note::getFileLastModified() {
    return this->fileLastModified;
}

QDateTime Note::getModified() {
    return this->modified;
}

qint64 Note::getCryptoKey() {
    return this->cryptoKey;
}

QString Note::getCryptoPassword() {
    return this->cryptoPassword;
}

QString Note::getShareUrl() {
    return this->shareUrl;
}

int Note::getShareId() {
    return this->shareId;
}

qint64 Note::getFileSize() {
    return this->fileSize;
}

bool Note::isShared() {
    return this->shareId > 0;
}

QString Note::getFileName() {
    return this->fileName;
}

NoteSubFolder Note::getNoteSubFolder() {
    return NoteSubFolder::fetch(this->noteSubFolderId);
}

int Note::getNoteSubFolderId() {
    return this->noteSubFolderId;
}

void Note::setNoteSubFolder(NoteSubFolder noteSubFolder) {
    setNoteSubFolderId(noteSubFolder.getId());
}

void Note::setNoteSubFolderId(int id) {
    this->noteSubFolderId = id;
}

QString Note::getNoteText() {
    return this->noteText;
}

void Note::setHasDirtyData(bool hasDirtyData) {
    this->hasDirtyData = hasDirtyData;
}

bool Note::getHasDirtyData() {
    return this->hasDirtyData;
}

void Note::setName(QString text) {
    this->name = text;
}

void Note::setShareUrl(QString url) {
    this->shareUrl = url;
}

void Note::setShareId(int id) {
    this->shareId = id;
}

void Note::setCryptoKey(qint64 cryptoKey) {
    this->cryptoKey = cryptoKey;
}

void Note::setNoteText(QString text) {
    this->noteText = text;
}

void Note::setDecryptedNoteText(QString text) {
    this->decryptedNoteText = text;
}

bool Note::addNote(QString name, QString fileName, QString text) {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    query.prepare(
            "INSERT INTO note ( name, file_name, note_text ) "
                    "VALUES ( :name, :file_name, :note_text )");
    query.bindValue(":name", name);
    query.bindValue(":file_name", fileName);
    query.bindValue(":note_text", text);
    return query.exec();
}

Note Note::fetch(int id) {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    Note note;

    query.prepare("SELECT * FROM note WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        if (query.first()) {
            note = noteFromQuery(query);
        }
    }

    return note;
}

Note Note::fetchByFileName(QString fileName, int noteSubFolderId) {
    Note note;

    // get the active note subfolder id if none was set
    if (noteSubFolderId == -1) {
        noteSubFolderId = NoteSubFolder::activeNoteSubFolderId();
    }

    note.fillByFileName(fileName, noteSubFolderId);
    return note;
}

bool Note::fillByFileName(QString fileName, int noteSubFolderId) {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    // get the active note subfolder id if none was set
    if (noteSubFolderId == -1) {
        noteSubFolderId = NoteSubFolder::activeNoteSubFolderId();
    }

    query.prepare("SELECT * FROM note WHERE file_name = :file_name AND "
                          "note_sub_folder_id = :note_sub_folder_id");
    query.bindValue(":file_name", fileName);
    query.bindValue(":note_sub_folder_id", noteSubFolderId);

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

bool Note::remove(bool withFile) {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    query.prepare("DELETE FROM note WHERE id = :id");
    query.bindValue(":id", this->id);

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
 * @brief Copies a note to an other path
 *
 * @param destinationPath
 * @return bool
 */
bool Note::copy(QString destinationPath) {
    QDir d;
    if (this->fileExists() && (d.exists(destinationPath))) {
        QFile file(fullNoteFilePath());
        QString destinationFileName =
                destinationPath + QDir::separator() + this->fileName;

        if (d.exists(destinationFileName)) {
            qDebug() << destinationFileName << "already exists!";

            // find a new filename for the note
            QDateTime currentDateTime = QDateTime::currentDateTime();
            destinationFileName =
                    destinationPath + QDir::separator() + this->name + " " +
                    currentDateTime.toString(Qt::ISODate).replace(":", "_") +
                    "." + defaultNoteFileExtension();

            qDebug() << "New file name:" << destinationFileName;
        }

        return file.copy(destinationFileName);
    }

    return false;
}

/**
 * @brief Moves a note to an other path
 *
 * @param destinationPath
 * @return bool
 */
bool Note::move(QString destinationPath) {
    bool result = this->copy(destinationPath);

    if (result) {
        return remove(true);
    }

    return false;
}

/**
 * Fetches a note by its share id
 *
 * @param shareId
 * @return
 */
Note Note::fetchByShareId(int shareId) {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);
    Note note;

    query.prepare("SELECT * FROM note WHERE share_id = :share_id");
    query.bindValue(":share_id", shareId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        if (query.first()) {
            note = noteFromQuery(query);
        }
    }

    return note;
}

Note Note::fetchByName(QString name, int noteSubFolderId) {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);
    Note note;

    // get the active note subfolder id if none was set
    if (noteSubFolderId == -1) {
        noteSubFolderId = NoteSubFolder::activeNoteSubFolderId();
    }

    query.prepare("SELECT * FROM note WHERE name = :name AND "
                          "note_sub_folder_id = :note_sub_folder_id");
    query.bindValue(":name", name);
    query.bindValue(":note_sub_folder_id", noteSubFolderId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        if (query.first()) {
            note = noteFromQuery(query);
        }
    }

    return note;
}

Note Note::noteFromQuery(QSqlQuery query) {
    Note note;
    note.fillFromQuery(query);
    return note;
}

bool Note::fillFromQuery(QSqlQuery query) {
    id = query.value("id").toInt();
    name = query.value("name").toString();
    fileName = query.value("file_name").toString();
    shareUrl = query.value("share_url").toString();
    shareId = query.value("share_id").toInt();
    noteSubFolderId = query.value("note_sub_folder_id").toInt();
    noteText = query.value("note_text").toString();
    decryptedNoteText = query.value("decrypted_note_text").toString();
    cryptoKey = query.value("crypto_key").toLongLong();
    fileSize = query.value("file_size").toLongLong();
    cryptoPassword = query.value("crypto_password").toString();
    hasDirtyData = query.value("has_dirty_data").toInt() == 1;
    fileCreated = query.value("file_created").toDateTime();
    fileLastModified = query.value("file_last_modified").toDateTime();
    created = query.value("created").toDateTime();
    modified = query.value("modified").toDateTime();

    return true;
}

QList<Note> Note::fetchAll(int limit) {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    QList<Note> noteList;
    QString sql = "SELECT * FROM note ORDER BY file_last_modified DESC";

    if (limit >= 0) {
        sql += " LIMIT :limit";
    }

    query.prepare(sql);

    if (limit >= 0) {
        query.bindValue(":limit", limit);
    }

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            Note note = noteFromQuery(query);
            noteList.append(note);
        }
    }

    return noteList;
}

QList<int> Note::fetchAllIds() {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    QList<int> noteIdList;
    QString sql = "SELECT * FROM note";

    query.prepare(sql);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            Note note = noteFromQuery(query);
            noteIdList.append(note.getId());
        }
    }

    return noteIdList;
}

QList<Note> Note::fetchAllByNoteSubFolderId(int noteSubFolderId) {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    QList<Note> noteList;
    QString sql = "SELECT * FROM note WHERE note_sub_folder_id = "
            ":note_sub_folder_id ORDER BY file_last_modified DESC";

    query.prepare(sql);
    query.bindValue(":note_sub_folder_id", noteSubFolderId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            Note note = noteFromQuery(query);
            noteList.append(note);
        }
    }

    return noteList;
}

/**
 * Gets a list of note ids from a note list
 */
QList<int> Note::noteIdListFromNoteList(QList<Note> noteList) {
    QListIterator<Note> itr(noteList);
    QList<int> idList;

    while (itr.hasNext()) {
        Note note = itr.next();
        idList << note.getId();
    }

    return idList;
}

/**
 * Returns all notes that are not tagged
 */
QList<Note> Note::fetchAllNotTagged() {
    QList<Note> noteList = Note::fetchAll();
    QList<Note> untaggedNoteList;

    QListIterator<Note> itr(noteList);

    while (itr.hasNext()) {
        Note note = itr.next();
        int tagCount = Tag::countAllOfNote(note);
        if (tagCount == 0) {
            untaggedNoteList << note;
        }
    }

    return untaggedNoteList;
}

/**
 * Returns all notes names that are not tagged
 */
QStringList Note::fetchAllNotTaggedNames() {
    QList<Note> noteList = Note::fetchAll();
    QStringList untaggedNoteFileNameList;

    QListIterator<Note> itr(noteList);

    while (itr.hasNext()) {
        Note note = itr.next();
        int tagCount = Tag::countAllOfNote(note);
        if (tagCount == 0) {
            untaggedNoteFileNameList << note.getName();
        }
    }

    return untaggedNoteFileNameList;
}

/**
 * Counts all notes that are not tagged
 */
int Note::countAllNotTagged() {
    QList<Note> noteList = Note::fetchAllNotTagged();
    return noteList.count();
}

QList<Note> Note::search(QString text) {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    QList<Note> noteList;

    query.prepare("SELECT * FROM note WHERE note_text LIKE :text "
                          "ORDER BY file_last_modified DESC");
    query.bindValue(":text", "%" + text + "%");

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            Note note = noteFromQuery(query);
            noteList.append(note);
        }
    }

    return noteList;
}

QList<QString> Note::searchAsNameList(QString text, bool searchInNameOnly) {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    QList<QString> nameList;
    QString textSearchSql = !searchInNameOnly ? "OR note_text LIKE :text " : "";
    int noteSubFolderId = NoteSubFolder::activeNoteSubFolderId();

    query.prepare("SELECT name FROM note WHERE (name LIKE :text " +
            textSearchSql + ") AND note_sub_folder_id = :note_sub_folder_id "
            "ORDER BY file_last_modified DESC");
    query.bindValue(":text", "%" + text + "%");
    query.bindValue(":note_sub_folder_id", noteSubFolderId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            nameList.append(query.value("name").toString());
        }
    }

    return nameList;
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
QList<int> Note::searchInNotes(QString search, bool ignoreNoteSubFolder,
                               int noteSubFolderId) {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);
    QList<int> noteIdList;
    QStringList sqlList;

    // get the active note subfolder id if none was set
    if ((noteSubFolderId == -1) && !ignoreNoteSubFolder) {
        noteSubFolderId = NoteSubFolder::activeNoteSubFolderId();
    }

    // build the string list of the search string
    QStringList queryStrings = buildQueryStringList(search);

    for (int i = 0; i < queryStrings.count(); i++) {
        sqlList.append("note_text LIKE ?");
    }

    QString sql;

    // build the query
    if (ignoreNoteSubFolder) {
        sql = "SELECT id FROM note WHERE " + sqlList.join(" AND ");
        query.prepare(sql);
    } else {
        sql = "SELECT id FROM note WHERE note_sub_folder_id = "
                      ":note_sub_folder_id AND " + sqlList.join(" AND ");
        query.prepare(sql);
        query.bindValue(0, noteSubFolderId);
    }

    // add the values to the query
    for (int i = 0; i < queryStrings.count(); i++) {
        int pos = ignoreNoteSubFolder ? i : i + 1;
        query.bindValue(pos, "%" + queryStrings[i] + "%");
    }

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            noteIdList.append(query.value("id").toInt());
        }
    }

    return noteIdList;
}

/**
 * Builds a string list of a search string
 */
QStringList Note::buildQueryStringList(QString searchString) {
    QStringList queryStrings;

    // check for strings in ""
    QRegularExpression re("\"([^\"]+)\"");
    QRegularExpressionMatchIterator i = re.globalMatch(searchString);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        queryStrings.append(match.captured(1));
        searchString.remove(match.captured(0));
    }

    // remove a possible remaining "
    searchString.remove("\"");
    // remove multiple spaces and spaces in front and at the end
    searchString = searchString.simplified();

    // add the remaining strings
    queryStrings.append(searchString.split(" "));

    // remove empty items, so the search will not run amok
    queryStrings.removeAll("");

    // remove duplicate query items
    queryStrings.removeDuplicates();

    return queryStrings;
}

QStringList Note::fetchNoteNames() {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    QStringList list;
    int noteSubFolderId = NoteSubFolder::activeNoteSubFolderId();

    query.prepare("SELECT DISTINCT(name) FROM note WHERE "
                          "note_sub_folder_id = :note_sub_folder_id "
                          "ORDER BY file_last_modified DESC");
    query.bindValue(":note_sub_folder_id", noteSubFolderId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            QString name = query.value("name").toString();
            if (!name.isEmpty()) {
                list.append(name);
            }
        }
    }

    return list;
}

QStringList Note::fetchNoteFileNames() {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    QStringList list;

    query.prepare(
            "SELECT file_name FROM note ORDER BY file_last_modified DESC");
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            list.append(query.value("file_name").toString());
        }
    }

    return list;
}

bool Note::storeNewText(QString text) {
    this->noteText = text;
    this->hasDirtyData = true;

    return this->store();
}

bool Note::storeNewDecryptedText(QString text) {
    this->decryptedNoteText = text;
    this->hasDirtyData = true;

    return this->store();
}

/**
 * Returns the default note file extension (`md`, `txt` or custom extensions)
 */
QString Note::defaultNoteFileExtension() {
    QSettings settings;
    QString extension = settings.value(
            "defaultNoteFileExtension", "md").toString();
    return extension;
}

/**
 * Returns the a list of the custom note file extensions
 */
QStringList Note::customNoteFileExtensionList(QString prefix) {
    QSettings settings;
    QStringList list = settings.value(
            "customNoteFileExtensionList").toStringList();

    if (!prefix.isEmpty()) {
        list.replaceInStrings(QRegularExpression("^"), prefix);
    }

    return list;
}

/**
 * Checks if it is allowed to have a different note file name than the headline
 */
bool Note::allowDifferentFileName() {
    QSettings settings;
    return settings.value("allowDifferentNoteFileName").toBool();
}

//
// inserts or updates a note object in the database
//
bool Note::store() {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    if (fileName.isEmpty()) {
        // don't store notes with empty filename and empty name
        if (name.isEmpty()) {
            return false;
        }

        fileName = name + "." + defaultNoteFileExtension();
    }

    if (id > 0) {
        query.prepare("UPDATE note SET "
                              "name = :name,"
                              "share_url = :share_url,"
                              "share_id = :share_id,"
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
                              "modified = :modified "
                              "WHERE id = :id");
        query.bindValue(":id", id);
    } else {
        query.prepare("INSERT INTO note"
                              "(name, share_url, share_id, file_name, "
                              "file_size, note_text, has_dirty_data, "
                              "file_last_modified, file_created, crypto_key,"
                              "modified, crypto_password, decrypted_note_text, "
                              "note_sub_folder_id) "
                              "VALUES (:name, :share_url, :share_id, "
                              ":file_name, :file_size, :note_text,"
                              ":has_dirty_data, :file_last_modified,"
                              ":file_created, :crypto_key, :modified,"
                              ":crypto_password, :decrypted_note_text,"
                              ":note_sub_folder_id)");
    }

    QDateTime modified = QDateTime::currentDateTime();

    // get the size of the note text
    QByteArray bytes = noteText.toUtf8();
    fileSize = bytes.size();

    query.bindValue(":name", name);
    query.bindValue(":share_url", shareUrl);
    query.bindValue(":share_id", shareId);
    query.bindValue(":file_name", fileName);
    query.bindValue(":file_size", fileSize);
    query.bindValue(":note_sub_folder_id", noteSubFolderId);
    query.bindValue(":note_text", noteText);
    query.bindValue(":decrypted_note_text", decryptedNoteText);
    query.bindValue(":has_dirty_data", hasDirtyData ? 1 : 0);
    query.bindValue(":file_created", fileCreated);
    query.bindValue(":file_last_modified", fileLastModified);
    query.bindValue(":crypto_key", cryptoKey);
    query.bindValue(":crypto_password", cryptoPassword);
    query.bindValue(":modified", modified);

    // on error
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else if (id == 0) {  // on insert
        id = query.lastInsertId().toInt();
    }

    this->modified = modified;
    return true;
}

/**
 * Stores a note text file to disk
 * The file name will be changed if needed
 */
bool Note::storeNoteTextFileToDisk() {
    QString oldName = name;

    if (allowDifferentFileName()) {
        // check if a QML function wants to set an other note file name and
        // modify it accordingly
        modifyNoteTextFileNameFromQMLHook();
    } else {
        // checks if filename has to be changed (and change it if needed)
        handleNoteTextFileName();
    }

    QString newName = name;

    // assign the tags to the new name if the name has changed
    if (oldName != newName) {
        // TODO(pbek): we need to heed note subfolders here
        Tag::renameNoteFileNamesOfLinks(oldName, newName);

        // handle the replacing of all note urls if a note was renamed
        Note::handleNoteRenaming(oldName, newName);
    }

    QFile file(fullNoteFilePath());
    bool fileExists = this->fileExists();

    qDebug() << "storing note file: " << this->fileName;

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCritical() << file.errorString();
        return false;
    }

    // if we find a decrypted text to encrypt, then we attempt encrypt it
    if (!decryptedNoteText.isEmpty()) {
        noteText = decryptedNoteText;
        encryptNoteText();
        decryptedNoteText = "";
    }

    // transform all types of newline to \n
    // (maybe the ownCloud-sync works better then)
    QString text = Utils::Misc::transformLinefeeds(this->noteText);

//    diff_match_patch *diff = new diff_match_patch();
//    QList<Diff> diffList = diff->diff_main( this->noteText, text );

//    QString html = diff->diff_prettyHtml( diffList );
//    diff->diff_cleanupSemantic( diffList );
//    qDebug() << __func__ << " - 'diffList': " << diffList[0].toString();
//    qDebug() << __func__ << " - 'html': " << html;

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << text;
    file.flush();
    file.close();

    this->hasDirtyData = false;
    this->fileLastModified = QDateTime::currentDateTime();

    if (!fileExists) {
        this->fileCreated = this->fileLastModified;
    }

    return this->store();
}

/**
 * Does a file name cleanup
 */
QString Note::cleanupFileName(QString name) {
    // remove characters from the name that are problematic
    name.remove(QRegularExpression("[\\/\\\\:]"));

    // remove multiple whitespaces from the name
    name.replace(QRegularExpression("\\s+"), " ");

    return name;
}

/**
 * Checks if a QML function wants to set an other note file name and
 * modifies it accordingly
 */
bool Note::modifyNoteTextFileNameFromQMLHook() {
    // check if a QML function wants to set an other note name
    QString newName = ScriptingService::instance()->
            callHandleNoteTextFileNameHook(this);

    // set the file name from the QML hook
    if (!newName.isEmpty() && (newName != name)) {
        qDebug() << __func__ << " - 'newName': " << newName;

        // remove old note file
        removeNoteFile();

        // store new name and filename
        name = newName;
        fileName = newName + "." + fileNameSuffix();
        store();

        return true;
    }

    return false;
}

/**
 * Checks if the filename has to be changed
 * Generates a new name and filename and removes the old file
 * (the new file is not stored to a note text file!)
 */
void Note::handleNoteTextFileName() {
    // split the text into a string list
    QStringList noteTextLines = this->noteText.split(
            QRegExp("(\\r\\n)|(\\n\\r)|\\r|\\n"));

    // do nothing if there is no text
    if (noteTextLines.count() == 0) return;

    QString name = noteTextLines[0];
    // do nothing if the first line is empty
    if (name == "") return;

    // remove a leading "# " for markdown headlines
    name.remove(QRegularExpression("^#\\s"));

    // cleanup additional characters
    name = cleanupFileName(name);

    // check if name has changed
    if (name != this->name) {
        qDebug() << __func__ << " - 'name' was changed: " << name;
        QString fileName = name + "." + defaultNoteFileExtension();

        // check if note with this filename already exists
        Note note = Note::fetchByFileName(fileName);
        if (note.id > 0) {
            // find new filename for the note (not very safe yet)
            QDateTime currentDateTime = QDateTime::currentDateTime();
            name += " " + currentDateTime
                    .toString(Qt::ISODate).replace(":", "_");
        }

        // remove old note file
        // TODO(pbek): note doesn't seem to be removed very often
        this->removeNoteFile();

        // update the first line of the note text
        // TODO(pbek): UI has to be updated too then!
        noteTextLines[0] = name;
        this->noteText = noteTextLines.join("\n");

        // store new name and filename
        this->name = name;
        this->fileName = name + "." + defaultNoteFileExtension();
        this->store();
    }

    qDebug() << __func__ << " - 'name': " << name;
    qDebug() << __func__ << " - 'this->id': " << this->id;
}

bool Note::updateNoteTextFromDisk() {
    QFile file(fullNoteFilePath());

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << __func__ << " - 'file': " << file.fileName();
        qDebug() << __func__ << " - " << file.errorString();
        return false;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");
    this->noteText = in.readAll();
    file.close();

    // strangely it sometimes gets null
    if (this->noteText.isNull()) this->noteText = "";

    return true;
}

QString Note::getFullNoteFilePathForFile(QString fileName) {
    QSettings settings;

    // prepend the portable data path if we are in portable mode
    QString notesPath = Utils::Misc::prependPortableDataPathIfNeeded(
            settings.value("notesPath").toString());

    return Utils::Misc::removeIfEndsWith(notesPath, "/") +
            Utils::Misc::dirSeparator() + fileName;
}

/**
 * Returns the full path of the note file
 */
QString Note::fullNoteFilePath() {
    return getFullNoteFilePathForFile(relativeNoteFilePath());
}

/**
 * Returns the relative path of the note file
 */
QString Note::relativeNoteFilePath(QString separator) {
    QString fullFileName = fileName;

    if (separator.isEmpty()) {
        separator = Utils::Misc::dirSeparator();
    }

    if (noteSubFolderId > 0) {
        NoteSubFolder noteSubFolder = getNoteSubFolder();
        if (noteSubFolder.isFetched()) {
            fullFileName.prepend(noteSubFolder.relativePath() + separator);
        }
    }

    return fullFileName;
}

/**
 * Returns the full url of the note file
 */
QUrl Note::fullNoteFileUrl() {
    QString windowsSlash = "";

#ifdef Q_OS_WIN32
    // we need an other slash for Windows
    windowsSlash = "/";
#endif

    return QUrl("file://" + windowsSlash + fullNoteFilePath());
}

/**
 * Stores all notes that were changed to disk
 *
 * @param currentNote will be set by this method if the filename of the current
 * note has changed
 * @param currentNoteChanged true if current note was changed
 * @param noteWasRenamed true if a note was renamed
 * @return amount of notes that were saved
 */
int Note::storeDirtyNotesToDisk(Note &currentNote, bool *currentNoteChanged,
                                bool *noteWasRenamed) {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);
    ScriptingService* scriptingService = ScriptingService::instance();
    Note note;
//    qDebug() << "storeDirtyNotesToDisk";

    query.prepare("SELECT * FROM note WHERE has_dirty_data = 1");
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return 0;
    } else {
        int count = 0;
        for (int r = 0; query.next(); r++) {
            note = noteFromQuery(query);
            QString oldName = note.getName();
            note.storeNoteTextFileToDisk();
            QString newName = note.getName();

            // check if the file name has changed
            if (oldName != newName) {
                // rename the note file names of note tag links
                Tag::renameNoteFileNamesOfLinks(oldName, newName);
                *noteWasRenamed = true;

                // override the current note because the file name has changed
                currentNote = note;

                // handle the replacing of all note urls if a note was renamed
                // we don't need to do that here, it would be called two
                // times this way
//                Note::handleNoteRenaming(oldName, newName);
            }

            // emit the signal for the QML that the note was stored
            emit scriptingService->noteStored(
                    QVariant::fromValue(
                            static_cast<QObject*>(NoteApi::fromNote(note))));

            // reassign currentNote if filename of currentNote has changed
            if (note.isSameFile(currentNote)) {
                *currentNoteChanged = true;
            }

            qDebug() << "stored note: " << note;
            count++;
        }

        return count;
    }
}

void Note::createFromFile(QFile &file, int noteSubFolderId) {
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        in.setCodec("UTF-8");

        // qDebug() << file.size() << in.readAll();
        QString noteText = in.readAll();
        file.close();

        QFileInfo fileInfo;
        fileInfo.setFile(file);

        // create a nicer name by removing the extension
        // TODO(pbek): make sure name is ownCloud Notes conform
        QString name = fileInfo.fileName();

        int lastPoint = name.lastIndexOf(".");
        name = name.left(lastPoint);

        this->name = name;
        this->fileName = fileInfo.fileName();
        this->noteSubFolderId = noteSubFolderId;
        this->noteText = noteText;
        this->fileCreated = fileInfo.created();
        this->fileLastModified = fileInfo.lastModified();
        this->store();
    }
}

/**
 * Updates or creates a note from a file
 *
 * @param file
 * @param noteSubFolder
 * @return
 */
Note Note::updateOrCreateFromFile(QFile &file, NoteSubFolder noteSubFolder) {
    QFileInfo fileInfo(file);
    Note note = fetchByFileName(fileInfo.fileName(), noteSubFolder.getId());

    // regardless if the file was found or not, if the size differs or the
    // file was modified after the internal note was modified we want to load
    // the note content again
    if ((fileInfo.size() != note.getFileSize()) ||
                (fileInfo.lastModified() > note.getModified())) {
        // load file data and store note
        note.createFromFile(file, noteSubFolder.getId());

//        qDebug() << __func__ << " - 'file modified': " << file.fileName();
    }

    return note;
}

//
// deletes all notes in the database
//
bool Note::deleteAll() {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    // no truncate in sqlite
    query.prepare("DELETE FROM note");
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else {
        return true;
    }
}

//
// checks if file of note exists in the filesystem
//
bool Note::fileExists() {
    QFile file(fullNoteFilePath());
    return file.exists();
}

//
// checks if the current note still exists in the database
//
bool Note::exists() {
    Note note = Note::fetch(this->id);
    return note.id > 0;
}

//
// reloads the current Note (by fileName)
//
bool Note::refetch() {
    return this->fillByFileName(fileName, noteSubFolderId);
}

/**
 * Returns the suffix of the note file name
 */
QString Note::fileNameSuffix() {
    QFileInfo fileInfo;
    fileInfo.setFile(fileName);
    return fileInfo.suffix();
}

/**
 * Returns the base name of the note file name
 */
QString Note::fileBaseName(bool withFullName) {
    if (withFullName) {
        QStringList parts = fileName.split(".");
        parts.removeLast();
        return parts.join(".");
    } else {
        QFileInfo fileInfo;
        fileInfo.setFile(fileName);
        return fileInfo.baseName();
    }
}

/**
 * Renames a note
 */
bool Note::renameNoteFile(QString newName) {
    // cleanup not allowed characters characters
    newName = cleanupFileName(newName);

    // add the old file suffix to the name
    QString newFileName = newName + "." + fileNameSuffix();

    // check if name has really changed
    if (name == newName) {
        return false;
    }

    // check if name already exists
    Note existingNote = Note::fetchByName(newName);
    if (existingNote.isFetched() && (existingNote.getId() != id)) {
        return false;
    }

    // get the note file to rename it
    QFile file(fullNoteFilePath());

    // store the new note file name
    fileName = newFileName;
    name = newName;
    store();

    // rename the note file name
    return file.rename(fullNoteFilePath());
}

//
// remove the file of the note
//
bool Note::removeNoteFile() {
    if (this->fileExists()) {
        QFile file(fullNoteFilePath());
        qDebug() << __func__ << " - 'this->fileName': " << this->fileName;
        qDebug() << __func__ << " - 'file': " << file.fileName();
        return file.remove();
    }

    return false;
}

/**
 * @brief Returns html rendered markdown of the note text
 * @param notesPath for transforming relative local urls to absolute ones
 * @param maxImageWidth defined maximum image width (ignored if forExport is true)
 * @param forExport defines whether the export or preview stylesheet
 * @return
 */
QString Note::toMarkdownHtml(QString notesPath, int maxImageWidth,
                             bool forExport, bool decrypt) {
    hoedown_renderer *renderer =
            hoedown_html_renderer_new(HOEDOWN_HTML_USE_XHTML, 16);

    // we want to show quotes in the html, so we don't translate them into
    // `<q>` tags
    // HOEDOWN_EXT_MATH and HOEDOWN_EXT_MATH_EXPLICIT don't seem to do anything
    hoedown_extensions extensions =
            (hoedown_extensions) ((HOEDOWN_EXT_BLOCK | HOEDOWN_EXT_SPAN |
                    HOEDOWN_EXT_MATH_EXPLICIT) & ~HOEDOWN_EXT_QUOTE);
    hoedown_document *document = hoedown_document_new(renderer, extensions, 16);

    // get the decrypted note text (or the normal note text if there isn't any)
    QString str = decrypt ? getDecryptedNoteText() : noteText;

    QString windowsSlash = "";

#ifdef Q_OS_WIN32
    // we need an other slash for Windows
    windowsSlash = "/";
#endif

    // parse for relative file urls and make them absolute
    // (for example to show images under the note path)
    str.replace(
            QRegularExpression("([\\(<])file:\\/\\/([^\\/].+)([\\)>])"),
            "\\1file://" + windowsSlash + QRegularExpression::escape(notesPath)
            + "/\\2\\3");

    unsigned char *sequence = (unsigned char *) qstrdup(
            str.toUtf8().constData());
    qint64 length = strlen((char *) sequence);

    // return an empty string if the note is empty
    if (length == 0) {
        return "";
    }

    hoedown_buffer *html = hoedown_buffer_new(length);

    // render markdown html
    hoedown_document_render(document, html, sequence, length);

    // get markdown html
    QString result = QString::fromUtf8((char *) html->data, html->size);

//    qDebug() << __func__ << " - 'result': " << result;

    /* Cleanup */
    free(sequence);
    hoedown_buffer_free(html);

    hoedown_document_free(document);
    hoedown_html_renderer_free(renderer);

    QSettings(settings);
    QString fontString = settings.value("MainWindow/noteTextView.code.font")
            .toString();

    // set the stylesheet for the <code> blocks
    QString codeStyleSheet = "";
    if (fontString != "") {
        // set the note text view font
        QFont font;
        font.fromString(fontString);

        // add the font for the code block
        codeStyleSheet = QString(
                "pre, code { %1; }").arg(encodeCssFont(font));
    }

    bool darkModeColors = settings.value("darkModeColors").toBool();

    QString codeBackgroundColor = darkModeColors ? "#444444" : "#f1f1f1";

    // do some more code formatting
    codeStyleSheet += QString(
            "pre, code { padding: 16px; overflow: auto;"
                    " line-height: 1.45em; background-color: %1;"
                    " border-radius: 3px; }").arg(codeBackgroundColor);

    // remove double code blocks
    result.replace("<pre><code>", "<pre>")
            .replace("</code></pre>", "</pre>");

    // correct the strikeout tag
    result.replace(QRegularExpression("<del>([^<]+)<\\/del>"), "<s>\\1</s>");
    bool rtl = settings.value("MainWindow/noteTextView.rtl").toBool();
    QString rtlStyle = rtl ? "body {text-align: right; direction: rtl;}" : "";

    if (forExport) {
        // get defined body font from settings
        QString bodyFontString = settings.value("MainWindow/noteTextView.font")
                .toString();

        // create export stylesheet
        QString exportStyleSheet = "";
        if (bodyFontString != "") {
            QFont bodyFont;
            bodyFont.fromString(bodyFontString);

            exportStyleSheet = QString(
                    "body { %1; }").arg(encodeCssFont(bodyFont));
        }

        result = QString("<html><head><meta charset=\"utf-8\"/><style>"
                    "h1 { margin: 5px 0 20px 0; }"
                    "h2, h3 { margin: 10px 0 15px 0; }"
                    "img { max-width: 100%; }"
                    "a { color: #FF9137; text-decoration: none; } %1 %2 %4"
                    "</style></head><body>%3</body></html>")
            .arg(codeStyleSheet, exportStyleSheet, result, rtlStyle);
    } else {
        // for preview
        result = QString("<html><head><style>"
                    "h1 { margin: 5px 0 20px 0; }"
                    "h2, h3 { margin: 10px 0 15px 0; }"
                    "a { color: #FF9137; text-decoration: none; } %1 %3"
                    "</style></head><body>%2</body></html>")
            .arg(codeStyleSheet, result, rtlStyle);
    }

    // check if width of embedded local images is too high
    QRegularExpression re("<img src=\"file:\\/\\/([^\"]+)\"");
    QRegularExpressionMatchIterator i = re.globalMatch(result);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString fileName = match.captured(1);

#ifdef Q_OS_WIN
        // remove the leading slash under Windows to get a more correct filename
        QString fileNameWindows = Utils::Misc::removeIfStartsWith(fileName, "/");

        // fix for "\" in paths
        fileNameWindows.replace("%5C", "/");

        QImage image(fileNameWindows);
#else
        QImage image(fileName);
#endif

        if (forExport) {
            result.replace(
                    QRegularExpression("<img src=\"file:\\/\\/" +
                                       QRegularExpression::escape(fileName) +
                                       "\""),
                    QString("<img src=\"file://%2\"").arg(fileName));
        } else {
            // for preview
            // cap the image width at 980px or the note text view width
            if (image.width() > maxImageWidth) {
                result.replace(
                        QRegularExpression("<img src=\"file:\\/\\/" +
                                           QRegularExpression::escape(fileName) +
                                           "\""),
                        QString("<img width=\"%1\" src=\"file://%2\"").arg(
                                QString::number(maxImageWidth), fileName));
            }
        }
    }

    // check if there is a script that wants to modify the content
    QString scriptResult = ScriptingService::instance()
            ->callNoteToMarkdownHtmlHook(this, result);

    if (!scriptResult.isEmpty()) {
        result = scriptResult;
    }

    return result;
}

/**
 * Returns the CSS code for a QFont
 * Thank you to Phil Weinstein for the code
 */
QString Note::encodeCssFont(const QFont& refFont)
{
//-----------------------------------------------------------------------
// This function assembles a CSS Font specification string from
// a QFont. This supports most of the QFont attributes settable in
// the Qt 4.8 and Qt 5.3 QFontDialog.
//
// (1) Font Family
// (2) Font Weight (just bold or not)
// (3) Font Style (possibly Italic or Oblique)
// (4) Font Size (in either pixels or points)
// (5) Decorations (possibly Underline or Strikeout)
//
// Not supported: Writing System (e.g. Latin).
//
// See the corresponding decode function, below.
// QFont decodeCssFontString (const QString cssFontStr)
//-----------------------------------------------------------------------

    QStringList fields; // CSS font attribute fields

// ***************************************************
// *** (1) Font Family: Primary plus Substitutes ***
// ***************************************************

    const QString family = refFont.family();

// NOTE [9-2014, Qt 4.8.6]: This isn't what I thought it was. It
// does not return a list of "fallback" font faces (e.g. Georgia,
// Serif for "Times New Roman"). In my testing, this is always
// returning an empty list.
//
    QStringList famSubs = QFont::substitutes (family);

    if (!famSubs.contains (family))
        famSubs.prepend (family);

    static const QChar DBL_QUOT ('"');
    const int famCnt = famSubs.count();
    QStringList famList;
    for (int inx = 0; inx < famCnt; ++inx)
    {
// Place double quotes around family names having space characters,
// but only if double quotes are not already there.
//
        const QString fam = famSubs [inx];
        if (fam.contains (' ') && !fam.startsWith (DBL_QUOT))
            famList << (DBL_QUOT + fam + DBL_QUOT);
        else
            famList << fam;
    }

    const QString famStr = QString ("font-family: ") + famList.join (", ");
    fields << famStr;

// **************************************
// *** (2) Font Weight: Bold or Not ***
// **************************************

    const bool bold = refFont.bold();
    if (bold)
        fields << "font-weight: bold";

// ****************************************************
// *** (3) Font Style: possibly Italic or Oblique ***
// ****************************************************

    const QFont::Style style = refFont.style();
    switch (style)
    {
        case QFont::StyleNormal: break;
        case QFont::StyleItalic: fields << "font-style: italic"; break;
        case QFont::StyleOblique: fields << "font-style: oblique"; break;
    }

// ************************************************
// *** (4) Font Size: either Pixels or Points ***
// ************************************************

    const double sizeInPoints = refFont.pointSizeF(); // <= 0 if not defined.
    const int sizeInPixels = refFont.pixelSize(); // <= 0 if not defined.
    if (sizeInPoints > 0.0)
        fields << QString ("font-size: %1pt") .arg (sizeInPoints);
    else if (sizeInPixels > 0)
        fields << QString ("font-size: %1px") .arg (sizeInPixels);

// ***********************************************
// *** (5) Decorations: Underline, Strikeout ***
// ***********************************************

    const bool underline = refFont.underline();
    const bool strikeOut = refFont.strikeOut();

    if (underline && strikeOut)
        fields << "text-decoration: underline line-through";
    else if (underline)
        fields << "text-decoration: underline";
    else if (strikeOut)
        fields << "text-decoration: line-through";

    const QString cssFontStr = fields.join ("; ");
    return cssFontStr;
}

bool Note::isFetched() {
    return (this->id > 0);
}

/**
 * @brief Generates a text that can be used in a link
 * @param text
 * @return
 */
QString Note::generateTextForLink(QString text) {
    // replace everything but characters and numbers with "_"
    // we want to treat unicode characters as normal characters
    // to support links to notes with unicode characters in their names
    QRegularExpression re(
            "[^\\d\\w]", QRegularExpression::CaseInsensitiveOption |
                    QRegularExpression::UseUnicodePropertiesOption);
    text.replace(re, "_");
    return text;
}

/**
 * Generates a qint64 hash from a QString
 */
qint64 Note::qint64Hash(const QString &str) {
    QByteArray hash = QCryptographicHash::hash(
            QByteArray::fromRawData((const char *) str.utf16(),
                                    str.length() * 2),
            QCryptographicHash::Md5);
    Q_ASSERT(hash.size() == 16);
    QDataStream stream(hash);
    qint64 a, b;
    stream >> a >> b;
    return a ^ b;
}

/**
 * Encrypts the note text with the note's crypto key
 */
QString Note::encryptNoteText() {
    // split the text into a string list
    QStringList noteTextLines = this->noteText.split(
            QRegExp("(\\r\\n)|(\\n\\r)|\\r|\\n"));

    // keep the first two lines unencrypted
    noteText = noteTextLines.at(0) + "\n" + noteTextLines.at(1) + "\n\n" +
               QString(NOTE_TEXT_ENCRYPTION_PRE_STRING) + "\n";

    // remove the first two lines for encryption
    noteTextLines.removeFirst();
    noteTextLines.removeFirst();

    // remove the 3rd line too if it is empty
    if (noteTextLines.at(0) == "") {
        noteTextLines.removeFirst();
    }

    // join the remaining lines
    QString text = noteTextLines.join("\n");

    // empty notes will be detected as "can't be decrypted",
    // so we will add a space
    if (text.isEmpty()) {
        text = " ";
    }

    // check if we have an external encryption method
    QString encryptedText = ScriptingService::instance()->callEncryptionHook(
            text, cryptoPassword, false);

    // check if a hook changed the text
    if (encryptedText.isEmpty()) {
        // fallback to Botan
        // encrypt the text
        BotanWrapper botanWrapper;
        botanWrapper.setPassword(cryptoPassword);
        botanWrapper.setSalt(BOTAN_SALT);
        encryptedText = botanWrapper.Encrypt(text);

//    SimpleCrypt *crypto = new SimpleCrypt(static_cast<quint64>(cryptoKey));
//    QString encryptedText = crypto->encryptToString(text);
    }

    // add the encrypted text to the new note text
    noteText += encryptedText + "\n" +
                QString(NOTE_TEXT_ENCRYPTION_POST_STRING);

    // store note
    store();

    return noteText;
}

/**
 * Returns the regular expression to match encrypted text
 */
QRegularExpression Note::getEncryptedNoteTextRegularExpression() {
    // match the encrypted string
    QRegularExpression re(
            QRegularExpression::escape(NOTE_TEXT_ENCRYPTION_PRE_STRING) +
            "\\s+(.+)\\s+" +
            QRegularExpression::escape(NOTE_TEXT_ENCRYPTION_POST_STRING));

    re.setPatternOptions(
            QRegularExpression::MultilineOption |
            QRegularExpression::DotMatchesEverythingOption);

    return re;
}

/**
 * Returns encrypted note text if it is encrypted
 */
QString Note::getEncryptedNoteText() {
    QString noteText = this->noteText;

    // get regular expression for the encrypted string
    QRegularExpression re = getEncryptedNoteTextRegularExpression();

    // check if we have an encrypted note text and return it if so
    QRegularExpressionMatch match = re.match(noteText);
    return match.hasMatch() ? match.captured(1) : "";
}

/**
 * Returns encrypted note text if it is encrypted
 */
bool Note::hasEncryptedNoteText() {
    return !getEncryptedNoteText().isEmpty();
}

/**
 * Checks if note text can be decrypted
 */
bool Note::canDecryptNoteText() {
    QString encryptedNoteText = getEncryptedNoteText();

    if (encryptedNoteText == "") {
        return false;
    }

    // check if we have an external decryption method
    QString decryptedNoteText =
            ScriptingService::instance()->callEncryptionHook(
                    encryptedNoteText, cryptoPassword, true);

    // check if a hook changed the text
    if (decryptedNoteText.isEmpty()) {
        // decrypt the note text with Botan
        BotanWrapper botanWrapper;
        botanWrapper.setPassword(cryptoPassword);
        botanWrapper.setSalt(BOTAN_SALT);
        decryptedNoteText = botanWrapper.Decrypt(encryptedNoteText);

        // fallback to SimpleCrypt
        if (decryptedNoteText == "") {
            SimpleCrypt *crypto = new SimpleCrypt(
                    static_cast<quint64>(cryptoKey));
            decryptedNoteText = crypto->decryptToString(encryptedNoteText);
        }
    }

    return decryptedNoteText != "";
}

/**
 * Sets the password to generate the cryptoKey
 */
void Note::setCryptoPassword(QString password) {
    cryptoKey = qint64Hash(password);
    cryptoPassword = password;
}

/**
 * Returns decrypted note text if it is encrypted
 * The crypto key has to be set in the object
 */
QString Note::getDecryptedNoteText() {
    QString noteText = this->noteText;
    QString encryptedNoteText = getEncryptedNoteText();

    if (encryptedNoteText == "") {
        return noteText;
    }

    // check if we have an external decryption method
    QString decryptedNoteText =
            ScriptingService::instance()->callEncryptionHook(
                    encryptedNoteText, cryptoPassword, true);

    // check if a hook changed the text
    if (decryptedNoteText.isEmpty()) {
        // decrypt the note text
        BotanWrapper botanWrapper;
        botanWrapper.setPassword(cryptoPassword);
        botanWrapper.setSalt(BOTAN_SALT);
        decryptedNoteText = botanWrapper.Decrypt(encryptedNoteText);

        // fallback to SimpleCrypt
        if (decryptedNoteText == "") {
            SimpleCrypt *crypto = new SimpleCrypt(
                    static_cast<quint64>(cryptoKey));
            decryptedNoteText = crypto->decryptToString(encryptedNoteText);
        }
    }

    if (decryptedNoteText == "") {
        return noteText;
    }

    // get regular expression for the encrypted string
    QRegularExpression re = getEncryptedNoteTextRegularExpression();

    // replace the encrypted text with the decrypted text
    noteText.replace(re, decryptedNoteText);
    return noteText;
}


/**
 * Expire crypto keys in the database after 10min
 */
bool Note::expireCryptoKeys() {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    // 10min ago
    QDateTime expiryDate = QDateTime::currentDateTime();
    expiryDate = expiryDate.addSecs(-600);

    // reset expired crypto keys
    query.prepare("UPDATE note SET crypto_key = 0, crypto_password = '' WHERE "
                          "modified < :expiryDate AND crypto_key != 0");
    query.bindValue(":expiryDate", expiryDate);

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
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    query.prepare("SELECT COUNT(*) AS cnt FROM note");

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        return query.value("cnt").toInt();
    }

    return 0;
}

/**
 * Counts all notes by note sub folder id
 */
int Note::countByNoteSubFolderId(int noteSubFolderId) {
    QSqlDatabase db = QSqlDatabase::database("memory");
    QSqlQuery query(db);

    query.prepare("SELECT COUNT(*) AS cnt FROM note WHERE note_sub_folder_id "
                          "= :note_sub_folder_id");
    query.bindValue(":note_sub_folder_id", noteSubFolderId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        return query.value("cnt").toInt();
    }

    return 0;
}

/**
 * Checks if the notes are the same (by file)
 *
 * @param note
 * @return
 */
bool Note::isSameFile(Note note) {
    return (id == note.getId()) &&
            (noteSubFolderId == note.getNoteSubFolderId());
}

/**
 * Finds notes that that link to a note with fileName via note://
 *
 * @param fileName
 * @return list of note ids
 */
QList<int> Note::findLinkedNotes(QString fileName) {
    QString linkText = getNoteURL(fileName);
    QList<int> noteIdList = searchInNotes("<" + linkText + ">");
    QList<int> noteIdList2 = searchInNotes("](" + linkText + ")");
    noteIdList.append(noteIdList2);

    // remove duplicates and return list
    return noteIdList.toSet().toList();
}

/**
 * Returns the url to a note
 *
 * @param fileName
 * @return
 */
const QString Note::getNoteURL(const QString &fileName) {
    return "note://" + generateTextForLink(fileName);
}

/**
 * Handles the replacing of all note urls if a note was renamed
 *
 * @param oldFileName
 * @param newFileName
 */
void Note::handleNoteRenaming(QString oldFileName, QString newFileName) {
    QList<int> noteIdList = Note::findLinkedNotes(oldFileName);
    int noteCount = noteIdList.count();

    if (noteCount == 0) {
        return;
    }

    QString oldUrl = getNoteURL(oldFileName);
    QString newUrl = getNoteURL(newFileName);

    if (QMessageBox::information(
            Q_NULLPTR,
            QObject::tr("Note filename changed"),
            QObject::tr("A change of the note name was detected. Would you "
                                "like to replace all occurrences of "
                                "<strong>%1</strong> links with "
                                "<strong>%2</strong>"
                                " in <strong>%n</strong> note file(s)?", "",
                        noteCount).arg(oldUrl, newUrl),
            QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
        // replace the urls in all found notes
        Q_FOREACH(int noteId, noteIdList) {
                Note note = Note::fetch(noteId);
                if (!note.isFetched()) {
                    continue;
                }

                QString text = note.getNoteText();
                text.replace("<" + oldUrl + ">", "<" + newUrl + ">");
                text.replace("](" + oldUrl + ")", "](" + newUrl + ")");
                note.storeNewText(text);
            }
    }
}

/**
 * Creates a note headline from a name
 *
 * @param name
 * @return
 */
QString Note::createNoteHeader(QString name) {
    QString header = name + "\n";

    for (int i = 0; i < name.length(); i++) {
        header.append("=");
    }

    header.append("\n\n");
    return header;
}

/**
 * Returns the markdown of the inserted media file into a note
 */
QString Note::getInsertMediaMarkdown(QFile *file, bool addNewLine) {
    if (file->exists() && (file->size() > 0)) {
        QDir mediaDir(NoteFolder::currentMediaPath());

        // created the media folder if it doesn't exist
        if (!mediaDir.exists()) {
            mediaDir.mkpath(mediaDir.path());
        }

        QFileInfo fileInfo(file->fileName());

        // find a random name for the new file
        QString newFileName =
                QString::number(qrand()) + "." + fileInfo.suffix();

        QString newFilePath = mediaDir.path() + QDir::separator() + newFileName;

        // copy the file the the media folder
        file->copy(newFilePath);

        QFile newFile(newFilePath);
        scaleDownImageFileIfNeeded(newFile);

        // return the image link
        // we add a "\n" in the end so that hoedown recognizes multiple images
        return "![" + fileInfo.baseName() + "](file://media/" +
               newFileName + ")" + (addNewLine ? "\n" : "");
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
    QSettings settings;

    // load image scaling settings
    bool scaleImageDown = settings.value("imageScaleDown", false).toBool();

    if (!scaleImageDown) {
        return true;
    }

    QImage image;

    if (!image.load(file.fileName())) {
        return false;
    }

    int width = settings.value("imageScaleDownMaximumWidth", 1024).toInt();
    int height = settings.value("imageScaleDownMaximumHeight", 1024).toInt();

    QPixmap pixmap;
    pixmap = pixmap.fromImage(image.scaled(
            width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    file.open(QIODevice::WriteOnly);
    pixmap.save(&file);
    file.close();

    return true;
}

/**
 * Fetches all tags of the note
 */
//QList<Tag> Note::tags() {
//    return Tag::fetchAllOfNote(this);
//}

QDebug operator<<(QDebug dbg, const Note &note) {
    NoteSubFolder noteSubFolder = NoteSubFolder::fetch(note.noteSubFolderId);
    dbg.nospace() << "Note: <id>" << note.id << " <name>" << note.name <<
        " <fileName>" << note.fileName <<
        " <noteSubFolderId>" << note.noteSubFolderId <<
        " <relativePath>" << noteSubFolder.relativePath() <<
        " <hasDirtyData>" << note.hasDirtyData;
    return dbg.space();
}
