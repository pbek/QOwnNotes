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

#import <AppKit/AppKit.h>

using namespace Sonnet;

NSSpellCheckerDict::NSSpellCheckerDict(const QString &lang)
    : SpellerPlugin(lang)
    , m_langCode([lang.toNSString() retain])
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
    NSSpellChecker *checker = [NSSpellChecker sharedSpellChecker];
    NSArray *suggestions = [checker guessesForWordRange:NSMakeRange(0, word.length())
        inString:nsWord language:m_langCode inSpellDocumentWithTag:0];
        
    QStringList lst;
    NSDictionary *replacements = [checker userReplacementsDictionary];
    QString replacement;
    if ([replacements objectForKey:nsWord]) {
        // return the replacement text from the userReplacementsDictionary first.
        replacement = QString::fromNSString([replacements valueForKey:nsWord]);
        lst << replacement;
    }
    for (NSString *suggestion in suggestions) {
        // the replacement text from the userReplacementsDictionary will be in
        // the suggestions list; don't add it again.
        QString str = QString::fromNSString(suggestion);
        if (str != replacement) {
            lst << str;
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
    NSString *nsWord = word.toNSString();
    NSSpellChecker *checker = [NSSpellChecker sharedSpellChecker];
    if (![checker hasLearnedWord:nsWord]) {
        [checker learnWord:nsWord];
        [checker updatePanels];
    }
    return true;
}

bool NSSpellCheckerDict::addToSession(const QString &word)
{
    qCDebug(SONNET_NSSPELLCHECKER) << "Not storing" << word << "in the session dictionary";
    return false;
}
