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
        QTextDocument *parent,  HighlightingOptions highlightingOptions)
        : MarkdownHighlighter(parent, highlightingOptions) {
    Q_UNUSED(parent)
    Q_UNUSED(highlightingOptions)

    spellchecker = new Sonnet::Speller();
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
    highlightMarkdown(text);
    highlightSpellChecking(text);
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

QString QOwnNotesMarkdownHighlighter::currentLanguage() const {
    return spellchecker->language();
}

void QOwnNotesMarkdownHighlighter::setCurrentLanguage(const QString &lang) {
    spellchecker->setLanguage(lang);
}

bool QOwnNotesMarkdownHighlighter::isWordMisspelled(const QString &word) {
    return spellchecker->isMisspelled(word);
}

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
    QTextCharFormat format = QSyntaxHighlighter::format(start);

    //turn off the spell-check underline if it is turned on.
    //Note: Don't use - format.fontUnderline() - to check whether
    //font underlining is on. It is often off but the lines are appearing
    //so we just check whether it has spellcheck underline.
    if(format.underlineStyle() == QTextCharFormat::SpellCheckUnderline) {
        format.setFontUnderline(false);
    }
    setFormat(start, count, format);
}

void QOwnNotesMarkdownHighlighter::highlightSpellChecking(const QString &text) {
    //TODO: include other characters, for other languages
    //      add auto detection of languages
//    static int i = 0;
//    if(i=1){
//    qDebug () <<"Langs: "<< spellchecker->availableLanguages();
//    qDebug () <<"Bck: "<< spellchecker->availableBackends();
//    qDebug () <<"Dicts: "<< spellchecker->availableDictionaries();
//    qDebug () <<"Lang names: "<< spellchecker->availableLanguageNames();
//    i++; }

    QRegularExpression regex("[a-zA-Z]+");
    QRegularExpressionMatchIterator it = regex.globalMatch(text);
    while(it.hasNext()){
        QRegularExpressionMatch m = it.next();
        QString word = m.captured();
        bool isMisspelled = !word.isEmpty()
                         && word.length() > 1
                         && isWordMisspelled(word);
        if(isMisspelled)
            setMisspelled(m.capturedStart(0), m.capturedEnd());
        else {
            unsetMisspelled(m.capturedStart(0), m.capturedEnd());
        }
    }
}

