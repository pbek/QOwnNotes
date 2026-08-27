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

        // Store the current layout in case we changed something.
        _mainWindow->storeCurrentLayout();

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
        settings.setValue(QStringLiteral("DistractionFreeMode/centralWidget"),
                          _mainWindow->centralWidgetIdentifier());

        if (_mainWindow->notePreviewIsCentralWidget()) {
            _mainWindow->setCentralWidgetIdentifier(QStringLiteral("note-edit"));
        }

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

        // Hide the status bar in distraction free mode if the setting is enabled
        const bool hideStatusBarInDFM =
            settings.value(QStringLiteral("DistractionFreeMode/hideStatusBar")).toBool();
        settings.setValue(QStringLiteral("DistractionFreeMode/statusBarWasVisible"),
                          _mainWindow->statusBar()->isVisible());
        if (hideStatusBarInDFM) {
            _mainWindow->statusBar()->hide();
        }

        const bool openInFullScreen =
            settings.value(QStringLiteral("DistractionFreeMode/openInFullScreen"), true).toBool();
        const bool enterFullScreen = openInFullScreen && !_mainWindow->isFullScreen();
        settings.setValue(QStringLiteral("DistractionFreeMode/leaveFullScreenOnExit"),
                          enterFullScreen);

        if (_leaveFullScreenModeButton != nullptr) {
            _mainWindow->statusBar()->removeWidget(_leaveFullScreenModeButton);
            disconnect(_leaveFullScreenModeButton, nullptr, nullptr, nullptr);
        }

        if (_leaveDistractionFreeModeButton == nullptr) {
            _leaveDistractionFreeModeButton = new QPushButton(tr("Leave"));
        }

        _leaveDistractionFreeModeButton->setText(tr("Leave"));
        _leaveDistractionFreeModeButton->setFlat(true);
        _leaveDistractionFreeModeButton->setToolTip(tr("Leave"));
        _leaveDistractionFreeModeButton->setStyleSheet(
            QStringLiteral("QPushButton {padding: 0 5px}"));

        _leaveDistractionFreeModeButton->setIcon(QIcon::fromTheme(
            QStringLiteral("zoom-original"),
            QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/zoom-original.svg"))));

        disconnect(_leaveDistractionFreeModeButton, nullptr, nullptr, nullptr);
        connect(_leaveDistractionFreeModeButton, &QPushButton::clicked, this,
                &DistractionFreeManager::toggleDistractionFreeMode);

        _mainWindow->statusBar()->addPermanentWidget(_leaveDistractionFreeModeButton);

        if (enterFullScreen) {
            on_actionToggle_fullscreen_triggered();
        }

        _ui->noteEditTabWidget->tabBar()->hide();
    } else {
        //
        // leave the distraction free mode
        //

        if (_leaveDistractionFreeModeButton != nullptr) {
            _mainWindow->statusBar()->removeWidget(_leaveDistractionFreeModeButton);
            disconnect(_leaveDistractionFreeModeButton, nullptr, nullptr, nullptr);
            delete _leaveDistractionFreeModeButton;
            _leaveDistractionFreeModeButton = nullptr;
        }

        // Restore the status bar visibility if it was hidden in distraction free mode
        const bool statusBarWasVisible =
            settings.value(QStringLiteral("DistractionFreeMode/statusBarWasVisible"), true)
                .toBool();
        _mainWindow->statusBar()->setVisible(statusBarWasVisible);

        const QString centralWidget =
            settings.value(QStringLiteral("DistractionFreeMode/centralWidget")).toString();
        if (!centralWidget.isEmpty() && (_mainWindow->centralWidgetIdentifier() != centralWidget)) {
            _mainWindow->setCentralWidgetIdentifier(centralWidget);
        }

        // restore states and sizes
        _mainWindow->restoreState(
            settings.value(QStringLiteral("DistractionFreeMode/windowState")).toByteArray());
        _ui->menuBar->setVisible(
            settings.value(QStringLiteral("DistractionFreeMode/menuBarVisible")).toBool());
        _ui->menuBar->restoreGeometry(
            settings.value(QStringLiteral("DistractionFreeMode/menuBarGeometry")).toByteArray());
        _ui->menuBar->setFixedHeight(
            settings.value(QStringLiteral("DistractionFreeMode/menuBarHeight")).toInt());

        const bool leaveFullScreenOnExit =
            settings.value(QStringLiteral("DistractionFreeMode/leaveFullScreenOnExit")).toBool();
        settings.setValue(QStringLiteral("DistractionFreeMode/leaveFullScreenOnExit"), false);

        if (leaveFullScreenOnExit && _mainWindow->isFullScreen()) {
            on_actionToggle_fullscreen_triggered();
        } else if (_mainWindow->isFullScreen()) {
#ifndef Q_OS_MAC
            if (_leaveFullScreenModeButton == nullptr) {
                _leaveFullScreenModeButton = new QPushButton(tr("Leave"));
            }

            _leaveFullScreenModeButton->setText(tr("Leave"));
            _leaveFullScreenModeButton->setFlat(true);
            _leaveFullScreenModeButton->setToolTip(tr("Leave"));
            _leaveFullScreenModeButton->setStyleSheet(
                QStringLiteral("QPushButton {padding: 0 5px}"));
            _leaveFullScreenModeButton->setIcon(QIcon::fromTheme(
                QStringLiteral("zoom-original"),
                QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/zoom-original.svg"))));

            disconnect(_leaveFullScreenModeButton, nullptr, nullptr, nullptr);
            connect(_leaveFullScreenModeButton, &QPushButton::clicked, this,
                    &DistractionFreeManager::on_actionToggle_fullscreen_triggered);

            _mainWindow->statusBar()->addPermanentWidget(_leaveFullScreenModeButton);
#endif
        }

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
        _leaveFullScreenModeButton = new QPushButton(tr("Leave"));
    }

    if (_mainWindow->isFullScreen()) {
        _mainWindow->showNormal();

        // we need a showNormal() first to exist full-screen mode
        if (_isMaximizedBeforeFullScreen) {
            _mainWindow->showMaximized();
        } else if (_isMinimizedBeforeFullScreen) {
            _mainWindow->showMinimized();
        }

        if (_leaveFullScreenModeButton != nullptr) {
            _mainWindow->statusBar()->removeWidget(_leaveFullScreenModeButton);
            disconnect(_leaveFullScreenModeButton, nullptr, nullptr, nullptr);
            delete _leaveFullScreenModeButton;
            _leaveFullScreenModeButton = nullptr;
        }
    } else {
        _isMaximizedBeforeFullScreen = _mainWindow->isMaximized();
        _isMinimizedBeforeFullScreen = _mainWindow->isMinimized();
        _mainWindow->showFullScreen();

#ifndef Q_OS_MAC
        if (_leaveDistractionFreeModeButton == nullptr) {
            _leaveFullScreenModeButton->setText(tr("Leave"));
            _leaveFullScreenModeButton->setFlat(true);
            _leaveFullScreenModeButton->setToolTip(tr("Leave"));
            _leaveFullScreenModeButton->setStyleSheet(
                QStringLiteral("QPushButton {padding: 0 5px}"));

            _leaveFullScreenModeButton->setIcon(QIcon::fromTheme(
                QStringLiteral("zoom-original"),
                QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/zoom-original.svg"))));

            disconnect(_leaveFullScreenModeButton, nullptr, nullptr, nullptr);
            connect(_leaveFullScreenModeButton, &QPushButton::clicked, this,
                    &DistractionFreeManager::on_actionToggle_fullscreen_triggered);

            _mainWindow->statusBar()->addPermanentWidget(_leaveFullScreenModeButton);
        }
#endif
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
