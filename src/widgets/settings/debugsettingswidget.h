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

#include <QWidget>

namespace Ui {
class DebugSettingsWidget;
}

class DebugSettingsWidget : public QWidget {
    Q_OBJECT

   public:
    explicit DebugSettingsWidget(QWidget *parent = nullptr);
    ~DebugSettingsWidget();

    void initialize();
    void outputSettings();

   signals:
    void aboutToOutputSettings();
    void issueAssistantRequested();

   private slots:
    void on_gitHubLineBreaksCheckBox_toggled(bool checked);
    void on_debugInfoAnonymizeCheckBox_toggled(bool checked);
    void on_saveDebugInfoButton_clicked();
    void on_copyDebugInfoButton_clicked();
    void on_issueAssistantPushButton_clicked();
    void on_openSettingsFileButton_clicked();

   private:
    Ui::DebugSettingsWidget *ui;
};
