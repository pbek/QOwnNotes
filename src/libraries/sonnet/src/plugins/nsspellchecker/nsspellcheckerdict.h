/**
 * nsspellcheckerdict.h
 *
 * Copyright (C)  2015  Nick Shaforostoff <shaforostoff@gmail.com>
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
#ifndef KSPELL_NSSPELLDICT_H
#define KSPELL_NSSPELLDICT_H

#import <Foundation/Foundation.h>

#include "spellerplugin_p.h"

class NSSpellCheckerDict : public Sonnet::SpellerPlugin {
   public:
    explicit NSSpellCheckerDict(const QString &lang);
    ~NSSpellCheckerDict();
    virtual bool isCorrect(const QString &word) const;

    virtual QStringList suggest(const QString &word) const;

    virtual bool storeReplacement(const QString &bad, const QString &good);

    virtual bool addToPersonal(const QString &word);
    virtual bool addToSession(const QString &word);

   private:
#ifdef __OBJC__
    NSString *m_langCode;
    NSInteger m_tag;
#else
    void *m_langCode;
#endif
};

#endif
