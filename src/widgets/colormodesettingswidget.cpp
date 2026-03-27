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

#include "colormodesettingswidget.h"

#include <services/settingsservice.h>
#include <utils/gui.h>
#include <utils/schema.h>

#include <QMessageBox>
#include <QSettings>

#include "ui_colormodesettingswidget.h"

ColorModeSettingsWidget::ColorModeSettingsWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::ColorModeSettingsWidget) {
    ui->setupUi(this);

    // Ensure built-in color modes exist
    ColorMode::ensureBuiltInModesExist();

    _initialColorModeId = ColorMode::currentColorModeId();
}

ColorModeSettingsWidget::~ColorModeSettingsWidget() { delete ui; }

QString ColorModeSettingsWidget::initialColorModeId() const { return _initialColorModeId; }

/**
 * Populates the editor color schema combo box
 */
void ColorModeSettingsWidget::initEditorSchemaComboBox() {
    ui->colorModeEditorColorSchemaComboBox->clear();

    // Load default schemas
    QStringList defaultSchemaKeys = Utils::Schema::schemaSettings->defaultSchemaKeys();
    QMap<QString, QString> defaultSchemaNameKeys;

    const QSettings &defaultSchemaSettings = Utils::Schema::schemaSettings->defaultSchemaSettings();

    for (const QString &schemaKey : defaultSchemaKeys) {
        QString name = defaultSchemaSettings.value(schemaKey + "/Name").toString();

        // Enforce the light theme to be on top
        if (schemaKey == QStringLiteral("EditorColorSchema-6033d61b-cb96-46d5-a3a8-20d5172017eb")) {
            name = QStringLiteral("  ") + name;
        }

        // Enforce the dark theme to be 2nd
        if (schemaKey == QStringLiteral("EditorColorSchema-cdbf28fc-1ddc-4d13-bb21-6a4043316a2f")) {
            name = QStringLiteral(" ") + name;
        }

        defaultSchemaNameKeys.insert(name, schemaKey);
    }

    for (const QString &schemaKey : defaultSchemaNameKeys.values()) {
        const QString name = defaultSchemaSettings.value(schemaKey + "/Name").toString();
        ui->colorModeEditorColorSchemaComboBox->addItem(name.trimmed(), schemaKey);
    }

    // Load custom schemas
    SettingsService settings;
    QStringList schemes = settings.value(QStringLiteral("Editor/ColorSchemes")).toStringList();
    for (const QString &schemaKey : schemes) {
        settings.beginGroup(schemaKey);
        QString name = settings.value(QStringLiteral("Name")).toString();
        ui->colorModeEditorColorSchemaComboBox->addItem(name, schemaKey);
        settings.endGroup();
    }
}

/**
 * Initializes the color modes page
 */
void ColorModeSettingsWidget::initialize() {
    initEditorSchemaComboBox();

    ui->colorModeEditFrame->setEnabled(false);

    QList<ColorMode> colorModes = ColorMode::fetchAll();

    // Populate the color mode list
    for (const ColorMode &mode : colorModes) {
        auto *item = new QListWidgetItem(mode.getName());
        item->setData(Qt::UserRole, mode.getId());
        ui->colorModeListWidget->addItem(item);

        // Select the current color mode
        if (mode.isCurrent()) {
            ui->colorModeListWidget->setCurrentItem(item);
        }
    }

    // If nothing was selected, select the first item
    if (ui->colorModeListWidget->currentRow() == -1 && ui->colorModeListWidget->count() > 0) {
        ui->colorModeListWidget->setCurrentRow(0);
    }
}

/**
 * Applies the current color mode settings to the global application settings
 */
void ColorModeSettingsWidget::applyColorModeSettings() {
    const ColorMode mode = ColorMode::currentColorMode();

    SettingsService settings;
    settings.setValue(QStringLiteral("darkMode"), mode.isDarkMode());
    settings.setValue(QStringLiteral("darkModeColors"), mode.isDarkModeColors());
    settings.setValue(QStringLiteral("darkModeTrayIcon"), mode.isDarkModeTrayIcon());
    settings.setValue(QStringLiteral("darkModeIconTheme"), mode.isDarkModeIconTheme());
    settings.setValue(QStringLiteral("internalIconTheme"), mode.isInternalIconTheme());
    settings.setValue(QStringLiteral("systemIconTheme"), mode.isSystemIconTheme());

    if (!mode.getEditorColorSchemaKey().isEmpty()) {
        settings.setValue(QStringLiteral("Editor/CurrentSchemaKey"),
                          mode.getEditorColorSchemaKey());
    }

    Utils::Gui::fixDarkModeIcons(this);
    Utils::Gui::applyDarkModeSettings();
}

/**
 * Handles selection change in the color mode list
 */
void ColorModeSettingsWidget::on_colorModeListWidget_currentItemChanged(QListWidgetItem *current,
                                                                        QListWidgetItem *previous) {
    Q_UNUSED(previous)

    if (current == nullptr) {
        ui->colorModeEditFrame->setEnabled(false);
        return;
    }

    ui->colorModeEditFrame->setEnabled(true);

    const QString colorModeId = current->data(Qt::UserRole).toString();
    _selectedColorMode = ColorMode::fetch(colorModeId);

    // Block signals while populating the UI
    const QSignalBlocker nameBlocker(ui->colorModeNameLineEdit);
    Q_UNUSED(nameBlocker)
    const QSignalBlocker activeBlocker(ui->colorModeActiveCheckBox);
    Q_UNUSED(activeBlocker)
    const QSignalBlocker darkModeBlocker(ui->colorModeDarkModeCheckBox);
    Q_UNUSED(darkModeBlocker)
    const QSignalBlocker darkModeColorsBlocker(ui->colorModeDarkModeColorsCheckBox);
    Q_UNUSED(darkModeColorsBlocker)
    const QSignalBlocker darkModeTrayIconBlocker(ui->colorModeDarkModeTrayIconCheckBox);
    Q_UNUSED(darkModeTrayIconBlocker)
    const QSignalBlocker darkModeIconThemeBlocker(ui->colorModeDarkModeIconThemeCheckBox);
    Q_UNUSED(darkModeIconThemeBlocker)
    const QSignalBlocker internalIconThemeBlocker(ui->colorModeInternalIconThemeCheckBox);
    Q_UNUSED(internalIconThemeBlocker)
    const QSignalBlocker systemIconThemeBlocker(ui->colorModeSystemIconThemeCheckBox);
    Q_UNUSED(systemIconThemeBlocker)
    const QSignalBlocker schemaBlocker(ui->colorModeEditorColorSchemaComboBox);
    Q_UNUSED(schemaBlocker)

    ui->colorModeNameLineEdit->setText(_selectedColorMode.getName());
    ui->colorModeActiveCheckBox->setChecked(_selectedColorMode.isCurrent());

    ui->colorModeDarkModeCheckBox->setChecked(_selectedColorMode.isDarkMode());
    ui->colorModeDarkModeColorsCheckBox->setChecked(_selectedColorMode.isDarkModeColors());
    ui->colorModeDarkModeTrayIconCheckBox->setChecked(_selectedColorMode.isDarkModeTrayIcon());
    ui->colorModeDarkModeIconThemeCheckBox->setChecked(_selectedColorMode.isDarkModeIconTheme());

    ui->colorModeInternalIconThemeCheckBox->setChecked(_selectedColorMode.isInternalIconTheme());
    ui->colorModeSystemIconThemeCheckBox->setChecked(_selectedColorMode.isSystemIconTheme());

    // Set the editor color schema combo box to the correct value
    const QString schemaKey = _selectedColorMode.getEditorColorSchemaKey();
    for (int i = 0; i < ui->colorModeEditorColorSchemaComboBox->count(); i++) {
        if (ui->colorModeEditorColorSchemaComboBox->itemData(i).toString() == schemaKey) {
            ui->colorModeEditorColorSchemaComboBox->setCurrentIndex(i);
            break;
        }
    }

    // Built-in modes cannot have their names edited
    ui->colorModeNameLineEdit->setReadOnly(_selectedColorMode.isBuiltIn());

    // Disable the remove button for built-in modes
    ui->colorModeRemoveButton->setEnabled(!_selectedColorMode.isBuiltIn());

#ifndef Q_OS_LINUX
    ui->colorModeSystemIconThemeCheckBox->setHidden(true);
    ui->colorModeSystemIconThemeCheckBox->setChecked(false);
#endif
}

/**
 * Adds a new custom color mode
 */
void ColorModeSettingsWidget::on_colorModeAddButton_clicked() {
    ColorMode newMode = ColorMode::createCustom(tr("New color mode"));
    newMode.store();

    auto *item = new QListWidgetItem(newMode.getName());
    item->setData(Qt::UserRole, newMode.getId());
    ui->colorModeListWidget->addItem(item);

    // Select the new item
    ui->colorModeListWidget->setCurrentRow(ui->colorModeListWidget->count() - 1);

    // Focus and select the name
    ui->colorModeNameLineEdit->setFocus();
    ui->colorModeNameLineEdit->selectAll();
}

/**
 * Removes the currently selected custom color mode
 */
void ColorModeSettingsWidget::on_colorModeRemoveButton_clicked() {
    if (_selectedColorMode.isBuiltIn()) {
        return;
    }

    if (Utils::Gui::question(
            this, tr("Remove color mode"),
            tr("Remove the color mode <strong>%1</strong>?").arg(_selectedColorMode.getName()),
            QStringLiteral("remove-color-mode")) == QMessageBox::Yes) {
        const bool wasCurrent = _selectedColorMode.isCurrent();

        _selectedColorMode.remove();

        // Remove the list item
        delete ui->colorModeListWidget->takeItem(ui->colorModeListWidget->currentRow());

        // If the removed mode was current, switch to the light mode
        if (wasCurrent) {
            ColorMode lightMode = ColorMode::fetch(ColorMode::LightModeId);
            lightMode.setAsCurrent();
            applyColorModeSettings();
        }
    }
}

/**
 * Handles name editing finished for a color mode
 */
void ColorModeSettingsWidget::on_colorModeNameLineEdit_editingFinished() {
    if (_selectedColorMode.isBuiltIn()) {
        return;
    }

    QString text = ui->colorModeNameLineEdit->text().remove(QStringLiteral("\n")).trimmed();
    text.truncate(50);

    if (text.isEmpty()) {
        text = tr("Color mode");
    }

    _selectedColorMode.setName(text);
    _selectedColorMode.store();

    ui->colorModeListWidget->currentItem()->setText(text);
}

/**
 * Handles setting a color mode as the active one
 */
void ColorModeSettingsWidget::on_colorModeActiveCheckBox_stateChanged(int arg1) {
    Q_UNUSED(arg1)

    if (!ui->colorModeActiveCheckBox->isChecked()) {
        const QSignalBlocker blocker(ui->colorModeActiveCheckBox);
        Q_UNUSED(blocker)
        ui->colorModeActiveCheckBox->setChecked(true);
    } else {
        _selectedColorMode.setAsCurrent();

        // Apply the color mode settings to the global settings
        applyColorModeSettings();
    }
}

/**
 * Handles dark mode checkbox toggle
 */
void ColorModeSettingsWidget::on_colorModeDarkModeCheckBox_toggled(bool checked) {
    _selectedColorMode.setDarkMode(checked);

    if (checked) {
        // When enabling dark mode, also enable related settings
        const QSignalBlocker colorsBlocker(ui->colorModeDarkModeColorsCheckBox);
        Q_UNUSED(colorsBlocker)
        ui->colorModeDarkModeColorsCheckBox->setChecked(true);
        _selectedColorMode.setDarkModeColors(true);

        const QSignalBlocker iconBlocker(ui->colorModeDarkModeIconThemeCheckBox);
        Q_UNUSED(iconBlocker)
        ui->colorModeDarkModeIconThemeCheckBox->setChecked(true);
        _selectedColorMode.setDarkModeIconTheme(true);
    }

    _selectedColorMode.store();

    if (_selectedColorMode.isCurrent()) {
        applyColorModeSettings();
    }
}

/**
 * Handles dark mode colors checkbox toggle
 */
void ColorModeSettingsWidget::on_colorModeDarkModeColorsCheckBox_toggled(bool checked) {
    _selectedColorMode.setDarkModeColors(checked);
    _selectedColorMode.store();

    if (_selectedColorMode.isCurrent()) {
        applyColorModeSettings();
    }
}

/**
 * Handles dark mode tray icon checkbox toggle
 */
void ColorModeSettingsWidget::on_colorModeDarkModeTrayIconCheckBox_toggled(bool checked) {
    _selectedColorMode.setDarkModeTrayIcon(checked);
    _selectedColorMode.store();

    if (_selectedColorMode.isCurrent()) {
        applyColorModeSettings();
    }
}

/**
 * Handles dark mode icon theme checkbox toggle
 */
void ColorModeSettingsWidget::on_colorModeDarkModeIconThemeCheckBox_toggled(bool checked) {
    _selectedColorMode.setDarkModeIconTheme(checked);
    _selectedColorMode.store();

    if (_selectedColorMode.isCurrent()) {
        applyColorModeSettings();
    }
}

/**
 * Handles internal icon theme checkbox toggle
 */
void ColorModeSettingsWidget::on_colorModeInternalIconThemeCheckBox_toggled(bool checked) {
    _selectedColorMode.setInternalIconTheme(checked);

    if (checked) {
        const QSignalBlocker blocker(ui->colorModeSystemIconThemeCheckBox);
        Q_UNUSED(blocker)
        ui->colorModeSystemIconThemeCheckBox->setChecked(false);
        _selectedColorMode.setSystemIconTheme(false);
        ui->colorModeSystemIconThemeCheckBox->setEnabled(false);
    } else {
        ui->colorModeSystemIconThemeCheckBox->setEnabled(true);
    }

    _selectedColorMode.store();

    if (_selectedColorMode.isCurrent()) {
        applyColorModeSettings();
    }
}

/**
 * Handles system icon theme checkbox toggle
 */
void ColorModeSettingsWidget::on_colorModeSystemIconThemeCheckBox_toggled(bool checked) {
    _selectedColorMode.setSystemIconTheme(checked);

    if (checked) {
        const QSignalBlocker blocker(ui->colorModeInternalIconThemeCheckBox);
        Q_UNUSED(blocker)
        ui->colorModeInternalIconThemeCheckBox->setChecked(false);
        _selectedColorMode.setInternalIconTheme(false);
        ui->colorModeInternalIconThemeCheckBox->setEnabled(false);
        ui->colorModeDarkModeIconThemeCheckBox->setEnabled(false);
    } else {
        ui->colorModeInternalIconThemeCheckBox->setEnabled(true);
        ui->colorModeDarkModeIconThemeCheckBox->setEnabled(true);
    }

    _selectedColorMode.store();

    if (_selectedColorMode.isCurrent()) {
        applyColorModeSettings();
    }
}

/**
 * Handles editor color schema selection change
 */
void ColorModeSettingsWidget::on_colorModeEditorColorSchemaComboBox_currentIndexChanged(int index) {
    if (index < 0) {
        return;
    }

    const QString schemaKey = ui->colorModeEditorColorSchemaComboBox->itemData(index).toString();
    _selectedColorMode.setEditorColorSchemaKey(schemaKey);
    _selectedColorMode.store();

    if (_selectedColorMode.isCurrent()) {
        applyColorModeSettings();
    }
}
