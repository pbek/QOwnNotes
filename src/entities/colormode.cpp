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

#include "colormode.h"

#include <QDebug>
#include <QUuid>

#include "services/settingsservice.h"

const QString ColorMode::LightModeId = QStringLiteral("ColorMode-light");
const QString ColorMode::DarkModeId = QStringLiteral("ColorMode-dark");

// Default editor color schema keys
static const QString LightEditorSchemaKey =
    QStringLiteral("EditorColorSchema-6033d61b-cb96-46d5-a3a8-20d5172017eb");
static const QString DarkEditorSchemaKey =
    QStringLiteral("EditorColorSchema-cdbf28fc-1ddc-4d13-bb21-6a4043316a2f");

ColorMode::ColorMode() = default;

QString ColorMode::getId() const { return _id; }

QString ColorMode::getName() const { return _name; }

bool ColorMode::isDarkMode() const { return _darkMode; }

bool ColorMode::isDarkModeColors() const { return _darkModeColors; }

bool ColorMode::isDarkModeTrayIcon() const { return _darkModeTrayIcon; }

bool ColorMode::isDarkModeIconTheme() const { return _darkModeIconTheme; }

bool ColorMode::isInternalIconTheme() const { return _internalIconTheme; }

bool ColorMode::isSystemIconTheme() const { return _systemIconTheme; }

QString ColorMode::getEditorColorSchemaKey() const { return _editorColorSchemaKey; }

bool ColorMode::isBuiltIn() const { return _id == LightModeId || _id == DarkModeId; }

void ColorMode::setId(const QString &id) { _id = id; }

void ColorMode::setName(const QString &name) { _name = name; }

void ColorMode::setDarkMode(bool value) { _darkMode = value; }

void ColorMode::setDarkModeColors(bool value) { _darkModeColors = value; }

void ColorMode::setDarkModeTrayIcon(bool value) { _darkModeTrayIcon = value; }

void ColorMode::setDarkModeIconTheme(bool value) { _darkModeIconTheme = value; }

void ColorMode::setInternalIconTheme(bool value) { _internalIconTheme = value; }

void ColorMode::setSystemIconTheme(bool value) { _systemIconTheme = value; }

void ColorMode::setEditorColorSchemaKey(const QString &key) { _editorColorSchemaKey = key; }

QString ColorMode::settingsGroupKey(const QString &id) {
    return QStringLiteral("ColorModes/") + id;
}

/**
 * Stores the color mode to settings
 */
bool ColorMode::store() const {
    if (_id.isEmpty()) {
        qWarning() << "Cannot store color mode with empty id";
        return false;
    }

    SettingsService settings;
    const QString group = settingsGroupKey(_id);

    settings.setValue(group + QStringLiteral("/name"), _name);
    settings.setValue(group + QStringLiteral("/darkMode"), _darkMode);
    settings.setValue(group + QStringLiteral("/darkModeColors"), _darkModeColors);
    settings.setValue(group + QStringLiteral("/darkModeTrayIcon"), _darkModeTrayIcon);
    settings.setValue(group + QStringLiteral("/darkModeIconTheme"), _darkModeIconTheme);
    settings.setValue(group + QStringLiteral("/internalIconTheme"), _internalIconTheme);
    settings.setValue(group + QStringLiteral("/systemIconTheme"), _systemIconTheme);
    settings.setValue(group + QStringLiteral("/editorColorSchemaKey"), _editorColorSchemaKey);

    // Update the list of color mode IDs
    QStringList ids = settings.value(QStringLiteral("ColorModes/ids")).toStringList();
    if (!ids.contains(_id)) {
        ids.append(_id);
        settings.setValue(QStringLiteral("ColorModes/ids"), ids);
    }

    return true;
}

/**
 * Removes the color mode from settings
 */
bool ColorMode::remove() const {
    if (isBuiltIn()) {
        qWarning() << "Cannot remove built-in color mode:" << _name;
        return false;
    }

    SettingsService settings;
    const QString group = settingsGroupKey(_id);

    settings.remove(group);

    // Update the list of color mode IDs
    QStringList ids = settings.value(QStringLiteral("ColorModes/ids")).toStringList();
    ids.removeAll(_id);
    settings.setValue(QStringLiteral("ColorModes/ids"), ids);

    return true;
}

/**
 * Sets this color mode as the current one
 */
void ColorMode::setAsCurrent() const {
    SettingsService settings;
    settings.setValue(QStringLiteral("ColorModes/currentId"), _id);
}

/**
 * Checks if this is the currently active color mode
 */
bool ColorMode::isCurrent() const { return _id == currentColorModeId(); }

/**
 * Fetches a color mode by ID from settings
 */
ColorMode ColorMode::fetch(const QString &id) {
    SettingsService settings;
    const QString group = settingsGroupKey(id);

    ColorMode mode;
    mode._id = id;
    mode._name = settings.value(group + QStringLiteral("/name")).toString();
    mode._darkMode = settings.value(group + QStringLiteral("/darkMode")).toBool();
    mode._darkModeColors = settings.value(group + QStringLiteral("/darkModeColors")).toBool();
    mode._darkModeTrayIcon = settings.value(group + QStringLiteral("/darkModeTrayIcon")).toBool();
    mode._darkModeIconTheme = settings.value(group + QStringLiteral("/darkModeIconTheme")).toBool();
    mode._internalIconTheme = settings.value(group + QStringLiteral("/internalIconTheme")).toBool();
    mode._systemIconTheme = settings.value(group + QStringLiteral("/systemIconTheme")).toBool();
    mode._editorColorSchemaKey =
        settings.value(group + QStringLiteral("/editorColorSchemaKey")).toString();

    return mode;
}

/**
 * Fetches all color modes from settings
 */
QList<ColorMode> ColorMode::fetchAll() {
    SettingsService settings;
    QList<ColorMode> modes;

    QStringList ids = settings.value(QStringLiteral("ColorModes/ids")).toStringList();

    for (const QString &id : ids) {
        modes.append(fetch(id));
    }

    return modes;
}

/**
 * Returns the current color mode
 */
ColorMode ColorMode::currentColorMode() {
    const QString id = currentColorModeId();
    if (id.isEmpty()) {
        // Default to light mode
        return fetch(LightModeId);
    }
    return fetch(id);
}

/**
 * Returns the current color mode ID
 */
QString ColorMode::currentColorModeId() {
    SettingsService settings;
    return settings.value(QStringLiteral("ColorModes/currentId"), LightModeId).toString();
}

/**
 * Ensures that the two built-in color modes (Light and Dark) exist in settings.
 * On first run the user's existing dark mode, icon theme and editor color
 * schema settings are read so the active built-in mode mirrors the previous
 * configuration.
 */
void ColorMode::ensureBuiltInModesExist() {
    SettingsService settings;
    QStringList ids = settings.value(QStringLiteral("ColorModes/ids")).toStringList();

    // Check whether this is the very first migration (no color modes yet)
    const bool firstMigration = ids.isEmpty();

    // Read the user's existing global settings so we can seed the matching
    // built-in mode with them
    const bool existingDarkMode = settings.value(QStringLiteral("darkMode")).toBool();
    const bool existingDarkModeColors = settings.value(QStringLiteral("darkModeColors")).toBool();
    const bool existingDarkModeTrayIcon =
        settings.value(QStringLiteral("darkModeTrayIcon")).toBool();
    const bool existingDarkModeIconTheme =
        settings.value(QStringLiteral("darkModeIconTheme"), existingDarkMode).toBool();
    const bool existingInternalIconTheme =
        settings.value(QStringLiteral("internalIconTheme")).toBool();
    const bool existingSystemIconTheme = settings.value(QStringLiteral("systemIconTheme")).toBool();
    const QString existingSchemaKey =
        settings.value(QStringLiteral("Editor/CurrentSchemaKey")).toString();

    if (!ids.contains(LightModeId)) {
        ColorMode light;
        light._id = LightModeId;
        light._name = QObject::tr("Light");
        light._darkMode = false;
        light._darkModeColors = false;
        light._darkModeTrayIcon = false;
        light._darkModeIconTheme = false;
        light._editorColorSchemaKey = LightEditorSchemaKey;

        // On first migration, if the user was in light mode, carry over the
        // icon theme and editor color schema settings
        if (firstMigration && !existingDarkMode) {
            light._internalIconTheme = existingInternalIconTheme;
            light._systemIconTheme = existingSystemIconTheme;
            if (!existingSchemaKey.isEmpty()) {
                light._editorColorSchemaKey = existingSchemaKey;
            }
        }

        light.store();
    }

    if (!ids.contains(DarkModeId)) {
        ColorMode dark;
        dark._id = DarkModeId;
        dark._name = QObject::tr("Dark");
        dark._darkMode = true;
        dark._darkModeColors = true;
        dark._darkModeTrayIcon = true;
        dark._darkModeIconTheme = true;
        dark._editorColorSchemaKey = DarkEditorSchemaKey;

        // On first migration, if the user was in dark mode, carry over the
        // icon theme and editor color schema settings
        if (firstMigration && existingDarkMode) {
            dark._darkModeColors = existingDarkModeColors;
            dark._darkModeTrayIcon = existingDarkModeTrayIcon;
            dark._darkModeIconTheme = existingDarkModeIconTheme;
            dark._internalIconTheme = existingInternalIconTheme;
            dark._systemIconTheme = existingSystemIconTheme;
            if (!existingSchemaKey.isEmpty()) {
                dark._editorColorSchemaKey = existingSchemaKey;
            }
        }

        dark.store();
    }

    // Set default current mode if not set, based on whether dark mode was
    // enabled before the migration
    if (settings.value(QStringLiteral("ColorModes/currentId")).toString().isEmpty()) {
        settings.setValue(QStringLiteral("ColorModes/currentId"),
                          existingDarkMode ? DarkModeId : LightModeId);
    }
}

/**
 * Creates a new custom color mode with a unique ID
 */
ColorMode ColorMode::createCustom(const QString &name) {
    ColorMode mode;
    mode._id = QStringLiteral("ColorMode-") +
               QUuid::createUuid().toString().remove(QLatin1Char('{')).remove(QLatin1Char('}'));
    mode._name = name;
    mode._editorColorSchemaKey = LightEditorSchemaKey;
    return mode;
}
