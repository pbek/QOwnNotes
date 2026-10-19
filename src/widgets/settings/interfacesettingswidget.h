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

#include <QWidget>

namespace Ui {
class InterfaceSettingsWidget;
}

class InterfaceSettingsWidget : public QWidget {
    Q_OBJECT

   public:
    explicit InterfaceSettingsWidget(QWidget *parent = nullptr);
    ~InterfaceSettingsWidget();

    void initialize();
    void readSettings();
    void storeSettings();
    void updateSearchIcons();

   signals:
    void needRestart();
    void systemTrayToggled(bool checked);

   private slots:
    void on_interfaceStyleComboBox_currentTextChanged(const QString &arg1);
    void on_showSystemTrayCheckBox_toggled(bool checked);
    void on_interfaceFontSizeSpinBox_valueChanged(int arg1);
    void on_overrideInterfaceFontSizeGroupBox_toggled(bool arg1);
    void on_languageSearchLineEdit_textChanged(const QString &arg1);
    void on_showStatusBarNotePathCheckBox_toggled(bool checked);
    void on_overrideInterfaceScalingFactorGroupBox_toggled(bool arg1);
    void on_interfaceScalingFactorSpinBox_valueChanged(int arg1);
    void on_itemHeightResetButton_clicked();
    void on_toolbarIconSizeResetButton_clicked();

   protected:
    void showEvent(QShowEvent *event) override;

   private:
    Ui::InterfaceSettingsWidget *ui;

    void loadInterfaceStyleComboBox();
    void loadKdeColorSchemeComboBox();
    void updateKdeColorSchemeState();
    void storeKdeColorScheme();
};
