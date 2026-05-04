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

#include <QListWidget>
#include <QWidget>

#include "entities/script.h"

namespace Ui {
class ScriptingSettingsWidget;
}

class ScriptingSettingsWidget : public QWidget {
    Q_OBJECT

   public:
    explicit ScriptingSettingsWidget(QWidget *parent = nullptr);
    ~ScriptingSettingsWidget();

    void initialize();
    void storeSettings();
    void searchScriptInRepository(bool checkForUpdates = false);

   private slots:
    void on_scriptSearchLineEdit_textChanged(const QString &arg1);
    void on_scriptRemoveButton_clicked();
    void on_scriptPathButton_clicked();
    void on_scriptListWidget_currentItemChanged(QListWidgetItem *current,
                                                QListWidgetItem *previous);
    void on_scriptNameLineEdit_editingFinished();
    void on_scriptValidationButton_clicked();
    void on_scriptReloadEngineButton_clicked();
    void on_scriptReloadEngineButton2_clicked();
    void on_scriptListWidget_itemChanged(QListWidgetItem *item);

    void checkForScriptUpdates();
    void addLocalScript();

   private:
    Ui::ScriptingSettingsWidget *ui;
    Script _selectedScript;
    QString _newScriptName;

    void reloadScriptList() const;
    void reloadCurrentScriptPage();
    void validateCurrentScript();
    void storeScriptListEnabledState();
    void clearScriptInfoJsonUi() const;
    void applyScriptInfoJsonToUi(const ScriptInfoJson &infoJson, bool showRepositoryLink) const;
    static bool getLocalScriptInfoJson(const QString &scriptPath, ScriptInfoJson &infoJson);
    static bool scriptMatchesSearchFilter(const Script &script, const QString &searchText);
};
