#include "test_utilsgit.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QTemporaryDir>
#include <QTest>

#include "entities/notefolder.h"
#include "services/settingsservice.h"
#include "utils/git.h"

namespace {
QStringList notePaths() {
    return {QStringLiteral("ordinary.md"),
            QStringLiteral("sub folder/notes with spaces.md"),
            QStringLiteral("sync-note\";echo INJECTED>injected.txt;\"z.md"),
            QStringLiteral("$(echo INJECTED>injected.txt).md"),
            QStringLiteral("`echo INJECTED>injected.txt`.md"),
            QStringLiteral("quotes'\"\\ & | < > ; $HOME %PATH%.md"),
            QStringLiteral("line one\nline two.md"),
            QStringLiteral("Grüße-日本語-%notePath%.md")};
}
}    // namespace

void TestUtilsGit::testPrepareLogProcess_data() {
    QTest::addColumn<QString>("command");
    QTest::addColumn<QString>("filePath");
    QTest::addColumn<QString>("program");
    QTest::addColumn<QStringList>("arguments");

    QTest::newRow("quoted-program")
        << QStringLiteral("\"/path with spaces/git viewer\" --all \"two words\" %notePath%")
        << QStringLiteral("note.md") << QStringLiteral("/path with spaces/git viewer")
        << QStringList{"--all", "two words", "note.md"};
    QTest::newRow("windows-path") << QStringLiteral(
                                         "\"C:\\Program Files\\Git\\gitk.exe\" \"%notePath%\"")
                                  << QStringLiteral("C:\\Notes\\a b.md")
                                  << QStringLiteral("C:\\Program Files\\Git\\gitk.exe")
                                  << QStringList{QStringLiteral("C:\\Notes\\a b.md")};
    QTest::newRow("single-quotes-and-empty-argument")
        << QStringLiteral("'git viewer' '' 'two words' '%notePath%'") << QStringLiteral("note.md")
        << QStringLiteral("git viewer") << QStringList{"", "two words", "note.md"};
    QTest::newRow("repeated-placeholder")
        << QStringLiteral("viewer %notePath% --compare \"%notePath%\"") << QStringLiteral("note.md")
        << QStringLiteral("viewer") << QStringList{"note.md", "--compare", "note.md"};
    QTest::newRow("option-like-filename")
        << QStringLiteral("gitk") << QStringLiteral("--output=unexpected.md")
        << QStringLiteral("gitk") << QStringList{"./--output=unexpected.md"};
    QTest::newRow("whitespace") << QStringLiteral(" \tgitk\t--all\n %notePath% ")
                                << QStringLiteral("note.md") << QStringLiteral("gitk")
                                << QStringList{"--all", "note.md"};

    const QStringList commands{QStringLiteral("gitk --"), QStringLiteral("gitk -- %notePath%"),
                               QStringLiteral("gitk -- \"%notePath%\"")};
    int index = 0;
    for (const QString &command : commands) {
        for (const QString &path : notePaths()) {
            QTest::newRow(qPrintable(QStringLiteral("literal-path-%1").arg(index++)))
                << command << path << QStringLiteral("gitk") << QStringList{"--", path};
        }
    }
}

void TestUtilsGit::testPrepareLogProcess() {
    QFETCH(QString, command);
    QFETCH(QString, filePath);
    QFETCH(QString, program);
    QFETCH(QStringList, arguments);

    QProcess process;
    QVERIFY(Utils::Git::prepareLogProcess(process, command, filePath));
    QCOMPARE(process.program(), program);
    QCOMPARE(process.arguments(), arguments);
}

void TestUtilsGit::testInvalidLogCommand_data() {
    QTest::addColumn<QString>("command");
    QTest::addColumn<QString>("filePath");

    QTest::newRow("empty") << QString() << QStringLiteral("note.md");
    QTest::newRow("whitespace") << QStringLiteral(" \t\n") << QStringLiteral("note.md");
    QTest::newRow("empty-program") << QStringLiteral("\"\" --all") << QStringLiteral("note.md");
    QTest::newRow("unclosed-double-quote")
        << QStringLiteral("\"git viewer") << QStringLiteral("note.md");
    QTest::newRow("unclosed-single-quote")
        << QStringLiteral("gitk '%notePath%") << QStringLiteral("note.md");
    QTest::newRow("placeholder-as-program")
        << QStringLiteral("%notePath% --all") << QStringLiteral("note.md");
    QTest::newRow("placeholder-in-program")
        << QStringLiteral("viewer%notePath%") << QStringLiteral("note.md");
    QTest::newRow("inline-shell-script")
        << QStringLiteral("bash -c 'git log %notePath%'") << QStringLiteral("note.md");
    QTest::newRow("embedded-placeholder")
        << QStringLiteral("viewer --file=%notePath%") << QStringLiteral("note.md");
    QTest::newRow("empty-path") << QStringLiteral("gitk") << QString();
    QTest::newRow("null-in-command")
        << (QStringLiteral("gitk") + QChar::Null + QStringLiteral("other"))
        << QStringLiteral("note.md");
    QTest::newRow("null-in-path") << QStringLiteral("gitk")
                                  << (QStringLiteral("note") + QChar::Null + QStringLiteral(".md"));
}

void TestUtilsGit::testInvalidLogCommand() {
    QFETCH(QString, command);
    QFETCH(QString, filePath);

    QProcess process;
    process.setProgram(QStringLiteral("unchanged"));
    process.setArguments({QStringLiteral("unchanged")});
    QVERIFY(!Utils::Git::prepareLogProcess(process, command, filePath));
    QCOMPARE(process.program(), QStringLiteral("unchanged"));
    QCOMPARE(process.arguments(), QStringList{QStringLiteral("unchanged")});
}

void TestUtilsGit::testShowLog_data() {
    QTest::addColumn<QString>("commandSuffix");
    QTest::addColumn<QString>("filePath");
    int index = 0;
    for (const QString &suffix : {QStringLiteral(" --"), QStringLiteral(" -- %notePath%"),
                                  QStringLiteral(" -- \"%notePath%\"")}) {
        for (const QString &path : notePaths()) {
            QTest::newRow(qPrintable(QStringLiteral("launch-%1").arg(index++))) << suffix << path;
        }
    }
}

void TestUtilsGit::testShowLog() {
    QFETCH(QString, commandSuffix);
    QFETCH(QString, filePath);

    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString executable = directory.filePath(QStringLiteral("git log recorder") +
#ifdef Q_OS_WIN
                                                  QStringLiteral(".exe")
#else
                                                  QString()
#endif
    );
    QVERIFY(QFile::copy(QString::fromUtf8(GIT_LOG_RECORDER_PATH), executable));
    QVERIFY(
        QFile::setPermissions(executable, QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner));
    const QString outputPath = directory.filePath(QStringLiteral("recorded arguments.json"));

    NoteFolder folder;
    folder.setName(QStringLiteral("Git log regression test"));
    folder.setLocalPath(directory.path());
    QVERIFY(folder.store());

    SettingsService settings;
    const QString commandKey = QStringLiteral("gitLogCommand");
    const QString folderKey = QStringLiteral("currentNoteFolderId");
    const bool hadCommand = settings.contains(commandKey);
    const bool hadFolder = settings.contains(folderKey);
    const QVariant previousCommand = settings.value(commandKey);
    const QVariant previousFolder = settings.value(folderKey);
    settings.setValue(commandKey,
                      QStringLiteral("\"%1\" \"%2\"").arg(executable, outputPath) + commandSuffix);
    settings.setValue(folderKey, folder.getId());

    Utils::Git::showLog(filePath);

    // Restore settings before any assertion can leave the test early.
    if (hadCommand) {
        settings.setValue(commandKey, previousCommand);
    } else {
        settings.remove(commandKey);
    }
    if (hadFolder) {
        settings.setValue(folderKey, previousFolder);
    } else {
        settings.remove(folderKey);
    }
    QVERIFY(folder.remove());

    QTRY_VERIFY(QFile::exists(outputPath));
    QFile output(outputPath);
    QVERIFY(output.open(QIODevice::ReadOnly));
    const QJsonObject result = QJsonDocument::fromJson(output.readAll()).object();
    QCOMPARE(result.value(QStringLiteral("arguments")).toArray(),
             QJsonArray::fromStringList({QStringLiteral("--"), filePath}));
    QCOMPARE(result.value(QStringLiteral("workingDirectory")).toString(),
             QDir(directory.path()).canonicalPath());
    QVERIFY(!QFile::exists(directory.filePath(QStringLiteral("injected.txt"))));
}
