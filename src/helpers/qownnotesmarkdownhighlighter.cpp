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

#include "qownnotesmarkdownhighlighter.h"

#include <entities/note.h>

#include <QApplication>
#include <QDebug>
#include <QObject>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "libraries/sonnet/src/core/languagefilter_p.h"
#include "libraries/sonnet/src/core/tokenizer_p.h"
#include "qownspellchecker.h"

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
    spellchecker = nullptr;
    languageFilter = nullptr;
    wordTokenizer = nullptr;

    commentHighlightingOn = true;
    codeHighlightingOn = true;
}

QOwnNotesMarkdownHighlighter::~QOwnNotesMarkdownHighlighter() {
    if (languageFilter != nullptr) delete languageFilter;
    if (wordTokenizer != nullptr) delete wordTokenizer;
    if (spellchecker) delete spellchecker;
}

void QOwnNotesMarkdownHighlighter::updateCurrentNote(const Note _note) {
    _currentNote = std::move(_note);
}

void QOwnNotesMarkdownHighlighter::setSpellChecker(
    QOwnSpellChecker *spellChecker) {
    spellchecker = spellChecker;
    languageFilter =
        new Sonnet::LanguageFilter(new Sonnet::SentenceTokenizer());
    wordTokenizer = new Sonnet::WordTokenizer();
}

void QOwnNotesMarkdownHighlighter::setCommentHighlighting(bool state) {
    if (state == commentHighlightingOn) {
        return;
    }
    commentHighlightingOn = state;
}

void QOwnNotesMarkdownHighlighter::setCodeHighlighting(bool state) {
    if (state == codeHighlightingOn) {
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
    if (currentBlockState() == HeadlineEnd) {
        currentBlock().previous().setUserState(NoState);
        addDirtyBlock(currentBlock().previous());
    }
    setCurrentBlockState(HighlighterState::NoState);
    currentBlock().setUserState(HighlighterState::NoState);

    // do the markdown highlighting before the spellcheck highlighting
    // if we do it afterwards, it overwrites the spellcheck highlighting
    MarkdownHighlighter::highlightMarkdown(text);
    if (text.contains(QLatin1String("note://")) ||
        text.contains(QLatin1String(".md"))) {
        highlightBrokenNotesLink(text);
    }

    // skip spell checking empty blocks and blocks with just "spaces"
    // the rest of the highlighting needs to be done e.g. for code blocks with
    // empty lines
    if (spellchecker != nullptr) {
        if (!text.isEmpty() && spellchecker->isActive()) {
            highlightSpellChecking(text);
        }
    }

    _highlightingFinished = true;
}

/**
 * Highlight broken note links
 *
 * @param text
 */
void QOwnNotesMarkdownHighlighter::highlightBrokenNotesLink(
    const QString &text) {
    QRegularExpression regex(QStringLiteral(R"(note:\/\/[^\s\)>]+)"));
    QRegularExpressionMatch match = regex.match(text);

    if (match.hasMatch()) {    // check legacy note:// links
        const QString noteLink = match.captured(0);

        // try to fetch a note from the url string
        const Note note = Note::fetchByUrlString(noteLink);

        // if the note exists we don't need to do anything
        if (note.isFetched()) {
            return;
        }
    } else {    // check <note file.md> links
        regex = QRegularExpression(
            QStringLiteral("<([^\\s`][^`]*?\\.[^`]*?[^\\s`]\\.md)>"));
        match = regex.match(text);

        if (match.hasMatch()) {
            const QString fileName = Note::urlDecodeNoteUrl(match.captured(1));

            // skip urls
            if (fileName.contains(QStringLiteral("://"))) {
                return;
            }

            const Note note = _currentNote.fetchByRelativeFileName(fileName);

            // if the note exists we don't need to do anything
            if (note.isFetched()) {
                return;
            }
        } else {    // check [note](note file.md) links
            regex = QRegularExpression(
                QStringLiteral(R"(\[[^\[\]]+\]\((\S+\.md|.+?\.md)\)\B)"));
            match = regex.match(text);

            if (match.hasMatch()) {
                const QString fileName = Note::urlDecodeNoteUrl(match.captured(1));

                // skip urls
                if (fileName.contains(QStringLiteral("://"))) {
                    return;
                }

                const Note note = _currentNote.fetchByRelativeFileName(fileName);

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

void QOwnNotesMarkdownHighlighter::setMisspelled(const int start,
                                                 const int count) {
    // append to the already existing text format.
    // creating a new format will destroy pre-existing format
    QTextCharFormat format = QSyntaxHighlighter::format(start + 1);
    format.setFontUnderline(true);
    format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
    format.setUnderlineColor(Qt::red);
    setFormat(start, count, format);
}

/**
 * Uses the QOwnSpellChecker to check words for correctness and underlines the
 * incorrect words
 *
 * @param text
 */
void QOwnNotesMarkdownHighlighter::highlightSpellChecking(const QString &text) {
    if (text.length() < 2) {
        return;
    }
    if (!spellchecker->isValid()) {
        qWarning() << "[Sonnet]Spellchecker invalid!";
        return;
    }
    if (currentBlockState() == HighlighterState::HeadlineEnd ||
        currentBlockState() == HighlighterState::CodeBlock ||
        currentBlockState() >= HighlighterState::CodeCpp)
        return;

    // use our own settings, as KDE users might face issues with Autodetection
    const bool autodetectLanguage = spellchecker->isAutoDetectOn();
    // spellchecker->testAttribute(Sonnet::Speller::AutoDetectLanguage) : false;
    LanguageCache *languageCache = nullptr;
    if (autodetectLanguage) {
        languageCache = dynamic_cast<LanguageCache *>(currentBlockUserData());
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
            const QPair<int, int> spos =
                QPair<int, int>(sentence.position(), sentence.length());
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

            // if the word has _ at the end, word tokenizer misses that, so cut
            // it off
            if (word.endsWith(QLatin1Char('_'))) {
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

            // in case it's not a word, like an email or a number
            if (!wordTokenizer->isSpellcheckable()) {
                continue;
            }
            // if the word is misspelled
            if (spellchecker->isWordMisspelled(word.toString())) {
                setMisspelled(word.position() + offset, word.length());
                // else we do nothing and move on to the next word
            } else {
                // unsetMisspelled(word.position()+offset, word.length());
            }
        }
    }
}
