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

#include "webappclientservice.h"

#include <utils/misc.h>

#include <QSettings>
#include <QWebSocket>

#include "metricsservice.h"

using namespace std;

QT_USE_NAMESPACE

WebAppClientService::WebAppClientService(QObject *parent)
    : QObject(parent) {
    if (!Utils::Misc::isWebAppSupportEnabled()) {
        return;
    }

//    connect(&_webSocket, &QWebSocket::connected, this, &WebAppClientService::onConnected);
//    connect(&_webSocket, &QWebSocket::disconnected, this, &WebAppClientService::closed);

    open();
}

void WebAppClientService::open() {
    _url = getServerUrl() + "/ws";
    qDebug() << "Opening socket connection to " << _url;
    _webSocket.open(_url);
}

void WebAppClientService::close() {
    _webSocket.close();
    _url = "";
}

QString WebAppClientService::getServerUrl() {
    return QSettings().value(QStringLiteral("webAppClientService/serverUrl"),
                   getDefaultServerUrl()).toString();
}

QString WebAppClientService::getOrGenerateToken() {
    QString token = QSettings().value(QStringLiteral("webAppClientService/token")).toString();

    // if not token was set
    if (token.isEmpty()) {
        token = Utils::Misc::generateRandomString(32);
        QSettings().setValue(QStringLiteral("webAppClientService/token"), token);
    }

    return token;
}

QString WebAppClientService::getDefaultServerUrl() {
#ifdef QT_NO_DEBUG
    return "https://app.qownnotes.org";
#else
    return "http://localhost:8080";
#endif
}

WebAppClientService::~WebAppClientService() {
    _webSocket.close();
}

void WebAppClientService::onConnected() {}

void WebAppClientService::onTextMessageReceived(QString message) {
    qDebug() << __func__ << " - 'message': " << message;
}
