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

#include "systemtraymanager.h"

#include <entities/calendaritem.h>
#include <entities/note.h>
#include <entities/notefolder.h>
#include <services/settingsservice.h>
#include <utils/gui.h>

#include <QMenu>

#include "mainwindow.h"
#include "ui_mainwindow.h"

SystemTrayManager::SystemTrayManager(MainWindow *mainWindow, Ui::MainWindow *ui,
                                     bool showSystemTray, QObject *parent)
    : QObject(parent),
      _mainWindow(mainWindow),
      _ui(ui),
      _trayIcon(nullptr),
      _showSystemTray(showSystemTray) {}

void SystemTrayManager::createSystemTrayIcon() {
    _trayIcon = new QSystemTrayIcon(_mainWindow);

    connect(_trayIcon, &QSystemTrayIcon::activated, this,
            &SystemTrayManager::systemTrayIconClicked);

    if (_showSystemTray) {
        _trayIcon->setIcon(getSystemTrayIcon());
        _trayIcon->show();
    }
}

/**
 * Returns a proper system tray icon
 *
 * @return
 */
QIcon SystemTrayManager::getSystemTrayIcon() {
    const SettingsService settings;
    const bool darkModeIcon = settings.value(QStringLiteral("darkModeTrayIcon"), false).toBool();
    const QString file = darkModeIcon ? QStringLiteral(":/images/icon-dark.png")
                                      : QStringLiteral(":/images/icon.png");
    return QIcon(file);
}

QSystemTrayIcon *SystemTrayManager::trayIcon() const { return _trayIcon; }

void SystemTrayManager::systemTrayIconClicked(QSystemTrayIcon::ActivationReason reason) {
    // don't show or hide the app on OS X with a simple click because also the
    // context menu will be triggered
#ifndef Q_OS_MAC
    if (reason == QSystemTrayIcon::Trigger) {
        if (_mainWindow->isVisible() && !_mainWindow->isMinimized()) {
            _mainWindow->hide();
        } else {
            showWindow();
        }
    }
#else
    Q_UNUSED(reason);
#endif
}

/**
 * Shows the window (also brings it to the front and un-minimizes it)
 */
void SystemTrayManager::showWindow() {
    // show the window in case we are using the system tray
    _mainWindow->show();

    // bring application window to the front
    _mainWindow->activateWindow();    // for Windows
    _mainWindow->setWindowState((_mainWindow->windowState() & ~Qt::WindowMinimized) |
                                Qt::WindowActive);
    _mainWindow->raise();    // for MacOS

    // Really show the window, by bringing it to focus
    _mainWindow->setFocus();

    // parse the current note for the navigation panel in case it wasn't parsed
    // while the mainwindow was hidden (https://github.com/pbek/QOwnNotes/issues/2110)
    _mainWindow->startNavigationParser();
}

/**
 * Generates the system tray context menu
 */
void SystemTrayManager::generateSystemTrayContextMenu() {
    // trying to destroy the old context menu as fix for Ubuntu 14.04
    // just clearing an existing menu resulted in empty sub-menus
    //    QMenu *menu = _trayIcon->contextMenu();
    //    delete(menu);

    // QMenu(this) is not allowed here or it will not be recognized as child of
    // the tray icon later (see: https://github.com/pbek/QOwnNotes/issues/1239)
    auto *menu = new QMenu();
    menu->setTitle(QStringLiteral("QOwnNotes"));

    // add menu entry to open the app
    QString openActionText = tr("Open QOwnNotes");
#ifdef QT_DEBUG
    openActionText += QStringLiteral(" (Debug)");
#endif
    QAction *openAction = menu->addAction(openActionText);
    openAction->setIcon(getSystemTrayIcon());

    connect(openAction, &QAction::triggered, this, &SystemTrayManager::showWindow);

    menu->addSeparator();
    menu->addAction(_ui->actionSend_clipboard);
    menu->addAction(_ui->actionSend_clipboard_as_text);
    menu->addSeparator();

    const QList<NoteFolder> noteFolders = NoteFolder::fetchAll();
    const int noteFoldersCount = noteFolders.count();

    if (noteFoldersCount > 1) {
        // didn't resulted in a visible text
        //        QWidgetAction* action = new QWidgetAction(menu);
        //        QLabel* label = new
        //        QLabel(NoteFolder::currentNoteFolder().getName(), menu);
        //        action->setDefaultWidget(label);
        //        menu->addAction(action);

        QMenu *noteFolderMenu = menu->addMenu(tr("Note folders"));

        // populate the note folder menu
        for (const auto &noteFolder : noteFolders) {
            // don't show not existing folders or if path is empty
            if (!noteFolder.localPathExists()) {
                continue;
            }

            // add a menu entry
            QAction *action = noteFolderMenu->addAction(noteFolder.getName());
            action->setToolTip(noteFolder.getLocalPath());
            action->setStatusTip(noteFolder.getLocalPath());

            if (noteFolder.isCurrent()) {
                QFont font = action->font();
                // setting it bold didn't do anything for me
                font.setBold(true);
                action->setFont(font);

                action->setIcon(Utils::Gui::folderIcon());
            }

            const int folderId = noteFolder.getId();
            connect(action, &QAction::triggered, _mainWindow,
                    [this, folderId]() { _mainWindow->changeNoteFolder(folderId); });
        }

        menu->addSeparator();
    }

    // add menu entry to create a new note
    QAction *createNoteAction = menu->addAction(tr("New note"));
    createNoteAction->setIcon(
        QIcon::fromTheme(QStringLiteral("document-new"),
                         QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/document-new.svg"))));

    connect(createNoteAction, &QAction::triggered, _mainWindow,
            [this]() { _mainWindow->createNewNote(); });

    int maxNotes = Note::countAll();

    if (maxNotes > 0) {
        if (maxNotes > 9) {
            maxNotes = 9;
        }

        // add a menu for recent notes
        QMenu *noteMenu = menu->addMenu(tr("Recent notes"));

        const auto noteList = Note::fetchAll(maxNotes);

        for (const Note &note : noteList) {
            QAction *action = noteMenu->addAction(note.getName());
            action->setIcon(Utils::Gui::noteIcon());
            int noteId = note.getId();
            connect(action, &QAction::triggered, _mainWindow,
                    [this, noteId]() { _mainWindow->setCurrentNoteFromNoteId(noteId); });
        }
    }

    menu->addSeparator();

    // add menu entry to show the tasks
    QAction *taskAction = menu->addAction(tr("Show todo lists"));
    taskAction->setIcon(QIcon::fromTheme(
        QStringLiteral("view-calendar-tasks"),
        QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/view-calendar-tasks.svg"))));

    connect(taskAction, &QAction::triggered, _mainWindow,
            [this]() { _mainWindow->openTodoDialog(); });

    QList<CalendarItem> taskList = CalendarItem::fetchAllForSystemTray(10);
    if (taskList.count() > 0) {
        // add a menu for recent tasks
        QMenu *taskMenu = menu->addMenu(tr("Recent tasks"));

        // add menu entries to jump to tasks
        QListIterator<CalendarItem> itr(taskList);
        while (itr.hasNext()) {
            CalendarItem task = itr.next();

            QAction *action = taskMenu->addAction(task.getSummary());
            action->setIcon(QIcon::fromTheme(
                QStringLiteral("view-task"),
                QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/view-task.svg"))));

            connect(action, &QAction::triggered, _mainWindow,
                    [this, task]() { _mainWindow->openTodoDialog(task.getUid()); });
        }
    }

    menu->addSeparator();

    // add menu entry to quit the app
    QAction *quitAction = menu->addAction(tr("Quit"));
    quitAction->setIcon(QIcon::fromTheme(
        QStringLiteral("application-exit"),
        QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/application-exit.svg"))));
    connect(quitAction, &QAction::triggered, _mainWindow, &MainWindow::on_action_Quit_triggered);

    _trayIcon->setContextMenu(menu);
#ifdef QT_DEBUG
    _trayIcon->setToolTip(QStringLiteral("QOwnNotes (Debug)"));
#else
    _trayIcon->setToolTip(QStringLiteral("QOwnNotes"));
#endif
}

void SystemTrayManager::on_actionShow_Hide_application_triggered() {
    // isVisible() or isHidden() didn't work properly
    if (_mainWindow->isActiveWindow()) {
        _mainWindow->hide();
    } else {
        showWindow();
    }
}
