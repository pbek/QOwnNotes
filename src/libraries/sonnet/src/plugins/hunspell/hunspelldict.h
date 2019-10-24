/**
 * kspell_aspelldict.h
 *
 * Copyright (C)  2009  Montel Laurent <montel@kde.org>
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
#ifndef KSPELL_HUNSPELLDICT_H
#define KSPELL_HUNSPELLDICT_H

#include "spellerplugin_p.h"
#include "hunspell/src/hunspell/hunspell.hxx"

class HunspellDict : public Sonnet::SpellerPlugin
{
public:
    explicit HunspellDict(const QString &lang, QString path);
    ~HunspellDict() override;
    bool isCorrect(const QString &word) const override;

    QStringList suggest(const QString &word) const override;

    bool storeReplacement(const QString &bad, const QString &good) override;

    bool addToPersonal(const QString &word) override;
    bool addToSession(const QString &word) override;

private:
    QByteArray toDictEncoding(const QString &word) const;

    Hunspell *m_speller = nullptr;
    QTextCodec *m_codec = nullptr;
};

#endif
