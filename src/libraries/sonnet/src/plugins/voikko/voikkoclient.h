/**
 * voikkoclient.h
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

#ifndef SONNET_VOIKKOCLIENT_H
#define SONNET_VOIKKOCLIENT_H

#include "client_p.h"

class VoikkoClient : public Sonnet::Client
{
    Q_OBJECT
    Q_INTERFACES(Sonnet::Client)
    Q_PLUGIN_METADATA(IID "org.kde.Sonnet.VoikkoClient")

public:
    explicit VoikkoClient(QObject *parent = nullptr);
    ~VoikkoClient();

    int reliability() const override;

    Sonnet::SpellerPlugin *createSpeller(const QString &language) override;

    QStringList languages() const override;

    QString name() const override;

private:
    QStringList m_supportedLanguages;
};

#endif //SONNET_VOIKKOCLIENT_H
