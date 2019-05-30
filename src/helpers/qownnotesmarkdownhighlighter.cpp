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

#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <entities/note.h>
#include "qownnotesmarkdownhighlighter.h"


/**
 * Markdown syntax highlighting
 *
 * markdown syntax:
 * http://daringfireball.net/projects/markdown/syntax
 *
 * @param parent
 * @return
 */
QOwnNotesMarkdownHighlighter::QOwnNotesMarkdownHighlighter(
        QTextDocument *parent, HighlightingOptions highlightingOptions)
        : MarkdownHighlighter(parent, highlightingOptions) {
    Q_UNUSED(parent);
    Q_UNUSED(highlightingOptions);
}

/**
 * Does the markdown highlighting
 * We need to override this method so our highlightMarkdown gets called
 *
 * @param text
 */
void QOwnNotesMarkdownHighlighter::highlightBlock(const QString &text) {
    setCurrentBlockState(HighlighterState::NoState);
    currentBlock().setUserState(HighlighterState::NoState);
    highlightMarkdown(text);
    _highlightingFinished = true;
}

void QOwnNotesMarkdownHighlighter::highlightMarkdown(const QString& text) {
    if (!text.isEmpty()) {
        highlightAdditionalRules(_highlightingRulesPre, text);

        // needs to be called after the horizontal ruler highlighting
        highlightHeadline(text);

        highlightAdditionalRules(_highlightingRulesAfter, text);

        // highlight broken note links
        highlightBrokenNotesLink(text);
    }

    highlightCommentBlock(text);
    highlightCodeBlock(text);
}

/**
 * Highlight broken note links
 *
 * @param text
 */
void QOwnNotesMarkdownHighlighter::highlightBrokenNotesLink(const QString& text) {
    QRegularExpression regex(R"(note:\/\/[^\s\)>]+)");
    QRegularExpressionMatch match = regex.match(text);

    if (!match.hasMatch()) {
        return;
    }

    QString noteLink = match.captured(0);

    // try to fetch a note from the url string
    Note note = Note::fetchByUrlString(noteLink);

    // if the note exists we don't need to do anything
    if (note.isFetched()) {
        return;
    }

    auto state = HighlighterState(HighlighterState::BrokenLink);

    setFormat(match.capturedStart(0), match.capturedLength(0), _formats[state]);
}
