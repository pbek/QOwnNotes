/**
 * kspell_hspelldict.h
 *
 * Copyright (C)  2003  Zack Rusin <zack@kde.org>
 * Copyright (C)  2005  Mashrab Kuvatov <kmashrab@uni-bremen.de>
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
#ifndef KSPELL_HSPELLDICT_H
#define KSPELL_HSPELLDICT_H

#include <QSet>

#include "spellerplugin_p.h"
/* libhspell is a C library and it does not have #ifdef __cplusplus */
extern "C" {
#include "hspell.h"
}

class HSpellDict : public Sonnet::SpellerPlugin
{
public:
    explicit HSpellDict(const QString &lang);
    ~HSpellDict();
    bool isCorrect(const QString &word) const override;

    QStringList suggest(const QString &word) const override;

    bool storeReplacement(const QString &bad, const QString &good) override;

    bool addToPersonal(const QString &word) override;
    bool addToSession(const QString &word) override;
    inline bool isInitialized() const
    {
        return initialized;
    }

private:
    void storePersonalWords();

    struct dict_radix *m_speller;
    QTextCodec *codec;
    bool initialized;
    QSet<QString> m_sessionWords;
    QSet<QString> m_personalWords;
    QHash<QString, QString> m_replacements;
};

#endif
