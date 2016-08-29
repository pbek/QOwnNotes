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
#include <QFontDatabase>
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
            Utils::Schema::textSettingsKey(key, TextPresetIndex), defaultValue);
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
        color = getForegroundColor(TextPresetIndex);
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
        color = getBackgroundColor(TextPresetIndex);
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
void Utils::Schema::setFormatStyle(pmh_element_type index,
                                   QTextCharFormat &format) {
    // get the correct font
    QFont font = getEditorFont(index);

    // set the font
    format.setFont(font);

    int fontSize = font.pointSize();

    switch (index) {
        case pmh_H1:
            fontSize *= 1.2;
            break;
        case pmh_H2:
            fontSize *= 1.1;
            break;
        default:
            break;
    }

    // override the font size
    format.setFontPointSize(fontSize);

    // set the foreground color
    format.setForeground(QBrush(Utils::Schema::getForegroundColor(index)));

    // set the background color
    format.setBackground(QBrush(Utils::Schema::getBackgroundColor(index)));

    // set the bold state
    format.setFontWeight(Utils::Schema::getSchemaValue(
            Utils::Schema::textSettingsKey("Bold", index)).toBool() ?
                         QFont::Bold : QFont::Normal);

    // set the italic state
    format.setFontItalic(Utils::Schema::getSchemaValue(
            Utils::Schema::textSettingsKey("Italic", index)).toBool());

    // set the underline state
    format.setFontUnderline(Utils::Schema::getSchemaValue(
            Utils::Schema::textSettingsKey("Underline", index)).toBool());
}

/**
 * Returns the editor text font
 *
 * @return
 */
QFont Utils::Schema::getEditorTextFont() {
    QTextEdit textEdit;
    QFont font = textEdit.font();
    QSettings settings;
    QString fontString = settings.value(
            "MainWindow/noteTextEdit.font").toString();

    if (fontString != "") {
        // set the note text edit font
        font.fromString(fontString);
    } else {
        // store the default settings
        fontString = textEdit.font().toString();
        settings.setValue("MainWindow/noteTextEdit.font", fontString);
    }

    return font;
}

/**
 * Returns the editor fixed font
 *
 * @return
 */
QFont Utils::Schema::getEditorFixedFont() {
    QTextEdit textEdit;
    QFont font = textEdit.font();
    QSettings settings;
    QString fontString = settings.value(
            "MainWindow/noteTextEdit.code.font").toString();

    if (fontString != "") {
        // set the code font
        font.fromString(fontString);
    } else {
        font = QFontDatabase::systemFont(QFontDatabase::FixedFont);

        // store the default settings
        settings.setValue(
                "MainWindow/noteTextEdit.code.font", font.toString());
    }

    return font;
}

/**
 * Returns the correct editor font
 *
 * @return
 */
QFont Utils::Schema::getEditorFont(int index) {
    QList<int> fixedFontIndices;
    fixedFontIndices << pmh_CODE << pmh_VERBATIM;

    return fixedFontIndices.contains(index) ?
        getEditorFixedFont() : getEditorTextFont();
}
