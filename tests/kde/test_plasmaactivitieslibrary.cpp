#include <services/plasmaactivitieslibrary.h>

#include <QCoreApplication>
#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QProcess>
#include <QSignalSpy>
#include <QtTest>

namespace {
const QString service = QStringLiteral("org.kde.ActivityManager");
const QString activitiesPath = QStringLiteral("/ActivityManager/Activities");
const QString activitiesInterface = QStringLiteral("org.kde.ActivityManager.Activities");
const QString autostartProperty = QStringLiteral("org.kde.KActivities.core.disableAutostart");
}    // namespace

// Same D-Bus structure "(ssssi)" as kactivitymanagerd's ActivityInfo
struct MockActivityInfo {
    QString id;
    QString name;
    QString description;
    QString icon;
    int state = 2;
};
Q_DECLARE_METATYPE(MockActivityInfo)

QDBusArgument &operator<<(QDBusArgument &argument, const MockActivityInfo &info) {
    argument.beginStructure();
    argument << info.id << info.name << info.description << info.icon << info.state;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, MockActivityInfo &info) {
    argument.beginStructure();
    argument >> info.id >> info.name >> info.description >> info.icon >> info.state;
    argument.endStructure();
    return argument;
}

class MockApplication : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.ActivityManager.Application")
   public:
    QString version;

   public slots:
    QString serviceVersion() { return version; }
};

/**
 * Simulates the activities of kactivitymanagerd
 */
class MockActivities : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.ActivityManager.Activities")
   public:
    MockActivities() {
        add(QStringLiteral("work-id"), QStringLiteral("Work"));
        add(QStringLiteral("home-id"), QStringLiteral("Home"));
        _current = QStringLiteral("work-id");
    }

   public slots:
    QString CurrentActivity() { return _current; }
    QStringList ListActivities() {
        QStringList ids;
        for (const auto &info : _activities) {
            ids << info.id;
        }
        return ids;
    }
    QList<MockActivityInfo> ListActivitiesWithInformation() { return _activities; }
    MockActivityInfo ActivityInformation(const QString &id) { return find(id); }
    QString ActivityName(const QString &id) { return find(id).name; }

    // Test controls
    void setCurrent(const QString &id) {
        _current = id;
        emit CurrentActivityChanged(id);
    }
    void add(const QString &id, const QString &name) {
        MockActivityInfo info;
        info.id = id;
        info.name = name;
        _activities << info;
        emit ActivityAdded(id);
    }
    void rename(const QString &id, const QString &name) {
        for (auto &info : _activities) {
            if (info.id == id) {
                info.name = name;
            }
        }
        emit ActivityNameChanged(id, name);
    }

   signals:
    void CurrentActivityChanged(const QString &activity);
    void ActivityAdded(const QString &activity);
    void ActivityRemoved(const QString &activity);
    void ActivityChanged(const QString &activity);
    void ActivityNameChanged(const QString &activity, const QString &name);

   private:
    MockActivityInfo find(const QString &id) const {
        for (const auto &info : _activities) {
            if (info.id == id) {
                return info;
            }
        }
        return {};
    }

    QList<MockActivityInfo> _activities;
    QString _current;
};

class TestPlasmaActivities : public QObject {
    Q_OBJECT
   private:
    QProcess _server;

    void startServer(const QString &version) {
        _server.start(QCoreApplication::applicationFilePath(),
                      {QStringLiteral("--mock-service"), version});
        QVERIFY(_server.waitForStarted());
        QTRY_VERIFY(isRegistered());
    }
    void stopServer() {
        _server.kill();
        QVERIFY(_server.waitForFinished());
        QTRY_VERIFY(!isRegistered());
    }
    static bool isRegistered() {
        return QDBusConnection::sessionBus().interface()->isServiceRegistered(service).value();
    }
    static QDBusMessage call(const QString &method, const QList<QVariant> &args) {
        QDBusInterface iface(service, activitiesPath, activitiesInterface,
                             QDBusConnection::sessionBus());
        return iface.callWithArgumentList(QDBus::Block, method, args);
    }
    static QString nameOf(const QString &id) {
        const auto activities = PlasmaActivitiesLibrary::instance()->activities();
        for (const auto &activity : activities) {
            if (activity.first == id) {
                return activity.second;
            }
        }
        return {};
    }

   private slots:
    void initTestCase() {
        QVERIFY2(qEnvironmentVariableIntValue("QON_KDE_TEST_BUS") == 1,
                 "Run tests on an isolated bus with dbus-run-session (see CMakeLists.txt)");
        QVERIFY(!isRegistered());
    }
    void versionComparison() {
        QVERIFY(PlasmaActivitiesLibrary::isSupportedVersion(QStringLiteral("6.2.0")));
        QVERIFY(PlasmaActivitiesLibrary::isSupportedVersion(QStringLiteral("6.7.4")));
        QVERIFY(PlasmaActivitiesLibrary::isSupportedVersion(QStringLiteral("7")));
        // The library treats a shorter "6.2" as older than 6.2.0
        QVERIFY(!PlasmaActivitiesLibrary::isSupportedVersion(QStringLiteral("6.2")));
        QVERIFY(!PlasmaActivitiesLibrary::isSupportedVersion(QStringLiteral("6.1.5")));
        QVERIFY(!PlasmaActivitiesLibrary::isSupportedVersion(QStringLiteral("5.27.11")));
        QVERIFY(!PlasmaActivitiesLibrary::isSupportedVersion(QString()));
    }
    void absentServiceIsNotStarted() {
        PlasmaActivitiesLibrary *library = PlasmaActivitiesLibrary::instance();
        library->start();
        QTest::qWait(200);
        QCOMPARE(library->state(), PlasmaActivitiesLibrary::State::Unchecked);
        QVERIFY(!library->isReady());
        QVERIFY(library->currentActivity().isEmpty());
        // The library (which could start the service) wasn't initialized
        QVERIFY(!qApp->property(autostartProperty.toLatin1()).isValid());
        QVERIFY(!isRegistered());
    }
    void oldServiceIsNotUsed() {
        PlasmaActivitiesLibrary *library = PlasmaActivitiesLibrary::instance();
        startServer(QStringLiteral("5.27.11"));
        QTRY_COMPARE(library->state(), PlasmaActivitiesLibrary::State::Unsupported);
        QVERIFY(!library->isReady());
        QVERIFY(library->activities().isEmpty());
        // The library would abort the application with this service
        QVERIFY(!qApp->property(autostartProperty.toLatin1()).isValid());
        stopServer();
    }
    void supportedServiceIsUsed() {
        PlasmaActivitiesLibrary *library = PlasmaActivitiesLibrary::instance();
        QSignalSpy current(library, &PlasmaActivitiesLibrary::currentActivityChanged);
        QSignalSpy changed(library, &PlasmaActivitiesLibrary::activitiesChanged);

        // An updated service is checked again
        startServer(QStringLiteral("6.7.4"));
        QTRY_VERIFY(library->isReady());
        QCOMPARE(library->state(), PlasmaActivitiesLibrary::State::Active);
        QVERIFY(qApp->property(autostartProperty.toLatin1()).toBool());

        QCOMPARE(library->currentActivity(), QStringLiteral("work-id"));
        QCOMPARE(library->activities().size(), 2);
        QCOMPARE(nameOf(QStringLiteral("work-id")), QStringLiteral("Work"));
        QCOMPARE(nameOf(QStringLiteral("home-id")), QStringLiteral("Home"));

        call(QStringLiteral("setCurrent"), {QStringLiteral("home-id")});
        QTRY_VERIFY(current.contains({QStringLiteral("home-id")}));
        QCOMPARE(library->currentActivity(), QStringLiteral("home-id"));

        changed.clear();
        call(QStringLiteral("rename"), {QStringLiteral("home-id"), QStringLiteral("Private")});
        QTRY_VERIFY(changed.count() > 0);
        QCOMPARE(nameOf(QStringLiteral("home-id")), QStringLiteral("Private"));

        changed.clear();
        call(QStringLiteral("add"), {QStringLiteral("new-id"), QStringLiteral("New")});
        QTRY_VERIFY(changed.count() > 0);
        QTRY_COMPARE(nameOf(QStringLiteral("new-id")), QStringLiteral("New"));
    }
    void serviceLossAndRestart() {
        PlasmaActivitiesLibrary *library = PlasmaActivitiesLibrary::instance();
        stopServer();
        QTRY_VERIFY(!library->isReady());
        QVERIFY(library->currentActivity().isEmpty());
        QVERIFY(library->activities().isEmpty());

        // Nothing restarts the service after it stopped
        QTest::qWait(200);
        QVERIFY(!isRegistered());

        startServer(QStringLiteral("6.7.4"));
        QTRY_VERIFY(library->isReady());
        QTRY_COMPARE(library->currentActivity(), QStringLiteral("work-id"));
        QCOMPARE(library->activities().size(), 2);
    }
    void cleanupTestCase() {
        _server.kill();
        _server.waitForFinished();
    }
};

int main(int argc, char **argv) {
    if (argc > 2 && QByteArray(argv[1]) == "--mock-service") {
        QCoreApplication app(argc, argv);
        qDBusRegisterMetaType<MockActivityInfo>();
        qDBusRegisterMetaType<QList<MockActivityInfo>>();
        MockApplication application;
        application.version = QString::fromLatin1(argv[2]);
        MockActivities activities;
        auto bus = QDBusConnection::sessionBus();
        const auto options = QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals;
        if (!bus.registerObject(QStringLiteral("/ActivityManager"), &application, options) ||
            !bus.registerObject(activitiesPath, &activities, options) ||
            !bus.registerService(service)) {
            return 1;
        }
        return app.exec();
    }
    QCoreApplication app(argc, argv);
    TestPlasmaActivities tests;
    return QTest::qExec(&tests, argc, argv);
}

#include "test_plasmaactivitieslibrary.moc"
