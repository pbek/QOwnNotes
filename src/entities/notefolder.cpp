#include "notefolder.h"
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
    ownCloudServerId = 0;
    remotePath = "";
    priority = 0;
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
    this->localPath = query.value("local_path").toString();
    this->ownCloudServerId = query.value("owncloud_server_id").toInt();
    this->remotePath = query.value("remote_path").toString();
    this->priority = query.value("priority").toInt();

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
                        "remote_path = :remotePath, priority = :priority "
                        "WHERE id = :id");
        query.bindValue(":id", this->id);
    } else {
        query.prepare(
                "INSERT INTO noteFolder (name, local_path, owncloud_server_id, "
                        "remote_path, priority) VALUES (:name, :localPath, "
                        ":ownCloudServerId, :remotePath, :priority)");
    }

    query.bindValue(":name", this->name);
    query.bindValue(":localPath", this->localPath);
    query.bindValue(":ownCloudServerId", this->ownCloudServerId);
    query.bindValue(":remotePath", this->remotePath);
    query.bindValue(":priority", this->priority);

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
    settings.setValue("notesPath", localPath);
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
 * Suggests a remote path
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
    QString notesPath = settings.value("notesPath").toString();

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
