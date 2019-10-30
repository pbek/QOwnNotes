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

#include <QApplication>
#include <QObject>
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
        QTextDocument *parent,
        QOwnSpellChecker *_spellchecker,
        HighlightingOptions highlightingOptions)
        : MarkdownHighlighter(parent, highlightingOptions) {
    Q_UNUSED(parent)
    Q_UNUSED(highlightingOptions)

    spellchecker = _spellchecker;
    languageFilter = new Sonnet::LanguageFilter(new Sonnet::SentenceTokenizer());
    wordTokenizer = new Sonnet::WordTokenizer();
    inlineCode = 0; // for `
    codeBlock = 0; // for ```
}

QOwnNotesMarkdownHighlighter::~QOwnNotesMarkdownHighlighter()
{
    delete languageFilter;
    delete wordTokenizer;
}

void QOwnNotesMarkdownHighlighter::updateCurrentNote() {
    _currentNote = Note::fetch(qApp->property("currentNoteId").toInt());
}

/**
 * Does the markdown highlighting
 * We need to override this method so our highlightMarkdown gets called
 *
 * @param text
 */
void QOwnNotesMarkdownHighlighter::highlightBlock(const QString &text) {
    updateCurrentNote();
    setCurrentBlockState(HighlighterState::NoState);
    currentBlock().setUserState(HighlighterState::NoState);
    if (spellchecker->isActive()) {
        highlightSpellChecking(text);
    }
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
    highlightFrontmatterBlock(text);
}

/**
 * Highlight broken note links
 *
 * @param text
 */
void QOwnNotesMarkdownHighlighter::highlightBrokenNotesLink(const QString& text) {
    QRegularExpression regex(R"(note:\/\/[^\s\)>]+)");
    QRegularExpressionMatch match = regex.match(text);

    if (match.hasMatch()) { // check legacy note:// links
        QString noteLink = match.captured(0);

        // try to fetch a note from the url string
        Note note = Note::fetchByUrlString(noteLink);

        // if the note exists we don't need to do anything
        if (note.isFetched()) {
            return;
        }
    } else { // check <note file.md> links
        regex = QRegularExpression("<([^\\s`][^`]*?\\.[^`]*?[^\\s`]\\.md)>");
        match = regex.match(text);

        if (match.hasMatch()) {
            QString fileName = match.captured(1);
            Note note = _currentNote.fetchByRelativeFileName(fileName);

            // if the note exists we don't need to do anything
            if (note.isFetched()) {
                return;
            }
        } else { // check [note](note file.md) links
            regex = QRegularExpression(R"(\[[^\[\]]+\]\((\S+\.md|.+?\.md)\)\B)");
            match = regex.match(text);

            if (match.hasMatch()) {
                QString fileName = match.captured(1);
                Note note = _currentNote.fetchByRelativeFileName(fileName);

                // if the note exists we don't need to do anything
                if (note.isFetched()) {
                    return;
                }
            } else {
                // no note link was found
                return;
            }
        }
    }

    auto state = HighlighterState(HighlighterState::BrokenLink);

    setFormat(match.capturedStart(0), match.capturedLength(0), _formats[state]);
}


/*
 * Spellchecker lives here
*/

void QOwnNotesMarkdownHighlighter::setMisspelled(const int start, const int count) {
    //append to the already existing text format.
    //creating a new format will destroy pre-existing format
    QTextCharFormat format = QSyntaxHighlighter::format(start+1);
    format.setFontUnderline(true);
    format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
    format.setUnderlineColor(Qt::red);
    setFormat(start, count, format);
}

void QOwnNotesMarkdownHighlighter::unsetMisspelled(int start, int count) {
    //keep the existing format
    QTextCharFormat format = QSyntaxHighlighter::format(start+1);

    //turn off the spell-check underline if it is turned on.
    //Note: Don't use - format.fontUnderline() - to check whether
    //font underlining is on. It is often off but the lines are appearing
    //so we just check whether it has spellcheck underline.
    if(format.underlineStyle() == QTextCharFormat::SpellCheckUnderline) {
        format.setFontUnderline(false);
    }
    setFormat(start, count, format);
}

static bool hasNotEmptyText(const QString &text)
{
    for (int i = 0; i < text.length(); ++i) {
        if (!text.at(i).isSpace()) {
            return true;
        }
    }
    return false;
}

/**
 * Uses the QOwnSpellChecker to check words for correctness and underlines the incorrect words
 *
 * @param text
 */
void QOwnNotesMarkdownHighlighter::highlightSpellChecking(const QString &text) {
    if (!spellchecker->isValid()) {
        qDebug () << "[Sonnet]Spellchecker invalid!";
        return;
    }
    //headline || subheadline
    if (text.contains(QStringLiteral("====")) || text.contains(QStringLiteral("----")) ) {
        return;
    }
    //if it's a code block, don't spell check
    if (text.contains(QStringLiteral("```"))) {
        ++codeBlock;
        return;
    }
    if (codeBlock % 2 != 0) {
        return;
    }
    //if it's empty line or just spaces, don't spell check
    if (!hasNotEmptyText(text)) {
        return;
    }

    languageFilter->setBuffer(text);

    LanguageCache *languageCache = dynamic_cast<LanguageCache*>(currentBlockUserData());
    if (!languageCache) {
        languageCache = new LanguageCache;
        setCurrentBlockUserData(languageCache);
    }

    const bool autodetectLanguage = spellchecker->isAutoDetectOn() ?
               spellchecker->testAttribute(Sonnet::Speller::AutoDetectLanguage) : false;
    while (languageFilter->hasNext()) {
        QStringRef sentence = languageFilter->next();
        if (autodetectLanguage) {
            QString lang;
            QPair<int, int> spos = QPair<int, int>(sentence.position(), sentence.length());
            // try cache first
            if (languageCache->languages.contains(spos)) {
                lang = languageCache->languages.value(spos);
            } else {
                lang = languageFilter->language();
                if (!languageFilter->isSpellcheckable()) {
                    lang.clear();
                }
                languageCache->languages[spos] = lang;
            }
            if (lang.isEmpty()) {
                continue;
            }
            spellchecker->setCurrentLanguage(lang);
        }

        wordTokenizer->setBuffer(sentence.toString());
        int offset = sentence.position();
        while (wordTokenizer->hasNext()) {
            QStringRef word = wordTokenizer->next();

            //in case it's not a word, like an email or a number
            if (!wordTokenizer->isSpellcheckable()) {
                continue;
            }
            //if the word is misspelled
            if (spellchecker->isWordMisspelled(word.toString())) {
                setMisspelled(word.position()+offset, word.length());
            //else we do nothing and move on to the next word
            } else {
                //unsetMisspelled(word.position()+offset, word.length());
            }
        }
    }

    setCurrentBlockState(0);
}

