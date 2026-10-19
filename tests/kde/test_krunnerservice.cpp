#include <services/krunnerservice.h>

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QElapsedTimer>
#include <QSettings>
#include <QSignalSpy>
#include <QtTest>

namespace {
const QString interfaceName = QStringLiteral("org.kde.krunner1");
}    // namespace

/**
 * Calls the runner like KRunner, over a separate bus connection
 */
class TestKRunner : public QObject {
    Q_OBJECT

   private:
    KRunnerService *_service = nullptr;
    QStringList _queries;
    QDBusConnection _client = QDBusConnection(QStringLiteral("unconnected"));

    QDBusPendingCall call(const QString &method, const QList<QVariant> &args = {}) {
        QDBusMessage message = QDBusMessage::createMethodCall(
            _service->serviceName(), KRunnerService::objectPath, interfaceName, method);
        message.setArguments(args);
        return _client.asyncCall(message);
    }

    // The service answers in the event loop of this thread, so blocking would time out
    static void waitFor(const QDBusPendingCall &pendingCall) {
        QElapsedTimer timer;
        timer.start();
        while (!pendingCall.isFinished() && timer.elapsed() < 5000) {
            QTest::qWait(10);
        }
    }

    KRunnerMatches match(const QString &query) {
        QDBusPendingReply<KRunnerMatches> reply = call(QStringLiteral("Match"), {query});
        waitFor(reply);
        if (reply.isError()) {
            qWarning() << reply.error();
        }
        return reply.value();
    }

   private slots:
    void initTestCase() {
        if (qgetenv("QON_KDE_TEST_BUS") != "1") {
            QSKIP("Run with dbus-run-session and QON_KDE_TEST_BUS=1");
        }

        _service = new KRunnerService(
            [this](const QString &query, int limit) {
                _queries.append(query);
                KRunnerMatches matches;
                // More results than allowed
                for (int i = 0; i < limit + 5; ++i) {
                    KRunnerMatch match;
                    match.id = QStringLiteral("1/note%1.md").arg(i);
                    match.text = query + QString::number(i);
                    match.iconName = QStringLiteral("text-markdown");
                    match.categoryRelevance = 50;
                    match.relevance = 0.8;
                    match.properties.insert(QStringLiteral("subtext"), QStringLiteral("Notes"));
                    matches.append(match);
                }
                return matches;
            },
            this);
        QVERIFY(_service->registerOnBus(QDBusConnection::sessionBus()));
        QVERIFY(_service->serviceName().startsWith(KRunnerService::serviceNamePrefix +
                                                   QStringLiteral(".i")));

        _client = QDBusConnection::connectToBus(QDBusConnection::SessionBus,
                                                QStringLiteral("krunner-client"));
        QVERIFY(_client.isConnected());
        QVERIFY(_client.interface()->isServiceRegistered(_service->serviceName()));
    }

    void pluginFileMatchesServiceName() {
        QSettings plugin(QStringLiteral(KRUNNER_PLUGIN_FILE), QSettings::IniFormat);
        plugin.beginGroup(QStringLiteral("Desktop Entry"));
        QString service = plugin.value(QStringLiteral("X-Plasma-DBusRunner-Service")).toString();
        QVERIFY(service.endsWith(QLatin1Char('*')));
        service.chop(1);
        // KRunner uses all services whose name starts with the part before the wildcard
        QVERIFY(_service->serviceName().startsWith(service));
        QVERIFY(service.startsWith(KRunnerService::serviceNamePrefix));
        QCOMPARE(plugin.value(QStringLiteral("X-Plasma-DBusRunner-Path")).toString(),
                 KRunnerService::objectPath);
        QCOMPARE(plugin.value(QStringLiteral("X-Plasma-Runner-Min-Letter-Count")).toInt(),
                 KRunnerService::minQueryLength);
    }

    void ignoresShortQueries() {
        _queries.clear();
        QVERIFY(match(QStringLiteral(" ab ")).isEmpty());
        QVERIFY(_queries.isEmpty());
    }

    void returnsCappedMatches() {
        _queries.clear();
        const KRunnerMatches matches = match(QStringLiteral(" note "));
        QCOMPARE(_queries, QStringList{QStringLiteral("note")});
        QCOMPARE(matches.size(), KRunnerService::maxResults);
        QCOMPARE(matches.first().id, QStringLiteral("1/note0.md"));
        QCOMPARE(matches.first().text, QStringLiteral("note0"));
        QCOMPARE(matches.first().iconName, QStringLiteral("text-markdown"));
        QCOMPARE(matches.first().categoryRelevance, 50);
        QCOMPARE(matches.first().relevance, 0.8);
        QCOMPARE(matches.first().properties.value(QStringLiteral("subtext")).toString(),
                 QStringLiteral("Notes"));
    }

    void newerQuerySupersedesPendingQuery() {
        _queries.clear();
        QDBusPendingReply<KRunnerMatches> first =
            call(QStringLiteral("Match"), {QStringLiteral("first")});
        QDBusPendingReply<KRunnerMatches> second =
            call(QStringLiteral("Match"), {QStringLiteral("second")});
        waitFor(second);
        waitFor(first);
        QVERIFY(!first.isError());
        QVERIFY(!second.isError());
        QCOMPARE(second.value().size(), KRunnerService::maxResults);
        QVERIFY(_queries.contains(QStringLiteral("second")));
        // The first query is only searched if it was processed before the second arrived
        QCOMPARE(first.value().isEmpty(), !_queries.contains(QStringLiteral("first")));
    }

    void hasNoActions() {
        QDBusPendingReply<KRunnerActions> reply = call(QStringLiteral("Actions"));
        waitFor(reply);
        QVERIFY(!reply.isError());
        QVERIFY(reply.value().isEmpty());
    }

    void runsWithActivationToken() {
        QSignalSpy spy(_service, &KRunnerService::runRequested);
        waitFor(call(QStringLiteral("SetActivationToken"), {QStringLiteral("test-token")}));
        waitFor(call(QStringLiteral("Run"), {QStringLiteral("1/note.md"), QString()}));
        QTRY_COMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).toString(), QStringLiteral("1/note.md"));
        QCOMPARE(spy.at(0).at(2).toString(), QStringLiteral("test-token"));

        // A token is only used once
        waitFor(call(QStringLiteral("Run"), {QStringLiteral("1/note.md"), QString()}));
        QTRY_COMPARE(spy.count(), 2);
        QVERIFY(spy.at(1).at(2).toString().isEmpty());
    }

    void unregisters() {
        const QString serviceName = _service->serviceName();
        _service->unregisterFromBus();
        QTRY_VERIFY(!_client.interface()->isServiceRegistered(serviceName));
    }
};

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    TestKRunner tests;
    return QTest::qExec(&tests, argc, argv);
}

#include "test_krunnerservice.moc"
