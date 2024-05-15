/*
 * Copyright (c) 2014-2024 Patrizio Bekerle -- <patrizio@bekerle.com>
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

#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>

class OpenAiService : public QObject {
    Q_OBJECT
   public:
    explicit OpenAiService(QObject* parent = nullptr);
    QStringList getModelsForBackend(const QString& backendId);
    QStringList getModelsForCurrentBackend();
    static OpenAiService* instance();
    bool setBackendId(const QString& id);
    QString getBackendId();
    bool setModelId(const QString& id);
    QString getModelId();
    static bool setEnabled(bool enabled);
    static bool getEnabled();

   private:
    QMap<QString, QStringList> backendModels;
    void initializeBackendModels();
    static OpenAiService* createInstance(QObject* parent);
    QString _backendId;
    QString _modelId;
    QString getCurrentModelSettingsKey();
};

class OpenAiCompleter : public QObject {
    Q_OBJECT
   public:
    explicit OpenAiCompleter(QString apiKey, QString modelId,
                             QString apiBaseUrl = "https://api.openai.com/v1/completions",
                             QObject* parent = nullptr);
    void complete(const QString& prompt);
    void setApiBaseUrl(const QString& url);
    void setModelId(const QString& id);

   signals:
    void completed(QString result);
    void errorOccurred(QString errorString);

   private slots:
    void replyFinished(QNetworkReply* reply);

   private:
    QNetworkAccessManager* networkManager;
    QString apiKey;
    QString apiBaseUrl;    // Store the API base URL
    QString modelId;       // Model ID used for API requests
};
