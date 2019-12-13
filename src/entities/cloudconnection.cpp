#include "cloudconnection.h"
#include "notefolder.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSettings>
#include <utility>
#include <QRegularExpression>
#include <utils/misc.h>
#include <services/cryptoservice.h>


CloudConnection::CloudConnection()
    : name(QString()), serverUrl(QString()), username(QString()), password(QString())
{
    id = 0;
    priority = 0;
}

int CloudConnection::getId() {
    return this->id;
}

QString CloudConnection::getName() {
    return this->name;
}

QString CloudConnection::getServerUrl() {
    return this->serverUrl;
}

QString CloudConnection::getServerUrlPath() {
    return QUrl(this->serverUrl).path();
}

QString CloudConnection::getServerUrlWithoutPath() {
    QString serverUrlWithoutPath = serverUrl;
    const QString serverUrlPath = getServerUrlPath();

    if (!serverUrlPath.isEmpty()) {
        // remove the path from the server url
        serverUrlWithoutPath.replace(QRegularExpression(
                QRegularExpression::escape(serverUrlPath) + "$"), QString());
    }

    return serverUrlWithoutPath;
}

QString CloudConnection::getUsername() {
    return this->username;
}

QString CloudConnection::getPassword() {
    return this->password;
}

int CloudConnection::getPriority() {
    return this->priority;
}

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
    QSettings settings;
    const int id = settings.value("ownCloud/todoCalendarCloudConnectionId", firstCloudConnection().getId()).toInt();

    return CloudConnection::fetch(id);
}

void CloudConnection::setName(QString text) {
    this->name = text.trimmed();
}

void CloudConnection::setServerUrl(QString text) {
    this->serverUrl = text.trimmed();
}

void CloudConnection::setUsername(QString text) {
    this->username = text.trimmed();
}

void CloudConnection::setPassword(QString text) {
    this->password = text.trimmed();
}

void CloudConnection::setPriority(int value) {
    this->priority = value;
}

bool CloudConnection::create(QString name, QString serverUrl,
                        QString username, QString password) {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    query.prepare("INSERT INTO cloudConnection ( name, server_url, "
                          "username, password ) "
                          "VALUES ( :name, :serverUrl, :username, "
                          ":password )");
    query.bindValue(":name", name);
    query.bindValue(":serverUrl", serverUrl);
    query.bindValue(":username", username);
    query.bindValue(":password", CryptoService::instance()->encryptToString(password));
    return query.exec();
}

CloudConnection CloudConnection::fetch(int id, bool ignoreTableWarning) {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    CloudConnection cloudConnection;

    query.prepare("SELECT * FROM cloudConnection WHERE id = :id");
    query.bindValue(":id", id);

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

    query.prepare("SELECT COUNT(*) AS cnt FROM cloudConnection");

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        return query.value("cnt").toInt();
    }

    return 0;
}

bool CloudConnection::remove() {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    query.prepare("DELETE FROM cloudConnection WHERE id = :id");
    query.bindValue(":id", this->id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else {
        return true;
    }
}

CloudConnection CloudConnection::cloudConnectionFromQuery(const QSqlQuery& query) {
    CloudConnection cloudConnection;
    cloudConnection.fillFromQuery(query);
    return cloudConnection;
}

bool CloudConnection::fillFromQuery(const QSqlQuery& query) {
    this->id = query.value("id").toInt();
    this->name = query.value("name").toString();
    this->serverUrl = query.value("server_url").toString();
    this->username = query.value("username").toString();
    this->password = CryptoService::instance()->decryptToString(query.value("password").toString());
    this->priority = query.value("priority").toInt();

    return true;
}

QList<CloudConnection> CloudConnection::fetchAll() {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    QList<CloudConnection> cloudConnectionList;

    query.prepare("SELECT * FROM cloudConnection ORDER BY priority ASC, id ASC");
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
                        "username = :username, password = :password, "
                        "priority = :priority WHERE "
                        "id = :id");
        query.bindValue(":id", this->id);
    } else {
        query.prepare(
                "INSERT INTO cloudConnection (name, server_url, username, "
                        "password, priority)"
                        " VALUES (:name, :serverUrl, :username, "
                        ":password, :priority)");
    }

    query.bindValue(":name", this->name);
    query.bindValue(":serverUrl", this->serverUrl);
    query.bindValue(":username", this->username);
    query.bindValue(":password", CryptoService::instance()->encryptToString(this->password));
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
 * Checks if the current cloudConnection still exists in the database
 */
bool CloudConnection::exists() {
    CloudConnection cloudConnection = CloudConnection::fetch(this->id);
    return cloudConnection.id > 0;
}

bool CloudConnection::isFetched() {
    return (this->id > 0);
}

/**
 * Checks if this note folder is the current one
 */
bool CloudConnection::isCurrent() {
    return currentCloudConnection().getId() == id;
}

/**
 * Migrate the connections settings to a CloudConnection object
 */
bool CloudConnection::migrateToCloudConnections() {
    if (countAll() > 0) {
        return false;
    }

    QSettings settings;
    const QString serverUrl = settings.value("ownCloud/serverUrl").toString();
    const QString username = settings.value("ownCloud/userName").toString();
    const QString password = CryptoService::instance()->decryptToString(
            settings.value("ownCloud/password").toString());

    // create the default CloudConnection
    CloudConnection cloudConnection;
    cloudConnection.setName(QObject::tr("Default"));
    cloudConnection.setServerUrl(serverUrl);
    cloudConnection.setUsername(username);
    cloudConnection.setPassword(password);
    cloudConnection.setPriority(1);
    cloudConnection.store();

    return true;
}

/**
 * @brief CloudConnection::fetchUsedCloudConnectionsIds returns a list of cloud connection ids that are in use
 * @return
 */
QList<int> CloudConnection::fetchUsedCloudConnectionsIds() {
    // we want to prevent duplicates
    QSet<int> idList;

    Q_FOREACH(NoteFolder noteFolder, NoteFolder::fetchAll()) {
        idList << noteFolder.getCloudConnectionId();
    }

    idList << CloudConnection::currentTodoCalendarCloudConnection().getId();

    return idList.toList();
}

QDebug operator<<(QDebug dbg, const CloudConnection &cloudConnection) {
    dbg.nospace() << "CloudConnection: <id>" << cloudConnection.id << " <name>" <<
            cloudConnection.name << " <serverUrl>" << cloudConnection.serverUrl <<
            " <username>" << cloudConnection.username <<
            " <priority>" << cloudConnection.priority;
    return dbg.space();
}
