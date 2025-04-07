/*  This file is part of the KDE libraries

    Copyright (c) 2004 Zack Rusin <zack@kde.org>
    Copyright (c) 2006 Jacob R Rideout <kde@jacobrideout.net>
    Copyright (c) 2009 Jakub Stachowski <qbast@go2.pl>

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

#include <QString>
#include <QVector>
// #include <QDebug>

#include "textbreaks_p.h"
#include "tokenizer_p.h"

namespace Sonnet {
class BreakTokenizerPrivate {
   public:
    enum Type { Words, Sentences };

    BreakTokenizerPrivate(Type s)
        : breakFinder(new TextBreaks),
          type(s),
          itemPosition(-1),
          cacheValid(false),
          inAddress(false),
          ignoreUppercase(false) {}

    ~BreakTokenizerPrivate() { delete breakFinder; }

    TextBreaks::Positions breaks() const;
    void invalidate();
    void shiftBreaks(int from, int offset);
    void replace(int pos, int len, const QString &newWord);
    void reset() { itemPosition = -1; }

    TextBreaks *breakFinder;
    Token last;
    QString buffer;

    Type type;
    int itemPosition;
    mutable bool cacheValid;
    bool inAddress;
    bool ignoreUppercase;

    bool hasNext() const;
    Token next();
    void setBuffer(const QString &b) {
        invalidate();
        buffer = b;
    }

   private:
    void regenerateCache() const;
    mutable TextBreaks::Positions cachedBreaks;
};

void BreakTokenizerPrivate::invalidate() {
    cacheValid = false;
    itemPosition = -1;
}

bool BreakTokenizerPrivate::hasNext() const {
    if (itemPosition >= (breaks().size() - 1)) {
        return false;
    }

    return true;
}

TextBreaks::Positions BreakTokenizerPrivate::breaks() const {
    if (!cacheValid) {
        regenerateCache();
    }

    return cachedBreaks;
}

void BreakTokenizerPrivate::shiftBreaks(int from, int offset) {
    auto size = cachedBreaks.size();
    for (int i = 0; i < size; i++) {
        if (cachedBreaks[i].start > from) {
            cachedBreaks[i].start = cachedBreaks[i].start - offset;
        }
    }
}

void BreakTokenizerPrivate::regenerateCache() const {
    if (!breakFinder || buffer.isEmpty()) {
        cachedBreaks = TextBreaks::Positions();
    }

    if (breakFinder) {
        breakFinder->setText(buffer);

        if (type == Sentences) {
            cachedBreaks = breakFinder->sentenceBreaks();
        } else if (type == Words) {
            cachedBreaks = breakFinder->wordBreaks();
        }
    }

    cacheValid = true;
}

Token BreakTokenizerPrivate::next() {
    Token block;

    if (!hasNext()) {
        last = block;
        return block;
    }

    itemPosition++;
    const auto textBreak = this->breaks().at(itemPosition);
    int st = textBreak.start;
    int len = textBreak.length;

    last.token = buffer.mid(st, len);
    last.positionInBuffer = st;

    return last;
}

void BreakTokenizerPrivate::replace(int pos, int len, const QString &newWord) {
    buffer.replace(pos, len, newWord);
    int offset = len - newWord.length();
    if (cacheValid) {
        shiftBreaks(pos, offset);
    }
}

/*-----------------------------------------------------------*/

WordTokenizer::WordTokenizer(const QString &buffer)
    : d(new BreakTokenizerPrivate(BreakTokenizerPrivate::Words)) {
    setBuffer(buffer);
}

WordTokenizer::~WordTokenizer() { delete d; }

bool WordTokenizer::hasNext() const { return d->hasNext(); }

void WordTokenizer::setBuffer(const QString &buffer) { d->setBuffer(buffer); }

Token WordTokenizer::next() {
    Token n = d->next();

    // end of address of url?
    if (d->inAddress && n.position() > 0 && d->buffer[n.position() - 1].isSpace()) {
        d->inAddress = false;
    }

    // check if this word starts an email address of url
    if (!d->inAddress || hasNext()) {
        int pos = n.position() + n.length();
        if (pos < d->buffer.size() && d->buffer.at(pos) == QLatin1Char('@')) {
            d->inAddress = true;
        }
        if (pos < d->buffer.size() && d->buffer.at(pos) == QLatin1Char(':') &&
            pos + 1 < d->buffer.size() && d->buffer.at(pos + 1) == QLatin1Char('/') &&
            d->buffer[pos + 2] == QLatin1Char('/')) {
            d->inAddress = true;
        }
    }
    return n;
}

QString WordTokenizer::buffer() const { return d->buffer; }

bool WordTokenizer::isUppercase(const QString &word) const {
    auto len = word.length();
    for (int i = 0; i < len; ++i) {
        if (word.at(i).isLetter() && !word.at(i).isUpper()) {
            return false;
        }
    }
    return true;
}

void WordTokenizer::setIgnoreUppercase(bool val) { d->ignoreUppercase = val; }

int WordTokenizer::count() const { return d->breaks().size(); }

void WordTokenizer::reset() { d->reset(); }

void WordTokenizer::replace(int pos, int len, const QString &newWord) {
    d->replace(pos, len, newWord);
}

bool WordTokenizer::isSpellcheckable() const {
    if (d->last.token.isNull() || d->last.token.isEmpty()) {
        return false;
    }
    if (!d->last.token.at(0).isLetter()) {
        return false;
    }
    if (d->inAddress) {
        return false;
    }
    if (d->ignoreUppercase && isUppercase(d->last.token)) {
        return false;
    }
    return true;
}

/* --------------------------------------------------------------------*/

SentenceTokenizer::SentenceTokenizer(const QString &buffer)
    : d(new BreakTokenizerPrivate(BreakTokenizerPrivate::Sentences)) {
    setBuffer(buffer);
}

SentenceTokenizer::~SentenceTokenizer() { delete d; }

bool SentenceTokenizer::hasNext() const { return d->hasNext(); }

void SentenceTokenizer::setBuffer(const QString &buffer) { d->setBuffer(buffer); }

Token SentenceTokenizer::next() { return d->next(); }

QString SentenceTokenizer::buffer() const { return d->buffer; }

void SentenceTokenizer::replace(int pos, int len, const QString &newWord) {
    d->replace(pos, len, newWord);
}
}    // namespace Sonnet
