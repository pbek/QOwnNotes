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

#include "editorsettingswidget.h"

#include <QSignalBlocker>

#include "services/settingsservice.h"
#include "ui_editorsettingswidget.h"
#include "utils/misc.h"

EditorSettingsWidget::EditorSettingsWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::EditorSettingsWidget) {
    ui->setupUi(this);
}

EditorSettingsWidget::~EditorSettingsWidget() { delete ui; }

/**
 * Initializes the editor settings widget (lazy init, called once on first page visit)
 */
void EditorSettingsWidget::initialize() {
    // Connect needRestart signals for settings that require application restart
    connect(ui->fullyHighlightedBlockquotesCheckBox, &QCheckBox::toggled, this,
            &EditorSettingsWidget::needRestart);
    connect(ui->vimModeCheckBox, &QCheckBox::toggled, this, &EditorSettingsWidget::needRestart);
    connect(ui->disableCursorBlinkingCheckBox, &QCheckBox::toggled, this,
            &EditorSettingsWidget::needRestart);
}

/**
 * Reads and applies all editor settings from QSettings
 */
void EditorSettingsWidget::readSettings() {
    SettingsService settings;

    ui->markdownHighlightingCheckBox->setChecked(
        settings.value(QStringLiteral("markdownHighlightingEnabled"), true).toBool());
    ui->fullyHighlightedBlockquotesCheckBox->setChecked(
        settings.value(QStringLiteral("fullyHighlightedBlockquotes")).toBool());
    ui->autoBracketClosingCheckBox->setChecked(
        settings.value(QStringLiteral("Editor/autoBracketClosing"), true).toBool());
    ui->autoBracketRemovalCheckBox->setChecked(
        settings.value(QStringLiteral("Editor/autoBracketRemoval"), true).toBool());
    ui->removeTrailingSpacesCheckBox->setChecked(
        settings.value(QStringLiteral("Editor/removeTrailingSpaces")).toBool());
    ui->showLineNumbersInEditorCheckBox->setChecked(
        settings.value(QStringLiteral("Editor/showLineNumbers")).toBool());
    ui->showNoteTextStatsCheckBox->setChecked(
        settings.value(QStringLiteral("Editor/showNoteTextStats"), false).toBool());
    ui->highlightCurrentLineCheckBox->setChecked(
        settings.value(QStringLiteral("Editor/highlightCurrentLine"), true).toBool());
    ui->headingFoldingCheckBox->setChecked(
        settings.value(QStringLiteral("Editor/headingFolding"), false).toBool());
    ui->hideFormattingSyntaxCheckBox->setChecked(
        settings.value(QStringLiteral("Editor/hideFormattingSyntax"), false).toBool());
    ui->enableWikiLinkSupportCheckBox->setChecked(
        settings.value(QStringLiteral("Editor/wikiLinkSupport"), false).toBool());
    // Initialize dependent widget state and emit cross-page signal
    on_enableWikiLinkSupportCheckBox_toggled(ui->enableWikiLinkSupportCheckBox->isChecked());
    ui->wikiLinkFileNameAutoSelectCheckBox->setChecked(
        settings.value(QStringLiteral("Editor/wikiLinkFileNameAutoSelect"), false).toBool());
    ui->hangingIndentCheckBox->setChecked(
        settings.value(QStringLiteral("Editor/hangingIndent"), false).toBool());
    ui->showMarkdownImagePreviewsCheckBox->setChecked(
        settings.value(QStringLiteral("Editor/showMarkdownImagePreviews"), true).toBool());
    ui->editorWidthInDFMOnlyCheckBox->setChecked(
        settings.value(QStringLiteral("Editor/editorWidthInDFMOnly"), true).toBool());
    ui->vimModeCheckBox->setChecked(settings.value(QStringLiteral("Editor/vimMode")).toBool());
    ui->disableCursorBlinkingCheckBox->setChecked(
        settings.value(QStringLiteral("Editor/disableCursorBlinking")).toBool());
    ui->useTabIndentCheckBox->setChecked(
        settings.value(QStringLiteral("Editor/useTabIndent")).toBool());
    ui->indentSizeSpinBox->setValue(Utils::Misc::indentSize());
    ui->cursorWidthSpinBox->setValue(settings.value(QStringLiteral("cursorWidth"), 1).toInt());
    ui->timeFormatLineEdit->setText(settings.value(QStringLiteral("insertTimeFormat")).toString());
}

/**
 * Stores all editor settings to QSettings
 */
void EditorSettingsWidget::storeSettings() {
    SettingsService settings;

    settings.setValue(QStringLiteral("markdownHighlightingEnabled"),
                      ui->markdownHighlightingCheckBox->isChecked());
    settings.setValue(QStringLiteral("fullyHighlightedBlockquotes"),
                      ui->fullyHighlightedBlockquotesCheckBox->isChecked());
    settings.setValue(QStringLiteral("Editor/autoBracketClosing"),
                      ui->autoBracketClosingCheckBox->isChecked());
    settings.setValue(QStringLiteral("Editor/autoBracketRemoval"),
                      ui->autoBracketRemovalCheckBox->isChecked());
    settings.setValue(QStringLiteral("Editor/removeTrailingSpaces"),
                      ui->removeTrailingSpacesCheckBox->isChecked());
    settings.setValue(QStringLiteral("Editor/showLineNumbers"),
                      ui->showLineNumbersInEditorCheckBox->isChecked());
    settings.setValue(QStringLiteral("Editor/showNoteTextStats"),
                      ui->showNoteTextStatsCheckBox->isChecked());
    settings.setValue(QStringLiteral("Editor/highlightCurrentLine"),
                      ui->highlightCurrentLineCheckBox->isChecked());
    settings.setValue(QStringLiteral("Editor/headingFolding"),
                      ui->headingFoldingCheckBox->isChecked());
    settings.setValue(QStringLiteral("Editor/hideFormattingSyntax"),
                      ui->hideFormattingSyntaxCheckBox->isChecked());
    settings.setValue(QStringLiteral("Editor/wikiLinkSupport"),
                      ui->enableWikiLinkSupportCheckBox->isChecked());
    settings.setValue(QStringLiteral("Editor/wikiLinkFileNameAutoSelect"),
                      ui->wikiLinkFileNameAutoSelectCheckBox->isChecked());
    settings.setValue(QStringLiteral("Editor/hangingIndent"),
                      ui->hangingIndentCheckBox->isChecked());
    settings.setValue(QStringLiteral("Editor/showMarkdownImagePreviews"),
                      ui->showMarkdownImagePreviewsCheckBox->isChecked());
    settings.setValue(QStringLiteral("Editor/editorWidthInDFMOnly"),
                      ui->editorWidthInDFMOnlyCheckBox->isChecked());
    settings.setValue(QStringLiteral("Editor/vimMode"), ui->vimModeCheckBox->isChecked());
    settings.setValue(QStringLiteral("Editor/disableCursorBlinking"),
                      ui->disableCursorBlinkingCheckBox->isChecked());
    settings.setValue(QStringLiteral("Editor/useTabIndent"), ui->useTabIndentCheckBox->isChecked());
    settings.setValue(QStringLiteral("Editor/indentSize"), ui->indentSizeSpinBox->value());
    settings.setValue(QStringLiteral("cursorWidth"), ui->cursorWidthSpinBox->value());
    settings.setValue(QStringLiteral("insertTimeFormat"), ui->timeFormatLineEdit->text());
}

/**
 * Resets the cursor width spin box to its default value
 */
void EditorSettingsWidget::on_cursorWidthResetButton_clicked() {
    ui->cursorWidthSpinBox->setValue(1);
}

/**
 * Enables or disables the fully highlighted blockquotes checkbox
 */
void EditorSettingsWidget::on_markdownHighlightingCheckBox_toggled(bool checked) {
    ui->fullyHighlightedBlockquotesCheckBox->setEnabled(checked);
}

/**
 * Enables or disables the wiki link filename auto-select checkbox and emits
 * a cross-page signal so the editor font color widget can update its visibility
 */
void EditorSettingsWidget::on_enableWikiLinkSupportCheckBox_toggled(bool checked) {
    ui->wikiLinkFileNameAutoSelectCheckBox->setEnabled(checked);
    emit wikiLinkSupportToggled(checked);
}

/**
 * Forces editorWidthInDFMOnly to be checked when line numbers are enabled,
 * since line numbers require the editor to fill the full width
 */
void EditorSettingsWidget::on_showLineNumbersInEditorCheckBox_toggled(bool checked) {
    if (checked && !ui->editorWidthInDFMOnlyCheckBox->isChecked()) {
        const QSignalBlocker blocker(ui->editorWidthInDFMOnlyCheckBox);
        ui->editorWidthInDFMOnlyCheckBox->setChecked(true);
    }
}

/**
 * Unchecks show line numbers when editorWidthInDFMOnly is turned off,
 * since line numbers require full editor width
 */
void EditorSettingsWidget::on_editorWidthInDFMOnlyCheckBox_toggled(bool checked) {
    if (!checked && ui->showLineNumbersInEditorCheckBox->isChecked()) {
        const QSignalBlocker blocker(ui->showLineNumbersInEditorCheckBox);
        ui->showLineNumbersInEditorCheckBox->setChecked(false);
    }
}
