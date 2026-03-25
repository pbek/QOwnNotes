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

#include "notetreemanager.h"

#include <dialogs/settingsdialog.h>
#include <entities/note.h>
#include <entities/notefolder.h>
#include <entities/notesubfolder.h>
#include <entities/tag.h>
#include <services/owncloudservice.h>
#include <services/scriptingservice.h>
#include <services/settingsservice.h>
#include <utils/git.h>
#include <utils/gui.h>
#include <utils/misc.h>
#include <widgets/notesubfoldertree.h>
#include <widgets/notetreewidgetitem.h>

#include <QDesktopServices>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <memory>

#include "mainwindow.h"
#include "ui_mainwindow.h"

NoteTreeManager::NoteTreeManager(MainWindow *mainWindow, Ui::MainWindow *ui, QObject *parent)
    : QObject(parent), _mainWindow(mainWindow), _ui(ui) {}

/**
 * Adds a note to the note tree widget
 */
bool NoteTreeManager::addNoteToNoteTreeWidget(const Note &note, QTreeWidgetItem *parent) {
    const QString name = note.getName();

    // skip notes without name
    if (name.isEmpty()) {
        return false;
    }

    const bool isNoteListPreview = Utils::Misc::isNoteListPreview();

    // add a note item to the tree
    auto *noteItem = new QTreeWidgetItem();
    Utils::Gui::setTreeWidgetItemToolTipForNote(noteItem, note);
    noteItem->setText(0, name);
    noteItem->setData(0, Qt::UserRole, note.getId());
    noteItem->setData(0, Qt::UserRole + 1, MainWindow::NoteType);

    // Store favorite status for sorting (UserRole + 2)
    const bool isFavorite = note.isFavorite();
    noteItem->setData(0, Qt::UserRole + 2, isFavorite);

    // Set the icon based on favorite status
    if (isFavorite) {
        noteItem->setIcon(0, Utils::Gui::favoriteNoteIcon());
    } else {
        noteItem->setIcon(0, Utils::Gui::noteIcon());
    }

    const Tag tag = Tag::fetchOneOfNoteWithColor(note);
    if (tag.isFetched()) {
        // set the color of the note tree widget item
        Utils::Gui::handleTreeWidgetItemTagColor(noteItem, tag);
    }

    const bool isEditable = Note::allowDifferentFileName();
    if (isEditable) {
        noteItem->setFlags(noteItem->flags() | Qt::ItemIsEditable);
    }

    const QSignalBlocker blocker(_ui->noteTreeWidget);
    Q_UNUSED(blocker)

    if (parent == nullptr) {
        // Insert notes with favorites at the top
        // Find the insertion position based on favorite status
        if (isFavorite) {
            // For favorite notes, find the last position of favorites
            int insertPos = 0;
            const int count = _ui->noteTreeWidget->topLevelItemCount();
            for (int i = 0; i < count; ++i) {
                QTreeWidgetItem *existingItem = _ui->noteTreeWidget->topLevelItem(i);
                if (existingItem->data(0, Qt::UserRole + 1) == MainWindow::NoteType &&
                    existingItem->data(0, Qt::UserRole + 2).toBool()) {
                    insertPos = i + 1;
                } else {
                    break;
                }
            }
            _ui->noteTreeWidget->insertTopLevelItem(insertPos, noteItem);
        } else {
            // Non-favorite notes go after all favorites
            _ui->noteTreeWidget->addTopLevelItem(noteItem);
        }
    } else {
        parent->addChild(noteItem);
    }

    if (isNoteListPreview) {
        updateNoteTreeWidgetItem(note, noteItem);
    }

    //    SettingsService settings;
    //    if (settings.value("notesPanelSort", SORT_BY_LAST_CHANGE).toInt() ==
    //    SORT_ALPHABETICAL) {
    //        ui->noteTreeWidget->addTopLevelItem(noteItem);
    //    } else {
    //        ui->noteTreeWidget->insertTopLevelItem(0, noteItem);
    //    }

    return true;
}

void NoteTreeManager::updateNoteTreeWidgetItem(const Note &note, QTreeWidgetItem *noteItem) {
    if (noteItem == nullptr) {
        noteItem = findNoteInNoteTreeWidget(note);
    }

    QWidget *widget = _ui->noteTreeWidget->itemWidget(noteItem, 0);
    auto *noteTreeWidgetItem = dynamic_cast<NoteTreeWidgetItem *>(widget);

    // check if we already set a NoteTreeWidgetItem in the past
    if (noteTreeWidgetItem != nullptr) {
        noteTreeWidgetItem->updateUserInterface(note);
    } else {
        noteTreeWidgetItem = new NoteTreeWidgetItem(note, _ui->noteTreeWidget);
    }

    // TODO: set background color
    //    noteTreeWidgetItem->setBackground(noteItem->background(0).color());
    // TODO: handle note renaming
    // TODO: handle updating when note gets changed
    // TODO: handle updating in handleTreeWidgetItemTagColor

    // this takes too long, it takes ages to do this on 1000 notes
    _ui->noteTreeWidget->setItemWidget(noteItem, 0, noteTreeWidgetItem);
}

/**
 * @brief makes the current note the first item in the note list without
 * reloading the whole list
 */
void NoteTreeManager::makeCurrentNoteFirstInNoteList() {
    QTreeWidgetItem *item = findNoteInNoteTreeWidget(_mainWindow->currentNote);

    if (item != nullptr) {
        const QSignalBlocker blocker(_ui->noteTreeWidget);
        Q_UNUSED(blocker)

        _ui->noteTreeWidget->takeTopLevelItem(_ui->noteTreeWidget->indexOfTopLevelItem(item));

        // Determine insertion position based on favorite status
        const bool isFavorite = _mainWindow->currentNote.isFavorite();
        int insertPos = 0;

        if (!isFavorite) {
            // Non-favorite note: insert after all favorites
            const int count = _ui->noteTreeWidget->topLevelItemCount();
            for (int i = 0; i < count; ++i) {
                QTreeWidgetItem *existingItem = _ui->noteTreeWidget->topLevelItem(i);
                if (existingItem->data(0, Qt::UserRole + 1) == MainWindow::NoteType &&
                    existingItem->data(0, Qt::UserRole + 2).toBool()) {
                    insertPos = i + 1;
                } else {
                    break;
                }
            }
        }
        // For favorite notes, insertPos stays at 0 (first position)

        _ui->noteTreeWidget->insertTopLevelItem(insertPos, item);

        // set the item as current item if it is visible
        if (!item->isHidden()) {
            _ui->noteTreeWidget->setCurrentItem(item);

            if (Utils::Misc::isNoteListPreview()) {
                // ui->noteTreeWidget->setCurrentItem seems to destroy the
                // NoteTreeWidgetItem
                // TODO: the list symbol is still gone
                updateNoteTreeWidgetItem(_mainWindow->currentNote, item);
            }
        }

        //        bool isInActiveNoteSubFolder =
        //                NoteSubFolder::activeNoteSubFolderId() ==
        //                currentNote.getNoteSubFolderId();

        // has problems with
        // NoteSubFolder::isNoteSubfoldersPanelShowNotesRecursively()
        //        if (!(isInActiveNoteSubFolder ||
        //        _showNotesFromAllNoteSubFolders)) {
        //            item->setHidden(true);
        //        } else {
        //            ui->noteTreeWidget->setCurrentItem(item);
        //        }
    }
}

/**
 * Finds a note in the note tree widget and returns its item
 *
 * @param note
 * @return
 */
QTreeWidgetItem *NoteTreeManager::findNoteInNoteTreeWidget(const Note &note) {
    const int noteId = note.getId();
    const int count = _ui->noteTreeWidget->topLevelItemCount();

    for (int i = 0; i < count; ++i) {
        QTreeWidgetItem *item = _ui->noteTreeWidget->topLevelItem(i);

        if (item->data(0, Qt::UserRole + 1) == MainWindow::NoteType &&
            item->data(0, Qt::UserRole).toInt() == noteId) {
            return item;
        }
    }

    return nullptr;
}

/**
 * Searches and removes note from the note tree widget
 */
void NoteTreeManager::removeNoteFromNoteTreeWidget(Note &note) const {
    auto *item = Utils::Gui::getTreeWidgetItemWithUserData(_ui->noteTreeWidget, note.getId());

    if (item != nullptr) {
        delete (item);
    }
}

/**
 * Finds the first visible tree widget item
 */
QTreeWidgetItem *NoteTreeManager::firstVisibleNoteTreeWidgetItem() {
    QTreeWidgetItemIterator it(_ui->noteTreeWidget, QTreeWidgetItemIterator::NotHidden);

    return *it;
}

int NoteTreeManager::getSelectedNotesCount() const {
    return _ui->noteTreeWidget->selectedItems().count();
}

/**
 * Returns a list of all selected notes
 *
 * @return
 */
QVector<Note> NoteTreeManager::selectedNotes() {
    QVector<Note> selectedNotes;

    const auto selectedItems = _ui->noteTreeWidget->selectedItems();
    for (QTreeWidgetItem *item : selectedItems) {
        if (item->data(0, Qt::UserRole + 1) != MainWindow::NoteType) {
            continue;
        }

        const int noteId = item->data(0, Qt::UserRole).toInt();
        const Note note = Note::fetch(noteId);

        if (note.isFetched()) {
            selectedNotes << note;
        }
    }

    return selectedNotes;
}

void NoteTreeManager::on_noteTreeWidget_currentItemChanged(QTreeWidgetItem *current,
                                                           QTreeWidgetItem *previous) {
    // in case all notes were removed
    if (current == nullptr) {
        return;
    }

    // handle changing of the current item for subfolders
    if (current->data(0, Qt::UserRole + 1).toInt() == MainWindow::FolderType) {
        _ui->noteSubFolderTreeWidget->currentItemChanged(current, previous);
        return;
    }
}

void NoteTreeManager::on_noteTreeWidget_customContextMenuRequested(const QPoint pos) {
    auto *item = _ui->noteTreeWidget->itemAt(pos);

    // if the user clicks at empty space, this is null and if it isn't handled
    // QON crashes
    if (item == nullptr) {
        return;
    }

    const QPoint globalPos = _ui->noteTreeWidget->mapToGlobal(pos);
    const auto selectedItems = _ui->noteTreeWidget->selectedItems();

    // Check if we have a mixed selection of notes and folders
    bool hasNotes = false;
    bool hasFolders = false;
    for (const auto *selectedItem : selectedItems) {
        const int itemType = selectedItem->data(0, Qt::UserRole + 1).toInt();
        if (itemType == MainWindow::NoteType) {
            hasNotes = true;
        } else if (itemType == MainWindow::FolderType) {
            hasFolders = true;
        }
        if (hasNotes && hasFolders) {
            break;
        }
    }

    // If we have notes in the selection (with or without folders),
    // show the notes context menu (which now supports both notes and folders)
    if (hasNotes) {
        openNotesContextMenu(globalPos, hasNotes, hasFolders);
    } else if (hasFolders) {
        // Only folders selected, show folder-specific context menu
        std::unique_ptr<QMenu> menu(NoteSubFolderTree::contextMenu(_ui->noteTreeWidget));
        menu->exec(globalPos);
    }
}

void NoteTreeManager::openNotesContextMenu(const QPoint globalPos, bool hasNotes, bool hasFolders) {
    QMenu noteMenu;
    QAction *renameAction = nullptr;

    // Calculate counts for proper labels
    const auto selectedItems = _ui->noteTreeWidget->selectedItems();
    int noteCount = 0;
    int folderCount = 0;
    for (const auto *item : selectedItems) {
        const int itemType = item->data(0, Qt::UserRole + 1).toInt();
        if (itemType == MainWindow::NoteType) {
            noteCount++;
        } else if (itemType == MainWindow::FolderType) {
            folderCount++;
        }
    }

    bool multiNoteMenuEntriesOnly = (noteCount + folderCount) > 1;

    if (!multiNoteMenuEntriesOnly) {
        auto *createNoteAction = noteMenu.addAction(tr("New note"));
        QObject::connect(createNoteAction, &QAction::triggered, _mainWindow,
                         &MainWindow::on_action_New_note_triggered);

        renameAction = noteMenu.addAction(tr("Rename note"));
        renameAction->setToolTip(
            tr("Allows you to rename the filename of "
               "the note"));
    }

    // Set dynamic remove action text based on selection
    QString removeActionText;
    if (hasNotes && hasFolders) {
        removeActionText = tr("&Remove notes and folders");
    } else if (hasFolders) {
        removeActionText = tr("&Remove folders");
    } else {
        removeActionText = tr("&Remove notes");
    }
    auto *removeAction = noteMenu.addAction(removeActionText);
    noteMenu.addSeparator();

    const QList<NoteFolder> noteFolders = NoteFolder::fetchAll();

    // show copy and move menu entries only if there
    // is at least one other note folder
    QMenu *moveDestinationMenu = nullptr;
    QMenu *copyDestinationMenu = nullptr;
    if (noteFolders.count() > 1) {
        moveDestinationMenu = noteMenu.addMenu(tr("&Move notes to…"));
        copyDestinationMenu = noteMenu.addMenu(tr("&Copy notes to…"));

        for (const NoteFolder &noteFolder : noteFolders) {
            // don't show not existing folders or if path is empty
            if (!noteFolder.localPathExists() || noteFolder.isCurrent()) {
                continue;
            }

            if (noteFolder.isShowSubfolders()) {
                auto *subFolderMoveMenu = moveDestinationMenu->addMenu(noteFolder.getName());
                _mainWindow->buildBulkNoteFolderSubFolderMenuTree(subFolderMoveMenu, false,
                                                                  noteFolder.getLocalPath());

                auto *subFolderCopyMenu = copyDestinationMenu->addMenu(noteFolder.getName());
                _mainWindow->buildBulkNoteFolderSubFolderMenuTree(subFolderCopyMenu, true,
                                                                  noteFolder.getLocalPath());
            } else {
                auto *moveAction = moveDestinationMenu->addAction(noteFolder.getName());
                moveAction->setData(noteFolder.getLocalPath());
                moveAction->setToolTip(noteFolder.getLocalPath());
                moveAction->setStatusTip(noteFolder.getLocalPath());

                auto *copyAction = copyDestinationMenu->addAction(noteFolder.getName());
                copyAction->setData(noteFolder.getLocalPath());
                copyAction->setToolTip(noteFolder.getLocalPath());
                copyAction->setStatusTip(noteFolder.getLocalPath());
            }
        }
    }

    QAction *moveToThisSubFolderAction = nullptr;
    const bool showSubFolders = NoteFolder::isCurrentShowSubfolders();
    const bool isEnableNoteTree = Utils::Misc::isEnableNoteTree();
    if (showSubFolders || isEnableNoteTree) {
        if (_ui->noteTreeWidget->selectedItems().count() == 1 && !isEnableNoteTree) {
            moveToThisSubFolderAction = noteMenu.addAction(tr("Jump to the note's subfolder"));
        }

        auto *subFolderMoveMenu = noteMenu.addMenu(tr("Move notes to subfolder…"));
        _mainWindow->buildBulkNoteSubFolderMenuTree(subFolderMoveMenu, false);

        auto *subFolderCopyMenu = noteMenu.addMenu(tr("Copy notes to subfolder…"));
        _mainWindow->buildBulkNoteSubFolderMenuTree(subFolderCopyMenu, true);
    }

    int tagCount = Tag::countAll();

    // show the tagging menu if at least one tag is present
    if (tagCount) {
        auto *tagMenu = noteMenu.addMenu(tr("&Tag selected notes with…"));
        _mainWindow->buildBulkNoteTagMenuTree(tagMenu);
    }

    QStringList noteNameList;
    for (QTreeWidgetItem *item : selectedItems) {
        // Only process note items, not folders
        if (item->data(0, Qt::UserRole + 1).toInt() != MainWindow::NoteType) {
            continue;
        }

        // the note names are not unique anymore but the note subfolder
        // path will be taken into account in
        // Tag::fetchAllWithLinkToNoteNames
        const QString name = item->text(0);
        const Note note = Note::fetchByName(name);
        if (note.isFetched()) {
            noteNameList << note.getName();
        }
    }

    const QVector<Tag> tagRemoveList = Tag::fetchAllWithLinkToNoteNames(noteNameList);

    // show the remove tags menu if at least one tag is present
    QMenu *tagRemoveMenu = nullptr;
    if (tagRemoveList.count() > 0) {
        tagRemoveMenu = noteMenu.addMenu(tr("&Remove tag from selected notes…"));

        for (const Tag &tag : tagRemoveList) {
            auto *action = tagRemoveMenu->addAction(tag.getName());
            action->setData(tag.getId());
            action->setToolTip(tag.getName());
            action->setStatusTip(tag.getName());
        }
    }

    QAction *openInExternalEditorAction = nullptr;
    QAction *openNoteWindowAction = nullptr;
    QAction *openNoteInNextcloudFilesAction = nullptr;
    QAction *openNoteInNextcloudNotesAction = nullptr;
    QAction *showInFileManagerAction = nullptr;
    QAction *showNoteGitLogAction = nullptr;
    QAction *copyNotePathToClipboardAction = nullptr;
    QAction *copyNoteFileNameToClipboardAction = nullptr;
    QAction *toggleFavoriteAction = nullptr;

    if (!multiNoteMenuEntriesOnly) {
        noteMenu.addSeparator();
    }

    if ((multiNoteMenuEntriesOnly && selectedItems.count() > 1) || !multiNoteMenuEntriesOnly) {
        auto *openNoteInTabAction = noteMenu.addAction(tr("Open selected notes in tabs"));
        QObject::connect(openNoteInTabAction, &QAction::triggered, _mainWindow,
                         &MainWindow::openSelectedNotesInTab);
    }

    if (!multiNoteMenuEntriesOnly) {
        openInExternalEditorAction = noteMenu.addAction(tr("Open note in external editor"));
        openNoteWindowAction = noteMenu.addAction(tr("Open note in different window"));
        if (OwnCloudService::isOwnCloudSupportEnabled()) {
            openNoteInNextcloudFilesAction = noteMenu.addAction(tr("Open note in Nextcloud Files"));
            openNoteInNextcloudNotesAction = noteMenu.addAction(tr("Open note in Nextcloud Notes"));
        };
        showInFileManagerAction = noteMenu.addAction(tr("Show note in file manager"));
        copyNotePathToClipboardAction = noteMenu.addAction(tr("Copy absolute path of note"));
        copyNoteFileNameToClipboardAction = noteMenu.addAction(tr("Copy note filename"));

        showNoteGitLogAction = new QAction(_mainWindow);
        if (Utils::Git::isCurrentNoteFolderUseGit() && Utils::Git::hasLogCommand()) {
            showNoteGitLogAction = noteMenu.addAction(tr("Show note git versions"));
        }

        // Add favorite toggle action for single note
        noteMenu.addSeparator();
        const Note note = _mainWindow->getCurrentNote();
        if (note.isFetched()) {
            if (note.isFavorite()) {
                toggleFavoriteAction = noteMenu.addAction(tr("Unmark as favorite"));
            } else {
                toggleFavoriteAction = noteMenu.addAction(tr("Mark as favorite"));
            }
        }
    }

    // add the custom actions to the context menu
    if (!_mainWindow->_noteListContextMenuActions.isEmpty()) {
        noteMenu.addSeparator();

        for (QAction *action : Utils::asConst(_mainWindow->_noteListContextMenuActions)) {
            noteMenu.addAction(action);
        }
    }

    QAction *selectAllAction = nullptr;
    if (!multiNoteMenuEntriesOnly) {
        noteMenu.addSeparator();
        selectAllAction = noteMenu.addAction(tr("Select &all notes"));
    }

    QAction *selectedItem = noteMenu.exec(globalPos);
    if (selectedItem) {
        if (selectedItem->parent() == moveDestinationMenu) {
            // move notes
            const QString destinationFolder = selectedItem->data().toString();
            _mainWindow->moveSelectedNotesToFolder(destinationFolder);
        } else if (selectedItem->parent() == copyDestinationMenu) {
            // copy notes
            const QString destinationFolder = selectedItem->data().toString();
            _mainWindow->copySelectedNotesToFolder(destinationFolder);
        } else if (selectedItem->parent() == tagRemoveMenu) {
            // remove tag from notes
            const Tag tag = Tag::fetch(selectedItem->data().toInt());

            if (tag.isFetched()) {
                _mainWindow->removeTagFromSelectedNotes(tag);
            }
        } else if (selectedItem == removeAction) {
            // remove notes
            _mainWindow->removeSelectedNotes();
        } else if (selectedItem == moveToThisSubFolderAction) {
            const int subFolderId = _mainWindow->getCurrentNote().getNoteSubFolderId();
            if (NoteSubFolder::activeNoteSubFolderId() != subFolderId) {
                _mainWindow->jumpToNoteSubFolder(subFolderId);
            }
        } else if (selectedItem == selectAllAction) {
            // select all notes
            _mainWindow->selectAllNotes();
        } else if (selectedItem == openInExternalEditorAction) {
            // open the current note in an external editor
            _mainWindow->on_action_Open_note_in_external_editor_triggered();
        } else if (selectedItem == openNoteWindowAction) {
            // open the current note in a dialog
            _mainWindow->on_actionView_note_in_new_window_triggered();
        } else if (selectedItem == openNoteInNextcloudFilesAction) {
            auto fileUrl = _mainWindow->currentNote.getNextcloudFileLink();
            qDebug() << __func__ << "fileUrl: " << fileUrl;
            QDesktopServices::openUrl(fileUrl);
        } else if (selectedItem == openNoteInNextcloudNotesAction) {
            auto url = _mainWindow->currentNote.getNextcloudNotesLink();
            qDebug() << __func__ << "url: " << url;
            QDesktopServices::openUrl(url);
        } else if (selectedItem == showInFileManagerAction) {
            // show the current note in the file manager
            _mainWindow->on_actionShow_note_in_file_manager_triggered();
        } else if (selectedItem == copyNotePathToClipboardAction) {
            _mainWindow->on_actionCopy_path_to_note_to_clipboard_triggered();
        } else if (selectedItem == copyNoteFileNameToClipboardAction) {
            _mainWindow->copyNoteFilenameToClipboardTriggered();
        } else if (selectedItem == showNoteGitLogAction) {
            // show the git log of the current note
            _mainWindow->on_actionShow_note_git_versions_triggered();
        } else if (selectedItem == toggleFavoriteAction) {
            // toggle favorite status of the current note
            _mainWindow->currentNote.toggleFavorite();
            // Reload the note list to update the icon and sorting
            _mainWindow->loadNoteDirectoryList();
        } else if (selectedItem == renameAction) {
            QTreeWidgetItem *item = _ui->noteTreeWidget->currentItem();

            if (Note::allowDifferentFileName()) {
                if (Utils::Misc::isNoteListPreview()) {
                    bool ok{};
                    const QString name = QInputDialog::getText(
                        _mainWindow, tr("Rename note"), tr("Name:"), QLineEdit::Normal,
                        _mainWindow->currentNote.getName(), &ok);

                    if (ok && !name.isEmpty()) {
                        item->setText(0, name);
                        on_noteTreeWidget_itemChanged(item, 0);
                    }
                } else {
                    _ui->noteTreeWidget->editItem(item);
                }
            } else {
                QMessageBox msgBox(QMessageBox::Warning, tr("Note renaming not enabled!"),
                                   tr("If you want to rename your note you have to enable "
                                      "the option to allow the note filename to be "
                                      "different from the headline."),
                                   QMessageBox::NoButton, _mainWindow);
                QPushButton *settingsButton =
                    msgBox.addButton(tr("Open &settings"), QMessageBox::AcceptRole);
                msgBox.addButton(tr("&Cancel"), QMessageBox::RejectRole);
                msgBox.setDefaultButton(settingsButton);
                msgBox.exec();

                if (msgBox.clickedButton() == settingsButton) {
                    _mainWindow->openSettingsDialog(SettingsDialog::NoteFolderPage);
                }
            }
        }
    }
}

/**
 * Renames a note file if the note was renamed in the note tree widget
 */
void NoteTreeManager::on_noteTreeWidget_itemChanged(QTreeWidgetItem *item, int /*column*/) {
    if (item == nullptr) {
        return;
    }

    // handle note subfolder renaming in a note tree
    if (item->data(0, Qt::UserRole + 1) == MainWindow::FolderType) {
        _ui->noteSubFolderTreeWidget->renameSubFolder(item);
        return;
    }

    if (!Note::allowDifferentFileName()) {
        return;
    }

    const int noteId = item->data(0, Qt::UserRole).toInt();
    Note note = Note::fetch(noteId);
    if (note.isFetched()) {
        qDebug() << __func__ << " - 'note': " << note;

        const QSignalBlocker blocker(_mainWindow->noteDirectoryWatcher);
        Q_UNUSED(blocker)

        const Note oldNote = note;
        const QString oldNoteName = note.getName();

        if (note.renameNoteFile(item->text(0))) {
            QString newNoteName = note.getName();

            if (oldNoteName != newNoteName) {
                note.refetch();
                _mainWindow->setCurrentNote(note);

                // rename the note file names of note tag links
                Tag::renameNoteFileNamesOfLinks(oldNoteName, newNoteName, note.getNoteSubFolder());

                // handle the replacing of all note urls if a note was renamed
                if (note.handleNoteMoving(oldNote)) {
                    // reload the current note if we had to change it
                    _mainWindow->reloadCurrentNoteByNoteId(true);
                }

                // reload the directory list if note name has changed
                //                loadNoteDirectoryList();

                // sort notes if note name has changed
                SettingsService settings;
                if (settings.value(QStringLiteral("notesPanelSort"), SORT_BY_LAST_CHANGE).toInt() ==
                    SORT_ALPHABETICAL) {
                    _ui->noteTreeWidget->sortItems(
                        0, Utils::Gui::toQtOrder(
                               settings.value(QStringLiteral("notesPanelOrder")).toInt()));
                    _ui->noteTreeWidget->scrollToItem(item);
                }

                // update the note list tooltip of the note
                Utils::Gui::setTreeWidgetItemToolTipForNote(item, note);
            }
        }

        const QSignalBlocker blocker2(_ui->noteTreeWidget);
        Q_UNUSED(blocker2)

        // set old name back in case the renaming failed or the file name got
        // altered in the renaming process
        item->setText(0, note.getName());

        if (Utils::Misc::isNoteListPreview()) {
            updateNoteTreeWidgetItem(note, item);
        }
    }
}

/**
 * Double-clicking a note calls a hook
 */
void NoteTreeManager::on_noteTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column) {
    Q_UNUSED(item)
    Q_UNUSED(column)

    // call a script hook that a new note was double-clicked
    const bool hookFound =
        ScriptingService::instance()->callHandleNoteDoubleClickedHook(&_mainWindow->currentNote);

    if (!hookFound) {
        _mainWindow->openCurrentNoteInTab();
    }
}

/**
 * Reloads the current note (and selected notes) tags if there were selected
 * multiple notes
 */
void NoteTreeManager::on_noteTreeWidget_itemSelectionChanged() {
    qDebug() << __func__;
    const auto &selectedItems = _ui->noteTreeWidget->selectedItems();
    if (selectedItems.size() == 1) {
        // Don't tread folders as notes
        if (selectedItems[0]->data(0, Qt::UserRole + 1).toInt() == MainWindow::FolderType) {
            return;
        }

        int noteId = selectedItems[0]->data(0, Qt::UserRole).toInt();
        Note note = Note::fetch(noteId);
        bool currentNoteChanged = _mainWindow->currentNote.getId() != noteId;
        _mainWindow->setCurrentNote(std::move(note), true, false);

        // Let's highlight the text from the search line edit and do an "in-note
        // search" if the current note has changed and there is a search term
        // in the search line edit
        if (currentNoteChanged && !_ui->searchLineEdit->text().isEmpty()) {
            _mainWindow->searchForSearchLineTextInNoteTextEdit();

            // prevent that the last occurrence of the search term is found
            // first, instead the first occurrence should be found first
            _ui->noteTextEdit->searchWidget()->doSearchDown();
            _ui->noteTextEdit->searchWidget()->updateSearchExtraSelections();
        }
    }

    // we also need to do this in setCurrentNote because of different timings
    _mainWindow->reloadCurrentNoteTags();
}

void NoteTreeManager::on_noteOperationsButton_clicked() {
    QPoint globalPos =
        _ui->noteOperationsButton->mapToGlobal(QPoint(0, _ui->noteOperationsButton->height()));
    openNotesContextMenu(globalPos, true);
}
