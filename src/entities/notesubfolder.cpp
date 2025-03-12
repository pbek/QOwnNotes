#include "entities/notesubfolder.h"

#include <utils/misc.h>

#ifndef INTEGRATION_TESTS
#include <utils/gui.h>
#endif

#include <QDebug>
#include <QDir>
#include <QSqlError>
#include <QSqlRecord>
#include <utility>

#include "note.h"
#include "notefolder.h"
#include "services/settingsservice.h"
#include "tag.h"

NoteSubFolder::NoteSubFolder() : _id{0}, _parentId{0}, _name{QLatin1String("")} {}

int NoteSubFolder::getId() const { return _id; }

int NoteSubFolder::getParentId() const { return _parentId; }

NoteSubFolder NoteSubFolder::getParent(const QString& connectionName) const {
    return NoteSubFolder::fetch(_parentId, connectionName);
}

QString NoteSubFolder::getName() const { return _name; }

QDateTime NoteSubFolder::getFileLastModified() const { return _fileLastModified; }

QDateTime NoteSubFolder::getModified() const { return _modified; }

void NoteSubFolder::setName(QString text) { _name = std::move(text); }

void NoteSubFolder::setParentId(int parentId) { _parentId = parentId; }

bool NoteSubFolder::isFetched() const { return (_id > 0); }

NoteSubFolder NoteSubFolder::fetch(int id, const QString& connectionName) {
    const QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    query.prepare(QStringLiteral("SELECT * FROM noteSubFolder WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), id);

    auto noteSubFolder = NoteSubFolder();

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        if (query.first()) {
            noteSubFolder = noteSubFolderFromQuery(query);
        }
    }

    query.finish();

    return noteSubFolder;
}

NoteSubFolder NoteSubFolder::fetchByNameAndParentId(const QString& name, int parentId) {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    query.prepare(
        QStringLiteral("SELECT * FROM noteSubFolder WHERE name = :name "
                       "AND parent_id = :parent_id"));
    query.bindValue(QStringLiteral(":name"), name);
    query.bindValue(QStringLiteral(":parent_id"), parentId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        if (query.first()) {
            return noteSubFolderFromQuery(query);
        }
    }

    return NoteSubFolder();
}

/**
 * Gets the relative path name of the note sub folder
 */
QString NoteSubFolder::relativePath(char separator, const QString& connectionName) const {
    return _parentId == 0 ? _name
                          : getParent(connectionName).relativePath(separator, connectionName) +
                                separator + _name;
}

/**
 * Gets the full path of the note sub folder
 */
QString NoteSubFolder::fullPath() const {
    return Utils::Misc::removeIfEndsWith(Note::getFullFilePathForFile(relativePath()),
                                         QStringLiteral("/"));
}

/**
 * Gets the full path of the note sub folder as QDir
 */
QDir NoteSubFolder::dir() const { return QDir(fullPath()); }

/**
 * Gets the path data of the note sub folder
 */
QString NoteSubFolder::pathData() const {
    return _parentId == 0 ? _name : getParent().pathData() + QChar('\n') + _name;
}

/**
 * Fetches a note sub folder by its path data
 */
NoteSubFolder NoteSubFolder::fetchByPathData(QString pathData, const QString& separator) {
    if (pathData.isEmpty()) return NoteSubFolder();

    pathData = Utils::Misc::removeIfStartsWith(std::move(pathData), separator);
    const QStringList pathList = pathData.split(separator);
    NoteSubFolder noteSubFolder;
    // loop through all names to fetch the deepest note sub folder
    for (const auto& name : pathList) {
        noteSubFolder = NoteSubFolder::fetchByNameAndParentId(name, noteSubFolder.getId());
        if (!noteSubFolder.isFetched()) return NoteSubFolder();
    }
    return noteSubFolder;
}

bool NoteSubFolder::remove() {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    query.prepare(QStringLiteral("DELETE FROM noteSubFolder WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), _id);

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
        const QString oldPath = fullPath();
        const QString oldRelativePath = relativePath();
        setName(newName);
        const QString newPath = fullPath();
        const QString newRelativePath = relativePath();

        // rename the note sub folder paths of note tag links
        // (needs to be done before the folder rename because folder renaming
        // will cause a reload which would trigger the removal of the tag links)
        Tag::renameNoteSubFolderPathsOfLinks(oldRelativePath, newRelativePath);

        // rename the note subfolder
        const bool ret = QDir().rename(oldPath, newPath);

        if (!ret) {
            qCritical() << "Renaming " << oldPath << " to " << newPath << " failed";
        }

        return ret;
    }

    return false;
}

NoteSubFolder NoteSubFolder::noteSubFolderFromQuery(const QSqlQuery& query) {
    return NoteSubFolder().fillFromQuery(query);
}

NoteSubFolder NoteSubFolder::fillFromQuery(const QSqlQuery& query) {
    _id = query.value(QStringLiteral("id")).toInt();
    _parentId = query.value(QStringLiteral("parent_id")).toInt();
    _name = query.value(QStringLiteral("name")).toString();
    _fileLastModified = query.value(QStringLiteral("file_last_modified")).toDateTime();
    _created = query.value(QStringLiteral("created")).toDateTime();
    _modified = query.value(QStringLiteral("modified")).toDateTime();

    return *this;
}

QVector<NoteSubFolder> NoteSubFolder::fetchAll(int limit) {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    QVector<NoteSubFolder> noteSubFolderList;
    QString sql = QStringLiteral(
        "SELECT * FROM noteSubFolder "
        "ORDER BY file_last_modified DESC");

    if (limit >= 0) {
        sql += QStringLiteral(" LIMIT :limit");
    }

    query.prepare(sql);

    if (limit >= 0) {
        noteSubFolderList.reserve(limit);
        query.bindValue(QStringLiteral(":limit"), limit);
    }

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            noteSubFolderList.append(noteSubFolderFromQuery(query));
        }
    }

    return noteSubFolderList;
}

QVector<int> NoteSubFolder::fetchAllIds() {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    QVector<int> idList;
    const QString sql = QStringLiteral("SELECT id FROM noteSubFolder");

    query.prepare(sql);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            const int id = query.value(QStringLiteral("id")).toInt();
            idList.append(id);
        }
    }

    return idList;
}

QVector<NoteSubFolder> NoteSubFolder::fetchAllByParentId(int parentId, const QString& sortBy) {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    QVector<NoteSubFolder> noteSubFolderList;
    const QString sql = QStringLiteral(
                            "SELECT * FROM noteSubFolder WHERE parent_id = "
                            ":parent_id ORDER BY ") +
                        sortBy;

    query.prepare(sql);
    query.bindValue(QStringLiteral(":parent_id"), parentId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            noteSubFolderList.append(noteSubFolderFromQuery(query));
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
QVector<int> NoteSubFolder::fetchIdsRecursivelyByParentId(int parentId) {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    QVector<int> idList = QVector<int>{parentId};

    const QString& sql = QStringLiteral(
        "SELECT id FROM noteSubFolder WHERE parent_id = "
        ":parent_id ORDER BY file_last_modified DESC");
    query.prepare(sql);
    query.bindValue(QStringLiteral(":parent_id"), parentId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            const int id = query.value(QStringLiteral("id")).toInt();
            idList << fetchIdsRecursivelyByParentId(id);
        }
    }

    return idList;
}

//
// inserts or updates a noteSubFolder object in the database
//
bool NoteSubFolder::store() {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    // don't store noteSubFolders with empty name
    if (_name.isEmpty()) {
        return false;
    }

    if (_id > 0) {
        query.prepare(
            "UPDATE noteSubFolder SET "
            "parent_id = :parent_id,"
            "name = :name,"
            "file_last_modified = :file_last_modified,"
            "modified = :modified "
            "WHERE id = :id");
        query.bindValue(QStringLiteral(":id"), _id);
    } else {
        query.prepare(
            "INSERT INTO noteSubFolder"
            "(name, file_last_modified, parent_id,"
            "modified) "
            "VALUES (:name, :file_last_modified, :parent_id,"
            ":modified)");
    }

    const QDateTime currentDateTime = QDateTime::currentDateTime();

    query.bindValue(QStringLiteral(":name"), _name);
    query.bindValue(QStringLiteral(":parent_id"), _parentId);
    query.bindValue(QStringLiteral(":file_last_modified"), _fileLastModified);
    query.bindValue(QStringLiteral(":modified"), currentDateTime);

    // on error
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else if (_id == 0) {    // on insert
        _id = query.lastInsertId().toInt();
    }

    _modified = currentDateTime;
    return true;
}

//
// deletes all noteSubFolders in the database
//
bool NoteSubFolder::deleteAll() {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    // no truncate in sqlite
    query.prepare(QStringLiteral("DELETE FROM noteSubFolder"));
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
bool NoteSubFolder::exists() const { return NoteSubFolder::fetch(_id)._id > 0; }

/**
 * Counts all notes
 */
int NoteSubFolder::countAll() {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    query.prepare(QStringLiteral("SELECT COUNT(*) AS cnt FROM noteSubFolder"));

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        return query.value(QStringLiteral("cnt")).toInt();
    }

    return 0;
}

/**
 * Counts all notes of a parent
 */
int NoteSubFolder::countAllParentId(int parentId) {
    const QSqlDatabase db = QSqlDatabase::database(QStringLiteral("memory"));
    QSqlQuery query(db);

    query.prepare(
        QStringLiteral("SELECT COUNT(*) AS cnt FROM noteSubFolder "
                       "WHERE parent_id = :parentId "));
    query.bindValue(QStringLiteral(":parentId"), parentId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        return query.value(QStringLiteral("cnt")).toInt();
    }

    return 0;
}

void NoteSubFolder::setAsActive() { NoteSubFolder::setAsActive(_id); }

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
bool NoteSubFolder::isActive() const { return activeNoteSubFolderId() == _id; }

/**
 * Returns the id of the current note sub folder of the current note folder
 */
int NoteSubFolder::activeNoteSubFolderId() { return activeNoteSubFolder().getId(); }

/**
 * Returns the current note sub folder of the current note folder
 */
NoteSubFolder NoteSubFolder::activeNoteSubFolder() {
    // we don't need to check if the note sub folder was really fetched
    // because we also want to get the root folder
    return NoteFolder::currentNoteFolder().getActiveNoteSubFolder();
}

/**
 * Saves the expand status of the item
 */
void NoteSubFolder::saveTreeWidgetExpandState(bool expanded) const {
    SettingsService settings;
    const QString settingsKey = treeWidgetExpandStateSettingsKey();

    // load the settings
    QStringList pathList = settings.value(settingsKey).toStringList();
    const QString path = relativePath();

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
    const SettingsService settings;
    const QString settingsKey = treeWidgetExpandStateSettingsKey();

    // load the settings
    const QStringList pathList = settings.value(settingsKey).toStringList();
    const QString path = relativePath();

    return pathList.contains(path);
}

/**
 * Checks if noteSubfoldersPanelShowNotesRecursively is set
 */
bool NoteSubFolder::isNoteSubfoldersPanelShowNotesRecursively() {
    return SettingsService()
        .value(QStringLiteral("noteSubfoldersPanelShowNotesRecursively"))
        .toBool();
}

/**
 * Returns the tree widget expand status settings key
 */
QString NoteSubFolder::treeWidgetExpandStateSettingsKey(int noteFolderId) {
    if (noteFolderId == 0) {
        noteFolderId = NoteFolder::currentNoteFolderId();
    }

    return QStringLiteral("MainWindow/noteSubFolderTreeWidgetExpandState-") +
           QString::number(noteFolderId);
}

/**
 * @brief NoteSubFolder::depth return the depth of the folder in regard to the
 * note folder
 * @return
 */
int NoteSubFolder::depth() const {
    const auto relativePath = this->relativePath('\n');

    if (relativePath.isEmpty()) {
        return 0;
    }

    return relativePath.split(QChar('\n')).count();
}

bool NoteSubFolder::willFolderBeIgnored(const QString& folderName, bool showWarning) {
    // ignore some folders
    const QStringList ignoreFolderList{".", "..", "media", "attachments", "trash"};

    if (ignoreFolderList.contains(folderName)) {
#ifndef INTEGRATION_TESTS
        if (showWarning) {
            Utils::Gui::warning(nullptr, QObject::tr("Folder will be hidden!"),
                                QObject::tr("Folder with name <b>%1</b> can't be created, "
                                            "because it's internally used by the "
                                            "application!")
                                    .arg(folderName),
                                "note-subfolder-hidden-internal");
        }
#else
        Q_UNUSED(showWarning)
#endif

        return true;
    }

    const SettingsService settings;
    const QStringList ignoredFolderRegExpList =
        settings.value(QStringLiteral("ignoreNoteSubFolders"), IGNORED_NOTE_SUBFOLDERS_DEFAULT)
            .toString()
            .split(QLatin1Char(';'));

    // ignore folders by regular expression
    if (Utils::Misc::regExpInListMatches(folderName, ignoredFolderRegExpList)) {
#ifndef INTEGRATION_TESTS
        if (showWarning) {
            Utils::Gui::warning(nullptr, QObject::tr("Folder will be hidden!"),
                                QObject::tr("Folder with name <b>%1</b> can't be created, "
                                            "because it's on the list of ignored subfolders! "
                                            "You can change that in the <i>Panels settings</i>.")
                                    .arg(folderName),
                                "note-subfolder-hidden-settings");
        }
#endif

        return true;
    }

    return false;
}

QDebug operator<<(QDebug dbg, const NoteSubFolder& noteSubFolder) {
    dbg.nospace() << "NoteSubFolder: <id>" << noteSubFolder._id << " <name>" << noteSubFolder._name
                  << " <parentId>" << noteSubFolder._parentId;
    return dbg.space();
}
