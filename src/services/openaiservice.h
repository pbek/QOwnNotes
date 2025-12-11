/*
 * Copyright (c) 2014-2025 Patrizio Bekerle -- <patrizio@bekerle.com>
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
#include <functional>

#include "services/settingsservice.h"

class OpenAiCompleter : public QObject {
    Q_OBJECT
   public:
    explicit OpenAiCompleter(QString apiKey, QString modelId,
                             QString apiBaseUrl = "https://api.openai.com/v1/completions",
                             QObject* parent = nullptr);
    void complete(const QString& prompt);
    void setApiBaseUrl(const QString& url);
    void setApiKey(const QString& key);
    void setModelId(const QString& id);
    QString completeSync(const QString& prompt);

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

class OpenAiService : public QObject {
    Q_OBJECT
   public:
    explicit OpenAiService(QObject* parent = nullptr);
    QStringList getModelsForBackend(const QString& backendId);
    QStringList getModelsForCurrentBackend();
    static OpenAiService* instance();
    static int getResponseTimeout();
    static void deleteInstance();
    bool setBackendId(const QString& id);
    QString getBackendId();
    bool setModelId(const QString& id);
    QString getModelId();
    static bool setEnabled(bool enabled);
    static bool getEnabled();
    static bool setAutocompleteEnabled(bool enabled);
    static bool getAutocompleteEnabled();
    QString complete(const QString& prompt);
    void completeAsync(const QString& prompt);
    void setApiKeyForCurrentBackend();
    void setApiKeyForCurrentBackend(const QString& apiKey);
    QMap<QString, QString> getBackendNames();
    QString getApiBaseUrlForBackend(const QString& backendId);

    // Callback for autocomplete results (avoids circular dependency)
    using AutocompleteCallback = std::function<void(const QString&)>;
    void setAutocompleteCallback(AutocompleteCallback callback);

   signals:
    void autocompleteCompleted(const QString& result);
    void autocompleteErrorOccurred(const QString& errorString);

   private:
    QMap<QString, QStringList> _backendModels;
    QMap<QString, QString> _backendApiBaseUrls;
    QMap<QString, QString> _backendApiKeys;
    QMap<QString, QString> _backendNames;
    void initializeBackends();
    static OpenAiService* createInstance(QObject* parent);
    QString _backendId;
    QString _modelId;
    OpenAiCompleter* _completer = nullptr;
    QString getCurrentModelSettingsKey();
    static QString getApiKeySettingsKeyForBackend(const QString& backendId);
    void initializeCompleter(QObject* parent);
    QString getApiBaseUrlForCurrentBackend();
    AutocompleteCallback _autocompleteCallback;
    QString getApiKeyForCurrentBackend();
};
