/*
 * Copyright (c) 2014-2026 Patrizio Bekerle -- <patrizio@bekerle.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 */

#include "notefolderdetection.h"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QHash>
#include <QRegularExpression>
#include <QSet>
#include <algorithm>

#include "utils/misc.h"

namespace {

// Maximum number of direct subfolders of a candidate that are searched for notes
constexpr int MaxSubFoldersToCount = 200;

QString normalizedFolderName(const QString &folderName) {
    // File names on macOS may be stored decomposed, so we compare composed and case-folded names
    return folderName.normalized(QString::NormalizationForm_C).toCaseFolded();
}

Qt::CaseSensitivity pathCaseSensitivity() {
#ifdef Q_OS_WIN
    return Qt::CaseInsensitive;
#else
    return Qt::CaseSensitive;
#endif
}

QString pathKey(const QString &path) {
    return pathCaseSensitivity() == Qt::CaseInsensitive ? path.toCaseFolded() : path;
}

/**
 * Returns the canonical path of an existing path, or the cleaned absolute path otherwise
 */
QString canonicalOrAbsolutePath(const QString &path) {
    const QFileInfo fileInfo(path);
    const QString canonicalPath = fileInfo.canonicalFilePath();

    return canonicalPath.isEmpty() ? QDir::cleanPath(fileInfo.absoluteFilePath()) : canonicalPath;
}

QStringList noteFileNameFilters(const QStringList &noteFileExtensions) {
    QStringList nameFilters;

    for (QString extension : noteFileExtensions) {
        extension = extension.trimmed();
        while (extension.startsWith(QLatin1Char('.'))) {
            extension.remove(0, 1);
        }

        if (!extension.isEmpty()) {
            nameFilters << QStringLiteral("*.") + extension;
        }
    }

    nameFilters.removeDuplicates();

    return nameFilters;
}

/**
 * Counts note files in a directory, stopping once "limit" files were found
 */
int countNoteFilesInDirectory(const QString &path, const QStringList &nameFilters, int limit) {
    int count = 0;

    // Name filters are matched case-insensitively, because QDir::CaseSensitive isn't set
    QDirIterator it(path, nameFilters, QDir::Files | QDir::NoDotAndDotDot);
    while (count < limit && it.hasNext()) {
        it.next();
        count++;
    }

    return count;
}

/**
 * Counts note files directly in a folder and in its direct, non-hidden subfolders
 *
 * Nextcloud Notes stores categories as subfolders, so one level of subfolders is searched.
 * Counting stops after "maxNoteCount" notes, "isCapped" is set if there are more notes.
 * "hasNotesInSubFolders" is set if notes were found in a subfolder.
 */
int countNotes(const QString &path, const QStringList &nameFilters, int maxNoteCount,
               bool *isCapped, bool *hasNotesInSubFolders) {
    *isCapped = false;
    *hasNotesInSubFolders = false;

    if (nameFilters.isEmpty() || maxNoteCount <= 0) {
        return 0;
    }

    // Count one more note than the maximum to find out if there are more notes
    const int limit = maxNoteCount + 1;
    int count = countNoteFilesInDirectory(path, nameFilters, limit);

    QDirIterator subFolderIterator(path, QDir::Dirs | QDir::NoDotAndDotDot);
    int subFolderCount = 0;
    while (count < limit && subFolderCount < MaxSubFoldersToCount && subFolderIterator.hasNext()) {
        subFolderIterator.next();
        subFolderCount++;
        const int subFolderNoteCount =
            countNoteFilesInDirectory(subFolderIterator.filePath(), nameFilters, limit - count);

        if (subFolderNoteCount > 0) {
            *hasNotesInSubFolders = true;
            count += subFolderNoteCount;
        }
    }

    if (count > maxNoteCount) {
        *isCapped = true;
        count = maxNoteCount;
    }

    return count;
}

QString homePath() {
    QString path = QDir::homePath() + QLatin1Char('/');

    // Remove the snap path for Snapcraft builds, like in Utils::Misc::defaultNotesPath()
    static const QRegularExpression re(QStringLiteral(R"(snap\/qownnotes\/\w\d+\/)"));
    path.remove(re);

    return QDir::cleanPath(path);
}

}    // namespace

/**
 * Returns the known names of the default folder of Nextcloud Notes in its translations
 *
 * Non-ASCII characters are escaped, so the names don't depend on the source file encoding
 */
QStringList Utils::NoteFolderDetection::knownNotesFolderNames() {
    return {
        QStringLiteral("Notes"),
        QStringLiteral("Notizen"),
        QStringLiteral("Notas"),
        QStringLiteral("Note"),
        QStringLiteral("Notities"),
        QStringLiteral("Noter"),
        QStringLiteral("Notater"),
        QStringLiteral("Notat"),
        QStringLiteral("Notatki"),
        QStringLiteral("Anteckningar"),
        QStringLiteral("Muistiinpanot"),
        QStringLiteral("Jegyzetek"),
        // Poznámky
        QStringLiteral("Pozn\u00e1mky"),
        QStringLiteral("Notlar"),
        // Notițe
        QStringLiteral("Noti\u021be"),
        // Bilješke
        QStringLiteral("Bilje\u0161ke"),
        // Beležka
        QStringLiteral("Bele\u017eka"),
        QStringLiteral("Catatan"),
        // Ghi chép
        QStringLiteral("Ghi ch\u00e9p"),
        // Märkmed
        QStringLiteral("M\u00e4rkmed"),
        // Piezīmes
        QStringLiteral("Piez\u012bmes"),
        // Užrašai
        QStringLiteral("U\u017era\u0161ai"),
        QStringLiteral("Oharrak"),
        QStringLiteral("Notoj"),
        // Nòtas
        QStringLiteral("N\u00f2tas"),
        // Nótaí
        QStringLiteral("N\u00f3ta\u00ed"),
        // Notennoù
        QStringLiteral("Notenno\u00f9"),
        // Nòtaichean
        QStringLiteral("N\u00f2taichean"),
        QStringLiteral("Nodiadau"),
        QStringLiteral("Minnispunktar"),
        // Shënime
        QStringLiteral("Sh\u00ebnime"),
        // Qeydlər
        QStringLiteral("Qeydl\u0259r"),
        QStringLiteral("Nota"),
        QStringLiteral("Mazingatio"),
        QStringLiteral("Tizmilin"),
        // Σημειώσεις
        QStringLiteral("\u03a3\u03b7\u03bc\u03b5\u03b9\u03ce\u03c3\u03b5\u03b9\u03c2"),
        // Заметки
        QStringLiteral("\u0417\u0430\u043c\u0435\u0442\u043a\u0438"),
        // Нотатки
        QStringLiteral("\u041d\u043e\u0442\u0430\u0442\u043a\u0438"),
        // Бележки
        QStringLiteral("\u0411\u0435\u043b\u0435\u0436\u043a\u0438"),
        // Белешке
        QStringLiteral("\u0411\u0435\u043b\u0435\u0448\u043a\u0435"),
        // Белешки
        QStringLiteral("\u0411\u0435\u043b\u0435\u0448\u043a\u0438"),
        // Нататкі
        QStringLiteral("\u041d\u0430\u0442\u0430\u0442\u043a\u0456"),
        // Тэмдэглэл
        QStringLiteral("\u0422\u044d\u043c\u0434\u044d\u0433\u043b\u044d\u043b"),
        // Նոթեր
        QStringLiteral("\u0546\u0578\u0569\u0565\u0580"),
        // הערות
        QStringLiteral("\u05d4\u05e2\u05e8\u05d5\u05ea"),
        // الملاحظات
        QStringLiteral("\u0627\u0644\u0645\u0644\u0627\u062d\u0638\u0627\u062a"),
        // ئىزاھاتلار
        QStringLiteral("\u0626\u0649\u0632\u0627\u06be\u0627\u062a\u0644\u0627\u0631"),
        // یادداشت‌ها (with a zero-width non-joiner)
        QStringLiteral("\u06cc\u0627\u062f\u062f\u0627\u0634\u062a\u200c\u0647\u0627"),
        // নোট
        QStringLiteral("\u09a8\u09cb\u099f"),
        // குறிப்புகள்
        QStringLiteral("\u0b95\u0bc1\u0bb1\u0bbf\u0baa\u0bcd\u0baa\u0bc1\u0b95\u0bb3\u0bcd"),
        // සටහන්
        QStringLiteral("\u0dc3\u0da7\u0dc4\u0db1\u0dca"),
        // โน้ต
        QStringLiteral("\u0e42\u0e19\u0e49\u0e15"),
        // ჩანაწერები
        QStringLiteral("\u10e9\u10d0\u10dc\u10d0\u10ec\u10d4\u10e0\u10d4\u10d1\u10d8"),
        // កំណត់​ចំណាំ (with a zero-width space)
        QStringLiteral("\u1780\u17c6\u178e\u178f\u17cb\u200b\u1785\u17c6\u178e\u17b6\u17c6"),
        // ノート
        QStringLiteral("\u30ce\u30fc\u30c8"),
        // 笔记
        QStringLiteral("\u7b14\u8bb0"),
        // 筆記
        QStringLiteral("\u7b46\u8a18"),
        // 메모
        QStringLiteral("\uba54\ubaa8"),
    };
}

/**
 * Checks case-insensitively if a folder name is a known name of the Nextcloud Notes folder
 */
bool Utils::NoteFolderDetection::isKnownNotesFolderName(const QString &folderName) {
    static const QSet<QString> names = [] {
        QSet<QString> set;
        const QStringList knownNames = knownNotesFolderNames();
        for (const QString &name : knownNames) {
            set.insert(normalizedFolderName(name));
        }
        return set;
    }();

    return names.contains(normalizedFolderName(folderName));
}

/**
 * Returns the file extensions that are commonly used for notes
 */
QStringList Utils::NoteFolderDetection::defaultNoteFileExtensions() {
    return {QStringLiteral("md"), QStringLiteral("txt"), QStringLiteral("markdown"),
            QStringLiteral("note"), QStringLiteral("org")};
}

/**
 * Returns the local Nextcloud / ownCloud sync folders whose subfolders are searched for notes
 *
 * Like Utils::Misc::defaultNotesPath(), the ownCloud folder gets a ".test" suffix in debug
 * builds. In portable mode no sync folders are searched.
 */
QStringList Utils::NoteFolderDetection::defaultCloudSyncRoots() {
    if (Utils::Misc::isInPortableMode()) {
        return {};
    }

    const QString home = homePath();
    QString ownCloudPath = home + QStringLiteral("/ownCloud");
#ifdef QT_DEBUG
    ownCloudPath.append(QStringLiteral(".test"));
#endif

    return {ownCloudPath, home + QStringLiteral("/Nextcloud")};
}

/**
 * Returns the folders whose subfolders are searched for notes, besides the sync folders
 *
 * In portable mode only the portable data path is searched.
 */
QStringList Utils::NoteFolderDetection::defaultPlainRoots() {
    if (Utils::Misc::isInPortableMode()) {
        return {Utils::Misc::portableDataPath()};
    }

    return {homePath()};
}

/**
 * Finds local folders that plausibly already contain notes
 *
 * Only the direct subfolders of the root folders are checked, so this is fast. A folder is a
 * candidate if it contains a QOwnNotes note folder database "notes.sqlite", or if its name is a
 * known name of the Nextcloud Notes folder and it contains at least one note file directly or
 * in a direct subfolder. Hidden folders and the excluded paths are ignored.
 *
 * Candidates with a "notes.sqlite" come first, then candidates with more notes.
 */
QVector<Utils::NoteFolderDetection::Candidate> Utils::NoteFolderDetection::findCandidates(
    const QStringList &cloudSyncRoots, const QStringList &plainRoots,
    const QStringList &noteFileExtensions, const QStringList &excludedPaths, int maxNoteCount) {
    const QStringList nameFilters = noteFileNameFilters(noteFileExtensions);

    QSet<QString> excludedPathKeys;
    for (const QString &excludedPath : excludedPaths) {
        if (!excludedPath.trimmed().isEmpty()) {
            excludedPathKeys.insert(pathKey(canonicalOrAbsolutePath(excludedPath)));
        }
    }

    QVector<Candidate> candidates;
    QHash<QString, int> candidateIndexByPathKey;

    auto searchRoot = [&](const QString &rootPath, bool isCloudSyncRoot) {
        if (rootPath.isEmpty() || !QFileInfo(rootPath).isDir()) {
            return;
        }

        // Hidden folders are skipped, because QDir::Hidden isn't set
        QDirIterator it(rootPath, QDir::Dirs | QDir::NoDotAndDotDot);
        while (it.hasNext()) {
            it.next();
            const QFileInfo folderInfo = it.fileInfo();
            const bool hasNoteFolderDatabase =
                QFileInfo::exists(folderInfo.filePath() + QStringLiteral("/notes.sqlite"));

            if (!hasNoteFolderDatabase && !isKnownNotesFolderName(folderInfo.fileName())) {
                continue;
            }

            const QString canonicalPath = folderInfo.canonicalFilePath();
            if (canonicalPath.isEmpty()) {
                continue;
            }

            const QString key = pathKey(canonicalPath);
            if (excludedPathKeys.contains(key)) {
                continue;
            }

            // The same folder may be reachable from several roots, e.g. through symbolic links
            const auto existingIndex = candidateIndexByPathKey.constFind(key);
            if (existingIndex != candidateIndexByPathKey.constEnd()) {
                candidates[existingIndex.value()].isInCloudSyncFolder |= isCloudSyncRoot;
                continue;
            }

            Candidate candidate;
            candidate.path = canonicalPath;
            candidate.hasNoteFolderDatabase = hasNoteFolderDatabase;
            candidate.isInCloudSyncFolder = isCloudSyncRoot;
            candidate.noteCount =
                countNotes(canonicalPath, nameFilters, maxNoteCount, &candidate.noteCountCapped,
                           &candidate.hasNotesInSubFolders);

            // A folder with a known name, but without notes is no candidate
            if (!hasNoteFolderDatabase && candidate.noteCount == 0) {
                continue;
            }

            candidateIndexByPathKey.insert(key, candidates.count());
            candidates.append(candidate);
        }
    };

    for (const QString &root : cloudSyncRoots) {
        searchRoot(root, true);
    }

    for (const QString &root : plainRoots) {
        searchRoot(root, false);
    }

    std::stable_sort(candidates.begin(), candidates.end(),
                     [](const Candidate &a, const Candidate &b) {
                         if (a.hasNoteFolderDatabase != b.hasNoteFolderDatabase) {
                             return a.hasNoteFolderDatabase;
                         }

                         if (a.noteCount != b.noteCount) {
                             return a.noteCount > b.noteCount;
                         }

                         if (a.isInCloudSyncFolder != b.isInCloudSyncFolder) {
                             return a.isInCloudSyncFolder;
                         }

                         return a.path.compare(b.path, Qt::CaseInsensitive) < 0;
                     });

    return candidates;
}

/**
 * Finds local folders that plausibly already contain notes in the default locations
 */
QVector<Utils::NoteFolderDetection::Candidate> Utils::NoteFolderDetection::findDefaultCandidates(
    const QStringList &noteFileExtensions, const QStringList &excludedPaths) {
    return findCandidates(defaultCloudSyncRoots(), defaultPlainRoots(), noteFileExtensions,
                          excludedPaths);
}
