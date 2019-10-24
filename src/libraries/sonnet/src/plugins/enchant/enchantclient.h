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
#ifndef QSPELL_ENCHANTCLIENT_H
#define QSPELL_ENCHANTCLIENT_H

#include "spellerplugin_p.h"
#include "client_p.h"

#include <QSet>
#include <enchant.h>

namespace Sonnet {
class SpellerPlugin;
}
using Sonnet::SpellerPlugin;

class QSpellEnchantClient : public Sonnet::Client
{
    Q_OBJECT
    Q_INTERFACES(Sonnet::Client)
    Q_PLUGIN_METADATA(IID "org.kde.Sonnet.EnchantClient")
public:
    explicit QSpellEnchantClient(QObject *parent = nullptr);
    ~QSpellEnchantClient();

    virtual int reliability() const
    {
        return 30;
    }

    virtual SpellerPlugin *createSpeller(const QString &language);

    virtual QStringList languages() const;

    virtual QString name() const
    {
        return QString::fromLatin1("Enchant");
    }

    void addLanguage(const QString &lang);

    void removeDictRef(EnchantDict *dict);

private:
    EnchantBroker *m_broker;
    QSet<QString> m_languages;
    QHash<EnchantDict *, int> m_dictRefs;
};

#endif
