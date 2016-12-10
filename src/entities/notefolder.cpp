#include "notefolder.h"
#include "notesubfolder.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSettings>
#include <QDir>
#include <utils/misc.h>


NoteFolder::NoteFolder() {
    id = 0;
    name = "";
    localPath = "";
    ownCloudServerId = 1;
    remotePath = "";
    priority = 0;
    activeTagId = 0;
    showSubfolders = false;
}

int NoteFolder::getId() {
    return this->id;
}

QString NoteFolder::getLocalPath() {
    return this->localPath;
}

int NoteFolder::getOwnCloudServerId() {
    return this->ownCloudServerId;
}

QString NoteFolder::getName() {
    return this->name;
}

QString NoteFolder::getRemotePath() {
    return this->remotePath;
}

int NoteFolder::getPriority() {
    return this->priority;
}

bool NoteFolder::isShowSubfolders() {
    return showSubfolders;
}

int NoteFolder::getActiveTagId() {
    return this->activeTagId;
}

NoteSubFolder NoteFolder::getActiveNoteSubFolder() {
    return NoteSubFolder::fetchByPathData(this->activeNoteSubFolderData);
}

void NoteFolder::setName(QString text) {
    this->name = text;
}

void NoteFolder::setOwnCloudServerId(int id) {
    this->ownCloudServerId = id;
}

void NoteFolder::setLocalPath(QString text) {
    this->localPath = text;
}

void NoteFolder::setRemotePath(QString text) {
    this->remotePath = text;
}

void NoteFolder::setPriority(int value) {
    this->priority = value;
}

void NoteFolder::setShowSubfolders(bool value) {
    showSubfolders = value;
}

void NoteFolder::setActiveTagId(int value) {
    this->activeTagId = value;
}

void NoteFolder::setActiveNoteSubFolder(NoteSubFolder noteSubFolder) {
    this->activeNoteSubFolderData = noteSubFolder.pathData();
}

void NoteFolder::resetActiveNoteSubFolder() {
    this->activeNoteSubFolderData.clear();
}

bool NoteFolder::create(QString name, QString localPath,
                        int ownCloudServerId, QString remotePath) {
    QSqlDatabase db = QSqlDatabase::database("disk");
    QSqlQuery query(db);

    query.prepare("INSERT INTO noteFolder ( name, local_path, "
                          "owncloud_server_id, remote_path ) "
                          "VALUES ( :name, :localPath, ownCloudServerId, "
                          ":remotePath )");
    query.bindValue(":name", name);
    query.bindValue(":localPath", localPath);
    query.bindValue(":ownCloudServerId", ownCloudServerId);
    query.bindValue(":remotePath", remotePath);
    return query.exec();
}

NoteFolder NoteFolder::fetch(int id) {
    QSqlDatabase db = QSqlDatabase::database("disk");
    QSqlQuery query(db);

    NoteFolder noteFolder;

    query.prepare("SELECT * FROM noteFolder WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        noteFolder.fillFromQuery(query);
    }

    return noteFolder;
}

int NoteFolder::countAll() {
    QSqlDatabase db = QSqlDatabase::database("disk");
    QSqlQuery query(db);

    query.prepare("SELECT COUNT(*) AS cnt FROM noteFolder");

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        return query.value("cnt").toInt();
    }

    return 0;
}

bool NoteFolder::localPathExists() {
    QDir folder(localPath);
    return folder.exists() && !localPath.isEmpty();
}

bool NoteFolder::remove() {
    QSqlDatabase db = QSqlDatabase::database("disk");
    QSqlQuery query(db);

    query.prepare("DELETE FROM noteFolder WHERE id = :id");
    query.bindValue(":id", this->id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else {
        return true;
    }
}

NoteFolder NoteFolder::noteFolderFromQuery(QSqlQuery query) {
    NoteFolder noteFolder;
    noteFolder.fillFromQuery(query);
    return noteFolder;
}

bool NoteFolder::fillFromQuery(QSqlQuery query) {
    this->id = query.value("id").toInt();
    this->name = query.value("name").toString();
    this->ownCloudServerId = query.value("owncloud_server_id").toInt();
    this->remotePath = query.value("remote_path").toString();
    this->priority = query.value("priority").toInt();
    this->showSubfolders = query.value("show_subfolders").toBool();
    this->activeTagId = query.value("active_tag_id").toInt();
    this->activeNoteSubFolderData =
            query.value("active_note_sub_folder_data").toString();

    // prepend the portable data path if we are in portable mode
    this->localPath = Utils::Misc::prependPortableDataPathIfNeeded(
            query.value("local_path").toString());

    return true;
}

QList<NoteFolder> NoteFolder::fetchAll() {
    QSqlDatabase db = QSqlDatabase::database("disk");
    QSqlQuery query(db);

    QList<NoteFolder> noteFolderList;

    query.prepare("SELECT * FROM noteFolder ORDER BY priority ASC, id ASC");
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            NoteFolder noteFolder = noteFolderFromQuery(query);
            noteFolderList.append(noteFolder);
        }
    }

    return noteFolderList;
}

/**
 * Inserts or updates a NoteFolder object in the database
 */
bool NoteFolder::store() {
    QSqlDatabase db = QSqlDatabase::database("disk");
    QSqlQuery query(db);

    if (this->id > 0) {
        query.prepare(
                "UPDATE noteFolder SET name = :name, local_path = :localPath, "
                        "owncloud_server_id = :ownCloudServerId, "
                        "remote_path = :remotePath, priority = :priority, "
                        "active_tag_id = :activeTagId, show_subfolders = "
                        ":showSubfolders, active_note_sub_folder_data = "
                        ":activeNoteSubFolderData WHERE id = :id");
        query.bindValue(":id", this->id);
    } else {
        query.prepare(
                "INSERT INTO noteFolder (name, local_path, owncloud_server_id, "
                        "remote_path, priority, active_tag_id, "
                        "show_subfolders, active_note_sub_folder_data)"
                        " VALUES (:name, :localPath, :ownCloudServerId, "
                        ":remotePath, :priority, :activeTagId, "
                        ":showSubfolders, :activeNoteSubFolderData)");
    }

    query.bindValue(":name", this->name);
    query.bindValue(":ownCloudServerId", this->ownCloudServerId);
    query.bindValue(":remotePath", this->remotePath);
    query.bindValue(":priority", this->priority);
    query.bindValue(":activeTagId", this->activeTagId);
    query.bindValue(":showSubfolders", this->showSubfolders);
    query.bindValue(":activeNoteSubFolderData", this->activeNoteSubFolderData);

    // make the path relative to the portable data path if we are in
    // portable mode
    query.bindValue(":localPath",
                    Utils::Misc::makePathRelativeToPortableDataPathIfNeeded(
                            this->localPath));

    if (!query.exec()) {
        // on error
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else if (this->id == 0) {
        // on insert
        this->id = query.lastInsertId().toInt();
    }

    return true;
}

/**
 * Checks if the current noteFolder still exists in the database
 */
bool NoteFolder::exists() {
    NoteFolder noteFolder = NoteFolder::fetch(this->id);
    return noteFolder.id > 0;
}

bool NoteFolder::isFetched() {
    return (this->id > 0);
}

void NoteFolder::setAsCurrent() {
    QSettings settings;
    settings.setValue("currentNoteFolderId", id);

    // make the path relative to the portable data path if we are in
    // portable mode
    settings.setValue("notesPath",
                      Utils::Misc::makePathRelativeToPortableDataPathIfNeeded(
                              localPath));
}

/**
 * Checks if this note folder is the current one
 */
bool NoteFolder::isCurrent() {
    return currentNoteFolderId() == id;
}

/**
 * Returns the id of the current note folder in the settings
 */
int NoteFolder::currentNoteFolderId() {
    QSettings settings;
    return settings.value("currentNoteFolderId").toInt();
}

/**
 * Returns the current note folder
 */
NoteFolder NoteFolder::currentNoteFolder() {
    return NoteFolder::fetch(currentNoteFolderId());
}

/**
 * Fetches the current remote path for the ownCloud service
 */
QString NoteFolder::currentRemotePath(bool addTrailingSlash) {
    QString remotePath;
    NoteFolder noteFolder = currentNoteFolder();

    if (noteFolder.isFetched()) {
        remotePath = noteFolder.getRemotePath();
    }

    // add a leading "/"
    remotePath = Utils::Misc::prependIfDoesNotStartWith(remotePath, "/");

    if (addTrailingSlash) {
        // add a trailing "/"
        remotePath = Utils::Misc::appendIfDoesNotEndWith(remotePath, "/");
    } else {
        // remove a trailing "/"
        remotePath = Utils::Misc::removeIfEndsWith(remotePath, "/");
    }

    return remotePath;
}

/**
 * Fetches the current local path
 */
QString NoteFolder::currentLocalPath() {
    QString path;
    NoteFolder noteFolder = currentNoteFolder();

    if (noteFolder.isFetched()) {
        path = noteFolder.getLocalPath();
    }

    // load notesPath as fallback
    if (path.isEmpty()) {
        QSettings settings;

        // prepend the portable data path if we are in portable mode
        path = Utils::Misc::prependPortableDataPathIfNeeded(
                settings.value("notesPath").toString());
    }

    path = Utils::Misc::removeIfEndsWith(path, QDir::separator());
    path = Utils::Misc::removeIfEndsWith(path, Utils::Misc::dirSeparator());

    return path;
}

/**
 * Fetches the current media path
 */
QString NoteFolder::currentMediaPath() {
    return NoteFolder::currentLocalPath() + QDir::separator() + "media";
}

/**
 * Checks if the subfolders should be viewed for the current note folder
 */
bool NoteFolder::isCurrentShowSubfolders() {
    NoteFolder noteFolder = NoteFolder::currentNoteFolder();
    return noteFolder.isFetched() ? noteFolder.isShowSubfolders() : false;
}

/**
 * Suggests a remote path from an old localOwnCloudPath
 */
QString NoteFolder::suggestRemotePath() {
    QSettings settings;
    QString localOwnCloudPath = settings.value(
            "ownCloud/localOwnCloudPath").toString();

    // get remote path from local ownCloud path
    if (!localOwnCloudPath.isEmpty()) {
        remotePath = QString(localPath);
        remotePath.remove(localOwnCloudPath);
        fixRemotePath();
    } else {
        remotePath = "Notes";
    }

    return remotePath;
}

/**
 * Removes a leading or trailing slash from the remote path
 */
QString NoteFolder::fixRemotePath() {
    remotePath = Utils::Misc::removeIfStartsWith(remotePath, "/");
    remotePath = Utils::Misc::removeIfEndsWith(remotePath, "/");
    return remotePath;
}

/**
 * Create the notesPath and the recentNoteFolders to NoteFolder objects
 */
bool NoteFolder::migrateToNoteFolders() {
    if (countAll() > 0) {
        return false;
    }

    QSettings settings;
    int priority = 0;

    // prepend the portable data path if we are in portable mode
    QString notesPath = Utils::Misc::prependPortableDataPathIfNeeded(
            settings.value("notesPath").toString());

    // create notes path as NoteFolder
    if (!notesPath.isEmpty()) {
        NoteFolder noteFolder;
        noteFolder.setName(QObject::tr("default"));
        noteFolder.setLocalPath(notesPath);
        noteFolder.setOwnCloudServerId(1);
        noteFolder.suggestRemotePath();
        noteFolder.setPriority(priority++);
        noteFolder.store();

        if (noteFolder.isFetched()) {
            noteFolder.setAsCurrent();
        }
    }

    QStringList recentNoteFolders =
            settings.value("recentNoteFolders").toStringList();

    // create recent note folders as NoteFolder
    if (recentNoteFolders.size() > 0) {
        Q_FOREACH(QString recentNoteFolderPath, recentNoteFolders) {
                if (notesPath != recentNoteFolderPath) {
                    NoteFolder noteFolder;
                    noteFolder.setName(recentNoteFolderPath);
                    noteFolder.setLocalPath(recentNoteFolderPath);
                    noteFolder.setOwnCloudServerId(1);
                    noteFolder.suggestRemotePath();
                    noteFolder.setPriority(priority++);
                    noteFolder.store();
                }
            }
    }

    return priority > 0;
}

QDebug operator<<(QDebug dbg, const NoteFolder &noteFolder) {
    dbg.nospace() << "NoteFolder: <id>" << noteFolder.id << " <name>" <<
            noteFolder.name << " <localPath>" << noteFolder.localPath <<
            " <remotePath>" << noteFolder.remotePath <<
            " <priority>" << noteFolder.priority;
    return dbg.space();
}
