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
class AiSettingsWidget;
}

class AiSettingsWidget : public QWidget {
    Q_OBJECT

   public:
    explicit AiSettingsWidget(QWidget *parent = nullptr);
    ~AiSettingsWidget();

    void initialize();
    void readSettings();
    void storeSettings();
    void buildAiScriptingTreeWidget();

   signals:
    void searchScriptRepositoryRequested();

   private slots:
    void on_openRouterApiKeyWebButton_clicked();
    void on_openRouterApiTestButton_clicked();
    void on_openRouterApiKeyLineEdit_textChanged(const QString &arg1);
    void on_groqApiKeyWebButton_clicked();
    void on_openAiApiKeyWebButton_clicked();
    void on_groqApiTestButton_clicked();
    void on_openAiApiTestButton_clicked();
    void on_groqApiKeyLineEdit_textChanged(const QString &arg1);
    void on_openAiApiKeyLineEdit_textChanged(const QString &arg1);
    void on_searchScriptRepositoryButton_clicked();

   private:
    Ui::AiSettingsWidget *ui;
    void runAiApiTest(QString backend, QString model, QString apiKey = QString());
};
