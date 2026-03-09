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

#pragma once

#include <QDateTime>
#include <QHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QPair>
#include <QStringList>
#include <QVector>

class Bookmark;
class QTcpServer;
class QTcpSocket;

class QWebSocketServer;
class QWebSocket;
class QString;
class WebSocketTokenDialog;

class WebSocketServerService : public QObject {
    Q_OBJECT
   public:
    explicit WebSocketServerService(quint16 port = 0, QObject *parent = nullptr);
    ~WebSocketServerService() override;

    quint16 getPort() const;

    static quint16 getSettingsPort();

    static quint16 getDefaultPort();

    static bool isBookmarkSuggestionApiEnabled();

    static quint16 getBookmarkSuggestionApiPort();

    static quint16 getBookmarkSuggestionApiDefaultPort();

    void refreshServers();

    void listen(quint16 port = 0);

    void close();

    static QString getBookmarksTag();

    static QString getBookmarksNoteName();

    static QString getCommandSnippetsTag();

    static QString getCommandSnippetsNoteName();

    static QString flashMessageJsonText(const QString &message);

    static QJsonArray createBookmarks(const QJsonObject &jsonObject);

    static int deleteBookmark(const QJsonObject &jsonObject);

    static int editBookmark(const QJsonObject &jsonObject);

   private slots:
    void onNewConnection();
    void processMessage(const QString &message);
    void socketDisconnected();

   private:
    QWebSocketServer *m_pWebSocketServer;
    QList<QWebSocket *> m_clients;
    quint16 m_port{};
    QTcpServer *m_pHttpServer;
    quint16 m_httpPort{};
    QHash<QString, QString> m_suggestionCache;
    QVector<Bookmark> m_cachedBookmarks;
    QDateTime m_bookmarkCacheTimestamp;

    static QString getBookmarksJsonText(bool hideCurrent = false);

    QVector<Bookmark> getBookmarksForSuggestions();

    QString homepageSuggestionResponse(const QString &query, int limit);

    static QPair<QString, QHash<QString, QString>> parseHttpRequestLineAndQuery(
        const QString &request);

    static QString httpResponse(
        int statusCode, const QByteArray &body, const QString &statusText = QStringLiteral("OK"),
        const QString &contentType = QStringLiteral("application/json; charset=utf-8"));

    void startSuggestionHttpServer();

    void stopSuggestionHttpServer();

    void handleHttpConnection();

    void processHttpRequest(QTcpSocket *socket, const QString &requestText);

    static QString getCommandSnippetsJsonText();

    static QString getNoteFolderSwitchedJsonText(bool switched);

    static QString getTokenQueryJsonText();

    static QString getNoteFoldersJsonText();

#ifndef INTEGRATION_TESTS
    WebSocketTokenDialog *_webSocketTokenDialog;
#endif
};
