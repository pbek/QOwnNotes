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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QList>

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)
QT_FORWARD_DECLARE_CLASS(QString)

class WebSocketServerService : public QObject
{
Q_OBJECT
public:
    explicit WebSocketServerService(quint16 port = 0,
            QObject *parent = nullptr);
    ~WebSocketServerService() override;

    quint16 getPort();

    static quint16 getSettingsPort();

    static quint16 getDefaultPort();

    void listen(quint16 port = 0);

    void close();

    static QString getBookmarksTag();

    static QString getBookmarksNoteName();

    QString flashMessageJsonText(QString message);

    static QJsonArray createBookmarks(const QJsonObject &jsonObject);

private slots:
    void onNewConnection();
    void processMessage(const QString &message);
    void socketDisconnected();

private:
    QWebSocketServer *m_pWebSocketServer;
    QList<QWebSocket *> m_clients;
    quint16 m_port;

    QString getBookmarksJsonText() const;
};
