/*
 * Copyright (c) 2014-2026 Patrizio Bekerle -- <patrizio@bekerle.com>
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

#pragma once

#include <QJsonObject>
#include <QList>
#include <QSqlQuery>
#include <QStringList>
#include <QUrl>
#include <QtCore/QDir>

class ScriptInfoJson {
   public:
    explicit ScriptInfoJson();
    explicit ScriptInfoJson(const QJsonObject& jsonObject);
    friend QDebug operator<<(QDebug dbg, const ScriptInfoJson& bookmark);

    QString name;
    QString identifier;
    QString version;
    QString minAppVersion;
    QString script;
    QString description;
    QStringList richAuthorList;
    QStringList platformList;
    QStringList richPlatformList;
    QStringList resources;
    QString richAuthorText;
    QString richPlatformText;
    bool platformSupported;
    bool appVersionSupported;
    bool isEmpty() const;
};

class Script {
   public:
    static const QString ScriptRepositoryRawContentUrlPrefix;

    explicit Script();

    int getId() const;
    static bool create(const QString& name, QString scriptPath);
    static Script fetch(int id);
    static Script scriptFromQuery(const QSqlQuery& query);
    bool store();
    friend QDebug operator<<(QDebug dbg, const Script& script);
    bool exists() const;
    bool fillFromQuery(const QSqlQuery& query);
    bool remove() const;
    bool isFetched() const;
    static QList<Script> fetchAll(bool enabledOnly = false);
    QString getName() const;
    QString getScriptPath() const;
    int getPriority() const;
    void setName(const QString& text);
    void setScriptPath(QString text);
    void setPriority(int value);
    static int countAll();
    bool scriptPathExists() const;
    void setEnabled(bool value);
    bool getEnabled() const;
    bool isEnabled() const;
    static int countEnabled();
    void setIdentifier(QString identifier);
    void setInfoJson(QString infoJson);
    QString getIdentifier() const;
    QJsonObject getInfoJsonObject() const;
    static QString globalScriptRepositoryPath();
    QString scriptRepositoryPath(bool removeRecursively = false) const;
    bool isScriptFromRepository() const;
    QUrl remoteScriptUrl() const;
    QUrl remoteFileUrl(const QString& fileName) const;
    static bool scriptFromRepositoryExists(const QString& identifier);
    void setSettingsVariablesJson(QString json);
    QString getSettingsVariablesJson(bool hideSecrets) const;
    QJsonObject getSettingsVariablesJsonObject() const;
    void setSettingsVariablesJson(const QJsonObject& jsonObject);
    QString getScriptDirPath() const;
    QList<QUrl> remoteFileUrls() const;
    ScriptInfoJson getScriptInfoJson() const;
    static Script fetchByIdentifier(const QString& identifier);
    bool refetch();
    bool fillFromId(int id);
    QUrl repositoryInfoJsonUrl() const;

   private:
    int id;
    QString name;
    QString identifier;
    QString infoJson;
    QString settingsVariablesJson;
    QString scriptPath;
    int priority;
    bool enabled;
};
