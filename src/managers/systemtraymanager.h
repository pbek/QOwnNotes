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

#include <QIcon>
#include <QObject>
#include <QSystemTrayIcon>

class MainWindow;
class QMenu;

namespace Ui {
class MainWindow;
}

class SystemTrayManager : public QObject {
    Q_OBJECT

   public:
    explicit SystemTrayManager(MainWindow *mainWindow, Ui::MainWindow *ui, bool showSystemTray,
                               QObject *parent = nullptr);

    void createSystemTrayIcon();
    static QIcon getSystemTrayIcon();
    QSystemTrayIcon *trayIcon() const;

   public slots:
    void systemTrayIconClicked(QSystemTrayIcon::ActivationReason reason);
    void showWindow();
    void generateSystemTrayContextMenu();
    void on_actionShow_Hide_application_triggered();

   private:
    MainWindow *_mainWindow;
    Ui::MainWindow *_ui;
    QSystemTrayIcon *_trayIcon;
    bool _showSystemTray;
};
