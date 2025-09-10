/*
 *
 * Copyright (C)  2004  Zack Rusin <zack@kde.org>
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
#ifndef SONNET_SPELLERPLUGIN_P_H
#define SONNET_SPELLERPLUGIN_P_H

#include <QString>
#include <QStringList>

namespace Sonnet {
/**
 * Class is returned by from Loader. It acts
 * as the actual spellchecker.
 *
 * @author Zack Rusin <zack@kde.org>
 * @short class used for actuall spell checking
 */
class SpellerPluginPrivate;
class SpellerPlugin {
   public:
    virtual ~SpellerPlugin();

    /**
     * Checks the given word.
     * @return false if the word is misspelled. true otherwise
     */
    virtual bool isCorrect(const QString &word) const = 0;

    /**
     * Checks the given word.
     * @return true if the word is misspelled. false otherwise
     */
    bool isMisspelled(const QString &word) const;

    /**
     * Fetches suggestions for the word.
     *
     * @return list of all suggestions for the word
     */
    virtual QStringList suggest(const QString &word) const = 0;

    /**
     * Convience method calling isCorrect() and suggest()
     * if the word isn't correct.
     */
    virtual bool checkAndSuggest(const QString &word, QStringList &suggestions) const;

    /**
     * Stores user defined good replacement for the bad word.
     * @returns true on success
     */
    virtual bool storeReplacement(const QString &bad, const QString &good) = 0;

    /**
     * Adds word to the list of of personal words.
     * @return true on success
     */
    virtual bool addToPersonal(const QString &word) = 0;

    /**
     * Adds word to the words recognizable in the current session.
     * @return true on success
     */
    virtual bool addToSession(const QString &word) = 0;

    /**
     * Returns language supported by this dictionary.
     */
    QString language() const;

   protected:
    SpellerPlugin(const QString &lang);

   private:
    SpellerPluginPrivate *const d;
};
}    // namespace Sonnet

#endif
