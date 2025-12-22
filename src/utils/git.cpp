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

#include "git.h"

#include <entities/notefolder.h>

#include <QDebug>
#include <QProcess>
#include <QStandardPaths>

#include "gui.h"
#include "misc.h"
#include "services/settingsservice.h"

#ifdef USE_LIBGIT2
#include <git2.h>

#include <QJSEngine>

#include "diff_match_patch.h"
#endif

/**
 * Checks if the current note folder uses git
 *
 * @return
 */
bool Utils::Git::isCurrentNoteFolderUseGit() { return NoteFolder::currentNoteFolder().isUseGit(); }

/**
 * Commits changes from the current note folder to git
 */
void Utils::Git::commitCurrentNoteFolder() {
    // check if git is enabled for the current note folder
    if (!isCurrentNoteFolderUseGit()) {
        return;
    }

    const QString &localPath = NoteFolder::currentLocalPath();

#ifdef USE_LIBGIT2
    initRepository(localPath);
    commitAll(localPath, "QOwnNotes commit");
#else
    const auto git = gitCommand();

    auto *process = new QProcess();
    process->setWorkingDirectory(localPath);

    if (!executeGitCommand(git, QStringList{"init"}, process) ||
        !executeGitCommand(git, QStringList{"config", "commit.gpgsign", "false"}, process) ||
        !executeGitCommand(git, QStringList{"add", "-A"}, process) ||
        !executeGitCommand(git, QStringList{"commit", "-m", "QOwnNotes commit"}, process)) {
    }

    delete (process);
#endif
}

/**
 * Executes a command
 *
 * @param command
 * @param process
 * @return
 */
bool Utils::Git::executeCommand(const QString &command, const QStringList &arguments,
                                QProcess *process, bool withErrorDialog) {
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

    //    QByteArray result = process->readAll();

    //    if (!result.isEmpty()) {
    //    qDebug() << "Result message by '" + command + "': " + result;
    //    }

    QByteArray errorMessage = process->readAllStandardError();

    if (!errorMessage.isEmpty()) {
        qWarning() << "Error message by '" + command + "' (" << arguments << "): " + errorMessage;
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
bool Utils::Git::executeGitCommand(const QString &gitExe, const QStringList &arguments,
                                   QProcess *process, bool withErrorDialog) {
    return executeCommand(gitExe, arguments, process, withErrorDialog);
}

/**
 * Returns the git command
 *
 * @return
 */
QString Utils::Git::gitCommand() {
    SettingsService settings;
    QString path = settings.value("gitExecutablePath").toString();

    // ok, user defined path
    if (!path.isEmpty()) {
        return path;
    }

#ifdef Q_OS_WIN
    path = "git.exe";    // FIXME: is the ".exe" even needed? I don't think so
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
    SettingsService settings;
    return !settings.value("gitLogCommand").toString().isEmpty();
}

/**
 * Shows a git log
 *
 * @param filePath
 */
void Utils::Git::showLog(const QString &filePath) {
    SettingsService settings;
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

    Utils::Misc::startDetachedProcess(command, parameters, NoteFolder::currentLocalPath());
}

#ifdef USE_LIBGIT2
bool Utils::Git::initRepository(const QString &path) {
    git_libgit2_init();

    git_repository *repo = nullptr;
    // First, check if the directory is already a git repository
    if (git_repository_open(&repo, path.toUtf8().constData()) == 0) {
        // Already a git repo
        git_repository_free(repo);
        return true;
    }

    // Not a repo, so initialize
    int error = git_repository_init(&repo, path.toUtf8().constData(), 0);

    if (error != 0) {
        const git_error *e = git_error_last();
        qWarning() << "Failed to initialize repo:" << (e ? e->message : "unknown");
        return false;
    }

    git_repository_free(repo);
    qDebug() << "Initialized repository at" << path;
    return true;
}

bool Utils::Git::commitAll(const QString &path, const QString &message) {
    git_libgit2_init();

    git_repository *repo = nullptr;
    if (git_repository_open(&repo, path.toUtf8().constData()) != 0) {
        qWarning() << "Failed to open repository at" << path;
        return false;
    }

    git_index *index = nullptr;
    git_oid tree_oid, commit_oid;

    if (git_repository_index(&index, repo) != 0) {
        qWarning() << "Failed to get index";
        git_repository_free(repo);
        return false;
    }

    git_index_add_all(index, nullptr, 0, nullptr, nullptr);
    git_index_write(index);
    git_index_write_tree(&tree_oid, index);

    // Compare with HEAD tree OID
    git_oid head_tree_oid;
    bool hasChanges = true;
    git_reference *head_ref = nullptr;
    if (git_repository_head(&head_ref, repo) == 0) {
        git_commit *head_commit = nullptr;
        if (git_reference_peel(reinterpret_cast<git_object **>(&head_commit), head_ref,
                               GIT_OBJECT_COMMIT) == 0) {
            git_tree *head_tree = nullptr;
            if (git_commit_tree(&head_tree, head_commit) == 0) {
                git_oid_cpy(&head_tree_oid, git_tree_id(head_tree));
                if (git_oid_cmp(&tree_oid, &head_tree_oid) == 0) {
                    hasChanges = false;
                }
                git_tree_free(head_tree);
            }
            git_commit_free(head_commit);
        }
        git_reference_free(head_ref);
    }

    if (!hasChanges) {
        git_index_free(index);
        git_repository_free(repo);
        return true;    // No changes to commit
    }

    git_tree *tree = nullptr;
    git_tree_lookup(&tree, repo, &tree_oid);

    git_signature *sig = nullptr;
    git_signature_now(&sig, "QOwnNotes", "noreply@qownnotes.org");

    git_commit *parent = nullptr;
    git_reference *ref = nullptr;
    int parent_count = 0;
    if (git_repository_head(&ref, repo) == 0) {
        git_reference_peel(reinterpret_cast<git_object **>(&parent), ref, GIT_OBJECT_COMMIT);
        parent_count = parent ? 1 : 0;
    }

    int error = git_commit_create_v(&commit_oid, repo, "HEAD", sig, sig, nullptr,
                                    message.toUtf8().constData(), tree, parent_count, parent);

    git_signature_free(sig);
    git_tree_free(tree);
    git_index_free(index);
    git_commit_free(parent);
    git_reference_free(ref);
    git_repository_free(repo);

    if (error != 0) {
        const git_error *e = git_error_last();
        qWarning() << "Failed to create commit:" << (e ? e->message : "unknown");
        return false;
    }

    qDebug() << "Created commit in" << path;
    return true;
}

/**
 * Gets the note versions from the Git repository
 *
 * We want to get something like this:
 * [
 *   {
 *     "timestamp": 1750925008,
 *     "humanReadableTimestamp": "2 minutes ago",
 *     "diffHtml": "Version test\n============\n\n- new<del>\n- new2</del>",
 *     "data": "Version test\n============\n\n- new"
 *   },
 *   {
 *     "timestamp": 1750924988,
 *     "humanReadableTimestamp": "2 minutes ago",
 *     "diffHtml": "Version test\n============\n\n<del>- new\n- new2</del>",
 *     "data": "Version test\n============\n\n"
 *   }
 * ]
 *
 * @param note
 * @return
 */
QJSValue Utils::Git::getNoteVersions(QJSEngine &engine, const Note &note, int limit) {
    QJSValue versions = engine.newArray();

    if (!note.exists()) {
        // Return an empty array if note does not exist
        return versions;
    }

    // Get the Git repository
    git_repository *repo = nullptr;
    QString noteFolderPath = NoteFolder::currentLocalPath();
    if (git_repository_open(&repo, noteFolderPath.toUtf8().constData()) != 0) {
        return versions;
    }

    // Get the file path relative to the repository root
    QString relativePath = note.relativeNoteFilePath();

    // Create a diff match patch instance for showing differences
    diff_match_patch differ;

    // Get all commits that modified this file
    git_revwalk *walker;
    git_revwalk_new(&walker, repo);
    git_revwalk_push_head(walker);

    git_oid oid;
    QString previousContent = note.getNoteText();
    bool canDecryptNoteText = note.canDecryptNoteText();

    if (canDecryptNoteText) {
        previousContent = note.getDecryptedNoteText(Note::parseEncryptedNoteText(previousContent));
    }

    bool isFirst = true;
    int versionIndex = 0;

    while (git_revwalk_next(&oid, walker) == 0 && versionIndex < limit) {
        git_commit *commit;
        if (git_commit_lookup(&commit, repo, &oid) == 0) {
            git_tree *tree;
            if (git_commit_tree(&tree, commit) == 0) {
                git_tree_entry *entry = nullptr;
                if (git_tree_entry_bypath(&entry, tree, relativePath.toUtf8().constData()) == 0) {
                    git_blob *blob = nullptr;
                    if (git_blob_lookup(&blob, repo, git_tree_entry_id(entry)) == 0) {
                        const char *content = (const char *)git_blob_rawcontent(blob);
                        const QString &originalCurrentContent = QString::fromUtf8(content);
                        QString currentContent = originalCurrentContent;

                        if (canDecryptNoteText) {
                            currentContent = note.getDecryptedNoteText(
                                Note::parseEncryptedNoteText(currentContent));
                        }

                        if (isFirst && currentContent == previousContent) {
                            // Skip the first version if it matches the current content
                            git_blob_free(blob);
                            git_tree_entry_free(entry);
                            isFirst = false;
                            continue;
                        }

                        QJSValue version = engine.newObject();

                        // Get commit timestamp
                        git_time_t timestamp = git_commit_time(commit);
                        QDateTime datetime = QDateTime::fromSecsSinceEpoch(timestamp);
                        version.setProperty("timestamp",
                                            engine.toScriptValue(static_cast<qint64>(timestamp)));
                        version.setProperty("humanReadableTimestamp",
                                            datetime.toString(QLocale::system().dateTimeFormat(
                                                QLocale::ShortFormat)));

                        // Store the content
                        version.setProperty("data", originalCurrentContent);

                        // Generate diff HTML
                        QList<Diff> diffs = differ.diff_main(previousContent, currentContent);
                        differ.diff_cleanupSemantic(diffs);
                        QString diffHtml = differ.diff_prettyHtml(diffs);
                        version.setProperty("diffHtml", diffHtml);

                        versions.setProperty(versionIndex++, version);
                        git_blob_free(blob);
                    }
                    git_tree_entry_free(entry);
                }
                git_tree_free(tree);
            }
            git_commit_free(commit);
        }
    }

    git_revwalk_free(walker);
    git_repository_free(repo);

    return versions;
}
#endif
