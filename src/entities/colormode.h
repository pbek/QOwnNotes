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

#include <QList>
#include <QString>

class ColorMode {
   public:
    ColorMode();

    QString getId() const;
    QString getName() const;
    bool isDarkMode() const;
    bool isDarkModeColors() const;
    bool isDarkModeTrayIcon() const;
    bool isDarkModeIconTheme() const;
    bool isInternalIconTheme() const;
    bool isSystemIconTheme() const;
    QString getEditorColorSchemaKey() const;
    bool isBuiltIn() const;

    void setId(const QString &id);
    void setName(const QString &name);
    void setDarkMode(bool value);
    void setDarkModeColors(bool value);
    void setDarkModeTrayIcon(bool value);
    void setDarkModeIconTheme(bool value);
    void setInternalIconTheme(bool value);
    void setSystemIconTheme(bool value);
    void setEditorColorSchemaKey(const QString &key);

    bool store() const;
    bool remove() const;
    void setAsCurrent() const;
    bool isCurrent() const;

    static QList<ColorMode> fetchAll();
    static ColorMode fetch(const QString &id);
    static ColorMode currentColorMode();
    static QString currentColorModeId();
    static void ensureBuiltInModesExist();
    static ColorMode createCustom(const QString &name);

    static const QString LightModeId;
    static const QString DarkModeId;

   private:
    QString _id;
    QString _name;
    bool _darkMode = false;
    bool _darkModeColors = false;
    bool _darkModeTrayIcon = false;
    bool _darkModeIconTheme = false;
    bool _internalIconTheme = false;
    bool _systemIconTheme = false;
    QString _editorColorSchemaKey;

    static QString settingsGroupKey(const QString &id);
};
