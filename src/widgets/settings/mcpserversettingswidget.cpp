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

#include "mcpserversettingswidget.h"

#include <QApplication>
#include <QClipboard>

#include "services/cryptoservice.h"
#include "services/mcpservice.h"
#include "services/settingsservice.h"
#include "ui_mcpserversettingswidget.h"
#include "utils/misc.h"

McpServerSettingsWidget::McpServerSettingsWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::McpServerSettingsWidget) {
    ui->setupUi(this);
}

McpServerSettingsWidget::~McpServerSettingsWidget() { delete ui; }

/**
 * @brief Connects signals for the MCP server settings page
 */
void McpServerSettingsWidget::initialize() {
    connect(ui->mcpServerEnabledCheckBox, SIGNAL(toggled(bool)), this, SIGNAL(needRestart()));
    connect(ui->mcpServerTokenLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(needRestart()));
}

/**
 * @brief Reads the MCP server settings from QSettings
 */
void McpServerSettingsWidget::readSettings() {
    ui->mcpServerEnabledCheckBox->setChecked(McpService::isEnabled());
    ui->mcpServerPortSpinBox->setValue(McpService::getPort());
    ui->mcpServerTokenLineEdit->setText(McpService::getOrGenerateToken());
    on_mcpServerEnabledCheckBox_toggled(ui->mcpServerEnabledCheckBox->isChecked());
}

/**
 * @brief Stores the MCP server settings to QSettings
 */
void McpServerSettingsWidget::storeSettings() {
    SettingsService settings;
    settings.setValue(QStringLiteral("ai/mcpServerEnabled"),
                      ui->mcpServerEnabledCheckBox->isChecked());
    settings.setValue(QStringLiteral("ai/mcpServerPort"), ui->mcpServerPortSpinBox->value());
    QString mcpServerToken = ui->mcpServerTokenLineEdit->text().trimmed();
    if (mcpServerToken.isEmpty()) {
        mcpServerToken = Utils::Misc::generateRandomString(32);
        ui->mcpServerTokenLineEdit->setText(mcpServerToken);
    }
    settings.setValue(QStringLiteral("ai/mcpServerToken"),
                      CryptoService::instance()->encryptToString(
                          mcpServerToken, QStringLiteral("settings/ai/mcpServerToken")));
}

void McpServerSettingsWidget::on_mcpServerEnabledCheckBox_toggled(bool checked) {
    ui->mcpServerPortLabel->setEnabled(checked);
    ui->mcpServerPortSpinBox->setEnabled(checked);
    ui->mcpServerPortResetButton->setEnabled(checked);
    ui->mcpServerTokenLabel->setEnabled(checked);
    ui->mcpServerTokenLineEdit->setEnabled(checked);
    ui->mcpServerShowTokenButton->setEnabled(checked);
    ui->mcpServerCopyTokenButton->setEnabled(checked);
    ui->mcpServerGenerateTokenButton->setEnabled(checked);
}

void McpServerSettingsWidget::on_mcpServerPortResetButton_clicked() {
    ui->mcpServerPortSpinBox->setValue(McpService::getDefaultPort());
}

void McpServerSettingsWidget::on_mcpServerShowTokenButton_clicked() {
    ui->mcpServerTokenLineEdit->setEchoMode(ui->mcpServerTokenLineEdit->echoMode() ==
                                                    QLineEdit::EchoMode::Password
                                                ? QLineEdit::EchoMode::Normal
                                                : QLineEdit::EchoMode::Password);
}

void McpServerSettingsWidget::on_mcpServerCopyTokenButton_clicked() {
    QApplication::clipboard()->setText(ui->mcpServerTokenLineEdit->text());
}

void McpServerSettingsWidget::on_mcpServerGenerateTokenButton_clicked() {
    ui->mcpServerTokenLineEdit->setText(Utils::Misc::generateRandomString(32));
    ui->mcpServerTokenLineEdit->setEchoMode(QLineEdit::EchoMode::Normal);
}
