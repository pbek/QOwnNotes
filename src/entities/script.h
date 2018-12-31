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

#pragma once

#include <QSqlQuery>
#include <QList>
#include <QJsonObject>
#include <QUrl>
#include <QStringList>
#include <QtCore/QDir>

class ScriptInfoJson {
public:
    explicit ScriptInfoJson(QJsonObject jsonObject);

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
};

class Script
{
public:
    static const QString ScriptRepositoryRawContentUrlPrefix;

    explicit Script();

    int getId();
    static bool create(QString name, QString scriptPath);
    static Script fetch(int id);
    static Script scriptFromQuery(QSqlQuery query);
    bool store();
    friend QDebug operator<<(QDebug dbg, const Script &script);
    bool exists();
    bool fillFromQuery(QSqlQuery query);
    bool remove();
    bool isFetched();
    static QList<Script> fetchAll(bool enabledOnly = false);
    QString getName();
    QString getScriptPath();
    int getPriority();
    void setName(QString text);
    void setScriptPath(QString text);
    void setPriority(int value);
    static int countAll();
    bool scriptPathExists();
    void setEnabled(bool value);
    bool getEnabled();
    bool isEnabled();
    static int countEnabled();
    void setIdentifier(QString identifier);
    void setInfoJson(QString infoJson);
    QString getIdentifier();
    QJsonObject getInfoJsonObject();
    static QString globalScriptRepositoryPath();
    QString scriptRepositoryPath(bool removeRecursively = false);
    bool isScriptFromRepository();
    QUrl remoteScriptUrl();
    QUrl remoteFileUrl(QString fileName);
    static bool scriptFromRepositoryExists(QString identifier);
    void setSettingsVariablesJson(QString json);
    QString getSettingsVariablesJson();
    QJsonObject getSettingsVariablesJsonObject();
    void setSettingsVariablesJson(QJsonObject jsonObject);
    QString getScriptDirPath();
    QList<QUrl> remoteFileUrls();
    ScriptInfoJson getScriptInfoJson();
    static Script fetchByIdentifier(QString identifier);
    bool refetch();
    bool fillFromId(int id);
    QUrl repositoryInfoJsonUrl();

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
