#include "test_languagetool.h"

#ifdef LANGUAGETOOL_ENABLED

#include <QSignalSpy>
#include <QtTest>

#include "mocklanguagetoolserver.h"
#include "services/languagetoolclient.h"

namespace {

// A response shaped like a real LanguageTool "/v2/check" reply. Free/local
// and Premium servers use the same response schema, so this fixture is
// reused for both.
QByteArray sampleCheckResponse() {
    return QByteArrayLiteral(R"({
      "matches": [
        {
          "message": "Possible spelling mistake found.",
          "shortMessage": "Spelling mistake",
          "offset": 5,
          "length": 4,
          "context": {"text": "This test sentence.", "offset": 5, "length": 4},
          "rule": {"id": "TEST_RULE", "category": {"id": "TYPOS"}, "url": "https://example.com/rule"},
          "replacements": [{"value": "tests"}, {"value": "testing"}]
        }
      ]
    })");
}

QByteArray responseWithWhitespaceMatch() {
    return QByteArrayLiteral(R"({
      "matches": [
        {
          "message": "Two consecutive spaces",
          "offset": 0,
          "length": 2,
          "context": {"text": "  hi", "offset": 0, "length": 2},
          "rule": {"id": "WHITESPACE_RULE", "category": {"id": "TYPOGRAPHY"}},
          "replacements": []
        },
        {
          "message": "Real issue",
          "offset": 3,
          "length": 2,
          "context": {"text": "  hi", "offset": 3, "length": 2},
          "rule": {"id": "REAL_RULE", "category": {"id": "TYPOS"}},
          "replacements": []
        }
      ]
    })");
}

}    // namespace

void TestLanguageTool::initTestCase() {
    qRegisterMetaType<LanguageToolMatch>("LanguageToolMatch");
    qRegisterMetaType<QVector<LanguageToolMatch>>("QVector<LanguageToolMatch>");
}

void TestLanguageTool::testFreeLocalServerCheck() {
    MockLanguageToolServer server;
    QVERIFY(server.start());
    server.setResponseBody(sampleCheckResponse());

    LanguageToolClient client;
    LanguageToolClient::RequestOptions options;
    options.requestId = 1;
    options.serverUrl = server.baseUrl();
    options.language = QStringLiteral("en-US");
    options.text = QStringLiteral("This test sentence.");
    options.timeoutMs = 5000;

    QSignalSpy finishedSpy(&client, &LanguageToolClient::checkFinished);
    QSignalSpy errorSpy(&client, &LanguageToolClient::checkError);

    client.checkText(options);

    QVERIFY(finishedSpy.wait(5000));
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(server.lastRequestPath(), QStringLiteral("/v2/check"));

    const auto matches = finishedSpy.at(0).at(1).value<QVector<LanguageToolMatch>>();
    QCOMPARE(matches.size(), 1);
    QCOMPARE(matches.first().ruleId, QStringLiteral("TEST_RULE"));
    QCOMPARE(matches.first().ruleCategory, QStringLiteral("TYPOS"));
    QCOMPARE(matches.first().offset, 5);
    QCOMPARE(matches.first().length, 4);
    QCOMPARE(matches.first().replacements.size(), 2);
    QCOMPARE(matches.first().replacements.first(), QStringLiteral("tests"));
}

void TestLanguageTool::testFreeServerDoesNotSendCredentials() {
    // Simulates a self-hosted/free LanguageTool server: no username or API
    // key configured, matching the "local server" setup that has no
    // premium account.
    MockLanguageToolServer server;
    QVERIFY(server.start());
    server.setResponseBody(sampleCheckResponse());

    LanguageToolClient client;
    LanguageToolClient::RequestOptions options;
    options.requestId = 1;
    options.serverUrl = server.baseUrl();
    options.language = QStringLiteral("auto");
    options.text = QStringLiteral("Some text");
    options.timeoutMs = 5000;

    QSignalSpy finishedSpy(&client, &LanguageToolClient::checkFinished);
    client.checkText(options);
    QVERIFY(finishedSpy.wait(5000));

    const QUrlQuery params = server.lastRequestBodyParams();
    QVERIFY(!params.hasQueryItem(QStringLiteral("username")));
    QVERIFY(!params.hasQueryItem(QStringLiteral("apiKey")));
    QCOMPARE(params.queryItemValue(QStringLiteral("text")), QStringLiteral("Some text"));
    QCOMPARE(params.queryItemValue(QStringLiteral("language")), QStringLiteral("auto"));
}

void TestLanguageTool::testPremiumServerSendsUsernameAndApiKey() {
    MockLanguageToolServer server;
    QVERIFY(server.start());
    server.setResponseBody(sampleCheckResponse());

    LanguageToolClient client;
    LanguageToolClient::RequestOptions options;
    options.requestId = 1;
    options.serverUrl = server.baseUrl();
    options.language = QStringLiteral("en-US");
    options.text = QStringLiteral("Premium test");
    options.username = QStringLiteral("user@example.com");
    options.apiKey = QStringLiteral("super-secret-api-key");
    options.timeoutMs = 5000;

    QSignalSpy finishedSpy(&client, &LanguageToolClient::checkFinished);
    client.checkText(options);
    QVERIFY(finishedSpy.wait(5000));

    const QUrlQuery params = server.lastRequestBodyParams();
    QCOMPARE(params.queryItemValue(QStringLiteral("username")), QStringLiteral("user@example.com"));
    QCOMPARE(params.queryItemValue(QStringLiteral("apiKey")),
             QStringLiteral("super-secret-api-key"));
}

void TestLanguageTool::testPremiumServerParsesMatches() {
    // Verifies that responses are parsed the same way regardless of whether
    // they came from a Premium account or a free server.
    MockLanguageToolServer server;
    QVERIFY(server.start());
    server.setResponseBody(sampleCheckResponse());

    LanguageToolClient client;
    LanguageToolClient::RequestOptions options;
    options.requestId = 1;
    options.serverUrl = server.baseUrl();
    options.text = QStringLiteral("This test sentence.");
    options.username = QStringLiteral("user@example.com");
    options.apiKey = QStringLiteral("super-secret-api-key");
    options.timeoutMs = 5000;

    QSignalSpy finishedSpy(&client, &LanguageToolClient::checkFinished);
    client.checkText(options);
    QVERIFY(finishedSpy.wait(5000));

    const auto matches = finishedSpy.at(0).at(1).value<QVector<LanguageToolMatch>>();
    QCOMPARE(matches.size(), 1);
    QCOMPARE(matches.first().ruleId, QStringLiteral("TEST_RULE"));
}

void TestLanguageTool::testWhitespaceRuleIsFiltered() {
    MockLanguageToolServer server;
    QVERIFY(server.start());
    server.setResponseBody(responseWithWhitespaceMatch());

    LanguageToolClient client;
    LanguageToolClient::RequestOptions options;
    options.requestId = 1;
    options.serverUrl = server.baseUrl();
    options.text = QStringLiteral("  hi");
    options.timeoutMs = 5000;

    QSignalSpy finishedSpy(&client, &LanguageToolClient::checkFinished);
    client.checkText(options);
    QVERIFY(finishedSpy.wait(5000));

    const auto matches = finishedSpy.at(0).at(1).value<QVector<LanguageToolMatch>>();
    QCOMPARE(matches.size(), 1);
    QCOMPARE(matches.first().ruleId, QStringLiteral("REAL_RULE"));
}

void TestLanguageTool::testInvalidServerUrlEmitsError() {
    LanguageToolClient client;
    LanguageToolClient::RequestOptions options;
    options.requestId = 1;
    options.serverUrl.clear();
    options.text = QStringLiteral("test");

    QSignalSpy errorSpy(&client, &LanguageToolClient::checkError);
    client.checkText(options);

    QCOMPARE(errorSpy.count(), 1);
}

void TestLanguageTool::testServerUrlWithExistingV2CheckPathIsNotDuplicated() {
    MockLanguageToolServer server;
    QVERIFY(server.start());
    server.setResponseBody(sampleCheckResponse());

    LanguageToolClient client;
    LanguageToolClient::RequestOptions options;
    options.requestId = 1;
    options.serverUrl = server.baseUrl() + QStringLiteral("/v2/check");
    options.text = QStringLiteral("test");
    options.timeoutMs = 5000;

    QSignalSpy finishedSpy(&client, &LanguageToolClient::checkFinished);
    client.checkText(options);
    QVERIFY(finishedSpy.wait(5000));

    QCOMPARE(server.lastRequestPath(), QStringLiteral("/v2/check"));
}

void TestLanguageTool::testServerUrlWithTrailingSlash() {
    MockLanguageToolServer server;
    QVERIFY(server.start());
    server.setResponseBody(sampleCheckResponse());

    LanguageToolClient client;
    LanguageToolClient::RequestOptions options;
    options.requestId = 1;
    options.serverUrl = server.baseUrl() + QStringLiteral("/");
    options.text = QStringLiteral("test");
    options.timeoutMs = 5000;

    QSignalSpy finishedSpy(&client, &LanguageToolClient::checkFinished);
    client.checkText(options);
    QVERIFY(finishedSpy.wait(5000));

    QCOMPARE(server.lastRequestPath(), QStringLiteral("/v2/check"));
}

void TestLanguageTool::testConnectionTimeoutEmitsError() {
    // Simulates an unreachable/hanging server so a request must time out.
    MockLanguageToolServer server;
    QVERIFY(server.start());
    server.setRespond(false);

    LanguageToolClient client;
    LanguageToolClient::RequestOptions options;
    options.requestId = 1;
    options.serverUrl = server.baseUrl();
    options.text = QStringLiteral("test");
    options.timeoutMs = 300;

    QSignalSpy errorSpy(&client, &LanguageToolClient::checkError);
    client.checkText(options);

    QVERIFY(errorSpy.wait(3000));
}

#endif
