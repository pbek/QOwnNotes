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
#include "enchantclient.h"
#include "enchantdict.h"

#include <QDebug>

using namespace Sonnet;

static void enchantDictDescribeFn(const char *const lang_tag, const char *const provider_name,
                                  const char *const provider_desc, const char *const provider_file,
                                  void *user_data)
{
    QSpellEnchantClient *client
        = reinterpret_cast<QSpellEnchantClient *>(user_data);
    //qDebug()<<lang_tag<<provider_name<<provider_desc<<provider_file;
    Q_UNUSED(provider_name);
    Q_UNUSED(provider_desc);
    Q_UNUSED(provider_file);
    client->addLanguage(QString::fromLatin1(lang_tag));
}

QSpellEnchantClient::QSpellEnchantClient(QObject *parent)
    : Client(parent)
{
    m_broker = enchant_broker_init();
    enchant_broker_list_dicts(m_broker,
                              enchantDictDescribeFn,
                              this);
}

QSpellEnchantClient::~QSpellEnchantClient()
{
    enchant_broker_free(m_broker);
}

SpellerPlugin *QSpellEnchantClient::createSpeller(
    const QString &language)
{
    EnchantDict *dict = enchant_broker_request_dict(m_broker,
                                                    language.toUtf8().constData());

    if (!dict) {
#ifndef NDEBUG
        const char *err = enchant_broker_get_error(m_broker);
        qDebug() << "Couldn't create speller for" << language << ": " << err;
#endif
        return 0;
    } else {
        //Enchant caches dictionaries, so it will always return the same one.
        int refs = m_dictRefs[dict];
        ++refs;
        m_dictRefs[dict] = refs;
        return new QSpellEnchantDict(this, m_broker, dict, language);
    }
}

QStringList QSpellEnchantClient::languages() const
{
    return m_languages.toList();
}

void QSpellEnchantClient::addLanguage(const QString &lang)
{
    m_languages.insert(lang);
}

void QSpellEnchantClient::removeDictRef(EnchantDict *dict)
{
    int refs = m_dictRefs[dict];
    --refs;
    m_dictRefs[dict] = refs;
    if (refs <= 0) {
        m_dictRefs.remove(dict);
        enchant_broker_free_dict(m_broker, dict);
    }
}
