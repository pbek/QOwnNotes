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

#include "workspacemanager.h"

#include <entities/notefolder.h>
#include <services/metricsservice.h>
#include <services/scriptingservice.h>
#include <services/settingsservice.h>
#include <utils/gui.h>
#include <utils/misc.h>

#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QDebug>
#include <QDockWidget>
#include <QInputDialog>
#include <QKeySequence>
#include <QMenu>
#include <QSignalBlocker>
#include <QTimer>

#include "mainwindow.h"
#include "ui_mainwindow.h"

WorkspaceManager::WorkspaceManager(MainWindow *mainWindow, Ui::MainWindow *ui, QObject *parent)
    : QObject(parent), _mainWindow(mainWindow), _ui(ui) {}

void WorkspaceManager::initWorkspaceComboBox() {
    _workspaceComboBox = new QComboBox(_mainWindow);
    connect(_workspaceComboBox,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            &WorkspaceManager::onWorkspaceComboBoxCurrentIndexChanged);
    _workspaceComboBox->setToolTip(tr("Workspaces"));
    _workspaceComboBox->setObjectName(QStringLiteral("workspaceComboBox"));
}

void WorkspaceManager::updateWorkspaceLists(bool rebuild) {
    SettingsService settings;
    const QStringList workspaces = getWorkspaceUuidList();
    const QString currentUuid = currentWorkspaceUuid();

    if (rebuild) {
        // we need to create a new combo box so the width gets updated in the
        // window toolbar
        initWorkspaceComboBox();

        _ui->menuWorkspaces->clear();

        _workspaceNameUuidMap.clear();
    }

    const QSignalBlocker blocker(_workspaceComboBox);
    Q_UNUSED(blocker)

    int currentIndex = 0;

    for (int i = 0; i < workspaces.count(); i++) {
        const QString &uuid = workspaces.at(i);

        if (uuid == currentUuid) {
            currentIndex = i;
        }

        // check if we want to skip the rebuilding part
        if (!rebuild) {
            continue;
        }

        const QString name =
            settings.value(QStringLiteral("workspace-") + uuid + QStringLiteral("/name"))
                .toString();
        const QString objectName = QStringLiteral("restoreWorkspace-") + uuid;

        _workspaceNameUuidMap.insert(name, uuid);

        _workspaceComboBox->addItem(name, uuid);

        auto *action = new QAction(name, _ui->menuWorkspaces);
        connect(action, &QAction::triggered, this, [this, uuid]() { setCurrentWorkspace(uuid); });

        // set an object name for creating shortcuts
        action->setObjectName(objectName);

        // try to load a key sequence from the settings
        QKeySequence shortcut = QKeySequence(
            settings.value(QStringLiteral("Shortcuts/MainWindow-") + objectName).toString());
        action->setShortcut(shortcut);

        //        if (uuid == currentUuid) {
        //            QFont font = action->font();
        //            font.setBold(true);
        //            action->setFont(font);
        //        }

        _ui->menuWorkspaces->addAction(action);
    }

    _workspaceComboBox->setCurrentIndex(currentIndex);

    if (rebuild) {
        // we need to adapt the width of the workspaces combo box
        _mainWindow->updateWindowToolbar();
    }

    // enable the remove button if there are at least two workspaces
    _ui->actionRemove_current_workspace->setEnabled(workspaces.count() > 1);
}

/**
 * Sets the new current workspace when the workspace combo box index has changed
 */
void WorkspaceManager::onWorkspaceComboBoxCurrentIndexChanged(int index) {
    Q_UNUSED(index)

    const QString uuid = _workspaceComboBox->currentData().toString();

    // set the new workspace
    setCurrentWorkspace(uuid);
}

void WorkspaceManager::setCurrentWorkspace(const QString &uuid) {
    // store the current workspace
    storeCurrentWorkspace();

    SettingsService settings;
    QString currentUuid = currentWorkspaceUuid();
    settings.setValue(QStringLiteral("previousWorkspace"), currentUuid);
    settings.setValue(QStringLiteral("currentWorkspace"), uuid);

    // restore the new workspace
    QTimer::singleShot(0, _mainWindow, SLOT(restoreCurrentWorkspace()));

    // update the menu and combo box (but don't rebuild it)
    updateWorkspaceLists(false);

    // update the preview in case it was disabled previously
    _mainWindow->setNoteTextFromNote(&_mainWindow->currentNote, true);

    ScriptingService::instance()->callWorkspaceSwitchedHook(currentUuid, uuid);
}

/**
 * Stores the current workspace
 */
void WorkspaceManager::storeCurrentWorkspace() {
    const bool forceQuit = qApp->property("clearAppDataAndExit").toBool();
    if (MainWindow::isInDistractionFreeMode() || forceQuit || _mainWindow->closeEventWasFired()) {
        return;
    }

    qDebug() << __func__;
    SettingsService settings;
    QString uuid = currentWorkspaceUuid();

    settings.setValue(QStringLiteral("workspace-") + uuid + QStringLiteral("/windowState"),
                      _mainWindow->saveState());
    settings.setValue(
        QStringLiteral("workspace-") + uuid + QStringLiteral("/noteEditIsCentralWidget"),
        _mainWindow->noteEditIsCentralWidget());
    settings.setValue(
        QStringLiteral("workspace-") + uuid + QStringLiteral("/noteSubFolderDockWidgetVisible"),
        _mainWindow->noteSubFolderDockWidgetVisible());
}

/**
 * Restores the current workspace
 */
void WorkspaceManager::restoreCurrentWorkspace() {
    SettingsService settings;
    QStringList workspaces = getWorkspaceUuidList();
    QWidget *focusWidget = qApp->focusWidget();

    // create a default workspace if there is none yet
    if (workspaces.count() == 0) {
        createNewWorkspace(tr("full", "full workspace"));

        _mainWindow->setDockWidgetVisible("taggingDockWidget", false);
        _mainWindow->setDockWidgetVisible("noteFolderDockWidget", false);
        _mainWindow->setDockWidgetVisible("noteNavigationDockWidget", false);
        _mainWindow->setDockWidgetVisible("noteTagDockWidget", false);
        _mainWindow->setDockWidgetVisible("notePreviewDockWidget", false);
        _mainWindow->setDockWidgetVisible("noteGraphicsViewDockWidget", false);
        createNewWorkspace(tr("minimal", "minimal workspace"));

        // TODO: maybe still create those workspaces initially?
    }

    QString uuid = currentWorkspaceUuid();

    // set the first workspace as current workspace if there is none set
    if (uuid.isEmpty()) {
        workspaces = getWorkspaceUuidList();

        if (workspaces.count() == 0) {
            return;
        }

        uuid = workspaces.at(0);
        settings.setValue(QStringLiteral("currentWorkspace"), uuid);

        // update the menu and combo box
        updateWorkspaceLists();
    }

    const QString noteEditIsCentralWidgetKey =
        QStringLiteral("workspace-") + uuid + QStringLiteral("/noteEditIsCentralWidget");
    const bool noteEditIsCentralWidget =
        settings.contains(noteEditIsCentralWidgetKey)
            ? settings.value(noteEditIsCentralWidgetKey).toBool()
            : settings.value(QStringLiteral("noteEditIsCentralWidget"), true).toBool();
    settings.setValue(QStringLiteral("noteEditIsCentralWidget"), noteEditIsCentralWidget);

    if (_mainWindow->noteEditIsCentralWidget() != noteEditIsCentralWidget) {
        _mainWindow->setNoteEditCentralWidgetEnabled(noteEditIsCentralWidget);
    }

    _mainWindow->restoreState(
        settings.value(QStringLiteral("workspace-") + uuid + QStringLiteral("/windowState"))
            .toByteArray());

    // handle the visibility of the note subfolder panel
    _mainWindow->handleNoteSubFolderVisibility();

    // update the panel lists
    _mainWindow->updatePanelMenu();

    // check if the user wanted the note subfolder dock widget visible
    _mainWindow->setNoteSubFolderDockWidgetVisible(
        settings
            .value(QStringLiteral("workspace-") + uuid +
                       QStringLiteral("/noteSubFolderDockWidgetVisible"),
                   true)
            .toBool());

    // set the visibility of the note subfolder dock widget
    _mainWindow->handleNoteSubFolderVisibility();

    // if app was newly installed we want to center and resize the window
    if (settings.value(QStringLiteral("initialWorkspace")).toBool()) {
        MetricsService::instance()->sendEventIfEnabled(
            QStringLiteral("app/initial-layout"), QStringLiteral("app"),
            QStringLiteral("initial-layout"),
            settings.value(QStringLiteral("initialLayoutIdentifier")).toString());

        settings.remove(QStringLiteral("initialWorkspace"));
        _mainWindow->centerAndResize();
    }

    if (focusWidget != nullptr) {
        // set the focus to the widget that had the focus before
        // the workspace was restored
        focusWidget->setFocus();
    }
}

/**
 * Creates a new workspace with name
 *
 * @param name
 * @return
 */
bool WorkspaceManager::createNewWorkspace(QString name) {
    name = name.trimmed();

    if (name.isEmpty()) {
        return false;
    }

    SettingsService settings;
    const QString currentUuid = currentWorkspaceUuid();
    settings.setValue(QStringLiteral("previousWorkspace"), currentUuid);

    const QString uuid = Utils::Misc::createUuidString();
    QStringList workspaces = getWorkspaceUuidList();
    workspaces.append(uuid);

    settings.setValue(QStringLiteral("workspaces"), workspaces);
    settings.setValue(QStringLiteral("currentWorkspace"), uuid);
    settings.setValue(QStringLiteral("workspace-") + uuid + QStringLiteral("/name"), name);

    // store the new current workspace
    storeCurrentWorkspace();

    // update the menu and combo box
    updateWorkspaceLists();

    return true;
}

/**
 * Returns the uuid of the current workspace
 *
 * @return
 */
QString WorkspaceManager::currentWorkspaceUuid() {
    SettingsService settings;
    return settings.value(QStringLiteral("currentWorkspace")).toString();
}

QStringList WorkspaceManager::getWorkspaceUuidList() {
    SettingsService settings;
    return settings.value(QStringLiteral("workspaces")).toStringList();
}

QString WorkspaceManager::getWorkspaceUuid(const QString &workspaceName) {
    return _workspaceNameUuidMap.value(workspaceName, "");
}

/**
 * Creates a new workspace with asking for its name
 */
void WorkspaceManager::on_actionStore_as_new_workspace_triggered() {
    const QString name =
        QInputDialog::getText(_mainWindow, tr("Create new workspace"), tr("Workspace name:"))
            .trimmed();

    if (name.isEmpty()) {
        return;
    }

    // store the current workspace
    storeCurrentWorkspace();

    // create the new workspace
    createNewWorkspace(name);
}

/**
 * Removes the current workspace
 */
void WorkspaceManager::on_actionRemove_current_workspace_triggered() {
    QStringList workspaces = getWorkspaceUuidList();

    // there have to be at least one workspace
    if (workspaces.count() < 2) {
        return;
    }

    QString uuid = currentWorkspaceUuid();

    // if no workspace is set we can't remove it
    if (uuid.isEmpty()) {
        return;
    }

    // ask for permission
    if (Utils::Gui::question(_mainWindow, tr("Remove current workspace"),
                             tr("Remove the current workspace?"),
                             QStringLiteral("remove-workspace")) != QMessageBox::Yes) {
        return;
    }

    // reset current workspace
    workspaces.removeAll(uuid);
    const QString newUuid = workspaces.at(0);

    // set the new workspace
    setCurrentWorkspace(newUuid);

    SettingsService settings;
    settings.setValue(QStringLiteral("workspaces"), workspaces);

    // remove all settings in the group
    settings.beginGroup(QStringLiteral("workspace-") + uuid);
    settings.remove(QLatin1String(""));
    settings.endGroup();

    // update the menu and combo box
    updateWorkspaceLists();
}

void WorkspaceManager::on_actionRename_current_workspace_triggered() {
    const QString uuid = currentWorkspaceUuid();

    // if no workspace is set we can't rename it
    if (uuid.isEmpty()) {
        return;
    }

    SettingsService settings;
    QString name =
        settings.value(QStringLiteral("workspace-") + uuid + QStringLiteral("/name")).toString();

    // ask for the new name
    name = QInputDialog::getText(_mainWindow, tr("Rename workspace"), tr("Workspace name:"),
                                 QLineEdit::Normal, name)
               .trimmed();

    if (name.isEmpty()) {
        return;
    }

    // rename the workspace
    settings.setValue(QStringLiteral("workspace-") + uuid + QStringLiteral("/name"), name);

    // update the menu and combo box
    updateWorkspaceLists();
}

/**
 * Switch to the previous workspace
 */
void WorkspaceManager::on_actionSwitch_to_previous_workspace_triggered() {
    SettingsService settings;
    QString uuid = settings.value(QStringLiteral("previousWorkspace")).toString();

    if (!uuid.isEmpty()) {
        setCurrentWorkspace(uuid);
    }
}

void WorkspaceManager::on_actionUnlock_panels_toggled(bool arg1) {
    const QSignalBlocker blocker(_ui->actionUnlock_panels);
    {
        Q_UNUSED(blocker)
        _ui->actionUnlock_panels->setChecked(arg1);
    }

    const QList<QDockWidget *> dockWidgets = _mainWindow->findChildren<QDockWidget *>();

    if (!arg1) {
        // remove the title bar widgets of all dock widgets
        for (QDockWidget *dockWidget : dockWidgets) {
            // we don't want to lock floating dock widgets
            if (dockWidget->isFloating()) {
                continue;
            }

            handleDockWidgetLocking(dockWidget);
        }
    } else {
        // add the old title bar widgets to all dock widgets
        _mainWindow->restoreDockWidgetTitleBars();

        for (QDockWidget *dockWidget : dockWidgets) {
            // reset the top margin of the enclosed widget
            dockWidget->widget()->setContentsMargins(0, 0, 0, 0);
        }
    }
}

void WorkspaceManager::handleDockWidgetLocking(QDockWidget *dockWidget) {
    // Remove the title bar widget
    dockWidget->setTitleBarWidget(new QWidget());

#ifndef Q_OS_MAC
    // Set 3px top margin for the enclosed widget
    dockWidget->widget()->setContentsMargins(0, 3, 0, 0);
#endif
}

/**
 * Shows all dock widgets
 */
void WorkspaceManager::on_actionShow_all_panels_triggered() {
    const QList<QDockWidget *> dockWidgets = _mainWindow->findChildren<QDockWidget *>();

    for (QDockWidget *dockWidget : dockWidgets) {
        dockWidget->setVisible(true);
    }

    _mainWindow->setNoteSubFolderDockWidgetVisible(true);

    // handle the visibility of the note subfolder panel
    _mainWindow->handleNoteSubFolderVisibility();

    // update the preview in case it was disabled previously
    _mainWindow->setNoteTextFromNote(&_mainWindow->currentNote, true);

    // filter notes according to selections
    _mainWindow->filterNotes();
}

/**
 * Reattaches all floating panels in case they can't be reattached manually anymore
 */
void WorkspaceManager::on_actionReattach_panels_triggered() {
    const QList<QDockWidget *> dockWidgets = _mainWindow->findChildren<QDockWidget *>();

    for (QDockWidget *dockWidget : dockWidgets) {
        if (!dockWidget->isFloating()) {
            continue;
        }

        dockWidget->setFloating(false);

        // Remove the title bar if panels are locked
        if (!_ui->actionUnlock_panels->isChecked()) {
            handleDockWidgetLocking(dockWidget);
        }
    }
}
