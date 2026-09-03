#pragma once

#ifdef LANGUAGETOOL_ENABLED

#include <QByteArray>
#include <QHash>
#include <QObject>
#include <QString>
#include <QTcpServer>
#include <QUrlQuery>

class QTcpSocket;

/**
 * Minimal in-process HTTP server that stands in for a LanguageTool instance
 * (self-hosted/free or Premium) in tests, so the test suite doesn't depend on
 * a real LanguageTool server or a Premium account being available.
 */
class MockLanguageToolServer : public QObject {
    Q_OBJECT

   public:
    explicit MockLanguageToolServer(QObject *parent = nullptr);

    bool start();
    QString baseUrl() const;

    void setResponseBody(const QByteArray &body);
    void setResponseStatus(int status, const QByteArray &reasonPhrase = "OK");
    // When false, accepted connections are never answered, to simulate an
    // unreachable/hanging server for timeout testing.
    void setRespond(bool respond);

    QString lastRequestPath() const;
    QUrlQuery lastRequestBodyParams() const;
    int requestCount() const;

   private Q_SLOTS:
    void handleNewConnection();

   private:
    void processSocket(QTcpSocket *socket);

    QTcpServer _server;
    QHash<QTcpSocket *, QByteArray> _buffers;
    QByteArray _responseBody;
    int _responseStatus = 200;
    QByteArray _responseReason = "OK";
    bool _respond = true;
    QString _lastRequestPath;
    QString _lastRequestBody;
    int _requestCount = 0;
};

#endif
