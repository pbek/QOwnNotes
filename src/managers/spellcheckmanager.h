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

namespace Ui {
class MainWindow;
}

class SpellCheckManager : public QObject {
    Q_OBJECT

   public:
    explicit SpellCheckManager(MainWindow *mainWindow, Ui::MainWindow *ui,
                               QObject *parent = nullptr);

    void loadDictionaryNames();
    void loadSpellingBackends();

   public slots:
    void on_actionCheck_spelling_toggled(bool checked);
    void onLanguageChanged(QAction *action);
    void onSpellBackendChanged(QAction *action);
    void on_actionManage_dictionaries_triggered();

   private:
    MainWindow *_mainWindow;
    Ui::MainWindow *_ui;
    QActionGroup *_languageGroup;
    QActionGroup *_spellBackendGroup;
};
