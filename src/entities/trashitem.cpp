#include "entities/trashitem.h"
#include <QDebug>
#include <QSettings>
#include <QDir>
#include <QSqlError>
#include "notefolder.h"
#include "notesubfolder.h"
#include <utils/misc.h>


TrashItem::TrashItem() {
    this->id = 0;
    this->noteSubFolderId = 0;
    this->fileSize = 0;
}

int TrashItem::getId() {
    return this->id;
}

QDateTime TrashItem::getCreated() {
    return this->created;
}

qint64 TrashItem::getFileSize() {
    return this->fileSize;
}

QString TrashItem::getFileName() {
    return this->fileName;
}

NoteSubFolder TrashItem::getNoteSubFolder() {
    return NoteSubFolder::fetch(this->noteSubFolderId);
}

void TrashItem::setNoteSubFolder(NoteSubFolder noteSubFolder) {
    this->noteSubFolderPathData = noteSubFolder.pathData();
}


TrashItem TrashItem::fetch(int id) {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);

    TrashItem trashItem;

    query.prepare("SELECT * FROM trashItem WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        if (query.first()) {
            trashItem = trashItemFromQuery(query);
        }
    }

    return trashItem;
}

bool TrashItem::remove(bool withFile) {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);

    query.prepare("DELETE FROM trashItem WHERE id = :id");
    query.bindValue(":id", this->id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else {
        if (withFile) {
            this->removeFile();
        }

        return true;
    }
}

/**
 * Returns the full path of the trashed file
 */
QString TrashItem::fullFilePath() {
    return NoteFolder::currentTrashPath() + QDir::separator() +
            QString::number(getId());
}

/**
 * Fetches the content of the trashed file
 *
 * @return
 */
QString TrashItem::loadFileFromDisk() {
    QFile file(fullFilePath());

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << __func__ << " - 'file': " << file.fileName();
        qDebug() << __func__ << " - " << file.errorString();
        return "";
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");
    QString text = in.readAll();
    file.close();

    return text;
}

/**
 * Add a note to the trash
 *
 * @param note
 * @return
 */
bool TrashItem::add(Note note) {
    return add(&note);
}

/**
 * Add a note to the trash
 *
 * @param note
 * @return
 */
bool TrashItem::add(Note *note) {
    TrashItem item;
    item.setNote(note);
    return item.doTrashing();
}

/**
 * Prepares a trash item from a note
 *
 * @param note
 * @return
 */
TrashItem TrashItem::prepare(Note *note) {
    TrashItem item;
    item.setNote(note);
    return item;
}

/**
 * Trashes the file of the current item
 *
 * @return
 */
bool TrashItem::doTrashing() {
    if (!this->store()) {
        return false;
    }

    QString destinationPath = NoteFolder::currentTrashPath();
    QDir destinationDir(destinationPath);

    // created the trash folder if it doesn't exist
    if (!destinationDir.exists() &&
        !destinationDir.mkpath(destinationDir.path())) {
        return false;
    }

    QFile file(_fullNoteFilePath);
    QString destinationFileName = destinationPath + QDir::separator() +
                                  QString::number( this->getId() );

    qDebug() << __func__ << " - 'destinationFileName': " << destinationFileName;

    // copy file to trash folder
    return file.copy(destinationFileName);
}

/**
 * Restores a trashed file and removes the trash item
 *
 * @return
 */
bool TrashItem::restoreFile() {
    if (!fileExists()) {
        return false;
    }

    QString newFilePath = restorationFilePath();
    if (newFilePath.isEmpty()) {
        return false;
    }

    QFile file(fullFilePath());
    if (file.rename(newFilePath)) {
        remove();
        return true;
    }

    return false;
}

/**
 * Returns the file path of the restored file
 *
 * @return
 */
QString TrashItem::restorationFilePath() {
    auto noteSubFolder = NoteSubFolder::fetchByPathData(noteSubFolderPathData);
    QString folderPath = noteSubFolder.fullPath();
    QString filePath = folderPath + QDir::separator() + fileName;

    QFile file(filePath);
    // prepend the current timestamp if the file already exists
    if ( file.exists() ) {
        filePath = folderPath + QDir::separator() +
                   QString::number(QDateTime::currentSecsSinceEpoch()) + "_" +
                   fileName;
    }

    file.setFileName(filePath);
    // if the file still exists use a random number
    if ( file.exists() ) {
        filePath = folderPath + QDir::separator() +
                   QString::number(qrand()) + "_" +
                   fileName;
    }

    file.setFileName(filePath);
    // if the file still exists quit
    if ( file.exists() ) {
        return "";
    }

    return filePath;
}

void TrashItem::setNote(Note note) {
    setNote(&note);
}

void TrashItem::setNote(Note *note) {
    noteSubFolderPathData = note->noteSubFolderPathData();
    fileName = note->getFileName();
    fileSize = note->getFileSize();
    _fullNoteFilePath = note->fullNoteFilePath();
}

TrashItem TrashItem::trashItemFromQuery(QSqlQuery query) {
    TrashItem trashItem;
    trashItem.fillFromQuery(query);
    return trashItem;
}

bool TrashItem::fillFromQuery(QSqlQuery query) {
    id = query.value("id").toInt();
    fileName = query.value("file_name").toString();
    noteSubFolderPathData = query.value("note_sub_folder_path_data").toString();
    fileSize = query.value("file_size").toLongLong();
    created = query.value("created").toDateTime();

    return true;
}

QList<TrashItem> TrashItem::fetchAll(int limit) {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);

    QList<TrashItem> noteList;
    QString sql = "SELECT * FROM trashItem ORDER BY created DESC";

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
            TrashItem trashItem = trashItemFromQuery(query);
            noteList.append(trashItem);
        }
    }

    return noteList;
}

//
// inserts or updates a trashItem object in the database
//
bool TrashItem::store() {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);

    if (fileName.isEmpty()) {
        return false;
    }

    if (id > 0) {
        query.prepare("UPDATE trashItem SET "
                              "file_name = :file_name,"
                              "file_size = :file_size,"
                              "note_sub_folder_path_data = "
                              ":note_sub_folder_path_data "
                              "WHERE id = :id");
        query.bindValue(":id", id);
    } else {
        query.prepare("INSERT INTO trashItem"
                              "(file_name, file_size,"
                              "note_sub_folder_path_data) "
                              "VALUES (:file_name, :file_size,"
                              ":note_sub_folder_path_data)");
    }

    query.bindValue(":file_name", fileName);
    query.bindValue(":file_size", fileSize);
    query.bindValue(":note_sub_folder_path_data", noteSubFolderPathData);

    // on error
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else if (id == 0) {  // on insert
        id = query.lastInsertId().toInt();

        // to get the created date
        refetch();
    }

    return true;
}

/**
 * Returns the relative path of the trashItem file
 */
QString TrashItem::relativeNoteFilePath(QString separator) {
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
 * Returns the path-data of the trashItem subfolder file
 */
QString TrashItem::getNoteSubFolderPathData() {
    return noteSubFolderPathData;
}

//
// deletes all notes in the database
//
bool TrashItem::deleteAll() {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);

    // no truncate in sqlite
    query.prepare("DELETE FROM trashItem");
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else {
        return true;
    }
}

/**
 * Checks if file of trashItem exists in the filesystem and is readable
 *
 * @return bool
 */
bool TrashItem::fileExists() {
    QFile file(fullFilePath());
    QFileInfo fileInfo(file);
    return file.exists() && fileInfo.isFile() && fileInfo.isReadable();
}

//
// checks if the current trashItem still exists in the database
//
bool TrashItem::exists() {
    TrashItem trashItem = TrashItem::fetch(this->id);
    return trashItem.id > 0;
}

bool TrashItem::fillFromId(int id) {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);

    query.prepare("SELECT * FROM trashItem WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        fillFromQuery(query);
        return true;
    }

    return false;
}

//
// reloads the current TrashItem (by fileName)
//
bool TrashItem::refetch() {
    return fillFromId(id);
}

/**
 * Returns the base name of the trashItem file name
 */
QString TrashItem::fileBaseName(bool withFullName) {
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

//
// remove the file of the trashItem
//
bool TrashItem::removeFile() {
    if (this->fileExists()) {
        QFile file(fullFilePath());
        qDebug() << __func__ << " - 'this->fileName': " << this->fileName;
        qDebug() << __func__ << " - 'file': " << file.fileName();
        return file.remove();
    }

    return false;
}

bool TrashItem::isFetched() {
    return (this->id > 0);
}

/**
 * Counts all notes
 */
int TrashItem::countAll() {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);

    query.prepare("SELECT COUNT(*) AS cnt FROM trashItem");

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        return query.value("cnt").toInt();
    }

    return 0;
}

/**
 * Fetches all tags of the trashItem
 */
//QList<Tag> TrashItem::tags() {
//    return Tag::fetchAllOfNote(this);
//}

QDebug operator<<(QDebug dbg, const TrashItem &trashItem) {
    NoteSubFolder noteSubFolder = NoteSubFolder::fetchByPathData(trashItem.noteSubFolderPathData);
    dbg.nospace() << "TrashItem: <id>" << trashItem.id <<
        " <fileName>" << trashItem.fileName <<
        " <noteSubFolderId>" << trashItem.noteSubFolderId <<
        " <_fullNoteFilePath>" << trashItem._fullNoteFilePath <<
        " <relativePath>" << noteSubFolder.relativePath();
    return dbg.space();
}
