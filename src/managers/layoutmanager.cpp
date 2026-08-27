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

#include "layoutmanager.h"

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

LayoutManager::LayoutManager(MainWindow *mainWindow, Ui::MainWindow *ui, QObject *parent)
    : QObject(parent), _mainWindow(mainWindow), _ui(ui) {}

void LayoutManager::initLayoutComboBox() {
    _layoutComboBox = new QComboBox(_mainWindow);
    connect(_layoutComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &LayoutManager::onLayoutComboBoxCurrentIndexChanged);
    _layoutComboBox->setToolTip(tr("Layouts"));
    _layoutComboBox->setObjectName(QStringLiteral("layoutComboBox"));
}

void LayoutManager::updateLayoutLists(bool rebuild) {
    SettingsService settings;
    const QStringList layouts = getLayoutUuidList();
    const QString currentUuid = currentLayoutUuid();

    if (rebuild) {
        // we need to create a new combo box so the width gets updated in the
        // window toolbar
        initLayoutComboBox();

        _ui->menuLayouts->clear();

        _layoutNameUuidMap.clear();
    }

    const QSignalBlocker blocker(_layoutComboBox);
    Q_UNUSED(blocker)

    int currentIndex = 0;

    for (int i = 0; i < layouts.count(); i++) {
        const QString &uuid = layouts.at(i);

        if (uuid == currentUuid) {
            currentIndex = i;
        }

        // check if we want to skip the rebuilding part
        if (!rebuild) {
            continue;
        }

        const QString name =
            settings.value(QStringLiteral("layout-") + uuid + QStringLiteral("/name")).toString();
        const QString objectName = QStringLiteral("restoreLayout-") + uuid;

        _layoutNameUuidMap.insert(name, uuid);

        _layoutComboBox->addItem(name, uuid);

        auto *action = new QAction(name, _ui->menuLayouts);
        connect(action, &QAction::triggered, this, [this, uuid]() { setCurrentLayout(uuid); });

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

        _ui->menuLayouts->addAction(action);
    }

    _layoutComboBox->setCurrentIndex(currentIndex);

    if (rebuild) {
        // we need to adapt the width of the layouts combo box
        _mainWindow->updateWindowToolbar();
    }

    // enable the remove button if there are at least two layouts
    _ui->actionRemove_current_layout->setEnabled(layouts.count() > 1);
}

/**
 * Sets the new current layout when the layout combo box index has changed
 */
void LayoutManager::onLayoutComboBoxCurrentIndexChanged(int index) {
    Q_UNUSED(index)

    const QString uuid = _layoutComboBox->currentData().toString();

    // set the new layout
    setCurrentLayout(uuid);
}

void LayoutManager::setCurrentLayout(const QString &uuid) {
    // store the current layout
    storeCurrentLayout();

    SettingsService settings;
    QString currentUuid = currentLayoutUuid();
    settings.setValue(QStringLiteral("previousLayout"), currentUuid);
    settings.setValue(QStringLiteral("currentLayout"), uuid);

    // restore the new layout
    QTimer::singleShot(0, _mainWindow, SLOT(restoreCurrentLayout()));

    // Check if the layout is new (not yet in the combo box) and needs a full rebuild
    bool needsRebuild = _layoutComboBox->findData(uuid) == -1;

    // update the menu and combo box
    updateLayoutLists(needsRebuild);

    // update the preview in case it was disabled previously
    _mainWindow->setNoteTextFromNote(&_mainWindow->currentNote, true);

    ScriptingService::instance()->callLayoutSwitchedHook(currentUuid, uuid);
}

/**
 * Stores the current layout
 */
void LayoutManager::storeCurrentLayout() {
    const bool forceQuit = qApp->property("clearAppDataAndExit").toBool();
    if (MainWindow::isInDistractionFreeMode() || forceQuit || _mainWindow->closeEventWasFired()) {
        return;
    }

    qDebug() << __func__;
    SettingsService settings;
    QString uuid = currentLayoutUuid();

    settings.setValue(QStringLiteral("layout-") + uuid + QStringLiteral("/windowState"),
                      _mainWindow->saveState());
    settings.setValue(QStringLiteral("layout-") + uuid + QStringLiteral("/noteEditIsCentralWidget"),
                      _mainWindow->noteEditIsCentralWidget());
    settings.setValue(
        QStringLiteral("layout-") + uuid + QStringLiteral("/noteSubFolderDockWidgetVisible"),
        _mainWindow->noteSubFolderDockWidgetVisible());
}

/**
 * Restores the current layout
 */
void LayoutManager::restoreCurrentLayout() {
    SettingsService settings;
    QStringList layouts = getLayoutUuidList();
    QWidget *focusWidget = qApp->focusWidget();

    // create a default layout if there is none yet
    if (layouts.count() == 0) {
        createNewLayout(tr("full", "full layout"));

        _mainWindow->setDockWidgetVisible("taggingDockWidget", false);
        _mainWindow->setDockWidgetVisible("noteFolderDockWidget", false);
        _mainWindow->setDockWidgetVisible("noteNavigationDockWidget", false);
        _mainWindow->setDockWidgetVisible("noteTagDockWidget", false);
        _mainWindow->setDockWidgetVisible("notePreviewDockWidget", false);
        _mainWindow->setDockWidgetVisible("noteGraphicsViewDockWidget", false);
        createNewLayout(tr("minimal", "minimal layout"));

        // TODO: maybe still create those layouts initially?
    }

    QString uuid = currentLayoutUuid();

    // set the first layout as current layout if there is none set
    if (uuid.isEmpty()) {
        layouts = getLayoutUuidList();

        if (layouts.count() == 0) {
            return;
        }

        uuid = layouts.at(0);
        settings.setValue(QStringLiteral("currentLayout"), uuid);

        // update the menu and combo box
        updateLayoutLists();
    }

    const QString noteEditIsCentralWidgetKey =
        QStringLiteral("layout-") + uuid + QStringLiteral("/noteEditIsCentralWidget");
    const bool noteEditIsCentralWidget =
        settings.contains(noteEditIsCentralWidgetKey)
            ? settings.value(noteEditIsCentralWidgetKey).toBool()
            : settings.value(QStringLiteral("noteEditIsCentralWidget"), true).toBool();
    settings.setValue(QStringLiteral("noteEditIsCentralWidget"), noteEditIsCentralWidget);

    if (_mainWindow->noteEditIsCentralWidget() != noteEditIsCentralWidget) {
        _mainWindow->setNoteEditCentralWidgetEnabled(noteEditIsCentralWidget);
    }

    _mainWindow->restoreState(
        settings.value(QStringLiteral("layout-") + uuid + QStringLiteral("/windowState"))
            .toByteArray());

    // handle the visibility of the note subfolder panel
    _mainWindow->handleNoteSubFolderVisibility();

    // update the panel lists
    _mainWindow->updatePanelMenu();

    // check if the user wanted the note subfolder dock widget visible
    _mainWindow->setNoteSubFolderDockWidgetVisible(
        settings
            .value(QStringLiteral("layout-") + uuid +
                       QStringLiteral("/noteSubFolderDockWidgetVisible"),
                   true)
            .toBool());

    // set the visibility of the note subfolder dock widget
    _mainWindow->handleNoteSubFolderVisibility();

    // if app was newly installed we want to center and resize the window
    if (settings.value(QStringLiteral("initialLayout")).toBool()) {
        MetricsService::instance()->sendEventIfEnabled(
            QStringLiteral("app/initial-layout"), QStringLiteral("app"),
            QStringLiteral("initial-layout"),
            settings.value(QStringLiteral("initialLayoutPresetIdentifier")).toString());

        settings.remove(QStringLiteral("initialLayout"));
        _mainWindow->centerAndResize();
    }

    if (focusWidget != nullptr) {
        // set the focus to the widget that had the focus before
        // the layout was restored
        focusWidget->setFocus();
    }
}

/**
 * Creates a new layout with name
 *
 * @param name
 * @return
 */
bool LayoutManager::createNewLayout(QString name) {
    name = name.trimmed();

    if (name.isEmpty()) {
        return false;
    }

    SettingsService settings;
    const QString currentUuid = currentLayoutUuid();
    settings.setValue(QStringLiteral("previousLayout"), currentUuid);

    const QString uuid = Utils::Misc::createUuidString();
    QStringList layouts = getLayoutUuidList();
    layouts.append(uuid);

    settings.setValue(QStringLiteral("layouts"), layouts);
    settings.setValue(QStringLiteral("currentLayout"), uuid);
    settings.setValue(QStringLiteral("layout-") + uuid + QStringLiteral("/name"), name);

    // store the new current layout
    storeCurrentLayout();

    // update the menu and combo box
    updateLayoutLists();

    return true;
}

bool LayoutManager::removeLayout(const QString &uuid) {
    SettingsService settings;
    QStringList layouts = getLayoutUuidList();
    if ((layouts.count() < 2) || !layouts.contains(uuid)) {
        return false;
    }

    const QString currentUuid = currentLayoutUuid();
    const int removedIndex = layouts.indexOf(uuid);
    layouts.removeAll(uuid);
    settings.setValue(QStringLiteral("layouts"), layouts);

    settings.beginGroup(QStringLiteral("layout-") + uuid);
    settings.remove(QLatin1String(""));
    settings.endGroup();
    settings.remove(QStringLiteral("Shortcuts/MainWindow-restoreLayout-") + uuid);
    settings.remove(QStringLiteral("GlobalShortcuts/MainWindow-restoreLayout-") + uuid);

    if (currentUuid == uuid) {
        const QString replacementUuid = layouts.at(qMin(removedIndex, layouts.count() - 1));
        settings.setValue(QStringLiteral("previousLayout"), QString());
        settings.setValue(QStringLiteral("currentLayout"), replacementUuid);
        QTimer::singleShot(0, _mainWindow, SLOT(restoreCurrentLayout()));
        _mainWindow->setNoteTextFromNote(&_mainWindow->currentNote, true);
        ScriptingService::instance()->callLayoutSwitchedHook(uuid, replacementUuid);
    } else if (settings.value(QStringLiteral("previousLayout")).toString() == uuid) {
        settings.setValue(QStringLiteral("previousLayout"), QString());
    }

    updateLayoutLists();
    return true;
}

bool LayoutManager::renameLayout(const QString &uuid, const QString &name) {
    const QString trimmedName = name.trimmed();
    if (trimmedName.isEmpty() || !getLayoutUuidList().contains(uuid)) {
        return false;
    }

    SettingsService settings;
    const QString key = QStringLiteral("layout-") + uuid + QStringLiteral("/name");
    if (settings.value(key).toString() == trimmedName) {
        return false;
    }

    settings.setValue(key, trimmedName);
    updateLayoutLists();
    return true;
}

bool LayoutManager::setLayoutOrder(const QStringList &uuids) {
    const QStringList storedUuids = getLayoutUuidList();
    QStringList currentUuids = storedUuids;
    QStringList sortedUuids = uuids;
    currentUuids.sort();
    sortedUuids.sort();
    if ((currentUuids != sortedUuids) || (uuids.count() != storedUuids.count())) {
        return false;
    }

    SettingsService settings;
    settings.setValue(QStringLiteral("layouts"), uuids);
    updateLayoutLists();
    return true;
}

bool LayoutManager::layoutNoteEditIsCentralWidget(const QString &uuid) const {
    SettingsService settings;
    const QString key =
        QStringLiteral("layout-") + uuid + QStringLiteral("/noteEditIsCentralWidget");
    return settings.value(key, settings.value(QStringLiteral("noteEditIsCentralWidget"), true))
        .toBool();
}

void LayoutManager::setLayoutNoteEditIsCentralWidget(const QString &uuid, bool enabled) {
    if (!getLayoutUuidList().contains(uuid)) {
        return;
    }

    SettingsService settings;
    settings.setValue(QStringLiteral("layout-") + uuid + QStringLiteral("/noteEditIsCentralWidget"),
                      enabled);

    if (uuid == currentLayoutUuid()) {
        settings.setValue(QStringLiteral("noteEditIsCentralWidget"), enabled);
        _mainWindow->setNoteEditCentralWidgetEnabled(enabled);
        storeCurrentLayout();
    }
}

/**
 * Returns the uuid of the current layout
 *
 * @return
 */
QString LayoutManager::currentLayoutUuid() {
    SettingsService settings;
    return settings.value(QStringLiteral("currentLayout")).toString();
}

QStringList LayoutManager::getLayoutUuidList() {
    SettingsService settings;
    return settings.value(QStringLiteral("layouts")).toStringList();
}

QString LayoutManager::getLayoutUuid(const QString &layoutName) {
    return _layoutNameUuidMap.value(layoutName, "");
}

/**
 * Creates a new layout with asking for its name
 */
void LayoutManager::on_actionStore_as_new_layout_triggered() {
    const QString name =
        QInputDialog::getText(_mainWindow, tr("Create new layout"), tr("Layout name:")).trimmed();

    if (name.isEmpty()) {
        return;
    }

    // store the current layout
    storeCurrentLayout();

    // create the new layout
    createNewLayout(name);
}

/**
 * Removes the current layout
 */
void LayoutManager::on_actionRemove_current_layout_triggered() {
    QStringList layouts = getLayoutUuidList();

    // there have to be at least one layout
    if (layouts.count() < 2) {
        return;
    }

    QString uuid = currentLayoutUuid();

    // if no layout is set we can't remove it
    if (uuid.isEmpty()) {
        return;
    }

    // ask for permission
    if (Utils::Gui::question(_mainWindow, tr("Remove current layout"),
                             tr("Remove the current layout?"),
                             QStringLiteral("remove-layout")) != QMessageBox::Yes) {
        return;
    }

    removeLayout(uuid);
}

void LayoutManager::on_actionRename_current_layout_triggered() {
    const QString uuid = currentLayoutUuid();

    // if no layout is set we can't rename it
    if (uuid.isEmpty()) {
        return;
    }

    SettingsService settings;
    QString name =
        settings.value(QStringLiteral("layout-") + uuid + QStringLiteral("/name")).toString();

    // ask for the new name
    name = QInputDialog::getText(_mainWindow, tr("Rename layout"), tr("Layout name:"),
                                 QLineEdit::Normal, name)
               .trimmed();

    renameLayout(uuid, name);
}

/**
 * Switch to the previous layout
 */
void LayoutManager::on_actionSwitch_to_previous_layout_triggered() {
    SettingsService settings;
    QString uuid = settings.value(QStringLiteral("previousLayout")).toString();

    if (!uuid.isEmpty()) {
        setCurrentLayout(uuid);
    }
}

void LayoutManager::on_actionUnlock_panels_toggled(bool arg1) {
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

void LayoutManager::handleDockWidgetLocking(QDockWidget *dockWidget) {
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
void LayoutManager::on_actionShow_all_panels_triggered() {
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
void LayoutManager::on_actionReattach_panels_triggered() {
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
