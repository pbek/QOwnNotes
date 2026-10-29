#include "test_network.h"

#include <QSignalSpy>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUrl>
#include <QtTest>

#include "entities/calendaritem.h"
#include "services/cloudservice.h"
#include "services/databaseservice.h"
#include "services/settingsservice.h"
#include "utils/misc.h"

namespace {
class CalendarServer : public QTcpServer {
   public:
    QByteArray request;
    int statusCode = 204;
    QByteArray responseBody;

    CalendarServer() {
        connect(this, &QTcpServer::newConnection, this, [this]() {
            auto *socket = nextPendingConnection();
            connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
            connect(socket, &QTcpSocket::readyRead, this, [this, socket]() {
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
                socket->write(
                    "HTTP/1.1 " + QByteArray::number(statusCode) +
                    " Test\r\nContent-Length: " + QByteArray::number(responseBody.size()) +
                    "\r\nConnection: close\r\n\r\n" + responseBody);
                socket->disconnectFromHost();
            });
        });
    }

    QString calendarUrl() const {
        return QStringLiteral("http://127.0.0.1:%1/calendar/").arg(serverPort());
    }
};
}    // namespace

void TestNetwork::initTestCase() {
    DatabaseService::createConnection();
    DatabaseService::setupTables();
}

void TestNetwork::testCalendarWrite_data() {
    QTest::addColumn<int>("statusCode");
    QTest::addColumn<QString>("operation");
    for (const QString &operation : {QStringLiteral("rename"), QStringLiteral("complete"),
                                     QStringLiteral("move"), QStringLiteral("create")}) {
        for (int status : {201, 204, 302, 403, 404, 500}) {
            QTest::newRow(qPrintable(operation + "-" + QString::number(status)))
                << status << operation;
        }
    }
}

void TestNetwork::testCalendarWrite() {
    QFETCH(int, statusCode);
    QFETCH(QString, operation);
    CalendarServer server;
    server.statusCode = statusCode;
    QVERIFY(server.listen(QHostAddress::LocalHost));

    SettingsService settings;
    settings.setValue(QStringLiteral("ownCloud/todoCalendarBackend"), CloudService::CalDAVCalendar);
    settings.setValue(QStringLiteral("ownCloud/todoCalendarCalDAVServerUrl"), server.calendarUrl());
    settings.setValue(QStringLiteral("ownCloud/todoCalendarEnabledList"),
                      QStringList{QStringLiteral("Calendar write test")});
    settings.setValue(QStringLiteral("ownCloud/todoCalendarEnabledUrlList"),
                      QStringList{server.calendarUrl()});
    settings.setValue(QStringLiteral("TodoDialog/todoListSelectorSelectedItem"),
                      QStringLiteral("Calendar write test"));

    CalendarItem item = CalendarItem::createNewTodoItem(QStringLiteral("Original"),
                                                        QStringLiteral("Calendar write test"));
    QVERIFY(!CalendarItem::fetchByUid(item.getUid()).isFetched());
    const bool creating = operation == QStringLiteral("create");
    if (!creating) {
        item.setETag(QStringLiteral("unchanged-server-etag"));
        QVERIFY(item.store());
    }
    CalendarItem original = item;
    if (operation == QStringLiteral("complete")) {
        item.updateCompleted(true);
    } else if (operation == QStringLiteral("move")) {
        item.setRelatedUid(QStringLiteral("new-parent"));
    } else {
        item.setSummary(QStringLiteral("Changed"));
    }

    CloudService service;
    QSignalSpy finished(&service, &CloudService::calendarItemWriteFinished);
    bool successCallbackCalled = false;
    service.postCalendarItemToServer(item, nullptr, [&]() { successCallbackCalled = true; });
    // Pending writes must not alter the cache either.
    QCOMPARE(CalendarItem::fetchByUid(item.getUid()).isFetched(), !creating);
    if (!creating) {
        QCOMPARE(CalendarItem::fetchByUid(item.getUid()).getSummary(), original.getSummary());
        QVERIFY(!CalendarItem::fetchByUid(item.getUid()).isCompleted());
        QVERIFY(CalendarItem::fetchByUid(item.getUid()).getRelatedUid().isEmpty());
    }
    QTRY_COMPARE(finished.count(), 1);
    const bool success = statusCode >= 200 && statusCode < 300;
    QCOMPARE(finished.first().at(0).toString(), item.getUrl());
    QCOMPARE(finished.first().at(1).toBool(), success);
    QCOMPARE(finished.first().at(2).toString().isEmpty(), success);
    QCOMPARE(successCallbackCalled, success);
    QVERIFY(server.request.startsWith("PUT /calendar/qownnotes-"));
    QVERIFY(server.request.contains("Content-Type: text/calendar; charset=utf-8\r\n"));
    QVERIFY(server.request.contains("BEGIN:VTODO"));

    CalendarItem cached = CalendarItem::fetchByUid(item.getUid());
    QCOMPARE(cached.isFetched(), !creating || success);
    if (cached.isFetched()) {
        CalendarItem expected = success ? item : original;
        QCOMPARE(cached.getSummary(), expected.getSummary());
        QCOMPARE(cached.isCompleted(), expected.isCompleted());
        QCOMPARE(cached.getRelatedUid(), expected.getRelatedUid());
        QCOMPARE(cached.getETag(), expected.getETag());
        QVERIFY(cached.remove());
    }
    settings.clear();
}

void TestNetwork::testCalendarReadFailurePreservesICS() {
    CalendarServer server;
    server.statusCode = 404;
    server.responseBody = "Not found";
    QVERIFY(server.listen(QHostAddress::LocalHost));
    SettingsService settings;
    settings.setValue(QStringLiteral("ownCloud/todoCalendarBackend"), CloudService::CalDAVCalendar);
    settings.setValue(QStringLiteral("ownCloud/todoCalendarCalDAVServerUrl"), server.calendarUrl());
    CalendarItem item;
    item.setUrl(QUrl(server.calendarUrl() + "card-53.ics"));
    item.setICSData(QStringLiteral("Original ICS data"));
    CloudService service;
    QVERIFY(!service.updateICSDataOfCalendarItem(&item));
    QCOMPARE(item.getICSData(), QStringLiteral("Original ICS data"));
    QVERIFY(server.request.startsWith("GET /calendar/card-53.ics"));
    settings.clear();
}

void TestNetwork::testQOwnNotesApiDiscovery() {
    QCOMPARE(
        CloudService::apiBaseFromCapabilities(
            R"({"ocs":{"data":{"capabilities":{"qownnotes":{"api_base":"/index.php/apps/qownnotes/api/v1/"}}}}})"),
        QStringLiteral("/index.php/apps/qownnotes/api/v1/"));
    for (
        const QByteArray &data :
        {QByteArray("{}"), QByteArray("<ocs/>"),
         QByteArray(
             R"({"ocs":{"data":{"capabilities":{"qownnotes":{"api_base":"https://evil.test/"}}}}})"),
         QByteArray(
             R"({"ocs":{"data":{"capabilities":{"qownnotes":{"api_base":"//evil.test/"}}}}})")}) {
        QCOMPARE(CloudService::apiBaseFromCapabilities(data),
                 QStringLiteral("/index.php/apps/qownnotesapi/api/v1/"));
    }
}

// Disabled on Linux because of https://github.com/pbek/QOwnNotes/issues/3212
#ifndef Q_OS_LINUX
void TestNetwork::testSSL() {
    auto data = Utils::Misc::downloadUrl(QUrl("https://www.github.com"));

    QVERIFY(data.size() > 0);
}
#endif
