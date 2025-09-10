/*  This file is part of the KDE libraries

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

#ifndef ABSTRACTTOKENIZER_H
#define ABSTRACTTOKENIZER_H

#include <QString>

#include "sonnetcore_export.h"

namespace Sonnet {

struct Token {
    QString token;
    int positionInBuffer = -1;

    QString toString() const { return token; }

    /**
     * @brief length of this token
     */
    int length() const { return token.length(); }

    /**
     * @brief position in buffer of which the @ref token is a view
     */
    int position() const { return positionInBuffer; }
};

/**
 * @short AbstractTokenizer breaks text into smaller pieces - words, sentences, paragraphs.
 *
 * AbstractTokenizer is an abstract class that must be subclassed to be used. It provides API
 * modelled after Java-style iterators. During tokenization buffer can be modified using provided
 * replace() method.
 *
 * @since 4.3
 */
class AbstractTokenizer {
   public:
    virtual ~AbstractTokenizer() {}

    /**
     * Sets text to tokenize. It also resets tokenizer state.
     */
    virtual void setBuffer(const QString &buffer = QString()) = 0;
    /**
     * Returns true if there is another token avaiable.
     * @return true if another token is available, false if not.
     */
    virtual bool hasNext() const = 0;

    /**
     * Returns next token or null QString if there is none
     */
    virtual Token next() = 0;

    /** Returns content of currently tokenized buffer*/
    virtual QString buffer() const = 0;

    /**
     * Replace part of text in current buffer. Always use this function instead of directly
     * changing data in underlying buffer or tokenizer's internal state may become inconsistent.
     */
    virtual void replace(int position, int len, const QString &newWord) = 0;
};

class BreakTokenizerPrivate;

/**
@short WordTokenizer splits supplied buffer into individual words.

WordTokenizer splits buffer into words according to rules from Unicode standard 5.1.
If purpose is to check spelling, use isSpellcheckable() to determine if current word should be
checked or ignored.

Usage example:

@code
WordTokenizer t(buffer);
Speller sp;
while (t.hasNext()) {
    QStringRef word=t.next();
    if (!t.isSpellcheckable()) continue;
    qDebug() << word.toString() << " " << sp.isCorrect(word.toString());
}
@endcode

This example checks spelling of given buffer
 * @since 4.3
*/
class WordTokenizer : public AbstractTokenizer {
   public:
    /**
     * Constructor for word tokenizer
     * @param buffer
     */
    WordTokenizer(const QString &buffer = QString());
    ~WordTokenizer() override;

    void setBuffer(const QString &buffer) override;
    bool hasNext() const override;
    Token next() override;
    QString buffer() const override;
    void replace(int position, int len, const QString &newWord) override;

    /** Returns true if this word should be spell checked. This ignores email addresses, URLs and
     * other things according to configuration */
    bool isSpellcheckable() const;

    /** If ignore uppercase is true, then any word containing only uppercase letters will be
     * considered unsuitable for spell check */
    void setIgnoreUppercase(bool val);
    int count() const;
    void reset();

   private:
    bool isUppercase(const QString &word) const;
    BreakTokenizerPrivate *const d;
};

/**
@short SentenceTokenizer splits supplied buffer into individual sentences.

SentenceTokenizer splits buffer into sentences according to rules from Unicode standard 5.1.
 * @since 4.3
*/
class SentenceTokenizer : public AbstractTokenizer {
   public:
    SentenceTokenizer(const QString &buffer = QString());
    ~SentenceTokenizer() override;
    void setBuffer(const QString &buffer) override;
    bool hasNext() const override;
    Token next() override;
    QString buffer() const override;
    void replace(int position, int len, const QString &newWord) override;

   private:
    BreakTokenizerPrivate *const d;
};
}    // namespace Sonnet
#endif
