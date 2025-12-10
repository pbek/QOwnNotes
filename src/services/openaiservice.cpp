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

#include "openaiservice.h"

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QTimer>
#include <utility>

#include "cryptoservice.h"
#include "scriptingservice.h"
#include "services/settingsservice.h"

using namespace std;

QT_USE_NAMESPACE

OpenAiService::OpenAiService(QObject* parent) : QObject(parent) {
    initializeBackends();
    initializeCompleter(parent);

    QObject::connect(_completer, &OpenAiCompleter::completed, this, [this](const QString& result) {
        qDebug() << "'result': " << result;
        qDebug() << "OpenAiService - emitting autocompleteCompleted signal";
        emit autocompleteCompleted(result);
        qDebug() << "OpenAiService - autocompleteCompleted signal emitted";
    });
    QObject::connect(_completer, &OpenAiCompleter::errorOccurred, this,
                     [this](const QString& errorString) {
                         qDebug() << "'errorString': " << errorString;
                         emit autocompleteErrorOccurred(errorString);
                     });
}

void OpenAiService::initializeCompleter(QObject* parent) {
    _completer = new OpenAiCompleter(getApiKeyForCurrentBackend(), getModelId(),
                                     getApiBaseUrlForCurrentBackend(), parent);
}

/**
 * Fetches the global instance of the class
 * The instance will be created if it doesn't exist.
 */
OpenAiService* OpenAiService::instance() {
    auto* service = qApp->property("openAiService").value<OpenAiService*>();

    if (service == nullptr) {
        service = createInstance(nullptr);
    }

    return service;
}

int OpenAiService::getResponseTimeout() {
    return SettingsService().value(QStringLiteral("ai/responseTimeout"), 15).toInt();
}

/**
 * Creates a global instance of the class
 */
OpenAiService* OpenAiService::createInstance(QObject* parent) {
    auto* service = new OpenAiService(parent);
    qApp->setProperty("openAiService", QVariant::fromValue<OpenAiService*>(service));

    return service;
}

void OpenAiService::deleteInstance() {
    qApp->setProperty("openAiService", QVariant::fromValue<OpenAiService*>(nullptr));
}

void OpenAiService::initializeBackends() {
    _backendModels.clear();
    _backendModels[QStringLiteral("openai")] =
        QStringList{"gpt-4o",     "chatgpt-4o-latest", "gpt-4o-mini",   "o1-mini",
                    "o1-preview", "gpt-4-turbo",       "gpt-3.5-turbo", "gpt-4"};
    _backendModels[QStringLiteral("groq")] =
        QStringList{"llama-3.1-8b-instant",
                    "deepseek-r1-distill-llama-70b",
                    "gemma2-9b-it",
                    "llama-3.3-70b-versatile",
                    "qwen/qwen3-32b",
                    "groq/compound",
                    "groq/compound-mini",
                    "meta-llama/llama-4-maverick-17b-128e-instruct",
                    "meta-llama/llama-4-scout-17b-16e-instruct",
                    "meta-llama/llama-guard-4-12b",
                    "meta-llama/llama-prompt-guard-2-86m",
                    "meta-llama/llama-prompt-guard-2-22m",
                    "moonshotai/kimi-k2-instruct",
                    "moonshotai/kimi-k2-instruct-0905",
                    "openai/gpt-oss-120b",
                    "openai/gpt-oss-20b"};

    _backendApiBaseUrls.clear();
    _backendApiBaseUrls[QStringLiteral("openai")] =
        QStringLiteral("https://api.openai.com/v1/chat/completions");
    _backendApiBaseUrls[QStringLiteral("groq")] =
        QStringLiteral("https://api.groq.com/openai/v1/chat/completions");

    _backendApiKeys.clear();
    SettingsService settings;
    _backendApiKeys[QStringLiteral("groq")] = CryptoService::instance()->decryptToString(
        settings.value(getApiKeySettingsKeyForBackend(QStringLiteral("groq"))).toString());
    _backendApiKeys[QStringLiteral("openai")] = CryptoService::instance()->decryptToString(
        settings.value(getApiKeySettingsKeyForBackend(QStringLiteral("openai"))).toString());

    _backendNames.clear();
    _backendNames[QStringLiteral("groq")] = QStringLiteral("Groq");
    _backendNames[QStringLiteral("openai")] = QStringLiteral("OpenAI");

    const QList<QVariantMap> backends = ScriptingService::instance()->callOpenAiBackendsHook();
    //    qDebug() << __func__ << " - 'backends': " << backends;
    //    qDebug() << __func__ << " - 'backends count': " << backends.count();

    foreach (const QVariantMap& backend, backends) {
        const QString id = backend["id"].toString();
        const QString name = backend["name"].toString();
        const QString baseUrl = backend["baseUrl"].toString();
        const QString apiKey = backend["apiKey"].toString();
        const QStringList models = backend["models"].toStringList();
        qDebug() << __func__ << " - 'id': " << id;

        if (id.isEmpty() || name.isEmpty() || baseUrl.isEmpty() || models.isEmpty()) {
            continue;
        }

        _backendNames[id] = name;
        _backendApiBaseUrls[id] = baseUrl;
        _backendApiKeys[id] = apiKey;
        _backendModels[id] = models;
    }
}

QStringList OpenAiService::getModelsForCurrentBackend() {
    return getModelsForBackend(getBackendId());
}

QMap<QString, QString> OpenAiService::getBackendNames() { return _backendNames; }

QString OpenAiService::getApiBaseUrlForCurrentBackend() {
    return getApiBaseUrlForBackend(getBackendId());
}

QString OpenAiService::getApiBaseUrlForBackend(const QString& backendId) {
    if (_backendApiBaseUrls.contains(backendId)) {
        return _backendApiBaseUrls.value(backendId);
    } else {
        return {};
    }
}

QString OpenAiService::getApiKeyForCurrentBackend() {
    return _backendApiKeys.value(getBackendId());
}

QStringList OpenAiService::getModelsForBackend(const QString& backendId) {
    // Check if the getBackendId exists in the map
    if (_backendModels.contains(backendId)) {
        // If yes, return the associated list of models
        return _backendModels.value(backendId);
    } else {
        // If not, return an empty QStringList
        return {};
    }
}

bool OpenAiService::setBackendId(const QString& id) {
    if (this->_backendId == id) {
        return false;
    }

    SettingsService settings;
    this->_backendId = id;
    settings.setValue(QStringLiteral("ai/currentBackend"), id);
    // Reset model id, so it needs to be read again
    this->_modelId = QString();

    // Set new completer data
    this->_completer->setApiBaseUrl(getApiBaseUrlForCurrentBackend());
    setApiKeyForCurrentBackend();
    this->_completer->setModelId(getModelId());

    return true;
}

void OpenAiService::setApiKeyForCurrentBackend(const QString& apiKey) {
    _completer->setApiKey(apiKey);
}

void OpenAiService::setApiKeyForCurrentBackend() {
    setApiKeyForCurrentBackend(getApiKeyForCurrentBackend());
}

QString OpenAiService::getBackendId() {
    // If no backend id is set yet, try to read the settings
    if (this->_backendId.isEmpty()) {
        SettingsService settings;
        this->_backendId =
            settings.value(QStringLiteral("ai/currentBackend"), QStringLiteral("groq")).toString();
    }

    return this->_backendId;
}

bool OpenAiService::setModelId(const QString& id) {
    //    if (this->_modelId == id) {
    //        SettingsService settings;
    //        this->_modelId = settings.value(getCurrentModelSettingsKey(), id).toString();
    //    }

    // Check if model wasn't changed or doesn't exist
    if (this->_modelId == id || !_backendModels[getBackendId()].contains(id)) {
        return false;
    }

    this->_modelId = id;
    SettingsService settings;
    settings.setValue(getCurrentModelSettingsKey(), id);

    // Set new completer data
    this->_completer->setModelId(id);

    return true;
}

QString OpenAiService::getModelId() {
    // If not set yet try to read the settings
    if (this->_modelId.isEmpty()) {
        SettingsService settings;
        this->_modelId =
            settings.value(getCurrentModelSettingsKey(), _backendModels[getBackendId()]).toString();
    }

    // If still not set get the first of the models
    if (this->_modelId.isEmpty()) {
        const QStringList& models = getModelsForCurrentBackend();
        this->_modelId = models.isEmpty() ? QLatin1String("") : models.first();
    }

    return this->_modelId;
}

QString OpenAiService::getCurrentModelSettingsKey() {
    return QStringLiteral("ai/") + getBackendId() + QStringLiteral("/") +
           QStringLiteral("currentModel");
}

QString OpenAiService::getApiKeySettingsKeyForBackend(const QString& backendId) {
    return QStringLiteral("ai/") + backendId + QStringLiteral("/") + QStringLiteral("apiKey");
}

bool OpenAiService::setEnabled(bool enabled) {
    SettingsService settings;
    settings.setValue(QStringLiteral("ai/enabled"), enabled);

    return true;
}

bool OpenAiService::getEnabled() {
    SettingsService settings;
    return settings.value(QStringLiteral("ai/enabled")).toBool();
}

bool OpenAiService::setAutocompleteEnabled(bool enabled) {
    SettingsService settings;
    settings.setValue(QStringLiteral("ai/autocompleteEnabled"), enabled);

    return true;
}

bool OpenAiService::getAutocompleteEnabled() {
    SettingsService settings;
    return settings.value(QStringLiteral("ai/autocompleteEnabled"), false).toBool();
}

void OpenAiService::completeAsync(const QString& prompt) {
    qDebug() << __func__ << " - called with prompt:" << prompt.left(50);

    if (!getEnabled() || !getAutocompleteEnabled()) {
        qDebug() << __func__ << " - not enabled, returning";
        return;
    }

    qDebug() << __func__ << " - calling _completer->complete()";
    _completer->complete(prompt);
    qDebug() << __func__ << " - _completer->complete() called";
}

QString OpenAiService::complete(const QString& prompt) {
    if (!getEnabled()) {
        return tr("AI system is disabled. Please enable it in the main menu or the AI toolbar.");
    }

    return _completer->completeSync(prompt);
}

OpenAiCompleter::OpenAiCompleter(QString apiKey, QString modelId, QString apiBaseUrl,
                                 QObject* parent)
    : QObject(parent),
      apiKey(std::move(apiKey)),
      apiBaseUrl(std::move(apiBaseUrl)),
      modelId(std::move(modelId)) {
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this,
            &OpenAiCompleter::replyFinished);
}

void OpenAiCompleter::complete(const QString& prompt) {
    QUrl url(apiBaseUrl);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + apiKey).toUtf8());

    QJsonObject json;
    json["model"] = modelId;    // Use the modelId set in the constructor or by setModelId

    QJsonArray messagesArray;
    QJsonObject messageObject;
    messageObject["role"] = "user";
    messageObject["content"] = prompt;
    messagesArray.append(messageObject);

    json["messages"] = messagesArray;

    qDebug() << __func__ << " - 'json': " << json;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    networkManager->post(request, data);
}

QString OpenAiCompleter::completeSync(const QString& prompt) {
    auto* manager = new QNetworkAccessManager();
    QEventLoop loop;
    QTimer timer;

    timer.setSingleShot(true);

    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

    // 15 sec timeout for the response by default
    timer.start(OpenAiService::getResponseTimeout() * 1000);

    QUrl url(apiBaseUrl);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + apiKey).toUtf8());

    QJsonObject json;
    json["model"] = modelId;    // Use the modelId set in the constructor or by setModelId

    QJsonArray messagesArray;
    QJsonObject messageObject;
    messageObject["role"] = "user";
    messageObject["content"] = prompt;
    messagesArray.append(messageObject);

    json["messages"] = messagesArray;

    qDebug() << __func__ << " - 'json': " << json;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    QNetworkReply* reply = manager->post(request, data);
    loop.exec();

    // if we didn't get a timeout let us return the content
    if (timer.isActive()) {
        if (reply->error()) {
            // TODO: We need to mark that as an error!
            return reply->errorString();
        }

        QByteArray response_data = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response_data);
        qDebug() << __func__ << " - 'json': " << jsonDoc;

        QJsonObject jsonObject = jsonDoc.object();
        QJsonArray choices = jsonObject["choices"].toArray();

        // Initializing an empty result string to hold the eventual content.
        QString text = "";

        // Look through the choices (though typically there's only one)
        // and parse the nested message content.
        if (!choices.isEmpty()) {
            QJsonObject firstChoice = choices[0].toObject();

            // Check if 'message' field exists, ensuring compatibility with the new structure
            if (firstChoice.contains("message")) {
                QJsonObject message = firstChoice["message"].toObject();
                // Check if the role is "assistant" before extracting content
                if (message["role"].toString() == "assistant") {
                    text = message["content"].toString();
                }
            } else {    // Fallback to directly parse 'text' if present, for backward compatibility
                        // or other responses
                text = firstChoice["text"].toString();
            }

            return text.trimmed();
        } else {
            return {};
        }
    }

    reply->deleteLater();
    delete (manager);

    // TODO: How to mark timeouts?
    return tr("Timeout, while waiting for the AI response");
}

void OpenAiCompleter::setApiBaseUrl(const QString& url) { this->apiBaseUrl = url; }

void OpenAiCompleter::setApiKey(const QString& key) { this->apiKey = key; }

void OpenAiCompleter::setModelId(const QString& id) { this->modelId = id; }

void OpenAiCompleter::replyFinished(QNetworkReply* reply) {
    if (reply->error()) {
        emit errorOccurred(reply->errorString());
        return;
    }

    QByteArray response_data = reply->readAll();
    QJsonDocument json = QJsonDocument::fromJson(response_data);
    qDebug() << __func__ << " - 'json': " << json;

    QJsonObject jsonObject = json.object();
    QJsonArray choices = jsonObject["choices"].toArray();

    // Initializing an empty result string to hold the eventual content.
    QString text = "";

    // Look through the choices (though typically there's only one)
    // and parse the nested message content.
    if (!choices.isEmpty()) {
        QJsonObject firstChoice = choices[0].toObject();

        // Check if 'message' field exists, ensuring compatibility with the new structure
        if (firstChoice.contains("message")) {
            QJsonObject message = firstChoice["message"].toObject();
            // Check if the role is "assistant" before extracting content
            if (message["role"].toString() == "assistant") {
                text = message["content"].toString();
            }
        } else {    // Fallback to directly parse 'text' if present, for backward compatibility or
                    // other responses
            text = firstChoice["text"].toString();
        }

        emit completed(text.trimmed());
    } else {
        emit errorOccurred("No choices found in the response");
    }

    reply->deleteLater();
}
