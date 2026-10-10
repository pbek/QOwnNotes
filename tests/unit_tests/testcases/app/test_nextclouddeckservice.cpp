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

void TestNextcloudDeckService::testMoveCard_data() {
    QTest::addColumn<int>("statusCode");
    QTest::addColumn<bool>("success");
    QTest::newRow("success") << 200 << true;
    QTest::newRow("no-content") << 204 << true;
    QTest::newRow("permission-denied") << 403 << false;
    QTest::newRow("server-error") << 500 << false;
    QTest::newRow("timeout") << 0 << false;
}

void TestNextcloudDeckService::testMoveCard() {
    QFETCH(int, statusCode);
    QFETCH(bool, success);

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
            socket->write("HTTP/1.1 " + QByteArray::number(statusCode) +
                          " Test\r\nContent-Length: 0\r\nConnection: close\r\n\r\n");
            socket->disconnectFromHost();
        });
    });

    // Close error dialogs automatically while still checking that failures are reported.
    int warningCount = 0;
    QTimer closeWarnings;
    connect(&closeWarnings, &QTimer::timeout, &closeWarnings, [&]() {
        for (auto *widget : QApplication::topLevelWidgets()) {
            auto *message = qobject_cast<QMessageBox *>(widget);
            if (message != nullptr && message->isVisible()) {
                ++warningCount;
                message->accept();
            }
        }
    });
    closeWarnings.start(10);

    NextcloudDeckService service(nullptr, connection.getId());
    // The request URL must use the source board/stack, not the configured defaults.
    service.setBoardAndStackIds(12, 34);
    QCOMPARE(service.moveCard(56, 78, 2), success);
    QCOMPARE(requestCount, 1);
    QVERIFY(request.startsWith(
        "PUT /index.php/apps/deck/api/v1.1/boards/12/stacks/34/cards/56/reorder HTTP/1.1\r\n"));
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
    QVERIFY(connection.remove());
}

void TestNextcloudDeckService::testInvalidMove() {
    NextcloudDeckService service(nullptr, CloudConnection::NoneCloudConnectionId);
    service.setBoardAndStackIds(12, 34);
    QVERIFY(!service.moveCard(0, 78));
    QVERIFY(!service.moveCard(56, 0));
    QVERIFY(!service.moveCard(56, 78, -1));
    service.setBoardAndStackIds(0, 34);
    QVERIFY(!service.moveCard(56, 78));
    service.setBoardAndStackIds(12, 0);
    QVERIFY(!service.moveCard(56, 78));
}
