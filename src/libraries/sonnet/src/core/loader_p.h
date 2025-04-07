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
#ifndef SONNET_LOADER_P_H
#define SONNET_LOADER_P_H

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QStringList>

namespace Sonnet {
class Settings;
class SpellerPlugin;
class LoaderPrivate;
/**
 * \internal
 * @short Class used to deal with dictionaries
 *
 * This class manages all dictionaries. It's the top level
 * Sonnet class, you can think of it as the kernel or manager
 * of the Sonnet architecture.
 */
class Loader : public QObject {
    Q_OBJECT
   public:
    /**
     * Constructs the loader.
     *
     * It's very important that you leave the return value in a Loader::Ptr.
     * Loader is reference counted so if you don't want to have it deleted
     * under you simply have to hold it in a Loader::Ptr for as long as
     * you're using it.
     */
    static Loader *openLoader();

   public:
    Loader();
    ~Loader();

    /**
     * Returns dictionary for the given language and preferred client.
     *
     * @param language specifies the language of the dictionary. If an
     *        empty string will be passed the default language will
     *        be used. Has to be one of the values returned by
     *        \ref languages()
     * @param client specifies the preferred client. If no client is
     *               specified a client which supports the given
     *               language is picked. If a few clients supports
     *               the same language the one with the biggest
     *               reliability value is returned.
     *
     */
    SpellerPlugin *createSpeller(const QString &language = QString(),
                                 const QString &client = QString()) const;

    /**
     * Returns a shared, cached, dictionary for the given language.
     *
     * @param language specifies the language of the dictionary. If an
     *        empty string will be passed the default language will
     *        be used. Has to be one of the values returned by
     *        \ref languages()
     */
    QSharedPointer<SpellerPlugin> cachedSpeller(const QString &language);

    /**
     * Returns a shared, cached, dictionary for the given language.
     *
     * @param language specifies the language of the dictionary. If an
     *        empty string will be passed the default language will
     *        be used. Has to be one of the values returned by
     *        \ref languages()
     */
    void clearSpellerCache();

    /**
     * Returns names of all supported clients (e.g. ISpell, ASpell)
     */
    QString clients() const;

    /**
     * Returns a list of supported languages.
     */
    QStringList languages() const;

    /**
     * Returns a localized list of names of supported languages.
     */
    QStringList languageNames() const;

    /**
     * @param langCode the dictionary name/language code, e.g. en_gb
     * @return the localized language name, e.g. "British English"
     * @since 4.2
     */
    QString languageNameForCode(const QString &langCode) const;

    /**
     * Returns the Settings object used by the loader.
     */
    Settings *settings() const;
   Q_SIGNALS:
    /**
     * Signal is emitted whenever the Settings object
     * associated with this Loader changes.
     */
    void configurationChanged();

    /**
     * Emitted when loading a dictionary fails, so that Ui parts can
     * display an appropriate error message informing the user about
     * the issue.
     * @param language the name of the dictionary that failed to be loaded
     * @since 5.56
     */
    void loadingDictionaryFailed(const QString &language) const;

   protected:
    friend class Settings;
    void changed();

   private:
    void loadPlugins();
    void loadPlugin(const QString &pluginPath);

   private:
    LoaderPrivate *const d;
};
}    // namespace Sonnet

#endif    // SONNET_LOADER_P_H
