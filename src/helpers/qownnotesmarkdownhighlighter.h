/*
 * Copyright (c) 2014-2020 Patrizio Bekerle -- <patrizio@bekerle.com>
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

QT_BEGIN_NAMESPACE
class QTextDocument;

QT_END_NAMESPACE

namespace Sonnet {
class WordTokenizer;
class LanguageFilter;
}
class QOwnSpellChecker;
class Note;

class QOwnNotesMarkdownHighlighter : public MarkdownHighlighter
{
Q_OBJECT

public:
    QOwnNotesMarkdownHighlighter(QTextDocument *parent = 0,
                                 HighlightingOptions highlightingOptions =
                                 HighlightingOption::None);
    ~QOwnNotesMarkdownHighlighter() Q_DECL_OVERRIDE;

    void updateCurrentNote(Note *note);
    void setCommentHighlighting(bool);
    void setCodeHighlighting(bool);
    void setSpellChecker(QOwnSpellChecker*);

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
    Sonnet::WordTokenizer *wordTokenizer;
    Sonnet::LanguageFilter *languageFilter;
    QOwnSpellChecker *spellchecker;
    Note *_currentNote;
    bool commentHighlightingOn;
    bool codeHighlightingOn;
};
