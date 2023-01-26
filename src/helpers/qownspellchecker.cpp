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

#include "qownspellchecker.h"

#include <QDebug>
#include <QSettings>

QOwnSpellChecker::QOwnSpellChecker() : _spellchecker{new Sonnet::Speller()} {
    _languageFilter = new Sonnet::LanguageFilter(new Sonnet::SentenceTokenizer());
    _wordTokenizer = new Sonnet::WordTokenizer();
#ifdef Q_OS_MACOS
    QStringList s = _spellchecker->availableLanguages();
    if (!s.contains(_spellchecker->defaultLanguage()) && !s.isEmpty()) {
        _spellchecker->setDefaultLanguage(s.at(0));
    }
#endif
}

Sonnet::WordTokenizer *QOwnSpellChecker::wordTokenizer() const { return _wordTokenizer; }

Sonnet::LanguageFilter *QOwnSpellChecker::languageFilter() const { return _languageFilter; }

QOwnSpellChecker::~QOwnSpellChecker() {
    delete _languageFilter;
    delete _wordTokenizer;
    delete _spellchecker;
}

QOwnSpellChecker *QOwnSpellChecker::instance() {
    static QOwnSpellChecker qonSpellchecker;
    return &qonSpellchecker;
}

QString QOwnSpellChecker::currentLanguage() const { return _spellchecker->language(); }

void QOwnSpellChecker::setCurrentLanguage(const QString &lang) { _spellchecker->setLanguage(lang); }

void QOwnSpellChecker::setActive(bool active) { _active = active; }

bool QOwnSpellChecker::isActive() const { return _active; }

void QOwnSpellChecker::addWordToDictionary(const QString &word) {
    _spellchecker->addToPersonal(word);
}

void QOwnSpellChecker::ignoreWord(const QString &word) { _spellchecker->addToSession(word); }

bool QOwnSpellChecker::isValid() { return _spellchecker->isValid(); }

bool QOwnSpellChecker::testAttribute(Sonnet::Speller::Attribute attr) const {
    return _spellchecker->testAttribute(attr);
}

void QOwnSpellChecker::setAutoDetect(bool autoDetect) { _autoDetect = autoDetect; }

bool QOwnSpellChecker::isAutoDetectOn() const { return _autoDetect; }

QStringList QOwnSpellChecker::suggestionsForWord(const QString &word, int max) {
    QStringList suggestions = _spellchecker->suggest(word);
    if (max >= 0 && suggestions.count() > max) {
        suggestions = suggestions.mid(0, max);
    }
    return suggestions;
}

QStringList QOwnSpellChecker::suggestionsForWord(const QString &word, const QTextCursor &cursor,
                                                 int max) {
    // detect the lang
    if (_autoDetect) {
        LanguageCache *cache = dynamic_cast<LanguageCache *>(cursor.block().userData());
        if (cache) {
            const QString cachedLanguage = cache->languageAtPos(cursor.positionInBlock());
            if (!cachedLanguage.isEmpty()) {
                _spellchecker->setLanguage(cachedLanguage);
            }
        }
    }
    // else use the user specified lang
    QStringList suggestions = _spellchecker->suggest(word);
    if (max >= 0 && suggestions.count() > max) {
        suggestions = suggestions.mid(0, max);
    }
    return suggestions;
}
