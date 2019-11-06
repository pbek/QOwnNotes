/**
 * nsspellcheckerclient.mm
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
#include "nsspellcheckerclient.h"
#include "nsspellcheckerdict.h"

#import <AppKit/AppKit.h>

using namespace Sonnet;

NSSpellCheckerClient::NSSpellCheckerClient(QObject *parent)
    : Client(parent)
{
}

NSSpellCheckerClient::~NSSpellCheckerClient()
{
}

int NSSpellCheckerClient::reliability() const
{
    return qEnvironmentVariableIsSet("SONNET_PREFER_NSSPELLCHECKER") ? 9999 : 30;
}

SpellerPlugin *NSSpellCheckerClient::createSpeller(const QString &language)
{
    return new NSSpellCheckerDict(language);
}

QStringList NSSpellCheckerClient::languages() const
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

    QStringList lst;
    NSArray* availableLanguages = [[NSSpellChecker sharedSpellChecker]
                                   availableLanguages];

//    for (NSString* lang_code in availableLanguages) {
//        lst.append(QString::fromNSString(lang_code));
//    }

    if (availableLanguages) {
        for (unsigned int i = 0; i < [availableLanguages count]; ++i) {
            lst += QString::fromUtf8([[availableLanguages objectAtIndex: i] UTF8String]);
        }
    }

    [pool release];
    return lst;
}

