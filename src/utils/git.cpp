/*
 * Copyright (c) 2014-2017 Patrizio Bekerle -- http://www.bekerle.com
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

#include <QDebug>
#include <QProcess>
#include <entities/notefolder.h>
#include <QtCore/QSettings>
#include "git.h"



/**
 * Commits changes from the current note folder to git
 */
void Utils::Git::commitCurrentNoteFolder() {
    // check if git is enabled for the current note folder
    if (!NoteFolder::currentNoteFolder().isUseGit()) {
        return;
    }

    QProcess *process = new QProcess();
    process->setWorkingDirectory(NoteFolder::currentLocalPath());

    if (!executeGitCommand("init", process) ||
        !executeGitCommand("config commit.gpgsign false", process) ||
        !executeGitCommand("add -A", process) ||
        !executeGitCommand("commit -m \"QOwnNotes commit\"", process)) {
        return;
    }
}

/**
 * Executes a command
 *
 * @param command
 * @param process
 * @return
 */
bool Utils::Git::executeCommand(QString command, QProcess *process) {
    if (process == Q_NULLPTR) {
        process = new QProcess();
    }

    process->start(command);

    if (!process->waitForFinished()) {
        qWarning() << "Command '" + command + "' failed";
        return false;
    }

//    QByteArray result = process->readAll();

//    if (!result.isEmpty()) {
//    qDebug() << "Result message by '" + command + "': " + result;
//    }

    QByteArray errorMessage = process->readAllStandardError();

    if (!errorMessage.isEmpty()) {
        qWarning() << "Error message by '" + command + "': " + errorMessage;
    }

    return true;
}

/**
 * Executes a git command
 *
 * @param arguments
 * @param process
 * @return
 */
bool Utils::Git::executeGitCommand(QString arguments, QProcess *process) {
    return executeCommand("\""+ gitCommand() + "\" " + arguments, process);
}

/**
 * Returns the git command
 *
 * @return
 */
QString Utils::Git::gitCommand() {
    QSettings settings;
    QString path = settings.value("gitExecutablePath").toString();

    if (path.isEmpty()) {
#ifdef Q_OS_WIN
        path = "git.exe";
#else
        path = "git";
#endif
    }

    return path;
}
