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
class WebApplicationSettingsWidget;
}

class WebApplicationSettingsWidget : public QWidget {
    Q_OBJECT

   public:
    explicit WebApplicationSettingsWidget(QWidget *parent = nullptr);
    ~WebApplicationSettingsWidget();

    void initialize();
    void readSettings();
    void storeSettings();
    void updateConnectedDevices(const QStringList &deviceNames);

   signals:
    void needRestart();

   private slots:
    void on_webAppServerUrlResetButton_clicked();
    void on_webAppShowTokenButton_clicked();
    void on_webAppCopyTokenButton_clicked();
    void on_webAppGenerateTokenButton_clicked();
    void on_enableWebApplicationCheckBox_toggled();
    void on_webAppTokenLineEdit_textChanged(const QString &arg1);
    void on_showQRCodeButton_clicked();
    void on_webAppConnectionNameResetButton_clicked();
    void on_webAppTestConnectionButton_clicked();
    void on_refreshConnectedDevicesButton_clicked();
    void on_webAppConnectionNameLineEdit_textChanged(const QString &arg1);

   private:
    Ui::WebApplicationSettingsWidget *ui;
};
