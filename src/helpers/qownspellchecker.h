/*
 * Copyright (c) 2019-2021 Waqar Ahmed <waqar.17a@gmail.com>
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
 * QOwnNotes Spell Check wrapper
 */

#ifndef QOWNSPELLCHECKER_H
#define QOWNSPELLCHECKER_H

#include <libraries/sonnet/src/core/speller.h>

#include "LanguageCache.h"
#include "libraries/sonnet/src/core/languagefilter_p.h"

class QOwnSpellChecker {
   public:
    ~QOwnSpellChecker();

    static QOwnSpellChecker *instance();

    /**
     * Returns the current language used for spell checking.
     *
     * @return the language code for the current language.
     */
    QString currentLanguage() const;

    /**
     * Set language to use for spell checking.
     *
     * @param language the language code for the new language to use.
     */
    void setCurrentLanguage(const QString &language);

    /**
     * Checks if a given word is marked as misspelled by the highlighter.
     *
     * @param word the word to be checked
     * @return true if the given word is misspelled.
     */
    inline bool isWordMisspelled(const QString &word) { return _spellchecker->isMisspelled(word); }

    /**
     * Returns a list of suggested replacements for the given misspelled word.
     * If the word is not misspelled, the list will be empty.
     *
     * @param word the misspelled word
     * @param max at most this many suggestions will be returned. If this is
     *            -1, as many suggestions as the spell backend supports will
     *            be returned.
     * @return a list of suggested replacements for the word
     */
    QStringList suggestionsForWord(const QString &word, int max);

    /**
     * Returns a list of suggested replacements for the given misspelled word.
     * If the word is not misspelled, the list will be empty.
     *
     * @param word the misspelled word
     * @param cursor the cursor pointing to the beginning of that word. This is
     * used to determine the language to use, when AutoDetectLanguage is
     * enabled.
     * @param max at most this many suggestions will be returned. If this is
     *            -1, as many suggestions as the spell backend supports will
     *            be returned.
     * @return a list of suggested replacements for the word
     */
    QStringList suggestionsForWord(const QString &word, const QTextCursor &cursor, int max);

    /**
     * @short Enable/Disable spell checking.
     *
     * If @p active is true then spell checking is enabled; otherwise it
     * is disabled.
     *
     * @param active if true, then spell checking is enabled
     */
    void setActive(bool active);

    /**
     * Returns the state of spell checking.
     *
     * @return true if spell checking is active
     *
     * @see setActive()
     */
    bool isActive() const;

    /**
     * Adds the given word permanently to the dictionary. It will never
     * be marked as misspelled again, even after restarting the application.
     *
     * @param word the word which will be added to the dictionary
     */
    void addWordToDictionary(const QString &word);

    /**
     * Ignores the given word. This word will not be marked misspelled for
     * this session. It will again be marked as misspelled when creating
     * new highlighters.
     *
     * @param word the word which will be ignored
     */
    void ignoreWord(const QString &word);

    /**
     *
     * @return true if spellchecker supports current language
     */
    bool isValid();

    bool testAttribute(Sonnet::Speller::Attribute) const;

    void setAutoDetect(bool autoDetect);
    bool isAutoDetectOn() const;

    Sonnet::LanguageFilter *languageFilter() const;

    Sonnet::WordTokenizer *wordTokenizer() const;

   private:
    QOwnSpellChecker();
    Sonnet::Speller *_spellchecker;
    Sonnet::LanguageFilter *_languageFilter;
    Sonnet::WordTokenizer *_wordTokenizer;
    bool _active;
    bool _autoDetect;
};

#endif    // QOWNSPELLCHECKER_H
