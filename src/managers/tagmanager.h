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

class MainWindow;
class QMenu;
class QTreeWidgetItem;
struct TagHeader;

namespace Ui {
class MainWindow;
}

class Tag;
class Note;

class TagManager : public QObject {
    Q_OBJECT

   public:
    explicit TagManager(MainWindow *mainWindow, Ui::MainWindow *ui, QObject *parent = nullptr);

    void reloadTagTree();
    void buildTagTreeForParentItem(QTreeWidgetItem *parent = nullptr, bool topLevel = false);
    QTreeWidgetItem *addTagToTagTreeWidget(QTreeWidgetItem *parent, const TagHeader &tag);
    void setupTags();
    void reloadCurrentNoteTags();
    void highlightCurrentNoteTagsInTagTree();
    void tagSelectedNotes(const Tag &tag);
    void removeTagFromSelectedNotes(const Tag &tag);
    void removeSelectedTags();
    void linkTagNameToCurrentNote(const QString &tagName, bool linkToSelectedNotes = false);
    void removeNoteTagClicked();
    void handleNoteTreeTagColoringForNote(const Note &note);
    void handleScriptingNoteTagging(Note note, const Tag &tag, bool doRemove,
                                    bool triggerPostMethods = true);
    void handleScriptingNotesTagUpdating();
    void handleScriptingNotesTagRenaming(const Tag &tag, const QString &newTagName);
    void handleScriptingNotesTagRemoving(const Tag &tag, bool forBulkOperation = false);
    void resizeTagTreeWidgetColumnToContents() const;
    void assignColorToTagItem(QTreeWidgetItem *item);
    void assignColorToSelectedTagItems();
    void disableColorOfTagItem(QTreeWidgetItem *item);
    void buildTagMoveMenuTree(QMenu *parentMenu, int parentTagId = 0);
    void buildBulkNoteTagMenuTree(QMenu *parentMenu, int parentTagId = 0);
    void moveSelectedTagsToTagId(int tagId);
    void tagSelectedNotesToTagId(int tagId);
    bool jumpToTag(int tagId);
    void storeTagTreeWidgetExpandState() const;
    void clearTagFilteringColumn();

   public slots:
    void on_tagLineEdit_returnPressed();
    void on_tagLineEdit_textChanged(const QString &arg1);
    void on_newNoteTagButton_clicked();
    void on_newNoteTagLineEdit_returnPressed();
    void on_newNoteTagLineEdit_editingFinished();
    void on_tagTreeWidget_itemChanged(QTreeWidgetItem *item, int column);
    void on_tagTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_tagTreeWidget_customContextMenuRequested(const QPoint pos);
    void on_tagTreeWidget_itemSelectionChanged();
    void on_tagTreeWidget_itemCollapsed(QTreeWidgetItem *item);
    void on_tagTreeWidget_itemExpanded(QTreeWidgetItem *item);
    void on_tagTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_action_new_tag_triggered();

   private:
    MainWindow *_mainWindow;
    Ui::MainWindow *_ui;
};
