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

#ifndef LANGUAGEFILTER_H
#define LANGUAGEFILTER_H

#include <QString>

#include "sonnetcore_export.h"
#include "tokenizer_p.h"

namespace Sonnet {
class LanguageFilterPrivate;

/**
@short Deternmines language for fragments of text

This class takes fragments produced by supplied tokenizer and provides additional information:
language used in each fragment and if there is spell and grammar checker suitable for the fragment.

*/
class LanguageFilter : public AbstractTokenizer {
   public:
    /** Creates language filter for given tokenizer. LanguageFilter takes complete ownership of
    given tokenizer. This means that no source's methods should be called anymore.
    */
    LanguageFilter(AbstractTokenizer *source);
    LanguageFilter(const LanguageFilter &other);

    ~LanguageFilter() override;

    /** Language for token last returned by next() */
    QString language() const;

    /** Returns true if there is spellchecker installed for last token's language  */
    bool isSpellcheckable() const;

    /** Returns true if there is grammar checker installed for last token's language */
    //  bool isGrammarCheckable() const;
    void setBuffer(const QString &buffer) override;
    bool hasNext() const override;
    Token next() override;
    QString buffer() const override;
    void replace(int position, int len, const QString &newWord) override;

   private:
    LanguageFilterPrivate *const d;
};
}    // namespace Sonnet
#endif
