/*
 * Copyright (c) 2014-2019 Patrizio Bekerle -- http://www.bekerle.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * QPlainTextEdit markdown highlighter
 */


#pragma once

#include <libraries/qmarkdowntextedit/markdownhighlighter.h>
#include <entities/note.h>

#ifdef WITH_SONNET
// needs libkf5sonnet-dev
//#include <KF5/SonnetUi/sonnet/spellcheckdecorator.h>
//#include <KF5/SonnetUi/Sonnet/Dialog>
//#include <KF5/SonnetUi/Sonnet/Highlighter>
#include <KF5/SonnetCore/Sonnet/Speller>
#include <KF5/SonnetUi/Sonnet/Highlighter>
#endif

#include "qownspellchecker.h"
#include "libraries/sonnet/src/core/languagefilter_p.h"
#include "libraries/sonnet/src/core/tokenizer_p.h"

QT_BEGIN_NAMESPACE
class QTextDocument;

QT_END_NAMESPACE

class QOwnNotesMarkdownHighlighter : public MarkdownHighlighter
{
Q_OBJECT

public:
    QOwnNotesMarkdownHighlighter(QTextDocument *parent = 0,
                                 QOwnSpellChecker *spellchecker = new QOwnSpellChecker(),
                                 HighlightingOptions highlightingOptions =
                                 HighlightingOption::None);
    ~QOwnNotesMarkdownHighlighter() Q_DECL_OVERRIDE;

    void updateCurrentNote(Note *_note = nullptr);

protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;
    void highlightMarkdown(const QString& text);
    void highlightBrokenNotesLink(const QString& text);

    //Unset Misspelled formatting
    void unsetMisspelled(int start, int count);
    //Set the format of a word as misspelled i.e., red wavy underline
    void setMisspelled(const int start, const int count);
    void highlightSpellChecking(const QString &text);

private:
    Note _currentNote;
    QOwnSpellChecker *spellchecker;

    Sonnet::LanguageFilter *languageFilter;
    Sonnet::WordTokenizer *wordTokenizer;
    int codeBlock;
};
