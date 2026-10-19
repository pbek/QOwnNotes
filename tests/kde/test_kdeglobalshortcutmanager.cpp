#include <services/kdeglobalshortcutmanager.h>

#include <KGlobalAccel>
#include <QApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusObjectPath>
#include <QDBusReply>
#include <QProcess>
#include <QSignalSpy>
#include <QtTest>

namespace {
const QString service = QStringLiteral("org.kde.kglobalaccel");
const QString component = QStringLiteral("QOwnNotes-kde-shortcut-tests");
const QString actionId = QStringLiteral("actionShow_Hide_application");
}    // namespace

class MockComponent : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kglobalaccel.Component")
   signals:
    void globalShortcutPressed(const QString &component, const QString &action,
                               qlonglong timestamp);
};

class MockAccel : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.KGlobalAccel")
   public:
    MockComponent shortcutComponent;

   public slots:
    void doRegister(const QStringList &) {}
    QDBusObjectPath getComponent(const QString &) { return QDBusObjectPath("/component/mock"); }
    QList<QKeySequence> setShortcutKeys(const QStringList &id, const QList<QKeySequence> &keys,
                                        uint flags) {
        if (flags & 8) {
            return keys;
        }
        const QString key = id.at(0) + QLatin1Char('|') + id.at(1);
        _flags.insert(key, flags);
        if ((flags & 4) || !_saved.contains(key)) {
            // Simulate a conflict with an unrelated desktop shortcut without stealing it.
            _saved.insert(key,
                          keys.contains(QKeySequence("Alt+F1")) ? QList<QKeySequence>() : keys);
        }
        _active.insert(key, true);
        return _saved.value(key);
    }
    QList<QKeySequence> shortcutKeys(const QStringList &id) {
        return _saved.value(id.at(0) + QLatin1Char('|') + id.at(1));
    }
    void setInactive(const QStringList &id) {
        _active.insert(id.at(0) + QLatin1Char('|') + id.at(1), false);
    }
    bool unregister(const QString &owner, const QString &action) {
        const QString key = owner + QLatin1Char('|') + action;
        _active.remove(key);
        return _saved.remove(key) > 0;
    }
    void seed(const QString &owner, const QString &action, const QList<QKeySequence> &keys) {
        _saved.insert(owner + QLatin1Char('|') + action, keys);
    }
    void change(const QString &owner, const QString &action, const QList<QKeySequence> &keys) {
        seed(owner, action, keys);
        emit yourShortcutsChanged({owner, action, owner, action}, keys);
    }
    void activate(const QString &owner, const QString &action) {
        emit shortcutComponent.globalShortcutPressed(owner, action, 0);
    }
    uint lastFlags(const QString &owner, const QString &action) {
        return _flags.value(owner + QLatin1Char('|') + action);
    }
    bool isActive(const QString &owner, const QString &action) {
        return _active.value(owner + QLatin1Char('|') + action);
    }

   signals:
    void yourShortcutsChanged(const QStringList &id, const QList<QKeySequence> &keys);

   private:
    QMap<QString, QList<QKeySequence>> _saved;
    QMap<QString, uint> _flags;
    QMap<QString, bool> _active;
};

class TestKdeShortcuts : public QObject {
    Q_OBJECT
   private:
    QProcess _server;

    QDBusMessage call(const QString &method, const QList<QVariant> &args = {}) {
        QDBusInterface iface(service, "/kglobalaccel", "org.kde.KGlobalAccel",
                             QDBusConnection::sessionBus());
        return iface.callWithArgumentList(QDBus::Block, method, args);
    }
    void startServer() {
        _server.start(QCoreApplication::applicationFilePath(), {"--mock-service"});
        QVERIFY(_server.waitForStarted());
        QTRY_VERIFY(KdeGlobalShortcutManager::isAvailable());
    }
    QMap<QString, KdeGlobalShortcutManager::ShortcutInfo> configured(const QString &key) {
        KdeGlobalShortcutManager::ShortcutInfo info;
        info.keySequence = QKeySequence(key);
        info.description = QStringLiteral("Show/Hide application");
        return {{actionId, info}};
    }
    QList<QKeySequence> saved() {
        const QDBusReply<QList<QKeySequence>> reply =
            call("shortcutKeys", {QStringList{component, actionId, component, actionId}});
        return reply.value();
    }

   private slots:
    void initTestCase() {
        QVERIFY2(qEnvironmentVariableIntValue("QON_KDE_TEST_BUS") == 1,
                 "Run tests on an isolated bus with dbus-run-session (see CMakeLists.txt)");
        QVERIFY(!KdeGlobalShortcutManager::isAvailable());
        KGlobalAccel::self();    // Register the D-Bus key sequence types without activating a
                                 // service.
    }
    void absentServiceAndLateArrival() {
        KdeGlobalShortcutManager manager;
        QSignalSpy changes(&manager, &KdeGlobalShortcutManager::serviceChanged);
        QVERIFY(!manager.configure(configured("Ctrl+Alt+F9")));
        QVERIFY(!KdeGlobalShortcutManager::isAvailable());
        startServer();
        QTRY_VERIFY(changes.count() > 0);
        QVERIFY(manager.configure(configured("Ctrl+Alt+F9")));
    }
    void restoreEditSynchronizeAndRemove() {
        const QList<QKeySequence> initial{QKeySequence("Ctrl+Alt+F10")};
        QVERIFY(call("seed", {component, actionId, QVariant::fromValue(initial)}).type() !=
                QDBusMessage::ErrorMessage);
        KdeGlobalShortcutManager manager;
        QSignalSpy changed(&manager, &KdeGlobalShortcutManager::shortcutChanged);
        QSignalSpy activated(&manager, &KdeGlobalShortcutManager::shortcutActivated);
        QVERIFY(manager.configure(configured("Ctrl+Alt+F9")));
        QCOMPARE(saved(), initial);
        QCOMPARE(changed.last().at(1).value<QKeySequence>(), initial.first());

        QVERIFY(manager.configure(configured("Ctrl+Alt+F11")));
        QCOMPARE(saved(), QList<QKeySequence>{QKeySequence("Ctrl+Alt+F11")});
        QVERIFY(QDBusReply<uint>(call("lastFlags", {component, actionId})).value() & 4);

        const QList<QKeySequence> external{QKeySequence("Ctrl+Alt+F12"), QKeySequence("Meta+F12")};
        call("change", {component, actionId, QVariant::fromValue(external)});
        QTRY_COMPARE(changed.last().at(1).value<QKeySequence>(), external.first());
        QVERIFY(manager.configure(configured("Ctrl+Alt+F12")));
        QCOMPARE(saved(), external);
        QVERIFY(!(QDBusReply<uint>(call("lastFlags", {component, actionId})).value() & 4));

        call("activate", {component, actionId});
        QTRY_COMPARE(activated.count(), 1);
        QCOMPARE(activated.first().first().toString(), actionId);

        QVERIFY(manager.configure(configured("")));
        QVERIFY(saved().isEmpty());
        QVERIFY(manager.configure({}));
        QTRY_VERIFY(!QDBusReply<bool>(call("isActive", {component, actionId})).value());
    }
    void shutdownKeepsSavedBinding() {
        // Removed actions keep Plasma's saved state, so start without a saved binding
        call("unregister", {component, actionId});
        {
            KdeGlobalShortcutManager manager;
            QVERIFY(manager.configure(configured("Ctrl+Alt+F9")));
        }
        QTRY_VERIFY(!QDBusReply<bool>(call("isActive", {component, actionId})).value());
        QCOMPARE(saved(), QList<QKeySequence>{QKeySequence("Ctrl+Alt+F9")});
        KdeGlobalShortcutManager replacement;
        QVERIFY(replacement.configure(configured("Ctrl+Alt+F10")));
        QCOMPARE(saved(), QList<QKeySequence>{QKeySequence("Ctrl+Alt+F9")});
        QVERIFY(replacement.configure(configured("Alt+F1")));
        QVERIFY(saved().isEmpty());
        QVERIFY(replacement.configure({}));
    }
    void unassignedAndLateActions() {
        const QString unassigned = QStringLiteral("actionNew_note");
        const QString late = QStringLiteral("customAction_late");
        const QList<QKeySequence> lateSaved{QKeySequence("Ctrl+Alt+F8")};
        call("seed", {component, late, QVariant::fromValue(lateSaved)});

        KdeGlobalShortcutManager manager;
        QSignalSpy changed(&manager, &KdeGlobalShortcutManager::shortcutChanged);
        auto shortcuts = configured("Ctrl+Alt+F9");
        KdeGlobalShortcutManager::ShortcutInfo empty;
        empty.description = QStringLiteral("New note");
        shortcuts.insert(unassigned, empty);
        QVERIFY(manager.configure(shortcuts));

        // Unassigned actions are offered to Plasma without writing empty settings
        QTRY_VERIFY(QDBusReply<bool>(call("isActive", {component, unassigned})).value());
        for (const auto &arguments : changed) {
            QVERIFY(arguments.at(0).toString() != unassigned);
        }

        // An action appearing later (e.g. from a script) keeps its saved Plasma binding
        shortcuts.insert(late, empty);
        QVERIFY(manager.configure(shortcuts));
        QDBusMessage reply = call("shortcutKeys", {QStringList{component, late, component, late}});
        QCOMPARE(QDBusReply<QList<QKeySequence>>(reply).value(), lateSaved);
        QCOMPARE(changed.last().at(0).toString(), late);

        // A disappearing action is only deactivated and keeps its binding
        shortcuts.remove(late);
        QVERIFY(manager.configure(shortcuts));
        QTRY_VERIFY(!QDBusReply<bool>(call("isActive", {component, late})).value());
        reply = call("shortcutKeys", {QStringList{component, late, component, late}});
        QCOMPARE(QDBusReply<QList<QKeySequence>>(reply).value(), lateSaved);
        QVERIFY(manager.configure({}));
    }
    void serviceLossAndRestart() {
        KdeGlobalShortcutManager manager;
        QVERIFY(manager.configure(configured("Ctrl+Alt+F9")));
        QSignalSpy changes(&manager, &KdeGlobalShortcutManager::serviceChanged);
        _server.kill();
        QVERIFY(_server.waitForFinished());
        QTRY_VERIFY(!KdeGlobalShortcutManager::isAvailable());
        QTRY_VERIFY(changes.count() > 0);
        QVERIFY(!manager.configure(configured("Ctrl+Alt+F9")));
        startServer();
        QVERIFY(manager.configure(configured("Ctrl+Alt+F9")));
        QCOMPARE(saved(), QList<QKeySequence>{QKeySequence("Ctrl+Alt+F9")});
    }
    void cleanupTestCase() {
        _server.kill();
        _server.waitForFinished();
    }
};

int main(int argc, char **argv) {
    if (argc > 1 && QByteArray(argv[1]) == "--mock-service") {
        QCoreApplication app(argc, argv);
        KGlobalAccel::self();
        MockAccel mock;
        auto bus = QDBusConnection::sessionBus();
        if (!bus.registerObject(
                "/kglobalaccel", &mock,
                QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals) ||
            !bus.registerObject("/component/mock", &mock.shortcutComponent,
                                QDBusConnection::ExportAllSignals) ||
            !bus.registerService(service)) {
            return 1;
        }
        return app.exec();
    }
    QApplication app(argc, argv);
    app.setApplicationName(component);
    TestKdeShortcuts tests;
    return QTest::qExec(&tests, argc, argv);
}

#include "test_kdeglobalshortcutmanager.moc"
