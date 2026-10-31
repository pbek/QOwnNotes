#include "test_openaiservice.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLineEdit>
#include <QTcpServer>
#include <QTcpSocket>
#include <QToolButton>
#include <QtTest>

#include "services/cryptoservice.h"
#include "services/openaiservice.h"
#include "services/settingsservice.h"
#include "widgets/settings/aisettingswidget.h"

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

void TestOpenAiService::testOpenRouterBackend() {
    QObject owner;
    OpenAiService service(&owner);
    QCOMPARE(service.getBackendNames().value(QStringLiteral("openrouter")),
             QStringLiteral("OpenRouter"));
    QCOMPARE(service.getApiBaseUrlForBackend(QStringLiteral("openrouter")),
             QStringLiteral("https://openrouter.ai/api/v1/chat/completions"));
    const auto models = service.getModelsForBackend(QStringLiteral("openrouter"));
    QCOMPARE(models.first(), QStringLiteral("openrouter/free"));
    QVERIFY(models.size() > 1);
    for (const auto &model : models) {
        QVERIFY(model == QStringLiteral("openrouter/free") || model.endsWith(":free"));
    }
    QVERIFY(!service.hasConfiguredBackend());

    SettingsService settings;
    settings.setValue(
        QStringLiteral("ai/openrouter/apiKey"),
        CryptoService::instance()->encryptToString(
            QStringLiteral("router-key"), QStringLiteral("settings/ai/openrouter/apiKey")));
    QVERIFY(service.hasConfiguredBackend());
    OpenAiService configuredService(&owner);
    QCOMPARE(configuredService.getApiKeyForBackend(QStringLiteral("openrouter")),
             QStringLiteral("router-key"));
    QCOMPARE(service.getBackendId(), QStringLiteral("openai"));
    QCOMPARE(service.getModelId(), QStringLiteral("gpt-5"));
    QVERIFY(configuredService.setBackendId(QStringLiteral("openrouter")));
    QCOMPARE(configuredService.getModelId(), QStringLiteral("openrouter/free"));
    QVERIFY(configuredService.setModelId(models.at(1)));
    QVERIFY(configuredService.setBackendId(QStringLiteral("openai")));
    QCOMPARE(configuredService.getModelId(), QStringLiteral("gpt-5"));
    QVERIFY(configuredService.setBackendId(QStringLiteral("openrouter")));
    QCOMPARE(configuredService.getModelId(), models.at(1));
    settings.setValue(QStringLiteral("ai/openrouter/apiKey"), QString());
    QVERIFY(!configuredService.hasConfiguredBackend());
}

void TestOpenAiService::testOpenRouterSettings() {
    AiSettingsWidget widget;
    widget.initialize();
    widget.readSettings();
    auto *keyEdit = widget.findChild<QLineEdit *>(QStringLiteral("openRouterApiKeyLineEdit"));
    auto *testButton = widget.findChild<QToolButton *>(QStringLiteral("openRouterApiTestButton"));
    QVERIFY(keyEdit);
    QVERIFY(testButton);
    QVERIFY(!testButton->isEnabled());
    auto *layout = qobject_cast<QGridLayout *>(keyEdit->parentWidget()->layout());
    QVERIFY(layout);
    int row, column, rowSpan, columnSpan;
    layout->getItemPosition(layout->indexOf(keyEdit), &row, &column, &rowSpan, &columnSpan);
    QCOMPARE(row, 0);

    keyEdit->setText(QStringLiteral("router-key"));
    QVERIFY(testButton->isEnabled());
    widget.storeSettings();
    const QString storedKey =
        SettingsService().value(QStringLiteral("ai/openrouter/apiKey")).toString();
    QVERIFY(storedKey != QStringLiteral("router-key"));
    QCOMPARE(CryptoService::instance()->decryptToString(storedKey), QStringLiteral("router-key"));
    keyEdit->clear();
    QVERIFY(!testButton->isEnabled());
    keyEdit->setText(QStringLiteral("   "));
    QVERIFY(!testButton->isEnabled());
    widget.readSettings();
    QCOMPARE(keyEdit->text(), QStringLiteral("router-key"));
    widget.buildAiScriptingTreeWidget();
    auto *scriptingGroup = widget.findChild<QGroupBox *>(QStringLiteral("aiScriptingGroupBox"));
    QVERIFY(scriptingGroup);
    QVERIFY(scriptingGroup->isHidden());
    QCOMPARE(SettingsService().value(QStringLiteral("ai/currentBackend")).toString(),
             QStringLiteral("openai"));
}

void TestOpenAiService::testIsolatedCompletion_data() {
    QTest::addColumn<int>("status");
    QTest::addColumn<QString>("model");
    QTest::newRow("success") << 200 << QStringLiteral("openai/gpt-oss-20b");
    QTest::newRow("error") << 401 << QStringLiteral("openai/gpt-oss-20b");
    QTest::newRow("timeout") << 0 << QStringLiteral("openai/gpt-oss-20b");
    QTest::newRow("openrouter-free-router") << 200 << QStringLiteral("openrouter/free");
    QTest::newRow("openrouter-free-model") << 200 << QStringLiteral("google/gemma-4-31b-it:free");
}

void TestOpenAiService::testIsolatedCompletion() {
    QFETCH(int, status);
    QFETCH(QString, model);
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
    OpenAiCompleter completer(QStringLiteral("test-key"), model, url);
    const QString result = completer.completeSync(QStringLiteral("Test"));
    QVERIFY(replied);
    QCOMPARE(requestJson.value(QStringLiteral("model")).toString(), model);
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
