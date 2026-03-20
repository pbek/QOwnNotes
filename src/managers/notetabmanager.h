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

#include <QObject>

class MainWindow;
class Note;
class QPoint;

namespace Ui {
class MainWindow;
}

class NoteTabManager : public QObject {
    Q_OBJECT

   public:
    explicit NoteTabManager(MainWindow *mainWindow, Ui::MainWindow *ui, QObject *parent = nullptr);

    void updateCurrentTabData(const Note &note) const;
    void closeOrphanedTabs() const;
    bool jumpToTab(const Note &note) const;
    int getNoteTabIndex(int noteId) const;
    QList<int> getNoteTabNoteIdList() const;
    bool removeNoteTab(int index) const;

   public slots:
    void openSelectedNotesInTab();
    void openNoteInTab(const Note &note, bool forceNewTab = false);
    void openCurrentNoteInTab();
    void on_noteEditTabWidget_currentChanged(int index);
    void on_noteEditTabWidget_tabCloseRequested(int index);
    void on_actionPrevious_note_tab_triggered();
    void on_actionNext_note_tab_triggered();
    void on_actionClose_current_note_tab_triggered();
    void on_actionNew_note_in_new_tab_triggered();
    void on_noteEditTabWidget_tabBarDoubleClicked(int index);
    void on_actionToggle_note_stickiness_of_current_tab_triggered();
    void on_noteEditTabWidget_tabBarClicked(int index);
    void showNoteEditTabWidgetContextMenu(const QPoint &point);

   private:
    MainWindow *_mainWindow;
    Ui::MainWindow *_ui;
    int _lastNoteId = 0;

    friend class MainWindow;
};
