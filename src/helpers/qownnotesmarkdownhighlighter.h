/*
 * Copyright (c) 2014-2025 Patrizio Bekerle -- <patrizio@bekerle.com>
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
 * QPlainTextEdit Markdown highlighter
 */

#pragma once

#include <entities/note.h>
#include <libraries/qmarkdowntextedit/markdownhighlighter.h>

QT_BEGIN_NAMESPACE
class QTextDocument;

QT_END_NAMESPACE

namespace Sonnet {
class WordTokenizer;
class LanguageFilter;
}    // namespace Sonnet
class QOwnSpellChecker;

class QOwnNotesMarkdownHighlighter : public MarkdownHighlighter {
    Q_OBJECT

   public:
    QOwnNotesMarkdownHighlighter(
        QTextDocument *parent = nullptr,
        HighlightingOptions highlightingOptions = HighlightingOption::None);

    void updateCurrentNote(Note *note);

    struct ScriptingHighlightingRule {
        explicit ScriptingHighlightingRule(const HighlighterState state_) : state(state_) {}
        ScriptingHighlightingRule() = default;

        QRegularExpression pattern;
        QString shouldContain;
        HighlighterState state = NoState;
        uint8_t capturingGroup = 0;
        uint8_t maskedGroup = 0;
    };

   protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

   private:
    void highlightBrokenNotesLink(const QString &text);

    // Set the format of a word as misspelled i.e., red wavy underline
    void setMisspelled(const int start, const int count);
    void highlightSpellChecking(const QString &text);

    void updateCachedRegexes(const QString &newExt);

   private:
    Note *_currentNote = nullptr;

    QString _defaultNoteFileExt;
    QRegularExpression _regexTagStyleLink;
    QRegularExpression _regexBracketLink;
    void highlightScriptingRules(const QVector<ScriptingHighlightingRule> &rules,
                                 const QString &text);
};
