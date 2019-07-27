#include "services/databaseservice.h"
#include "entities/calendaritem.h"
#include "owncloudservice.h"
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
#include <utils/misc.h>
#include <entities/tag.h>

DatabaseService::DatabaseService() = default;

/**
 * Returns the path to the database (on disk)
 *
 * @return
 */
QString DatabaseService::getDiskDatabasePath() {
    QString databaseFileName = Utils::Misc::appDataPath() +
            Utils::Misc::dirSeparator() + "QOwnNotes.sqlite";
    qDebug() << __func__ << " - 'databaseFileName': " << databaseFileName;

    return databaseFileName;
}


/**
 * @brief Returns the path to the note folder database
 * @return string
 */
QString DatabaseService::getNoteFolderDatabasePath() {
    return NoteFolder::currentLocalPath() + Utils::Misc::dirSeparator()
           + "notes.sqlite";
}

bool DatabaseService::removeDiskDatabase() {
    QFile file(getDiskDatabasePath());

    if (file.exists()) {
        // the database file will not get deleted under Windows if the
        // database isn't closed
        QSqlDatabase dbDisk = QSqlDatabase::database("disk");
        dbDisk.close();

        // remove the file
        bool result = file.remove();

        QString text = result ? "Removed" : "Could not remove";
        qWarning() << text + " database file: " << file.fileName();
        return result;
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
    QSqlDatabase dbDisk = getNoteFolderDatabase();
    QSqlQuery queryDisk(dbDisk);

    queryDisk.exec("CREATE TABLE IF NOT EXISTS appData ("
                           "name VARCHAR(255) PRIMARY KEY, "
                           "value VARCHAR(255))");
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
                               "tag (name)");

        queryDisk.exec("CREATE TABLE IF NOT EXISTS noteTagLink ("
                               "id INTEGER PRIMARY KEY,"
                               "tag_id INTEGER,"
                               "note_file_name VARCHAR(255),"
                               "created DATETIME DEFAULT current_timestamp)");

        queryDisk.exec("CREATE UNIQUE INDEX IF NOT EXISTS idxUniqueTagNoteLink"
                               " ON noteTagLink (tag_id, note_file_name)");

        version = 1;
    }

    if (version < 2) {
        queryDisk.exec("ALTER TABLE tag ADD parent_id INTEGER DEFAULT 0");
        queryDisk.exec("CREATE INDEX IF NOT EXISTS idxTagParent "
                               "ON tag( parent_id )");
        version = 2;
    }

    if (version < 3) {
        queryDisk.exec("DROP INDEX IF EXISTS idxUniqueTag");
        queryDisk.exec("CREATE UNIQUE INDEX IF NOT EXISTS idxUniqueTag ON "
                               "tag (name, parent_id)");
        version = 3;
    }

    if (version < 4) {
        queryDisk.exec("ALTER TABLE noteTagLink ADD note_sub_folder_path TEXT");
        version = 4;
    }

    if (version < 5) {
        queryDisk.exec("DROP INDEX IF EXISTS idxUniqueTagNoteLink");
        queryDisk.exec("CREATE UNIQUE INDEX IF NOT EXISTS idxUniqueTagNoteLink "
                               "ON noteTagLink (tag_id, note_file_name, "
                               "note_sub_folder_path)");
        version = 5;
    }

    if (version < 6) {
        // we need to add a `DEFAULT ''` to column note_sub_folder_path
        queryDisk.exec("ALTER TABLE noteTagLink RENAME TO _noteTagLink");
        queryDisk.exec("CREATE TABLE IF NOT EXISTS noteTagLink ("
                               "id INTEGER PRIMARY KEY,"
                               "tag_id INTEGER,"
                               "note_file_name VARCHAR(255) DEFAULT '',"
                               "note_sub_folder_path TEXT DEFAULT '',"
                               "created DATETIME DEFAULT current_timestamp)");
        queryDisk.exec("INSERT INTO noteTagLink (tag_id, note_file_name, "
                               "note_sub_folder_path, created) "
                               "SELECT tag_id, note_file_name, "
                               "note_sub_folder_path, created "
                               "FROM _noteTagLink ORDER BY id");
        queryDisk.exec("DROP INDEX IF EXISTS idxUniqueTagNoteLink");
        queryDisk.exec("CREATE UNIQUE INDEX IF NOT EXISTS idxUniqueTagNoteLink "
                               "ON noteTagLink (tag_id, note_file_name, "
                               "note_sub_folder_path)");
        queryDisk.exec("DROP TABLE _noteTagLink");
        queryDisk.exec("UPDATE noteTagLink SET note_sub_folder_path = '' "
                               "WHERE note_sub_folder_path IS NULL");
        version = 6;
    }

    if (version < 7) {
        // convert backslashes to slashes in the noteTagLink table to fix
        // problems with Windows
        Tag::convertDirSeparator();
        version = 7;
    }

    if (version < 8) {
        queryDisk.exec("ALTER TABLE tag ADD color VARCHAR(20)");
        version = 8;
    }

    if (version < 9) {
        queryDisk.exec("ALTER TABLE tag ADD dark_color VARCHAR(20)");
        version = 9;
    }

    if (version < 10) {
        // set the non-darkMode colors as darkMode colors for all tags
        Tag::migrateDarkColors();
        version = 10;
    }

    if (version < 11) {
        // create a case insensitive index
        queryDisk.exec("DROP INDEX IF EXISTS idxUniqueTag");
        queryDisk.exec("CREATE UNIQUE INDEX IF NOT EXISTS idxUniqueTag ON "
                               "tag (name COLLATE NOCASE, parent_id)");
        version = 11;
    }

    if (version < 12) {
        // create new tag table, because
        //     ALTER TABLE tag ADD updated DEFAULT CURRENT_TIMESTAMP
        // is not supported by sqlite -- you can't add a column with
        // a non-constant default value. And if collate ... is used
        // on a column, it's also defaulted to indices on that column.
        queryDisk.exec("ALTER TABLE tag RENAME TO _tag");
        queryDisk.exec("CREATE TABLE IF NOT EXISTS tag ("
                               "id INTEGER PRIMARY KEY,"
                               "name VARCHAR(255) COLLATE NOCASE,"
                               "priority INTEGER DEFAULT 0,"
                               "created DATETIME DEFAULT current_timestamp,"
                               "parent_id INTEGER DEFAULT 0,"
                               "color VARCHAR(20),"
                               "dark_color VARCHAR(20),"
                               "updated DATETIME DEFAULT current_timestamp)");

        // recreate the indices
        queryDisk.exec("DROP INDEX IF EXISTS idxUniqueTag");
        queryDisk.exec("CREATE UNIQUE INDEX IF NOT EXISTS idxUniqueTag ON "
                               "tag (name, parent_id)");
        queryDisk.exec("DROP INDEX IF EXISTS idxTagParent");
        queryDisk.exec("CREATE INDEX IF NOT EXISTS idxTagParent "
                               "ON tag( parent_id )");

        // convert old values to new table
        queryDisk.exec("INSERT INTO tag ( "
                           "id, name, priority, created, parent_id, "
                           "color, dark_color, updated "
                       ") SELECT "
                           "id, name, priority, created, parent_id, "
                           "color, dark_color, created "
                       "FROM _tag ORDER BY id");

        queryDisk.exec("DROP TABLE _tag");
        version = 12;
    }

    if (version < 13) {
        queryDisk.exec("CREATE TABLE IF NOT EXISTS trashItem ("
                               "id INTEGER PRIMARY KEY,"
                               "file_name VARCHAR(255),"
                               "file_size INTEGER,"
                               "note_sub_folder_path_data TEXT,"
                               "created DATETIME DEFAULT current_timestamp)");

        version = 13;
    }

    if (version != oldVersion) {
        setAppData("database_version",
                   QString::number(version), "note_folder");
    }

    closeDatabaseConnection(dbDisk, queryDisk);

    return true;
}

QSqlDatabase DatabaseService::getNoteFolderDatabase() {
#ifdef Q_OS_WIN32
    if (Utils::Misc::doAutomaticNoteFolderDatabaseClosing()) {
        // open database if it was closed in closeDatabaseConnection
        createNoteFolderConnection();
    }
#endif

    QSqlDatabase db = QSqlDatabase::database("note_folder");
//    db.transaction();
    return db;
}

/**
 * Closes a database connection if it was open
 *
 * @param db
 */
void DatabaseService::closeDatabaseConnection(QSqlDatabase &db,
                                              QSqlQuery &query) {
    query.finish();
    query.clear();

//    db.commit();
#ifdef Q_OS_WIN32
    if (Utils::Misc::doAutomaticNoteFolderDatabaseClosing()) {
        if (db.isOpen()) {
            db.close();
        }
    }
#else
    Q_UNUSED(db);
#endif
}

bool DatabaseService::setupTables() {
    QSqlDatabase dbDisk = QSqlDatabase::database("disk");
    QSqlQuery queryDisk(dbDisk);
    QSettings settings;

    queryDisk.exec("CREATE TABLE IF NOT EXISTS appData ("
                           "name VARCHAR(255) PRIMARY KEY, "
                           "value VARCHAR(255))");
    int version = getAppData("database_version").toInt();
    int oldVersion = version;
    qDebug() << __func__ << " - 'database_version': " << version;

    if (version > 0) {
        settings.setValue("guiFirstRunInit", true);
    }

    QSqlDatabase dbMemory = QSqlDatabase::database("memory");
    QSqlQuery queryMemory(dbMemory);
    queryMemory.exec("CREATE TABLE IF NOT EXISTS note ("
                             "id INTEGER PRIMARY KEY,"
                             "name VARCHAR(255),"
                             "file_name VARCHAR(255),"
                             "file_size INT64 DEFAULT 0,"
                             "note_sub_folder_id int,"
                             "note_text TEXT,"
                             "decrypted_note_text TEXT,"
                             "has_dirty_data INTEGER DEFAULT 0,"
                             "file_last_modified DATETIME,"
                             "file_created DATETIME,"
                             "crypto_key INT64 DEFAULT 0,"
                             "crypto_password VARCHAR(255),"
                             "share_url VARCHAR(255),"
                             "share_id int,"
                             "created DATETIME default current_timestamp,"
                             "modified DATETIME default current_timestamp)");
    queryMemory.exec("CREATE TABLE IF NOT EXISTS noteSubFolder ("
                             "id INTEGER PRIMARY KEY,"
                             "name VARCHAR(255),"
                             "parent_id int,"
                             "file_last_modified DATETIME,"
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
                               "ON calendarItem( url )");
        queryDisk.exec("ALTER TABLE calendarItem ADD completed_date DATETIME");
        queryDisk.exec("ALTER TABLE calendarItem "
                               "ADD sort_priority INTEGER DEFAULT 0");
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
        // remove the main splitter sizes for the tags pane
        settings.remove("mainSplitterSizes");

        version = 5;
    }

    if (version < 6) {
        // remove the obsolete activeTagId setting
        settings.remove("activeTagId");

        version = 6;
    }

    if (version < 7) {
        queryDisk.exec("ALTER TABLE noteFolder ADD active_tag_id INTEGER");
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

    if (version < 9) {
        queryDisk.exec("ALTER TABLE noteFolder ADD show_subfolders BOOLEAN "
                               "DEFAULT 0");
        version = 9;
    }

    if (version < 10) {
        queryDisk.exec("ALTER TABLE noteFolder "
                               "ADD active_note_sub_folder_data TEXT");
        version = 10;
    }

    if (version < 11) {
        // remove the oneColumnModeEnabled setting, that wrongly
        // was turned on by default
        settings.remove("oneColumnModeEnabled");

        version = 11;
    }

    if (version < 12) {
        bool darkModeColors = settings.value("darkModeColors").toBool();

        // set an initial schema key
        QString schemaKey = darkModeColors ?
                "EditorColorSchema-cdbf28fc-1ddc-4d13-bb21-6a4043316a2f":
                "EditorColorSchema-6033d61b-cb96-46d5-a3a8-20d5172017eb";
        settings.setValue("Editor/CurrentSchemaKey", schemaKey);

        version = 12;
    }

    if (version < 14) {
        // remove all calendar items so that all task items will be reloaded
        // from the server because we are using the display name now instead
        // of the last part of the calendar url
        CalendarItem::removeAll();

        // remove the todoCalendar list setting, so that the user has
        // to select the items again because the items will use the display
        // name now instead of the last part of the calendar url
        settings.remove("ownCloud/todoCalendarEnabledList");
        settings.remove("ownCloud/todoCalendarEnabledUrlList");
        settings.remove("ownCloud/todoCalendarUrlList");

        version = 14;
    }

    if (version < 15) {
        // turn off the DFM initially after the dock widget update
        settings.remove("DistractionFreeMode/isEnabled");

        // remove some deprecated settings
        settings.remove("MainWindow/windowState");
        settings.remove("windowState");
        settings.remove("markdownViewEnabled");
        settings.remove("tagsEnabled");
        settings.remove("noteEditPaneEnabled");
        settings.remove("dockWindowState");
        settings.remove("verticalPreviewModeEnabled");
        settings.remove("mainSplitterSizes");
        settings.remove("DistractionFreeMode/mainSplitterSizes");
        settings.remove("mainSplitterState-0-0-0-0");
        settings.remove("mainSplitterState-0-0-0-1");
        settings.remove("mainSplitterState-0-0-1-0");
        settings.remove("mainSplitterState-0-0-1-1");
        settings.remove("mainSplitterState-0-1-0-0");
        settings.remove("mainSplitterState-0-1-0-1");
        settings.remove("mainSplitterState-0-1-1-0");
        settings.remove("mainSplitterState-0-1-1-1");
        settings.remove("mainSplitterState-1-0-0-0");
        settings.remove("mainSplitterState-1-0-0-1");
        settings.remove("mainSplitterState-1-0-1-0");
        settings.remove("mainSplitterState-1-0-1-1");
        settings.remove("mainSplitterState-1-1-0-0");
        settings.remove("mainSplitterState-1-1-0-1");
        settings.remove("mainSplitterState-1-1-1-0");
        settings.remove("mainSplitterState-1-1-1-1");
        settings.remove("noteListSplitterState");
        settings.remove("tagFrameSplitterState");
        settings.remove("verticalNoteFrameSplitterState");

        version = 15;
    }

    if (version < 16) {
        // remove some deprecated settings
        settings.remove("dockWindowGeometry");
        settings.remove("MainWindow/showRecentNoteFolderInMainArea");

        version = 16;
    }

    if (version < 17) {
        // remove some deprecated settings
        settings.beginGroup("LogDialog");
        settings.remove("");
        settings.endGroup();

        settings.remove("LogWidget/geometry");
        settings.remove("LogWidget/showAtStartup");

        version = 17;
    }

    if (version < 18) {
        // set a new markdownHighlightingEnabled setting
        settings.setValue(
            "markdownHighlightingEnabled",
            settings.value("markdownHighlightingInterval", 200).toInt() > 0);

        // remove the deprecated markdownHighlightingInterval setting
        settings.remove("markdownHighlightingInterval");

        version = 18;
    }

    if (version < 19) {
        // set the ownCloud support enabled setting
        bool ownCloudEnabled = OwnCloudService::hasOwnCloudSettings(false);
        settings.setValue("ownCloud/supportEnabled", ownCloudEnabled);

        version = 19;
    }

    if (version < 20) {
#ifdef Q_OS_MAC
        // disable restoreCursorPosition for macOS by default, because there
        // are users where it causes troubles
        settings.setValue("restoreCursorPosition", false);
#endif

        version = 20;
    }

    if (version < 21) {
        // migrate to the new Portuguese translation
        QString locale = settings.value("interfaceLanguage").toString();
        if (locale == "pt") {
            settings.setValue("interfaceLanguage", "pt_BR");
        }

        version = 21;
    }

    if (version < 22) {
        queryDisk.exec("ALTER TABLE noteFolder ADD use_git BOOLEAN DEFAULT 0");
        version = 22;
    }

    if (version < 23) {
        queryDisk.exec("ALTER TABLE script ADD identifier VARCHAR(255)");
        queryDisk.exec("ALTER TABLE script ADD info_json TEXT");
        version = 23;
    }

    if (version < 24) {
        queryDisk.exec("ALTER TABLE script ADD settings_variables_json TEXT");
        version = 24;
    }

    if (version < 25) {
        // migrate old sort and order settings + set defaults if unset
        // if settings.s;
        if (settings.contains("SortingModeAlphabetically")) {
            bool sort = settings.value("SortingModeAlphabetically").toBool(); // read old setting
            settings.setValue("notesPanelSort",
                              sort ? SORT_ALPHABETICAL : SORT_BY_LAST_CHANGE);
            settings.remove("SortingModeAlphabetically");
        }

        if (settings.contains("NoteSortOrder")) {
            int order = static_cast<Qt::SortOrder>(settings.value("NoteSortOrder").toInt());
            settings.setValue("notesPanelOrder", order); // see defines in MainWindow.h
            settings.remove("NoteSortOrder");
        }

        // set defaults for now settings if not set already
        if (!settings.contains("notesPanelSort")) {
            settings.value("notesPanelSort", SORT_BY_LAST_CHANGE);
        }
        if (!settings.contains("notesPanelOrder")) {
            settings.value("notesPanelOrder", ORDER_DESCENDING);
        }

        if (!settings.contains("noteSubfoldersPanelSort")) {
            settings.value("noteSubfoldersPanelSort", SORT_BY_LAST_CHANGE);
        }
        if (!settings.contains("noteSubfoldersOrder")) {
            settings.value("noteSubfoldersOrder", ORDER_ASCENDING);
        }

        if (!settings.contains("tagsPanelSort")) {
            settings.value("tagsPanelSort", SORT_ALPHABETICAL);
        }
        if (!settings.contains("tagsPanelOrder")) {
            settings.value("tagsPanelOrder", ORDER_ASCENDING);
        }
        version = 25;
    }

    if (version < 26) {
        // remove setting with wrong default value
        settings.remove("localTrash/autoCleanupDays");

        version = 26;
    }

    if (version < 27) {
        // if the application was not started for the first time we want to
        // disable that the note edit is the central widget
        if (oldVersion != 0) {
            settings.setValue("noteEditIsCentralWidget", false);
        }

        version = 27;
    }

    if (version < 28) {
#ifndef Q_OS_MAC
        // we only want one app instance on Windows and Linux by default
        settings.setValue("allowOnlyOneAppInstance", true);
#endif

        version = 28;
    }

    if (version != oldVersion) {
        setAppData("database_version", QString::number(version));
    }

    return true;
}

bool DatabaseService::setAppData(const QString& name, const QString& value,
                                 const QString& connectionName) {
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);

    query.prepare("REPLACE INTO appData ( name, value ) "
                          "VALUES ( :name, :value )");
    query.bindValue(":name", name);
    query.bindValue(":value", value);
    return query.exec();
}

QString DatabaseService::getAppData(const QString& name, const QString& connectionName) {
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
