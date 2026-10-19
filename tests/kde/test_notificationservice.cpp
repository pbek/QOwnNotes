#include <services/notificationservice.h>

#include <KNotifyConfig>
#include <QApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QSignalSpy>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QtTest>

namespace {
const QString service = QStringLiteral("org.freedesktop.Notifications");
const QString path = QStringLiteral("/org/freedesktop/Notifications");
}    // namespace

/**
 * Records notifications like a desktop notification server
 */
class MockNotifications : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Notifications")

   public slots:
    QStringList GetCapabilities() {
        return {QStringLiteral("actions"), QStringLiteral("body"), QStringLiteral("body-markup")};
    }
    uint Notify(const QString &appName, uint, const QString &, const QString &summary,
                const QString &body, const QStringList &actions, const QVariantMap &hints, int) {
        _appName = appName;
        _summary = summary;
        _body = body;
        _actions = actions;
        _hints = hints;
        return ++_count;
    }
    void CloseNotification(uint id) { emit NotificationClosed(id, 3); }

    int count() { return static_cast<int>(_count); }
    QString lastAppName() { return _appName; }
    QString lastSummary() { return _summary; }
    QString lastBody() { return _body; }
    QStringList lastActions() { return _actions; }
    QString lastHint(const QString &key) { return _hints.value(key).toString(); }
    void invokeLast(const QString &action) {
        emit ActivationToken(_count, QStringLiteral("test-token"));
        emit ActionInvoked(_count, action);
    }

   signals:
    void ActionInvoked(uint id, const QString &actionKey);
    void ActivationToken(uint id, const QString &token);
    void NotificationClosed(uint id, uint reason);

   private:
    uint _count = 0;
    QString _appName;
    QString _summary;
    QString _body;
    QStringList _actions;
    QVariantMap _hints;
};

class TestNotifications : public QObject {
    Q_OBJECT
   private:
    QProcess _server;

    QDBusMessage call(const QString &method, const QList<QVariant> &args = {}) {
        QDBusInterface iface(service, path, service, QDBusConnection::sessionBus());
        return iface.callWithArgumentList(QDBus::Block, method, args);
    }
    int count() { return QDBusReply<int>(call(QStringLiteral("count"))).value(); }
    QString text(const QString &method, const QList<QVariant> &args = {}) {
        return QDBusReply<QString>(call(method, args)).value();
    }
    QString userConfigPath() {
        return QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) +
               QStringLiteral("/qownnotes.notifyrc");
    }

   private slots:
    void initTestCase() {
        QVERIFY2(qEnvironmentVariableIntValue("QON_KDE_TEST_BUS") == 1,
                 "Run tests on an isolated bus with dbus-run-session (see CMakeLists.txt)");
        QVERIFY(!NotificationService::isDesktopNotificationServiceAvailable());
        QFile::remove(userConfigPath());
    }
    void fallbackWithoutService() {
        NotificationService notifications;
        QSignalSpy status(&notifications, &NotificationService::statusMessageRequested);

        // Without a notification service or tray, scripts fall back to the status bar
        QVERIFY(!notifications.notify(NotificationService::Event::ScriptNotification,
                                      QStringLiteral("Title"), QStringLiteral("Text")));
        QCOMPARE(status.count(), 1);
        QCOMPARE(status.at(0).at(0).toString(), QStringLiteral("Title: Text"));

        // Identical notifications are suppressed
        QVERIFY(!notifications.notify(NotificationService::Event::ScriptNotification,
                                      QStringLiteral("Title"), QStringLiteral("Text")));
        QCOMPARE(status.count(), 1);

        // Built-in events already have their own status bar messages
        QVERIFY(!notifications.notify(NotificationService::Event::ExternalNoteConflict,
                                      QStringLiteral("Title"), QStringLiteral("Text")));
        QCOMPARE(status.count(), 1);
    }
    void desktopNotificationAndAction() {
        _server.start(QCoreApplication::applicationFilePath(), {QStringLiteral("--mock-service")});
        QVERIFY(_server.waitForStarted());
        QTRY_VERIFY(NotificationService::isDesktopNotificationServiceAvailable());

        NotificationService notifications;
        QSignalSpy status(&notifications, &NotificationService::statusMessageRequested);
        bool triggered = false;
        QVERIFY(notifications.notify(NotificationService::Event::ScriptNotification,
                                     QStringLiteral("Script title"),
                                     QStringLiteral("a <b>bold</b> & text"), QStringLiteral("Show"),
                                     [&triggered]() { triggered = true; }));
        QTRY_COMPARE(count(), 1);

        // The application name comes from the embedded notifyrc file
        QCOMPARE(text(QStringLiteral("lastAppName")), QStringLiteral("QOwnNotes"));
        QCOMPARE(text(QStringLiteral("lastSummary")), QStringLiteral("Script title"));
        // Text is escaped, so it can't contain markup
        QCOMPARE(text(QStringLiteral("lastBody")),
                 QStringLiteral("a &lt;b&gt;bold&lt;/b&gt; &amp; text"));
        QCOMPARE(QDBusReply<QStringList>(call(QStringLiteral("lastActions"))).value(),
                 QStringList({QStringLiteral("default"), QStringLiteral("Show")}));
        QCOMPARE(text(QStringLiteral("lastHint"), {QStringLiteral("x-kde-appname")}),
                 QStringLiteral("qownnotes"));
        QCOMPARE(text(QStringLiteral("lastHint"), {QStringLiteral("x-kde-eventId")}),
                 QStringLiteral("scriptNotification"));

        // Clicking the notification runs the action with the activation token
        call(QStringLiteral("invokeLast"), {QStringLiteral("default")});
        QTRY_VERIFY(triggered);
        QCOMPARE(qgetenv("XDG_ACTIVATION_TOKEN"), QByteArray("test-token"));
        qunsetenv("XDG_ACTIVATION_TOKEN");

        QVERIFY(notifications.notify(NotificationService::Event::ExternalNoteConflict,
                                     QStringLiteral("Conflict"), QStringLiteral("Review")));
        QTRY_COMPARE(count(), 2);
        QCOMPARE(text(QStringLiteral("lastHint"), {QStringLiteral("x-kde-eventId")}),
                 QStringLiteral("externalNoteConflict"));
        QVERIFY(QDBusReply<QStringList>(call(QStringLiteral("lastActions"))).value().isEmpty());
        QCOMPARE(status.count(), 0);
    }
    void disabledEventIsNotShown() {
        // Like disabling the event in Plasma's notification settings
        QVERIFY(QDir().mkpath(QFileInfo(userConfigPath()).absolutePath()));
        QFile config(userConfigPath());
        QVERIFY(config.open(QIODevice::WriteOnly));
        config.write("[Event/scriptNotification]\nAction=None\n");
        config.close();
        KNotifyConfig::reparseConfiguration();

        NotificationService notifications;
        QSignalSpy status(&notifications, &NotificationService::statusMessageRequested);
        const int before = count();
        QVERIFY(notifications.notify(NotificationService::Event::ScriptNotification,
                                     QStringLiteral("Disabled"), QStringLiteral("Text")));
        QTest::qWait(300);
        QCOMPARE(count(), before);
        // The user's choice is respected, so there's no fallback either
        QCOMPARE(status.count(), 0);

        QVERIFY(QFile::remove(userConfigPath()));
        KNotifyConfig::reparseConfiguration();
    }
    void serviceLossFallsBack() {
        _server.kill();
        QVERIFY(_server.waitForFinished());
        QTRY_VERIFY(!NotificationService::isDesktopNotificationServiceAvailable());

        NotificationService notifications;
        QSignalSpy status(&notifications, &NotificationService::statusMessageRequested);
        QVERIFY(!notifications.notify(NotificationService::Event::ScriptNotification, QString(),
                                      QStringLiteral("Only text")));
        QCOMPARE(status.count(), 1);
        QCOMPARE(status.at(0).at(0).toString(), QStringLiteral("Only text"));
    }
    void cleanupTestCase() {
        _server.kill();
        _server.waitForFinished();
    }
};

int main(int argc, char **argv) {
    if (argc > 1 && QByteArray(argv[1]) == "--mock-service") {
        QCoreApplication app(argc, argv);
        MockNotifications mock;
        auto bus = QDBusConnection::sessionBus();
        if (!bus.registerObject(
                path, &mock, QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals) ||
            !bus.registerService(service)) {
            return 1;
        }
        return app.exec();
    }
    // Keep the user's notification settings out of the tests
    QTemporaryDir home;
    if (!home.isValid()) {
        return 1;
    }
    qputenv("XDG_CONFIG_HOME", QFile::encodeName(home.filePath(QStringLiteral("config"))));
    qputenv("XDG_DATA_HOME", QFile::encodeName(home.filePath(QStringLiteral("data"))));
    QApplication app(argc, argv);
    TestNotifications tests;
    return QTest::qExec(&tests, argc, argv);
}

#include "test_notificationservice.moc"
