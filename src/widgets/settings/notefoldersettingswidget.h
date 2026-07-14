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

#include <QListWidget>
#include <QStatusBar>
#include <QTreeWidgetItem>
#include <QWidget>

#include "entities/cloudconnection.h"
#include "entities/notefolder.h"

namespace Ui {
class NoteFolderSettingsWidget;
}

class NoteFolderSettingsWidget : public QWidget {
    Q_OBJECT

   public:
    explicit NoteFolderSettingsWidget(QWidget *parent = nullptr);
    ~NoteFolderSettingsWidget();

    void initialize();
    void readSettings();
    void storeSettings();

    void setNoteFolderRemotePathList(QStringList pathList);
    void populateCloudConnectionComboBox(const QList<CloudConnection> &connections, int selectedId);

   signals:
    void storeSettingsRequested();

   private slots:
    void on_noteFolderListWidget_currentItemChanged(QListWidgetItem *current,
                                                    QListWidgetItem *previous);
    void on_noteFolderAddButton_clicked();
    void on_noteFolderRemoveButton_clicked();
    void on_noteFolderNameLineEdit_editingFinished();
    void on_noteFolderRemotePathLineEdit_editingFinished();
    void on_noteFolderLocalPathButton_clicked();
    void on_noteFolderActiveCheckBox_stateChanged(int arg1);
    void on_noteFolderRemotePathButton_clicked();
    void on_noteFolderRemotePathTreeWidget_currentItemChanged(QTreeWidgetItem *current,
                                                              QTreeWidgetItem *previous);
    void on_noteFolderCloudConnectionComboBox_currentIndexChanged(int index);
    void on_useCloudPathButton_clicked();
    void on_noteFolderShowSubfoldersCheckBox_toggled(bool checked);
    void on_noteFolderAllSubfoldersCheckBox_toggled(bool checked);
    void on_allowDifferentNoteFileNameCheckBox_toggled(bool checked);
    void on_noteFolderGitCommitCheckBox_toggled(bool checked);
    void on_noteFolderListWidget_itemChanged(QListWidgetItem *item);
    void onSubfolderTreeItemChanged(QTreeWidgetItem *item, int column);
    void saveSubfolderTreeSelection();

   private:
    Ui::NoteFolderSettingsWidget *ui;
    NoteFolder _selectedNoteFolder;
    QStatusBar *_noteFolderRemotePathTreeStatusBar = nullptr;
    bool _updatingSubfolderTreeCheckStates = false;

    void setNoteFolderRemotePathTreeWidgetFrameVisibility(bool visible);
    void addPathToNoteFolderRemotePathTreeWidget(QTreeWidgetItem *parent, const QString &path);
    QTreeWidgetItem *findNoteFolderRemotePathTreeWidgetItem(QTreeWidgetItem *parent,
                                                            const QString &text);
    QString generatePathFromCurrentNoteFolderRemotePathItem(QTreeWidgetItem *item);
    void updateSubfolderVisibility();
    void updateNoteFolderListActiveState();
    void populateSubfolderTree();
    void populateSubfolderTreeFromDir(QTreeWidgetItem *parentItem, const QString &path,
                                      const QString &relativePath);
    void applySubfolderTreeCheckStates(QTreeWidget *tree, const QStringList &excludedPaths);
    void applyCheckStateToItem(QTreeWidgetItem *item, const QStringList &excludedPaths);
    void setSubfolderTreeChildrenCheckState(QTreeWidgetItem *item, Qt::CheckState checkState);
    void updateSubfolderTreeParentCheckStates(QTreeWidgetItem *item);
    Qt::CheckState subfolderTreeParentCheckState(QTreeWidgetItem *item);
    void collectExcludedSubfolderPaths(QTreeWidgetItem *item, QStringList &excludedPaths);
};
