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

#include "websocketserverservice.h"
#include <QtWebSockets>
#include <utils/misc.h>
#ifndef INTEGRATION_TESTS
#include <mainwindow.h>
#endif

using namespace std;

QT_USE_NAMESPACE

static QString getIdentifier(QWebSocket *peer) {
    if (peer == Q_NULLPTR) {
        return "";
    }

    return QStringLiteral("%1:%2").arg(peer->peerAddress().toString(),
                                       QString::number(peer->peerPort()));
}

WebSocketServerService::WebSocketServerService(quint16 port, QObject *parent) :
        QObject(parent),
        m_pWebSocketServer(new QWebSocketServer(
                QStringLiteral("QOwnNotes Server"),
                QWebSocketServer::NonSecureMode, this)) {
    listen(port);
}

void WebSocketServerService::listen(quint16 port) {
    m_port = 0;

    if (port == 0) {
        port = getSettingsPort();
    }

    if (m_pWebSocketServer->isListening()) {
        m_pWebSocketServer->close();
    }

    if (m_pWebSocketServer->listen(QHostAddress::Any, port)) {
        Utils::Misc::printInfo(tr("QOwnNotes server listening on port %1").arg(
                QString::number(port)));

        connect(m_pWebSocketServer, SIGNAL(newConnection()),
                this, SLOT(onNewConnection()));
        m_port = port;
    } else {
        qWarning() << tr("Could not start QOwnNotes server on port %1!").arg(
                QString::number(port));
    }
}

quint16 WebSocketServerService::getPort() {
    return m_port;
}

quint16 WebSocketServerService::getSettingsPort() {
    QSettings settings;
    quint16 port = static_cast<quint16>(settings.value(
            "webSocketServerService/port", getDefaultPort()).toULongLong());
    return port;
}

quint16 WebSocketServerService::getDefaultPort() {
    quint16 defaultPort = 22222;

#ifndef QT_NO_DEBUG
    defaultPort = 22223;
#endif

    return defaultPort;
}

WebSocketServerService::~WebSocketServerService() {
    m_pWebSocketServer->close();
}

void WebSocketServerService::onNewConnection() {
    auto pSocket = m_pWebSocketServer->nextPendingConnection();

    if (pSocket == Q_NULLPTR) {
        return;
    }

    Utils::Misc::printInfo(tr("%1 connected to QOwnNotes server!").arg(
            getIdentifier(pSocket)));
    pSocket->setParent(this);

    connect(pSocket, &QWebSocket::textMessageReceived,
            this, &WebSocketServerService::processMessage);
    connect(pSocket, &QWebSocket::disconnected,
            this, &WebSocketServerService::socketDisconnected);

    m_clients << pSocket;
}

void WebSocketServerService::processMessage(const QString &message) {
    QJsonDocument jsonResponse = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject jsonObject = jsonResponse.object();
    QString type = jsonObject.value("type").toString();

    if (type == "newNote") {
#ifndef INTEGRATION_TESTS
        MainWindow *mainWindow = MainWindow::instance();
        if (mainWindow == Q_NULLPTR) {
            return;
        }

        const QString contentType = jsonObject.value("contentType").toString();
        const QString name = Note::cleanupFileName(Note::extendedCleanupFileName(
                jsonObject.value("headline").toString())).trimmed();
        const QString text = jsonObject.value("text").toString().trimmed();
        const bool contentTypeIsHTML = contentType == "html";

        mainWindow->createNewNote(
                name,
                contentTypeIsHTML ? "" : text,
                MainWindow::CreateNewNoteOptions(
                        MainWindow::CreateNewNoteOption::UseNameAsHeadline));

        if (contentTypeIsHTML) {
            mainWindow->insertHtml(text);
        }
#endif
    } else {
        auto *pSender = qobject_cast<QWebSocket *>(sender());
        pSender->sendTextMessage("Received: " + message);
    }
}

void WebSocketServerService::socketDisconnected() {
    auto *pClient = qobject_cast<QWebSocket *>(sender());

    if (pClient != Q_NULLPTR) {
        Utils::Misc::printInfo(
                tr("%1 was disconnected from QOwnNotes server").arg(
                        getIdentifier(pClient)));

        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}