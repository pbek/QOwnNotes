/*  This file is part of the KDE libraries
    Copyright (c) 2006 Jacob R Rideout <kde@jacobrideout.net>
    Copyright (c) 2006 Martin Sandsmark <martin.sandsmark@kde.org>

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

#include <QHash>
#include <QString>
#include <QTextBoundaryFinder>

#include "textbreaks_p.h"

namespace Sonnet {
class TextBreaksPrivate {
   public:
    TextBreaksPrivate() {}

    QString text;
};

TextBreaks::TextBreaks(const QString &text) : d(new TextBreaksPrivate()) { setText(text); }

TextBreaks::~TextBreaks() { delete d; }

QString TextBreaks::text() const { return d->text; }

void TextBreaks::setText(const QString &text) { d->text = text; }

TextBreaks::Positions TextBreaks::wordBreaks(const QString &text) {
    Positions breaks;

    if (text.isEmpty()) {
        return breaks;
    }

    QTextBoundaryFinder boundaryFinder(QTextBoundaryFinder::Word, text);
    auto len = text.length();
    while (boundaryFinder.position() < len) {
        if (!(boundaryFinder.boundaryReasons().testFlag(QTextBoundaryFinder::StartOfItem))) {
            if (boundaryFinder.toNextBoundary() == -1) {
                break;
            }
            continue;
        }

        Position pos;
        pos.start = boundaryFinder.position();
        int end = boundaryFinder.toNextBoundary();
        if (end == -1) {
            break;
        }
        pos.length = end - pos.start;
        if (pos.length < 1) {
            continue;
        }
        breaks.append(pos);

        //        if (boundaryFinder.toNextBoundary() == -1) {
        //            break;
        //        }
    }
    return breaks;
}

TextBreaks::Positions TextBreaks::sentenceBreaks(const QString &text) {
    Positions breaks;

    if (text.isEmpty()) {
        return breaks;
    }

    QTextBoundaryFinder boundaryFinder(QTextBoundaryFinder::Sentence, text);
    auto len = text.length();
    while (boundaryFinder.position() < len) {
        Position pos;
        pos.start = boundaryFinder.position();
        int end = boundaryFinder.toNextBoundary();
        if (end == -1) {
            break;
        }
        pos.length = end - pos.start;
        if (pos.length < 1) {
            continue;
        }
        breaks.append(pos);
    }
    return breaks;
}

TextBreaks::Positions TextBreaks::wordBreaks() const { return wordBreaks(d->text); }

TextBreaks::Positions TextBreaks::sentenceBreaks() const { return sentenceBreaks(d->text); }
}    // namespace Sonnet
