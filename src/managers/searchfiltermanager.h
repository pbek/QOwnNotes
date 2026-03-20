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

class SearchFilterManager : public QObject {
    Q_OBJECT

   public:
    explicit SearchFilterManager(MainWindow *mainWindow, Ui::MainWindow *ui,
                                 QObject *parent = nullptr);

    void filterNotes(bool searchForText = true);
    void filterNotesBySearchLineEditText(bool searchInNote = true);
    void filterNotesByTag();
    void filterNotesByNoteSubFolders();
    void searchInNoteTextEdit(QString str);
    void doSearchInNote(QString searchText);
    void searchForSearchLineTextInNoteTextEdit();
    void jumpToNoteOrCreateNew(bool disableLoadNoteDirectoryList = false);
    void storeSavedSearch();
    void initSavedSearchesCompleter();

   public slots:
    void on_actionFind_notes_in_all_subfolders_triggered();
    void on_actionReplace_in_current_note_triggered();
    void on_action_Find_note_triggered();
    void on_searchLineEdit_textChanged(const QString &arg1);
    void on_searchLineEdit_returnPressed();
    void on_action_Find_text_in_note_triggered();

   private:
    MainWindow *_mainWindow;
    Ui::MainWindow *_ui;
};
