#include "test_nextclouddeckservice.h"

#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QtTest>

#include "entities/cloudconnection.h"
#include "services/nextclouddeckservice.h"
#include "services/settingsservice.h"

void TestNextcloudDeckService::testMoveCard_data() {
    QTest::addColumn<int>("statusCode");
    QTest::addColumn<QByteArray>("responseBody");
    QTest::addColumn<bool>("success");
    QTest::newRow("cross-board") << 200 << QByteArray("destination") << true;
    QTest::newRow("same-board") << 200 << QByteArray("destination") << true;
    QTest::newRow("unchanged-stack") << 200 << QByteArray("[{\"id\":56,\"stackId\":34}]") << false;
    QTest::newRow("missing-card") << 200 << QByteArray("[{\"id\":57,\"stackId\":78}]") << false;
    QTest::newRow("invalid-json") << 200 << QByteArray("not JSON") << false;
    QTest::newRow("no-content") << 204 << QByteArray() << false;
    QTest::newRow("permission-denied")
        << 403 << QByteArray("{\"message\":\"Permission denied on destination stack\"}") << false;
    QTest::newRow("suppressed-warning") << 500 << QByteArray() << false;
    QTest::newRow("server-error") << 500 << QByteArray() << false;
    QTest::newRow("timeout") << 0 << QByteArray() << false;
}

void TestNextcloudDeckService::testMoveCard() {
    QFETCH(int, statusCode);
    QFETCH(QByteArray, responseBody);
    QFETCH(bool, success);
    const int targetBoardId = QByteArray(QTest::currentDataTag()) == "same-board" ? 12 : 90;
    SettingsService settings;
    const QString warningKey = QStringLiteral("MessageBoxOverride/nextcloud-deck-move-failed");
    settings.setValue(warningKey, QMessageBox::Ok);

    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost));
    CloudConnection connection;
    connection.setName(QStringLiteral("Deck move test"));
    connection.setServerUrl(QStringLiteral("http://127.0.0.1:%1").arg(server.serverPort()));
    connection.setUsername(QStringLiteral("deck-user"));
    connection.setPassword(QStringLiteral("deck-password"));
    QVERIFY(connection.store());

    QByteArray request;
    int requestCount = 0;
    connect(&server, &QTcpServer::newConnection, &server, [&]() {
        auto *socket = server.nextPendingConnection();
        connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
        connect(socket, &QTcpSocket::readyRead, &server, [&, socket]() {
            request += socket->readAll();
            const int headerEnd = request.indexOf("\r\n\r\n");
            if (headerEnd < 0) {
                return;
            }
            int contentLength = 0;
            for (const auto &line : request.left(headerEnd).split('\n')) {
                if (line.toLower().startsWith("content-length:")) {
                    contentLength = line.mid(line.indexOf(':') + 1).trimmed().toInt();
                }
            }
            if (request.size() < headerEnd + 4 + contentLength) {
                return;
            }
            ++requestCount;
            if (statusCode == 0) {
                return;
            }
            if (responseBody == "destination") {
                // Model Nextcloud route parameters overriding the JSON stackId. A source URL
                // would return HTTP 200 with the card still in its original stack.
                const int routeStackId = request.split('/').value(9).toInt();
                responseBody = "[{\"id\":56,\"stackId\":" + QByteArray::number(routeStackId) + "}]";
            }
            socket->write("HTTP/1.1 " + QByteArray::number(statusCode) +
                          " Test\r\nContent-Type: application/json\r\nContent-Length: " +
                          QByteArray::number(responseBody.size()) +
                          "\r\nConnection: close\r\n\r\n" + responseBody);
            socket->disconnectFromHost();
        });
    });

    // Close error dialogs automatically while still checking that failures are reported.
    int warningCount = 0;
    QString warningText;
    QTimer closeWarnings;
    connect(&closeWarnings, &QTimer::timeout, &closeWarnings, [&]() {
        for (auto *widget : QApplication::topLevelWidgets()) {
            auto *message = qobject_cast<QMessageBox *>(widget);
            if (message != nullptr && message->isVisible()) {
                ++warningCount;
                warningText = message->text();
                message->accept();
            }
        }
    });
    closeWarnings.start(10);

    NextcloudDeckService service(nullptr, connection.getId());
    // Source IDs and configured defaults must not override the destination.
    service.setBoardAndStackIds(12, 34);
    QCOMPARE(service.moveCard(56, targetBoardId, 78, 2), success);
    QCOMPARE(requestCount, 1);
    QVERIFY(request.startsWith("PUT /index.php/apps/deck/api/v1.1/boards/" +
                               QByteArray::number(targetBoardId) +
                               "/stacks/78/cards/56/reorder HTTP/1.1\r\n"));
    const auto headerValue = [&](const QByteArray &name) {
        for (const auto &line : request.left(request.indexOf("\r\n\r\n")).split('\n')) {
            const int colon = line.indexOf(':');
            if (colon > 0 && line.left(colon).toLower() == name.toLower()) {
                return line.mid(colon + 1).trimmed();
            }
        }
        return QByteArray();
    };
    QCOMPARE(headerValue("OCS-APIRequest"), QByteArray("true"));
    QCOMPARE(headerValue("Authorization"),
             "Basic " + QByteArray("deck-user:deck-password").toBase64());
    const auto body =
        QJsonDocument::fromJson(request.mid(request.indexOf("\r\n\r\n") + 4)).object();
    QCOMPARE(body.size(), 2);
    QCOMPARE(body.value(QStringLiteral("stackId")).toInt(), 78);
    QCOMPARE(body.value(QStringLiteral("order")).toInt(), 2);
    QCOMPARE(warningCount, success ? 0 : 1);
    if (statusCode == 403) {
        QVERIFY(warningText.contains(QStringLiteral("Permission denied on destination stack")));
    } else if (statusCode >= 200 && statusCode < 300 && !success) {
        QVERIFY(warningText.contains(QStringLiteral("did not confirm")));
    }
    settings.remove(warningKey);
    QVERIFY(connection.remove());
}

void TestNextcloudDeckService::testInvalidMove() {
    NextcloudDeckService service(nullptr, CloudConnection::NoneCloudConnectionId);
    service.setBoardAndStackIds(12, 34);
    QVERIFY(!service.moveCard(0, 90, 78));
    QVERIFY(!service.moveCard(56, 0, 78));
    QVERIFY(!service.moveCard(56, 90, 0));
    QVERIFY(!service.moveCard(56, 90, 78, -1));
    service.setBoardAndStackIds(0, 34);
    QVERIFY(!service.moveCard(56, 90, 78));
    service.setBoardAndStackIds(12, 0);
    QVERIFY(!service.moveCard(56, 90, 78));
}
