#include "script.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QDir>
#include <QJsonDocument>
#include <utils/misc.h>


Script::Script() {
    id = 0;
    name = "";
    scriptPath = "";
    priority = 0;
    enabled = true;
}

int Script::getId() {
    return this->id;
}

/**
 * Returns the script path
 * In portable mode the path will only be returned if it isn't just the data
 * path from the prepend function
 *
 * @return
 */
QString Script::getScriptPath() {
    return this->scriptPath;
}

QString Script::getName() {
    return this->name;
}

QString Script::getIdentifier() {
    return this->identifier;
}

int Script::getPriority() {
    return this->priority;
}

bool Script::getEnabled() {
    return this->enabled;
}

bool Script::isEnabled() {
    return getEnabled();
}

void Script::setName(QString text) {
    this->name = text;
}

void Script::setIdentifier(QString identifier) {
    this->identifier = identifier;
}

void Script::setInfoJson(QString infoJson) {
    this->infoJson = infoJson;
}

void Script::setScriptPath(QString text) {
    this->scriptPath = text;
}

void Script::setPriority(int value) {
    this->priority = value;
}

void Script::setEnabled(bool value) {
    this->enabled = value;
}

bool Script::create(QString name, QString scriptPath) {
    QSqlDatabase db = QSqlDatabase::database("disk");
    QSqlQuery query(db);

    query.prepare("INSERT INTO script ( name, script_path ) "
                          "VALUES ( :name, :scriptPath )");
    query.bindValue(":name", name);

    // make the path relative to the portable data path if we are in
    // portable mode
    query.bindValue(":scriptPath",
                    Utils::Misc::makePathRelativeToPortableDataPathIfNeeded(
                            scriptPath));

    return query.exec();
}

Script Script::fetch(int id) {
    QSqlDatabase db = QSqlDatabase::database("disk");
    QSqlQuery query(db);

    Script script;

    query.prepare("SELECT * FROM script WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        script.fillFromQuery(query);
    }

    return script;
}

int Script::countAll() {
    QSqlDatabase db = QSqlDatabase::database("disk");
    QSqlQuery query(db);

    query.prepare("SELECT COUNT(*) AS cnt FROM script");

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        return query.value("cnt").toInt();
    }

    return 0;
}

int Script::countEnabled() {
    QSqlDatabase db = QSqlDatabase::database("disk");
    QSqlQuery query(db);

    query.prepare("SELECT COUNT(*) AS cnt FROM script WHERE enabled = 1");

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        return query.value("cnt").toInt();
    }

    return 0;
}

bool Script::scriptPathExists() {
    QFile file(scriptPath);
    return file.exists() && !scriptPath.isEmpty();
}

bool Script::remove() {
    QSqlDatabase db = QSqlDatabase::database("disk");
    QSqlQuery query(db);

    query.prepare("DELETE FROM script WHERE id = :id");
    query.bindValue(":id", this->id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else {
        return true;
    }
}

Script Script::scriptFromQuery(QSqlQuery query) {
    Script script;
    script.fillFromQuery(query);
    return script;
}

bool Script::fillFromQuery(QSqlQuery query) {
    this->id = query.value("id").toInt();
    this->name = query.value("name").toString();
    this->identifier = query.value("identifier").toString();
    this->infoJson = query.value("info_json").toString();
    this->priority = query.value("priority").toInt();
    this->enabled = query.value("enabled").toBool();

    // prepend the portable data path if we are in portable mode
    this->scriptPath = Utils::Misc::prependPortableDataPathIfNeeded(
            query.value("script_path").toString(), true);

    return true;
}

QList<Script> Script::fetchAll(bool enabledOnly) {
    QSqlDatabase db = QSqlDatabase::database("disk");
    QSqlQuery query(db);

    QList<Script> scriptList;
    query.prepare(
            QString("SELECT * FROM script %1 ORDER BY priority ASC, id ASC")
                    .arg(enabledOnly ? "WHERE enabled = 1" : ""));

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            Script script = scriptFromQuery(query);
            scriptList.append(script);
        }
    }

    return scriptList;
}

/**
 * Inserts or updates a Script object in the database
 */
bool Script::store() {
    QSqlDatabase db = QSqlDatabase::database("disk");
    QSqlQuery query(db);

    if (this->id > 0) {
        query.prepare(
                "UPDATE script SET name = :name, script_path = :scriptPath, "
                        "priority = :priority, enabled = :enabled, "
                        "identifier = :identifier, info_json = :info_json "
                        "WHERE id = :id");
        query.bindValue(":id", this->id);
    } else {
        query.prepare(
                "INSERT INTO script (name, script_path, "
                        "priority, enabled, identifier, info_json) VALUES "
                        "(:name, :scriptPath, :priority, :enabled, "
                        ":identifier, :info_json)");
    }

    query.bindValue(":name", this->name);
    query.bindValue(":priority", this->priority);
    query.bindValue(":enabled", this->enabled);
    query.bindValue(":identifier", this->identifier);
    query.bindValue(":info_json", this->infoJson);

    // make the path relative to the portable data path if we are in
    // portable mode
    query.bindValue(":scriptPath",
                    Utils::Misc::makePathRelativeToPortableDataPathIfNeeded(
                            this->scriptPath));

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
 * Checks if the current script still exists in the database
 */
bool Script::exists() {
    Script script = Script::fetch(this->id);
    return script.id > 0;
}

bool Script::isFetched() {
    return (this->id > 0);
}

/**
 * Returns the json object of the infoJson field
 *
 * @return
 */
QJsonObject Script::getInfoJsonObject() {
    QJsonDocument jsonResponse = QJsonDocument::fromJson(infoJson.toUtf8());
    return jsonResponse.object();
}

/**
 * Returns the path where the script repositories will be stored locally
 *
 * @return
 */
QString Script::globalScriptRepositoryPath() {
    QString path = Utils::Misc::appDataPath() + "/scripts";
    QDir dir;

    // create path if it doesn't exist yet
    dir.mkpath(path);
    return path;
}

/**
 * Returns the path where the qml (and possibly other files) of the script
 * will be stored
 *
 * @return
 */
QString Script::scriptRepositoryPath() {
    QString path = globalScriptRepositoryPath() + "/" + identifier;
    QDir dir;

    // create path if it doesn't exist yet
    dir.mkpath(path);
    return path;
}

QDebug operator<<(QDebug dbg, const Script &script) {
    dbg.nospace() << "Script: <id>" << script.id << " <name>" <<
            script.name << " <scriptPath>" << script.scriptPath <<
            " <enabled>" << script.enabled <<
            " <priority>" << script.priority;
    return dbg.space();
}
