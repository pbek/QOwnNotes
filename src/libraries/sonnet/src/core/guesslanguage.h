/*  This file is part of the KDE libraries
    Copyright (c) 2006 Jacob R Rideout <kde@jacobrideout.net>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef GUESSLANGUAGE_H
#define GUESSLANGUAGE_H

#include <QString>
#include <QStringList>

namespace Sonnet {
// Amount of trigrams in each file
static const int MAXGRAMS = 300;

class GuessLanguagePrivate;

/**
 * @short GuessLanguage determines the language of a given text.
 *
 * GuessLanguage can determine the differnce between ~75 languages for a given string. It is
 * based off a perl script originally written by Maciej Ceglowski <maciej@ceglowski.com>
 * called Languid. His script used a 2 part huristic to determine language. First the text
 * is is checked for the scripts it contains, then for each set of languages useing those
 * scripts a n-gram frequency model of a given language is compared to a model of the text.
 * The most similar language model is assumed to be the language. If no language is found
 * an empty string is returned.
 *
 *
 * @author Jacob Rideout <kde@jacobrideout.net>
 * @since 4.3
 */
class GuessLanguage {
   public:
    /** Constructor
     * Creates a new GuessLanguage instance. If @p text is specified,
     * it sets the text to be checked.
     * @param text the text that is to be checked
     */
    GuessLanguage();

    /** Destructor
     */
    ~GuessLanguage();

    GuessLanguage(const GuessLanguage &) = delete;
    GuessLanguage &operator=(const GuessLanguage &) = delete;

    /**
     * Sets limits to number of languages returned by identify(). The confidence for each language
     * is computed as difference between this and next language on the list normalized to 0-1 range.
     * Reasonable value to get fairly sure result is 0.1 . Default is returning best guess without
     * caring about confidence - exactly as after call to setLimits(1,0).
     * @param maxItems The list returned by identify() will never have more than maxItems item
     * @param minConfidence The list will have only enough items for their summary confidence equal
     * or exceed minConfidence.
     */
    void setLimits(int maxItems, double minConfidence);

    /**
     * Returns the 2 digit ISO 639-1 code for the language of the currently
     * set text and. Three digits are returned only in the case where a 2 digit
     * code does not exist. If @p text isn't empty, set the text to checked.
     * @param text to be identified
     * @return list of the presumed languages of the text, sorted by decreasing confidence. Empty
     * list means it is impossible to determine language with confidence required by setLimits
     */
    QString identify(const QString &text, const QStringList &suggestions = QStringList()) const;

   private:
    GuessLanguagePrivate *const d;
};
}    // namespace Sonnet

#endif
