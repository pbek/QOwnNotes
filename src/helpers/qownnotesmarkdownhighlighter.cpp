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
        HighlightingOptions highlightingOptions)
        : MarkdownHighlighter(parent, highlightingOptions) {
    Q_UNUSED(parent)
    Q_UNUSED(highlightingOptions)

    spellchecker = nullptr;
    languageFilter = new Sonnet::LanguageFilter(new Sonnet::SentenceTokenizer());
    wordTokenizer = new Sonnet::WordTokenizer();
    codeBlock = 0; // for ```

    commentHighlightingOn = true;
    codeHighlightingOn = true;
}

QOwnNotesMarkdownHighlighter::~QOwnNotesMarkdownHighlighter()
{
    delete languageFilter;
    delete wordTokenizer;
}


void QOwnNotesMarkdownHighlighter::updateCurrentNote(const Note &_note) {
        _currentNote = _note;
}

void QOwnNotesMarkdownHighlighter::setSpellChecker(QOwnSpellChecker *spellChecker)
{
    spellchecker = spellChecker;
}

void QOwnNotesMarkdownHighlighter::setCommentHighlighting(bool state)
{
    if (state == commentHighlightingOn){
        return;
    }
    commentHighlightingOn = state;
}

void QOwnNotesMarkdownHighlighter::setCodeHighlighting(bool state)
{
    if (state == codeHighlightingOn){
        return;
    }
    codeHighlightingOn = state;
}

/**
 * Does the markdown highlighting
 * We need to override this method so our highlightMarkdown gets called
 *
 * @param text
 */
void QOwnNotesMarkdownHighlighter::highlightBlock(const QString &text) {
    //updateCurrentNote();
    setCurrentBlockState(HighlighterState::NoState);
    currentBlock().setUserState(HighlighterState::NoState);

    //do the markdown highlighting before the spellcheck highlighting
    //if we do it afterwards, it overwrites the spellcheck highlighting
    highlightMarkdown(text);

    // skip spell checking empty blocks and blocks with just "spaces"
    // the rest of the highlighting needs to be done e.g. for code blocks with empty lines
    if (spellchecker != nullptr) {
        if (!(text.isEmpty()) && spellchecker->isActive()) {
            highlightSpellChecking(text);
        }
    }

    _highlightingFinished = true;
}

void QOwnNotesMarkdownHighlighter::highlightMarkdown(const QString& text) {
    if (!text.isEmpty()) {

        const QString &next = currentBlock().next().text();
        const bool isHeading =
                text.startsWith(QLatin1String("# ")) || text.startsWith(QLatin1String("## ")) ||
                text.startsWith(QLatin1String("### ")) || text.startsWith(QLatin1String("#### ")) ||
                text.startsWith(QLatin1String("##### ")) || text.startsWith(QLatin1String("###### "));
        const bool isHeadWithUnderline =
                text.startsWith(QLatin1String("===")) || text.startsWith(QLatin1String("---"));
        const bool nextHasUnderLine =
                next.startsWith(QLatin1String("===")) || next.startsWith(QLatin1String("---"));
        const bool isCodeBlock = previousBlockState() == CodeBlock ||
                previousBlockState() >= HighlighterState::CodeCpp ||
                text.startsWith(QLatin1String("```"));

        if (!isCodeBlock) {
            highlightAdditionalRules(_highlightingRulesPre, text);

            // needs to be called after the horizontal ruler highlighting
            if (isHeading || isHeadWithUnderline || nextHasUnderLine) {
                highlightHeadline(text);
            }

            highlightAdditionalRules(_highlightingRulesAfter, text);

            // highlight broken note links
            if (text.contains(QLatin1String("note://")) || text.contains(QLatin1String(".md"))) {
                highlightBrokenNotesLink(text);
            }
        }

    }

    if (commentHighlightingOn) {
        highlightCommentBlock(text);
    }
    if (codeHighlightingOn) {
        if (previousBlockState() == HighlighterState::CodeBlock ||
            previousBlockState() == HighlighterState::CodeBlockEnd ||
            previousBlockState() >= HighlighterState::CodeCpp ||
            text.startsWith(QLatin1String("```"))) {
            highlightCodeBlock(text);
        }
    }

    highlightFrontmatterBlock(text);

}

/**
 * Highlight broken note links
 *
 * @param text
 */
void QOwnNotesMarkdownHighlighter::highlightBrokenNotesLink(const QString& text) {
    QRegularExpression regex(QStringLiteral(R"(note:\/\/[^\s\)>]+)"));
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
        regex = QRegularExpression(QStringLiteral("<([^\\s`][^`]*?\\.[^`]*?[^\\s`]\\.md)>"));
        match = regex.match(text);

        if (match.hasMatch()) {
            QString fileName = match.captured(1);
            Note note = _currentNote.fetchByRelativeFileName(fileName);

            // if the note exists we don't need to do anything
            if (note.isFetched()) {
                return;
            }
        } else { // check [note](note file.md) links
            regex = QRegularExpression(QStringLiteral(R"(\[[^\[\]]+\]\((\S+\.md|.+?\.md)\)\B)"));
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

/**
 * Uses the QOwnSpellChecker to check words for correctness and underlines the incorrect words
 *
 * @param text
 */
void QOwnNotesMarkdownHighlighter::highlightSpellChecking(const QString &text) {
    if (text.length() < 2) {
        return;
    }
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


    //use our own settings, as KDE users might face issues with Autodetection
    const bool autodetectLanguage = spellchecker->isAutoDetectOn();
               //spellchecker->testAttribute(Sonnet::Speller::AutoDetectLanguage) : false;
    LanguageCache *languageCache = nullptr;
    if (autodetectLanguage) {
        languageCache = dynamic_cast<LanguageCache*>(currentBlockUserData());
        if (!languageCache) {
            languageCache = new LanguageCache;
            setCurrentBlockUserData(languageCache);
        }
    }
    languageFilter->setBuffer(text);
    while (languageFilter->hasNext()) {
        const QStringRef sentence = languageFilter->next();
        if (autodetectLanguage) {
            QString lang;
            const QPair<int, int> spos = QPair<int, int>(sentence.position(), sentence.length());
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

            //if the word has _ at the end, word tokenizer misses that, so cut it off
            if (word.endsWith('_')) {
#if QT_VERSION >= 0x050800
                word.chop(1);
#elif QT_VERSION >= 0x050600
                word.truncate(word.length() - 1);
#else
                QString temp = word.toString();
                temp.chop(1);
                word = QStringRef(&temp);
#endif
            }

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
}

