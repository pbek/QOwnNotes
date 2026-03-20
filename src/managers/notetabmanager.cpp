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

#include "notetabmanager.h"

#include <entities/note.h>
#include <services/settingsservice.h>
#include <utils/gui.h>

#include <QMenu>
#include <QTabBar>
#include <QTreeWidget>
#include <QVBoxLayout>

#include "mainwindow.h"
#include "ui_mainwindow.h"

NoteTabManager::NoteTabManager(MainWindow *mainWindow, Ui::MainWindow *ui, QObject *parent)
    : QObject(parent), _mainWindow(mainWindow), _ui(ui) {}

void NoteTabManager::updateCurrentTabData(const Note &note) const {
    Utils::Gui::updateTabWidgetTabData(_ui->noteEditTabWidget,
                                       _ui->noteEditTabWidget->currentIndex(), note);
}

void NoteTabManager::closeOrphanedTabs() const {
    const int maxIndex = _ui->noteEditTabWidget->count() - 1;

    for (int i = maxIndex; i >= 0; i--) {
        const int noteId = Utils::Gui::getTabWidgetNoteId(_ui->noteEditTabWidget, i);

        if (!Note::noteIdExists(noteId)) {
            removeNoteTab(i);
        }
    }
}

bool NoteTabManager::jumpToTab(const Note &note) const {
    const int noteId = note.getId();
    const int tabIndexOfNote = getNoteTabIndex(noteId);

    if (tabIndexOfNote == -1) {
        return false;
    }

    _ui->noteEditTabWidget->setCurrentIndex(tabIndexOfNote);
    QWidget *widget = _ui->noteEditTabWidget->currentWidget();

    if (widget->layout() == nullptr) {
        widget->setLayout(_ui->noteEditTabWidgetLayout);
        closeOrphanedTabs();
    }

    return true;
}

void NoteTabManager::openSelectedNotesInTab() {
    const auto selectedItems = _ui->noteTreeWidget->selectedItems();
    for (QTreeWidgetItem *item : selectedItems) {
        if (item->data(0, Qt::UserRole + 1) != MainWindow::NoteType) {
            continue;
        }

        const int noteId = item->data(0, Qt::UserRole).toInt();
        Note note = Note::fetch(noteId);

        if (!note.isFetched()) {
            continue;
        }

        //        setCurrentNote(note);
        //        openCurrentNoteInTab();

        openNoteInTab(note);
    }
}

void NoteTabManager::openNoteInTab(const Note &note, bool forceNewTab) {
    // Simulate a newly opened tab by updating the current tab with the last note
    if (_lastNoteId > 0) {
        auto previousNote = Note::fetch(_lastNoteId);

        // Open the previous note in a new tab only if it is not already open in a tab
        if (previousNote.isFetched() && getNoteTabIndex(_lastNoteId) == -1) {
            updateCurrentTabData(previousNote);
        }
    }

    const QString &noteName = note.getName();
    const int noteId = note.getId();

    // If forceNewTab is true, always create a new tab (set tabIndex to -1)
    // Otherwise, check if the note is already open in a tab
    int tabIndex = forceNewTab ? -1 : getNoteTabIndex(noteId);

    if (tabIndex == -1) {
        auto *widgetPage = new QWidget();
        widgetPage->setLayout(_ui->noteEditTabWidgetLayout);
        tabIndex = _ui->noteEditTabWidget->addTab(widgetPage, noteName);
    }

    Utils::Gui::updateTabWidgetTabData(_ui->noteEditTabWidget, tabIndex, note);

    _ui->noteEditTabWidget->setCurrentIndex(tabIndex);

    // remove the tab initially created by the ui file
    if (_ui->noteEditTabWidget->widget(0)->property("note-id").isNull()) {
        _ui->noteEditTabWidget->removeTab(0);
    }
}

void NoteTabManager::openCurrentNoteInTab() {
    // simulate a newly opened tab by updating the current tab with the last note
    if (_lastNoteId > 0) {
        auto previousNote = Note::fetch(_lastNoteId);

        // open the previous note in a new tab only if it is not already open in a tab
        if (previousNote.isFetched() && getNoteTabIndex(_lastNoteId) == -1) {
            updateCurrentTabData(previousNote);
        }
    }

    Note &currentNote = _mainWindow->currentNote;
    const QString &noteName = currentNote.getName();
    const int noteId = currentNote.getId();
    int tabIndex = getNoteTabIndex(noteId);

    if (tabIndex == -1) {
        auto *widgetPage = new QWidget();
        widgetPage->setLayout(_ui->noteEditTabWidgetLayout);
        tabIndex = _ui->noteEditTabWidget->addTab(widgetPage, noteName);
    }

    Utils::Gui::updateTabWidgetTabData(_ui->noteEditTabWidget, tabIndex, currentNote);

    _ui->noteEditTabWidget->setCurrentIndex(tabIndex);

    // remove the tab initially created by the ui file
    if (_ui->noteEditTabWidget->widget(0)->property("note-id").isNull()) {
        _ui->noteEditTabWidget->removeTab(0);
    }
}

int NoteTabManager::getNoteTabIndex(int noteId) const {
    return Utils::Gui::getTabWidgetIndexByProperty(_ui->noteEditTabWidget,
                                                   QStringLiteral("note-id"), noteId);
}

/**
 * Returns a list of note ids that are opened in tabs
 */
QList<int> NoteTabManager::getNoteTabNoteIdList() const {
    QList<int> resultList;

    for (int i = 0; i < _ui->noteEditTabWidget->count(); i++) {
        auto widget = _ui->noteEditTabWidget->widget(i);
        const int noteId = widget->property("note-id").toInt();
        resultList.append(noteId);
    }

    return resultList;
}

void NoteTabManager::on_noteEditTabWidget_currentChanged(int index) {
    QWidget *widget = _ui->noteEditTabWidget->currentWidget();

    if (widget == nullptr) {
        return;
    }

    const int noteId = widget->property("note-id").toInt();

    // close the tab if note doesn't exist anymore
    if (!Note::noteIdExists(noteId)) {
        removeNoteTab(index);
        return;
    }

    // Allow the subfolder of the note to be selected in the subfolder list
    // See: https://github.com/pbek/QOwnNotes/issues/2861
    if (SettingsService()
            .value(QStringLiteral("noteSubfoldersPanelTabsUnsetAllNotesSelection"))
            .toBool()) {
        _mainWindow->setShowNotesFromAllNoteSubFolders(false);
    }

    _mainWindow->setCurrentNoteFromNoteId(noteId);
    widget->setLayout(_ui->noteEditTabWidgetLayout);

    closeOrphanedTabs();
}

void NoteTabManager::on_noteEditTabWidget_tabCloseRequested(int index) { removeNoteTab(index); }

void NoteTabManager::on_actionPrevious_note_tab_triggered() {
    int index = _ui->noteEditTabWidget->currentIndex() - 1;

    if (index < 0) {
        index = _ui->noteEditTabWidget->count() - 1;
    }

    _ui->noteEditTabWidget->setCurrentIndex(index);
    _mainWindow->focusNoteTextEdit();
}

void NoteTabManager::on_actionNext_note_tab_triggered() {
    int index = _ui->noteEditTabWidget->currentIndex() + 1;

    if (index >= _ui->noteEditTabWidget->count()) {
        index = 0;
    }

    _ui->noteEditTabWidget->setCurrentIndex(index);
    _mainWindow->focusNoteTextEdit();
}

void NoteTabManager::on_actionClose_current_note_tab_triggered() {
    removeNoteTab(_ui->noteEditTabWidget->currentIndex());
}

void NoteTabManager::on_actionNew_note_in_new_tab_triggered() {
    _mainWindow->on_action_New_note_triggered();
    openCurrentNoteInTab();
}

/**
 * Close a note tab on a specific index.
 * @param index The index of the tab to close.
 */
bool NoteTabManager::removeNoteTab(int index) const {
    const int maxIndex = _ui->noteEditTabWidget->count() - 1;

    if (maxIndex <= 0 || index > maxIndex) {
        return false;
    }

    _ui->noteEditTabWidget->removeTab(index);
    return true;
}

void NoteTabManager::on_noteEditTabWidget_tabBarDoubleClicked(int index) {
    // If the empty area of the tab widget is clicked, open a new note in a new tab
    if (index == -1) {
        _mainWindow->createNewNote();
        openCurrentNoteInTab();
        return;
    }

    // Make the note tab "sticky"
    Utils::Gui::setTabWidgetTabSticky(
        _ui->noteEditTabWidget, index,
        !Utils::Gui::isTabWidgetTabSticky(_ui->noteEditTabWidget, index));
}

void NoteTabManager::on_actionToggle_note_stickiness_of_current_tab_triggered() {
    on_noteEditTabWidget_tabBarDoubleClicked(_ui->noteEditTabWidget->currentIndex());
}

/**
 * If the tab of current note was clicked now the subfolder of the note is
 * activated if that is needed to show the note in the note list
 */
void NoteTabManager::on_noteEditTabWidget_tabBarClicked(int index) {
    if (_ui->noteEditTabWidget->currentIndex() != index) {
        return;
    }

    if (!_mainWindow->showNotesFromAllNoteSubFolders() &&
        !_mainWindow->currentNote.isInCurrentNoteSubFolder()) {
        _mainWindow->jumpToNoteSubFolder(_mainWindow->currentNote.getNoteSubFolderId());
    }
}

/**
 * Note tab context menu
 */
void NoteTabManager::showNoteEditTabWidgetContextMenu(const QPoint &point) {
    if (point.isNull()) {
        return;
    }

    int tabIndex = _ui->noteEditTabWidget->tabBar()->tabAt(point);
    auto *menu = new QMenu();

    // Toggle note stickiness
    auto *stickAction = menu->addAction(tr("Toggle note stickiness"));
    connect(stickAction, &QAction::triggered, this,
            [this, tabIndex]() { on_noteEditTabWidget_tabBarDoubleClicked(tabIndex); });

    // Close other note tabs
    auto *closeOtherAction = menu->addAction(tr("Close other note tabs"));
    connect(closeOtherAction, &QAction::triggered, this, [this, tabIndex]() {
        const int maxIndex = _ui->noteEditTabWidget->count() - 1;
        const int keepNoteId = Utils::Gui::getTabWidgetNoteId(_ui->noteEditTabWidget, tabIndex);

        for (int i = maxIndex; i >= 0; i--) {
            const int noteId = Utils::Gui::getTabWidgetNoteId(_ui->noteEditTabWidget, i);

            if (noteId != keepNoteId) {
                removeNoteTab(i);
            }
        }
    });

    // Close note tab
    auto *closeAction = menu->addAction(tr("Close note tab"));
    connect(closeAction, &QAction::triggered, this,
            [this, tabIndex]() { removeNoteTab(tabIndex); });

    menu->exec(_ui->noteEditTabWidget->tabBar()->mapToGlobal(point));
}
