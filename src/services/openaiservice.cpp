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

#include "openaiservice.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QSettings>
#include <QCoreApplication>
#include <utility>

#include "cryptoservice.h"

using namespace std;

QT_USE_NAMESPACE

OpenAiService::OpenAiService(QObject* parent) : QObject(parent) {
    initializeBackendModels();
    QSettings settings;
    auto apiKey = CryptoService::instance()->decryptToString(
        settings.value(QStringLiteral("ai/groqApiKey")).toString());
    auto _completer = new OpenAiCompleter(
        apiKey, "llama3-8b-8192", "https://api.groq.com/openai/v1/chat/completions", parent);

    _completer->complete("Who am I?");

    QObject::connect(_completer, &OpenAiCompleter::completed, this,
                     [this](const QString& result) { qDebug() << "'result': " << result; });

    QObject::connect(
        _completer, &OpenAiCompleter::errorOccurred, this,
        [this](const QString& errorString) { qDebug() << "'errorString': " << errorString; });
}

/**
 * Fetches the global instance of the class
 * The instance will be created if it doesn't exist.
 */
OpenAiService *OpenAiService::instance() {
    auto * service = qApp->property("openAiService").value<OpenAiService *>();

    if (service == nullptr) {
        service = createInstance(nullptr);
    }

    return service;
}

/**
 * Creates a global instance of the class
 */
OpenAiService *OpenAiService::createInstance(QObject *parent) {
    auto *service = new OpenAiService(parent);

    qApp->setProperty("openAiService", QVariant::fromValue<OpenAiService *>(service));

    return service;
}

void OpenAiService::initializeBackendModels() {
    backendModels[QStringLiteral("groq")] = QStringList{"llama3-70b-8192", "llama3-8b-8192", "llama2-70b-4096", "mixtral-8x7b-32768", "gemma-7b-it"};
}

QStringList OpenAiService::getModelsForBackend(const QString& backendId) {
    // Check if the backendId exists in the map
    if(backendModels.contains(backendId)) {
        // If yes, return the associated list of models
        return backendModels.value(backendId);
    } else {
        // If not, return an empty QStringList
        return {};
    }
}

void OpenAiService::setBackendId(const QString& id) { this->backenId = id; }
void OpenAiService::setModelId(const QString& id) { this->modelId = id; }


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

void OpenAiCompleter::setApiBaseUrl(const QString& url) { this->apiBaseUrl = url; }

void OpenAiCompleter::setModelId(const QString& id) { this->modelId = id; }

void OpenAiCompleter::replyFinished(QNetworkReply* reply) {
    if (reply->error()) {
        emit errorOccurred(reply->errorString());
        return;
    }

    QByteArray response_data = reply->readAll();
    QJsonDocument json = QJsonDocument::fromJson(response_data);
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
