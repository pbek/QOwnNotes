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

#pragma once

#include <QObject>
#include <QSslError>
#include <QTimer>

class QWebSocket;
class QSslError;
class QString;

class WebAppClientService : public QObject {
    Q_OBJECT
   public:
    void initClipboardService();
    explicit WebAppClientService(QObject *parent = nullptr);
    ~WebAppClientService() override;

    static QString getServerUrl();
    static QString getDefaultServerUrl();
    static QString getOrGenerateToken();
    void open();
    void close();

   private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString &message);
    static void onSslErrors(const QList<QSslError> &errors);
    void onSendHeartbeatText();
    void onReconnect();

   private:
    QWebSocket *_webSocket{};
    QString _url;
    const int _heartbeatTime = 600000;    // heartbeat data transmission time interval in ms
    const int _reconnectHeartbeatTimerCount =
        10;                              // needs heartbeat reconnection transmission failures
    const int _reconnectTime = 20000;    // reconnection time interval in ms
    int _heartbeatFailedCount = 0;       // send heartbeat failures
    int _reconnectFailedCount = 0;       // reconnection failures
    QTimer _timerHeartbeat;              // send heartbeat timer
    QTimer _timerReconnect;              // reconnection timer
    QString _heartbeatText;
    QString _sessionId;

    void generateSessionId();
    void sendInsertIntoClipboard(const QString &mimeType, const QString &content) const;
};
