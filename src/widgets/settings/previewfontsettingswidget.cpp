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

#include "previewfontsettingswidget.h"

#include <QFontDatabase>
#include <QFontDialog>
#include <QLineEdit>
#include <QTextBrowser>
#include <QTextEdit>

#include "services/settingsservice.h"
#include "ui_previewfontsettingswidget.h"
#include "utils/gui.h"
#include "utils/misc.h"

PreviewFontSettingsWidget::PreviewFontSettingsWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::PreviewFontSettingsWidget) {
    ui->setupUi(this);
}

PreviewFontSettingsWidget::~PreviewFontSettingsWidget() { delete ui; }

void PreviewFontSettingsWidget::readSettings() {
    SettingsService settings;

    // Rendering settings
    ui->noteTextViewRTLCheckBox->setChecked(
        settings.value(QStringLiteral("MainWindow/noteTextView.rtl")).toBool());
    ui->noteTextViewIgnoreCodeFontSizeCheckBox->setChecked(
        settings.value(QStringLiteral("MainWindow/noteTextView.ignoreCodeFontSize"), true)
            .toBool());
    ui->noteTextViewUnderlineCheckBox->setChecked(
        settings.value(QStringLiteral("MainWindow/noteTextView.underline"), true).toBool());
    ui->noteTextViewUseEditorStylesCheckBox->setChecked(Utils::Misc::isPreviewUseEditorStyles());
    ui->noteTextViewRefreshDebounceTimeSpinBox->setValue(
        Utils::Misc::getPreviewRefreshDebounceTime());

    // Exporting settings
    ui->useInternalExportStylingCheckBox->setChecked(
        Utils::Misc::useInternalExportStylingForPreview());

    // Load note text view font
    QString fontString = settings.value(QStringLiteral("MainWindow/noteTextView.font")).toString();

    // Store the current font if there isn't any set yet
    if (fontString.isEmpty()) {
        auto *textEdit = new QTextEdit();
        fontString = textEdit->font().toString();
        settings.setValue(QStringLiteral("MainWindow/noteTextView.font"), fontString);
        delete textEdit;
    }

    noteTextViewFont.fromString(fontString);
    setFontLabel(ui->noteTextViewFontLabel, noteTextViewFont);

    // Load export font, falling back to the previous preview-font behavior
    fontString = settings
                     .value(QStringLiteral("MainWindow/noteTextView.export.font"),
                            Utils::Misc::previewFontString())
                     .toString();
    noteTextViewExportFont.fromString(fontString);
    setFontLabel(ui->noteTextViewExportFontLabel, noteTextViewExportFont);

    // Load note text view code font
    fontString = settings.value(QStringLiteral("MainWindow/noteTextView.code.font")).toString();

    // Set a default note text view code font
    if (fontString.isEmpty()) {
        // Reset the note text view code font
        on_noteTextViewCodeResetButton_clicked();

        fontString = noteTextViewCodeFont.toString();
        settings.setValue(QStringLiteral("MainWindow/noteTextView.code.font"), fontString);
    } else {
        noteTextViewCodeFont.fromString(fontString);
    }

    setFontLabel(ui->noteTextViewCodeFontLabel, noteTextViewCodeFont);
}

void PreviewFontSettingsWidget::storeSettings() {
    SettingsService settings;

    // Rendering settings
    settings.setValue(QStringLiteral("MainWindow/noteTextView.rtl"),
                      ui->noteTextViewRTLCheckBox->isChecked());
    settings.setValue(QStringLiteral("MainWindow/noteTextView.ignoreCodeFontSize"),
                      ui->noteTextViewIgnoreCodeFontSizeCheckBox->isChecked());
    settings.setValue(QStringLiteral("MainWindow/noteTextView.underline"),
                      ui->noteTextViewUnderlineCheckBox->isChecked());
    settings.setValue(QStringLiteral("MainWindow/noteTextView.useEditorStyles"),
                      ui->noteTextViewUseEditorStylesCheckBox->isChecked());
    settings.setValue(QStringLiteral("MainWindow/noteTextView.useInternalExportStyling"),
                      ui->useInternalExportStylingCheckBox->isChecked());
    settings.setValue(QStringLiteral("MainWindow/noteTextView.refreshDebounceTime"),
                      ui->noteTextViewRefreshDebounceTimeSpinBox->value());

    // Font settings
    settings.setValue(QStringLiteral("MainWindow/noteTextView.font"), noteTextViewFont.toString());
    settings.setValue(QStringLiteral("MainWindow/noteTextView.code.font"),
                      noteTextViewCodeFont.toString());
    settings.setValue(QStringLiteral("MainWindow/noteTextView.export.font"),
                      noteTextViewExportFont.toString());
}

void PreviewFontSettingsWidget::on_noteTextViewButton_clicked() {
    bool ok;
    QFont font = Utils::Gui::fontDialogGetFont(&ok, noteTextViewFont, this);
    if (ok) {
        noteTextViewFont = font;
        setFontLabel(ui->noteTextViewFontLabel, noteTextViewFont);
    }
}

void PreviewFontSettingsWidget::on_noteTextViewCodeButton_clicked() {
    bool ok;
    QFont font = Utils::Gui::fontDialogGetFont(&ok, noteTextViewCodeFont, this, QString(),
                                               QFontDialog::MonospacedFonts);
    if (ok) {
        noteTextViewCodeFont = font;
        setFontLabel(ui->noteTextViewCodeFontLabel, noteTextViewCodeFont);
    }
}

void PreviewFontSettingsWidget::on_noteTextViewExportButton_clicked() {
    bool ok;
    QFont font = Utils::Gui::fontDialogGetFont(&ok, noteTextViewExportFont, this);
    if (ok) {
        noteTextViewExportFont = font;
        setFontLabel(ui->noteTextViewExportFontLabel, noteTextViewExportFont);
    }
}

/**
 * Resets the font for the note Markdown view
 */
void PreviewFontSettingsWidget::on_noteTextViewResetButton_clicked() {
    QTextBrowser textView;
    noteTextViewFont = textView.font();
    setFontLabel(ui->noteTextViewFontLabel, noteTextViewFont);
}

/**
 * Resets the font for the note Markdown code view
 */
void PreviewFontSettingsWidget::on_noteTextViewCodeResetButton_clicked() {
    noteTextViewCodeFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    setFontLabel(ui->noteTextViewCodeFontLabel, noteTextViewCodeFont);
}

void PreviewFontSettingsWidget::on_noteTextViewExportResetButton_clicked() {
    noteTextViewExportFont = noteTextViewFont;
    setFontLabel(ui->noteTextViewExportFontLabel, noteTextViewExportFont);
}

void PreviewFontSettingsWidget::on_noteTextViewUseEditorStylesCheckBox_toggled(bool checked) {
    ui->previewFontsGroupBox->setDisabled(checked);
}

void PreviewFontSettingsWidget::on_noteTextViewRefreshDebounceTimeResetButton_clicked() {
    ui->noteTextViewRefreshDebounceTimeSpinBox->setValue(600);
}

void PreviewFontSettingsWidget::setFontLabel(QLineEdit *label, const QFont &font) {
    label->setText(font.family() + " (" + QString::number(font.pointSize()) + ")");
    label->setFont(font);
}
