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
    languageFilter = new Sonnet::LanguageFilter(new Sonnet::SentenceTokenizer());
    wordTokenizer = new Sonnet::WordTokenizer();
    wordCount = 0;
    errorCount = 0;
    codeBlock = 0;

    qDebug () <<"[Sonnet]Available Langs: "<< spellchecker->availableLanguages();
    qDebug () <<"[Sonnet]Available Backend: "<< spellchecker->availableBackends();
    qDebug () <<"[Sonnet]Available Dicts: "<< spellchecker->availableDictionaries();
    qDebug () <<"[Sonnet]Available Lang names: "<< spellchecker->availableLanguageNames();
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

//LanguageCache class
// * Copyright (C)  2004  Zack Rusin <zack@kde.org>
// * Copyright (C)  2006  Laurent Montel <montel@kde.org>
// * Copyright (C)  2013  Martin Sandsmark <martin.sandsmark@org>
class LanguageCache : public QTextBlockUserData
{
public:
    // Key: QPair<start, length>
    // Value: language name
    QMap<QPair<int, int>, QString> languages;

    // Remove all cached language information after @p pos
    void invalidate(int pos)
    {
        QMutableMapIterator<QPair<int, int>, QString> it(languages);
        it.toBack();
        while (it.hasPrevious()) {
            it.previous();
            if (it.key().first+it.key().second >= pos) {
                it.remove();
            } else {
                break;
            }
        }
    }

    QString languageAtPos(int pos) const
    {
        // The data structure isn't really great for such lookups...
        QMapIterator<QPair<int, int>, QString> it(languages);
        while (it.hasNext()) {
            it.next();
            if (it.key().first <= pos && it.key().first + it.key().second >= pos) {
                return it.value();
            }
        }
        return QString();
    }
};

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

void QOwnNotesMarkdownHighlighter::highlightSpellChecking(const QString &text) {
    //TODO: include other characters, for other languages
    //      add auto detection of languages
    if (text == "```") {
        codeBlock++;
    }
    if (codeBlock % 2 != 0) {
        return;
    }
    if (!hasNotEmptyText(text)) {
        return;
    }
    if (!spellchecker->isValid()) {
        qDebug () << "[Sonnet]Spellchercher invalid!";
    }

    languageFilter->setBuffer(text);

    LanguageCache *languageCache = dynamic_cast<LanguageCache*>(currentBlockUserData());
    if (!languageCache) {
        languageCache = new LanguageCache;
        setCurrentBlockUserData(languageCache);
    }

    const bool autodetectLanguage = spellchecker->testAttribute(Sonnet::Speller::AutoDetectLanguage);
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
            qDebug () << "Sentence: " << sentence;
            qDebug () << "Language detected: " << lang;
            spellchecker->setLanguage(lang);
        }

        wordTokenizer->setBuffer(sentence.toString());
        int offset = sentence.position();
        while (wordTokenizer->hasNext()) {
            QStringRef word = wordTokenizer->next();
            if (!wordTokenizer->isSpellcheckable()) {
                continue;
            }
            ++wordCount;
            if (spellchecker->isMisspelled(word.toString())) {
                ++errorCount;
                qDebug () << "Word->Position + offset" << word.position() + offset;
                qDebug () << "Word->length" << word.length();
                setMisspelled(word.position()+offset, word.length());
            } else {
                //unsetMisspelled(word.position()+offset, word.length());
            }
        }
    }

    setCurrentBlockState(0);

/*
 * Old implementation
 * Will be removed later
 */
//    QRegularExpression regex("[a-zA-Z]+");
//    QRegularExpressionMatchIterator it = regex.globalMatch(text);
//    while(it.hasNext()){
//        QRegularExpressionMatch m = it.next();
//        QString word = m.captured();
//        bool isMisspelled = !word.isEmpty()
//                         && word.length() > 3
//                         && isWordMisspelled(word)
//                         && word != " ";
//        if(isMisspelled) {
//            setMisspelled(m.capturedStart(0), m.capturedEnd());
//        }
//        else {
//            //disabling this for now because it's breaking the markdown highlighting
//            //unsetMisspelled(m.capturedStart(0), m.capturedEnd());
//        }
//    }
}

