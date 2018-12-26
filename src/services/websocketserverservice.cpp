/*
 * Copyright (c) 2014-2018 Patrizio Bekerle -- http://www.bekerle.com
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

using namespace std;

QT_USE_NAMESPACE

static QString getIdentifier(QWebSocket *peer) {
    return QStringLiteral("%1:%2").arg(peer->peerAddress().toString(),
                                       QString::number(peer->peerPort()));
}

WebSocketServerService::WebSocketServerService(quint16 port, QObject *parent) :
        QObject(parent),
        m_pWebSocketServer(new QWebSocketServer(
                QStringLiteral("QOwnNotes Server"),
                QWebSocketServer::NonSecureMode, this)) {

    if (port == 0) {
        QSettings settings;
        port = static_cast<quint16>(settings.value("webSocketServerService/port",
                22222).toULongLong());
    }
    if (m_pWebSocketServer->listen(QHostAddress::Any, port)) {
        Utils::Misc::printInfo(tr("QOwnNotes server listening on port %1").arg(
                QString::number(port)));

        connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
                this, &WebSocketServerService::onNewConnection);
    } else {
        qWarning() << tr("Could not start QOwnNotes server on port %1!").arg(
                QString::number(port));
    }
}

WebSocketServerService::~WebSocketServerService() {
    m_pWebSocketServer->close();
}

void WebSocketServerService::onNewConnection() {
    auto pSocket = m_pWebSocketServer->nextPendingConnection();
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
    auto *pSender = qobject_cast<QWebSocket *>(sender());
    for (QWebSocket *pClient : qAsConst(m_clients)) {
//        if (pClient != pSender) //don't echo message back to sender
            pClient->sendTextMessage(message);
    }

    pSender->sendTextMessage("Message sent!");
}

void WebSocketServerService::socketDisconnected() {
    auto *pClient = qobject_cast<QWebSocket *>(sender());
    Utils::Misc::printInfo(tr("%1 was disconnected from QOwnNotes server").arg(
            getIdentifier(pClient)));

    if (pClient) {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}