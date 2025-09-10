/**
 *
 * Copyright (C)  2003  Zack Rusin <zack@kde.org>
 * Copyright (C)  2006  Laurent Montel <montel@kde.org>
 * Copyright (C)  2013  Martin Sandsmark <martin.sandsmark@kde.org>
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
#include <QLocale>
#include <QMap>
#include <QSettings>

#include "loader_p.h"
#include "settings_p.h"

namespace Sonnet {
class SettingsPrivate {
   public:
    Loader *loader = nullptr;    // can't be a Ptr since we don't want to hold a ref on it
    bool modified = false;

    QString defaultLanguage;
    QStringList preferredLanguages;
    QString defaultClient;

    bool checkUppercase = false;
    bool skipRunTogether = false;
    bool backgroundCheckerEnabled = false;
    bool checkerEnabledByDefault = false;
    bool autodetectLanguage = false;

    int disablePercentage;
    int disableWordCount;

    QMap<QString, bool> ignore;
};

Settings::Settings(Loader *loader) : d(new SettingsPrivate) {
    d->loader = loader;

    d->modified = false;
    d->checkerEnabledByDefault = false;
    restore();
}

Settings::~Settings() { delete d; }

bool Settings::setDefaultLanguage(const QString &lang) {
    const QStringList cs = d->loader->languages();
    if (cs.indexOf(lang) != -1 && d->defaultLanguage != lang) {
        d->defaultLanguage = lang;
        d->modified = true;
        d->loader->changed();
        return true;
    }
    return false;
}

QString Settings::defaultLanguage() const { return d->defaultLanguage; }

bool Settings::setPreferredLanguages(const QStringList &lang) {
    if (d->preferredLanguages != lang) {
        d->modified = true;
        d->preferredLanguages = lang;
        return true;
    }

    return false;
}

QStringList Settings::preferredLanguages() const { return d->preferredLanguages; }

bool Settings::setDefaultClient(const QString &client) {
    // Different from setDefaultLanguage because
    // the number of clients can't be even close
    // as big as the number of languages
    if (d->loader->clients().contains(client)) {
        d->defaultClient = client;
        d->modified = true;
        d->loader->changed();
        return true;
    }
    return false;
}

QString Settings::defaultClient() const { return d->defaultClient; }

bool Settings::setCheckUppercase(bool check) {
    if (d->checkUppercase != check) {
        d->modified = true;
        d->checkUppercase = check;
        return true;
    }
    return false;
}

bool Settings::checkUppercase() const { return d->checkUppercase; }

bool Settings::setAutodetectLanguage(bool detect) {
    if (d->autodetectLanguage != detect) {
        d->modified = true;
        d->autodetectLanguage = detect;
        return true;
    }
    return false;
}

bool Settings::autodetectLanguage() const { return d->autodetectLanguage; }

bool Settings::setSkipRunTogether(bool skip) {
    if (d->skipRunTogether != skip) {
        d->modified = true;
        d->skipRunTogether = skip;
        return true;
    }
    return false;
}

bool Settings::skipRunTogether() const { return d->skipRunTogether; }

bool Settings::setCheckerEnabledByDefault(bool check) {
    if (d->checkerEnabledByDefault != check) {
        d->modified = true;
        d->checkerEnabledByDefault = check;
        return true;
    }
    return false;
}

bool Settings::checkerEnabledByDefault() const { return d->checkerEnabledByDefault; }

bool Settings::setBackgroundCheckerEnabled(bool enable) {
    if (d->backgroundCheckerEnabled != enable) {
        d->modified = true;
        d->backgroundCheckerEnabled = enable;
        return true;
    }
    return false;
}

bool Settings::backgroundCheckerEnabled() const { return d->backgroundCheckerEnabled; }

bool Settings::setCurrentIgnoreList(const QStringList &ignores) {
    bool changed = setQuietIgnoreList(ignores);
    d->modified = true;
    return changed;
}

bool Settings::setQuietIgnoreList(const QStringList &ignores) {
    bool changed = false;
    d->ignore = QMap<QString, bool>();    // clear out
    for (QStringList::const_iterator itr = ignores.begin(); itr != ignores.end(); ++itr) {
        d->ignore.insert(*itr, true);
        changed = true;
    }
    return changed;
}

QStringList Settings::currentIgnoreList() const { return d->ignore.keys(); }

bool Settings::addWordToIgnore(const QString &word) {
    if (!d->ignore.contains(word)) {
        d->modified = true;
        d->ignore.insert(word, true);
        return true;
    }
    return false;
}

bool Settings::ignore(const QString &word) { return d->ignore.contains(word); }

int Settings::disablePercentageWordError() const { return d->disablePercentage; }

int Settings::disableWordErrorCount() const { return d->disableWordCount; }

void Settings::save() {
    QSettings settings(QStringLiteral("KDE"), QStringLiteral("Sonnet"));
    settings.setValue(QStringLiteral("defaultClient"), d->defaultClient);
    settings.setValue(QStringLiteral("defaultLanguage"), d->defaultLanguage);
    settings.setValue(QStringLiteral("preferredLanguages"), d->preferredLanguages);
    settings.setValue(QStringLiteral("checkUppercase"), d->checkUppercase);
    settings.setValue(QStringLiteral("skipRunTogether"), d->skipRunTogether);
    settings.setValue(QStringLiteral("backgroundCheckerEnabled"), d->backgroundCheckerEnabled);
    settings.setValue(QStringLiteral("checkerEnabledByDefault"), d->checkerEnabledByDefault);
    settings.setValue(QStringLiteral("autodetectLanguage"), d->autodetectLanguage);
    QString defaultLanguage = QStringLiteral("ignore_%1").arg(d->defaultLanguage);
    if (settings.contains(defaultLanguage) && d->ignore.isEmpty()) {
        settings.remove(defaultLanguage);
    } else if (!d->ignore.isEmpty()) {
        settings.setValue(defaultLanguage, QStringList(d->ignore.keys()));
    }
}

// A static list of KDE specific words that we want to recognize
static QStringList kdeWords() {
    QStringList l;
    l.append(QStringLiteral("KMail"));
    l.append(QStringLiteral("KOrganizer"));
    l.append(QStringLiteral("KAddressBook"));
    l.append(QStringLiteral("KHTML"));
    l.append(QStringLiteral("KIO"));
    l.append(QStringLiteral("KJS"));
    l.append(QStringLiteral("Konqueror"));
    l.append(QStringLiteral("Sonnet"));
    l.append(QStringLiteral("Kontact"));
    l.append(QStringLiteral("Qt"));
    l.append(QStringLiteral("Okular"));
    l.append(QStringLiteral("KMix"));
    l.append(QStringLiteral("Amarok"));
    l.append(QStringLiteral("KDevelop"));
    l.append(QStringLiteral("Nepomuk"));
    return l;
}

void Settings::restore() {
    QSettings settings(QStringLiteral("KDE"), QStringLiteral("Sonnet"));
    d->defaultClient = settings.value(QStringLiteral("defaultClient"), QString()).toString();
    d->defaultLanguage =
        settings.value(QStringLiteral("defaultLanguage"), QLocale::system().name()).toString();
    d->preferredLanguages = settings.value(QStringLiteral("preferredLanguages")).toStringList();

    // same defaults are in the default filter (filter.cpp)
    d->checkUppercase = settings.value(QStringLiteral("checkUppercase"), true).toBool();
    d->skipRunTogether = settings.value(QStringLiteral("skipRunTogether"), true).toBool();
    d->backgroundCheckerEnabled =
        settings.value(QStringLiteral("backgroundCheckerEnabled"), true).toBool();
    d->checkerEnabledByDefault =
        settings.value(QStringLiteral("checkerEnabledByDefault"), false).toBool();
    d->disablePercentage =
        settings.value(QStringLiteral("Sonnet_AsYouTypeDisablePercentage"), 90).toInt();
    d->disableWordCount =
        settings.value(QStringLiteral("Sonnet_AsYouTypeDisableWordCount"), 100).toInt();
    d->autodetectLanguage = settings.value(QStringLiteral("autodetectLanguage"), true).toBool();

    const QString ignoreEntry = QStringLiteral("ignore_%1").arg(d->defaultLanguage);
    const QStringList ignores = settings.value(ignoreEntry, kdeWords()).toStringList();
    setQuietIgnoreList(ignores);
}

bool Settings::modified() const { return d->modified; }

void Settings::setModified(bool modified) { d->modified = modified; }

}    // namespace Sonnet
