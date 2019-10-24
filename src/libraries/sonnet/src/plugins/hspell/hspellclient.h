/**
 * kspell_hspellclient.h
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
#ifndef KSPELL_HSPELLCLIENT_H
#define KSPELL_HSPELLCLIENT_H

#include "client_p.h"

/* libhspell is a C library and it does not have #ifdef __cplusplus */
extern "C" {
#include "hspell.h"
}

namespace Sonnet {
class SpellerPlugin;
}
using Sonnet::SpellerPlugin;

class HSpellClient : public Sonnet::Client
{
    Q_OBJECT
    Q_INTERFACES(Sonnet::Client)
    Q_PLUGIN_METADATA(IID "org.kde.Sonnet.HSpellClient")
public:
    explicit HSpellClient(QObject *parent = nullptr);
    ~HSpellClient();

    int reliability() const override
    {
        return 20;
    }

    SpellerPlugin *createSpeller(const QString &language) override;

    QStringList languages() const override;

    QString name() const override
    {
        return QString::fromLatin1("HSpell");
    }

private:
};

#endif
