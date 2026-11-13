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

#pragma once

#include <QString>
#include <QStringList>
#include <QVector>

namespace Utils {
namespace NoteFolderDetection {

/**
 * A local folder that plausibly already contains notes
 */
struct Candidate {
    // Absolute, canonical path of the folder
    QString path;

    // Number of note files found, capped at the maximum note count
    int noteCount = 0;

    // True if counting stopped at the maximum note count
    bool noteCountCapped = false;

    // True if notes were found in a subfolder, like a Nextcloud Notes category
    bool hasNotesInSubFolders = false;

    // True if the folder contains a QOwnNotes note folder database "notes.sqlite"
    bool hasNoteFolderDatabase = false;

    // True if the folder is located in a Nextcloud / ownCloud sync folder
    bool isInCloudSyncFolder = false;
};

// Default maximum number of note files that are counted per candidate
constexpr int DefaultMaxNoteCount = 1000;

QStringList knownNotesFolderNames();
bool isKnownNotesFolderName(const QString &folderName);
QStringList defaultNoteFileExtensions();
QStringList defaultCloudSyncRoots();
QStringList defaultPlainRoots();

QVector<Candidate> findCandidates(const QStringList &cloudSyncRoots, const QStringList &plainRoots,
                                  const QStringList &noteFileExtensions,
                                  const QStringList &excludedPaths = QStringList(),
                                  int maxNoteCount = DefaultMaxNoteCount);

QVector<Candidate> findDefaultCandidates(const QStringList &noteFileExtensions,
                                         const QStringList &excludedPaths = QStringList());

}    // namespace NoteFolderDetection
}    // namespace Utils
