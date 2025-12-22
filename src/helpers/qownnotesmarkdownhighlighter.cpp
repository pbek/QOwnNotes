/*
 * Copyright (c) 2014-2026 Patrizio Bekerle -- <patrizio@bekerle.com>
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

#include "qownnotesmarkdownhighlighter.h"

#include <entities/note.h>
#include <services/scriptingservice.h>

#include <QApplication>
#include <QDebug>
#include <QObject>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "mainwindow.h"
#include "qownspellchecker.h"

QOwnNotesMarkdownHighlighter::QOwnNotesMarkdownHighlighter(QTextDocument *parent,
                                                           HighlightingOptions highlightingOptions)
    : MarkdownHighlighter(parent, highlightingOptions) {
    _defaultNoteFileExt = Note::defaultNoteFileExtension();
    connect(MainWindow::instance(), &MainWindow::settingsChanged, this, [this]() {
        _defaultNoteFileExt = Note::defaultNoteFileExtension();
        updateCachedRegexes(_defaultNoteFileExt);
    });
}

void QOwnNotesMarkdownHighlighter::updateCurrentNote(Note *note) {
    if (note != nullptr) {
        _currentNote = note;
    }
}

/**
 * Does the Markdown highlighting
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

    // do the Markdown highlighting before the spellcheck highlighting
    // if we do it afterward, it overwrites the spellcheck highlighting
    MarkdownHighlighter::highlightMarkdown(text);
    if (text.contains(QLatin1String("note://")) ||
        text.contains(QChar('.') + _defaultNoteFileExt)) {
        highlightBrokenNotesLink(text);
    }

    // skip spell checking empty blocks and blocks with just "spaces"
    // the rest of the highlighting needs to be done e.g. for code blocks with
    // empty lines
    if (!text.isEmpty() && QOwnSpellChecker::instance()->isActive()) {
        highlightSpellChecking(text);
    }

    highlightScriptingRules(ScriptingService::instance()->getHighlightingRules(), text);

    _highlightingFinished = true;
}

void QOwnNotesMarkdownHighlighter::highlightScriptingRules(
    const QVector<ScriptingHighlightingRule> &rules, const QString &text) {
    if (rules.isEmpty()) {
        return;
    }

    const auto &maskedFormat = _formats[HighlighterState::MaskedSyntax];

    for (const ScriptingHighlightingRule &rule : rules) {
        const bool contains = text.contains(rule.shouldContain);
        if (!contains) continue;

        auto iterator = rule.pattern.globalMatch(text);
        const uint8_t capturingGroup = rule.capturingGroup;
        const uint8_t maskedGroup = rule.maskedGroup;
        const QTextCharFormat &format = _formats[rule.state];

        // find and format all occurrences
        while (iterator.hasNext()) {
            QRegularExpressionMatch match = iterator.next();

            // if there is a capturingGroup set then first highlight
            // everything as MaskedSyntax and highlight capturingGroup
            // with the real format
            if (capturingGroup > 0) {
                QTextCharFormat currentMaskedFormat = maskedFormat;
                // set the font size from the current rule's font format
                if (format.fontPointSize() > 0) {
                    currentMaskedFormat.setFontPointSize(format.fontPointSize());
                }

                setFormat(match.capturedStart(maskedGroup), match.capturedLength(maskedGroup),
                          currentMaskedFormat);
            }

            setFormat(match.capturedStart(capturingGroup), match.capturedLength(capturingGroup),
                      format);
        }
    }
}

void QOwnNotesMarkdownHighlighter::updateCachedRegexes(const QString &newExt) {
    _regexTagStyleLink = QRegularExpression(R"(<([^\s`][^`]*?\.)" + newExt + R"()>)");
    _regexBracketLink = QRegularExpression(R"(\[[^\[\]]+\]\((\S+\.)" + newExt + R"(|.+?\.)" +
                                           newExt + R"()(#[^\)]+)?\)\B)");
}

/**
 * Highlight broken note links
 *
 * @param text
 */
void QOwnNotesMarkdownHighlighter::highlightBrokenNotesLink(const QString &text) {
    static const QRegularExpression regex(QStringLiteral(R"(note:\/\/[^\s\)>]+)"));
    QRegularExpressionMatch match = regex.match(text);

    if (match.hasMatch()) {    // check legacy note:// links
        const QString noteLink = match.captured(0);

        // try to fetch a note from the url string
        const Note note = Note::fetchByUrlString(noteLink);

        // if the note exists we don't need to do anything
        if (note.isFetched()) {
            return;
        }
    } else {
        // don't make any further checks if no current note was set
        if (_currentNote == nullptr) {
            return;
        }

        // check <note file.md> links
        // Example: <([^\s`][^`]*?\.md)>
        match = _regexTagStyleLink.match(text);

        if (match.hasMatch()) {
            const QString fileName = Note::urlDecodeNoteUrl(match.captured(1));

            // skip urls
            if (fileName.contains(QStringLiteral("://"))) {
                return;
            }

            const Note note = _currentNote->fetchByRelativeFileName(fileName);

            // if the note exists we don't need to do anything
            if (note.isFetched()) {
                return;
            }
        } else {    // check [note](note file.md) or [note](note file.md#heading) links
            // Example: R"(\[[^\[\]]+\]\((\S+\.md|.+?\.md)(#[^\)]+)?\)\B)")
            match = _regexBracketLink.match(text);

            if (match.hasMatch()) {
                const QString fileName = Note::urlDecodeNoteUrl(match.captured(1));

                // skip urls
                if (fileName.contains(QStringLiteral("://"))) {
                    return;
                }

                const Note note = _currentNote->fetchByRelativeFileName(fileName);

                // if the note exists we don't need to do anything
                if (note.isFetched()) {
                    return;
                }
            }
            // no note link was found
        }
    }

    auto state = HighlighterState(HighlighterState::BrokenLink);

    setFormat(match.capturedStart(0), match.capturedLength(0), _formats[state]);
}

void QOwnNotesMarkdownHighlighter::setMisspelled(const int start, const int count) {
    if (MarkdownHighlighter::isPosInACodeSpan(currentBlock().blockNumber(), start)) return;

    // append to the already existing text format.
    // creating a new format will destroy pre-existing format
    QTextCharFormat format = QSyntaxHighlighter::format(start);
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
    if (!QOwnSpellChecker::instance()->isValid()) {
        qWarning() << "Spellchecker invalid for current language!";
        return;
    }
    int state = currentBlockState();
    if (state == HighlighterState::HeadlineEnd || state == HighlighterState::CodeBlock ||
        state >= HighlighterState::CodeCpp)
        return;

    // use our own settings, as KDE users might face issues with Autodetection
    const bool autodetectLanguage = QOwnSpellChecker::instance()->isAutoDetectOn();
    LanguageCache *languageCache = nullptr;
    if (autodetectLanguage) {
        languageCache = dynamic_cast<LanguageCache *>(currentBlockUserData());
        if (!languageCache) {
            languageCache = new LanguageCache;
            setCurrentBlockUserData(languageCache);
        }
    }
    auto languageFilter = QOwnSpellChecker::instance()->languageFilter();
    languageFilter->setBuffer(text);
    while (languageFilter->hasNext()) {
        const Sonnet::Token sentence = languageFilter->next();
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
            QOwnSpellChecker::instance()->setCurrentLanguage(lang);
        }

        const auto wordTokenizer = QOwnSpellChecker::instance()->wordTokenizer();
        wordTokenizer->setBuffer(sentence.toString());
        const int offset = sentence.position();
        while (wordTokenizer->hasNext()) {
            Sonnet::Token w = wordTokenizer->next();

            // if the word has _ at the end, word tokenizer misses that, so cut
            // it off
            QString word = w.token;
            if (word.endsWith(QLatin1Char('_'))) {
                word.chop(1);
            }

            // in case it's not a word, like an email or a number
            if (!wordTokenizer->isSpellcheckable()) {
                continue;
            }
            // if the word is misspelled
            if (QOwnSpellChecker::instance()->isWordMisspelled(word)) {
                setMisspelled(w.position() + offset, w.length());
            } else {
                // unsetMisspelled(word.position()+offset, word.length());
            }
        }
    }
}
