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

#include <entities/cloudconnection.h>
#include <enums/oklabelstatus.h>

#include <QWidget>

class SettingsDialog;

namespace Ui {
class CloudSettingsWidget;
}

class CloudSettingsWidget : public QWidget {
    Q_OBJECT

   public:
    // OKLabelStatus is a global enum defined in enums/oklabelstatus.h
    using OKLabelStatus = ::OKLabelStatus;

    explicit CloudSettingsWidget(QWidget *parent = nullptr);
    ~CloudSettingsWidget();

    void initialize();
    void readSettings();
    void storeSettings();
    void connectTestCallback(bool appIsValid, QString appVersion, QString serverVersion,
                             QString notesPathExistsText, QString connectionErrorMessage);
    void setOKLabelData(int number, const QString &text, OKLabelStatus status);
    void resetOKLabelData();
    void cancelConnectionTest();

    QString serverUrl() const;

    void setSettingsDialog(SettingsDialog *dialog);

   signals:
    void cloudConnectionsChanged(const QList<CloudConnection> &connections);

   public slots:
    void storeCloudDebugData() const;

   private slots:
    void on_connectButton_clicked();
    void on_cloudSupportCheckBox_toggled();
    void on_cloudConnectionComboBox_currentIndexChanged(int index);
    void on_cloudConnectionAddButton_clicked();
    void on_cloudConnectionRemoveButton_clicked();
    void storeSelectedCloudConnection();
    void on_cloudServerAppPageButton_clicked();
    void on_cloudServerAppPasswordPageButton_clicked();
    void on_loginFlowButton_clicked();
    void on_loginFlowCancelButton_clicked();
    void on_appNextcloudDeckCheckBox_toggled(bool checked);
    void onSettingsConnectionTestFinished();

   private:
    Ui::CloudSettingsWidget *ui;
    SettingsDialog *_settingsDialog = nullptr;
    CloudConnection _selectedCloudConnection;
    int _loginFlowPollCount = 0;
    QString _installInfoTextLabel1Html;
    QString _installInfoTextLabel2Html;
    QString _installInfoTextLabel3Html;
    bool _connectionTestInProgress = false;
    bool _appIsValid = false;
    QString _appVersion;
    QString _serverVersion;
    QString _notesPathExistsText;
    QString _connectionErrorMessage;

    void startConnectionTest();
    void setConnectionTestInProgress(bool inProgress);
    bool connectionTestCanBeStarted() const;
    void initCloudConnectionComboBox(int selectedId = -1);
};
