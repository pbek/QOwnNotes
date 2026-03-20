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

#include "mainwindow.h"

class MainWindow;
class QMenu;

namespace Ui {
class MainWindow;
}

class NoteSubFolder;

class NoteOperationsManager : public QObject {
    Q_OBJECT

   public:
    explicit NoteOperationsManager(MainWindow *mainWindow, Ui::MainWindow *ui,
                                   QObject *parent = nullptr);

    void removeCurrentNote();
    void removeSelectedNotes();
    void moveSelectedNotesToFolder(const QString &destinationFolder);
    void copySelectedNotesToFolder(const QString &destinationFolder,
                                   const QString &noteFolderPath = QString());
    void createNewNote(QString noteName, bool withNameAppend = true);
    void createNewNote(
        QString name, QString text,
        MainWindow::CreateNewNoteOptions options = MainWindow::CreateNewNoteOption::None);
    void moveSelectedNotesToNoteSubFolder(const NoteSubFolder &noteSubFolder);
    void copySelectedNotesToNoteSubFolder(const NoteSubFolder &noteSubFolder);
    void moveSelectedNotesToNoteSubFolderId(int noteSubFolderId);
    void copySelectedNotesToNoteSubFolderId(int noteSubFolderId);
    void buildBulkNoteSubFolderMenuTree(QMenu *parentMenu, bool doCopy,
                                        int parentNoteSubFolderId = 0);
    void buildBulkNoteFolderSubFolderMenuTree(QMenu *parentMenu, bool doCopy,
                                              const QString &parentNoteSubFolderPath,
                                              bool isRoot = true);

   public slots:
    void on_actionSplit_note_at_cursor_position_triggered();
    void on_action_Remove_note_triggered();
    void on_action_New_note_triggered();

   private:
    MainWindow *_mainWindow;
    Ui::MainWindow *_ui;
};
