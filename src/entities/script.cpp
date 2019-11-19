/*
 * Copyright (c) 2014-2019 Patrizio Bekerle -- http://www.bekerle.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 */

#include "script.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QDir>
#include <QJsonDocument>
#include <utils/misc.h>
#include <QtCore/QJsonArray>
#include <services/metricsservice.h>
#include <services/updateservice.h>
#include <libraries/versionnumber/versionnumber.h>
#include <version.h>

const QString Script::ScriptRepositoryRawContentUrlPrefix =
        QStringLiteral("https://raw.githubusercontent.com/qownnotes/scripts/master/");


Script::Script() : id(0), name(QString()), scriptPath(QString()),
                   priority(0), enabled(true)
{
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

/**
 * Returns the directory of the script
 *
 * @return
 */
QString Script::getScriptDirPath() {
    QFileInfo info(scriptPath);
    return info.canonicalPath();
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

void Script::setName(const QString &text) {
    this->name = text;
}

void Script::setIdentifier(QString identifier) {
    this->identifier = std::move(identifier);
}

void Script::setInfoJson(QString infoJson) {
    this->infoJson = std::move(infoJson);
}

void Script::setSettingsVariablesJson(QString json) {
    this->settingsVariablesJson = std::move(json);
}

void Script::setSettingsVariablesJson(const QJsonObject &jsonObject) {
    QJsonDocument document(jsonObject);
    this->settingsVariablesJson = document.toJson();
}

void Script::setScriptPath(QString text) {
    this->scriptPath = std::move(text);
}

void Script::setPriority(int value) {
    this->priority = value;
}

void Script::setEnabled(bool value) {
    this->enabled = value;
}

bool Script::create(const QString& name, QString scriptPath) {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    query.prepare(QStringLiteral("INSERT INTO script ( name, script_path ) "
                          "VALUES ( :name, :scriptPath )"));
    query.bindValue(QStringLiteral(":name"), name);

    // make the path relative to the portable data path if we are in
    // portable mode
    query.bindValue(QStringLiteral(":scriptPath"),
                    Utils::Misc::makePathRelativeToPortableDataPathIfNeeded(
                            std::move(scriptPath)));

    return query.exec();
}

Script Script::fetch(int id) {
    Script script;
    script.fillFromId(id);
    return script;
}

bool Script::fillFromId(int id) {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    Script script;

    query.prepare(QStringLiteral("SELECT * FROM script WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        fillFromQuery(query);
        return true;
    }

    return false;
}

int Script::countAll() {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    query.prepare(QStringLiteral("SELECT COUNT(*) AS cnt FROM script"));

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        return query.value(QStringLiteral("cnt")).toInt();
    }

    return 0;
}

int Script::countEnabled() {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    query.prepare(QStringLiteral("SELECT COUNT(*) AS cnt FROM script WHERE enabled = 1"));

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        return query.value(QStringLiteral("cnt")).toInt();
    }

    return 0;
}

/**
 * Checks if a script identifier already exists
 *
 * @param identifier
 * @return
 */
bool Script::scriptFromRepositoryExists(const QString &identifier) {
    Script script = fetchByIdentifier(identifier);
    return script.isFetched();
}

/**
 * Fetches a script by its identifier
 *
 * @param identifier
 * @return
 */
Script Script::fetchByIdentifier(const QString& identifier) {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    Script script;

    query.prepare(QStringLiteral("SELECT * FROM script WHERE identifier = :identifier"));
    query.bindValue(QStringLiteral(":identifier"), identifier);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        script.fillFromQuery(query);
    }

    return script;
}

/**
 * Refetch the current object
 *
 * @return
 */
bool Script::refetch() {
    return fillFromId(id);
}

bool Script::scriptPathExists() {
    QFile file(scriptPath);
    return file.exists() && !scriptPath.isEmpty();
}

/**
 * Removes a script from the database
 * If the script was from the script repository also the local repository
 * path will be removed
 *
 * @return
 */
bool Script::remove() {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);
    QString path;
    bool isFromRepository = isScriptFromRepository();

    if (isFromRepository) {
        path = scriptRepositoryPath();
    }

    query.prepare(QStringLiteral("DELETE FROM script WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), this->id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else {
        // if the script was from the script repository remove also the
        // local path
        if (isFromRepository && !path.isEmpty()) {
            QDir dir(path);
            dir.removeRecursively();

            MetricsService::instance()->sendVisitIfEnabled(
                    QStringLiteral("script-repository/remove/") + identifier);
        }

        return true;
    }
}

Script Script::scriptFromQuery(const QSqlQuery& query) {
    Script script;
    script.fillFromQuery(query);
    return script;
}

bool Script::fillFromQuery(const QSqlQuery& query) {
    this->id = query.value(QStringLiteral("id")).toInt();
    this->name = query.value(QStringLiteral("name")).toString();
    this->identifier = query.value(QStringLiteral("identifier")).toString();
    this->infoJson = query.value(QStringLiteral("info_json")).toString();
    this->settingsVariablesJson =
            query.value(QStringLiteral("settings_variables_json")).toString();
    this->priority = query.value(QStringLiteral("priority")).toInt();
    this->enabled = query.value(QStringLiteral("enabled")).toBool();

    // prepend the portable data path if we are in portable mode
    this->scriptPath = Utils::Misc::prependPortableDataPathIfNeeded(
            query.value(QStringLiteral("script_path")).toString(), true);

    return true;
}

QList<Script> Script::fetchAll(bool enabledOnly) {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    QList<Script> scriptList;
    query.prepare(
            QStringLiteral("SELECT * FROM script %1 ORDER BY priority ASC, id ASC")
                    .arg(enabledOnly ? QStringLiteral("WHERE enabled = 1") : QString()));

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
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    if (this->id > 0) {
        query.prepare(
                QStringLiteral("UPDATE script SET name = :name, script_path = :scriptPath, "
                        "priority = :priority, enabled = :enabled, "
                        "identifier = :identifier, info_json = :info_json, "
                        "settings_variables_json = :settings_variables_json "
                        "WHERE id = :id"));
        query.bindValue(":id", this->id);
    } else {
        query.prepare(
                QStringLiteral("INSERT INTO script (name, script_path, "
                        "priority, enabled, identifier, info_json,"
                        "settings_variables_json) VALUES "
                        "(:name, :scriptPath, :priority, :enabled, "
                        ":identifier, :info_json, :settings_variables_json)"));
    }

    query.bindValue(QStringLiteral(":name"), this->name);
    query.bindValue(QStringLiteral(":priority"), this->priority);
    query.bindValue(QStringLiteral(":enabled"), this->enabled);
    query.bindValue(QStringLiteral(":identifier"), this->identifier);
    query.bindValue(QStringLiteral(":info_json"), this->infoJson);
    query.bindValue(QStringLiteral(":settings_variables_json"), this->settingsVariablesJson);

    // make the path relative to the portable data path if we are in
    // portable mode
    query.bindValue(QStringLiteral(":scriptPath"),
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
 * Returns the ScriptInfoJson object of the infoJson field
 *
 * @return
 */
ScriptInfoJson Script::getScriptInfoJson() {
    return ScriptInfoJson(getInfoJsonObject());
}

/**
 * Returns the json object of the settingsVariablesJson field
 *
 * @return
 */
QJsonObject Script::getSettingsVariablesJsonObject() {
    QJsonDocument jsonResponse = QJsonDocument::fromJson(
            settingsVariablesJson.toUtf8());
    return jsonResponse.object();
}

/**
 * Returns the json text of the settingsVariablesJson field
 *
 * @return
 */
QString Script::getSettingsVariablesJson() {
    return settingsVariablesJson;
}

/**
 * Returns the path where the script repositories will be stored locally
 *
 * @return
 */
QString Script::globalScriptRepositoryPath() {
    QString path = Utils::Misc::appDataPath() + QStringLiteral("/scripts");
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
QString Script::scriptRepositoryPath(bool removeRecursively) {
    if (identifier.isEmpty()) {
        return QString();
    }

    QString path = globalScriptRepositoryPath() + QStringLiteral("/") + identifier;
    QDir dir(path);

    // remove the old files in the script path
    if (removeRecursively) {
        dir.removeRecursively();
    }

    // create path if it doesn't exist yet
    dir.mkpath(path);
    return path;
}

/**
 * Checks if the script is a script from the sript repository
 *
 * @return
 */
bool Script::isScriptFromRepository() {
    return !identifier.isEmpty() || !infoJson.isEmpty();
}

/**
 * Returns the url of the script in the remote script repository
 *
 * @return
 */
QUrl Script::remoteScriptUrl() {
    QJsonObject jsonObject = getInfoJsonObject();
    QString scriptName = jsonObject.value(QStringLiteral("script")).toString();

    if (scriptName.isEmpty()) {
        return QUrl();
    }

    return remoteFileUrl(scriptName);
}

/**
 * Returns the url of a file of the script in the remote script repository
 *
 * @return
 */
QUrl Script::remoteFileUrl(const QString& fileName) {
    if (fileName.isEmpty()) {
        return QUrl();
    }

    return QUrl(QStringLiteral("https://raw.githubusercontent.com/qownnotes/scripts/master/")
           + identifier + QStringLiteral("/") + fileName);
}

/**
 * Returns a list of the remote script url and the resource file urls of the
 * script
 *
 * @return
 */
QList<QUrl> Script::remoteFileUrls() {
    QList<QUrl> urlList;
    ScriptInfoJson infoJson = getScriptInfoJson();
    QString scriptName = infoJson.script;

    if (!scriptName.isEmpty()) {
        urlList << remoteFileUrl(scriptName);
    }

    foreach( QString fileName, infoJson.resources ) {
            if (!fileName.isEmpty()) {
                urlList << remoteFileUrl(fileName);
            }
        }

    return urlList;
}



QDebug operator<<(QDebug dbg, const Script &script) {
    dbg.nospace() << "Script: <id>" << script.id << " <name>" <<
            script.name << " <scriptPath>" << script.scriptPath <<
            " <enabled>" << script.enabled <<
            " <priority>" << script.priority;
    return dbg.space();
}

/**
 * Aggregates the infoJson data from a QJsonObject
 *
 * @param jsonObject
 */
ScriptInfoJson::ScriptInfoJson(const QJsonObject& jsonObject) {
    if (jsonObject.isEmpty()) {
        return;
    }

    name = jsonObject.value(QStringLiteral("name")).toString();
    identifier = jsonObject.value(QStringLiteral("identifier")).toString();
    version = jsonObject.value(QStringLiteral("version")).toString();
    minAppVersion = jsonObject.value(QStringLiteral("minAppVersion")).toString();
    description = jsonObject.value(QStringLiteral("description")).toString();
    description = description.replace(QStringLiteral("\n"), QStringLiteral("<br>"));
    script = jsonObject.value(QStringLiteral("script")).toString();
    QJsonArray authors = jsonObject.value(QStringLiteral("authors")).toArray();
    QJsonArray platforms = jsonObject.value(QStringLiteral("platforms")).toArray();

    // generate the author list
    richAuthorList.clear();
    foreach(const QJsonValue &value, authors) {
            QString author = value.toString().trimmed();

            // create links to GitHub
            if (author.startsWith(QStringLiteral("@"))) {
                author = author.remove(0, 1);
                author = QStringLiteral("<a href='https://github.com/") + author +
                        QStringLiteral("'>") + author + QStringLiteral("</a>");
            }

            richAuthorList << author;
        }
    richAuthorText = richAuthorList.join(QStringLiteral(", "));

    // generate the platform list
    platformList.clear();
    richPlatformList.clear();
    foreach(const QJsonValue &value, platforms) {
            QString platform = value.toString().trimmed();
            platformList << platform;
        }
    if (platformList.count() == 0) {
        platformList << "linux" << "macos" << "windows";
    }
    QHash<QString, QString> platformHash;
    platformHash[QStringLiteral("linux")] = QStringLiteral("Linux");
    platformHash[QStringLiteral("macos")] = QStringLiteral("macOS");
    platformHash[QStringLiteral("windows")] = QStringLiteral("Windows");
    foreach(QString platform, platformList) {
            if (platformHash.contains(platform)) {
                richPlatformList << platformHash[platform];
            }

        }
    QString currentPlatform = QString(PLATFORM);
    if (currentPlatform == QStringLiteral("macosx")) {
        currentPlatform = QStringLiteral("macos");
    }
    platformSupported = platformList.contains(currentPlatform);
    richPlatformText = richPlatformList.join(QStringLiteral(", "));

    // get the resources file names
    QJsonArray resourcesArray = jsonObject.value(QStringLiteral("resources")).toArray();
    resources.clear();
    foreach(const QJsonValue &value, resourcesArray) {
            QString fileName = value.toString().trimmed();

            if (!fileName.isEmpty()) {
                resources << fileName;
            }
        }

    // check if app version is supported
    appVersionSupported = VersionNumber(VERSION) >=
            VersionNumber(minAppVersion);
}

/**
 * Returns the URL of the info.json file of the script repository
 *
 * @return
 */
QUrl Script::repositoryInfoJsonUrl() {
    return ScriptRepositoryRawContentUrlPrefix + getIdentifier() + QStringLiteral("/info.json");
}
