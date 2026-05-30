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

#include "webapplicationsettingswidget.h"

#include <QApplication>
#include <QClipboard>
#include <QDir>
#include <QMessageBox>
#include <QSysInfo>

#include "services/settingsservice.h"
#include "services/webappclientservice.h"
#include "ui_webapplicationsettingswidget.h"
#include "utils/misc.h"

WebApplicationSettingsWidget::WebApplicationSettingsWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::WebApplicationSettingsWidget) {
    ui->setupUi(this);

    // Hide QR code widget by default
    ui->qrCodeWidget->hide();

    // Connect signals that require restart
    connect(ui->enableWebApplicationCheckBox, SIGNAL(toggled(bool)), this, SIGNAL(needRestart()));
    connect(ui->webAppServerUrlLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(needRestart()));
    connect(ui->webAppTokenLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(needRestart()));
    connect(ui->webAppConnectionNameLineEdit, SIGNAL(textChanged(QString)), this,
            SIGNAL(needRestart()));
}

WebApplicationSettingsWidget::~WebApplicationSettingsWidget() { delete ui; }

void WebApplicationSettingsWidget::initialize() {
    ui->webAppLabel->setText(
        ui->webAppLabel->text().arg(QStringLiteral("https://app.qownnotes.org/")));
}

void WebApplicationSettingsWidget::readSettings() {
    ui->enableWebApplicationCheckBox->setChecked(Utils::Misc::isWebAppSupportEnabled());
    on_enableWebApplicationCheckBox_toggled();

    ui->webAppServerUrlLineEdit->setText(WebAppClientService::getServerUrl());
    ui->webAppTokenLineEdit->setText(WebAppClientService::getOrGenerateToken());
    ui->webAppConnectionNameLineEdit->setText(WebAppClientService::getOrGenerateConnectionName());
}

void WebApplicationSettingsWidget::storeSettings() {
    SettingsService settings;
    settings.setValue(QStringLiteral("enableWebAppSupport"),
                      ui->enableWebApplicationCheckBox->isChecked());
    settings.setValue(QStringLiteral("webAppClientService/serverUrl"),
                      ui->webAppServerUrlLineEdit->text());
    settings.setValue(QStringLiteral("webAppClientService/token"), ui->webAppTokenLineEdit->text());
    settings.setValue(QStringLiteral("webAppClientService/connectionName"),
                      ui->webAppConnectionNameLineEdit->text());
}

void WebApplicationSettingsWidget::on_webAppServerUrlResetButton_clicked() {
    ui->webAppServerUrlLineEdit->setText(WebAppClientService::getDefaultServerUrl());
}

void WebApplicationSettingsWidget::on_webAppShowTokenButton_clicked() {
    ui->webAppTokenLineEdit->setEchoMode(ui->webAppTokenLineEdit->echoMode() ==
                                                 QLineEdit::EchoMode::Password
                                             ? QLineEdit::EchoMode::Normal
                                             : QLineEdit::EchoMode::Password);
}

void WebApplicationSettingsWidget::on_webAppCopyTokenButton_clicked() {
    QApplication::clipboard()->setText(ui->webAppTokenLineEdit->text());
}

void WebApplicationSettingsWidget::on_webAppGenerateTokenButton_clicked() {
    ui->webAppTokenLineEdit->setText(Utils::Misc::generateRandomString(32));
    ui->webAppTokenLineEdit->setEchoMode(QLineEdit::EchoMode::Normal);
}

void WebApplicationSettingsWidget::on_enableWebApplicationCheckBox_toggled() {
    bool checked = ui->enableWebApplicationCheckBox->isChecked();
    ui->webAppFrame->setEnabled(checked);
}

void WebApplicationSettingsWidget::on_webAppTokenLineEdit_textChanged(const QString &arg1) {
    ui->qrCodeWidget->setText(QStringLiteral("qontoken://") + arg1);
}

void WebApplicationSettingsWidget::on_showQRCodeButton_clicked() {
    ui->showQRCodeButton->hide();
    ui->qrCodeWidget->show();
}

void WebApplicationSettingsWidget::on_webAppConnectionNameResetButton_clicked() {
    ui->webAppConnectionNameLineEdit->setText(WebAppClientService::generateDefaultConnectionName());
}

void WebApplicationSettingsWidget::on_webAppTestConnectionButton_clicked() {
    auto *service = WebAppClientService::instance();
    const bool connected = service != nullptr && service->checkIsConnected();
    if (connected) {
        QMessageBox::information(this, tr("Connection test"),
                                 tr("Successfully connected to the web application server."));
    } else {
        QMessageBox::warning(this, tr("Connection test"),
                             tr("Not connected to the web application server. "
                                "Please check the server URL and your network connection."));
    }
}

void WebApplicationSettingsWidget::on_refreshConnectedDevicesButton_clicked() {
    // Request the list of connected devices from the service
    auto *service = WebAppClientService::instance();
    if (service != nullptr) {
        service->sendRequestConnectedDevices();
    }
}

void WebApplicationSettingsWidget::updateConnectedDevices(const QStringList &deviceNames) {
    ui->connectedDevicesListWidget->clear();
    for (const QString &name : deviceNames) {
        ui->connectedDevicesListWidget->addItem(name);
    }
}
