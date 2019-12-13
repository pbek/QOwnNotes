#include <utility>
#include "entities/notesubfolder.h"
#include "notefolder.h"
#include "note.h"
#include "tag.h"
#include <QDebug>
#include <QSqlRecord>
#include <QSqlError>
#include <QDir>
#include <QSettings>
#include <utils/misc.h>


NoteSubFolder::NoteSubFolder() {
    this->id = 0;
    this->parentId = 0;
    this->name = QString();
}

int NoteSubFolder::getId() const {
    return this->id;
}

int NoteSubFolder::getParentId() const {
    return this->parentId;
}

NoteSubFolder NoteSubFolder::getParent() const {
    return NoteSubFolder::fetch(parentId);
}

QString NoteSubFolder::getName() const {
    return this->name;
}

QDateTime NoteSubFolder::getFileLastModified() const {
    return this->fileLastModified;
}

QDateTime NoteSubFolder::getModified() const {
    return this->modified;
}

void NoteSubFolder::setName(QString text) {
    this->name = std::move(text);
}

void NoteSubFolder::setParentId(int parentId) {
    this->parentId = parentId;
}

bool NoteSubFolder::isFetched() const {
    return (this->id > 0);
}

NoteSubFolder NoteSubFolder::fetch(int id) {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    NoteSubFolder noteSubFolder;

    query.prepare("SELECT * FROM noteSubFolder WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        if (query.first()) {
            noteSubFolder = noteSubFolderFromQuery(query);
        }
    }

    return noteSubFolder;
}

NoteSubFolder NoteSubFolder::fetchByNameAndParentId(
        const QString& name, int parentId) {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    NoteSubFolder noteSubFolder;

    query.prepare("SELECT * FROM noteSubFolder WHERE name = :name "
                          "AND parent_id = :parent_id");
    query.bindValue(":name", name);
    query.bindValue(":parent_id", parentId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        if (query.first()) {
            noteSubFolder = noteSubFolderFromQuery(query);
        }
    }

    return noteSubFolder;
}

/**
 * Gets the relative path name of the note sub folder
 */
QString NoteSubFolder::relativePath(QString separator) const {
    if (separator.isEmpty()) {
        // be aware that the separator has to be same on all platforms to
        // work cross platform
        separator = Utils::Misc::dirSeparator();
    }

    return parentId == 0 ?
           name :
           getParent().relativePath(separator) + separator + name;
}

/**
 * Gets the full path of the note sub folder
 */
QString NoteSubFolder::fullPath() const {
    return Utils::Misc::removeIfEndsWith(
            Note::getFullFilePathForFile(relativePath()), "/");
}

/**
 * Gets the full path of the note sub folder as QDir
 */
QDir NoteSubFolder::dir() const {
    return QDir(fullPath());
}

/**
 * Gets the path data of the note sub folder
 */
QString NoteSubFolder::pathData() const {
    return parentId == 0 ?
           name :
           getParent().pathData() + "\n" + name;
}

/**
 * Fetches a note sub folder by its path data
 */
NoteSubFolder NoteSubFolder::fetchByPathData(QString pathData,
                                             const QString& separator) {
    pathData = Utils::Misc::removeIfStartsWith(pathData, separator);
    QStringList pathList = pathData.split(separator);
    NoteSubFolder noteSubFolder;
    QStringListIterator itr(pathList);

    // loop through all names to fetch the deepest note sub folder
    while (itr.hasNext()) {
        QString name = itr.next();
        noteSubFolder = NoteSubFolder::fetchByNameAndParentId(
                name, noteSubFolder.getId());
        if (!noteSubFolder.isFetched()) {
            return NoteSubFolder();
        }
    }

    return noteSubFolder;
}

bool NoteSubFolder::remove() {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    query.prepare("DELETE FROM noteSubFolder WHERE id = :id");
    query.bindValue(":id", this->id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else {
        return true;
    }
}

/**
 * Removes the directory recursively from the file system
 */
bool NoteSubFolder::removeFromFileSystem() const {
    QDir dir = this->dir();

    if (dir.exists()) {
        return dir.removeRecursively();
    }

    return false;
}

/**
 * Renames the note subfolder in the file system
 */
bool NoteSubFolder::rename(const QString& newName) {
    QDir dir = this->dir();

    if (dir.exists() && !newName.isEmpty()) {
        QString oldPath = fullPath();
        QString oldRelativePath = relativePath();
        setName(newName);
        QString newPath = fullPath();
        QString newRelativePath = relativePath();

        // rename the note sub folder paths of note tag links
        // (needs to be done before the folder rename because folder renaming
        // will cause a reload which would trigger the removal of the tag links)
        Tag::renameNoteSubFolderPathsOfLinks(oldRelativePath, newRelativePath);

        // rename the note subfolder
        return dir.rename(oldPath, newPath);
    }

    return false;
}

NoteSubFolder NoteSubFolder::noteSubFolderFromQuery(const QSqlQuery& query) {
    NoteSubFolder noteSubFolder;
    noteSubFolder.fillFromQuery(query);
    return noteSubFolder;
}

bool NoteSubFolder::fillFromQuery(const QSqlQuery& query) {
    id = query.value("id").toInt();
    parentId = query.value("parent_id").toInt();
    name = query.value("name").toString();
    fileLastModified = query.value("file_last_modified").toDateTime();
    created = query.value("created").toDateTime();
    modified = query.value("modified").toDateTime();

    return true;
}

QList<NoteSubFolder> NoteSubFolder::fetchAll(int limit) {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    QList<NoteSubFolder> noteSubFolderList;
    QString sql = QStringLiteral("SELECT * FROM noteSubFolder "
            "ORDER BY file_last_modified DESC");

    if (limit >= 0) {
        sql += " LIMIT :limit";
    }

    query.prepare(sql);

    if (limit >= 0) {
        noteSubFolderList.reserve(limit);
        query.bindValue(":limit", limit);
    }

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            NoteSubFolder noteSubFolder = noteSubFolderFromQuery(query);
            noteSubFolderList.append(noteSubFolder);
        }
    }

    return noteSubFolderList;
}

QList<int> NoteSubFolder::fetchAllIds() {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    QList<int> idList;
    QString sql = "SELECT * FROM noteSubFolder";

    query.prepare(sql);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            NoteSubFolder noteSubFolder = noteSubFolderFromQuery(query);
            idList.append(noteSubFolder.getId());
        }
    }

    return idList;
}

QList<NoteSubFolder> NoteSubFolder::fetchAllByParentId(int parentId,
                                                       const QString& sortBy) {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    QList<NoteSubFolder> noteSubFolderList;
    QString sql = "SELECT * FROM noteSubFolder WHERE parent_id = "
                  ":parent_id ORDER BY " + sortBy;

    query.prepare(sql);
    query.bindValue(":parent_id", parentId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            NoteSubFolder noteSubFolder = noteSubFolderFromQuery(query);
            noteSubFolderList.append(noteSubFolder);
        }
    }

    return noteSubFolderList;
}

/**
 * Fetches a list of all ids recursively by a parent id
 * The parent id is included in the list
 *
 * @param parentId
 * @return
 */
QList<int> NoteSubFolder::fetchIdsRecursivelyByParentId(int parentId) {
    QList<int> idList = QList<int>() << parentId;
    auto noteSubFolders = fetchAllByParentId(parentId);
    idList.reserve(noteSubFolders.count());

    Q_FOREACH(const NoteSubFolder &noteSubFolder, noteSubFolders) {
            int id = noteSubFolder.getId();
            idList << fetchIdsRecursivelyByParentId(id);
        }

    return idList;
}

//
// inserts or updates a noteSubFolder object in the database
//
bool NoteSubFolder::store() {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    // don't store noteSubFolders with empty name
    if (name.isEmpty()) {
        return false;
    }

    if (id > 0) {
        query.prepare("UPDATE noteSubFolder SET "
                              "parent_id = :parent_id,"
                              "name = :name,"
                              "file_last_modified = :file_last_modified,"
                              "modified = :modified "
                              "WHERE id = :id");
        query.bindValue(":id", id);
    } else {
        query.prepare("INSERT INTO noteSubFolder"
                              "(name, file_last_modified, parent_id,"
                              "modified) "
                              "VALUES (:name, :file_last_modified, :parent_id,"
                              ":modified)");
    }

    QDateTime modified = QDateTime::currentDateTime();

    query.bindValue(":name", name);
    query.bindValue(":parent_id", parentId);
    query.bindValue(":file_last_modified", fileLastModified);
    query.bindValue(":modified", modified);

    // on error
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else if (id == 0) {  // on insert
        id = query.lastInsertId().toInt();
    }

    modified = modified;
    return true;
}

//
// deletes all noteSubFolders in the database
//
bool NoteSubFolder::deleteAll() {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    // no truncate in sqlite
    query.prepare("DELETE FROM noteSubFolder");
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else {
        return true;
    }
}

//
// checks if the current noteSubFolder still exists in the database
//
bool NoteSubFolder::exists() const {
    NoteSubFolder noteSubFolder = NoteSubFolder::fetch(this->id);
    return noteSubFolder.id > 0;
}

/**
 * Counts all notes
 */
int NoteSubFolder::countAll() {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    query.prepare("SELECT COUNT(*) AS cnt FROM noteSubFolder");

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        return query.value("cnt").toInt();
    }

    return 0;
}

/**
 * Counts all notes of a parent
 */
int NoteSubFolder::countAllParentId(int parentId) {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    query.prepare("SELECT COUNT(*) AS cnt FROM noteSubFolder "
                          "WHERE parent_id = :parentId ");
    query.bindValue(":parentId", parentId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        return query.value("cnt").toInt();
    }

    return 0;
}

void NoteSubFolder::setAsActive() {
    NoteSubFolder::setAsActive(id);
}

/**
 * Set a note sub folder as active note sub folder for the current note folder
 */
bool NoteSubFolder::setAsActive(int noteSubFolderId) {
    NoteFolder noteFolder = NoteFolder::currentNoteFolder();
    if (!noteFolder.isFetched()) {
        return false;
    }

    // we don't need to check if the note sub folder was really fetched
    // because we also want to get the root folder
    NoteSubFolder noteSubFolder = NoteSubFolder::fetch(noteSubFolderId);
    noteFolder.setActiveNoteSubFolder(noteSubFolder);
    return noteFolder.store();
}

/**
 * Checks if this note sub folder is the current one
 */
bool NoteSubFolder::isActive() const {
    return activeNoteSubFolderId() == id;
}

/**
 * Returns the id of the current note sub folder of the current note folder
 */
int NoteSubFolder::activeNoteSubFolderId() {
    return activeNoteSubFolder().getId();
}

/**
 * Returns the current note sub folder of the current note folder
 */
NoteSubFolder NoteSubFolder::activeNoteSubFolder() {
    // we don't need to check if the note sub folder was really fetched
    // because we also want to get the root folder
    NoteFolder noteFolder = NoteFolder::currentNoteFolder();
    return noteFolder.getActiveNoteSubFolder();
}

/**
 * Saves the expand status of the item
 */
void NoteSubFolder::saveTreeWidgetExpandState(bool expanded) const {
    QSettings settings;
    QString settingsKey = treeWidgetExpandStateSettingsKey();

    // load the settings
    QStringList pathList = settings.value(settingsKey).toStringList();
    QString path = relativePath();

    if (!expanded) {
        // if item is not expanded remove the path from the list
        pathList.removeAll(path);
    } else if (!pathList.contains(path)) {
        // if item is collapsed and not already exists in the list then add it
        pathList.append(path);
    }

    // store the settings again
    settings.setValue(settingsKey, pathList);
}

/**
 * Fetches the expand status of the item
 */
bool NoteSubFolder::treeWidgetExpandState() const {
    QSettings settings;
    QString settingsKey = treeWidgetExpandStateSettingsKey();

    // load the settings
    QStringList pathList = settings.value(settingsKey).toStringList();
    QString path = relativePath();

    return pathList.contains(path);
}

/**
 * Checks if noteSubfoldersPanelShowNotesRecursively is set
 */
bool NoteSubFolder::isNoteSubfoldersPanelShowNotesRecursively() {
    QSettings settings;
    return settings.value("noteSubfoldersPanelShowNotesRecursively").toBool();
}

/**
 * Returns the tree widget expand status settings key
 */
QString NoteSubFolder::treeWidgetExpandStateSettingsKey(int noteFolderId) {
    if (noteFolderId == 0) {
        noteFolderId = NoteFolder::currentNoteFolderId();
    }

    return "MainWindow/noteSubFolderTreeWidgetExpandState-" +
            QString::number(noteFolderId);
}

/**
 * @brief NoteSubFolder::depth return the depth of the folder in regard to the note folder
 * @return
 */
int NoteSubFolder::depth() const {
    auto relativePath = this->relativePath("\n");

    if (relativePath.isEmpty()) {
        return 0;
    }

    return relativePath.split("\n").count();
}

QDebug operator<<(QDebug dbg, const NoteSubFolder &noteSubFolder) {
    dbg.nospace() << "NoteSubFolder: <id>" << noteSubFolder.id <<
            " <name>" << noteSubFolder.name <<
            " <parentId>" << noteSubFolder.parentId;
    return dbg.space();
}
