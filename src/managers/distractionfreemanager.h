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

class MainWindow;
class QAction;
class QPushButton;

namespace Ui {
class MainWindow;
}

class DistractionFreeManager : public QObject {
    Q_OBJECT

   public:
    explicit DistractionFreeManager(MainWindow *mainWindow, Ui::MainWindow *ui,
                                    QObject *parent = nullptr);

    void restoreDistractionFreeMode();
    static bool isInDistractionFreeMode();
    void toggleDistractionFreeMode();
    void setDistractionFreeMode(const bool enabled = true);
    void changeDistractionFreeMode(const bool enabled);

   public slots:
    void dfmEditorWidthActionTriggered(QAction *action);
    void on_actionToggle_distraction_free_mode_triggered();
    void on_actionToggle_fullscreen_triggered();
    void disableFullScreenMode();
    void on_actionEditorWidthCustom_triggered();

   private:
    MainWindow *_mainWindow;
    Ui::MainWindow *_ui;
    QPushButton *_leaveDistractionFreeModeButton = nullptr;
    QPushButton *_leaveFullScreenModeButton = nullptr;
    bool _isMaximizedBeforeFullScreen = false;
    bool _isMinimizedBeforeFullScreen = false;
};
