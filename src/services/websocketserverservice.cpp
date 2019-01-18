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
    if (Utils::Misc::isSocketServerEnabled()) {
        listen(port);
    }
}

void WebSocketServerService::listen(quint16 port) {
    if (port == 0) {
        port = getSettingsPort();
    }

    close();

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

void WebSocketServerService::close() {
    if (m_pWebSocketServer->isListening()) {
        m_pWebSocketServer->close();
        m_port = 0;
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
    } else if (type == "getBookmarks") {
#ifndef INTEGRATION_TESTS
//        pSender->sendTextMessage(
//                R"({ "type": "bookmarks", "data": [ { "name": "Test1", "url": "http://www.qownnotes.org" } ] })");

        QString jsonText = getBookmarksJsonText();

        if (jsonText.isEmpty()) {
            return;
        }

        auto *pSender = qobject_cast<QWebSocket *>(sender());
        pSender->sendTextMessage(jsonText);
#endif
    } else {
        auto *pSender = qobject_cast<QWebSocket *>(sender());
        pSender->sendTextMessage("Received: " + message);
    }
}

QString WebSocketServerService::getBookmarksJsonText() const {
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow == Q_NULLPTR) {
            return "";
        }

    Tag tag = Tag::fetchByName(getBookmarksTag());
    QList<Note> noteList = tag.fetchAllLinkedNotes();
    QList<Bookmark> bookmarks;

    // get all bookmark links from notes tagged with the bookmarks tag
    Q_FOREACH(Note note, noteList) {
            QList<Bookmark> noteBookmarks = note.getParsedBookmarks();

            Q_FOREACH(Bookmark bookmark, noteBookmarks) {
                        if (!bookmarks.contains(bookmark)) {
                            bookmarks.append(bookmark);
                        }
                }
        }

    // extract links from the current note
    QList<Bookmark> currentNoteBookmarks = Bookmark::parseBookmarks(
                mainWindow->activeNoteTextEdit()->toPlainText(), true);

    Q_FOREACH(Bookmark bookmark, currentNoteBookmarks) {
                if (!bookmarks.contains(bookmark)) {
                    bookmarks.append(bookmark);
                }
            }

    QString jsonText = Bookmark::bookmarksWebServiceJsonText(bookmarks);

    return jsonText;
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

QString WebSocketServerService::getBookmarksTag() {
    QSettings settings;
    QString bookmarksTag = settings.value(
            "webSocketServerService/bookmarksTag", "bookmarks").toString();
    return bookmarksTag;
}
