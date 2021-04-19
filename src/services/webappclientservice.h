/*
 * Copyright (c) 2014-2021 Patrizio Bekerle -- <patrizio@bekerle.com>
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

class QWebSocket;
class QSslError;
class QString;

class WebAppClientService : public QObject {
    Q_OBJECT
   public:
    explicit WebAppClientService(QObject *parent = nullptr);
    ~WebAppClientService() override;

    static QString getServerUrl();
    static QString getDefaultServerUrl();
    static QString getOrGenerateToken();
    void open();
    void close();

   private slots:
    static void onConnected();
    static void onTextMessageReceived(const QString &message);
    static void onSslErrors(const QList<QSslError>& errors);

   private:
    QWebSocket *_webSocket{};
    QString _url;
};
