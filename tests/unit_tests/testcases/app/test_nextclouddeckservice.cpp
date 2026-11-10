#include "test_nextclouddeckservice.h"

#include <QApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QSettings>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QtTest>

#include "entities/cloudconnection.h"
#include "services/nextclouddeckservice.h"
#include "services/settingsservice.h"

void TestNextcloudDeckService::testRecentCards() {
    CloudConnection connection;
    connection.setName(QStringLiteral("Recent Deck cards test"));
    connection.setServerUrl(QStringLiteral("http://127.0.0.1:1"));
    QVERIFY(connection.store());
    CloudConnection otherConnection;
    otherConnection.setName(QStringLiteral("Other recent Deck cards test"));
    QVERIFY(otherConnection.store());

    // All history operations work offline, even with Deck disabled and no configured stack.
    NextcloudDeckService service(nullptr, connection.getId());
    NextcloudDeckService otherService(nullptr, otherConnection.getId());
    QVERIFY(service.getRecentCards().isEmpty());
    for (int id = 1; id <= 12; ++id) {
        NextcloudDeckService::Card card{};
        card.id = id;
        card.boardId = 34;
        card.title = QStringLiteral("Card %1").arg(id);
        card.description = QStringLiteral("Private description must not be stored");
        service.rememberRecentCard(card);
    }
    const auto cards = service.getRecentCards();
    QCOMPARE(cards.size(), 10);
    for (int i = 0; i < cards.size(); ++i) {
        QCOMPARE(cards.at(i).id, 12 - i);
    }

    // Revisiting a card updates its title and board without creating a duplicate.
    NextcloudDeckService::Card revisited{};
    revisited.id = 5;
    revisited.boardId = 90;
    revisited.title = QStringLiteral("Renamed & moved card");
    service.rememberRecentCard(revisited);
    QCOMPARE(service.getRecentCards().size(), 10);
    QCOMPARE(service.getRecentCards().first().id, 5);
    QCOMPARE(service.getRecentCards().first().boardId, 90);
    QCOMPARE(service.getRecentCards().first().title, revisited.title);

    auto otherCard = revisited;
    otherCard.title = QStringLiteral("Different account, same card ID");
    otherService.rememberRecentCard(otherCard);
    QCOMPARE(otherService.getRecentCards().size(), 1);
    QCOMPARE(service.getRecentCards().first().title, revisited.title);

    SettingsService().sync();
    NextcloudDeckService reopened(nullptr, connection.getId());
    QCOMPARE(reopened.getRecentCards().size(), 10);
    QCOMPARE(reopened.getRecentCards().first().title, revisited.title);
    // Read the disk settings directly instead of the SettingsService cache, as after a restart.
    const auto stored = QSettings()
                            .value(QStringLiteral("CloudConnection-%1/nextcloudDeckRecentCards")
                                       .arg(connection.getId()))
                            .toList();
    QCOMPARE(stored.size(), 10);
    QCOMPARE(stored.first().toMap().size(), 3);
    QVERIFY(!stored.first().toMap().contains(QStringLiteral("description")));

    service.forgetRecentCard(5);
    QCOMPARE(service.getRecentCards().size(), 9);
    QCOMPARE(service.getRecentCards().first().id, 12);
    service.forgetRecentCard(999);
    QCOMPARE(service.getRecentCards().size(), 9);
    revisited.id = 12;
    revisited.archived = true;
    service.rememberRecentCard(revisited);
    QCOMPARE(service.getRecentCards().size(), 8);
    QCOMPARE(service.getRecentCards().first().id, 11);
    QCOMPARE(otherService.getRecentCards().size(), 1);

    QVERIFY(connection.remove());
    QVERIFY(otherConnection.remove());
    // Deleting a connection must also clear cached history before its ID can be reused.
    QVERIFY(service.getRecentCards().isEmpty());
    QVERIFY(otherService.getRecentCards().isEmpty());
}

void TestNextcloudDeckService::testInvalidRecentCards() {
    CloudConnection connection;
    connection.setName(QStringLiteral("Invalid recent Deck cards test"));
    QVERIFY(connection.store());
    NextcloudDeckService service(nullptr, connection.getId());
    NextcloudDeckService::Card card{};
    card.title = QStringLiteral("Unsaved card");
    service.rememberRecentCard(card);
    card.id = 1;
    service.rememberRecentCard(card);
    card.boardId = 2;
    card.title.clear();
    service.rememberRecentCard(card);
    QVERIFY(service.getRecentCards().isEmpty());

    NextcloudDeckService invalidConnection(nullptr, 0);
    card.title = QStringLiteral("No cloud connection");
    invalidConnection.rememberRecentCard(card);
    QVERIFY(invalidConnection.getRecentCards().isEmpty());
    invalidConnection.forgetRecentCard(card.id);
    QVERIFY(connection.remove());
}

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

void TestNextcloudDeckService::testParseLabels() {
    QCOMPARE(NextcloudDeckService::labelsFromJson(QJsonValue()).count(), 0);

    const auto labels = NextcloudDeckService::labelsFromJson(
        QJsonDocument::fromJson("[{\"id\":5,\"title\":\"Finished\",\"color\":\"31CC7C\"},"
                                "{\"id\":\"6\",\"title\":\"Later\",\"color\":\"#F1DB50\"},"
                                "{\"title\":\"Invalid\"}]")
            .array());
    QCOMPARE(labels.count(), 2);
    QCOMPARE(labels.at(0).id, 5);
    QCOMPARE(labels.at(0).title, QStringLiteral("Finished"));
    QCOMPARE(labels.at(0).color, QStringLiteral("31CC7C"));
    QCOMPARE(labels.at(1).id, 6);
    QCOMPARE(labels.at(1).color, QStringLiteral("F1DB50"));
}

void TestNextcloudDeckService::testLabelRequests() {
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost));
    CloudConnection connection;
    connection.setName(QStringLiteral("Deck label test"));
    connection.setServerUrl(QStringLiteral("http://127.0.0.1:%1").arg(server.serverPort()));
    connection.setUsername(QStringLiteral("deck-user"));
    connection.setPassword(QStringLiteral("deck-password"));
    QVERIFY(connection.store());

    // Each connection gets a single response, the requests are recorded as "VERB path body"
    QStringList requests;
    QByteArray createResponse =
        "{\"id\":9,\"title\":\"Urgent\",\"color\":\"FF7A66\",\"boardId\":12}";
    connect(&server, &QTcpServer::newConnection, &server, [&]() {
        auto *socket = server.nextPendingConnection();
        auto *buffer = new QByteArray();
        connect(socket, &QTcpSocket::disconnected, socket, [socket, buffer]() {
            delete buffer;
            socket->deleteLater();
        });
        connect(socket, &QTcpSocket::readyRead, &server, [&, socket, buffer]() {
            *buffer += socket->readAll();
            const int headerEnd = buffer->indexOf("\r\n\r\n");
            if (headerEnd < 0) {
                return;
            }
            int contentLength = 0;
            for (const auto &line : buffer->left(headerEnd).split('\n')) {
                if (line.toLower().startsWith("content-length:")) {
                    contentLength = line.mid(line.indexOf(':') + 1).trimmed().toInt();
                }
            }
            if (buffer->size() < headerEnd + 4 + contentLength) {
                return;
            }
            const QList<QByteArray> requestLine = buffer->left(buffer->indexOf("\r\n")).split(' ');
            requests.append(QString::fromUtf8(requestLine.value(0) + ' ' + requestLine.value(1) +
                                              ' ' + buffer->mid(headerEnd + 4)));
            const QByteArray body =
                requestLine.value(0) == "POST" ? createResponse : QByteArray("{}");
            socket->write("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " +
                          QByteArray::number(body.size()) + "\r\nConnection: close\r\n\r\n" + body);
            socket->disconnectFromHost();
        });
    });

    NextcloudDeckService service(nullptr, connection.getId());
    service.setBoardAndStackIds(12, 34);

    const auto label = service.createLabel(QStringLiteral("  Urgent "), QStringLiteral("#ff7a66"));
    QCOMPARE(label.id, 9);
    QCOMPARE(label.title, QStringLiteral("Urgent"));
    QCOMPARE(label.color, QStringLiteral("FF7A66"));
    QCOMPARE(requests.count(), 1);
    QCOMPARE(requests.at(0), QStringLiteral("POST /index.php/apps/deck/api/v1.1/boards/12/labels "
                                            "{\"color\":\"FF7A66\",\"title\":\"Urgent\"}"));

    // Only the differences between the current and the wanted labels are sent
    requests.clear();
    QVERIFY(service.setCardLabels(56, {1, 2}, {2, 3}));
    QCOMPARE(requests,
             QStringList({QStringLiteral("PUT /index.php/apps/deck/api/v1.1/boards/12/stacks/34/"
                                         "cards/56/removeLabel {\"labelId\":1}"),
                          QStringLiteral("PUT /index.php/apps/deck/api/v1.1/boards/12/stacks/34/"
                                         "cards/56/assignLabel {\"labelId\":3}")}));

    requests.clear();
    QVERIFY(service.setCardLabels(56, {2}, {2}));
    QVERIFY(requests.isEmpty());
    QVERIFY(!service.assignLabel(0, 3));
    QVERIFY(requests.isEmpty());

    QVERIFY(connection.remove());
}
