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

#include "generalsettingswidget.h"

#include <entities/note.h>
#include <entities/notefolder.h>
#include <utils/gui.h>
#include <utils/misc.h>

#include <QButtonGroup>
#include <QCheckBox>
#include <QInputDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>

#include "dialogs/filedialog.h"
#include "mainwindow.h"
#include "services/databaseservice.h"
#include "services/settingsservice.h"
#include "services/updateservice.h"
#include "ui_generalsettingswidget.h"
#include "widgets/settings/debugoptionsettingswidget.h"

GeneralSettingsWidget::GeneralSettingsWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::GeneralSettingsWidget) {
    ui->setupUi(this);
}

GeneralSettingsWidget::~GeneralSettingsWidget() { delete ui; }

/**
 * Initializes the general settings widget
 */
void GeneralSettingsWidget::initialize() {
    // Set the note save interval time tooltip from the label
    ui->noteSaveIntervalTime->setToolTip(ui->noteSaveIntervalTimeLabel->toolTip());

    // Disable the remove button until an item is selected
    ui->removeCustomNoteFileExtensionButton->setDisabled(true);

#ifndef Q_OS_WIN32
    // Hide the Windows-only setting on non-Windows platforms
    ui->automaticNoteFolderDatabaseClosingCheckBox->hide();
#endif

#ifdef Q_OS_MAC
    // We don't need app instance settings on macOS
    ui->appInstanceGroupBox->setVisible(false);
    ui->allowOnlyOneAppInstanceCheckBox->setChecked(false);
#endif

    // Set up the note notification button group to allow un-checking all checkboxes
    _noteNotificationButtonGroup = new QButtonGroup(this);
    _noteNotificationButtonGroup->addButton(ui->notifyAllExternalModificationsCheckBox);
    _noteNotificationButtonGroup->addButton(ui->ignoreAllExternalModificationsCheckBox);
    _noteNotificationButtonGroup->addButton(ui->acceptAllExternalModificationsCheckBox);

    // Create a hidden checkbox so we can un-check the above checkboxes
    _noteNotificationNoneCheckBox = new QCheckBox(this);
    _noteNotificationNoneCheckBox->setHidden(true);
    _noteNotificationButtonGroup->addButton(_noteNotificationNoneCheckBox);
    connect(_noteNotificationButtonGroup, SIGNAL(buttonPressed(QAbstractButton *)), this,
            SLOT(noteNotificationButtonGroupPressed(QAbstractButton *)));

    // Set up the search engine combo-box
    initSearchEngineComboBox();
}

/**
 * Reads the general settings from the settings service
 */
void GeneralSettingsWidget::readSettings() {
    SettingsService settings;

    // Prepend the portable data path if we are in portable mode
    ui->externalEditorPathLineEdit->setText(Utils::Misc::prependPortableDataPathIfNeeded(
        settings.value(QStringLiteral("externalEditorPath")).toString(), true));
    ui->externalDiffToolPathLineEdit->setText(Utils::Misc::prependPortableDataPathIfNeeded(
        settings.value(QStringLiteral("externalDiffToolPath")).toString(), true));

    ui->disableAutomaticUpdateDialogCheckBox->setChecked(
        settings.value(QStringLiteral("disableAutomaticUpdateDialog")).toBool());
    ui->notifyAllExternalModificationsCheckBox->setChecked(
        settings.value(QStringLiteral("notifyAllExternalModifications")).toBool());
    ui->ignoreAllExternalModificationsCheckBox->setChecked(
        settings.value(QStringLiteral("ignoreAllExternalModifications")).toBool());
    ui->acceptAllExternalModificationsCheckBox->setChecked(
        settings.value(QStringLiteral("acceptAllExternalModifications")).toBool());
    ui->ignoreAllExternalNoteFolderChangesCheckBox->setChecked(
        settings.value(QStringLiteral("ignoreAllExternalNoteFolderChanges")).toBool());
    ui->enableNoteChecksumChecks->setChecked(
        settings.value(QStringLiteral("enableNoteChecksumChecks"), false).toBool());
    ui->newNoteAskHeadlineCheckBox->setChecked(
        settings.value(QStringLiteral("newNoteAskHeadline")).toBool());
    ui->useUNIXNewlineCheckBox->setChecked(
        settings.value(QStringLiteral("useUNIXNewline")).toBool());

#ifdef Q_OS_MAC
    bool restoreCursorPositionDefault = false;
#else
    bool restoreCursorPositionDefault = true;
#endif
    ui->restoreCursorPositionCheckBox->setChecked(
        settings.value(QStringLiteral("restoreCursorPosition"), restoreCursorPositionDefault)
            .toBool());
    ui->restoreLastNoteAtStartupCheckBox->setChecked(
        settings.value(QStringLiteral("restoreLastNoteAtStartup"), true).toBool());
    ui->noteSaveIntervalTime->setValue(
        settings.value(QStringLiteral("noteSaveIntervalTime"), 10).toInt());
    ui->allowOnlyOneAppInstanceCheckBox->setChecked(
        settings.value(QStringLiteral("allowOnlyOneAppInstance")).toBool());

    // Load the custom note file extensions
    QListIterator<QString> itr(Note::noteFileExtensionList());
    while (itr.hasNext()) {
        const QString fileExtension = itr.next();
        addCustomNoteFileExtension(fileExtension);
    }

    const auto noteFileExtensionItems = ui->defaultNoteFileExtensionListWidget->findItems(
        Note::defaultNoteFileExtension(), Qt::MatchExactly);
    if (noteFileExtensionItems.count() > 0) {
        ui->defaultNoteFileExtensionListWidget->setCurrentItem(noteFileExtensionItems.at(0));
    }

    // Load image scaling settings
    const bool scaleImageDown = settings.value(QStringLiteral("imageScaleDown"), false).toBool();
    ui->maximumImageHeightSpinBox->setValue(
        settings.value(QStringLiteral("imageScaleDownMaximumHeight"), 1024).toInt());
    ui->maximumImageWidthSpinBox->setValue(
        settings.value(QStringLiteral("imageScaleDownMaximumWidth"), 1024).toInt());
    ui->imageScaleDownCheckBox->setChecked(scaleImageDown);
    ui->imageScalingFrame->setVisible(scaleImageDown);

    ui->automaticNoteFolderDatabaseClosingCheckBox->setChecked(
        Utils::Misc::doAutomaticNoteFolderDatabaseClosing());
    ui->legacyLinkingCheckBox->setChecked(settings.value(QStringLiteral("legacyLinking")).toBool());

    ui->stripLeadingEmojiFromNoteFilenameCheckBox->setChecked(
        settings.value(QStringLiteral("stripLeadingEmojiFromNoteFilename"), true).toBool());

    const bool enableReadOnlyMode =
        settings.value(QStringLiteral("enableReadOnlyMode"), true).toBool();
    ui->enableReadOnlyModeCheckBox->setChecked(enableReadOnlyMode);
    ui->readOnlyModeSettingsFrame->setEnabled(enableReadOnlyMode);

    ui->startInReadOnlyModeCheckBox->setChecked(
        settings.value(QStringLiteral("startInReadOnlyMode")).toBool());
    ui->autoReadOnlyModeCheckBox->setChecked(
        settings.value(QStringLiteral("autoReadOnlyMode")).toBool());
    ui->autoReadOnlyModeTimeoutSpinBox->setValue(
        settings.value(QStringLiteral("autoReadOnlyModeTimeout"), 30).toInt());

    initSearchEngineComboBox();
}

/**
 * Stores the general settings to the settings service
 */
void GeneralSettingsWidget::storeSettings() {
    SettingsService settings;
    const bool oldIgnoreAllExternalNoteFolderChanges =
        settings.value(QStringLiteral("ignoreAllExternalNoteFolderChanges")).toBool();
    const bool ignoreAllExternalNoteFolderChanges =
        ui->ignoreAllExternalNoteFolderChangesCheckBox->isChecked();

    settings.setValue(QStringLiteral("disableAutomaticUpdateDialog"),
                      ui->disableAutomaticUpdateDialogCheckBox->isChecked());
    settings.setValue(QStringLiteral("notifyAllExternalModifications"),
                      ui->notifyAllExternalModificationsCheckBox->isChecked());
    settings.setValue(QStringLiteral("ignoreAllExternalModifications"),
                      ui->ignoreAllExternalModificationsCheckBox->isChecked());
    settings.setValue(QStringLiteral("acceptAllExternalModifications"),
                      ui->acceptAllExternalModificationsCheckBox->isChecked());
    settings.setValue(QStringLiteral("ignoreAllExternalNoteFolderChanges"),
                      ignoreAllExternalNoteFolderChanges);
    settings.setValue(QStringLiteral("enableNoteChecksumChecks"),
                      ui->enableNoteChecksumChecks->isChecked());
    settings.setValue(QStringLiteral("newNoteAskHeadline"),
                      ui->newNoteAskHeadlineCheckBox->isChecked());
    settings.setValue(QStringLiteral("useUNIXNewline"), ui->useUNIXNewlineCheckBox->isChecked());

    if (oldIgnoreAllExternalNoteFolderChanges != ignoreAllExternalNoteFolderChanges) {
        MainWindow::instance()->updateNoteDirectoryWatcher();
    }

    settings.setValue(QStringLiteral("restoreCursorPosition"),
                      ui->restoreCursorPositionCheckBox->isChecked());
    settings.setValue(QStringLiteral("restoreLastNoteAtStartup"),
                      ui->restoreLastNoteAtStartupCheckBox->isChecked());
    settings.setValue(QStringLiteral("noteSaveIntervalTime"), ui->noteSaveIntervalTime->value());

    if (ui->defaultNoteFileExtensionListWidget->currentItem() != nullptr) {
        settings.setValue(QStringLiteral("defaultNoteFileExtension"),
                          ui->defaultNoteFileExtensionListWidget->currentItem()->text());
    }

    // Store the custom note file extensions
    QStringList noteFileExtensionList;
    for (int i = 0; i < ui->defaultNoteFileExtensionListWidget->count(); i++) {
        QListWidgetItem *item = ui->defaultNoteFileExtensionListWidget->item(i);
        noteFileExtensionList.append(item->text());
    }
    noteFileExtensionList.removeDuplicates();
    settings.setValue(QStringLiteral("noteFileExtensionList"), noteFileExtensionList);

    // Make the path relative to the portable data path if we are in portable mode
    settings.setValue(QStringLiteral("externalEditorPath"),
                      Utils::Misc::makePathRelativeToPortableDataPathIfNeeded(
                          ui->externalEditorPathLineEdit->text()));
    settings.setValue(QStringLiteral("externalDiffToolPath"),
                      Utils::Misc::makePathRelativeToPortableDataPathIfNeeded(
                          ui->externalDiffToolPathLineEdit->text()));

    settings.setValue(QStringLiteral("allowOnlyOneAppInstance"),
                      ui->allowOnlyOneAppInstanceCheckBox->isChecked());

    // Store image scaling settings
    settings.setValue(QStringLiteral("imageScaleDown"), ui->imageScaleDownCheckBox->isChecked());
    settings.setValue(QStringLiteral("imageScaleDownMaximumHeight"),
                      ui->maximumImageHeightSpinBox->value());
    settings.setValue(QStringLiteral("imageScaleDownMaximumWidth"),
                      ui->maximumImageWidthSpinBox->value());

    settings.setValue(QStringLiteral("SearchEngineId"),
                      ui->searchEngineSelectionComboBox->currentData().toInt());

    settings.setValue(QStringLiteral("automaticNoteFolderDatabaseClosing"),
                      ui->automaticNoteFolderDatabaseClosingCheckBox->isChecked());
    settings.setValue(QStringLiteral("legacyLinking"), ui->legacyLinkingCheckBox->isChecked());
    settings.setValue(QStringLiteral("stripLeadingEmojiFromNoteFilename"),
                      ui->stripLeadingEmojiFromNoteFilenameCheckBox->isChecked());
    settings.setValue(QStringLiteral("enableReadOnlyMode"),
                      ui->enableReadOnlyModeCheckBox->isChecked());
    settings.setValue(QStringLiteral("startInReadOnlyMode"),
                      ui->startInReadOnlyModeCheckBox->isChecked());
    settings.setValue(QStringLiteral("autoReadOnlyMode"),
                      ui->autoReadOnlyModeCheckBox->isChecked());
    settings.setValue(QStringLiteral("autoReadOnlyModeTimeout"),
                      ui->autoReadOnlyModeTimeoutSpinBox->value());
}

/**
 * Sets the allowOnlyOneAppInstance checkbox (called when system tray is enabled)
 */
void GeneralSettingsWidget::setAllowOnlyOneAppInstance(bool checked) {
    ui->allowOnlyOneAppInstanceCheckBox->setChecked(checked);
}

/**
 * Sets up the search engine combo-box
 */
void GeneralSettingsWidget::initSearchEngineComboBox() {
    SettingsService settings;

    const QHash<int, Utils::Misc::SearchEngine> searchEngines =
        Utils::Misc::getSearchEnginesHashMap();

    ui->searchEngineSelectionComboBox->clear();

    Q_FOREACH (int id, Utils::Misc::getSearchEnginesIds()) {
        const Utils::Misc::SearchEngine searchEngine = searchEngines[id];
        ui->searchEngineSelectionComboBox->addItem(searchEngine.name, QString::number(id));
    }

    const int savedEngineId =
        settings.value(QStringLiteral("SearchEngineId"), Utils::Misc::getDefaultSearchEngineId())
            .toInt();
    int savedEngineIndex =
        ui->searchEngineSelectionComboBox->findData(QVariant(savedEngineId).toString());
    savedEngineIndex = (savedEngineIndex == -1) ? 0 : savedEngineIndex;
    ui->searchEngineSelectionComboBox->setCurrentIndex(savedEngineIndex);
}

void GeneralSettingsWidget::on_reinitializeDatabaseButton_clicked() {
    if (QMessageBox::question(this, tr("Database"),
                              tr("Do you really want to clear the local database? "
                                 "This will also remove your configured note "
                                 "folders and your cached todo items!"),
                              QMessageBox::Yes | QMessageBox::Cancel,
                              QMessageBox::Cancel) == QMessageBox::Yes) {
        DatabaseService::reinitializeDiskDatabase();
        NoteFolder::migrateToNoteFolders();

        Utils::Gui::information(
            this, tr("Database"),
            tr("The Database was reinitialized. Please restart the application now!"),
            QStringLiteral("database-reinitialized"));
    }
}

void GeneralSettingsWidget::on_databaseIntegrityCheckButton_clicked() {
    if (DatabaseService::checkDiskDatabaseIntegrity()) {
        Utils::Gui::information(this, tr("Database"),
                                tr("The integrity of the disk database is valid."),
                                QStringLiteral("database-integrity-check-valid"));
    } else {
        Utils::Gui::warning(this, tr("Database"),
                            tr("The integrity of the disk database is not valid!"),
                            QStringLiteral("database-integrity-check-not-valid"));
    }
}

/**
 * Allows the user to clear all settings and the database and exit the app
 */
void GeneralSettingsWidget::on_clearAppDataAndExitButton_clicked() {
    if (QMessageBox::question(this, tr("Clear app data and exit"),
                              tr("Do you really want to clear all settings, remove the "
                                 "database and exit QOwnNotes?\n\n"
                                 "Your notes will stay intact!"),
                              QMessageBox::Yes | QMessageBox::Cancel,
                              QMessageBox::Cancel) == QMessageBox::Yes) {
        SettingsService settings;
        settings.clear();
        DatabaseService::removeDiskDatabase();

        // Remove the log file
        DebugOptionSettingsWidget::removeLogFile();

        // Make sure no settings get written after quitting
        qApp->setProperty("clearAppDataAndExit", true);
        qApp->quit();
    }
}

/**
 * Sets a path to an external editor
 */
void GeneralSettingsWidget::on_setExternalEditorPathToolButton_clicked() {
    const QString filePath =
        selectExecutablePath(ui->externalEditorPathLineEdit->text(),
                             QStringLiteral("ExternalEditor"), tr("Select editor application"));

    if (!filePath.isEmpty()) {
        ui->externalEditorPathLineEdit->setText(filePath);
    }
}

/**
 * Sets a path to an external diff tool
 */
void GeneralSettingsWidget::on_setExternalDiffToolPathToolButton_clicked() {
    const QString filePath = selectExecutablePath(ui->externalDiffToolPathLineEdit->text(),
                                                  QStringLiteral("ExternalDiffTool"),
                                                  tr("Select diff tool application"));

    if (!filePath.isEmpty()) {
        ui->externalDiffToolPathLineEdit->setText(filePath);
    }
}

QString GeneralSettingsWidget::selectExecutablePath(const QString &path, const QString &dialogName,
                                                    const QString &windowTitle) {
    QString dirPath = path;

    // Get the path of the directory if an application path was set
    if (!path.isEmpty()) {
        dirPath = QFileInfo(path).dir().path();
    }

    // In portable mode the data path will be opened if path was empty
    if (path.isEmpty() && Utils::Misc::isInPortableMode()) {
        dirPath = Utils::Misc::portableDataPath();
    }

    QStringList mimeTypeFilters;
    mimeTypeFilters << QStringLiteral("application/x-executable")
                    << QStringLiteral("application/octet-stream");

    FileDialog dialog(dialogName);

    if (!dirPath.isEmpty()) {
        dialog.setDirectory(dirPath);
    }

    if (!path.isEmpty()) {
        dialog.selectFile(path);
    }

    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.setWindowTitle(windowTitle);
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        const QStringList fileNames = dialog.selectedFiles();
        if (fileNames.empty()) {
            return QString();
        }

        return fileNames.at(0);
    }

    return QString();
}

/**
 * Adds a custom file extension
 */
void GeneralSettingsWidget::on_addCustomNoteFileExtensionButton_clicked() {
    bool ok;
    QString fileExtension;
    fileExtension = QInputDialog::getText(this, tr("File extension"),
                                          tr("Please enter a new note file extension:"),
                                          QLineEdit::Normal, fileExtension, &ok);

    if (!ok) {
        return;
    }

    // Make sure the file extension doesn't start with a point
    fileExtension = Utils::Misc::removeIfStartsWith(std::move(fileExtension), QStringLiteral("."));

    QListWidgetItem *item = addCustomNoteFileExtension(fileExtension);

    if (item != nullptr) {
        ui->defaultNoteFileExtensionListWidget->setCurrentItem(item);
    }
}

/**
 * Adds a custom note file extension to the list widget
 */
QListWidgetItem *GeneralSettingsWidget::addCustomNoteFileExtension(const QString &fileExtension) {
    const QString trimmed = fileExtension.trimmed();

    if (ui->defaultNoteFileExtensionListWidget->findItems(trimmed, Qt::MatchExactly).count() > 0) {
        return nullptr;
    }

    auto *item = new QListWidgetItem(trimmed);
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    if (trimmed == QLatin1String("md")) {
        item->setToolTip(tr("Markdown file"));
    } else if (trimmed == QLatin1String("txt")) {
        item->setToolTip(tr("Plain text file"));
    }

    ui->defaultNoteFileExtensionListWidget->addItem(item);

    return item;
}

/**
 * Removes a custom file extension
 */
void GeneralSettingsWidget::on_removeCustomNoteFileExtensionButton_clicked() {
    if (ui->defaultNoteFileExtensionListWidget->count() <= 1) {
        return;
    }

    auto *item = ui->defaultNoteFileExtensionListWidget->currentItem();

    if (Utils::Gui::question(this, tr("Remove note file extension"),
                             tr("Do you really want to remove the note file extension "
                                "<strong>%1</strong>? You will not see files with this "
                                "extension in the note list any more!")
                                 .arg(item->text()),
                             QStringLiteral("remove-note-file-extension")) != QMessageBox::Yes) {
        return;
    }

    delete item;

    ui->removeCustomNoteFileExtensionButton->setEnabled(
        ui->defaultNoteFileExtensionListWidget->count() > 1);
}

/**
 * Updates a custom file extension
 */
void GeneralSettingsWidget::on_defaultNoteFileExtensionListWidget_itemChanged(
    QListWidgetItem *item) {
    // Make sure the file extension doesn't start with a point
    const QString fileExtension =
        Utils::Misc::removeIfStartsWith(item->text(), QStringLiteral(".")).trimmed();

    if (fileExtension != item->text()) {
        item->setText(fileExtension);
    }
}

void GeneralSettingsWidget::on_defaultNoteFileExtensionListWidget_itemSelectionChanged() {
    ui->removeCustomNoteFileExtensionButton->setEnabled(
        ui->defaultNoteFileExtensionListWidget->count() > 1);
}

/**
 * Resets the overrides for all message boxes
 */
void GeneralSettingsWidget::on_resetMessageBoxesButton_clicked() {
    if (QMessageBox::question(this, tr("Reset message boxes"),
                              tr("Do you really want to reset the overrides of all message "
                                 "boxes?")) == QMessageBox::Yes) {
        SettingsService settings;

        // Remove all settings in the group
        settings.beginGroup(QStringLiteral("MessageBoxOverride"));
        settings.remove(QLatin1String(""));
        settings.endGroup();
    }
}

/**
 * Exports settings to an INI file
 */
void GeneralSettingsWidget::on_exportSettingsButton_clicked() {
    FileDialog dialog(QStringLiteral("SettingsExport"));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("INI files") + " (*.ini)");
    dialog.setWindowTitle(tr("Export settings"));
    dialog.selectFile(QStringLiteral("QOwnNotes-settings.ini"));
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QString fileName = dialog.selectedFile();

        if (!fileName.isEmpty()) {
            if (QFileInfo(fileName).suffix().isEmpty()) {
                fileName.append(QStringLiteral(".ini"));
            }

            QSettings exportSettings(fileName, QSettings::IniFormat);

            // Clear the settings in case the settings file already existed
            exportSettings.clear();

            exportSettings.setValue(QStringLiteral("SettingsExport/platform"),
                                    QStringLiteral(PLATFORM));

            SettingsService settings;

            const QStringList keys = settings.allKeys();
            Q_FOREACH (const QString &key, keys) {
                exportSettings.setValue(key, settings.value(key));
            }
        }
    }
}

/**
 * Imports settings from an INI file
 */
void GeneralSettingsWidget::on_importSettingsButton_clicked() {
    const QString title = tr("Import settings");
    const QString text = tr("Do you really want to import settings? Your current "
                            "settings will get removed and not every setting may "
                            "get restored, like the note folder settings and which "
                            "scripts you were using. "
                            "You also will need to adjust some settings, especially "
                            "across platforms, but your notes will stay intact!") +
                         QLatin1String("\n\n") +
                         tr("The application will be restarted after the import.") +
                         Utils::Misc::appendSingleAppInstanceTextIfNeeded();

    if (QMessageBox::question(this, title, text, QMessageBox::Yes | QMessageBox::No,
                              QMessageBox::No) == QMessageBox::No) {
        return;
    }

    FileDialog dialog(QStringLiteral("SettingsExport"));
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilter(tr("INI files") + " (*.ini)");
    dialog.setWindowTitle(tr("Import settings"));
    int ret = dialog.exec();

    if (ret != QDialog::Accepted) {
        return;
    }

    const QString fileName = dialog.selectedFile();
    SettingsService settings;
    QSettings importSettings(fileName, QSettings::IniFormat);
    settings.clear();
    DatabaseService::removeDiskDatabase();

    const QStringList keys = importSettings.allKeys();
    Q_FOREACH (const QString &key, keys) {
        const QVariant value = importSettings.value(key);
        settings.setValue(key, value);
    }

    // Make sure no settings get written after quitting
    qApp->setProperty("clearAppDataAndExit", true);

    Utils::Misc::restartApplication();
}

void GeneralSettingsWidget::on_imageScaleDownCheckBox_toggled(bool checked) {
    ui->imageScalingFrame->setVisible(checked);
}

void GeneralSettingsWidget::on_enableReadOnlyModeCheckBox_toggled(bool checked) {
    ui->readOnlyModeSettingsFrame->setEnabled(checked);
}

/**
 * Checks if the _noteNotificationNoneCheckBox should be checked when a
 * notification checkbox is unchecked
 */
void GeneralSettingsWidget::noteNotificationButtonGroupPressed(QAbstractButton *button) {
    if (button->isChecked()) {
        QTimer::singleShot(100, this, SLOT(noteNotificationNoneCheckBoxCheck()));
    }
}

/**
 * Checks the _noteNotificationNoneCheckBox to effectively uncheck all notification checkboxes
 */
void GeneralSettingsWidget::noteNotificationNoneCheckBoxCheck() {
    _noteNotificationNoneCheckBox->setChecked(true);
}
