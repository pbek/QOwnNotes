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

#include "mcpservice.h"

#include <utils/misc.h>

#include <QJsonArray>
#include <QJsonDocument>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUrlQuery>
#include <QUuid>

#include "entities/note.h"
#include "entities/notefolder.h"
#include "services/cryptoservice.h"
#include "services/settingsservice.h"

static const char *MCP_PROTOCOL_VERSION = "2024-11-05";

McpService::McpService(QObject *parent) : QObject(parent), m_server(new QTcpServer(this)) {}

McpService::~McpService() { stop(); }

void McpService::start() {
    if (!isEnabled()) {
        return;
    }

    const quint16 port = getPort();

    if (m_server->isListening() && m_port == port) {
        return;
    }

    if (m_server->isListening()) {
        stop();
    }

    disconnect(m_server, &QTcpServer::newConnection, this, &McpService::handleNewConnection);
    connect(m_server, &QTcpServer::newConnection, this, &McpService::handleNewConnection);

    if (!m_server->listen(QHostAddress::LocalHost, port)) {
        qWarning() << "MCP server: could not listen on port" << port;
        return;
    }

    m_port = port;
    Utils::Misc::printInfo(tr("MCP server listening on port %1").arg(QString::number(port)));
}

void McpService::stop() {
    // Close all SSE client connections
    for (auto it = m_sseClients.begin(); it != m_sseClients.end(); ++it) {
        QTcpSocket *socket = it.value();
        if (socket && socket->isOpen()) {
            socket->disconnectFromHost();
        }
    }
    m_sseClients.clear();

    if (m_server && m_server->isListening()) {
        disconnect(m_server, &QTcpServer::newConnection, this, &McpService::handleNewConnection);
        m_server->close();
    }

    m_port = 0;
}

bool McpService::isRunning() const { return m_server && m_server->isListening(); }

// Settings helpers

bool McpService::isEnabled() {
    return SettingsService().value(QStringLiteral("ai/mcpServerEnabled"), false).toBool();
}

quint16 McpService::getPort() {
    return static_cast<quint16>(SettingsService()
                                    .value(QStringLiteral("ai/mcpServerPort"), getDefaultPort())
                                    .toULongLong());
}

quint16 McpService::getDefaultPort() {
#ifndef QT_NO_DEBUG
    return 22227;
#else
    return 22226;
#endif
}

QString McpService::getToken() {
    SettingsService settings;
    const QString key = QStringLiteral("ai/mcpServerToken");
    const QString stored = settings.value(key).toString();
    QString token = CryptoService::instance()->decryptToString(stored);

    // Plaintext fallback for manually set values
    if (token.isEmpty() && !stored.isEmpty()) {
        token = stored;
        settings.setValue(key, CryptoService::instance()->encryptToString(
                                   token, QStringLiteral("settings/") + key));
    }

    return token;
}

QString McpService::getOrGenerateToken() {
    QString token = getToken();

    if (token.isEmpty()) {
        token = Utils::Misc::generateRandomString(32);
        SettingsService settings;
        settings.setValue(QStringLiteral("ai/mcpServerToken"),
                          CryptoService::instance()->encryptToString(
                              token, QStringLiteral("settings/ai/mcpServerToken")));
    }

    return token;
}

// HTTP connection handling

void McpService::handleNewConnection() {
    while (m_server->hasPendingConnections()) {
        QTcpSocket *socket = m_server->nextPendingConnection();
        if (!socket) continue;

        connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
        connect(socket, &QTcpSocket::disconnected, this, [this, socket]() {
            // Remove from SSE clients if present
            const QString sessionId = m_sseClients.key(socket);
            if (!sessionId.isEmpty()) {
                m_sseClients.remove(sessionId);
            }
        });

        connect(socket, &QTcpSocket::readyRead, this, [this, socket]() {
            const QByteArray data = socket->readAll();
            if (!data.contains("\r\n\r\n")) {
                return;
            }
            processRequest(socket, data);
        });
    }
}

void McpService::processRequest(QTcpSocket *socket, const QByteArray &requestData) {
    const QString requestText = QString::fromUtf8(requestData);
    const QString method = parseRequestMethod(requestText);
    const QString path = parseRequestPath(requestText);

    // Handle CORS preflight
    if (method == QLatin1String("OPTIONS")) {
        sendCorsHeaders(socket);
        return;
    }

    // Validate Bearer token
    if (!validateBearerToken(requestText)) {
        sendHttpResponse(socket, 401, QByteArray("{\"error\":\"Unauthorized\"}"));
        return;
    }

    // GET /sse - Open SSE stream and send the endpoint URL
    if (method == QLatin1String("GET") && path == QLatin1String("/sse")) {
        openSseStream(socket);
        return;
    }

    // POST /message?sessionId=... - JSON-RPC message from the client
    if (method == QLatin1String("POST") && path.startsWith(QLatin1String("/message"))) {
        const QString sessionId = parseQueryParam(requestText, QStringLiteral("sessionId"));
        const QByteArray body = parseRequestBody(requestData);
        handleJsonRpcOverHttp(socket, body, sessionId);
        return;
    }

    sendHttpResponse(socket, 404, QByteArray("{\"error\":\"Not found\"}"));
}

// HTTP helpers

void McpService::sendHttpResponse(QTcpSocket *socket, int statusCode, const QByteArray &body,
                                  const QString &contentType, const QString &statusText) {
    const QString header = QString::fromLatin1(
                               "HTTP/1.1 %1 %2\r\n"
                               "Content-Type: %3\r\n"
                               "Content-Length: %4\r\n"
                               "Access-Control-Allow-Origin: http://localhost\r\n"
                               "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
                               "Access-Control-Allow-Headers: Content-Type, Authorization\r\n"
                               "Cache-Control: no-store\r\n"
                               "Connection: close\r\n"
                               "\r\n")
                               .arg(QString::number(statusCode), statusText, contentType,
                                    QString::number(body.size()));

    socket->write(header.toUtf8());
    socket->write(body);
    socket->disconnectFromHost();
}

void McpService::sendCorsHeaders(QTcpSocket *socket) {
    const QByteArray response =
        "HTTP/1.1 204 No Content\r\n"
        "Access-Control-Allow-Origin: http://localhost\r\n"
        "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type, Authorization\r\n"
        "Access-Control-Max-Age: 86400\r\n"
        "Content-Length: 0\r\n"
        "Connection: close\r\n"
        "\r\n";

    socket->write(response);
    socket->disconnectFromHost();
}

// SSE helpers

void McpService::openSseStream(QTcpSocket *socket) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    const QString sessionId = QUuid::createUuid().toString(QUuid::WithoutBraces);
#else
    QString sessionId = QUuid::createUuid().toString();
    sessionId.remove(QLatin1Char('{'));
    sessionId.remove(QLatin1Char('}'));
#endif
    m_sseClients.insert(sessionId, socket);

    // Send SSE headers, connection stays open
    const QByteArray header =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/event-stream\r\n"
        "Cache-Control: no-cache\r\n"
        "Access-Control-Allow-Origin: http://localhost\r\n"
        "Connection: keep-alive\r\n"
        "\r\n";

    socket->write(header);

    // Send the endpoint event so the client knows where to POST messages
    const QString endpointUrl = QStringLiteral("http://localhost:%1/message?sessionId=%2")
                                    .arg(QString::number(m_port), sessionId);

    sendSseEvent(socket, QStringLiteral("endpoint"), endpointUrl.toUtf8());
}

void McpService::sendSseEvent(QTcpSocket *socket, const QString &event, const QByteArray &data) {
    if (!socket || !socket->isOpen()) return;

    QByteArray frame;
    frame.append("event: " + event.toUtf8() + "\n");
    frame.append("data: " + data + "\n\n");
    socket->write(frame);
    socket->flush();
}

// JSON-RPC over HTTP + SSE

void McpService::handleJsonRpcOverHttp(QTcpSocket *socket, const QByteArray &body,
                                       const QString &sessionId) {
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(body, &parseError);

    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        const QByteArray errorBody = QByteArray(
            "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32700,"
            "\"message\":\"Parse error\"},\"id\":null}");
        sendHttpResponse(socket, 200, errorBody);
        return;
    }

    const QJsonObject request = doc.object();
    const QJsonObject response = dispatchJsonRpc(request);

    // Send HTTP 202 Accepted to the POST sender
    sendHttpResponse(socket, 202, QByteArray("Accepted"), QStringLiteral("text/plain"),
                     QStringLiteral("Accepted"));

    // Send the JSON-RPC response via SSE to the corresponding session
    QTcpSocket *sseSocket = m_sseClients.value(sessionId);
    if (sseSocket && sseSocket->isOpen()) {
        const QByteArray responseData = QJsonDocument(response).toJson(QJsonDocument::Compact);
        sendSseEvent(sseSocket, QStringLiteral("message"), responseData);
    }
}

// MCP JSON-RPC dispatch

QJsonObject McpService::dispatchJsonRpc(const QJsonObject &request) {
    const QString method = request.value(QStringLiteral("method")).toString();
    const QJsonValue id = request.value(QStringLiteral("id"));
    const QJsonObject params = request.value(QStringLiteral("params")).toObject();

    // Notifications (no id) acknowledge silently
    if (id.isUndefined() || id.isNull()) {
        return {};
    }

    if (method == QLatin1String("initialize")) {
        return handleInitialize(params, id);
    }

    if (method == QLatin1String("tools/list")) {
        return handleToolsList(id);
    }

    if (method == QLatin1String("tools/call")) {
        return handleToolsCall(params, id);
    }

    // Method not found
    QJsonObject error;
    error[QStringLiteral("code")] = -32601;
    error[QStringLiteral("message")] = QStringLiteral("Method not found");
    QJsonObject resp;
    resp[QStringLiteral("jsonrpc")] = QStringLiteral("2.0");
    resp[QStringLiteral("id")] = id;
    resp[QStringLiteral("error")] = error;
    return resp;
}

// MCP method handlers

QJsonObject McpService::handleInitialize(const QJsonObject &params, const QJsonValue &id) {
    Q_UNUSED(params)

    QJsonObject capabilities;

    // Declare tools capability
    QJsonObject toolsCap;
    capabilities[QStringLiteral("tools")] = toolsCap;

    QJsonObject serverInfo;
    serverInfo[QStringLiteral("name")] = QStringLiteral("qownnotes-mcp");
    serverInfo[QStringLiteral("version")] = QStringLiteral("1.0.0");

    QJsonObject result;
    result[QStringLiteral("protocolVersion")] = QLatin1String(MCP_PROTOCOL_VERSION);
    result[QStringLiteral("capabilities")] = capabilities;
    result[QStringLiteral("serverInfo")] = serverInfo;

    QJsonObject resp;
    resp[QStringLiteral("jsonrpc")] = QStringLiteral("2.0");
    resp[QStringLiteral("id")] = id;
    resp[QStringLiteral("result")] = result;
    return resp;
}

QJsonObject McpService::handleToolsList(const QJsonValue &id) {
    QJsonArray tools;

    // search_notes tool
    {
        QJsonObject tool;
        tool[QStringLiteral("name")] = QStringLiteral("search_notes");
        tool[QStringLiteral("description")] = QStringLiteral(
            "Search for notes by text query. Returns a list of matching notes "
            "with their names, IDs and text previews.");

        QJsonObject inputSchema;
        inputSchema[QStringLiteral("type")] = QStringLiteral("object");

        QJsonObject properties;
        QJsonObject queryProp;
        queryProp[QStringLiteral("type")] = QStringLiteral("string");
        queryProp[QStringLiteral("description")] = QStringLiteral(
            "The search query string. Supports multiple words (AND logic) "
            "and quoted phrases.");
        properties[QStringLiteral("query")] = queryProp;

        QJsonObject limitProp;
        limitProp[QStringLiteral("type")] = QStringLiteral("integer");
        limitProp[QStringLiteral("description")] =
            QStringLiteral("Maximum number of results to return (default 20, max 100).");
        properties[QStringLiteral("limit")] = limitProp;

        inputSchema[QStringLiteral("properties")] = properties;
        inputSchema[QStringLiteral("required")] = QJsonArray{QStringLiteral("query")};

        tool[QStringLiteral("inputSchema")] = inputSchema;
        tools.append(tool);
    }

    // fetch_note tool
    {
        QJsonObject tool;
        tool[QStringLiteral("name")] = QStringLiteral("fetch_note");
        tool[QStringLiteral("description")] =
            QStringLiteral("Fetch the full content of a note by its name or ID.");

        QJsonObject inputSchema;
        inputSchema[QStringLiteral("type")] = QStringLiteral("object");

        QJsonObject properties;
        QJsonObject nameProp;
        nameProp[QStringLiteral("type")] = QStringLiteral("string");
        nameProp[QStringLiteral("description")] = QStringLiteral("The name of the note to fetch.");
        properties[QStringLiteral("name")] = nameProp;

        QJsonObject idProp;
        idProp[QStringLiteral("type")] = QStringLiteral("integer");
        idProp[QStringLiteral("description")] = QStringLiteral(
            "The ID of the note to fetch. Takes precedence over name if both "
            "are provided.");
        properties[QStringLiteral("id")] = idProp;

        inputSchema[QStringLiteral("properties")] = properties;

        tool[QStringLiteral("inputSchema")] = inputSchema;
        tools.append(tool);
    }

    QJsonObject result;
    result[QStringLiteral("tools")] = tools;

    QJsonObject resp;
    resp[QStringLiteral("jsonrpc")] = QStringLiteral("2.0");
    resp[QStringLiteral("id")] = id;
    resp[QStringLiteral("result")] = result;
    return resp;
}

QJsonObject McpService::handleToolsCall(const QJsonObject &params, const QJsonValue &id) {
    const QString toolName = params.value(QStringLiteral("name")).toString();
    const QJsonObject arguments = params.value(QStringLiteral("arguments")).toObject();

    QJsonObject toolResult;

    if (toolName == QLatin1String("search_notes")) {
        toolResult = toolSearchNotes(arguments);
    } else if (toolName == QLatin1String("fetch_note")) {
        toolResult = toolFetchNote(arguments);
    } else {
        // Unknown tool
        QJsonArray content;
        QJsonObject textContent;
        textContent[QStringLiteral("type")] = QStringLiteral("text");
        textContent[QStringLiteral("text")] = QStringLiteral("Unknown tool: %1").arg(toolName);
        content.append(textContent);

        QJsonObject result;
        result[QStringLiteral("content")] = content;
        result[QStringLiteral("isError")] = true;

        QJsonObject resp;
        resp[QStringLiteral("jsonrpc")] = QStringLiteral("2.0");
        resp[QStringLiteral("id")] = id;
        resp[QStringLiteral("result")] = result;
        return resp;
    }

    QJsonObject resp;
    resp[QStringLiteral("jsonrpc")] = QStringLiteral("2.0");
    resp[QStringLiteral("id")] = id;
    resp[QStringLiteral("result")] = toolResult;
    return resp;
}

// MCP tool implementations

QJsonObject McpService::toolSearchNotes(const QJsonObject &arguments) {
    const QString query = arguments.value(QStringLiteral("query")).toString();
    int limit = arguments.value(QStringLiteral("limit")).toInt(20);
    if (limit < 1) limit = 1;
    if (limit > 100) limit = 100;

    // Use Note::search for full-text LIKE search
    const QVector<Note> notes = Note::search(query);

    QJsonArray resultsArray;
    int count = 0;

    for (const Note &note : notes) {
        if (count >= limit) break;

        QJsonObject noteObj;
        noteObj[QStringLiteral("id")] = note.getId();
        noteObj[QStringLiteral("name")] = note.getName();
        noteObj[QStringLiteral("fileName")] = note.getFileName();
        noteObj[QStringLiteral("preview")] = note.getNotePreviewText(false, 5);

        resultsArray.append(noteObj);
        count++;
    }

    // Build MCP content response
    QJsonObject resultObj;
    resultObj[QStringLiteral("query")] = query;
    resultObj[QStringLiteral("count")] = count;
    resultObj[QStringLiteral("results")] = resultsArray;

    QJsonArray content;
    QJsonObject textContent;
    textContent[QStringLiteral("type")] = QStringLiteral("text");
    textContent[QStringLiteral("text")] =
        QString::fromUtf8(QJsonDocument(resultObj).toJson(QJsonDocument::Compact));
    content.append(textContent);

    QJsonObject toolResult;
    toolResult[QStringLiteral("content")] = content;
    return toolResult;
}

QJsonObject McpService::toolFetchNote(const QJsonObject &arguments) {
    Note note;

    // Prefer ID over name
    if (arguments.contains(QStringLiteral("id"))) {
        const int noteId = arguments.value(QStringLiteral("id")).toInt();
        note = Note::fetch(noteId);
    } else if (arguments.contains(QStringLiteral("name"))) {
        const QString name = arguments.value(QStringLiteral("name")).toString();
        note = Note::fetchByName(name);
    }

    QJsonArray content;

    if (!note.isFetched()) {
        QJsonObject textContent;
        textContent[QStringLiteral("type")] = QStringLiteral("text");
        textContent[QStringLiteral("text")] = QStringLiteral("Note not found.");
        content.append(textContent);

        QJsonObject toolResult;
        toolResult[QStringLiteral("content")] = content;
        toolResult[QStringLiteral("isError")] = true;
        return toolResult;
    }

    QJsonObject noteObj;
    noteObj[QStringLiteral("id")] = note.getId();
    noteObj[QStringLiteral("name")] = note.getName();
    noteObj[QStringLiteral("fileName")] = note.getFileName();
    noteObj[QStringLiteral("noteText")] = note.getNoteText();
    noteObj[QStringLiteral("noteFolder")] = NoteFolder::currentNoteFolder().getName();

    QJsonObject textContent;
    textContent[QStringLiteral("type")] = QStringLiteral("text");
    textContent[QStringLiteral("text")] =
        QString::fromUtf8(QJsonDocument(noteObj).toJson(QJsonDocument::Compact));
    content.append(textContent);

    QJsonObject toolResult;
    toolResult[QStringLiteral("content")] = content;
    return toolResult;
}

// Auth helpers

bool McpService::validateBearerToken(const QString &requestText) const {
    const QString token = parseBearerToken(requestText);
    const QString expectedToken = getOrGenerateToken();
    return !expectedToken.isEmpty() && token == expectedToken;
}

QString McpService::parseBearerToken(const QString &requestText) {
    // Look for "Authorization: Bearer <token>" header
    const QStringList lines = requestText.split(QStringLiteral("\r\n"));

    for (const QString &line : lines) {
        if (line.startsWith(QLatin1String("Authorization:"), Qt::CaseInsensitive)) {
            const QString value = line.mid(line.indexOf(QLatin1Char(':')) + 1).trimmed();
            if (value.startsWith(QLatin1String("Bearer "), Qt::CaseInsensitive)) {
                return value.mid(7).trimmed();
            }
        }
    }

    return {};
}

QString McpService::parseRequestPath(const QString &requestText) {
    const QString requestLine = requestText.section(QStringLiteral("\r\n"), 0, 0);
    const QStringList parts = requestLine.split(QLatin1Char(' '));
    if (parts.size() < 2) return {};
    const QUrl url(parts.at(1));
    return url.path();
}

QString McpService::parseRequestMethod(const QString &requestText) {
    const QString requestLine = requestText.section(QStringLiteral("\r\n"), 0, 0);
    const QStringList parts = requestLine.split(QLatin1Char(' '));
    if (parts.isEmpty()) return {};
    return parts.at(0).trimmed();
}

QByteArray McpService::parseRequestBody(const QByteArray &requestData) {
    const int headerEnd = requestData.indexOf("\r\n\r\n");
    if (headerEnd < 0) return {};
    return requestData.mid(headerEnd + 4);
}

QString McpService::parseQueryParam(const QString &requestText, const QString &key) {
    const QString requestLine = requestText.section(QStringLiteral("\r\n"), 0, 0);
    const QStringList parts = requestLine.split(QLatin1Char(' '));
    if (parts.size() < 2) return {};

    const QUrl url(parts.at(1));
    const QUrlQuery query(url);
    return query.queryItemValue(key, QUrl::FullyDecoded);
}
