/**
 * voikkoclient.cpp
 *
 * Copyright (C)  2015  Jesse Jaara <jesse.jaara@gmail.com>
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

#include "voikkoclient.h"
#include "voikkodict.h"
#include "voikkodebug.h"

VoikkoClient::VoikkoClient(QObject *parent) : Sonnet::Client(parent)
{
    qCDebug(SONNET_VOIKKO) << "Initializing Voikko spell checker plugin.";

    char **dictionaries = voikkoListSupportedSpellingLanguages(nullptr);

    if (!dictionaries) {
        return;
    }

    for (int i = 0; dictionaries[i] != nullptr; ++i) {
        QString language = QString::fromUtf8(dictionaries[i]);
        m_supportedLanguages.append(language);
        qCDebug(SONNET_VOIKKO) << "Found dictionary for language:" << language;
    }

    voikkoFreeCstrArray(dictionaries);
}

VoikkoClient::~VoikkoClient()
{
}

int VoikkoClient::reliability() const
{
    return 50;
}

Sonnet::SpellerPlugin *VoikkoClient::createSpeller(const QString &language)
{
    VoikkoDict *speller = new VoikkoDict(language);
    if (speller->initFailed()) {
        delete speller;
        return nullptr;
    }

    return speller;
}

QStringList VoikkoClient::languages() const
{
    return m_supportedLanguages;
}

QString VoikkoClient::name() const
{
    return QStringLiteral("Voikko");
}
