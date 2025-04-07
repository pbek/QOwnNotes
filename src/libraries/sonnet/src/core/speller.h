/*
 *
 * Copyright (C)  2007  Zack Rusin <zack@kde.org>
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
#ifndef SONNET_SPELLER_H
#define SONNET_SPELLER_H

#include <QMap>
#include <QString>
#include <QStringList>

namespace Sonnet {
class SpellerPrivate;
/**
 * Spell checker object.
 *
 * @short class used for actuall spell checking
 */
class Speller {
   public:
    explicit Speller(const QString &lang = QString());
    ~Speller();

    Speller(const Speller &speller);
    Speller &operator=(const Speller &speller);

    /**
     * Returns true if the speller supports currently selected
     * language.
     */
    bool isValid() const;

    /**
     * Sets the language supported by this speller.
     */
    void setLanguage(const QString &lang);

    /**
     * Returns language supported by this speller.
     */
    QString language() const;

    /**
     * Checks the given word.
     * @return false if the word is misspelled. true otherwise
     */
    bool isCorrect(const QString &word) const;

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
    QStringList suggest(const QString &word) const;

    /**
     * Convience method calling isCorrect() and suggest()
     * if the word isn't correct.
     */
    bool checkAndSuggest(const QString &word, QStringList &suggestions) const;

    /**
     * Stores user defined good replacement for the bad word.
     * @returns true on success
     */
    bool storeReplacement(const QString &bad, const QString &good);

    /**
     * Adds word to the list of of personal words.
     * @return true on success
     */
    bool addToPersonal(const QString &word);

    /**
     * Adds word to the words recognizable in the current session.
     * @return true on success
     */
    bool addToSession(const QString &word);

   public:    // Configuration API
    enum Attribute { CheckUppercase, SkipRunTogether, AutoDetectLanguage };
    void save();
    void restore();

    /**
     * Returns names of all supported backends (e.g. ISpell, ASpell)
     */
    QString availableBackends() const;

    /**
     * Returns a list of supported languages.
     *
     * Note: use availableDictionaries
     */
    static QStringList availableLanguages();

    /**
     * Returns a localized list of names of supported languages.
     *
     * Note: use availableDictionaries
     */
    static QStringList availableLanguageNames();

    /**
     * Returns a map of all available dictionaies with language descriptions and
     * their codes. The key is the description, the code the value.
     */
    QMap<QString, QString> availableDictionaries() const;

    /**
     * Returns a map of user preferred dictionaies with language descriptions and
     * their codes. The key is the description, the code the value.
     * @since 5.54
     */
    QMap<QString, QString> preferredDictionaries() const;

    void setDefaultLanguage(const QString &lang);
    QString defaultLanguage() const;

    void setDefaultClient(const QString &client);
    QString defaultClient() const;

    void setAttribute(Attribute attr, bool b = true);
    bool testAttribute(Attribute attr) const;

   private:
    SpellerPrivate *const d;
};
}    // namespace Sonnet
#endif
