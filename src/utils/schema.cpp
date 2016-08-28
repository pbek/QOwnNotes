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

#include <QInputDialog>
#include <QTreeWidgetItem>
#include <QColorDialog>
#include <QDebug>
#include <QSettings>
#include <QStringList>
#include <QTextEdit>
#include "schema.h"


/**
 * Returns the default schema keys
 *
 * @return
 */
QStringList Utils::Schema::defaultSchemaKeys() {
    QSettings schemaSettings(":/configurations/schemes.conf",
                             QSettings::IniFormat);
    return schemaSettings.value("Editor/DefaultColorSchemes").toStringList();
}

/**
 * Returns the current schema key
 *
 * @return
 */
QString Utils::Schema::currentSchemaKey() {
    QSettings settings;
    QStringList schemaKeys = defaultSchemaKeys();

    return settings.value(
            "Editor/CurrentSchemaKey",
            schemaKeys.length() > 0 ? schemaKeys[0] : "")
            .toString();
}

/**
 * Checks if the ucrrent schema is a default schema
 *
 * @return
 */
bool Utils::Schema::currentSchemaIsDefault() {
    return defaultSchemaKeys().contains(currentSchemaKey());
}

/**
 * Returns the current schema settings
 *
 * @return
 */
QSettings* Utils::Schema::getSchemaSettings() {
    return currentSchemaIsDefault() ?
           new QSettings(":/configurations/schemes.conf",
                         QSettings::IniFormat) :new QSettings();
}

/**
 * Returns a value of the current schema
 *
 * @param key
 * @param defaultValue
 * @return
 */
QVariant Utils::Schema::getSchemaValue(QString key, QVariant defaultValue) {
    QSettings *settings = Utils::Schema::getSchemaSettings();
    settings->beginGroup(currentSchemaKey());
    return settings->value(key, defaultValue);
}

/**
 * Returns the text settings key for an index
 *
 * @param key
 * @param index
 * @return
 */
QString Utils::Schema::textSettingsKey(QString key, int index) {
    return key + "_" + QString::number(index);
}

/**
 * Returns a schema value for the default text
 *
 * @param key
 * @param defaultValue
 * @return
 */
QVariant Utils::Schema::getDefaultTextSchemaValue(
        QString key, QVariant defaultValue) {
    return Utils::Schema::getSchemaValue(
            Utils::Schema::textSettingsKey(key, TextIndex), defaultValue);
}

/**
 * Trys to fetch the correct foreground color for an index
 *
 * @param index
 * @return
 */
QColor Utils::Schema::getForegroundColor(int index) {
    // get the foreground color
    bool enabled = getSchemaValue(
            textSettingsKey("ForegroundColorEnabled", index)).toBool();
    QColor color;

    // if the foreground color is enabled try to fetch it
    if (enabled) {
        color = getSchemaValue(
                textSettingsKey("ForegroundColor", index)).value<QColor>();
    }

    // if the color was not valid, try to fetch the color for "Text"
    if (!color.isValid() && (index >= 0)) {
        color = getForegroundColor(TextIndex);
    }

    // if the color still was not valid, try to fetch the color from a QTextEdit
    if (!color.isValid()) {
        QTextEdit textEdit;
        color = textEdit.textColor();
    }

    // if the color still was not valid, use black
    if (!color.isValid()) {
        color = QColor(Qt::black);
    }

    return color;
}

/**
 * Trys to fetch the correct background color for an index
 *
 * @param index
 * @return
 */
QColor Utils::Schema::getBackgroundColor(int index) {
    // get the foreground color
    bool enabled = getSchemaValue(
            textSettingsKey("BackgroundColorEnabled", index)).toBool();
    QColor color;

    // if the foreground color is enabled try to fetch it
    if (enabled) {
        color = getSchemaValue(
                textSettingsKey("BackgroundColor", index)).value<QColor>();
    }

    // if the color was not valid, try to fetch the color for "Text"
    if (!color.isValid() && (index >= 0)) {
        color = getBackgroundColor(TextIndex);
    }

    // if the color still was not valid, use black
    if (!color.isValid()) {
        color = QColor(Qt::white);
    }

    return color;
}

/**
 * Sets the foreground and background color for an format
 *
 * @param format
 * @param index
 */
void Utils::Schema::setFormatColors(int index, QTextCharFormat &format) {
    format.setForeground(QBrush(Utils::Schema::getForegroundColor(index)));
    format.setBackground(QBrush(Utils::Schema::getBackgroundColor(index)));
}
