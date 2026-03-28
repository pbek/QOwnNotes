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

#include "noteoperationsmanager.h"

#include <entities/note.h>
#include <entities/notefolder.h>
#include <entities/notesubfolder.h>
#include <entities/tag.h>
#include <services/settingsservice.h>
#include <utils/gui.h>
#include <utils/misc.h>
#include <widgets/htmlpreviewwidget.h>

#include <QDir>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QTextCursor>
#include <QTimer>
#include <QTreeWidgetItem>

#include "mainwindow.h"
#include "ui_mainwindow.h"

NoteOperationsManager::NoteOperationsManager(MainWindow *mainWindow, Ui::MainWindow *ui,
                                             QObject *parent)
    : QObject(parent), _mainWindow(mainWindow), _ui(ui) {}

/**
 * Removes the current note
 */
void NoteOperationsManager::removeCurrentNote() {
    // store updated notes to disk
    _mainWindow->storeUpdatedNotesToDisk();

    // Warn if other notes contain links pointing to this note
    const QVector<int> backlinkIds = _mainWindow->currentNote.findBacklinkedNoteIds();
    if (!backlinkIds.isEmpty()) {
        if (Utils::Gui::question(_mainWindow, tr("Note has backlinks"),
                                 tr("The note <strong>%1</strong> is linked from %n other note(s). "
                                    "Deleting it will leave those links broken. "
                                    "Do you still want to remove it?",
                                    "", backlinkIds.size())
                                     .arg(_mainWindow->currentNote.getName()),
                                 QStringLiteral("remove-note-with-backlinks")) !=
            QMessageBox::Yes) {
            return;
        }
    }

    if (Utils::Gui::question(
            _mainWindow, tr("Remove current note"),
            tr("Remove current note: <strong>%1</strong>?").arg(_mainWindow->currentNote.getName()),
            QStringLiteral("remove-note")) == QMessageBox::Yes) {
        const QSignalBlocker blocker2(_ui->noteTextEdit);
        Q_UNUSED(blocker2)

#ifdef USE_QLITEHTML
        const QSignalBlocker blocker3(_mainWindow->_notePreviewWidget);
#else
        const QSignalBlocker blocker3(_ui->noteTextView);
#endif
        Q_UNUSED(blocker3)

        const QSignalBlocker blocker4(_ui->encryptedNoteTextEdit);
        Q_UNUSED(blocker4)

        const QSignalBlocker blocker5(_mainWindow->noteDirectoryWatcher);
        Q_UNUSED(blocker5)

        // we try to fix problems with note subfolders
        _mainWindow->directoryWatcherWorkaround(true);

        {
            const QSignalBlocker blocker1(_ui->noteTreeWidget);
            Q_UNUSED(blocker1)

            // search and remove note from the note tree widget
            _mainWindow->removeNoteFromNoteTreeWidget(_mainWindow->currentNote);

            // delete note in database and on file system
            _mainWindow->currentNote.remove(true);

            _mainWindow->unsetCurrentNote();
        }

        // set a new current note
        _mainWindow->resetCurrentNote(false);

        // we try to fix problems with note subfolders
        // we need to wait some time to turn the watcher on again because
        // something is happening after this method that reloads the
        // note folder
        _mainWindow->directoryWatcherWorkaround(false);
    }
}

/**
 * @brief Removes selected notes after a confirmation
 */
void NoteOperationsManager::removeSelectedNotes() {
    // store updated notes to disk
    _mainWindow->storeUpdatedNotesToDisk();

    const auto selItems = _ui->noteTreeWidget->selectedItems();
    const int selectedItemsCount = selItems.count();

    if (selectedItemsCount == 0) {
        return;
    }

    // Separate notes and folders from selected items
    QVector<QTreeWidgetItem *> noteItems;
    QVector<QTreeWidgetItem *> folderItems;
    QStringList noteSubFolderPathList;
    QVector<NoteSubFolder> noteSubFolderList;

    for (QTreeWidgetItem *item : selItems) {
        const int itemType = item->data(0, Qt::UserRole + 1).toInt();
        if (itemType == MainWindow::NoteType) {
            noteItems.append(item);
        } else if (itemType == MainWindow::FolderType) {
            folderItems.append(item);
            const int id = item->data(0, Qt::UserRole).toInt();
            const NoteSubFolder noteSubFolder = NoteSubFolder::fetch(id);
            if (noteSubFolder.isFetched()) {
                noteSubFolderList.append(noteSubFolder);
                noteSubFolderPathList.append(noteSubFolder.fullPath());
            }
        }
    }

    const int noteCount = noteItems.count();
    const int folderCount = noteSubFolderList.count();

    if (noteCount == 0 && folderCount == 0) {
        return;
    }

    // Warn if any of the selected notes have backlinks pointing to them
    if (noteCount > 0) {
        int notesWithBacklinks = 0;
        int totalBacklinks = 0;

        for (QTreeWidgetItem *item : noteItems) {
            const int id = item->data(0, Qt::UserRole).toInt();
            const Note note = Note::fetch(id);
            const QVector<int> backlinkIds = note.findBacklinkedNoteIds();
            if (!backlinkIds.isEmpty()) {
                notesWithBacklinks++;
                totalBacklinks += backlinkIds.size();
            }
        }

        if (notesWithBacklinks > 0) {
            if (Utils::Gui::question(_mainWindow, tr("Notes have backlinks"),
                                     tr("%n of the selected note(s) are linked from other notes "
                                        "(%1 link(s) in total). "
                                        "Deleting them will leave those links broken. "
                                        "Do you still want to remove them?",
                                        "", notesWithBacklinks)
                                         .arg(totalBacklinks),
                                     QStringLiteral("remove-notes-with-backlinks")) !=
                QMessageBox::Yes) {
                return;
            }
        }
    }

    // Build confirmation message based on what's selected
    QString title;
    QString message;
    QString dialogName;

    if (noteCount > 0 && folderCount > 0) {
        // Both notes and folders selected
        title = tr("Remove selected notes and folders");
        message = Utils::Misc::replaceOwnCloudText(
            tr("Remove <strong>%n</strong> selected note(s) and "
               "<strong>%1</strong> folder(s)?"
               "<ul><li>%2</li></ul>"
               "All files and folders in these folders will be removed as well!\n\n"
               "If the trash is enabled on your ownCloud server you should be able to restore "
               "the notes from there.",
               "", noteCount)
                .arg(folderCount)
                .arg(noteSubFolderPathList.join(QStringLiteral("</li><li>"))));
        dialogName = QStringLiteral("remove-notes-and-folders");
    } else if (folderCount > 0) {
        // Only folders selected
        title = tr("Remove selected folders");
        message = tr("Remove <strong>%n</strong> selected folder(s)?"
                     "<ul><li>%1</li></ul>"
                     "All files and folders in these folders will be removed as well!",
                     "", folderCount)
                      .arg(noteSubFolderPathList.join(QStringLiteral("</li><li>")));
        dialogName = QStringLiteral("remove-folders");
    } else {
        // Only notes selected
        title = tr("Remove selected notes");
        message = Utils::Misc::replaceOwnCloudText(
            tr("Remove <strong>%n</strong> selected note(s)?\n\n"
               "If the trash is enabled on your ownCloud server you should be able to restore "
               "them from there.",
               "", noteCount));
        dialogName = QStringLiteral("remove-notes");
    }

    if (Utils::Gui::question(_mainWindow, title, message, dialogName) == QMessageBox::Yes) {
        const QSignalBlocker blocker(_mainWindow->noteDirectoryWatcher);
        Q_UNUSED(blocker)

        const QSignalBlocker blocker2(_mainWindow->activeNoteTextEdit());
        Q_UNUSED(blocker2)

#ifndef USE_QLITEHTML
        const QSignalBlocker blocker3(_ui->noteTextView);
#else
        const QSignalBlocker blocker3(_mainWindow->_notePreviewWidget);
#endif
        Q_UNUSED(blocker3)

        const QSignalBlocker blocker4(_ui->encryptedNoteTextEdit);
        Q_UNUSED(blocker4)

        // We try to fix problems with note subfolders
        _mainWindow->directoryWatcherWorkaround(true);

        {
            const QSignalBlocker blocker1(_ui->noteTreeWidget);
            Q_UNUSED(blocker1)

            // Remove notes
            for (QTreeWidgetItem *item : noteItems) {
                const int id = item->data(0, Qt::UserRole).toInt();
                Note note = Note::fetch(id);

                // Search and remove note from the note tree widget
                _mainWindow->removeNoteFromNoteTreeWidget(note);

                note.remove(true);
                qDebug() << "Removed note " << note.getName();
            }

            // Remove folders
            for (const auto &noteSubFolder : Utils::asConst(noteSubFolderList)) {
                // remove the directory recursively from the file system
                if (noteSubFolder.removeFromFileSystem()) {
                    _mainWindow->showStatusBarMessage(
                        tr("Removed note subfolder: %1").arg(noteSubFolder.fullPath()),
                        QStringLiteral("📁"));
                }
            }

            // clear the text edit so it stays clear after removing notes
            // (either directly or via folder deletion)
            _mainWindow->activeNoteTextEdit()->clear();
        }

        // We try to fix problems with note subfolders
        // we need to wait some time to turn the watcher on again because
        // something is happening after this method that reloads the note folder
        _mainWindow->directoryWatcherWorkaround(false);

        // Set a new current note (needed whether notes or folders were deleted,
        // as folders may contain the currently displayed note)
        _mainWindow->resetCurrentNote(false);

        // Reload note folder if folders were deleted
        if (folderCount > 0) {
            _mainWindow->reloadNoteFolderAction()->trigger();
        }
    }

    _mainWindow->loadNoteDirectoryList();
}

/**
 * @brief Moves selected notes after a confirmation
 * @param destinationFolder
 */
void NoteOperationsManager::moveSelectedNotesToFolder(const QString &destinationFolder) {
    // store updated notes to disk
    _mainWindow->storeUpdatedNotesToDisk();

    // Count only note items (not folders) for the confirmation dialog
    const auto selItems = _ui->noteTreeWidget->selectedItems();
    int noteCount = 0;
    for (const auto *item : selItems) {
        if (item->data(0, Qt::UserRole + 1).toInt() == MainWindow::NoteType) {
            noteCount++;
        }
    }

    if (noteCount == 0) {
        return;
    }

    if (Utils::Gui::question(_mainWindow, tr("Move selected notes"),
                             tr("Move %n selected note(s) to <strong>%2</strong>?", "", noteCount)
                                 .arg(destinationFolder),
                             QStringLiteral("move-notes")) == QMessageBox::Yes) {
        const QSignalBlocker blocker(_mainWindow->noteDirectoryWatcher);
        Q_UNUSED(blocker)

        for (QTreeWidgetItem *item : selItems) {
            if (item->data(0, Qt::UserRole + 1).toInt() != MainWindow::NoteType) {
                continue;
            }

            const int noteId = item->data(0, Qt::UserRole).toInt();
            Note note = Note::fetch(noteId);

            if (!note.isFetched()) {
                continue;
            }

            // remove note path form directory watcher
            _mainWindow->noteDirectoryWatcher.removePath(note.fullNoteFilePath());

            if (note.getId() == _mainWindow->currentNote.getId()) {
                // unset the current note
                _mainWindow->unsetCurrentNote();
            }

            // move note
            const bool result = note.moveToPath(destinationFolder);
            if (result) {
                qDebug() << "Note was moved:" << note.getName();
            } else {
                qWarning() << "Could not move note:" << note.getName();
            }
        }

        _mainWindow->loadNoteDirectoryList();
    }
}

/**
 * @brief Copies selected notes after a confirmation
 * @param destinationFolder
 */
void NoteOperationsManager::copySelectedNotesToFolder(const QString &destinationFolder,
                                                      const QString &noteFolderPath) {
    // Count only note items (not folders) for the confirmation dialog
    const auto selItems = _ui->noteTreeWidget->selectedItems();
    int noteCount = 0;
    for (const auto *item : selItems) {
        if (item->data(0, Qt::UserRole + 1).toInt() == MainWindow::NoteType) {
            noteCount++;
        }
    }

    if (noteCount == 0) {
        return;
    }

    if (Utils::Gui::question(_mainWindow, tr("Copy selected notes"),
                             tr("Copy %n selected note(s) to <strong>%2</strong>?", "", noteCount)
                                 .arg(destinationFolder),
                             QStringLiteral("copy-notes")) == QMessageBox::Yes) {
        int copyCount = 0;
        for (QTreeWidgetItem *item : selItems) {
            if (item->data(0, Qt::UserRole + 1).toInt() != MainWindow::NoteType) {
                continue;
            }

            const int noteId = item->data(0, Qt::UserRole).toInt();
            Note note = Note::fetch(noteId);

            if (!note.isFetched()) {
                continue;
            }

            // copy note
            const bool result = note.copyToPath(destinationFolder, noteFolderPath);
            if (result) {
                copyCount++;
                qDebug() << "Note was copied:" << note.getName();
            } else {
                qWarning() << "Could not copy note:" << note.getName();
            }
        }

        Utils::Gui::information(_mainWindow, tr("Done"),
                                tr("%n note(s) were copied to <strong>%2</strong>.", "", copyCount)
                                    .arg(destinationFolder),
                                QStringLiteral("notes-copied"));
    }
}

/**
 * Creates a new note
 *
 * @param noteName
 */
void NoteOperationsManager::createNewNote(QString noteName, bool withNameAppend) {
    // turn on note editing if it was disabled
    if (!Utils::Misc::isNoteEditingAllowed()) {
        _ui->actionAllow_note_editing->trigger();
    }

    // show the window in case we are using the system tray
    _mainWindow->show();

    if (noteName.isEmpty()) {
        noteName = tr("Note", "name for new note");
    }

    if (withNameAppend) {
        QDateTime currentDate = QDateTime::currentDateTime();

        // Format the date and time like "2025-04-18 11h54s09"
        noteName = noteName + QStringLiteral(" ") +
                   currentDate.toString(QStringLiteral("yyyy-MM-dd HH'h'mm's'ss"));
    }

    const QSignalBlocker blocker(_ui->searchLineEdit);
    Q_UNUSED(blocker)

    _ui->searchLineEdit->setText(noteName);

    // create a new note or jump to the existing
    _mainWindow->jumpToNoteOrCreateNew();
}

/**
 * Creates a new note (to restore a trashed note)
 * This is a public callback function for the trash dialog.
 *
 * @brief NoteOperationsManager::createNewNote
 * @param name
 * @param text
 * @param cursorAtEnd
 */
void NoteOperationsManager::createNewNote(QString name, QString text,
                                          MainWindow::CreateNewNoteOptions options) {
    const QString extension = Note::defaultNoteFileExtension();
    auto *f = new QFile(_mainWindow->notesPath + QDir::separator() + name + QStringLiteral(".") +
                        extension);
    const bool useNameAsHeadline =
        options.testFlag(MainWindow::CreateNewNoteOption::UseNameAsHeadline);

    // change the name and headline if note exists
    if (f->exists()) {
        QDateTime currentDate = QDateTime::currentDateTime();
        name.append(
            QStringLiteral(" ") +
            currentDate.toString(Qt::ISODate).replace(QStringLiteral(":"), QStringLiteral(".")));

        if (!useNameAsHeadline) {
            QString preText = Note::createNoteHeader(name);
            text.prepend(preText);
        }
    }

    // create a new note
    _ui->searchLineEdit->setText(name);

    _mainWindow->jumpToNoteOrCreateNew(
        options.testFlag(MainWindow::CreateNewNoteOption::DisableLoadNoteDirectoryList));

    // check if to append the text or replace the text of the note
    if (useNameAsHeadline) {
        QTextCursor c = _ui->noteTextEdit->textCursor();
        // make sure the cursor is really at the end to be able to
        // insert the text on the correct position
        c.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        c.insertText(QStringLiteral("\n\n") + text);
        _ui->noteTextEdit->setTextCursor(c);
    } else {
        _ui->noteTextEdit->setText(text);
    }

    // move the cursor to the end of the note
    if (options.testFlag(MainWindow::CreateNewNoteOption::CursorAtEnd)) {
        QTextCursor c = _ui->noteTextEdit->textCursor();
        c.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        _ui->noteTextEdit->setTextCursor(c);
    }
}

/**
 * Moves selected notes to a note subfolder id
 */
void NoteOperationsManager::moveSelectedNotesToNoteSubFolderId(int noteSubFolderId) {
    qDebug() << __func__ << " - 'noteSubFolderId': " << noteSubFolderId;
    const NoteSubFolder noteSubFolder = NoteSubFolder::fetch(noteSubFolderId);

    // move selected notes to note subfolder
    if (noteSubFolder.isFetched() || (noteSubFolderId == 0)) {
        moveSelectedNotesToNoteSubFolder(noteSubFolder);
    }
}

/**
 * Copies selected notes to a note subfolder id
 */
void NoteOperationsManager::copySelectedNotesToNoteSubFolderId(int noteSubFolderId) {
    qDebug() << __func__ << " - 'noteSubFolderId': " << noteSubFolderId;
    const NoteSubFolder noteSubFolder = NoteSubFolder::fetch(noteSubFolderId);

    // copy selected notes to note subfolder
    if (noteSubFolder.isFetched() || (noteSubFolderId == 0)) {
        copySelectedNotesToNoteSubFolder(noteSubFolder);
    }
}

/**
 * Moves selected notes to a note subfolder
 */
void NoteOperationsManager::moveSelectedNotesToNoteSubFolder(const NoteSubFolder &noteSubFolder) {
    // Count only note items (not folders) for the confirmation dialog
    const auto selItems = _ui->noteTreeWidget->selectedItems();
    int noteCount = 0;
    for (const auto *item : selItems) {
        if (item->data(0, Qt::UserRole + 1).toInt() == MainWindow::NoteType) {
            noteCount++;
        }
    }

    if (noteCount == 0) {
        return;
    }

    const QString text = tr("Move %n selected note(s) to note subfolder "
                            "<strong>%2</strong>?",
                            "", noteCount)
                             .arg(noteSubFolder.getName());

    if (Utils::Gui::question(_mainWindow, tr("Move selected notes"), text,
                             QStringLiteral("move-notes")) == QMessageBox::Yes) {
        const QSignalBlocker blocker(_mainWindow->noteDirectoryWatcher);
        Q_UNUSED(blocker)

        // unset the current note
        //      unsetCurrentNote();

        int noteSubFolderCount = 0;

        // disable the externally removed check, because it might trigger
        _mainWindow->_noteExternallyRemovedCheckEnabled = false;

        bool forceReload = false;
        for (QTreeWidgetItem *item : selItems) {
            if (item->data(0, Qt::UserRole + 1).toInt() != MainWindow::NoteType) {
                continue;
            }

            const int noteId = item->data(0, Qt::UserRole).toInt();
            Note note = Note::fetch(noteId);
            Note oldNote = note;

            if (!note.isFetched()) {
                continue;
            }

            // don't move note if source and destination paths are the same
            if (noteSubFolder.fullPath() == note.fullNoteFileDirPath()) {
                qWarning() << "Note was not moved because source and "
                              "destination paths were the same:"
                           << note.getName();

                continue;
            }

            // fetch the tags to tag the note after moving it
            const QVector<Tag> tags = Tag::fetchAllOfNote(note);

            if (note.getId() == _mainWindow->currentNote.getId()) {
                // unset the current note
                _mainWindow->unsetCurrentNote();
            }

            // move note
            const bool result = note.moveToPath(noteSubFolder.fullPath());
            if (result) {
                noteSubFolderCount++;
                qDebug() << "Note was moved:" << note.getName();

                // set the new subfolder so the tags are stored correctly
                note.setNoteSubFolder(noteSubFolder);

                // tag the note again
                for (const Tag &tag : tags) {
                    tag.linkToNote(note);
                }

                // handle the replacing of all note links from other notes
                // because the note was moved
                if (note.handleNoteMoving(oldNote)) {
                    // reload the current note if we had to change it
                    _mainWindow->reloadCurrentNoteByNoteId(true);
                    forceReload = true;
                }

                // re-link images
                const bool mediaFileLinksUpdated = note.updateRelativeMediaFileLinks();

                // re-link attachments
                const bool attachmentFileLinksUpdated = note.updateRelativeAttachmentFileLinks();

                if (mediaFileLinksUpdated || attachmentFileLinksUpdated) {
                    note.storeNoteTextFileToDisk();
                }
            } else {
                qWarning() << "Could not move note:" << note.getName();
            }
        }

        // rebuild the index after the move
        if (noteSubFolderCount > 0) {
            // for some reason this only works with a small delay, otherwise
            // not all changes will be recognized
            QTimer::singleShot(150, _mainWindow, [this, forceReload] {
                // If the outgoing links to other notes were changed, we have to really reload the
                // note folder
                if (forceReload) {
                    _mainWindow->buildNotesIndexAndLoadNoteDirectoryList(true, true);
                } else {
                    _mainWindow->buildNotesIndexAndLoadNoteDirectoryList();
                }
            });
        }

        _mainWindow->showStatusBarMessage(
            tr("%n note(s) were moved to note subfolder \"%2\"", "", noteSubFolderCount)
                .arg(noteSubFolder.getName()),
            QStringLiteral("📁"), 5000);

        // wait some time to enable the check again to prevent troubles on macOS
        QTimer::singleShot(4000, _mainWindow, SLOT(enableNoteExternallyRemovedCheck()));
    }
}

/**
 * Copies selected notes to a note subfolder
 */
void NoteOperationsManager::copySelectedNotesToNoteSubFolder(const NoteSubFolder &noteSubFolder) {
    // Count only note items (not folders) for the confirmation dialog
    const auto selItems = _ui->noteTreeWidget->selectedItems();
    int noteCount = 0;
    for (const auto *item : selItems) {
        if (item->data(0, Qt::UserRole + 1).toInt() == MainWindow::NoteType) {
            noteCount++;
        }
    }

    if (noteCount == 0) {
        return;
    }

    const QString text = tr("Copy %n selected note(s) to note subfolder "
                            "<strong>%2</strong>?",
                            "", noteCount)
                             .arg(noteSubFolder.getName());

    if (Utils::Gui::question(_mainWindow, tr("Copy selected notes"), text,
                             QStringLiteral("copy-notes")) == QMessageBox::Yes) {
        const QSignalBlocker blocker(_mainWindow->noteDirectoryWatcher);
        Q_UNUSED(blocker)

        int noteSubFolderCount = 0;
        for (QTreeWidgetItem *item : selItems) {
            if (item->data(0, Qt::UserRole + 1).toInt() != MainWindow::NoteType) {
                continue;
            }

            const int noteId = item->data(0, Qt::UserRole).toInt();
            Note note = Note::fetch(noteId);

            if (!note.isFetched()) {
                continue;
            }

            // don't copy note if source and destination paths are the same
            if (noteSubFolder.fullPath() == note.fullNoteFileDirPath()) {
                qWarning() << "Note was not copied because source and "
                              "destination paths were the same:"
                           << note.getName();

                continue;
            }

            // fetch the tags to tag the note after copying it
            const QVector<Tag> tags = Tag::fetchAllOfNote(note);

            // copy note
            const bool result = note.copyToPath(noteSubFolder.fullPath());
            if (result) {
                noteSubFolderCount++;
                qDebug() << "Note was copied:" << note.getName();

                // set the new subfolder so the tags are stored correctly
                note.setNoteSubFolder(noteSubFolder);

                // tag the note again
                for (const Tag &tag : tags) {
                    tag.linkToNote(note);
                }

                // re-link images
                const bool mediaFileLinksUpdated = note.updateRelativeMediaFileLinks();

                // re-link attachments
                const bool attachmentFileLinksUpdated = note.updateRelativeAttachmentFileLinks();

                if (mediaFileLinksUpdated || attachmentFileLinksUpdated) {
                    note.storeNoteTextFileToDisk();
                }
            } else {
                qWarning() << "Could not copy note:" << note.getName();
            }
        }

        // rebuild the index after the copy
        if (noteSubFolderCount > 0) {
            // for some reason this only works with a small delay, otherwise
            // not all changes will be recognized
            QTimer::singleShot(150, _mainWindow, SLOT(buildNotesIndexAndLoadNoteDirectoryList()));
        }

        _mainWindow->showStatusBarMessage(
            tr("%n note(s) were copied to note subfolder \"%2\"", "", noteSubFolderCount)
                .arg(noteSubFolder.getName()),
            QStringLiteral("📁"), 5000);
    }
}

/**
 * Populates a subfolder menu tree for bulk note moving or copying
 */
void NoteOperationsManager::buildBulkNoteSubFolderMenuTree(QMenu *parentMenu, bool doCopy,
                                                           int parentNoteSubFolderId) {
    const QVector<NoteSubFolder> noteSubFolderList =
        NoteSubFolder::fetchAllByParentId(parentNoteSubFolderId, QStringLiteral("name ASC"));

    for (const auto &noteSubFolder : noteSubFolderList) {
        const int noteSubFolderId = noteSubFolder.getId();
        const QString name = noteSubFolder.getName();

        const int count = NoteSubFolder::countAllParentId(noteSubFolderId);
        if (count > 0) {
            // if there are sub-noteSubFolder build a new menu level
            QMenu *noteSubFolderMenu = parentMenu->addMenu(name);
            buildBulkNoteSubFolderMenuTree(noteSubFolderMenu, doCopy, noteSubFolderId);
        } else {
            // if there are no sub-noteSubFolders just create a named action
            QAction *action = parentMenu->addAction(name);

            connect(action, &QAction::triggered, this, [this, doCopy, noteSubFolderId]() {
                doCopy ? copySelectedNotesToNoteSubFolderId(noteSubFolderId)
                       : moveSelectedNotesToNoteSubFolderId(noteSubFolderId);
            });
        }
    }

    // add an action to copy or move to this subfolder
    parentMenu->addSeparator();
    const QString text =
        (parentNoteSubFolderId == 0)
            ? (doCopy ? tr("Copy to note folder") : tr("Move to note folder"))
            : (doCopy ? tr("Copy to this subfolder") : tr("Move to this subfolder"));
    QAction *action = parentMenu->addAction(text);
    action->setData(parentNoteSubFolderId);

    connect(action, &QAction::triggered, this, [this, doCopy, parentNoteSubFolderId]() {
        doCopy ? copySelectedNotesToNoteSubFolderId(parentNoteSubFolderId)
               : moveSelectedNotesToNoteSubFolderId(parentNoteSubFolderId);
    });
}

/**
 * Populates a subfolder menu tree for bulk note moving or copying to
 * subfolders of other note folders
 */
void NoteOperationsManager::buildBulkNoteFolderSubFolderMenuTree(
    QMenu *parentMenu, bool doCopy, const QString &parentNoteSubFolderPath, bool isRoot) {
    QDir dir(parentNoteSubFolderPath);
    QStringList nameFilters{};

    if (isRoot) {
        nameFilters << QStringList({"media", "trash", "attachments"});
    }

    // show newest entry first
    QStringList directoryNames = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);

    if (isRoot) {
        const auto names = QStringList({"media", "trash", "attachments"});
        for (const QString &name : names) {
            directoryNames.removeAll(name);
        }
    }

    for (const QString &directoryName : Utils::asConst(directoryNames)) {
        const QString fullPath = parentNoteSubFolderPath + QLatin1Char('/') + directoryName;
        QDir subDir(fullPath);
        const QStringList subDirectoryNames = subDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

        if (subDirectoryNames.count() > 0) {
            // if there are sub folders build a new menu level
            QMenu *noteSubFolderMenu = parentMenu->addMenu(directoryName);
            buildBulkNoteFolderSubFolderMenuTree(noteSubFolderMenu, doCopy, fullPath, false);
        } else {
            // if there are no sub folders just create a named action
            QAction *action = parentMenu->addAction(directoryName);
            action->setToolTip(fullPath);
            action->setStatusTip(fullPath);

            connect(action, &QAction::triggered, this, [this, doCopy, fullPath]() {
                doCopy ? copySelectedNotesToFolder(fullPath) : moveSelectedNotesToFolder(fullPath);
            });
        }
    }

    // add an action to copy or move to this subfolder
    parentMenu->addSeparator();
    const QString text =
        (isRoot) ? (doCopy ? tr("Copy to note folder") : tr("Move to note folder"))
                 : (doCopy ? tr("Copy to this subfolder") : tr("Move to this subfolder"));
    auto *action = parentMenu->addAction(text);
    action->setToolTip(parentNoteSubFolderPath);
    action->setStatusTip(parentNoteSubFolderPath);

    connect(action, &QAction::triggered, this, [this, doCopy, parentNoteSubFolderPath]() {
        doCopy ? copySelectedNotesToFolder(parentNoteSubFolderPath)
               : moveSelectedNotesToFolder(parentNoteSubFolderPath);
    });
}

/**
 * Splits the current note into two notes at the current cursor position
 */
void NoteOperationsManager::on_actionSplit_note_at_cursor_position_triggered() {
    if (Utils::Gui::question(_mainWindow, tr("Split note"),
                             tr("Split note at current cursor position? "
                                "The text after the cursor will be moved to a new note. "
                                "The new note will be linked to the old note."),
                             QStringLiteral("split-note")) != QMessageBox::Yes) {
        return;
    }

    QString name = _mainWindow->currentNote.getName();
    const QVector<Tag> tags = Tag::fetchAllOfNote(_mainWindow->currentNote);

    QOwnNotesMarkdownTextEdit *textEdit = _mainWindow->activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();

    // select the text to get into a new note
    c.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    const QString selectedText = c.selectedText();

    // remove the selected text
    c.removeSelectedText();
    textEdit->setTextCursor(c);

    Note previousNote = _mainWindow->currentNote;

    // create a new note
    createNewNote(std::move(name));

    // adding a link to new note into the old note
    previousNote.refetch();
    const QString noteLink = previousNote.getNoteUrlForLinkingTo(_mainWindow->currentNote);
    QString previousNoteText = previousNote.getNoteText();
    previousNoteText.reserve(3 + noteLink.size() + 1);
    previousNoteText += QStringLiteral("\n\n<") + noteLink + QStringLiteral(">");
    previousNote.storeNewText(std::move(previousNoteText));

    // add the previously removed text
    textEdit = _mainWindow->activeNoteTextEdit();
    textEdit->insertPlainText(selectedText);

    // link the tags of the old note to the new note
    for (const Tag &tag : tags) {
        tag.linkToNote(_mainWindow->currentNote);
    }
}

void NoteOperationsManager::on_action_Remove_note_triggered() { removeCurrentNote(); }

/**
 * Triggered by the shortcut to create a new note with date in the headline
 */
void NoteOperationsManager::on_action_New_note_triggered() {
    SettingsService settings;
    const bool newNoteAskHeadline = settings.value(QStringLiteral("newNoteAskHeadline")).toBool();

    // check if we want to ask for a headline
    if (newNoteAskHeadline) {
        bool ok;
        QString headline = QInputDialog::getText(_mainWindow, tr("New note"), tr("Note headline"),
                                                 QLineEdit::Normal, QString(), &ok);

        if (!ok) {
            return;
        }

        if (!headline.isEmpty()) {
            createNewNote(headline, false);
            return;
        }
    }

    // create a new note with date in the headline
    createNewNote(QString(), true);
}
