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

#include <QHash>
#include <QObject>
#include <QString>
#include <QStringList>

class MainWindow;
class QComboBox;
class QDockWidget;

namespace Ui {
class MainWindow;
}

class WorkspaceManager : public QObject {
    Q_OBJECT

   public:
    explicit WorkspaceManager(MainWindow *mainWindow, Ui::MainWindow *ui,
                              QObject *parent = nullptr);

    void initWorkspaceComboBox();
    void updateWorkspaceLists(bool rebuild = true);
    void setCurrentWorkspace(const QString &uuid);
    void storeCurrentWorkspace();
    void restoreCurrentWorkspace();
    bool createNewWorkspace(QString name);
    QString currentWorkspaceUuid();
    QStringList getWorkspaceUuidList();
    QString getWorkspaceUuid(const QString &workspaceName);

    QComboBox *workspaceComboBox() const { return _workspaceComboBox; }

    // Panel management
    void handleDockWidgetLocking(QDockWidget *dockWidget);

   public slots:
    void onWorkspaceComboBoxCurrentIndexChanged(int index);
    void on_actionStore_as_new_workspace_triggered();
    void on_actionRemove_current_workspace_triggered();
    void on_actionRename_current_workspace_triggered();
    void on_actionSwitch_to_previous_workspace_triggered();
    void on_actionUnlock_panels_toggled(bool arg1);
    void on_actionShow_all_panels_triggered();
    void on_actionReattach_panels_triggered();

   private:
    MainWindow *_mainWindow;
    Ui::MainWindow *_ui;
    QComboBox *_workspaceComboBox = nullptr;
    QHash<QString, QString> _workspaceNameUuidMap;
};
