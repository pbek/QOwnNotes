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

#include "webappclientservice.h"

#ifndef INTEGRATION_TESTS
#include <mainwindow.h>
#include <utils/gui.h>
#endif

#include <utils/misc.h>

#include <QApplication>
#include <QBuffer>
#include <QClipboard>
#include <QJsonDocument>
#include <QSslError>
#include <QWebSocket>
#include <QtWebSockets>

#include "metricsservice.h"
#include "services/settingsservice.h"

using namespace std;

QT_USE_NAMESPACE

WebAppClientService::WebAppClientService(QObject *parent) : QObject(parent) {
    if (!Utils::Misc::isWebAppSupportEnabled()) {
        return;
    }

    _webSocket = new QWebSocket();
    _heartbeatText = "qon-ping";
    generateSessionId();

    connect(_webSocket, &QWebSocket::connected, this, &WebAppClientService::onConnected);
    connect(_webSocket, &QWebSocket::disconnected, this, &WebAppClientService::onDisconnected);
    connect(_webSocket, &QWebSocket::sslErrors, this, &WebAppClientService::onSslErrors);
    connect(_webSocket, &QWebSocket::textMessageReceived, this,
            &WebAppClientService::onTextMessageReceived);
    connect(&_timerHeartbeat, SIGNAL(timeout()), this, SLOT(onSendHeartbeatText()));
    connect(&_timerReconnect, SIGNAL(timeout()), this, SLOT(onReconnect()));

    open();

    initClipboardService();
}

void WebAppClientService::open() {
    _url = getServerUrl() + "/ws/" + getOrGenerateToken();
    // we don't want to show the token in the log
    qDebug() << "Opening socket connection to " << getServerUrl();
    _webSocket->open(_url);
}

void WebAppClientService::close() {
    _webSocket->close();
    _url = "";
}

/**
 * Keeps the current clipboard content
 *
 * @return true if something was kept
 */
bool WebAppClientService::keepClipboard() {
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    const QPixmap pixmap = clipboard->pixmap();
    if (!pixmap.isNull()) {
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);
        pixmap.save(&buffer, "PNG");

        _clipboardMimeType = "image/png";
        _clipboardContent = byteArray.toBase64();
    } else if (mimeData->hasHtml()) {
        _clipboardMimeType = "text/html";
        _clipboardContent = clipboard->text();
    } else if (mimeData->hasText()) {
        _clipboardMimeType = "text/plain";
        _clipboardContent = clipboard->text();
        // qDebug() << __func__ << "_clipboardContent: " << _clipboardContent;
    } else {
        return false;
    }

    return true;
}

bool WebAppClientService::sendClipboard() const {
    if (_clipboardContent == "" || _clipboardMimeType == "") {
        return false;
    }

    qDebug() << __func__ << "_clipboardMimeType: " << _clipboardMimeType;
    qDebug() << __func__ << "_clipboardContent: " << _clipboardContent;

    sendInsertIntoClipboard(_clipboardMimeType, _clipboardContent);
    return true;
}

void WebAppClientService::initClipboardService() {
    QClipboard *clipboard = QApplication::clipboard();

    // React to clipboard changes
    connect(clipboard, &QClipboard::dataChanged, [this]() {
        // We need to store the clipboard ourselves to preserve external clipboard changes we would
        // not catch otherwise
        keepClipboard();
    });
}

void WebAppClientService::sendInsertIntoClipboard(const QString &mimeType,
                                                  const QString &content) const {
    if (!_webSocket->isValid()) {
        return;
    }

    QJsonObject jsonObject;
    jsonObject["command"] = "insertIntoClipboard";
    jsonObject["mimeType"] = mimeType;
    jsonObject["sessionId"] = _sessionId;
    jsonObject["content"] = content;

    QJsonDocument jsonDoc(jsonObject);
    _webSocket->sendTextMessage(jsonDoc.toJson(QJsonDocument::Compact));
}

QString WebAppClientService::getServerUrl() {
    return SettingsService()
        .value(QStringLiteral("webAppClientService/serverUrl"), getDefaultServerUrl())
        .toString();
}

QString WebAppClientService::getOrGenerateToken() {
    QString token = SettingsService().value(QStringLiteral("webAppClientService/token")).toString();

    // if not token was set
    if (token.isEmpty()) {
        token = Utils::Misc::generateRandomString(32);
        SettingsService().setValue(QStringLiteral("webAppClientService/token"), token);
    }

    return token;
}

void WebAppClientService::generateSessionId() {
    _sessionId = Utils::Misc::generateRandomString(20);
}

QString WebAppClientService::getDefaultServerUrl() {
#ifdef QT_NO_DEBUG
    return "wss://app.qownnotes.org";
#else
    return "ws://localhost:8080";
#endif
}

WebAppClientService::~WebAppClientService() {
    _timerHeartbeat.stop();
    _timerReconnect.stop();
    _webSocket->close();
}

void WebAppClientService::onConnected() {
    _timerHeartbeat.start(_heartbeatTime);
    _timerReconnect.stop();
    _reconnectFailedCount = 0;
    _heartbeatFailedCount = 0;

    Utils::Misc::printInfo(
        tr("QOwnNotes is now connected via websocket to %1").arg(getServerUrl()));
}

void WebAppClientService::onDisconnected() {
    // try to connect again after a time
    _timerHeartbeat.stop();
    _timerReconnect.start(_reconnectTime);

    Utils::Misc::printInfo(
        tr("QOwnNotes is now disconnected from websocket to %1").arg(getServerUrl()));
}

void WebAppClientService::onTextMessageReceived(const QString &message) {
    QJsonDocument jsonResponse = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject jsonObject = jsonResponse.object();
    const QString command = jsonObject.value(QStringLiteral("command")).toString();
    MetricsService::instance()->sendVisitIfEnabled("webapp/command/" + command);
    qDebug() << __func__ << "message: " << message;
    qDebug() << __func__ << "command: " << command;

    if (message == _heartbeatText) {
        // Ignore heartbeat messages
        return;
    }

    if (command == "showWarning") {
        const QString msg = jsonObject.value(QStringLiteral("msg")).toString();
        qWarning() << "Web app warning: " << msg;

#ifndef INTEGRATION_TESTS
        Utils::Gui::warning(nullptr, tr("Web app warning"), msg, "wepappclientservice-warning");
#endif
    } else if (command == "insertFile") {
#ifndef INTEGRATION_TESTS
        MainWindow *mainWindow = MainWindow::instance();
        if (mainWindow == nullptr) {
            return;
        }

        const QString fileDataUrl = jsonObject.value(QStringLiteral("file")).toString();

        if (Utils::Gui::question(mainWindow, tr("Image received"),
                                 tr("Insert image received from QOwnNotes web application?"),
                                 QStringLiteral("webapp-insert-image")) == QMessageBox::Yes) {
            // insert image into current note
            mainWindow->insertDataUrlAsFileIntoCurrentNote(fileDataUrl);
        }

        _webSocket->sendTextMessage(R"({"command": "confirmInsert"})");
#endif
    } else if (command == "insertIntoClipboard") {
        const QString sessionId = jsonObject.value(QStringLiteral("sessionId")).toString();

        // Skip messages from our own session to prevent clipboard loops
        if (sessionId == _sessionId) {
            qDebug() << "Skipping insertIntoClipboard from own session";
            return;
        }

        QClipboard *clipboard = QApplication::clipboard();
        const QString mimeType = jsonObject.value(QStringLiteral("mimeType")).toString();
        const QString content = jsonObject.value(QStringLiteral("content")).toString();
        qDebug() << __func__ << "mimeType: " << mimeType;
        qDebug() << __func__ << "content: " << content.left(200);

#ifndef INTEGRATION_TESTS
        MainWindow *mainWindow = MainWindow::instance();

        if (mimeType == "text/plain") {
            clipboard->setText(content, QClipboard::Clipboard);
            mainWindow->showStatusBarMessage(
                tr("Text received from web app and copied to clipboard"), QStringLiteral("📋"),
                5000);
        } else if (mimeType == "text/html") {
            clipboard->setText(content, QClipboard::Clipboard);
            mainWindow->showStatusBarMessage(
                tr("HTML received from web app and copied to clipboard"), QStringLiteral("📋"),
                5000);
        } else if (mimeType == "image/png") {
            const QByteArray imageData = QByteArray::fromBase64(content.toUtf8());
            QImage image;
            image.loadFromData(imageData);
            clipboard->setImage(image, QClipboard::Clipboard);
            mainWindow->showStatusBarMessage(
                tr("Image received from web app and copied to clipboard"), QStringLiteral("📋"),
                5000);
        } else {
            qWarning() << "Unknown mime data type from web app: " << mimeType;
        }
#endif
    } else {
        qWarning() << "Unknown message from web app: " << message;
    }
}

void WebAppClientService::onSslErrors(const QList<QSslError> &errors) { qCritical() << errors; }

void WebAppClientService::onSendHeartbeatText() {
    auto sendByte = _webSocket->sendTextMessage(_heartbeatText);

    if (sendByte != _heartbeatText.toLocal8Bit().length()) {
        _heartbeatFailedCount++;
        qDebug() << "WebAppClientService heartbeat failed";

        if (_heartbeatFailedCount >= _reconnectHeartbeatTimerCount) {
            _timerHeartbeat.stop();
            _timerReconnect.start(_reconnectTime);
        }
    }
}

void WebAppClientService::onReconnect() {
    // close websocket
    _webSocket->close();

    if (_reconnectFailedCount >= _reconnectHeartbeatTimerCount) {
        _timerReconnect.stop();
    } else {
        qDebug() << "WebAppClientService attempts a reconnect";

        open();
        _reconnectFailedCount++;
    }
}
