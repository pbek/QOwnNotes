/*
 * Copyright (c) 2014-2026 Patrizio Bekerle -- <patrizio@bekerle.com>
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

#include <utils/misc.h>

#include <QtWebSockets>

#include "dialogs/websockettokendialog.h"
#include "entities/bookmark.h"
#include "entities/commandsnippet.h"
#include "entities/notefolder.h"
#include "entities/tag.h"
#include "metricsservice.h"
#include "services/settingsservice.h"
#include "widgets/qownnotesmarkdowntextedit.h"
#ifndef INTEGRATION_TESTS
#include <mainwindow.h>
#include <services/scriptingservice.h>
#endif

using namespace std;

QT_USE_NAMESPACE

static QString getIdentifier(QWebSocket *peer) {
    if (peer == nullptr) {
        return {};
    }

    return QStringLiteral("%1:%2").arg(peer->peerAddress().toString(),
                                       QString::number(peer->peerPort()));
}

WebSocketServerService::WebSocketServerService(quint16 port, QObject *parent)
    : QObject(parent),
      m_pWebSocketServer(new QWebSocketServer(QStringLiteral("QOwnNotes Server"),
                                              QWebSocketServer::NonSecureMode, this)) {
#ifndef INTEGRATION_TESTS
    _webSocketTokenDialog = nullptr;
#endif

    if (Utils::Misc::isSocketServerEnabled()) {
        listen(port);
    }
}

void WebSocketServerService::listen(quint16 port) {
    if (port == 0) {
        port = getSettingsPort();
    }

    close();

    if (m_pWebSocketServer->listen(QHostAddress::LocalHost, port)) {
        Utils::Misc::printInfo(
            tr("QOwnNotes server listening on port %1").arg(QString::number(port)));

        connect(m_pWebSocketServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
        m_port = port;
    } else {
        qWarning() << tr("Could not start QOwnNotes server on port %1!").arg(QString::number(port));
    }
}

void WebSocketServerService::close() {
    if (m_pWebSocketServer->isListening()) {
        m_pWebSocketServer->close();
        m_port = 0;
    }
}

quint16 WebSocketServerService::getPort() const { return m_port; }

quint16 WebSocketServerService::getSettingsPort() {
    SettingsService settings;
    quint16 port = static_cast<quint16>(
        settings.value(QStringLiteral("webSocketServerService/port"), getDefaultPort())
            .toULongLong());
    return port;
}

quint16 WebSocketServerService::getDefaultPort() {
    // use port 22223 in debug mode
#ifndef QT_NO_DEBUG
    return 22223;
#else
    return 22222;
#endif
}

WebSocketServerService::~WebSocketServerService() { m_pWebSocketServer->close(); }

void WebSocketServerService::onNewConnection() {
    auto pSocket = m_pWebSocketServer->nextPendingConnection();

    if (pSocket == nullptr) {
        return;
    }

    Utils::Misc::printInfo(tr("%1 connected to QOwnNotes server!").arg(getIdentifier(pSocket)));
    pSocket->setParent(this);

    connect(pSocket, &QWebSocket::textMessageReceived, this,
            &WebSocketServerService::processMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &WebSocketServerService::socketDisconnected);

    m_clients << pSocket;
}

void WebSocketServerService::processMessage(const QString &message) {
    QJsonDocument jsonResponse = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject jsonObject = jsonResponse.object();
    QString type = jsonObject.value(QStringLiteral("type")).toString();
    auto *pSender = qobject_cast<QWebSocket *>(sender());
    MetricsService::instance()->sendVisitIfEnabled("websocket/message/" + type);
    const QString token = jsonObject.value(QStringLiteral("token")).toString();
    SettingsService settings;
    QString storedToken = settings.value(QStringLiteral("webSocketServerService/token")).toString();

    // request the token if not set
    if (token.isEmpty() || storedToken.isEmpty() || token != storedToken) {
        pSender->sendTextMessage(getTokenQueryJsonText());

#ifndef INTEGRATION_TESTS
        if (_webSocketTokenDialog == nullptr) {
            _webSocketTokenDialog = new WebSocketTokenDialog();
        }

        if (!_webSocketTokenDialog->isVisible()) {
            _webSocketTokenDialog->open();
        }
#endif

        return;
    }

    const bool isHandleRawData = type == QLatin1String("handleRawData");

    if ((type == QLatin1String("newNote")) || isHandleRawData) {
#ifndef INTEGRATION_TESTS
        MainWindow *mainWindow = MainWindow::instance();
        if (mainWindow == nullptr) {
            return;
        }

        const QString pageUrl = jsonObject.value(QStringLiteral("pageUrl")).toString().trimmed();
        const QString contentType = jsonObject.value(QStringLiteral("contentType")).toString();
        const QString name =
            Note::cleanupFileName(Note::extendedCleanupFileName(
                                      jsonObject.value(QStringLiteral("headline")).toString()))
                .trimmed();
        QString text = jsonObject.value(QStringLiteral("text")).toString().trimmed();

        // Attempt to transform links starting with a "/" like "/my-page.html" to
        // "http://domain.com/my-page.html"
        text = Utils::Misc::createAbsolutePathsInHtml(text, pageUrl);

        const bool contentTypeIsHTML = contentType == QLatin1String("html");
        bool isCreateNewNote = true;

        // if the type was "handleRawData" let's check if there is a
        // websocketRawDataHook used in a script
        if (isHandleRawData) {
            const QString requestType =
                jsonObject.value(QStringLiteral("requestType")).toString().trimmed();
            const QString pageTitle =
                jsonObject.value(QStringLiteral("pageTitle")).toString().trimmed();
            const QString rawData =
                jsonObject.value(QStringLiteral("rawData")).toString().trimmed();
            const QString screenshotDataUrl =
                jsonObject.value(QStringLiteral("screenshotDataUrl")).toString().trimmed();

            // call websocketRawDataHook
            isCreateNewNote = !ScriptingService::instance()->callHandleWebsocketRawDataHook(
                requestType, pageUrl, pageTitle, rawData, screenshotDataUrl);
        }

        if (isCreateNewNote) {
            mainWindow->createNewNote(name, contentTypeIsHTML ? QString() : text,
                                      MainWindow::CreateNewNoteOptions(
                                          MainWindow::CreateNewNoteOption::UseNameAsHeadline));

            if (contentTypeIsHTML) {
                mainWindow->insertHtmlAsMarkdownIntoCurrentNote(std::move(text));
            }
        }
#endif
    } else if (type == QLatin1String("getBookmarks")) {
#ifndef INTEGRATION_TESTS
        //        pSender->sendTextMessage(
        //                R"({ "type": "bookmarks", "data": [ { "name": "Test1",
        //                "url": "https://www.qownnotes.org" } ] })");

        const bool hideCurrent = jsonObject.value(QStringLiteral("hideCurrent")).toBool();
        QString jsonText = getBookmarksJsonText(hideCurrent);

        if (jsonText.isEmpty()) {
            return;
        }

        pSender->sendTextMessage(jsonText);
#endif
    } else if (type == QLatin1String("newBookmarks")) {
        QJsonArray bookmarkList = createBookmarks(jsonObject);

        pSender->sendTextMessage(
            flashMessageJsonText(tr("%n bookmark(s) created", "", bookmarkList.count())));

        // Reload current note if the bookmark was deleted from the current note
        MainWindow *mainWindow = MainWindow::instance();
        if (mainWindow != nullptr) {
            if (getBookmarksNoteName() == mainWindow->getCurrentNote().getName()) {
                mainWindow->reloadCurrentNoteByNoteId(true);
            }
        }
    } else if (type == QLatin1String("switchNoteFolder")) {
#ifndef INTEGRATION_TESTS
        MainWindow *mainWindow = MainWindow::instance();

        if (mainWindow == nullptr) {
            pSender->sendTextMessage(getNoteFolderSwitchedJsonText(false));

            return;
        }

        const int noteFolderId = jsonObject.value(QStringLiteral("data")).toInt();

        if (noteFolderId == NoteFolder::currentNoteFolderId()) {
            pSender->sendTextMessage(getNoteFolderSwitchedJsonText(false));

            return;
        }

        const bool switched = mainWindow->changeNoteFolder(noteFolderId);

        pSender->sendTextMessage(getNoteFolderSwitchedJsonText(switched));

        const bool hideCurrent = jsonObject.value(QStringLiteral("hideCurrent")).toBool();
        QString jsonText = getBookmarksJsonText(hideCurrent);
        pSender->sendTextMessage(jsonText);
#endif
    } else if (type == QLatin1String("getNoteFolders")) {
#ifndef INTEGRATION_TESTS
        QString jsonText = getNoteFoldersJsonText();

        if (jsonText.isEmpty()) {
            return;
        }

        pSender->sendTextMessage(jsonText);
#endif
    } else if (type == QLatin1String("getCommandSnippets")) {
#ifndef INTEGRATION_TESTS
        QString jsonText = getCommandSnippetsJsonText();

        if (jsonText.isEmpty()) {
            return;
        }

        pSender->sendTextMessage(jsonText);
#endif
    } else if (type == QLatin1String("deleteBookmark")) {
        MainWindow *mainWindow = MainWindow::instance();
        if (mainWindow == nullptr) {
            return;
        }

        const int noteCount = deleteBookmark(jsonObject);

        pSender->sendTextMessage(
            flashMessageJsonText(tr("Bookmark deleted from %n note(s)", "", noteCount)));

        // Reload current note in case the bookmark was deleted from the current note
        mainWindow->reloadCurrentNoteByNoteId(true);
    } else if (type == QLatin1String("editBookmark")) {
        MainWindow *mainWindow = MainWindow::instance();
        if (mainWindow == nullptr) {
            return;
        }

        const int noteCount = editBookmark(jsonObject);

        pSender->sendTextMessage(
            flashMessageJsonText(tr("Bookmark edited in %n note(s)", "", noteCount)));

        // Reload current note in case the bookmark was edited in the current note
        mainWindow->reloadCurrentNoteByNoteId(true);
    } else {
        QJsonObject resultObject;
        resultObject.insert(QStringLiteral("type"), QJsonValue::fromVariant("unknownMessage"));
        resultObject.insert(QStringLiteral("data"), message);

        QJsonDocument doc(resultObject);
        pSender->sendTextMessage(doc.toJson(QJsonDocument::Compact));
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
    QJsonArray bookmarkList = jsonObject.value(QStringLiteral("data")).toArray();

    Q_FOREACH (QJsonValue bookmarkObject, bookmarkList) {
        const QJsonObject data = bookmarkObject.toObject();
        const QString name = data.value(QStringLiteral("name"))
                                 .toString()
                                 .trimmed()
                                 .remove(QStringLiteral("["))
                                 .remove(QStringLiteral("]"));
        const QString url = data.value(QStringLiteral("url")).toString().trimmed();
        const QString description = data.value(QStringLiteral("description")).toString().trimmed();

        noteText += "\n- [" + name + "](" + url + ")";

        if (!description.isEmpty()) {
            noteText += " " + description;
        }
    }

    noteText += QLatin1String("\n");
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

/**
 * Deletes a bookmark (as Markdown) from all notes tagged as bookmarks
 *
 * @param jsonObject
 * @return
 */
int WebSocketServerService::deleteBookmark(const QJsonObject &jsonObject) {
    // Get the "data" object first
    QJsonObject dataObject = jsonObject.value("data").toObject();

    // Get the "markdown" attribute from the "data" object
    QString markdown = dataObject.value("markdown").toString().trimmed();

    if (markdown.isEmpty()) {
        return 0;
    }

    // Search for the Markdown text in all notes with the "bookmarks" tag
    Tag tag = Tag::fetchByName(getBookmarksTag());
    QVector<Note> noteList = tag.fetchAllLinkedNotes();
    int noteCount = 0;

    for (Note &note : noteList) {
        auto noteText = note.getNoteText();
        if (noteText.contains(markdown)) {
            // Remove the bookmark from the note (try "\n" and without "\n")
            noteText.remove(markdown + QStringLiteral("\n"));
            noteText.remove(markdown);
            note.setNoteText(noteText);
            note.store();
            note.storeNoteTextFileToDisk();
            noteCount++;
        }
    }

    // Remove the Markdown text in the current note
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != nullptr) {
        //        auto note = mainWindow->getCurrentNote();
        //        auto textBefore = note.hasEncryptedNoteText() && note.canDecryptNoteText()
        //                           ? note.fetchDecryptedNoteText()
        //                           : note.getNoteText();
        auto textBefore = mainWindow->activeNoteTextEdit()->toPlainText();
        auto textAfter = textBefore;
        // Remove the bookmark from the note (try "\n" and without "\n")
        textAfter.remove(markdown + QStringLiteral("\n"));
        textAfter.remove(markdown);

        if (textBefore != textAfter) {
            //            note.setDecryptedNoteText(textAfter);
            mainWindow->allowNoteEditing();
            mainWindow->activeNoteTextEdit()->setPlainText(textAfter);
            noteCount++;

            auto note = mainWindow->getCurrentNote();
            if (note.hasEncryptedNoteText()) {
                mainWindow->editEncryptedNoteAsync();
            }
        }
    }

    return noteCount;
}

/**
 * Edits a bookmark (as Markdown) in all notes tagged as bookmarks
 *
 * @param jsonObject
 * @return
 */
int WebSocketServerService::editBookmark(const QJsonObject &jsonObject) {
    // Get the "data" object first
    QJsonObject dataObject = jsonObject.value("data").toObject();

    // Get the "markdown" attribute from the "data" object
    QString markdown = dataObject.value("markdown").toString().trimmed();

    // Make sure there was no newline character in the string
    // https://github.com/pbek/QOwnNotes/issues/3105
    QString newMarkdown = dataObject.value("newMarkdown").toString().remove(QChar('\n')).trimmed();

    if (markdown.isEmpty() || newMarkdown.isEmpty()) {
        return 0;
    }

    // Search for the Markdown text in all notes with the "bookmarks" tag
    Tag tag = Tag::fetchByName(getBookmarksTag());
    QVector<Note> noteList = tag.fetchAllLinkedNotes();
    int noteCount = 0;

    for (Note &note : noteList) {
        auto noteText = note.getNoteText();
        if (noteText.contains(markdown)) {
            // Replace the bookmark in the note (try "\n" and without "\n")
            noteText.replace(markdown + QStringLiteral("\n"), newMarkdown + QStringLiteral("\n"));
            note.setNoteText(noteText);
            note.store();
            note.storeNoteTextFileToDisk();
            noteCount++;
        }
    }

    // Replace the Markdown text in the current note
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow != nullptr) {
        auto textBefore = mainWindow->activeNoteTextEdit()->toPlainText();
        auto textAfter = textBefore;
        // Replace the bookmark in the note (try "\n" and without "\n")
        textAfter.replace(markdown + QStringLiteral("\n"), newMarkdown + QStringLiteral("\n"));

        if (textBefore != textAfter) {
            mainWindow->allowNoteEditing();
            mainWindow->activeNoteTextEdit()->setPlainText(textAfter);
            noteCount++;

            auto note = mainWindow->getCurrentNote();
            if (note.hasEncryptedNoteText()) {
                mainWindow->editEncryptedNoteAsync();
            }
        }
    }

    return noteCount;
}

QString WebSocketServerService::getBookmarksJsonText(bool hideCurrent) {
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow == nullptr) {
        return {};
    }

    Tag tag = Tag::fetchByName(getBookmarksTag());
    const QVector<Note> noteList = tag.fetchAllLinkedNotes();
    QVector<Bookmark> bookmarks;

    // get all bookmark links from notes tagged with the bookmarks tag
    for (const Note &note : noteList) {
        QVector<Bookmark> noteBookmarks = note.getParsedBookmarks();

        // merge bookmark lists
        Bookmark::mergeListInList(noteBookmarks, bookmarks);
    }

    if (!hideCurrent) {
        // extract links from the current note
        QVector<Bookmark> currentNoteBookmarks =
            Bookmark::parseBookmarks(mainWindow->activeNoteTextEdit()->toPlainText(), true);

        // merge bookmark lists
        Bookmark::mergeListInList(currentNoteBookmarks, bookmarks);
    }

    QString jsonText = Bookmark::bookmarksWebServiceJsonText(bookmarks);

    return jsonText;
}

QString WebSocketServerService::getNoteFoldersJsonText() {
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow == nullptr) {
        return {};
    }

    QJsonArray noteFolderObjectList;

    const auto noteFolders = NoteFolder::fetchAll();
    for (const NoteFolder &noteFolder : noteFolders) {
        noteFolderObjectList.push_back(noteFolder.jsonObject());
    }

    QJsonObject noteFolderResultObject;
    noteFolderResultObject.insert(QStringLiteral("type"), QJsonValue::fromVariant("noteFolders"));
    noteFolderResultObject.insert(QStringLiteral("data"), noteFolderObjectList);
    noteFolderResultObject.insert(QStringLiteral("currentId"), NoteFolder::currentNoteFolderId());

    QJsonDocument doc(noteFolderResultObject);

    return doc.toJson(QJsonDocument::Compact);
}

QString WebSocketServerService::getCommandSnippetsJsonText() {
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow == nullptr) {
        return {};
    }

    Tag tag = Tag::fetchByName(getCommandSnippetsTag());
    const QVector<Note> noteList = tag.fetchAllLinkedNotes();
    QVector<CommandSnippet> commandSnippets;

    // get all command snippet from notes tagged with the command snippets tag
    for (const Note &note : noteList) {
        QVector<CommandSnippet> noteCommandSnippets = note.getParsedCommandSnippets();

        // merge command snippet lists
        CommandSnippet::mergeListInList(noteCommandSnippets, commandSnippets);
    }

    // extract command snippets from the current note
    QVector<CommandSnippet> currentNoteCommandSnippets =
        CommandSnippet::parseCommandSnippets(mainWindow->activeNoteTextEdit()->toPlainText(), true);

    // merge command snippet lists
    CommandSnippet::mergeListInList(currentNoteCommandSnippets, commandSnippets);

    QString jsonText = CommandSnippet::commandSnippetsWebServiceJsonText(commandSnippets);

    return jsonText;
}

/**
 * Returns the json text after switching note folders
 *
 * @param switched
 * @return
 */
QString WebSocketServerService::getNoteFolderSwitchedJsonText(bool switched) {
    QJsonObject object;
    object.insert(QStringLiteral("type"), QJsonValue::fromVariant("switchedNoteFolder"));
    object.insert(QStringLiteral("data"), QJsonValue::fromVariant(switched));
    QJsonDocument doc(object);

    return doc.toJson(QJsonDocument::Compact);
}

/**
 * Returns the json text for the token request
 *
 * @return
 */
QString WebSocketServerService::getTokenQueryJsonText() {
    QJsonObject object;
    object.insert(QStringLiteral("type"), QJsonValue::fromVariant("tokenQuery"));
    QJsonDocument doc(object);

    return doc.toJson(QJsonDocument::Compact);
}

void WebSocketServerService::socketDisconnected() {
    auto *pClient = qobject_cast<QWebSocket *>(sender());

    if (pClient != nullptr) {
        Utils::Misc::printInfo(
            tr("%1 was disconnected from QOwnNotes server").arg(getIdentifier(pClient)));

        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}

QString WebSocketServerService::getBookmarksTag() {
    return SettingsService()
        .value(QStringLiteral("webSocketServerService/bookmarksTag"), "bookmarks")
        .toString();
}

QString WebSocketServerService::getBookmarksNoteName() {
    return SettingsService()
        .value(QStringLiteral("webSocketServerService/bookmarksNoteName"), "Bookmarks")
        .toString();
}

QString WebSocketServerService::getCommandSnippetsTag() {
    return SettingsService()
        .value(QStringLiteral("webSocketServerService/commandSnippetsTag"), "commands")
        .toString();
}

QString WebSocketServerService::getCommandSnippetsNoteName() {
    return SettingsService()
        .value(QStringLiteral("webSocketServerService/commandSnippetsNoteName"), "Commands")
        .toString();
}

QString WebSocketServerService::flashMessageJsonText(const QString &message) {
    QJsonObject resultObject;
    resultObject.insert(QStringLiteral("type"), QJsonValue::fromVariant("flashMessage"));
    resultObject.insert(QStringLiteral("message"), message);
    resultObject.insert(QStringLiteral("noteFolderName"),
                        NoteFolder::currentNoteFolder().getName());

    QJsonDocument doc(resultObject);

    return doc.toJson(QJsonDocument::Compact);
}
