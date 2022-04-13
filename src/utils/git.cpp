/*
 * Copyright (c) 2014-2022 Patrizio Bekerle -- <patrizio@bekerle.com>
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
#include <QStandardPaths>

#include "gui.h"
#include "misc.h"
#include "mainwindow.h"

/**
 * Checks if the current note folder uses git
 *
 * @return
 */
bool Utils::Git::isCurrentNoteFolderUseGit() {
    return NoteFolder::currentNoteFolder().isUseGit();
}

bool Utils::Git::isCurrentNoteFolderUseGitPush() {
    return NoteFolder::currentNoteFolder().isUseGitPush();
}


/**
 * Commits changes from the current note folder to git
 */
void Utils::Git::commitCurrentNoteFolder(MainWindow *mw) {
    // check if git is enabled for the current note folder
    if (!isCurrentNoteFolderUseGit()) {
        return;
    }

    const auto git = gitCommand();

    auto* process = new QProcess();
    process->setWorkingDirectory(NoteFolder::currentLocalPath());

    if (!executeGitCommand(git, QStringList{"init"}, process) ||
        !executeGitCommand(git, QStringList{"config", "commit.gpgsign", "false"}, process) ||
        !executeGitCommand(git, QStringList{"add", "-A"}, process) ||
        !executeGitCommand(git, QStringList{"commit", "-m", "QOwnNotes commit"}, process, false, true)) {
        if ( mw != nullptr ) {
            mw->showStatusBarMessage(QObject::tr("Git: Commit failed."), 10000);
        }
    }

    delete (process);
}

/**
 * Commits changes from the current note folder to git
 */
void Utils::Git::pushCurrentNoteFolder(MainWindow *mw) {
    // check if git is enabled for the current note folder
    if (!isCurrentNoteFolderUseGit()) {
        return;
    }
    if (!isCurrentNoteFolderUseGitPush()) {
        return;
    }


    const auto git = gitCommand();

    auto* process = new QProcess();
    process->setWorkingDirectory(NoteFolder::currentLocalPath());

    if (!executeGitCommand(git, QStringList{"pull", "--no-commit", "--no-ff"}, process)) {
        /* pull with merge failed */
        executeGitCommand(git, QStringList{"merge", "--abort"}, process);

        if ( mw != nullptr ) {
            mw->showStatusBarMessage(QObject::tr("Git: Pulling and merging failed. Please check on the command line."), 10000);
        }
        return;
    } else {
        executeGitCommand(git, QStringList{"commit", "-am", "Merge commit"}, process, false, true);
    }

    if (!executeGitCommand(git, QStringList{"push", "-q"}, process)) {
        if ( mw != nullptr ) {
            mw->showStatusBarMessage(QObject::tr("Git: Push to remote failed."), 10000);
        }
    } else {
        if ( mw != nullptr ) {
            mw->showStatusBarMessage(QObject::tr("Git: Pushed."), 4000);
        }
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
                                QProcess* process, bool withErrorDialog, bool ignoreReturnValue) {
    if (process == nullptr) {
        process = new QProcess();
    }

    process->start(command, arguments);

    if (!process->waitForFinished()) {
        qWarning() << "Command '" + command + "' (" << arguments << ") failed";

        if (withErrorDialog) {
            Utils::Gui::warning(
                nullptr, QObject::tr("Command failed!"),
                QObject::tr("The command <code>%1</code> with arguments <code>%2</code> failed!")
                    .arg(command, arguments.join(QStringLiteral(", "))),
                "command-failed");
        }

        return false;
    }

    if (process->exitCode() != 0 && !ignoreReturnValue) {
        QByteArray errorMessage = process->readAllStandardError();

        qWarning() << "Error Code " << process->exitCode() << " by '" + command + "' (" << arguments << "): ";

        if (!errorMessage.isEmpty()) {
            qWarning() << "Error message by '" + command + "' (" << arguments
                << "): " + errorMessage;
        }
        return false;
    }
    //    QByteArray result = process->readAll();

    //    if (!result.isEmpty()) {
    //    qDebug() << "Result message by '" + command + "': " + result;
    //    }

    return true;
}

/**
 * Executes a git command
 *
 * @param arguments
 * @param process
 * @return
 */
bool Utils::Git::executeGitCommand(const QString &gitExe, const QStringList& arguments, QProcess* process,
                                   bool withErrorDialog, bool ignoreReturnValue) {
    return executeCommand(gitExe, arguments, process, withErrorDialog, ignoreReturnValue);
}

/**
 * Returns the git command
 *
 * @return
 */
QString Utils::Git::gitCommand() {
    QSettings settings;
    QString path = settings.value("gitExecutablePath").toString();

    // ok, user defined path
    if (!path.isEmpty()) {
        return path;
    }

#ifdef Q_OS_WIN
    path = "git.exe"; // FIXME: is the ".exe" even needed? I don't think so
#else
    path = "git";
#endif

    // Ensure the "git" is from a standard path, and not a random exe named git
    auto fullGitPath = QStandardPaths::findExecutable(path);
    if (fullGitPath.isEmpty()) {
        return {};
    }

    return fullGitPath;
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

#ifdef Q_OS_WIN
    // we are executing the full command with parameters directly
    // using "cmd.exe /c" doesn't work
    QString command = gitLogCommand;
#else
    // we need a shell to be able to use the executable with the parameters
    // together
    QString command = "bash";
    parameters << "-c" << gitLogCommand;
#endif

    Utils::Misc::startDetachedProcess(command, parameters,
                                      NoteFolder::currentLocalPath());
}
