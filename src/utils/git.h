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

#include <QProcess>
#include <QString>

#ifdef USE_LIBGIT2
#include <QJSValue>

#include "entities/note.h"
#endif

/*  Git functions that can be useful */

namespace Utils {
namespace Git {
void commitCurrentNoteFolder();
bool executeCommand(const QString& command, const QStringList& arguments,
                    QProcess* process = nullptr, bool withErrorDialog = false);
bool executeGitCommand(const QString& gitExe, const QStringList& arguments,
                       QProcess* process = nullptr, bool withErrorDialog = true);
QString gitCommand();
void showLog(const QString& filePath);
bool hasLogCommand();
bool isCurrentNoteFolderUseGit();
#ifdef USE_LIBGIT2
bool initRepository(const QString& path);
bool commitAll(const QString& path, const QString& message);
QJSValue getNoteVersions(QJSEngine& engine, const Note& note, int limit = 100);
#endif
}    // namespace Git
}    // namespace Utils
