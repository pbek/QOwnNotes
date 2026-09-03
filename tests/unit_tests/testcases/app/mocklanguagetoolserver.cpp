#ifdef LANGUAGETOOL_ENABLED

#include "mocklanguagetoolserver.h"

#include <QHostAddress>
#include <QStringList>
#include <QTcpSocket>

MockLanguageToolServer::MockLanguageToolServer(QObject *parent) : QObject(parent) {
    connect(&_server, &QTcpServer::newConnection, this,
            &MockLanguageToolServer::handleNewConnection);
}

bool MockLanguageToolServer::start() { return _server.listen(QHostAddress::LocalHost); }

QString MockLanguageToolServer::baseUrl() const {
    return QStringLiteral("http://127.0.0.1:%1").arg(_server.serverPort());
}

void MockLanguageToolServer::setResponseBody(const QByteArray &body) { _responseBody = body; }

void MockLanguageToolServer::setResponseStatus(int status, const QByteArray &reasonPhrase) {
    _responseStatus = status;
    _responseReason = reasonPhrase;
}

void MockLanguageToolServer::setRespond(bool respond) { _respond = respond; }

QString MockLanguageToolServer::lastRequestPath() const { return _lastRequestPath; }

QUrlQuery MockLanguageToolServer::lastRequestBodyParams() const {
    return QUrlQuery(_lastRequestBody);
}

int MockLanguageToolServer::requestCount() const { return _requestCount; }

void MockLanguageToolServer::handleNewConnection() {
    while (_server.hasPendingConnections()) {
        QTcpSocket *socket = _server.nextPendingConnection();
        if (socket == nullptr) {
            continue;
        }

        connect(socket, &QTcpSocket::readyRead, this, [this, socket]() { processSocket(socket); });
        connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    }
}

void MockLanguageToolServer::processSocket(QTcpSocket *socket) {
    // Deliberately swallow the data without ever answering, to simulate an
    // unresponsive server for timeout tests.
    if (!_respond) {
        socket->readAll();
        return;
    }

    _buffers[socket] += socket->readAll();
    const QByteArray &buf = _buffers[socket];

    const int headerEnd = buf.indexOf("\r\n\r\n");
    if (headerEnd < 0) {
        return;
    }

    const QByteArray headerPart = buf.left(headerEnd);
    const QStringList headerLines = QString::fromLatin1(headerPart).split(QStringLiteral("\r\n"));

    int contentLength = 0;
    for (const QString &line : headerLines) {
        if (line.startsWith(QStringLiteral("Content-Length:"), Qt::CaseInsensitive)) {
            contentLength = line.section(QLatin1Char(':'), 1).trimmed().toInt();
        }
    }

    if (!headerLines.isEmpty()) {
        _lastRequestPath = headerLines.first().section(QLatin1Char(' '), 1, 1);
    }

    const int bodyStart = headerEnd + 4;
    if (buf.size() - bodyStart < contentLength) {
        // Body not fully received yet.
        return;
    }

    _lastRequestBody = QString::fromUtf8(buf.mid(bodyStart, contentLength));
    ++_requestCount;
    _buffers.remove(socket);

    QByteArray response =
        "HTTP/1.1 " + QByteArray::number(_responseStatus) + " " + _responseReason + "\r\n";
    response += "Content-Type: application/json\r\n";
    response += "Content-Length: " + QByteArray::number(_responseBody.size()) + "\r\n";
    response += "Connection: close\r\n\r\n";
    response += _responseBody;

    socket->write(response);
    socket->flush();
    socket->disconnectFromHost();
}

#endif
