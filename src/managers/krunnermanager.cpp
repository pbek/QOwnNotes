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

#include "krunnermanager.h"

#include <entities/note.h>
#include <entities/notefolder.h>
#include <services/windowactivationservice.h>
#include <utils/misc.h>

#include <QDBusConnection>
#include <QDebug>
#include <QStandardPaths>
#include <QUrl>
#include <algorithm>

#include "mainwindow.h"

const QString KRunnerManager::pluginFileName =
    QStringLiteral("krunner/dbusplugins/org.qownnotes.QOwnNotes.desktop");

namespace {
// Number of found notes that are checked for the best results, so short queries that
// match many notes stay fast
const int maxScannedNotes = 500;

// KRunner::QueryMatch::CategoryRelevance
const int moderateCategoryRelevance = 50;
const int lowCategoryRelevance = 30;
}    // namespace

KRunnerManager::KRunnerManager(MainWindow *mainWindow, QObject *parent)
    : QObject(parent), _mainWindow(mainWindow), _service(nullptr) {
    // Without the plugin file KRunner doesn't use the service
    if (!isPluginInstalled()) {
        return;
    }

    _service = new KRunnerService(&KRunnerManager::searchNotes, this);
    connect(_service, &KRunnerService::runRequested, this,
            [this](const QString &matchId, const QString &actionId, const QString &token) {
                Q_UNUSED(actionId)
                openNote(matchId, token);
            });

    _service->registerOnBus(QDBusConnection::sessionBus());
}

bool KRunnerManager::isPluginInstalled() {
    return !QStandardPaths::locate(QStandardPaths::GenericDataLocation, pluginFileName).isEmpty();
}

/**
 * Searches notes of the current note folder with the search of the note list
 *
 * Notes whose name matches are ranked higher. The text of encrypted notes is never
 * searched, and no note text is returned.
 */
KRunnerMatches KRunnerManager::searchNotes(const QString &query, int limit) {
    const NoteFolder noteFolder = NoteFolder::currentNoteFolder();
    if (!noteFolder.isFetched() || limit <= 0) {
        return {};
    }

    const bool ignoreAccents = Utils::Misc::isSearchIgnoreAccentsEnabled();
    const QVector<int> noteIds =
        Note::searchInNotes(query, true, -1, QStringLiteral("memory"), ignoreAccents);
    const QVector<NoteSearchTerm> searchTerms = Note::buildSearchTermList(query, ignoreAccents);

    struct Result {
        Note note;
        qreal relevance;
        bool nameMatch;
    };
    QVector<Result> results;

    for (int i = 0; i < noteIds.size() && i < maxScannedNotes; ++i) {
        const Note note = Note::fetch(noteIds.at(i));
        if (!note.isFetched()) {
            continue;
        }

        const QString name = note.getName();
        const bool nameMatch = std::all_of(searchTerms.cbegin(), searchTerms.cend(),
                                           [&name](const NoteSearchTerm &term) {
                                               return Note::textMatchesSearchTerm(name, term);
                                           });

        qreal relevance = 0.5;
        if (name.compare(query, Qt::CaseInsensitive) == 0) {
            relevance = 1.0;
        } else if (nameMatch && name.startsWith(query, Qt::CaseInsensitive)) {
            relevance = 0.9;
        } else if (nameMatch) {
            relevance = 0.8;
        }

        results.append({note, relevance, nameMatch});
    }

    std::stable_sort(results.begin(), results.end(), [](const Result &a, const Result &b) {
        if (a.relevance != b.relevance) {
            return a.relevance > b.relevance;
        }
        return a.note.getName().compare(b.note.getName(), Qt::CaseInsensitive) < 0;
    });

    KRunnerMatches matches;
    const QString noteFolderName = noteFolder.getName();

    for (int i = 0; i < results.size() && i < limit; ++i) {
        const Note &note = results.at(i).note;
        const QString subFolderPath = note.relativeNoteSubFolderPath();

        KRunnerMatch match;
        // Stays the same while the note isn't moved or renamed
        match.id = QString::number(noteFolder.getId()) + QLatin1Char('/') +
                   note.relativeNoteFilePath(QStringLiteral("/"));
        match.text = note.getName();
        match.iconName = QStringLiteral("text-markdown");
        match.categoryRelevance =
            results.at(i).nameMatch ? moderateCategoryRelevance : lowCategoryRelevance;
        match.relevance = results.at(i).relevance;
        match.properties.insert(QStringLiteral("subtext"),
                                subFolderPath.isEmpty()
                                    ? noteFolderName
                                    : noteFolderName + QStringLiteral(" / ") + subFolderPath);
        // There are no additional actions
        match.properties.insert(QStringLiteral("actions"), QStringList());
        match.properties.insert(
            QStringLiteral("urls"),
            QStringList{QUrl::fromLocalFile(note.fullNoteFilePath()).toString()});
        matches.append(match);
    }

    return matches;
}

void KRunnerManager::openNote(const QString &matchId, const QString &xdgActivationToken) {
    const int separatorIndex = matchId.indexOf(QLatin1Char('/'));
    if (separatorIndex < 1) {
        return;
    }

    // The note folder may have been switched since the search
    if (matchId.left(separatorIndex).toInt() != NoteFolder::currentNoteFolderId()) {
        qWarning() << "KRunner: the note is not in the current note folder:" << matchId;
        return;
    }

    Note note = Note::fetchByRelativeFilePath(matchId.mid(separatorIndex + 1));
    if (!note.isFetched()) {
        qWarning() << "KRunner: the note was not found:" << matchId;
        return;
    }

    WindowActivationService::setXdgActivationToken(xdgActivationToken);
    _mainWindow->showWindow();
    WindowActivationService::clearXdgActivationToken(xdgActivationToken);
    _mainWindow->setCurrentNote(std::move(note));
}
