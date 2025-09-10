/*
 *
 * Copyright (C)  2003  Zack Rusin <zack@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */
#ifndef SONNET_SETTINGS_P_H
#define SONNET_SETTINGS_P_H

#include <QString>
#include <QStringList>

namespace Sonnet {
class Loader;
class SettingsPrivate;
/**
 * Settings class
 */
class Settings {
   public:
    ~Settings();

    Settings(const Settings &) = delete;
    Settings &operator=(const Settings &) = delete;

    bool modified() const;
    void setModified(bool modified);

    bool setDefaultLanguage(const QString &lang);
    QString defaultLanguage() const;

    bool setPreferredLanguages(const QStringList &lang);
    QStringList preferredLanguages() const;

    bool setDefaultClient(const QString &client);
    QString defaultClient() const;

    bool setCheckUppercase(bool);
    bool checkUppercase() const;

    bool setAutodetectLanguage(bool);
    bool autodetectLanguage() const;

    bool setSkipRunTogether(bool);
    bool skipRunTogether() const;

    bool setBackgroundCheckerEnabled(bool);
    bool backgroundCheckerEnabled() const;

    bool setCheckerEnabledByDefault(bool);
    bool checkerEnabledByDefault() const;

    bool setCurrentIgnoreList(const QStringList &ignores);
    bool addWordToIgnore(const QString &word);
    QStringList currentIgnoreList() const;
    bool ignore(const QString &word);

    void save();
    void restore();

    int disablePercentageWordError() const;
    int disableWordErrorCount() const;

   private:
    void readIgnoreList();
    bool setQuietIgnoreList(const QStringList &ignores);

   private:
    friend class Loader;
    explicit Settings(Loader *loader);

   private:
    SettingsPrivate *const d;
};
}    // namespace Sonnet

#endif    // SONNET_SETTINGS_P_H
