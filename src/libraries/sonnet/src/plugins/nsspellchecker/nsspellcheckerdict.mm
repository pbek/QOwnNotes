/**
 * nsspellcheckerdict.mm
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
#include "nsspellcheckerdict.h"
#include "nsspellcheckerdebug.h"

#import <AppKit/NSSpellChecker.h>
#import <Foundation/NSArray.h>
#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSString.h>

using namespace Sonnet;

NSSpellCheckerDict::NSSpellCheckerDict(const QString &lang)
    : SpellerPlugin(lang)
{

	m_langCode = [[NSString alloc] initWithCharacters:reinterpret_cast<const unichar*>(lang.unicode()) length:lang.length()];
    
    m_tag = [NSSpellChecker uniqueSpellDocumentTag];
    
    //NSSpellChecker *checker = [NSSpellChecker sharedSpellChecker];
    if ([[NSSpellChecker sharedSpellChecker] setLanguage:m_langCode]) {
        qCDebug(SONNET_NSSPELLCHECKER) << "Loading dictionary for" << lang;
        [[NSSpellChecker sharedSpellChecker] updatePanels];
    } else {
        qCWarning(SONNET_NSSPELLCHECKER) << "Loading dictionary for unsupported language" << lang;
    }
}

NSSpellCheckerDict::~NSSpellCheckerDict()
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	[m_langCode release];
	
	[[NSSpellChecker sharedSpellChecker] closeSpellDocumentWithTag:m_tag];

	[pool release];
    //[m_langCode release];
}

bool NSSpellCheckerDict::isCorrect(const QString &word) const
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	NSString *nsWord = [NSString stringWithCharacters:reinterpret_cast<const unichar*>(word.unicode()) length:word.length()];
	
    //NSString *nsWord = word.toNSString();
    //NSSpellChecker *checker = [NSSpellChecker sharedSpellChecker];
    
    NSRange range = [[NSSpellChecker sharedSpellChecker] checkSpellingOfString:nsWord
                    startingAt:0
                    language:m_langCode
                    wrap:NO
                    inSpellDocumentWithTag:m_tag
                    wordCount:nullptr];
                    
//disable this for now
/*
    if (range.length == 0) {
        // Check if the user configured a replacement text for this string. Sadly
        // we can only signal an error if that's the case, Sonnet has no other way
        // to take such substitutions into account.
        if (NSDictionary *replacements = [checker userReplacementsDictionary]) {
            return [replacements objectForKey:nsWord] == nil;
        } else {
            return true;
        }
    }
*/
    if (range.length == 0) {
        [pool release];
        return true;
    }
    
    [pool release];
    return false;
}

QStringList NSSpellCheckerDict::suggest(const QString &word) const
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

    NSString *nsWord = [NSString stringWithCharacters:reinterpret_cast<const unichar*>(word.unicode()) length:word.length()];
    //NSString *nsWord = word.toNSString();
    //NSSpellChecker *checker = [NSSpellChecker sharedSpellChecker];
    NSRange range;
    range.location = 0;
    range.length = word.length();
    
    NSArray *suggestions = [[NSSpellChecker sharedSpellChecker] guessesForWordRange:range
                        inString:nsWord
                        language:m_langCode
                        inSpellDocumentWithTag:m_tag];
        
    QStringList lst;

    if (suggestions) {
        for (unsigned int i = 0; i < [suggestions count]; ++i) {
            nsWord = [suggestions objectAtIndex: i];
            lst += QString::fromUtf8([nsWord UTF8String]);
        }
    }
    
    [pool release];
    return lst;
}

bool NSSpellCheckerDict::storeReplacement(const QString &bad,
                                    const QString &good)
{
    qCDebug(SONNET_NSSPELLCHECKER) << "Not storing replacement" << good << "for" << bad;
    return false;
}

bool NSSpellCheckerDict::addToPersonal(const QString &word)
{
    NSString *nsWord = [NSString stringWithCharacters:reinterpret_cast<const unichar*>(word.unicode()) length:word.length()];
    //NSSpellChecker *checker = [NSSpellChecker sharedSpellChecker];
    if (![[NSSpellChecker sharedSpellChecker] hasLearnedWord:nsWord]) {
        [[NSSpellChecker sharedSpellChecker] learnWord:nsWord];
        [[NSSpellChecker sharedSpellChecker] updatePanels];
    }
    return true;
}

bool NSSpellCheckerDict::addToSession(const QString &word)
{
    qCDebug(SONNET_NSSPELLCHECKER) << "Not storing" << word << "in the session dictionary";
    return false;
}
