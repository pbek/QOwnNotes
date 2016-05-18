#include "services/databaseservice.h"
#include "entities/calendaritem.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QApplication>
#include <QSettings>
#include <QSqlError>
#include <entities/notefolder.h>

DatabaseService::DatabaseService() {
}

/**
 * @brief Returns the path to the database (on disk)
 * @return
 */
QString DatabaseService::getDiskDatabasePath() {

    QStandardPaths::StandardLocation location;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    location = QStandardPaths::AppDataLocation;
#else
    location = QStandardPaths::GenericDataLocation;
#endif

    // get the path to store the database
    QString path = QStandardPaths::writableLocation(location);
    QDir dir;

    // create path if it doesn't exist yet
    dir.mkpath(path);

    return path + QDir::separator() + "QOwnNotes.sqlite";
}


/**
 * @brief Returns the path to the note folder database
 * @return string
 */
QString DatabaseService::getNoteFolderDatabasePath() {

    return NoteFolder::currentLocalPath() + QDir::separator()
           + "notes.sqlite";
}

bool DatabaseService::removeDiskDatabase() {
    QFile file(getDiskDatabasePath());

    if (file.exists()) {
        qDebug() << __func__ << " - 'removing database file': "
        << file.fileName();
        return file.remove();
    }

    return false;
}

bool DatabaseService::createConnection() {
    return createMemoryConnection() && createDiskConnection();
}

bool DatabaseService::reinitializeDiskDatabase() {
    return removeDiskDatabase() && createDiskConnection() && setupTables();
}

bool DatabaseService::createMemoryConnection() {
    QSqlDatabase dbMemory = QSqlDatabase::addDatabase("QSQLITE", "memory");
    dbMemory.setDatabaseName(":memory:");

    if (!dbMemory.open()) {
        QMessageBox::critical(
                0, QWidget::tr("Cannot open memory database"),
                QWidget::tr(
                        "Unable to establish a memory database connection."),
                QMessageBox::Ok);
        return false;
    }

    return true;
}

bool DatabaseService::createDiskConnection() {
    QSqlDatabase dbDisk = QSqlDatabase::addDatabase("QSQLITE", "disk");
    QString path = getDiskDatabasePath();
    dbDisk.setDatabaseName(path);

    if (!dbDisk.open()) {
        QMessageBox::critical(
                0, QWidget::tr("Cannot open disk database"),
                QWidget::tr(
                      "Unable to establish a database connection with "
                              "file '%1'.\nAre the folder and the file "
                              "writeable?").arg(path),
                QMessageBox::Ok);
        return false;
    }

    return true;
}

bool DatabaseService::createNoteFolderConnection() {
    QSqlDatabase dbDisk = QSqlDatabase::contains("note_folder") ?
                          QSqlDatabase::database("note_folder") :
                          QSqlDatabase::addDatabase("QSQLITE", "note_folder");

    QString path = getNoteFolderDatabasePath();
    dbDisk.setDatabaseName(path);

    if (!dbDisk.open()) {
        QMessageBox::critical(
                0, QWidget::tr("Cannot open note folder database"),
                QWidget::tr(
                        "Unable to establish a database connection with "
                                "file '%1'.\nAre the folder and the file "
                                "writeable?").arg(path),
                QMessageBox::Ok);
        return false;
    }

    return true;
}

/**
 * Creates or updates the note folder tables
 */
bool DatabaseService::setupNoteFolderTables() {
    QSqlDatabase dbDisk = QSqlDatabase::database("note_folder");
    QSqlQuery queryDisk(dbDisk);

    queryDisk.exec("CREATE TABLE IF NOT EXISTS appData ("
                           "name VARCHAR(255) PRIMARY KEY, "
                           "value VARCHAR(255));");
    int version = getAppData("database_version", "note_folder").toInt();
    int oldVersion = version;
    qDebug() << __func__ << " - 'database version': " << version;

    if (version < 1) {
        queryDisk.exec("CREATE TABLE IF NOT EXISTS tag ("
                               "id INTEGER PRIMARY KEY,"
                               "name VARCHAR(255),"
                               "priority INTEGER DEFAULT 0,"
                               "created DATETIME DEFAULT current_timestamp)");

        queryDisk.exec("CREATE UNIQUE INDEX IF NOT EXISTS idxUniqueTag ON "
                               "tag (name);");

        queryDisk.exec("CREATE TABLE IF NOT EXISTS noteTagLink ("
                               "id INTEGER PRIMARY KEY,"
                               "tag_id INTEGER,"
                               "note_file_name VARCHAR(255),"
                               "created DATETIME DEFAULT current_timestamp)");

        queryDisk.exec("CREATE UNIQUE INDEX IF NOT EXISTS idxUniqueTagNoteLink"
                               " ON noteTagLink (tag_id, note_file_name);");

        version = 1;
    }

    if (version < 2) {
        queryDisk.exec("ALTER TABLE tag ADD parent_id INTEGER DEFAULT 0;");
        queryDisk.exec("CREATE INDEX IF NOT EXISTS idxTagParent "
                               "ON tag( parent_id );");
        version = 2;
    }

    if (version < 3) {
        queryDisk.exec("DROP INDEX IF EXISTS idxUniqueTag;");
        queryDisk.exec("CREATE UNIQUE INDEX IF NOT EXISTS idxUniqueTag ON "
                               "tag (name, parent_id);");
        version = 3;
    }

    if (version != oldVersion) {
        setAppData("database_version",
                   QString::number(version), "note_folder");
    }

    return true;
}

bool DatabaseService::setupTables() {
    QSqlDatabase dbDisk = QSqlDatabase::database("disk");
    QSqlQuery queryDisk(dbDisk);

    queryDisk.exec("CREATE TABLE IF NOT EXISTS appData ("
                           "name VARCHAR(255) PRIMARY KEY, "
                           "value VARCHAR(255));");
    int version = getAppData("database_version").toInt();
    int oldVersion = version;
    qDebug() << __func__ << " - 'database_version': " << version;

    QSqlDatabase dbMemory = QSqlDatabase::database("memory");
    QSqlQuery queryMemory(dbMemory);
    queryMemory.exec("CREATE TABLE IF NOT EXISTS note ("
                             "id INTEGER PRIMARY KEY,"
                             "name VARCHAR(255),"
                             "file_name VARCHAR(255),"
                             "note_text TEXT,"
                             "decrypted_note_text TEXT,"
                             "has_dirty_data INTEGER DEFAULT 0,"
                             "file_last_modified DATETIME,"
                             "file_created DATETIME,"
                             "crypto_key INT64 DEFAULT 0,"
                             "crypto_password VARCHAR(255),"
                             "created DATETIME default current_timestamp,"
                             "modified DATETIME default current_timestamp)");

    if (version < 1) {
        queryDisk.exec("CREATE TABLE IF NOT EXISTS calendarItem ("
                               "id INTEGER PRIMARY KEY,"
                               "summary VARCHAR(255),"
                               "url VARCHAR(255),"
                               "description TEXT,"
                               "has_dirty_data INTEGER DEFAULT 0,"
                               "completed INTEGER DEFAULT 0,"
                               "priority INTEGER,"
                               "calendar VARCHAR(255),"
                               "uid VARCHAR(255),"
                               "ics_data TEXT,"
                               "alarm_date DATETIME,"
                               "etag VARCHAR(255),"
                               "last_modified_string VARCHAR(255),"
                               "created DATETIME DEFAULT current_timestamp,"
                               "modified DATETIME DEFAULT current_timestamp)");

        queryDisk.exec("CREATE UNIQUE INDEX IF NOT EXISTS idxUrl "
                               "ON calendarItem( url );");
        queryDisk.exec("ALTER TABLE calendarItem ADD completed_date DATETIME;");
        queryDisk.exec("ALTER TABLE calendarItem "
                               "ADD sort_priority INTEGER DEFAULT 0;");
        version = 1;
    }

    if (version < 2) {
        CalendarItem::updateAllSortPriorities();
        version = 2;
    }

    if (version < 3) {
        queryDisk.exec("CREATE TABLE IF NOT EXISTS noteFolder ("
                               "id INTEGER PRIMARY KEY,"
                               "name VARCHAR(255),"
                               "local_path VARCHAR(255),"
                               "remote_path VARCHAR(255),"
                               "owncloud_server_id INTEGER DEFAULT 0,"
                               "priority INTEGER DEFAULT 0 )");
        version = 3;
    }

    // we need to remove the main splitter sizes for version 4 and 5
    if (version < 5) {
        QSettings settings;
        // remove the main splitter sizes for the tags pane
        settings.remove("mainSplitterSizes");

        version = 5;
    }

    if (version < 6) {
        QSettings settings;
        // remove the obsolete activeTagId setting
        settings.remove("activeTagId");

        version = 6;
    }

    if (version < 7) {
        queryDisk.exec("ALTER TABLE noteFolder ADD active_tag_id INTEGER;");
        version = 7;
    }

    if (version < 8) {
        queryDisk.exec("CREATE TABLE IF NOT EXISTS script ("
                               "id INTEGER PRIMARY KEY,"
                               "name VARCHAR(255),"
                               "script_path TEXT,"
                               "enabled BOOLEAN DEFAULT 1,"
                               "priority INTEGER DEFAULT 0 )");
        version = 8;
    }

    if (version != oldVersion) {
        setAppData("database_version", QString::number(version));
    }

    return true;
}

bool DatabaseService::setAppData(QString name, QString value,
                                 QString connectionName) {
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);

    query.prepare("REPLACE INTO appData ( name, value ) "
                          "VALUES ( :name, :value )");
    query.bindValue(":name", name);
    query.bindValue(":value", value);
    return query.exec();
}

QString DatabaseService::getAppData(QString name, QString connectionName) {
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);

    query.prepare("SELECT value FROM appData WHERE name = :name");
    query.bindValue(":name", name);

    if (!query.exec()) {
        qCritical() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        return query.value("value").toString();
    }

    return "";
}
