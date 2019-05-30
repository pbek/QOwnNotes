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

QT_BEGIN_NAMESPACE
class QTextDocument;

QT_END_NAMESPACE

class QOwnNotesMarkdownHighlighter : public MarkdownHighlighter
{
Q_OBJECT

public:
    QOwnNotesMarkdownHighlighter(QTextDocument *parent = 0,
                                 HighlightingOptions highlightingOptions =
                                 HighlightingOption::None);

protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;
    void highlightMarkdown(const QString& text);
    void highlightBrokenNotesLink(const QString& text);
};
