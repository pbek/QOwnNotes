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
#include "metricsservice.h"
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
    auto *pSender = qobject_cast<QWebSocket *>(sender());
    MetricsService::instance()->sendVisitIfEnabled("websocket/message/" + type);

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

        pSender->sendTextMessage(jsonText);
#endif
    } else if (type == "newBookmarks") {
        QJsonArray bookmarkList = createBookmarks(jsonObject);

        pSender->sendTextMessage(flashMessageJsonText(
                tr("%n bookmark(s) created", "", bookmarkList.count())));
    } else if (type == "switchNoteFolder") {
#ifndef INTEGRATION_TESTS
        MainWindow *mainWindow = MainWindow::instance();

        if (mainWindow == Q_NULLPTR) {
            return;
        }

        const int noteFolderId = jsonObject.value("data").toInt();

        if (noteFolderId == NoteFolder::currentNoteFolderId()) {
            return;
        }

        mainWindow->changeNoteFolder(noteFolderId);
        QString jsonText = getBookmarksJsonText();

        pSender->sendTextMessage(jsonText);
#endif
    } else {
        pSender->sendTextMessage("Received: " + message);
    }
}

QJsonArray WebSocketServerService::createBookmarks(const QJsonObject &jsonObject) {
    const QString bookmarksNoteName = getBookmarksNoteName();
    Note bookmarksNote = Note::fetchByName(bookmarksNoteName);
    bool applyTag = false;

    // create new bookmarks note if it doesn't exist
    if (!bookmarksNote.isFetched()) {
        bookmarksNote.setName(bookmarksNoteName);
        bookmarksNote.setNoteText(Note::createNoteHeader(bookmarksNoteName));
        applyTag = true;
    }

    QString noteText = bookmarksNote.getNoteText().trimmed();
    const QJsonArray bookmarkList = jsonObject.value("data").toArray();

    Q_FOREACH(QJsonValue bookmarkObject, bookmarkList) {
            const QJsonObject data = bookmarkObject.toObject();
            const QString name = data.value("name").toString().trimmed().remove(
                    "[").remove("]");
            const QString url = data.value("url").toString().trimmed();
            const QString description = data.value(
                    "description").toString().trimmed();

            noteText += "\n- [" + name + "](" + url + ")";

            if (!description.isEmpty()) {
                noteText += " " + description;
            }
        }

    noteText += "\n";
    bookmarksNote.setNoteText(noteText);
    bookmarksNote.store();
    bookmarksNote.storeNoteTextFileToDisk();

    if (applyTag) {
        auto tag = Tag::fetchByName(getBookmarksTag());
        if (tag.isFetched()) {
            tag.linkToNote(bookmarksNote);
        }
    }

    return bookmarkList;
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

            // merge bookmark lists
            Bookmark::mergeListInList(noteBookmarks, bookmarks);
        }

    // extract links from the current note
    QList<Bookmark> currentNoteBookmarks = Bookmark::parseBookmarks(
                mainWindow->activeNoteTextEdit()->toPlainText(), true);

    // merge bookmark lists
    Bookmark::mergeListInList(currentNoteBookmarks, bookmarks);

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

QString WebSocketServerService::getBookmarksNoteName() {
    QSettings settings;
    QString bookmarksNoteName = settings.value(
            "webSocketServerService/bookmarksNoteName", "Bookmarks").toString();
    return bookmarksNoteName;
}

QString WebSocketServerService::flashMessageJsonText(QString message) {
    QJsonObject resultObject;
    resultObject.insert("type", QJsonValue::fromVariant("flashMessage"));
    resultObject.insert("message", message);
    resultObject.insert("noteFolderName", NoteFolder::currentNoteFolder().getName());

    QJsonDocument doc(resultObject);

    return doc.toJson(QJsonDocument::Compact);
}
