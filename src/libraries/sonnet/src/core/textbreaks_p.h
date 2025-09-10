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

#ifndef TEXTBREAKS_H
#define TEXTBREAKS_H

#include <QVector>

class QString;

namespace Sonnet {
class TextBreaksPrivate;

/**
 * @short TextBreaks determines the barriers between linguistic structures in any given text.
 *
 * TextBreaks is a class that determines the boundaries between graphemes
 * (characters as per the unicode definition,) words and sentences. The
 * default implementation conforms to Unicode Standard Annex #29 http://unicode.org/reports/tr29/.
 * You can subclass TextBreaks to create the correct behaviour for languages that require it.
 *
 * @author Jacob Rideout <kde@jacobrideout.net>
 * @since 4.3
 */
class TextBreaks {
   public:
    struct Position {
        int start, length;
    };

    /**
     * This structure abstracts the positions of breaks in the test. As per the
     * unicode annex, both the start and end of the text are returned.
     */
    typedef QVector<Position> Positions;

    /** Constructor
     * Creates a new TextBreaks instance. If @p text is specified,
     * it sets the text to be checked.
     * @param text the text that is to be checked
     */
    explicit TextBreaks(const QString &text = QString());

    /** Virtual Destructor
     */
    virtual ~TextBreaks();

    /**
     * Returns the text to be checked
     * @return text
     */
    QString text() const;

    /**
     * Sets the text to @p text
     * @param text to be set
     * @return true if the word is misspelled. false otherwise
     */
    void setText(const QString &text);

    /**
     * Return the Positions of each word for the given  @p text.
     * @param text to be checked
     * @return positions of breaks
     */
    static Positions wordBreaks(const QString &text);

    /**
     * Return the Positions of each sentence for the given  @p text.
     * @param text to be checked
     * @return positions of breaks
     */
    static Positions sentenceBreaks(const QString &text);

    /**
     * Return the Positions of each word for the text previously set.
     * @return positions of breaks
     */
    virtual Positions wordBreaks() const;

    /**
     * Return the Positions of each sentence for the text previously set.
     * @return positions of breaks
     */
    virtual Positions sentenceBreaks() const;

   private:
    TextBreaksPrivate *const d;
};
}    // namespace Sonnet
#endif
