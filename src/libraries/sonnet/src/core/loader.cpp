/**
 *
 * Copyright (C)  2003  Zack Rusin <zack@kde.org>
 * Copyright (C)  2012  Martin Sandsmark <martin.sandsmark@kde.org>
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
#include "loader_p.h"
#include "settings_p.h"
#include "client_p.h"
#include "spellerplugin_p.h"

#include <QHash>
#include <QMap>
#include <QLocale>
#include <QCoreApplication>
#include <QPluginLoader>
#include "core_debug.h"
#include <QDir>

#include <algorithm>

#define SONNET_STATIC 1

#ifdef SONNET_STATIC


#include "../plugins/hunspell/hunspellclient.h"

/*
#ifdef Q_OS_MACOS
#include "../plugins/nsspellchecker/nsspellcheckerclient.h"
#endif
*/
#endif

namespace Sonnet {
class LoaderPrivate
{
public:
    Settings *settings;

    // <language, Clients with that language >
    QMap<QString, QVector<Client *> > languageClients;
    QStringList clients;

    QStringList languagesNameCache;
    QHash<QString, QSharedPointer<SpellerPlugin> > spellerCache;
};

Q_GLOBAL_STATIC(Loader, s_loader)

Loader *Loader::openLoader()
{
    if (s_loader.isDestroyed()) {
        return nullptr;
    }

    return s_loader();
}

Loader::Loader()
    : d(new LoaderPrivate)
{
    d->settings = new Settings(this);
    d->settings->restore();
    loadPlugins();
}

Loader::~Loader()
{
    qCDebug(SONNET_LOG_CORE) << "Removing loader: " << this;
    delete d->settings;
    d->settings = nullptr;
    delete d;
}

SpellerPlugin *Loader::createSpeller(const QString &language, const QString &clientName) const
{
    QString backend = clientName;
    QString plang = language;
    if (plang.isEmpty()) {
        plang = d->settings->defaultLanguage();
    }

    auto clientsItr = d->languageClients.constFind(plang);
    if (clientsItr == d->languageClients.constEnd()) {
        qCWarning(SONNET_LOG_CORE) << "No language dictionaries for the language:" << plang;
        emit loadingDictionaryFailed(plang);
        return nullptr;
    }

    const QVector<Client *> lClients = *clientsItr;

    if (backend.isEmpty()) {
        backend = d->settings->defaultClient();
        if (!backend.isEmpty()) {
            // check if the default client supports the requested language;
            // if it does it will be an element of lClients.
            bool unknown = !std::any_of(lClients.constBegin(), lClients.constEnd(), [backend] (const Client *client) {
                    return client->name() == backend; });
            if (unknown) {
                qCWarning(SONNET_LOG_CORE) << "Default client" << backend << "doesn't support language:" << plang;
                backend = QString();
            }
        }
    }

    QVectorIterator<Client *> itr(lClients);
    while (itr.hasNext()) {
        Client *item = itr.next();
        if (!backend.isEmpty()) {
            if (backend == item->name()) {
                SpellerPlugin *dict = item->createSpeller(plang);
                qCDebug(SONNET_LOG_CORE) << "Using the" << item->name() << "plugin for language" << plang;
                return dict;
            }
        } else {
            //the first one is the one with the highest
            //reliability
            SpellerPlugin *dict = item->createSpeller(plang);
            qCDebug(SONNET_LOG_CORE) << "Using the" << item->name() << "plugin for language" << plang;
            return dict;
        }
    }

    qCWarning(SONNET_LOG_CORE) << "The default client" << backend << "has no language dictionaries for the language:" << plang;
    return nullptr;
}

QSharedPointer<SpellerPlugin> Loader::cachedSpeller(const QString &language)
{
    auto &speller = d->spellerCache[language];
    if (!speller) {
        speller.reset(createSpeller(language));
    }
    return speller;
}

void Loader::clearSpellerCache()
{
    d->spellerCache.clear();
}

QStringList Loader::clients() const
{
    return d->clients;
}

QStringList Loader::languages() const
{
    return d->languageClients.keys();
}

QString Loader::languageNameForCode(const QString &langCode) const
{
    QString currentDictionary = langCode,   // e.g. en_GB-ize-wo_accents
            isoCode,            // locale ISO name
            variantName,         // dictionary variant name e.g. w_accents
            localizedLang,       // localized language
            localizedCountry,    // localized country
            localizedVariant;
    QByteArray variantEnglish; // dictionary variant in English

    int minusPos,          // position of "-" char
        variantCount = 0;  // used to iterate over variantList

    struct variantListType {
        const char *variantShortName;
        const char *variantEnglishName;
    };

    /*
     * This redefines the QT_TRANSLATE_NOOP3 macro provided by Qt to indicate that
     * statically initialised text should be translated so that it expands to just
     * the string that should be translated, making it possible to use it in the
     * single string construct below.
     */
#undef QT_TRANSLATE_NOOP3
#define QT_TRANSLATE_NOOP3(a, b, c) b

    const variantListType variantList[] = {
        { "40", QT_TRANSLATE_NOOP3("Sonnet::Loader", "40", "dictionary variant") },             // what does 40 mean?
        { "60", QT_TRANSLATE_NOOP3("Sonnet::Loader", "60", "dictionary variant") },             // what does 60 mean?
        { "80", QT_TRANSLATE_NOOP3("Sonnet::Loader", "80", "dictionary variant") },             // what does 80 mean?
        { "ise", QT_TRANSLATE_NOOP3("Sonnet::Loader", "-ise suffixes", "dictionary variant") },
        { "ize", QT_TRANSLATE_NOOP3("Sonnet::Loader", "-ize suffixes", "dictionary variant") },
        { "ise-w_accents", QT_TRANSLATE_NOOP3("Sonnet::Loader", "-ise suffixes and with accents",
                                              "dictionary variant") },
        { "ise-wo_accents", QT_TRANSLATE_NOOP3("Sonnet::Loader",
                                               "-ise suffixes and without accents",
                                               "dictionary variant") },
        { "ize-w_accents", QT_TRANSLATE_NOOP3("Sonnet::Loader", "-ize suffixes and with accents",
                                              "dictionary variant") },
        { "ize-wo_accents", QT_TRANSLATE_NOOP3("Sonnet::Loader",
                                               "-ize suffixes and without accents",
                                               "dictionary variant") },
        { "lrg", QT_TRANSLATE_NOOP3("Sonnet::Loader", "large", "dictionary variant") },
        { "med", QT_TRANSLATE_NOOP3("Sonnet::Loader", "medium", "dictionary variant") },
        { "sml", QT_TRANSLATE_NOOP3("Sonnet::Loader", "small", "dictionary variant") },
        { "variant_0", QT_TRANSLATE_NOOP3("Sonnet::Loader", "variant 0", "dictionary variant") },
        { "variant_1", QT_TRANSLATE_NOOP3("Sonnet::Loader", "variant 1", "dictionary variant") },
        { "variant_2", QT_TRANSLATE_NOOP3("Sonnet::Loader", "variant 2", "dictionary variant") },
        { "wo_accents",
          QT_TRANSLATE_NOOP3("Sonnet::Loader", "without accents", "dictionary variant") },
        { "w_accents", QT_TRANSLATE_NOOP3("Sonnet::Loader", "with accents", "dictionary variant") },
        { "ye",
          QT_TRANSLATE_NOOP3("Sonnet::Loader", "with ye, modern russian",
                             "dictionary variant") },
        { "yeyo", QT_TRANSLATE_NOOP3("Sonnet::Loader", "with yeyo, modern and old russian",
                                     "dictionary variant") },
        { "yo",
          QT_TRANSLATE_NOOP3("Sonnet::Loader", "with yo, old russian", "dictionary variant") },
        { "extended", QT_TRANSLATE_NOOP3("Sonnet::Loader", "extended", "dictionary variant") },
        { nullptr, nullptr }
    };

    minusPos = currentDictionary.indexOf(QLatin1Char('-'));
    if (minusPos != -1) {
        variantName = currentDictionary.right(currentDictionary.length() - minusPos - 1);
        while (variantList[variantCount].variantShortName != nullptr) {
            if (QLatin1String(variantList[variantCount].variantShortName) == variantName) {
                break;
            } else {
                variantCount++;
            }
        }
        if (variantList[variantCount].variantShortName != nullptr) {
            variantEnglish = variantList[variantCount].variantEnglishName;
        } else {
            variantEnglish = variantName.toLatin1();
        }

        localizedVariant = tr(variantEnglish.constData(), "dictionary variant");
        isoCode = currentDictionary.left(minusPos);
    } else {
        isoCode = currentDictionary;
    }

    QLocale locale(isoCode);
    localizedCountry = locale.nativeCountryName();
    localizedLang = locale.nativeLanguageName();

    if (localizedLang.isEmpty() && localizedCountry.isEmpty()) {
        return isoCode;    // We have nothing
    }

    if (!localizedCountry.isEmpty() && !localizedVariant.isEmpty()) { // We have both a country name and a variant
        return tr("%1 (%2) [%3]", "dictionary name; %1 = language name, %2 = country name and %3 = language variant name"
                  ).arg(localizedLang, localizedCountry, localizedVariant);
    } else if (!localizedCountry.isEmpty()) { // We have a country name
        return tr("%1 (%2)", "dictionary name; %1 = language name, %2 = country name"
                  ).arg(localizedLang, localizedCountry);
    } else { // We only have a language name
        return localizedLang;
    }
}

QStringList Loader::languageNames() const
{
    /* For whatever reason languages() might change. So,
     * to be in sync with it let's do the following check.
     */
    if (d->languagesNameCache.count() == languages().count()) {
        return d->languagesNameCache;
    }

    QStringList allLocalizedDictionaries;
    for (const QString &langCode : languages()) {
        allLocalizedDictionaries.append(languageNameForCode(langCode));
    }
    // cache the list
    d->languagesNameCache = allLocalizedDictionaries;
    return allLocalizedDictionaries;
}

Settings *Loader::settings() const
{
    return d->settings;
}

void Loader::loadPlugins()
{

#ifndef SONNET_STATIC
    const QStringList libPaths = QCoreApplication::libraryPaths() << QStringLiteral(INSTALLATION_PLUGIN_PATH);
    const QLatin1String pathSuffix("/kf5/sonnet/");
    int plugins = 0;
    for (const QString &libPath : libPaths) {
        QDir dir(libPath + pathSuffix);
        if (!dir.exists()) {
            continue;
        }
        for (const QString &fileName : dir.entryList(QDir::Files)) {
            loadPlugin(dir.absoluteFilePath(fileName));
            plugins++;
        }
    }
    if (plugins == 0) {
        qCWarning(SONNET_LOG_CORE) << "Sonnet: No speller backends available!";
    }
#else
//#ifdef Q_OS_MACOS
//    loadPlugin(QStringLiteral("NSSpellchecker"));
//#endif //define mac
//#ifndef Q_OS_MACOS
    loadPlugin(QStringLiteral("Hunspell"));
//#endif //not def mac
#endif //not static
}

void Loader::loadPlugin(const QString &pluginPath)
{
#ifndef SONNET_STATIC
    QPluginLoader plugin(pluginPath);
    if (!plugin.load()) { // We do this separately for better error handling
        qCWarning(SONNET_LOG_CORE) << "Sonnet: Unable to load plugin" << pluginPath << "Error:"
                                   << plugin.errorString();
        return;
    }

    Client *client = qobject_cast<Client *>(plugin.instance());
    if (!client) {
        qCWarning(SONNET_LOG_CORE) << "Sonnet: Invalid plugin loaded" << pluginPath;
        plugin.unload(); // don't leave it in memory
        return;
    }
#else
//hunspell only for non Mac
    Client *client = nullptr;
//#ifndef Q_OS_MACOS
    if (pluginPath == QLatin1String("Hunspell")) {
        client = new HunspellClient(this);
    }
//#endif //not mac
//#ifdef Q_OS_MACOS
//    if (pluginPath == QLatin1String("NSSpellchecker")) {
    //    else {
//        client = new NSSpellCheckerClient(this);
//    }
//#endif //mac
#endif

    const QStringList languages = client->languages();
    d->clients.append(client->name());

    for (const QString &language : languages) {
        QVector<Client *> &languageClients = d->languageClients[language];
        if (languageClients.isEmpty()
            || client->reliability() < languageClients.first()->reliability()) {
            languageClients.append(client);    // less reliable, to the end
        } else {
            languageClients.prepend(client);    // more reliable, to the front
        }
    }
}

void Loader::changed()
{
    emit configurationChanged();
}
}
