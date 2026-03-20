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
#include <QPoint>
#include <QVector>

class MainWindow;
class QTreeWidgetItem;
class Note;

namespace Ui {
class MainWindow;
}

class NoteTreeManager : public QObject {
    Q_OBJECT

   public:
    explicit NoteTreeManager(MainWindow *mainWindow, Ui::MainWindow *ui, QObject *parent = nullptr);

    bool addNoteToNoteTreeWidget(const Note &note, QTreeWidgetItem *parent = nullptr);
    void updateNoteTreeWidgetItem(const Note &note, QTreeWidgetItem *noteItem = nullptr);
    void makeCurrentNoteFirstInNoteList();
    QTreeWidgetItem *findNoteInNoteTreeWidget(const Note &note);
    void removeNoteFromNoteTreeWidget(Note &note) const;
    QTreeWidgetItem *firstVisibleNoteTreeWidgetItem();
    int getSelectedNotesCount() const;
    QVector<Note> selectedNotes();
    void openNotesContextMenu(const QPoint globalPos, bool hasNotes, bool hasFolders = false);

   public slots:
    void on_noteTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_noteTreeWidget_itemChanged(QTreeWidgetItem *item, int column);
    void on_noteTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_noteTreeWidget_itemSelectionChanged();
    void on_noteTreeWidget_customContextMenuRequested(const QPoint pos);
    void on_noteOperationsButton_clicked();

   private:
    MainWindow *_mainWindow;
    Ui::MainWindow *_ui;
};
