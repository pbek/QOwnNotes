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

#include <entities/colormode.h>

#include <QWidget>

class QListWidgetItem;

namespace Ui {
class ColorModeSettingsWidget;
}

class ColorModeSettingsWidget : public QWidget {
    Q_OBJECT

   public:
    explicit ColorModeSettingsWidget(QWidget *parent = nullptr);
    ~ColorModeSettingsWidget();

    void initialize();

    QString initialColorModeId() const;

   private slots:
    void on_colorModeListWidget_currentItemChanged(QListWidgetItem *current,
                                                   QListWidgetItem *previous);

    void on_colorModeAddButton_clicked();

    void on_colorModeRemoveButton_clicked();

    void on_colorModeNameLineEdit_editingFinished();

    void on_colorModeActiveCheckBox_stateChanged(int arg1);

    void on_colorModeDarkModeCheckBox_toggled(bool checked);

    void on_colorModeDarkModeColorsCheckBox_toggled(bool checked);

    void on_colorModeDarkModeTrayIconCheckBox_toggled(bool checked);

    void on_colorModeDarkModeIconThemeCheckBox_toggled(bool checked);

    void on_colorModeInternalIconThemeCheckBox_toggled(bool checked);

    void on_colorModeSystemIconThemeCheckBox_toggled(bool checked);

    void on_colorModeEditorColorSchemaComboBox_currentIndexChanged(int index);

   private:
    Ui::ColorModeSettingsWidget *ui;
    ColorMode _selectedColorMode;
    QString _initialColorModeId;

    void initEditorSchemaComboBox();

    void applyColorModeSettings();
};
