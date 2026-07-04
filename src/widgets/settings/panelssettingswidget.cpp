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

#include "panelssettingswidget.h"

#include "entities/notesubfolder.h"
#include "mainwindow.h"
#include "services/settingsservice.h"
#include "ui_panelssettingswidget.h"
#include "utils/misc.h"

PanelsSettingsWidget::PanelsSettingsWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::PanelsSettingsWidget) {
    ui->setupUi(this);
}

PanelsSettingsWidget::~PanelsSettingsWidget() { delete ui; }

/**
 * @brief Connects signals for the panels settings page
 */
void PanelsSettingsWidget::initialize() {
    // Connect the panel sort radio buttons
    connect(ui->notesPanelSortAlphabeticalRadioButton, SIGNAL(toggled(bool)),
            ui->notesPanelOrderGroupBox, SLOT(setEnabled(bool)));
    connect(ui->noteSubfoldersPanelShowRootFolderNameCheckBox, SIGNAL(toggled(bool)),
            ui->noteSubfoldersPanelShowFullPathCheckBox, SLOT(setEnabled(bool)));
    connect(ui->noteSubfoldersPanelSortAlphabeticalRadioButton, SIGNAL(toggled(bool)),
            ui->noteSubfoldersPanelOrderGroupBox, SLOT(setEnabled(bool)));
    connect(ui->tagsPanelSortAlphabeticalRadioButton, SIGNAL(toggled(bool)),
            ui->tagsPanelOrderGroupBox, SLOT(setEnabled(bool)));

    // Append "(experimental)" label to the note list preview checkbox
    if (!ui->noteListPreviewCheckBox->text().contains(QLatin1String("(experimental)"))) {
        ui->noteListPreviewCheckBox->setText(ui->noteListPreviewCheckBox->text() +
                                             " (experimental)");
    }

    // Connect needRestart signals
    connect(ui->noteFolderButtonsCheckBox, SIGNAL(toggled(bool)), this, SIGNAL(needRestart()));
    connect(ui->noteListPreviewCheckBox, SIGNAL(toggled(bool)), this, SIGNAL(needRestart()));
    connect(ui->maxNoteFileSizeSpinBox, SIGNAL(valueChanged(int)), this, SIGNAL(needRestart()));
    connect(ui->enableNoteTreeCheckBox, SIGNAL(toggled(bool)), this, SIGNAL(needRestart()));
    connect(ui->ignoreNoteSubFoldersLineEdit, SIGNAL(textChanged(QString)), this,
            SIGNAL(needRestart()));
    connect(ui->detectLeadingEmojiCheckBox, SIGNAL(toggled(bool)), this, SIGNAL(needRestart()));
}

/**
 * @brief Reads the panels settings from QSettings
 */
void PanelsSettingsWidget::readSettings() {
    SettingsService settings;

    // Notes Panel Options
    if (settings.value(QStringLiteral("notesPanelSort"), SORT_BY_LAST_CHANGE).toInt() ==
        SORT_ALPHABETICAL) {
        ui->notesPanelSortAlphabeticalRadioButton->setChecked(true);
        ui->notesPanelOrderGroupBox->setEnabled(true);
    } else {
        ui->notesPanelSortByLastChangeRadioButton->setChecked(true);
        ui->notesPanelOrderGroupBox->setEnabled(false);
    }
    settings.value(QStringLiteral("notesPanelOrder")).toInt() == ORDER_DESCENDING
        ? ui->notesPanelOrderDescendingRadioButton->setChecked(true)
        : ui->notesPanelOrderAscendingRadioButton->setChecked(true);

    // Note Subfolders Panel Options
    ui->noteSubfoldersPanelHideSearchCheckBox->setChecked(
        settings.value(QStringLiteral("noteSubfoldersPanelHideSearch")).toBool());

    ui->noteSubfoldersPanelDisplayAsFullTreeCheckBox->setChecked(
        settings.value(QStringLiteral("noteSubfoldersPanelDisplayAsFullTree"), true).toBool());

    ui->noteSubfoldersPanelShowNotesRecursivelyCheckBox->setChecked(
        settings.value(QStringLiteral("noteSubfoldersPanelShowNotesRecursively")).toBool());

    ui->disableSavedSearchesAutoCompletionCheckBox->setChecked(
        settings.value(QStringLiteral("disableSavedSearchesAutoCompletion")).toBool());

    ui->showMatchesCheckBox->setChecked(
        settings.value(QStringLiteral("showMatches"), true).toBool());

    ui->noteSearchPanelOpenCreatedNotesInNewTabCheckBox->setChecked(
        settings.value(QStringLiteral("noteSearchPanelOpenCreatedNotesInNewTab")).toBool());

    if (settings.value(QStringLiteral("noteSubfoldersPanelShowRootFolderName"), true).toBool()) {
        ui->noteSubfoldersPanelShowRootFolderNameCheckBox->setChecked(true);
        ui->noteSubfoldersPanelShowFullPathCheckBox->setEnabled(true);
    } else {
        ui->noteSubfoldersPanelShowRootFolderNameCheckBox->setChecked(false);
        ui->noteSubfoldersPanelShowFullPathCheckBox->setEnabled(false);
    }

    ui->noteSubfoldersPanelTabsUnsetAllNotesSelectionCheckBox->setChecked(
        settings.value(QStringLiteral("noteSubfoldersPanelTabsUnsetAllNotesSelection")).toBool());

    ui->noteSubfoldersPanelShowFullPathCheckBox->setChecked(
        settings.value(QStringLiteral("noteSubfoldersPanelShowFullPath")).toBool());

    if (settings.value(QStringLiteral("noteSubfoldersPanelSort")).toInt() == SORT_ALPHABETICAL) {
        ui->noteSubfoldersPanelSortAlphabeticalRadioButton->setChecked(true);
        ui->noteSubfoldersPanelOrderGroupBox->setEnabled(true);
    } else {
        ui->noteSubfoldersPanelSortByLastChangeRadioButton->setChecked(true);
        ui->noteSubfoldersPanelOrderGroupBox->setEnabled(false);
    }

    settings.value(QStringLiteral("noteSubfoldersPanelOrder")).toInt() == ORDER_DESCENDING
        ? ui->noteSubfoldersPanelOrderDescendingRadioButton->setChecked(true)
        : ui->noteSubfoldersPanelOrderAscendingRadioButton->setChecked(true);

    // Tags Panel Options
    ui->tagsPanelHideSearchCheckBox->setChecked(
        settings.value(QStringLiteral("tagsPanelHideSearch")).toBool());
    ui->tagsPanelHideNoteCountCheckBox->setChecked(
        settings.value(QStringLiteral("tagsPanelHideNoteCount"), false).toBool());

    ui->taggingShowNotesRecursivelyCheckBox->setChecked(
        settings.value(QStringLiteral("taggingShowNotesRecursively")).toBool());
    ui->noteListPreviewCheckBox->setChecked(Utils::Misc::isNoteListPreview());
    ui->allowEmptyNotesCheckBox->setChecked(
        settings.value(QStringLiteral("allowEmptyNotes"), true).toBool());
    ui->detectLeadingEmojiCheckBox->setChecked(
        settings.value(QStringLiteral("detectLeadingEmojiInNoteTitle"), true).toBool());
    ui->maxNoteFileSizeSpinBox->setValue(Utils::Misc::getMaximumNoteFileSize() / 1024);

    if (settings.value(QStringLiteral("tagsPanelSort")).toInt() == SORT_ALPHABETICAL) {
        ui->tagsPanelSortAlphabeticalRadioButton->setChecked(true);
        ui->tagsPanelOrderGroupBox->setEnabled(true);
    } else {
        ui->tagsPanelSortByLastChangeRadioButton->setChecked(true);
        ui->tagsPanelOrderGroupBox->setEnabled(false);
    }

    settings.value(QStringLiteral("tagsPanelOrder")).toInt() == ORDER_DESCENDING
        ? ui->tagsPanelOrderDescendingRadioButton->setChecked(true)
        : ui->tagsPanelOrderAscendingRadioButton->setChecked(true);

    ui->ignoreNoteSubFoldersLineEdit->setText(
        settings.value(QStringLiteral("ignoreNoteSubFolders"), IGNORED_NOTE_SUBFOLDERS_DEFAULT)
            .toString());

    ui->ignoredNoteFilesLineEdit->setText(
        settings.value(QStringLiteral("ignoredNoteFiles")).toString());

    // Navigation Panel Options
    ui->navigationPanelHideSearchCheckBox->setChecked(
        settings.value(QStringLiteral("navigationPanelHideSearch")).toBool());

    ui->navigationPanelAutoSelectCheckBox->setChecked(
        settings.value(QStringLiteral("navigationPanelAutoSelect"), true).toBool());

    ui->enableNoteTreeCheckBox->setChecked(Utils::Misc::isEnableNoteTree());

    // Note folder panel options (previously in main readSettings)
    ui->noteEditCentralWidgetCheckBox->setChecked(
        settings.value(QStringLiteral("noteEditIsCentralWidget"), true).toBool());
    ui->restoreNoteTabsCheckBox->setChecked(
        settings.value(QStringLiteral("restoreNoteTabs"), true).toBool());
    ui->hideTabCloseButtonCheckBox->setChecked(
        settings.value(QStringLiteral("hideTabCloseButton")).toBool());
    ui->noteFolderButtonsCheckBox->setChecked(
        settings.value(QStringLiteral("useNoteFolderButtons")).toBool());
}

/**
 * @brief Stores the panels settings to QSettings
 */
void PanelsSettingsWidget::storeSettings() {
    SettingsService settings;

    // Notes Panel Options
    ui->notesPanelSortAlphabeticalRadioButton->isChecked()
        ? settings.setValue(QStringLiteral("notesPanelSort"), SORT_ALPHABETICAL)
        : settings.setValue(QStringLiteral("notesPanelSort"), SORT_BY_LAST_CHANGE);
    ui->notesPanelOrderDescendingRadioButton->isChecked()
        ? settings.setValue(QStringLiteral("notesPanelOrder"), ORDER_DESCENDING)
        : settings.setValue(QStringLiteral("notesPanelOrder"), ORDER_ASCENDING);

    // Note Subfolders Panel Options
    settings.setValue(QStringLiteral("noteSubfoldersPanelHideSearch"),
                      ui->noteSubfoldersPanelHideSearchCheckBox->isChecked());

    settings.setValue(QStringLiteral("noteSubfoldersPanelDisplayAsFullTree"),
                      ui->noteSubfoldersPanelDisplayAsFullTreeCheckBox->isChecked());

    settings.setValue(QStringLiteral("noteSubfoldersPanelShowRootFolderName"),
                      ui->noteSubfoldersPanelShowRootFolderNameCheckBox->isChecked());

    settings.setValue(QStringLiteral("noteSubfoldersPanelShowNotesRecursively"),
                      ui->noteSubfoldersPanelShowNotesRecursivelyCheckBox->isChecked());

    settings.setValue(QStringLiteral("disableSavedSearchesAutoCompletion"),
                      ui->disableSavedSearchesAutoCompletionCheckBox->isChecked());

    settings.setValue(QStringLiteral("showMatches"), ui->showMatchesCheckBox->isChecked());

    settings.setValue(QStringLiteral("noteSearchPanelOpenCreatedNotesInNewTab"),
                      ui->noteSearchPanelOpenCreatedNotesInNewTabCheckBox->isChecked());

    settings.setValue(QStringLiteral("noteSubfoldersPanelShowFullPath"),
                      ui->noteSubfoldersPanelShowFullPathCheckBox->isChecked());

    settings.setValue(QStringLiteral("noteSubfoldersPanelTabsUnsetAllNotesSelection"),
                      ui->noteSubfoldersPanelTabsUnsetAllNotesSelectionCheckBox->isChecked());

    ui->noteSubfoldersPanelSortAlphabeticalRadioButton->isChecked()
        ? settings.setValue(QStringLiteral("noteSubfoldersPanelSort"), SORT_ALPHABETICAL)
        : settings.setValue(QStringLiteral("noteSubfoldersPanelSort"), SORT_BY_LAST_CHANGE);

    ui->noteSubfoldersPanelOrderDescendingRadioButton->isChecked()
        ? settings.setValue(QStringLiteral("noteSubfoldersPanelOrder"), ORDER_DESCENDING)
        : settings.setValue(QStringLiteral("noteSubfoldersPanelOrder"), ORDER_ASCENDING);

    const QSignalBlocker blocker(ui->ignoreNoteSubFoldersLineEdit);
    settings.setValue(QStringLiteral("ignoreNoteSubFolders"),
                      ui->ignoreNoteSubFoldersLineEdit->text());

    const QSignalBlocker blocker2(ui->ignoredNoteFilesLineEdit);
    settings.setValue(QStringLiteral("ignoredNoteFiles"), ui->ignoredNoteFilesLineEdit->text());

    // Tags Panel Options
    settings.setValue(QStringLiteral("tagsPanelHideSearch"),
                      ui->tagsPanelHideSearchCheckBox->isChecked());
    settings.setValue(QStringLiteral("tagsPanelHideNoteCount"),
                      ui->tagsPanelHideNoteCountCheckBox->isChecked());

    settings.setValue(QStringLiteral("taggingShowNotesRecursively"),
                      ui->taggingShowNotesRecursivelyCheckBox->isChecked());
    settings.setValue(QStringLiteral("noteListPreview"), ui->noteListPreviewCheckBox->isChecked());
    settings.setValue(QStringLiteral("allowEmptyNotes"), ui->allowEmptyNotesCheckBox->isChecked());
    settings.setValue(QStringLiteral("detectLeadingEmojiInNoteTitle"),
                      ui->detectLeadingEmojiCheckBox->isChecked());
    settings.setValue(QStringLiteral("maxNoteFileSize"),
                      ui->maxNoteFileSizeSpinBox->value() * 1024);

    ui->tagsPanelSortAlphabeticalRadioButton->isChecked()
        ? settings.setValue(QStringLiteral("tagsPanelSort"), SORT_ALPHABETICAL)
        : settings.setValue(QStringLiteral("tagsPanelSort"), SORT_BY_LAST_CHANGE);

    ui->tagsPanelOrderDescendingRadioButton->isChecked()
        ? settings.setValue(QStringLiteral("tagsPanelOrder"), ORDER_DESCENDING)
        : settings.setValue(QStringLiteral("tagsPanelOrder"), ORDER_ASCENDING);

    // Navigation Panel Options
    settings.setValue(QStringLiteral("navigationPanelHideSearch"),
                      ui->navigationPanelHideSearchCheckBox->isChecked());

    settings.setValue(QStringLiteral("navigationPanelAutoSelect"),
                      ui->navigationPanelAutoSelectCheckBox->isChecked());

    settings.setValue(QStringLiteral("enableNoteTree"), ui->enableNoteTreeCheckBox->isChecked());

    // Note folder panel options (previously in main storeSettings)
    settings.setValue(QStringLiteral("noteEditIsCentralWidget"),
                      ui->noteEditCentralWidgetCheckBox->isChecked());
    settings.setValue(QStringLiteral("restoreNoteTabs"), ui->restoreNoteTabsCheckBox->isChecked());
    settings.setValue(QStringLiteral("hideTabCloseButton"),
                      ui->hideTabCloseButtonCheckBox->isChecked());
    settings.setValue(QStringLiteral("useNoteFolderButtons"),
                      ui->noteFolderButtonsCheckBox->isChecked());
}

void PanelsSettingsWidget::on_ignoreNoteSubFoldersResetButton_clicked() {
    ui->ignoreNoteSubFoldersLineEdit->setText(IGNORED_NOTE_SUBFOLDERS_DEFAULT);
}
