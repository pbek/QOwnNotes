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

#include <QFrame>

class QTreeWidgetItem;

namespace Ui {
class FontColorWidget;
}

class FontColorWidget : public QFrame {
    Q_OBJECT

   public:
    explicit FontColorWidget(QWidget *parent = nullptr);
    ~FontColorWidget();

    void updateAllTextItems();

   public slots:
    bool selectFirstLightSchema();

    bool selectFirstDarkSchema();

   private slots:
    void on_foregroundColorButton_clicked();

    void on_colorSchemeComboBox_currentIndexChanged(int index);

    void on_textTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_copySchemeButton_clicked();

    void on_backgroundColorButton_clicked();

    void on_foregroundColorCheckBox_toggled(bool checked);

    void on_backgroundColorCheckBox_toggled(bool checked);

    void on_deleteSchemeButton_clicked();

    void on_boldCheckBox_toggled(bool checked);

    void on_italicCheckBox_toggled(bool checked);

    void on_underlineCheckBox_toggled(bool checked);

    void on_exportSchemeButton_clicked();

    void on_importSchemeButton_clicked();

    void on_fontSizeAdaptionSpinBox_valueChanged(int value);

    void on_shareSchemaPushButton_clicked();

    void needRestart();

    void on_fontCheckBox_toggled(bool checked);

    void on_fontComboBox_currentFontChanged(const QFont &f);

   private:
    Ui::FontColorWidget *ui;
    QStringList _defaultSchemaKeys;
    QString _currentSchemaKey;
    bool _currentSchemaIsDefault;

    void addTextTreeWidgetItem(const QString &text, int index);

    void initTextTreeWidgetItems();

    void initSchemaSelector();

    void updateSchemeEditFrame();

    void setSchemaValue(const QString &key, const QVariant &value, QString schemaKey = QString());

    QString textSettingsKey(const QString &key, QTreeWidgetItem *item = nullptr);

    void updateTextItem(QTreeWidgetItem *item = nullptr);

    void updateForegroundColorCheckBox(bool checked, bool store = false);

    void updateBackgroundColorCheckBox(bool checked, bool store = false);

    void updateFontCheckBox(bool checked, bool store = false);

    int textSettingsIndex(QTreeWidgetItem *item = nullptr);

    void updateTextItems(int index);

    void storeCheckBoxState(const QString &name, bool checked);

    void selectLastSchema();

    void initFontSelectors();
};
