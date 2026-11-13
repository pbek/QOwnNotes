#include "test_notefolderdetection.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>
#include <QTest>

#include "utils/notefolderdetection.h"

using namespace Utils::NoteFolderDetection;

namespace {
bool createFile(const QString &filePath) {
    if (!QDir().mkpath(QFileInfo(filePath).absolutePath())) {
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    return file.write("# Note\n") > 0;
}

void createNotes(const QString &folderPath, int count,
                 const QString &extension = QStringLiteral("md")) {
    for (int i = 0; i < count; i++) {
        QVERIFY(createFile(folderPath + QStringLiteral("/note %1.").arg(i) + extension));
    }
}

QString canonicalPath(const QString &path) { return QFileInfo(path).canonicalFilePath(); }

const Candidate *findCandidate(const QVector<Candidate> &candidates, const QString &path) {
    const QString canonical = canonicalPath(path);
    for (const Candidate &candidate : candidates) {
        if (candidate.path == canonical) {
            return &candidate;
        }
    }

    return nullptr;
}
}    // namespace

void TestNoteFolderDetection::testKnownNotesFolderNames_data() {
    QTest::addColumn<QString>("folderName");
    QTest::addColumn<bool>("isKnown");

    QTest::newRow("english") << QStringLiteral("Notes") << true;
    QTest::newRow("german") << QStringLiteral("Notizen") << true;
    QTest::newRow("lower-case") << QStringLiteral("notizen") << true;
    QTest::newRow("upper-case") << QStringLiteral("NOTES") << true;
    QTest::newRow("czech-composed") << QStringLiteral("Pozn\u00e1mky") << true;
    QTest::newRow("czech-decomposed")
        << QStringLiteral("Pozn\u00e1mky").normalized(QString::NormalizationForm_D) << true;
    QTest::newRow("greek-upper-case")
        << QStringLiteral("\u03a3\u0397\u039c\u0395\u0399\u038f\u03a3\u0395\u0399\u03a3") << true;
    QTest::newRow("russian") << QStringLiteral("\u0417\u0430\u043c\u0435\u0442\u043a\u0438")
                             << true;
    QTest::newRow("persian-with-zwnj")
        << QStringLiteral("\u06cc\u0627\u062f\u062f\u0627\u0634\u062a\u200c\u0647\u0627") << true;
    QTest::newRow("khmer-with-zwsp")
        << QStringLiteral("\u1780\u17c6\u178e\u178f\u17cb\u200b\u1785\u17c6\u178e\u17b6\u17c6")
        << true;
    QTest::newRow("japanese") << QStringLiteral("\u30ce\u30fc\u30c8") << true;
    QTest::newRow("korean") << QStringLiteral("\uba54\ubaa8") << true;
    QTest::newRow("suffix") << QStringLiteral("Notes2") << false;
    QTest::newRow("unrelated") << QStringLiteral("Documents") << false;
    QTest::newRow("empty") << QString() << false;
}

void TestNoteFolderDetection::testKnownNotesFolderNames() {
    QFETCH(QString, folderName);
    QFETCH(bool, isKnown);

    QCOMPARE(isKnownNotesFolderName(folderName), isKnown);
}

void TestNoteFolderDetection::testFindsCandidates() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    const QString cloudRoot = tempDir.filePath(QStringLiteral("Nextcloud"));
    const QString homeRoot = tempDir.filePath(QStringLiteral("home"));

    // Localized Nextcloud Notes folder with a category subfolder
    createNotes(cloudRoot + QStringLiteral("/Notizen"), 2);
    createNotes(cloudRoot + QStringLiteral("/Notizen/Category"), 1);

    // Known name, but without notes
    QVERIFY(QDir().mkpath(cloudRoot + QStringLiteral("/Notes")));
    QVERIFY(createFile(cloudRoot + QStringLiteral("/Notes/image.png")));

    // QOwnNotes note folder with an arbitrary name, but without notes
    QVERIFY(createFile(cloudRoot + QStringLiteral("/My Stuff/notes.sqlite")));

    // Unknown name with notes
    createNotes(cloudRoot + QStringLiteral("/Documents"), 3);

    // Hidden folder with a known name
    createNotes(cloudRoot + QStringLiteral("/.Notes"), 3);

    // Notes in the root folder itself don't make the root folder a candidate
    createNotes(cloudRoot, 2);

    // Folder in the home directory
    createNotes(homeRoot + QStringLiteral("/notes"), 1, QStringLiteral("txt"));

    // Known name in a deeper level is ignored
    createNotes(homeRoot + QStringLiteral("/Projects/Notes"), 4);

    const QVector<Candidate> candidates =
        findCandidates({cloudRoot}, {homeRoot}, defaultNoteFileExtensions());

    QCOMPARE(candidates.count(), 3);

    const Candidate *notizen = findCandidate(candidates, cloudRoot + QStringLiteral("/Notizen"));
    QVERIFY(notizen != nullptr);
    QCOMPARE(notizen->noteCount, 3);
    QVERIFY(!notizen->noteCountCapped);
    QVERIFY(!notizen->hasNoteFolderDatabase);
    QVERIFY(notizen->isInCloudSyncFolder);
    QVERIFY(notizen->hasNotesInSubFolders);

    const Candidate *myStuff = findCandidate(candidates, cloudRoot + QStringLiteral("/My Stuff"));
    QVERIFY(myStuff != nullptr);
    QCOMPARE(myStuff->noteCount, 0);
    QVERIFY(myStuff->hasNoteFolderDatabase);
    QVERIFY(myStuff->isInCloudSyncFolder);

    const Candidate *homeNotes = findCandidate(candidates, homeRoot + QStringLiteral("/notes"));
    QVERIFY(homeNotes != nullptr);
    QCOMPARE(homeNotes->noteCount, 1);
    QVERIFY(!homeNotes->hasNoteFolderDatabase);
    QVERIFY(!homeNotes->isInCloudSyncFolder);
    QVERIFY(!homeNotes->hasNotesInSubFolders);
}

void TestNoteFolderDetection::testSortsCandidates() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    const QString root = tempDir.path();

    createNotes(root + QStringLiteral("/Notas"), 1);
    createNotes(root + QStringLiteral("/Notes"), 5);
    createNotes(root + QStringLiteral("/QOwnNotes"), 2);
    QVERIFY(createFile(root + QStringLiteral("/QOwnNotes/notes.sqlite")));
    createNotes(root + QStringLiteral("/Notizen"), 3);

    const QVector<Candidate> candidates = findCandidates({}, {root}, defaultNoteFileExtensions());

    QCOMPARE(candidates.count(), 4);
    QCOMPARE(candidates.at(0).path, canonicalPath(root + QStringLiteral("/QOwnNotes")));
    QCOMPARE(candidates.at(1).path, canonicalPath(root + QStringLiteral("/Notes")));
    QCOMPARE(candidates.at(2).path, canonicalPath(root + QStringLiteral("/Notizen")));
    QCOMPARE(candidates.at(3).path, canonicalPath(root + QStringLiteral("/Notas")));
}

void TestNoteFolderDetection::testCapsNoteCount() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    const QString root = tempDir.path();

    createNotes(root + QStringLiteral("/Notes"), 3);
    createNotes(root + QStringLiteral("/Notes/Category"), 3);
    createNotes(root + QStringLiteral("/Notizen"), 4);

    const QVector<Candidate> candidates =
        findCandidates({}, {root}, defaultNoteFileExtensions(), {}, 4);

    const Candidate *notes = findCandidate(candidates, root + QStringLiteral("/Notes"));
    QVERIFY(notes != nullptr);
    QCOMPARE(notes->noteCount, 4);
    QVERIFY(notes->noteCountCapped);

    const Candidate *notizen = findCandidate(candidates, root + QStringLiteral("/Notizen"));
    QVERIFY(notizen != nullptr);
    QCOMPARE(notizen->noteCount, 4);
    QVERIFY(!notizen->noteCountCapped);
}

void TestNoteFolderDetection::testCountsOnlyDirectSubfolders() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    const QString root = tempDir.path();

    // Notes in deeper levels or in hidden subfolders are not counted
    createNotes(root + QStringLiteral("/Notes/Category/Deeper"), 2);
    createNotes(root + QStringLiteral("/Notes/.hidden"), 2);

    QVERIFY(findCandidates({}, {root}, defaultNoteFileExtensions()).isEmpty());

    createNotes(root + QStringLiteral("/Notes/Category"), 1);

    const QVector<Candidate> candidates = findCandidates({}, {root}, defaultNoteFileExtensions());
    QCOMPARE(candidates.count(), 1);
    QCOMPARE(candidates.at(0).noteCount, 1);
    QVERIFY(candidates.at(0).hasNotesInSubFolders);
}

void TestNoteFolderDetection::testUsesNoteFileExtensions() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    const QString root = tempDir.path();

    createNotes(root + QStringLiteral("/Notes"), 1, QStringLiteral("MD"));
    createNotes(root + QStringLiteral("/Notes"), 2, QStringLiteral("markdown"));
    createNotes(root + QStringLiteral("/Notes"), 4, QStringLiteral("org"));
    createNotes(root + QStringLiteral("/Notes"), 8, QStringLiteral("pdf"));

    QVector<Candidate> candidates = findCandidates({}, {root}, defaultNoteFileExtensions());
    QCOMPARE(candidates.count(), 1);
    QCOMPARE(candidates.at(0).noteCount, 7);

    // Extensions may be passed with a leading dot
    candidates = findCandidates({}, {root}, {QStringLiteral(".org"), QStringLiteral("org")});
    QCOMPARE(candidates.count(), 1);
    QCOMPARE(candidates.at(0).noteCount, 4);

    QVERIFY(findCandidates({}, {root}, {QStringLiteral("txt")}).isEmpty());
}

void TestNoteFolderDetection::testExcludesPaths() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    const QString root = tempDir.path();

    createNotes(root + QStringLiteral("/Notes"), 1);
    createNotes(root + QStringLiteral("/Notizen"), 1);

    // Excluded paths are compared after resolving them
    const QVector<Candidate> candidates = findCandidates(
        {}, {root}, defaultNoteFileExtensions(),
        {root + QStringLiteral("/Notizen/../Notes"), QString(), root + QStringLiteral("/Missing")});

    QCOMPARE(candidates.count(), 1);
    QCOMPARE(candidates.at(0).path, canonicalPath(root + QStringLiteral("/Notizen")));
}

void TestNoteFolderDetection::testDeduplicatesCandidates() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    const QString cloudRoot = tempDir.filePath(QStringLiteral("Nextcloud"));

    createNotes(cloudRoot + QStringLiteral("/Notizen"), 2);

    // Searching the same root twice must not list a folder twice
    QVector<Candidate> candidates =
        findCandidates({cloudRoot}, {cloudRoot}, defaultNoteFileExtensions());
    QCOMPARE(candidates.count(), 1);
    QVERIFY(candidates.at(0).isInCloudSyncFolder);

#ifndef Q_OS_WIN
    // A symbolic link in the home directory resolves to the folder in the sync folder
    const QString homeRoot = tempDir.filePath(QStringLiteral("home"));
    QVERIFY(QDir().mkpath(homeRoot));
    QVERIFY(
        QFile::link(cloudRoot + QStringLiteral("/Notizen"), homeRoot + QStringLiteral("/Notizen")));

    candidates = findCandidates({cloudRoot}, {homeRoot}, defaultNoteFileExtensions());
    QCOMPARE(candidates.count(), 1);
    QCOMPARE(candidates.at(0).path, canonicalPath(cloudRoot + QStringLiteral("/Notizen")));
    QVERIFY(candidates.at(0).isInCloudSyncFolder);
#endif
}

void TestNoteFolderDetection::testIgnoresMissingRoots() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    QVERIFY(findCandidates({tempDir.filePath(QStringLiteral("ownCloud"))},
                           {tempDir.filePath(QStringLiteral("home")), QString()},
                           defaultNoteFileExtensions())
                .isEmpty());
}
