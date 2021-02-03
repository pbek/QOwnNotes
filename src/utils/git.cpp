/*
 * Copyright (c) 2014-2021 Patrizio Bekerle -- <patrizio@bekerle.com>
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

#include "git.h"

#include <entities/notefolder.h>

#include <QDebug>
#include <QProcess>
#include <QtCore/QSettings>

#include "gui.h"
#include "misc.h"

/**
 * Checks if the current note folder uses git
 *
 * @return
 */
bool Utils::Git::isCurrentNoteFolderUseGit() {
    return NoteFolder::currentNoteFolder().isUseGit();
}

/**
 * Commits changes from the current note folder to git
 */
void Utils::Git::commitCurrentNoteFolder() {
    // check if git is enabled for the current note folder
    if (!isCurrentNoteFolderUseGit()) {
        return;
    }

    auto* process = new QProcess();
    process->setWorkingDirectory(NoteFolder::currentLocalPath());

    if (!executeGitCommand(QStringList{"init"}, process) ||
        !executeGitCommand(QStringList{"config", "commit.gpgsign", "false"}, process) ||
        !executeGitCommand(QStringList{"add", "-A"}, process) ||
        !executeGitCommand(QStringList{"commit", "-m", "QOwnNotes commit"}, process)) {
    }

    delete (process);
}

/**
 * Executes a command
 *
 * @param command
 * @param process
 * @return
 */
bool Utils::Git::executeCommand(const QString& command, const QStringList& arguments,
                                QProcess* process, bool withErrorDialog) {
    if (process == Q_NULLPTR) {
        process = new QProcess();
    }

    process->start(command, arguments);

    if (!process->waitForFinished()) {
        qWarning() << "Command '" + command + "' (" << arguments << ") failed";

        if (withErrorDialog) {
            Utils::Gui::warning(
                Q_NULLPTR, QObject::tr("Command failed!"),
                QObject::tr("The command <code>%1</code> with arguments <code>%2</code> failed!")
                    .arg(command, arguments.join(QStringLiteral(", "))),
                "command-failed");
        }

        return false;
    }

    //    QByteArray result = process->readAll();

    //    if (!result.isEmpty()) {
    //    qDebug() << "Result message by '" + command + "': " + result;
    //    }

    QByteArray errorMessage = process->readAllStandardError();

    if (!errorMessage.isEmpty()) {
        qWarning() << "Error message by '" + command + "' (" << arguments
                   << "): " + errorMessage;
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
bool Utils::Git::executeGitCommand(const QStringList& arguments, QProcess* process,
                                   bool withErrorDialog) {
    return executeCommand(gitCommand(), arguments, process, withErrorDialog);
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

/**
 * Checks if a git log command is set
 *
 * @return
 */
bool Utils::Git::hasLogCommand() {
    QSettings settings;
    return !settings.value("gitLogCommand").toString().isEmpty();
}

/**
 * Shows a git log
 *
 * @param filePath
 */
void Utils::Git::showLog(const QString& filePath) {
    QSettings settings;
    QString gitLogCommand = settings.value("gitLogCommand").toString();

    if (gitLogCommand.isEmpty()) {
        return;
    }

    // make sure the note path is added
    if (gitLogCommand.contains("%notePath%")) {
        gitLogCommand.replace("%notePath%", "\"" + filePath + "\"");
    } else {
        gitLogCommand += " \"" + filePath + "\"";
    }

    QStringList parameters = QStringList();

    // we need a shell to be able to use the executable with the parameters
    // together
#ifdef Q_OS_WIN
    QString command = "cmd.exe";
    parameters << "/c";
#else
    QString command = "bash";
    parameters << "-c";
#endif

    parameters << gitLogCommand;

    Utils::Misc::startDetachedProcess(command, parameters,
                                      NoteFolder::currentLocalPath());
}