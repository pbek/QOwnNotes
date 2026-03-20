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

#include "distractionfreemanager.h"

#include <services/settingsservice.h>

#include <QDockWidget>
#include <QIcon>
#include <QInputDialog>
#include <QPushButton>
#include <QStatusBar>
#include <QTabBar>
#include <QToolBar>

#include "mainwindow.h"
#include "ui_mainwindow.h"

DistractionFreeManager::DistractionFreeManager(MainWindow *mainWindow, Ui::MainWindow *ui,
                                               QObject *parent)
    : QObject(parent), _mainWindow(mainWindow), _ui(ui) {}

void DistractionFreeManager::restoreDistractionFreeMode() {
    if (isInDistractionFreeMode()) {
        setDistractionFreeMode(true);
    }
}

/**
 * Checks if we are in distraction free mode
 */
bool DistractionFreeManager::isInDistractionFreeMode() {
    SettingsService settings;
    return settings.value(QStringLiteral("DistractionFreeMode/isEnabled")).toBool();
}

/**
 * Toggles the distraction free mode
 */
void DistractionFreeManager::toggleDistractionFreeMode() {
    // Leave the one-column mode if active
    if (_ui->actionUse_one_column_mode->isChecked()) {
        _ui->actionUse_one_column_mode->toggle();
    }

    SettingsService settings;
    bool isInDFM = DistractionFreeManager::isInDistractionFreeMode();

    qDebug() << __func__ << " - 'isInDistractionFreeMode': " << isInDFM;

    // Store the window settings before we go into distraction-free mode
    if (!isInDFM) {
        _mainWindow->storeSettings();
    }

    isInDFM = !isInDFM;

    // Remember that we were using the distraction-free mode
    settings.setValue(QStringLiteral("DistractionFreeMode/isEnabled"), isInDFM);

    setDistractionFreeMode(isInDFM);

    // Enter or leave fullscreen mode if we are in or left distraction-free mode
    if ((isInDFM && !_mainWindow->isFullScreen()) || (!isInDFM && _mainWindow->isFullScreen())) {
        on_actionToggle_fullscreen_triggered();
    }
}

/**
 * Enables or disables the distraction free mode
 */
void DistractionFreeManager::setDistractionFreeMode(const bool enabled) {
    SettingsService settings;

    if (enabled) {
        //
        // enter the distraction free mode
        //

        // turn off line numbers because they would look broken in dfm
        _ui->noteTextEdit->setLineNumberEnabled(false);
        _ui->encryptedNoteTextEdit->setLineNumberEnabled(false);

        // store the current workspace in case we changed something
        _mainWindow->storeCurrentWorkspace();

        const bool menuBarWasVisible =
            settings.value(QStringLiteral("showMenuBar"), !_ui->menuBar->isHidden()).toBool();

        // set the menu bar visible so we get the correct height
        if (!menuBarWasVisible) {
            _ui->menuBar->setVisible(true);
        }

        // remember states, geometry and sizes
        settings.setValue(QStringLiteral("DistractionFreeMode/windowState"),
                          _mainWindow->saveState());
        settings.setValue(QStringLiteral("DistractionFreeMode/menuBarGeometry"),
                          _ui->menuBar->saveGeometry());
        settings.setValue(QStringLiteral("DistractionFreeMode/menuBarHeight"),
                          _ui->menuBar->height());
        settings.setValue(QStringLiteral("DistractionFreeMode/menuBarVisible"), menuBarWasVisible);

        // we must not hide the menu bar or else the shortcuts
        // will not work any more
        _ui->menuBar->setFixedHeight(0);

        // hide the toolbars
        const QList<QToolBar *> toolbars = _mainWindow->findChildren<QToolBar *>();
        for (QToolBar *toolbar : toolbars) {
            toolbar->hide();
        }

        if (!_mainWindow->noteEditIsCentralWidget()) {
            // show the note edit dock widget
            _mainWindow->noteEditDockWidget()->show();
        }

        // hide all dock widgets but the note edit dock widget
        const QList<QDockWidget *> dockWidgets = _mainWindow->findChildren<QDockWidget *>();
        for (QDockWidget *dockWidget : dockWidgets) {
            if (dockWidget->objectName() == QStringLiteral("noteEditDockWidget")) {
                continue;
            }
            dockWidget->hide();
        }

        // hide the status bar
        //        ui->statusBar->hide();

        _leaveDistractionFreeModeButton = new QPushButton(tr("leave"));
        _leaveDistractionFreeModeButton->setFlat(true);
        _leaveDistractionFreeModeButton->setToolTip(tr("Leave distraction free mode"));
        _leaveDistractionFreeModeButton->setStyleSheet(
            QStringLiteral("QPushButton {padding: 0 5px}"));

        _leaveDistractionFreeModeButton->setIcon(QIcon::fromTheme(
            QStringLiteral("zoom-original"),
            QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/zoom-original.svg"))));

        connect(_leaveDistractionFreeModeButton, &QPushButton::clicked, this,
                &DistractionFreeManager::toggleDistractionFreeMode);

        _mainWindow->statusBar()->addPermanentWidget(_leaveDistractionFreeModeButton);

        _ui->noteEditTabWidget->tabBar()->hide();
    } else {
        //
        // leave the distraction free mode
        //

        _mainWindow->statusBar()->removeWidget(_leaveDistractionFreeModeButton);
        disconnect(_leaveDistractionFreeModeButton, nullptr, nullptr, nullptr);

        // restore states and sizes
        _mainWindow->restoreState(
            settings.value(QStringLiteral("DistractionFreeMode/windowState")).toByteArray());
        _ui->menuBar->setVisible(
            settings.value(QStringLiteral("DistractionFreeMode/menuBarVisible")).toBool());
        _ui->menuBar->restoreGeometry(
            settings.value(QStringLiteral("DistractionFreeMode/menuBarGeometry")).toByteArray());
        _ui->menuBar->setFixedHeight(
            settings.value(QStringLiteral("DistractionFreeMode/menuBarHeight")).toInt());

        if (_ui->noteEditTabWidget->count() > 1) {
            _ui->noteEditTabWidget->tabBar()->show();
        }

        bool showLineNumbersInEditor =
            settings.value(QStringLiteral("Editor/showLineNumbers")).toBool();

        // turn line numbers on again if they were enabled
        if (showLineNumbersInEditor) {
            _ui->noteTextEdit->setLineNumberEnabled(true);
            _ui->encryptedNoteTextEdit->setLineNumberEnabled(true);
        }
    }

    _ui->noteTextEdit->setPaperMargins();
    _ui->encryptedNoteTextEdit->setPaperMargins();
    _mainWindow->activeNoteTextEdit()->setFocus();
}

/**
 * Sets the distraction free mode if it is currently other than we want it to be
 */
void DistractionFreeManager::changeDistractionFreeMode(const bool enabled) {
    if (isInDistractionFreeMode() != enabled) {
        setDistractionFreeMode(enabled);
    }
}

void DistractionFreeManager::dfmEditorWidthActionTriggered(QAction *action) {
    SettingsService settings;
    settings.setValue(QStringLiteral("DistractionFreeMode/editorWidthMode"),
                      action->whatsThis().toInt());

    _ui->noteTextEdit->setPaperMargins();
    _ui->encryptedNoteTextEdit->setPaperMargins();
}

void DistractionFreeManager::on_actionToggle_distraction_free_mode_triggered() {
    toggleDistractionFreeMode();
}

void DistractionFreeManager::on_actionToggle_fullscreen_triggered() {
    // #1302: we need to init the button in any case if the app was already in
    //        fullscreen mode or "disconnect" will crash the app
    if (_leaveFullScreenModeButton == nullptr) {
        _leaveFullScreenModeButton = new QPushButton(tr("leave"));
    }

    if (_mainWindow->isFullScreen()) {
        _mainWindow->showNormal();

        // we need a showNormal() first to exist full-screen mode
        if (_isMaximizedBeforeFullScreen) {
            _mainWindow->showMaximized();
        } else if (_isMinimizedBeforeFullScreen) {
            _mainWindow->showMinimized();
        }

        _mainWindow->statusBar()->removeWidget(_leaveFullScreenModeButton);
        disconnect(_leaveFullScreenModeButton, nullptr, nullptr, nullptr);
        delete _leaveFullScreenModeButton;
        _leaveFullScreenModeButton = nullptr;
    } else {
        _isMaximizedBeforeFullScreen = _mainWindow->isMaximized();
        _isMinimizedBeforeFullScreen = _mainWindow->isMinimized();
        _mainWindow->showFullScreen();

        _leaveFullScreenModeButton->setFlat(true);
        _leaveFullScreenModeButton->setToolTip(tr("Leave full-screen mode"));
        _leaveFullScreenModeButton->setStyleSheet(QStringLiteral("QPushButton {padding: 0 5px}"));

        _leaveFullScreenModeButton->setIcon(QIcon::fromTheme(
            QStringLiteral("zoom-original"),
            QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/zoom-original.svg"))));

        connect(_leaveFullScreenModeButton, &QPushButton::clicked, this,
                &DistractionFreeManager::on_actionToggle_fullscreen_triggered);

        _mainWindow->statusBar()->addPermanentWidget(_leaveFullScreenModeButton);
    }
}

void DistractionFreeManager::disableFullScreenMode() {
    if (_mainWindow->isFullScreen()) {
        on_actionToggle_fullscreen_triggered();
    }
}

void DistractionFreeManager::on_actionEditorWidthCustom_triggered() {
    SettingsService settings;
    bool ok;
    int characters = QInputDialog::getInt(
        _mainWindow, tr("Custom editor width"), tr("Characters:"),
        settings.value(QStringLiteral("DistractionFreeMode/editorWidthCustom"), 80).toInt(), 20,
        10000, 1, &ok);

    if (ok) {
        settings.setValue(QStringLiteral("DistractionFreeMode/editorWidthCustom"), characters);
    }
}
