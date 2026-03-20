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
class QActionGroup;
class QComboBox;
class QToolBar;
class WaitingSpinnerWidget;

namespace Ui {
class MainWindow;
}

class AiToolbarManager : public QObject {
    Q_OBJECT

   public:
    explicit AiToolbarManager(MainWindow *mainWindow, Ui::MainWindow *ui, QToolBar *aiToolbar,
                              QObject *parent = nullptr);

    void initializeOpenAiActivitySpinner();
    void reloadOpenAiControls();
    void buildAiToolbarAndActions();
    void enableOpenAiActivitySpinner(bool enable = true);
    void generateAiBackendComboBox();
    void generateAiModelComboBox();
    void generateAiModelMainMenu();
    void aiModelMainMenuSetCurrentItem();

    QToolBar *aiToolbar() const { return _aiToolbar; }
    WaitingSpinnerWidget *openAiActivitySpinner() const { return _openAiActivitySpinner; }

   public slots:
    void onAiBackendComboBoxCurrentIndexChanged(int index);
    void onAiModelComboBoxCurrentIndexChanged(int index);
    void onAiModelGroupChanged(QAction *action);
    void on_actionEnable_AI_toggled(bool arg1);

   private:
    MainWindow *_mainWindow;
    Ui::MainWindow *_ui;
    QToolBar *_aiToolbar;
    QComboBox *_aiBackendComboBox = nullptr;
    QComboBox *_aiModelComboBox = nullptr;
    QActionGroup *_aiModelGroup = nullptr;
    WaitingSpinnerWidget *_openAiActivitySpinner = nullptr;
};
