/*
 * Copyright (c) 2019 Waqar Ahmed <waqar.17a@gmail.com>
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
#include <QSettings>
#include <QDebug>

QOwnSpellChecker::QOwnSpellChecker(QObject *parent) : QObject(parent) {
    spellchecker = new Sonnet::Speller();
    QSettings settings;
    active = settings.value("checkSpelling", true).toBool();
    QString language = settings.value("spellCheckLanguage", "auto").toString();

#ifdef Q_OS_MACOS
    QString s = spellchecker->availableLanguages().at(0);
    spellchecker->setDefaultLanguage(s);
#endif
}

QOwnSpellChecker::~QOwnSpellChecker() {
    delete spellchecker;
}


QString QOwnSpellChecker::currentLanguage() const {
    return spellchecker->language();
}

void QOwnSpellChecker::setCurrentLanguage(const QString &lang) {
    spellchecker->setLanguage(lang);
}

bool QOwnSpellChecker::isWordMisspelled(const QString &word) {
    return spellchecker->isMisspelled(word);
}

void QOwnSpellChecker::setActive(bool _active)
{
    if (active == _active) {
        return;
    }
    active = _active;
}

bool QOwnSpellChecker::isActive() const {
    return active;
}

void QOwnSpellChecker::addWordToDictionary(const QString &word) {
    spellchecker->addToPersonal(word);
}

void QOwnSpellChecker::ignoreWord(const QString &word) {
    spellchecker->addToSession(word);
}

bool QOwnSpellChecker::isValid() {
    return spellchecker->isValid();
}

bool QOwnSpellChecker::testAttribute(Sonnet::Speller::Attribute attr) const {
    return spellchecker->testAttribute(attr);
}

void QOwnSpellChecker::setAutoDetect(bool _autoDetect) {
    if (_autoDetect == autoDetect) {
        return;
    }
    autoDetect = _autoDetect;
}

bool QOwnSpellChecker::isAutoDetectOn() const {
    return autoDetect;
}

QStringList QOwnSpellChecker::suggestionsForWord(const QString &word, int max) {
    QStringList suggestions = spellchecker->suggest(word);
    if (max >= 0 && suggestions.count() > max) {
        suggestions = suggestions.mid(0, max);
    }
    return suggestions;
}

QStringList QOwnSpellChecker::suggestionsForWord(const QString &word,
                                                 const QTextCursor &cursor, int max) {
    //detect the lang
    if (autoDetect) {
        LanguageCache *cache = dynamic_cast<LanguageCache *>(cursor.block().userData());
        if (cache) {
            const QString cachedLanguage = cache->languageAtPos(cursor.positionInBlock());
            if (!cachedLanguage.isEmpty()) {
                spellchecker->setLanguage(cachedLanguage);
            }
        }
    }
    //else use the user specified lang
    QStringList suggestions = spellchecker->suggest(word);
    if (max >= 0 && suggestions.count() > max) {
        suggestions = suggestions.mid(0, max);
    }
    return suggestions;
}
