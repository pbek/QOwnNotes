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

#include "debugsettingswidget.h"

#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QFile>
#include <QTextStream>

#include "dialogs/filedialog.h"
#include "services/settingsservice.h"
#include "ui_debugsettingswidget.h"
#include "utils/gui.h"
#include "utils/misc.h"

DebugSettingsWidget::DebugSettingsWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::DebugSettingsWidget) {
    ui->setupUi(this);
}

DebugSettingsWidget::~DebugSettingsWidget() { delete ui; }

/**
 * Initializes the debug info page (search frame and settings)
 */
void DebugSettingsWidget::initialize() {
    // Init the debug info search frame
    ui->debugInfoTextEdit->initSearchFrame(ui->debugInfoTextEditSearchFrame);

    SettingsService settings;
    ui->debugInfoAnonymizeCheckBox->setChecked(
        settings.value(QStringLiteral("debugInfoAnonymize")).toBool());
}

/**
 * Generates and displays the debug information
 */
void DebugSettingsWidget::outputSettings() {
    // Let the dialog store cloud debug data first
    emit aboutToOutputSettings();

    QString output = Utils::Misc::generateDebugInformation(
        ui->gitHubLineBreaksCheckBox->isChecked(), ui->debugInfoAnonymizeCheckBox->isChecked());

    ui->debugInfoTextEdit->setPlainText(output);
}

void DebugSettingsWidget::on_gitHubLineBreaksCheckBox_toggled(bool checked) {
    Q_UNUSED(checked)
    outputSettings();
}

void DebugSettingsWidget::on_debugInfoAnonymizeCheckBox_toggled(bool checked) {
    SettingsService settings;
    settings.setValue(QStringLiteral("debugInfoAnonymize"), checked);
    outputSettings();
}

void DebugSettingsWidget::on_saveDebugInfoButton_clicked() {
    Utils::Gui::information(this, tr("Debug information"),
                            tr("Please don't use this in the issue tracker, "
                               "copy the debug information text directly into the issue."),
                            QStringLiteral("debug-save"));

    FileDialog dialog(QStringLiteral("SaveDebugInfo"));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("Markdown files") + " (*.md)");
    dialog.setWindowTitle(tr("Save debug information"));
    dialog.selectFile(QStringLiteral("QOwnNotes Debug Information.md"));
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QString fileName = dialog.selectedFile();
        QFile file(fileName);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << file.errorString();
            return;
        }

        QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        out.setCodec("UTF-8");
#endif
        out << ui->debugInfoTextEdit->toPlainText();
        file.flush();
        file.close();
    }
}

void DebugSettingsWidget::on_copyDebugInfoButton_clicked() {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->debugInfoTextEdit->toPlainText());

    Utils::Gui::information(this, tr("Debug information"),
                            tr("The debug information was copied to the clipboard."),
                            QStringLiteral("debug-clipboard"));
}

void DebugSettingsWidget::on_issueAssistantPushButton_clicked() { emit issueAssistantRequested(); }
