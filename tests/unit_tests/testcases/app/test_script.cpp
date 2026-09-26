#include "test_script.h"

#include <QDir>
#include <QTemporaryDir>
#include <QTest>

#include "entities/script.h"

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
