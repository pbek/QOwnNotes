#include "test_openaiservice.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtTest>

#include "services/openaiservice.h"
#include "services/settingsservice.h"

void TestOpenAiService::init() {
    SettingsService settings;
    settings.clear();
    settings.setValue(QStringLiteral("ai/currentBackend"), QStringLiteral("openai"));
    settings.setValue(QStringLiteral("ai/openai/currentModel"), QStringLiteral("gpt-5"));
    settings.setValue(QStringLiteral("ai/groq/currentModel"),
                      QStringLiteral("openai/gpt-oss-120b"));
    settings.setValue(QStringLiteral("ai/enabled"), false);
    settings.setValue(QStringLiteral("ai/responseTimeout"), 1);
}

void TestOpenAiService::cleanup() { SettingsService().clear(); }

void TestOpenAiService::testBackendLookupPreservesSelection() {
    QObject owner;
    OpenAiService service(&owner);
    QCOMPARE(service.getBackendId(), QStringLiteral("openai"));
    QCOMPARE(service.getModelId(), QStringLiteral("gpt-5"));
    QCOMPARE(service.getModelsForBackend(QStringLiteral("groq")).first(),
             QStringLiteral("openai/gpt-oss-20b"));
    QCOMPARE(service.getApiBaseUrlForBackend(QStringLiteral("groq")),
             QStringLiteral("https://api.groq.com/openai/v1/chat/completions"));
    QVERIFY(service.getApiKeyForBackend(QStringLiteral("groq")).isEmpty());
    QCOMPARE(service.getBackendId(), QStringLiteral("openai"));
    QCOMPARE(service.getModelId(), QStringLiteral("gpt-5"));
    QCOMPARE(SettingsService().value(QStringLiteral("ai/groq/currentModel")).toString(),
             QStringLiteral("openai/gpt-oss-120b"));
}

void TestOpenAiService::testIsolatedCompletion_data() {
    QTest::addColumn<int>("status");
    QTest::newRow("success") << 200;
    QTest::newRow("error") << 401;
    QTest::newRow("timeout") << 0;
}

void TestOpenAiService::testIsolatedCompletion() {
    QFETCH(int, status);
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost));
    QByteArray request;
    QJsonObject requestJson;
    bool replied = false;
    connect(&server, &QTcpServer::newConnection, &server, [&]() {
        auto *socket = server.nextPendingConnection();
        connect(socket, &QTcpSocket::readyRead, &server, [&, socket]() {
            request += socket->readAll();
            const int headerEnd = request.indexOf("\r\n\r\n");
            if (headerEnd < 0 || replied) {
                return;
            }
            int contentLength = 0;
            for (const auto &header : request.left(headerEnd).split('\n')) {
                if (header.toLower().startsWith("content-length:")) {
                    contentLength = header.mid(header.indexOf(':') + 1).trimmed().toInt();
                }
            }
            if (request.size() < headerEnd + 4 + contentLength) {
                return;
            }
            requestJson = QJsonDocument::fromJson(request.mid(headerEnd + 4)).object();
            replied = true;
            if (status == 0) {
                return;
            }
            const QByteArray body =
                status == 200
                    ? QByteArray(
                          R"({"choices":[{"message":{"role":"assistant","content":"Test answer","reasoning":"Internal reasoning"}}]})")
                    : QByteArray(R"({"error":{"message":"Invalid API key"}})");
            socket->write("HTTP/1.1 " + QByteArray::number(status) +
                          (status == 200 ? " OK\r\n" : " Unauthorized\r\n") +
                          "Content-Type: application/json\r\nContent-Length: " +
                          QByteArray::number(body.size()) + "\r\nConnection: close\r\n\r\n" + body);
            socket->disconnectFromHost();
        });
    });

    const QString url =
        QStringLiteral("http://127.0.0.1:%1/chat/completions").arg(server.serverPort());
    OpenAiCompleter completer(QStringLiteral("test-key"), QStringLiteral("openai/gpt-oss-20b"),
                              url);
    const QString result = completer.completeSync(QStringLiteral("Test"));
    QVERIFY(replied);
    QCOMPARE(requestJson.value(QStringLiteral("model")).toString(),
             QStringLiteral("openai/gpt-oss-20b"));
    QCOMPARE(requestJson.value(QStringLiteral("messages"))
                 .toArray()
                 .first()
                 .toObject()
                 .value(QStringLiteral("content"))
                 .toString(),
             QStringLiteral("Test"));
    QVERIFY(request.contains("Bearer test-key"));
    if (status == 200) {
        QCOMPARE(result, QStringLiteral("Test answer"));
    } else if (status == 0) {
        QCOMPARE(result, OpenAiCompleter::tr("Timeout, while waiting for the AI response"));
    } else {
        QVERIFY(!result.isEmpty());
        QVERIFY(result != QStringLiteral("Test answer"));
    }

    SettingsService settings;
    QCOMPARE(settings.value(QStringLiteral("ai/currentBackend")).toString(),
             QStringLiteral("openai"));
    QCOMPARE(settings.value(QStringLiteral("ai/openai/currentModel")).toString(),
             QStringLiteral("gpt-5"));
    QCOMPARE(settings.value(QStringLiteral("ai/groq/currentModel")).toString(),
             QStringLiteral("openai/gpt-oss-120b"));
    QVERIFY(!settings.value(QStringLiteral("ai/enabled")).toBool());
}
