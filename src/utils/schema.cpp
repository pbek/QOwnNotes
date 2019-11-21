/*
 * Copyright (c) 2014-2019 Patrizio Bekerle -- http://www.bekerle.com
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
#include "math.h"

Utils::Schema::Settings* Utils::Schema::schemaSettings = nullptr;

/** The default schema table is initialized here, precisely once, as loading
 * a settings object from a resource is expensive, unlike most other uses of
 * QSettings. Only call this once, when setting Utils::Schema::schemaSettings */
Utils::Schema::Settings::Settings() :
    _defaultSchemaSettings(":/configurations/schemes.conf", QSettings::IniFormat) {

    _defaultFontSet = false;

    _defaultSchemaKeysList = _defaultSchemaSettings.value("Editor/DefaultColorSchemes").toStringList();

    // Determine the keys for a given schema
    _defaultSchemaSubkeylists.reserve(_defaultSchemaKeysList.size());
    Q_FOREACH(const QString& schema, _defaultSchemaKeysList) {
        QStringList groupKeys;
        Q_FOREACH(const QString& key, _defaultSchemaSettings.allKeys()) {
            if (key.startsWith(schema)) {
                groupKeys.append(QString(key).remove(0, schema.size()+1));
            }
        }
        _defaultSchemaSubkeys.insert(schema, _defaultSchemaSubkeylists.size());
        _defaultSchemaSubkeylists.append(groupKeys);
    }
}



/**
 * Returns the default schema keys
 *
 * @return
 */
const QStringList& Utils::Schema::Settings::defaultSchemaKeys() const {
    return _defaultSchemaKeysList;
}

/**
 * Returns the default schema settings object
 *
 * @return
 */
const QSettings& Utils::Schema::Settings::defaultSchemaSettings() const {
    return _defaultSchemaSettings;
}

/**
 * Returns the current schema key
 *
 * @return
 */
QString Utils::Schema::Settings::currentSchemaKey() const {
    QSettings settings;
    return settings.value(
            "Editor/CurrentSchemaKey",
            _defaultSchemaKeysList.length() > 0 ? _defaultSchemaKeysList[0] : "DefaultSchema")
            .toString();
}

/**
 * Checks if the current schema is a default schema
 *
 * @return
 */
bool Utils::Schema::Settings::currentSchemaIsDefault() const {
    return _defaultSchemaSubkeys.contains(currentSchemaKey());
}

/**
 * Returns the list of keys for a given schema.
 *
 * @param schema
 * @return
 */
QStringList Utils::Schema::Settings::getSchemaKeys(const QString& schema) const {
    if (_defaultSchemaSubkeys.contains(schema)) {
        return _defaultSchemaSubkeylists[_defaultSchemaSubkeys[schema]];
    } else {
        QStringList groupKeys;
        QSettings s;
        s.beginGroup(schema);
        return s.allKeys();
    }
}

/**
 * Returns a value of the current schema
 *
 * @param key
 * @param defaultValue
 * @return
 */
QVariant Utils::Schema::Settings::getSchemaValue(const QString& key, const QVariant& defaultValue) const {
    const QString& schema = currentSchemaKey();
    if (_defaultSchemaSubkeys.contains(schema)) {
        return _defaultSchemaSettings.value(schema + '/' + key, defaultValue);
    } else {
        return QSettings().value(schema + '/' + key, defaultValue);
    }
}

/**
 * Returns the text settings key for an index
 *
 * @param key
 * @param index
 * @return
 */
QString Utils::Schema::textSettingsKey(const QString& key, int index) {
    return key + "_" + QString::number(index);
}

/**
 * Tries to fetch the correct foreground color for an index
 *
 * @param index
 * @return
 */
QColor Utils::Schema::Settings::getForegroundColor(int index) const {
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
 * Tries to fetch the correct background color for an index
 *
 * @param index
 * @return
 */
QColor Utils::Schema::Settings::getBackgroundColor(int index) const {
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
void Utils::Schema::Settings::setFormatStyle(MarkdownHighlighter::HighlighterState index,
                                   QTextCharFormat &format) const {
    // get the correct font
    QFont font = getEditorFont(index);

    // set the font
    format.setFont(font);

    // adapt the font size
    adaptFontSize(index, font);

    // override the font size
    format.setFontPointSize(font.pointSize());

    // set the foreground color
    format.setForeground(QBrush(getForegroundColor(index)));

    bool backgroundColorEnabled = getSchemaValue(
            textSettingsKey("BackgroundColorEnabled", index)).toBool();

    // set the background (color) only if the background color is enabled, otherwise we get troubles
    // with the background overwriting the foreground of neighboring text (e.g. for italic text)
    format.setBackground(backgroundColorEnabled ? QBrush(getBackgroundColor(index)) : QBrush());

    // set the bold state
    format.setFontWeight(getSchemaValue(
            Utils::Schema::textSettingsKey("Bold", index)).toBool() ?
                         QFont::Bold : QFont::Normal);

    // set the italic state
    format.setFontItalic(getSchemaValue(
            Utils::Schema::textSettingsKey("Italic", index)).toBool());

    // set the underline state
    format.setFontUnderline(getSchemaValue(
            Utils::Schema::textSettingsKey("Underline", index)).toBool());
}

/**
 * Adapts the font size of a font for an index
 *
 * @param index
 * @param font
 */
void Utils::Schema::Settings::adaptFontSize(int index, QFont &font) const {
    int adaption = getSchemaValue(textSettingsKey("FontSizeAdaption", index),
                                  100).toInt();
    double fontSize = round(font.pointSize() * adaption / 100);

    if (fontSize > 0) {
        font.setPointSize(fontSize);
    }
}

/**
 * Returns the editor text font
 *
 * @return
 */
QFont Utils::Schema::Settings::getEditorTextFont() const {
    if (!_defaultFontSet) {
        _defaultTextEditFont = QTextEdit().font();
        _defaultFontSet = true;
    }
    QSettings settings;
    QString fontString = settings.value(
            "MainWindow/noteTextEdit.font").toString();

    QFont font(_defaultTextEditFont);
    if (fontString != "") {
        // set the note text edit font
        font.fromString(fontString);
    } else {
        // store the default settings
        fontString = _defaultTextEditFont.toString();
        settings.setValue("MainWindow/noteTextEdit.font", fontString);
    }

    return font;
}

/**
 * Returns the editor fixed font
 *
 * @return
 */
QFont Utils::Schema::Settings::getEditorFixedFont() const {
    if (!_defaultFontSet) {
        _defaultTextEditFont = QTextEdit().font();
        _defaultFontSet = true;
    }

    QSettings settings;
    QString fontString = settings.value(
            "MainWindow/noteTextEdit.code.font").toString();

    QFont font(_defaultTextEditFont);
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
QFont Utils::Schema::Settings::getEditorFont(int index) const {
    if (index == MarkdownHighlighter::CodeBlock ||
            index == MarkdownHighlighter::InlineCodeBlock ||
            index == MarkdownHighlighter::Table) {
        return getEditorFixedFont();
    } else {
        return getEditorTextFont();
    }
}
