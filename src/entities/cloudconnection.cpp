#include "cloudconnection.h"

#include <services/cryptoservice.h>
#include <utils/misc.h>

#include <QDebug>
#include <QRegularExpression>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QUrl>
#include <utility>

#include "notefolder.h"
#include "services/databaseservice.h"
#include "services/settingsservice.h"

CloudConnection::CloudConnection()
    : name(QString()), serverUrl(QString()), username(QString()), password(QString()) {
    id = 0;
    priority = 0;
}

int CloudConnection::getId() { return this->id; }

QString CloudConnection::getName() { return this->name; }

QString CloudConnection::getServerUrl() { return this->serverUrl; }

QString CloudConnection::getServerUrlPath() { return QUrl(this->serverUrl).path(); }

QString CloudConnection::getServerUrlWithoutPath() {
    QString serverUrlWithoutPath = serverUrl;
    const QString serverUrlPath = getServerUrlPath();

    if (!serverUrlPath.isEmpty()) {
        // remove the path from the server url
        serverUrlWithoutPath.replace(
            QRegularExpression(QRegularExpression::escape(serverUrlPath) + "$"), QString());
    }

    return serverUrlWithoutPath;
}

QString CloudConnection::getUsername() { return this->username; }

QString CloudConnection::getAccountId() { return this->accountId; }

QString CloudConnection::getPassword() { return this->password; }

bool CloudConnection::getAppQOwnNotesAPIEnabled() const { return this->appQOwnNotesAPIEnabled; }

bool CloudConnection::getNextcloudDeckEnabled() const {
    return extraSetting(QStringLiteral("nextcloudDeckEnabled"), false).toBool();
}

void CloudConnection::setNextcloudDeckEnabled(bool value) {
    setExtraSetting(QStringLiteral("nextcloudDeckEnabled"), value);
}

int CloudConnection::getNextcloudDeckBoardId() const {
    return extraSetting(QStringLiteral("nextcloudDeckBoardId"), 0).toInt();
}

void CloudConnection::setNextcloudDeckBoardId(int value) {
    setExtraSetting(QStringLiteral("nextcloudDeckBoardId"), value);
}

int CloudConnection::getNextcloudDeckStackId() const {
    return extraSetting(QStringLiteral("nextcloudDeckStackId"), 0).toInt();
}

void CloudConnection::setNextcloudDeckStackId(int value) {
    setExtraSetting(QStringLiteral("nextcloudDeckStackId"), value);
}

int CloudConnection::getPriority() const { return this->priority; }

CloudConnection CloudConnection::firstCloudConnection() {
    auto list = CloudConnection::fetchAll();
    return list.count() > 0 ? list[0] : CloudConnection();
}

CloudConnection CloudConnection::currentCloudConnection(bool ignoreTableWarning) {
    NoteFolder noteFolder = NoteFolder::currentNoteFolder();
    const int id = noteFolder.getCloudConnectionId();

    return CloudConnection::fetch(id, ignoreTableWarning);
}

CloudConnection CloudConnection::currentTodoCalendarCloudConnection() {
    SettingsService settings;
    const int id = settings
                       .value(QStringLiteral("ownCloud/todoCalendarCloudConnectionId"),
                              firstCloudConnection().getId())
                       .toInt();

    return CloudConnection::fetch(id);
}

void CloudConnection::setName(const QString &text) { this->name = text.trimmed(); }

void CloudConnection::setServerUrl(const QString &text) { this->serverUrl = text.trimmed(); }

void CloudConnection::setUsername(const QString &text) { this->username = text.trimmed(); }

void CloudConnection::setAccountId(const QString &text) { this->accountId = text.trimmed(); }

void CloudConnection::setPassword(const QString &text) { this->password = text.trimmed(); }

void CloudConnection::setPriority(int value) { this->priority = value; }
void CloudConnection::setAppQOwnNotesAPIEnabled(bool value) {
    this->appQOwnNotesAPIEnabled = value;
}

bool CloudConnection::create(const QString &name, const QString &serverUrl, const QString &username,
                             const QString &password) {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    query.prepare(
        "INSERT INTO cloudConnection ( name, server_url, "
        "username, password ) "
        "VALUES ( :name, :serverUrl, :username, "
        ":password )");
    query.bindValue(QStringLiteral(":name"), name);
    query.bindValue(QStringLiteral(":serverUrl"), serverUrl);
    query.bindValue(QStringLiteral(":username"), username);
    query.bindValue(QStringLiteral(":password"),
                    CryptoService::instance()->encryptToString(password));
    return query.exec();
}

CloudConnection CloudConnection::fetch(int id, bool ignoreTableWarning) {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    CloudConnection cloudConnection;

    query.prepare(QStringLiteral("SELECT * FROM cloudConnection WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), id);

    if (!query.exec()) {
        if (!ignoreTableWarning) {
            qWarning() << __func__ << ": " << query.lastError();
        }
    } else if (query.first()) {
        cloudConnection.fillFromQuery(query);
    }

    return cloudConnection;
}

int CloudConnection::countAll() {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    query.prepare(QStringLiteral("SELECT COUNT(*) AS cnt FROM cloudConnection"));

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        return query.value(QStringLiteral("cnt")).toInt();
    }

    return 0;
}

bool CloudConnection::remove() {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    query.prepare(QStringLiteral("DELETE FROM cloudConnection WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), this->id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else {
        removeExtraSettings();

        return true;
    }
}

void CloudConnection::removeExtraSettings() {
    SettingsService().remove(extraSettingsSettingsKey());
}

QString CloudConnection::extraSettingsSettingsKey() const {
    return QStringLiteral("CloudConnection-") + QString::number(this->id);
}

void CloudConnection::setExtraSetting(const QString &key, const QVariant &value) {
    SettingsService().setValue(extraSettingsSettingsKey() + QStringLiteral("/") + key, value);
}

QVariant CloudConnection::extraSetting(const QString &key, const QVariant &defaultValue) const {
    return SettingsService().value(extraSettingsSettingsKey() + QStringLiteral("/") + key,
                                   defaultValue);
}

CloudConnection CloudConnection::cloudConnectionFromQuery(const QSqlQuery &query) {
    CloudConnection cloudConnection;
    cloudConnection.fillFromQuery(query);
    return cloudConnection;
}

bool CloudConnection::fillFromQuery(const QSqlQuery &query) {
    this->id = query.value(QStringLiteral("id")).toInt();
    this->name = query.value(QStringLiteral("name")).toString();
    this->serverUrl = query.value(QStringLiteral("server_url")).toString();
    this->username = query.value(QStringLiteral("username")).toString();
    this->password = CryptoService::instance()->decryptToString(
        query.value(QStringLiteral("password")).toString());
    this->priority = query.value(QStringLiteral("priority")).toInt();

    const int databaseVersion =
        DatabaseService::getAppData(QStringLiteral("database_version")).toInt();

    this->accountId = databaseVersion >= 41 ? query.value(QStringLiteral("account_id")).toString()
                                            : QLatin1String("");

    this->appQOwnNotesAPIEnabled =
        databaseVersion >= 34 ? query.value(QStringLiteral("qownnotesapi_enabled")).toBool() : true;

    return true;
}

QList<CloudConnection> CloudConnection::fetchAll() {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    QList<CloudConnection> cloudConnectionList;

    query.prepare(QStringLiteral("SELECT * FROM cloudConnection ORDER BY priority ASC, id ASC"));
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            CloudConnection cloudConnection = cloudConnectionFromQuery(query);
            cloudConnectionList.append(cloudConnection);
        }
    }

    return cloudConnectionList;
}

/**
 * Inserts or updates a CloudConnection object in the database
 */
bool CloudConnection::store() {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    if (this->id > 0) {
        query.prepare(
            "UPDATE cloudConnection SET name = :name, server_url = :serverUrl, "
            "username = :username, account_id = :account_id, password = :password, "
            "priority = :priority, qownnotesapi_enabled = :qownnotesapi_enabled WHERE "
            "id = :id");
        query.bindValue(QStringLiteral(":id"), this->id);
    } else {
        query.prepare(
            "INSERT INTO cloudConnection (name, server_url, username, "
            "account_id, password, priority, qownnotesapi_enabled)"
            " VALUES (:name, :serverUrl, :username, :account_id, "
            ":password, :priority, :qownnotesapi_enabled)");
    }

    query.bindValue(QStringLiteral(":name"), this->name);
    query.bindValue(QStringLiteral(":serverUrl"), this->serverUrl);
    query.bindValue(QStringLiteral(":username"), this->username);
    query.bindValue(QStringLiteral(":account_id"), this->accountId);
    query.bindValue(QStringLiteral(":password"),
                    CryptoService::instance()->encryptToString(this->password));
    query.bindValue(QStringLiteral(":priority"), this->priority);
    query.bindValue(QStringLiteral(":qownnotesapi_enabled"), this->appQOwnNotesAPIEnabled);

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
 * Inserts or updates a CloudConnection object in the database (for the cloud connection migration)
 */
bool CloudConnection::storeMigratedCloudConnection() {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    if (this->id > 0) {
        query.prepare(
            "UPDATE cloudConnection SET name = :name, server_url = :serverUrl, "
            "username = :username, password = :password, "
            "priority = :priority WHERE "
            "id = :id");
        query.bindValue(QStringLiteral(":id"), this->id);
    } else {
        query.prepare(
            "INSERT INTO cloudConnection (name, server_url, username, "
            "password, priority)"
            " VALUES (:name, :serverUrl, :username, "
            ":password, :priority)");
    }

    query.bindValue(QStringLiteral(":name"), this->name);
    query.bindValue(QStringLiteral(":serverUrl"), this->serverUrl);
    query.bindValue(QStringLiteral(":username"), this->username);
    query.bindValue(QStringLiteral(":password"),
                    CryptoService::instance()->encryptToString(this->password));
    query.bindValue(QStringLiteral(":priority"), this->priority);

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
 * Checks if the current cloudConnection still exists in the database
 */
bool CloudConnection::exists() {
    CloudConnection cloudConnection = CloudConnection::fetch(this->id);
    return cloudConnection.id > 0;
}

bool CloudConnection::isFetched() { return (this->id > 0); }

/**
 * Checks if this note folder is the current one
 */
bool CloudConnection::isCurrent() { return currentCloudConnection().getId() == id; }

/**
 * Migrate the connections settings to a CloudConnection object
 */
bool CloudConnection::migrateToCloudConnections() {
    if (countAll() > 0) {
        return false;
    }

    SettingsService settings;
    const QString serverUrl = settings.value(QStringLiteral("ownCloud/serverUrl")).toString();
    const QString username = settings.value(QStringLiteral("ownCloud/userName")).toString();
    const QString password = CryptoService::instance()->decryptToString(
        settings.value(QStringLiteral("ownCloud/password")).toString());

    // create the default CloudConnection
    CloudConnection cloudConnection;
    cloudConnection.setName(QObject::tr("Default"));
    cloudConnection.setServerUrl(serverUrl);
    cloudConnection.setUsername(username);
    cloudConnection.setPassword(password);
    cloudConnection.setPriority(1);
    cloudConnection.storeMigratedCloudConnection();

    return true;
}

/**
 * @brief CloudConnection::fetchUsedCloudConnectionsIds returns a list of cloud
 * connection ids that are in use
 * @return
 */
QList<int> CloudConnection::fetchUsedCloudConnectionsIds() {
    // we want to prevent duplicates
    QSet<int> idList;

    const auto noteFolders = NoteFolder::fetchAll();
    for (const NoteFolder &noteFolder : noteFolders) {
        idList << noteFolder.getCloudConnectionId();
    }

    idList << CloudConnection::currentTodoCalendarCloudConnection().getId();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    return idList.values();
#else
    return idList.toList();
#endif
}

QDebug operator<<(QDebug dbg, const CloudConnection &cloudConnection) {
    dbg.nospace() << "CloudConnection: <id>" << cloudConnection.id << " <name>"
                  << cloudConnection.name << " <serverUrl>" << cloudConnection.serverUrl
                  << " <username>" << cloudConnection.username << " <accountId>"
                  << cloudConnection.accountId << " <priority>" << cloudConnection.priority;
    return dbg.space();
}
