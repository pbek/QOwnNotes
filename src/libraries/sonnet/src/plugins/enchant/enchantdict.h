/**
 * Copyright 2006  Zack Rusin <zack@kde.org>
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
#ifndef QSPELL_ENCHANTDICT_H
#define QSPELL_ENCHANTDICT_H

#include "spellerplugin_p.h"

#include <enchant.h>

class QSpellEnchantClient;

class QSpellEnchantDict : public Sonnet::SpellerPlugin
{
public:
    ~QSpellEnchantDict();
    virtual bool isCorrect(const QString &word) const;

    virtual QStringList suggest(const QString &word) const;

    virtual bool storeReplacement(const QString &bad, const QString &good);

    virtual bool addToPersonal(const QString &word);
    virtual bool addToSession(const QString &word);
protected:
    friend class QSpellEnchantClient;
    QSpellEnchantDict(QSpellEnchantClient *client, EnchantBroker *broker, EnchantDict *dict,
                      const QString &language);

private:
    EnchantBroker *m_broker;
    EnchantDict *m_dict;
    QSpellEnchantClient *m_client;
};

#endif
