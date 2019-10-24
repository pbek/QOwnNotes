/**
 * kspell_hspelldict.cpp
 *
 * Copyright (C)  2003  Zack Rusin <zack@kde.org>
 * Copyright (C)  2005  Mashrab Kuvatov <kmashrab@uni-bremen.de>
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

#include "hspelldict.h"

#include "hspell_debug.h"

#include <QTextCodec>
#include <QSettings>

using namespace Sonnet;

HSpellDict::HSpellDict(const QString &lang)
    : SpellerPlugin(lang)
{
    int int_error = hspell_init(&m_speller, HSPELL_OPT_DEFAULT);
    if (int_error == -1) {
        qCWarning(SONNET_LOG_HSPELL) << "HSpellDict::HSpellDict: Init failed";
        initialized = false;
    } else {
        /* hspell understans only iso8859-8-i            */
        codec = QTextCodec::codecForName("iso8859-8-i");
        initialized = true;
    }

    QSettings settings(QStringLiteral("KDE"), QStringLiteral("SonnetHSpellPlugin"));
    m_personalWords
        = QSet<QString>::fromList(settings.value(QStringLiteral("PersonalWords"),
                                                 QStringList()).toStringList());
    QVariantHash replacementMap
        = settings.value(QStringLiteral("Replacements"), QVariant()).toHash();
    for (const QString &key : replacementMap.keys()) {
        m_replacements[key] = replacementMap[key].toString();
    }
}

HSpellDict::~HSpellDict()
{
    /* It exists in =< hspell-0.8 */
    if (initialized) {
        hspell_uninit(m_speller);
    }
}

bool HSpellDict::isCorrect(const QString &word) const
{
    if (m_sessionWords.contains(word)) {
        return true;
    }

    if (m_personalWords.contains(word)) {
        return true;
    }

    if (!initialized) {
        // Not much we can do, so just return true (less annoying for the user)
        return true;
    }

    int preflen;
    QByteArray wordISO = codec->fromUnicode(word);

    // returns 1 if the word is correct, 0 otherwise
    int correct = hspell_check_word(m_speller,
                                    wordISO.constData(),
                                    &preflen); //this argument might be removed, it isn't useful

    // gimatria is a representation of numbers with hebrew letters, we accept these
    if (correct != 1) {
        if (hspell_is_canonic_gimatria(wordISO.constData()) != 0) {
            correct = 1;
        }
    }
    return correct == 1;
}

QStringList HSpellDict::suggest(const QString &word) const
{
    QStringList suggestions;

    if (m_replacements.contains(word)) {
        suggestions.append(m_replacements[word]);
    }

    struct corlist correctionList;
    int suggestionCount;
    corlist_init(&correctionList);
    hspell_trycorrect(m_speller, codec->fromUnicode(word).constData(), &correctionList);
    for (suggestionCount = 0; suggestionCount < corlist_n(&correctionList); suggestionCount++) {
        suggestions.append(codec->toUnicode(corlist_str(&correctionList, suggestionCount)));
    }
    corlist_free(&correctionList);
    return suggestions;
}

bool HSpellDict::storeReplacement(const QString &bad, const QString &good)
{
    m_replacements[bad] = good;
    storePersonalWords();
    return true;
}

bool HSpellDict::addToPersonal(const QString &word)
{
    m_personalWords.insert(word);
    storePersonalWords();
    return true;
}

bool HSpellDict::addToSession(const QString &word)
{
    m_sessionWords.insert(word);
    return true;
}

void HSpellDict::storePersonalWords()
{
    QSettings settings(QStringLiteral("KDE"), QStringLiteral("SonnetHSpellPlugin"));
    settings.setValue(QStringLiteral("PersonalWords"), QVariant(m_personalWords.toList()));
    QVariantHash variantHash;
    for (const QString &key : m_replacements.keys()) {
        variantHash[key] = QVariant(m_replacements[key]);
    }
    settings.setValue(QStringLiteral("Replacements"), variantHash);
}
