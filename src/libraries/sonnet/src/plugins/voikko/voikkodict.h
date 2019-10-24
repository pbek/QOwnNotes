/**
 * voikkodict.h
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

#ifndef SONNET_VOIKKODICT_H
#define SONNET_VOIKKODICT_H

#include "spellerplugin_p.h"
#include <libvoikko/voikko.h>

#include <QHash>
#include <QScopedPointer>

class VoikkoClient;
class VoikkoDictPrivate;

class VoikkoDict : public Sonnet::SpellerPlugin
{
public:
    /**
     * Declare VoikkoClient as friend so we can use the protected constructor.
     */
    friend class VoikkoClient;

    ~VoikkoDict();

    bool isCorrect(const QString &word) const override;
    QStringList suggest(const QString &word) const override;

    bool storeReplacement(const QString &bad, const QString &good) override;
    bool addToPersonal(const QString &word) override;
    bool addToSession(const QString &word) override;

    /**
     * @returns true if initializing Voikko backend failed.
     */
    bool initFailed() const Q_DECL_NOEXCEPT;

protected:
    /**
     * Constructor is protected so that only spellers created
     * and validated through VoikkoClient can be used.
     */
    explicit VoikkoDict(const QString &language) Q_DECL_NOEXCEPT;

private:
    QScopedPointer<VoikkoDictPrivate> d;
};

#endif //SONNET_VOIKKODICT_H
