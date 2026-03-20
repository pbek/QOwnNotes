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

namespace Ui {
class MainWindow;
}

class NoteIndexManager : public QObject {
    Q_OBJECT

   public:
    explicit NoteIndexManager(MainWindow *mainWindow, Ui::MainWindow *ui,
                              QObject *parent = nullptr);

    bool buildNotesIndex(int noteSubFolderId = 0, bool forceRebuild = false);
    void buildNotesIndexAndLoadNoteDirectoryList(bool forceBuild = false, bool forceLoad = false,
                                                 bool reloadTabs = true);
    void loadNoteDirectoryList();
    void updateNoteDirectoryWatcher();
    void clearNoteDirectoryWatcher();
    void removeConflictedNotesDatabaseCopies();
    void storeUpdatedNotesToDisk();
    void frequentPeriodicChecker();
    void directoryWatcherWorkaround(bool isNotesDirectoryWasModifiedDisabled,
                                    bool alsoHandleNotesWereModified = false);
    bool noteDirectoryWatcherAddPath(const QString &path);
    void addDirectoryToDirectoryWatcher(const QString &path);
    void connectFileWatcher(bool delayed = false);

   public slots:
    void notesWereModified(const QString &str);
    void notesDirectoryWasModified(const QString &str);

   private:
    MainWindow *_mainWindow;
    Ui::MainWindow *_ui;
};
