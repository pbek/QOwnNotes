#include <services/windowactivationservice.h>

#include <QApplication>
#include <QWidget>
#include <QtTest>

class TestWindowActivation : public QObject {
    Q_OBJECT

   private slots:
    void sendsActivationInformation() {
        qputenv("XDG_ACTIVATION_TOKEN", "wayland-token");
        qputenv("DESKTOP_STARTUP_ID", "x11-id_TIME123");
        WindowActivationService::captureStartupEnvironment();

        const QByteArray message =
            WindowActivationService::secondaryInstanceMessage(QStringLiteral("actionNew_note"));
        const WindowActivationService::Request request =
            WindowActivationService::parseSecondaryInstanceMessage(message);
        QCOMPARE(request.xdgActivationToken, QStringLiteral("wayland-token"));
        QCOMPARE(request.startupId, QByteArray("x11-id_TIME123"));
        QCOMPARE(request.startupAction, QStringLiteral("actionNew_note"));
    }

    void sendsMessageWithoutInformation() {
        qunsetenv("XDG_ACTIVATION_TOKEN");
        qunsetenv("DESKTOP_STARTUP_ID");
        WindowActivationService::captureStartupEnvironment();

        // The primary instance needs a message to know that no activation information follows
        const QByteArray message = WindowActivationService::secondaryInstanceMessage(QString());
        QVERIFY(!message.isEmpty());
        const WindowActivationService::Request request =
            WindowActivationService::parseSecondaryInstanceMessage(message);
        QVERIFY(request.xdgActivationToken.isEmpty());
        QVERIFY(request.startupId.isEmpty());
        QVERIFY(request.startupAction.isEmpty());
    }

    void keepsActionMessageOfOlderVersions() {
        WindowActivationService::captureStartupEnvironment();
        QCOMPARE(WindowActivationService::secondaryInstanceMessage(QStringLiteral("actionQuit")),
                 QByteArray("startupAction:actionQuit"));
        QCOMPARE(WindowActivationService::parseSecondaryInstanceMessage("startupAction:actionQuit")
                     .startupAction,
                 QStringLiteral("actionQuit"));
    }

    void ignoresInjectedLines() {
        WindowActivationService::captureStartupEnvironment();
        const WindowActivationService::Request request =
            WindowActivationService::parseSecondaryInstanceMessage(
                WindowActivationService::secondaryInstanceMessage(
                    QStringLiteral("actionQuit\nxdgActivationToken:injected")));
        QCOMPARE(request.startupAction, QStringLiteral("actionQuit"));
        QVERIFY(request.xdgActivationToken.isEmpty());
    }

    void clearsOnlyUnusedToken() {
        qputenv("XDG_ACTIVATION_TOKEN", "other-token");
        WindowActivationService::clearXdgActivationToken(QStringLiteral("token"));
        QCOMPARE(qgetenv("XDG_ACTIVATION_TOKEN"), QByteArray("other-token"));
        WindowActivationService::clearXdgActivationToken(QStringLiteral("other-token"));
        QVERIFY(qgetenv("XDG_ACTIVATION_TOKEN").isEmpty());
    }

    void tokensAreOnlyUsedOnWayland() {
        // The tests run with the minimal platform
        WindowActivationService::setXdgActivationToken(QStringLiteral("token"));
        QVERIFY(qgetenv("XDG_ACTIVATION_TOKEN").isEmpty());
    }

    void activatesMinimizedWindow() {
        QWidget window;
        window.showMinimized();
        WindowActivationService::activateWindow(&window, QStringLiteral("token"),
                                                QByteArray("x11-id"));
        QVERIFY(window.isVisible());
        QVERIFY(!window.isMinimized());
        QVERIFY(qgetenv("XDG_ACTIVATION_TOKEN").isEmpty());
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    TestWindowActivation tests;
    return QTest::qExec(&tests, argc, argv);
}

#include "test_windowactivationservice.moc"
