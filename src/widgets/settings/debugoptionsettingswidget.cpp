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

#include "debugoptionsettingswidget.h"

#include <services/settingsservice.h>
#include <utils/gui.h>
#include <utils/misc.h>

#include <QDebug>
#include <QDir>
#include <QFile>

#include "ui_debugoptionsettingswidget.h"

DebugOptionSettingsWidget::DebugOptionSettingsWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::DebugOptionSettingsWidget) {
    ui->setupUi(this);
}

DebugOptionSettingsWidget::~DebugOptionSettingsWidget() { delete ui; }

void DebugOptionSettingsWidget::initialize() {
    // Show the log file path
    ui->logFileLabel->setText(QDir::toNativeSeparators(Utils::Misc::logFilePath()));
}

void DebugOptionSettingsWidget::readSettings() {
    SettingsService settings;

    ui->oldVersionNumberCheckBox->setChecked(
        settings.value(QStringLiteral("Debug/fakeOldVersionNumber")).toBool());
    ui->oldScriptVersionsCheckBox->setChecked(
        settings.value(QStringLiteral("Debug/fakeOldScriptVersions")).toBool());
    ui->fileLoggingCheckBox->setChecked(
        settings.value(QStringLiteral("Debug/fileLogging")).toBool());
    on_fileLoggingCheckBox_toggled(ui->fileLoggingCheckBox->isChecked());
    ui->scriptProfilingThresholdSpinBox->setValue(
        settings.value(QStringLiteral("Debug/scriptProfilingThreshold"), 500).toInt());
}

void DebugOptionSettingsWidget::storeSettings() {
    SettingsService settings;

    settings.setValue(QStringLiteral("Debug/fakeOldVersionNumber"),
                      ui->oldVersionNumberCheckBox->isChecked());
    settings.setValue(QStringLiteral("Debug/fakeOldScriptVersions"),
                      ui->oldScriptVersionsCheckBox->isChecked());
    settings.setValue(QStringLiteral("Debug/fileLogging"), ui->fileLoggingCheckBox->isChecked());
    settings.setValue(QStringLiteral("Debug/scriptProfilingThreshold"),
                      ui->scriptProfilingThresholdSpinBox->value());
}

void DebugOptionSettingsWidget::removeLogFile() {
    QFile file(Utils::Misc::logFilePath());
    if (file.exists()) {
        bool result = file.remove();
        QString text = result ? "Removed" : "Could not remove";

        // In case that the settings are cleared logging to log file is
        // disabled by default and it will not be created again
        qWarning() << text + " log file: " << file.fileName();
    }
}

void DebugOptionSettingsWidget::on_fileLoggingCheckBox_toggled(bool checked) {
    ui->logFileFrame->setVisible(checked);
}

void DebugOptionSettingsWidget::on_clearLogFileButton_clicked() {
    removeLogFile();

    Utils::Gui::information(this, tr("Log file cleared"),
                            tr("The log file <strong>%1</strong> was cleared"
                               ".")
                                .arg(Utils::Misc::logFilePath()),
                            QStringLiteral("log-file-cleared"));
}
