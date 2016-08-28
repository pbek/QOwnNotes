/*
 * Copyright (C) 2016 Patrizio Bekerle -- http://www.bekerle.com
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
#include <QTreeWidgetItem>
#include <QSettings>

namespace Ui {
class FontColorWidget;
}

class FontColorWidget : public QFrame
{
    Q_OBJECT

public:
    explicit FontColorWidget(QWidget *parent = 0);
    ~FontColorWidget();

private slots:
    void on_foregroundColorButton_clicked();

    void on_colorSchemeComboBox_currentIndexChanged(int index);

    void on_textTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_copySchemeButton_clicked();

    void on_backgroundColorButton_clicked();

    void on_foregroundColorCheckBox_toggled(bool checked);

    void on_backgroundColorCheckBox_toggled(bool checked);

    void on_deleteSchemeButton_clicked();

private:
    Ui::FontColorWidget *ui;
    QStringList _defaultSchemaKeys;
    QString _currentSchemaKey;
    bool _currentSchemaIsDefault;

    void addTextTreeWidgetItem(QString text, int id);

    void initTextTreeWidgetItems();

    void initSchemaSelector();

    void updateSchemeEditFrame();

    void setSchemaValue(QString key, QVariant value, QString schemaKey = "");

    QString textSettingsKey(QString key, QTreeWidgetItem *item = Q_NULLPTR);

    void updateTextItem(QTreeWidgetItem *item = Q_NULLPTR);

    void updateForegroundColorCheckBox(bool checked, bool store = false);

    void updateBackgroundColorCheckBox(bool checked, bool store = false);

    int textSettingsIndex(QTreeWidgetItem *item = Q_NULLPTR);

    void updateAllTextItems();

    void updateTextItems(int index);
};
