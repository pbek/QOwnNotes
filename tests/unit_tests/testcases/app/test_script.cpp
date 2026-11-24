#include "test_script.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QLineEdit>
#include <QPushButton>
#include <QScopeGuard>
#include <QTemporaryDir>
#include <QTest>

#include "entities/script.h"
#include "services/scriptingservice.h"
#include "services/settingsservice.h"
#include "widgets/settings/scriptingsettingswidget.h"

void TestScript::testChangelogForVersionRange() {
    const QString changelog = QStringLiteral(
        "# Changelog\n\n"
        "## 2.1.0 - 2026-08-25\n\n- Newer change.\n\n"
        "## 2.0.0 - 2026-08-20\n\n- Target change.\n\n"
        "## 1.5.0 - 2026-08-10\n\n- Intermediate change.\n\n"
        "## 1.0.0 - 2026-08-01\n\n- Installed change.\n");

    const QString result = Script::changelogForVersionRange(changelog, QStringLiteral("1.0.0"),
                                                            QStringLiteral("2.0.0"));

    QVERIFY(result.contains(QStringLiteral("## 2.0.0")));
    QVERIFY(result.contains(QStringLiteral("## 1.5.0")));
    QVERIFY(!result.contains(QStringLiteral("## 2.1.0")));
    QVERIFY(!result.contains(QStringLiteral("## 1.0.0")));
}

void TestScript::testChangelogVersionOnlyHeadings() {
    const QString changelog = QStringLiteral(
        "# Changelog\r\n\r\n"
        "## 1.2.0\r\n\r\n- Latest change.\r\n\r\n"
        "## 1.1.0\r\n\r\n- Earlier change.\r\n");

    const QString result = Script::changelogForVersionRange(changelog, QStringLiteral("1.0.5"),
                                                            QStringLiteral("1.2.0"));

    QVERIFY(result.contains(QStringLiteral("## 1.2.0")));
    QVERIFY(result.contains(QStringLiteral("## 1.1.0")));
}

void TestScript::testChangelogForVersionRangeIgnoresNonVersionHeadings() {
    const QString changelog = QStringLiteral(
        "# Changelog\n\n"
        "## Unreleased\n\n- Future change.\n\n"
        "## 1.1.0 - 2026-08-25\n\n- Released change.\n");

    const QString result = Script::changelogForVersionRange(changelog, QStringLiteral("1.0.0"),
                                                            QStringLiteral("1.1.0"));

    QVERIFY(!result.contains(QStringLiteral("Future change")));
    QVERIFY(result.contains(QStringLiteral("Released change")));
}

void TestScript::testChangelogHtmlForVersionRange() {
    const QString changelog = QStringLiteral(
        "# Changelog\n\n"
        "## 2.0.0 - 2026-08-25\n\n- **Target** change.\n\n"
        "## 1.0.0 - 2026-08-01\n\n- Installed change.\n");

    const QString result = Script::changelogHtmlForVersionRange(changelog, QStringLiteral("1.0.0"),
                                                                QStringLiteral("2.0.0"));

    QVERIFY(result.contains(QStringLiteral("<h2>2.0.0 - 2026-08-25</h2>")));
    QVERIFY(result.contains(QStringLiteral("<strong>Target</strong> change.")));
    QVERIFY(!result.contains(QStringLiteral("Installed change")));
}

void TestScript::testRepositoryUrlsUseMainBranch() {
    Script script;
    script.setIdentifier(QStringLiteral("example-script"));

    QCOMPARE(script.remoteChangelogUrl(),
             QUrl(QStringLiteral("https://raw.githubusercontent.com/qownnotes/scripts/main/"
                                 "example-script/CHANGELOG.md")));
    QCOMPARE(script.repositoryChangelogUrl(),
             QUrl(QStringLiteral("https://github.com/qownnotes/scripts/blob/main/"
                                 "example-script/CHANGELOG.md")));
}

void TestScript::testSnapRepositoryPathUsesCurrentRevision() {
#ifndef Q_OS_LINUX
    QSKIP("Snap packages are only supported on Linux.");
#endif

    QTemporaryDir temporaryDir;
    QVERIFY(temporaryDir.isValid());

    const QByteArray oldXdgDataHome = qgetenv("XDG_DATA_HOME");
    const QByteArray oldSnapUserData = qgetenv("SNAP_USER_DATA");
    const QString snapUserData = temporaryDir.path() + QStringLiteral("/13781");
    QDir().mkpath(snapUserData);
    qputenv("XDG_DATA_HOME", (snapUserData + QStringLiteral("/.local/share")).toUtf8());
    qputenv("SNAP_USER_DATA", snapUserData.toUtf8());

    Script script;
    script.setIdentifier(QStringLiteral("example-script"));
    script.setInfoJson(QStringLiteral(R"({"script":"example.qml"})"));
    script.setScriptPath(temporaryDir.path() +
                         QStringLiteral("/13780/.local/share/PBE/QOwnNotesTests/scripts/"
                                        "example-script/example.qml"));
    const QString scriptPath = script.getScriptPath();

    if (oldXdgDataHome.isNull()) {
        qunsetenv("XDG_DATA_HOME");
    } else {
        qputenv("XDG_DATA_HOME", oldXdgDataHome);
    }
    if (oldSnapUserData.isNull()) {
        qunsetenv("SNAP_USER_DATA");
    } else {
        qputenv("SNAP_USER_DATA", oldSnapUserData);
    }

    QCOMPARE(scriptPath, temporaryDir.path() +
                             QStringLiteral("/current/.local/share/PBE/QOwnNotesTests/scripts/"
                                            "example-script/example.qml"));
}

void TestScript::testRepositoryPathsRejectUnsafeNames() {
    QTemporaryDir temporaryDir;
    QVERIFY(temporaryDir.isValid());
    const QByteArray oldXdgDataHome = qgetenv("XDG_DATA_HOME");
    qputenv("XDG_DATA_HOME", temporaryDir.filePath(QStringLiteral("data")).toUtf8());

    // Unsafe identifiers from the remote repository must never resolve to a
    // path that could be recursively removed or written to
    Script unsafeIdentifierScript;
    unsafeIdentifierScript.setIdentifier(QStringLiteral("../.."));
    unsafeIdentifierScript.setScriptPath(QStringLiteral("/stored/script.qml"));
    const QString unsafeIdentifierRepositoryPath =
        unsafeIdentifierScript.scriptRepositoryPath(true);
    const QString unsafeIdentifierScriptPath = unsafeIdentifierScript.getScriptPath();

    // Unsafe script names fall back to the file name of the stored script path
    Script unsafeScriptNameScript;
    unsafeScriptNameScript.setIdentifier(QStringLiteral("example-script"));
    unsafeScriptNameScript.setInfoJson(QStringLiteral(R"({"script":"../../evil.qml"})"));
    unsafeScriptNameScript.setScriptPath(QStringLiteral("/stored/example.qml"));
    const QString repositoryPath = unsafeScriptNameScript.scriptRepositoryPath();
    const QString unsafeScriptNamePath = unsafeScriptNameScript.getScriptPath();

    if (oldXdgDataHome.isNull()) {
        qunsetenv("XDG_DATA_HOME");
    } else {
        qputenv("XDG_DATA_HOME", oldXdgDataHome);
    }

    QVERIFY(unsafeIdentifierRepositoryPath.isEmpty());
    QCOMPARE(unsafeIdentifierScriptPath, QStringLiteral("/stored/script.qml"));
    QVERIFY(!repositoryPath.isEmpty());
    QCOMPARE(unsafeScriptNamePath, repositoryPath + QStringLiteral("/example.qml"));
}

void TestScript::testApplySettingsAndReloadEngine() {
    SettingsService appSettings;
    const QString trackingKey = QStringLiteral("appMetrics/disableTracking");
    const QVariant previousTrackingSetting = appSettings.value(trackingKey);
    appSettings.setValue(trackingKey, true);
    const auto restoreTracking = qScopeGuard([&]() {
        if (previousTrackingSetting.isValid()) {
            appSettings.setValue(trackingKey, previousTrackingSetting);
        } else {
            appSettings.remove(trackingKey);
        }
    });

    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString path = directory.filePath(QStringLiteral("url-title.qml"));
    const QDateTime initialTimestamp = QDateTime::currentDateTime().addSecs(-60);
    const auto writeScript = [&](const QByteArray &prefix, int revision) {
        const QByteArray source = QByteArray(R"(
import QtQml
QtObject {
    property string title
    property var settingsVariables: [
        { identifier: "title", name: "Title", type: "string", default: "initial" }
    ]
    function fetchUrlTitleHook(url) { return ")") +
                                  prefix + QByteArray(R"(" + title }
})");
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate) ||
            file.write(source) != source.size() || !file.flush()) {
            return false;
        }
        // Give each edit a distinct timestamp for QML's disk cache.
        return file.setFileTime(initialTimestamp.addSecs(revision),
                                QFileDevice::FileModificationTime);
    };
    QVERIFY(writeScript("", 0));

    Script script;
    script.setName(QStringLiteral("URL title test"));
    script.setScriptPath(path);
    script.setEnabled(true);
    QVERIFY(script.store());
    auto *service = ScriptingService::instance();
    const auto cleanup = qScopeGuard([&]() {
        script.remove();
        service->reloadEngine();
    });
    service->reloadEngine();

    const auto title = [service]() {
        return service->callFetchUrlTitleHook(QStringLiteral("https://example.com"));
    };
    QCOMPARE(title(), QStringLiteral("initial"));

    ScriptingSettingsWidget settings;
    settings.initialize();
    auto *editor = settings.findChild<QLineEdit *>(QStringLiteral("stringLineEdit"));
    QVERIFY(editor);
    QCOMPARE(editor->text(), QStringLiteral("initial"));
    QVERIFY(Script::fetch(script.getId()).getSettingsVariablesJsonObject().isEmpty());

    editor->setText(QStringLiteral("updated"));
    QCOMPARE(title(), QStringLiteral("initial"));
    settings.storeSettings();
    QCOMPARE(title(), QStringLiteral("updated"));

    QVERIFY(writeScript("First edit: ", 1));
    settings.storeSettings();
    QCOMPARE(title(), QStringLiteral("updated"));

    auto *reload = settings.findChild<QPushButton *>(QStringLiteral("scriptReloadEngineButton"));
    QVERIFY(reload);
    reload->click();
    QCOMPARE(title(), QStringLiteral("First edit: updated"));

    QVERIFY(writeScript("Second edit: ", 2));
    auto *reload2 = settings.findChild<QPushButton *>(QStringLiteral("scriptReloadEngineButton2"));
    QVERIFY(reload2);
    reload2->click();
    QCOMPARE(title(), QStringLiteral("Second edit: updated"));
}
