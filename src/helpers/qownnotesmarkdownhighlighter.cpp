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
#include <QColor>
#include <QDebug>
#include <QFont>
#include <QObject>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "helpers/LanguageCache.h"
#include "mainwindow.h"
#include "qownspellchecker.h"
#ifdef LANGUAGETOOL_ENABLED
#include "services/languagetoolchecker.h"
#endif
#ifdef HARPER_ENABLED
#include "services/harperchecker.h"
#endif

constexpr qreal ENCRYPTED_TEXT_FONT_SCALE = 1.5;

QOwnNotesMarkdownHighlighter::QOwnNotesMarkdownHighlighter(QTextDocument *parent,
                                                           HighlightingOptions highlightingOptions)
    : MarkdownHighlighter(parent, highlightingOptions) {
    _defaultNoteFileExt = Note::defaultNoteFileExtension();
    connect(MainWindow::instance(), &MainWindow::settingsChanged, this, [this]() {
        _defaultNoteFileExt = Note::defaultNoteFileExtension();
        updateCachedRegexes(_defaultNoteFileExt);
        clearWikiLinkCache();
    });
}

void QOwnNotesMarkdownHighlighter::updateCurrentNote(Note *note) {
    Q_ASSERT(note);

    _currentNote = note;
    _hasEncrypted = _currentNote->hasEncryptedNoteText();
    _highlightEncrypted = false;    // Reset state when note changes
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

    // Only check for encryption markers if the note has encrypted content
    if (_hasEncrypted) {
        if (Note::isEncryptedTextBegin(text)) {
            _highlightEncrypted = true;
        }
    }

    if (_highlightEncrypted) {
        auto format = _formats[MaskedSyntax];
        // Reduce the font size for encrypted text
        format.setFontPointSize(format.fontPointSize() / ENCRYPTED_TEXT_FONT_SCALE);
        setFormat(0, text.length(), format);
    } else {
        // do the Markdown highlighting before the spellcheck highlighting
        // if we do it afterward, it overwrites the spellcheck highlighting
        highlightMarkdown(text);
    }

    if (!_highlightEncrypted) {
        if (text.contains(QLatin1String("note://")) ||
            text.contains(QChar('.') + _defaultNoteFileExt)) {
            highlightBrokenNotesLink(text);
        }

        if (Note::isWikiLinkSupportEnabled() && text.contains(QStringLiteral("[["))) {
            highlightWikiLinks(text);
        }

        // skip spell checking empty blocks and blocks with just "spaces"
        // the rest of the highlighting needs to be done e.g. for code blocks with
        // empty lines
        if (!text.isEmpty() && QOwnSpellChecker::instance()->isActive()) {
            highlightSpellChecking(text);
        }

#ifdef LANGUAGETOOL_ENABLED
        if (!text.isEmpty()) {
            highlightLanguageTool(text);
        }
#endif

#ifdef HARPER_ENABLED
        if (!text.isEmpty()) {
            highlightHarper(text);
        }
#endif

        highlightScriptingRules(ScriptingService::instance()->getHighlightingRules(), text);

        // Call the per-block highlightingHook in scripts (only invoked if at
        // least one script provides the hook, checked via cached flag)
        highlightScriptingHook(text);

        // Apply LSP diagnostic underlines last so they overlay all other formatting
        if (!text.isEmpty()) {
            highlightMarkdownLsp(text);
        }
    }

    // Only check for encryption end marker if we're highlighting encrypted text
    if (_hasEncrypted && _highlightEncrypted && Note::isEncryptedTextEnd(text)) {
        _highlightEncrypted = false;
    }

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

        // Build the format: use custom format if provided, otherwise use the
        // predefined format for the given state
        QTextCharFormat format;
        if (rule.hasCustomFormat) {
            // Start from the state format if a valid state was specified
            if (rule.state != NoState) {
                format = _formats[rule.state];
            }

            // Override with custom properties
            if (!rule.foregroundColor.isEmpty()) {
                format.setForeground(QColor(rule.foregroundColor));
            }
            if (!rule.backgroundColor.isEmpty()) {
                format.setBackground(QColor(rule.backgroundColor));
            }
            if (rule.bold) {
                format.setFontWeight(QFont::Bold);
            }
            if (rule.italic) {
                format.setFontItalic(true);
            }
            if (rule.underline) {
                format.setFontUnderline(true);
            }
            if (rule.fontSize > 0) {
                format.setFontPointSize(rule.fontSize);
            }
        } else {
            format = _formats[rule.state];
        }

        // Find and format all occurrences
        while (iterator.hasNext()) {
            QRegularExpressionMatch match = iterator.next();

            // If there is a capturingGroup set then first highlight
            // everything as MaskedSyntax and highlight capturingGroup
            // with the real format
            if (capturingGroup > 0) {
                QTextCharFormat currentMaskedFormat = maskedFormat;
                // Set the font size from the current rule's font format
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

/**
 * Calls the highlightingHook in all script components for the current text
 * block and applies the returned highlight ranges
 */
void QOwnNotesMarkdownHighlighter::highlightScriptingHook(const QString &text) {
    ScriptingService *scriptingService = ScriptingService::instance();

    if (!scriptingService->highlightingHookExists()) {
        return;
    }

    const QVariantList highlights =
        scriptingService->callHighlightingHook(text, previousBlockState());

    for (const QVariant &item : highlights) {
        const QVariantMap m = item.toMap();
        const int start = m.value(QStringLiteral("start")).toInt();
        const int length = m.value(QStringLiteral("length")).toInt();

        if (length <= 0) {
            continue;
        }

        // Determine the format from the state or custom properties
        const int state = m.value(QStringLiteral("state"), -1).toInt();
        QTextCharFormat format;

        if (state >= 0) {
            format = _formats[static_cast<HighlighterState>(state)];
        }

        // Apply custom format overrides
        const QString fg = m.value(QStringLiteral("foregroundColor")).toString();
        if (!fg.isEmpty()) {
            format.setForeground(QColor(fg));
        }

        const QString bg = m.value(QStringLiteral("backgroundColor")).toString();
        if (!bg.isEmpty()) {
            format.setBackground(QColor(bg));
        }

        if (m.value(QStringLiteral("bold")).toBool()) {
            format.setFontWeight(QFont::Bold);
        }

        if (m.value(QStringLiteral("italic")).toBool()) {
            format.setFontItalic(true);
        }

        if (m.value(QStringLiteral("underline")).toBool()) {
            format.setFontUnderline(true);
        }

        const qreal fontSize = m.value(QStringLiteral("fontSize")).toReal();
        if (fontSize > 0) {
            format.setFontPointSize(fontSize);
        }

        setFormat(start, length, format);
    }
}

void QOwnNotesMarkdownHighlighter::updateCachedRegexes(const QString &newExt) {
    _regexTagStyleLink = QRegularExpression(R"(<([^\s`][^`]*?\.)" + newExt + R"()>)");
    _regexBracketLink = QRegularExpression(R"(\[[^\[\]]+\]\((\S+\.)" + newExt + R"(|.+?\.)" +
                                           newExt + R"()(#[^\)]+)?\)\B)");
}

void QOwnNotesMarkdownHighlighter::clearWikiLinkCache() { _wikiLinkCache.clear(); }

/**
 * Highlight internal note links (both valid and broken)
 *
 * @param text
 */
void QOwnNotesMarkdownHighlighter::highlightBrokenNotesLink(const QString &text) {
    const int blockState = currentBlockState();
    if (MarkdownHighlighter::isCodeBlock(blockState) ||
        MarkdownHighlighter::isCodeBlockEnd(blockState) ||
        blockState == HighlighterState::CodeBlockIndented) {
        return;
    }

    const auto isMatchInCodeSpan = [this](const QRegularExpressionMatch &match) {
        return MarkdownHighlighter::isPosInACodeSpan(currentBlock().blockNumber(),
                                                     match.capturedStart(0));
    };

    static const QRegularExpression regex(QStringLiteral(R"(note:\/\/[^\s\)>]+)"));
    QRegularExpressionMatch match = regex.match(text);
    bool noteExists = false;

    if (match.hasMatch()) {    // check legacy note:// links
        if (isMatchInCodeSpan(match)) {
            return;
        }

        const QString noteLink = match.captured(0);

        // try to fetch a note from the url string
        const Note note = Note::fetchByUrlString(noteLink);
        noteExists = note.isFetched();
    } else {
        // don't make any further checks if no current note was set
        if (_currentNote == nullptr) {
            return;
        }

        // check <note file.md> links
        // Example: <([^\s`][^`]*?\.md)>
        match = _regexTagStyleLink.match(text);

        if (match.hasMatch()) {
            if (isMatchInCodeSpan(match)) {
                return;
            }

            const QString fileName = Note::urlDecodeNoteUrl(match.captured(1));

            // skip urls
            if (fileName.contains(QStringLiteral("://"))) {
                return;
            }

            const Note note = _currentNote->fetchByRelativeFileName(fileName);
            noteExists = note.isFetched();
        } else {    // check [note](note file.md) or [note](note file.md#heading) links
            // Example: R"(\[[^\[\]]+\]\((\S+\.md|.+?\.md)(#[^\)]+)?\)\B)")
            match = _regexBracketLink.match(text);

            if (match.hasMatch()) {
                if (isMatchInCodeSpan(match)) {
                    return;
                }

                const QString fileName = Note::urlDecodeNoteUrl(match.captured(1));

                // skip urls
                if (fileName.contains(QStringLiteral("://"))) {
                    return;
                }

                const Note note = _currentNote->fetchByRelativeFileName(fileName);
                noteExists = note.isFetched();
            } else {
                // no note link was found
                return;
            }
        }
    }

    auto state = noteExists ? HighlighterState::LinkInternal : HighlighterState::BrokenLink;

    setFormat(match.capturedStart(0), match.capturedLength(0), _formats[state]);
}

void QOwnNotesMarkdownHighlighter::highlightWikiLinks(const QString &text) {
    if (_currentNote == nullptr) {
        return;
    }

    static const QRegularExpression regex(QStringLiteral(R"(\[\[([^\[\]]+?)\]\])"));
    const QTextCharFormat maskedFormat = currentMaskedFormat();
    auto iterator = regex.globalMatch(text);

    while (iterator.hasNext()) {
        const QRegularExpressionMatch match = iterator.next();
        const QString innerText = match.captured(1).trimmed();
        if (innerText.isEmpty()) {
            continue;
        }

        bool exists = _wikiLinkCache.value(innerText, false);
        if (!_wikiLinkCache.contains(innerText)) {
            exists =
                Note::resolveWikiLink(innerText, _currentNote->getNoteSubFolderId()).isFetched();
            _wikiLinkCache.insert(innerText, exists);
        }

        QTextCharFormat openFormat = maskedFormat;
        QTextCharFormat closeFormat = maskedFormat;
        QTextCharFormat bodyFormat =
            _formats[exists ? HighlighterState::WikiLink : HighlighterState::WikiLinkBroken];
        if (bodyFormat.fontPointSize() > 0) {
            openFormat.setFontPointSize(bodyFormat.fontPointSize());
            closeFormat.setFontPointSize(bodyFormat.fontPointSize());
        }

        setFormat(match.capturedStart(0), 2, openFormat);
        setFormat(match.capturedStart(1), match.capturedLength(1), bodyFormat);
        setFormat(match.capturedStart(0) + match.capturedLength(0) - 2, 2, closeFormat);
    }
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

#ifdef LANGUAGETOOL_ENABLED
void QOwnNotesMarkdownHighlighter::setLanguageToolUnderline(int start, int count,
                                                            const QColor &color,
                                                            const QString &toolTip) {
    if (MarkdownHighlighter::isPosInACodeSpan(currentBlock().blockNumber(), start)) {
        return;
    }

    // Apply underline properties per-character to preserve each character's
    // individual foreground color set by earlier highlight passes (e.g. headings,
    // bold/italic, links). Reading only the first character's format and broadcasting
    // it over the whole span would overwrite the foreground of every subsequent
    // character, causing wrong text colors under LanguageTool underlines (issue #3496).
    for (int i = start; i < start + count; ++i) {
        QTextCharFormat format = QSyntaxHighlighter::format(i);
        format.setFontUnderline(true);
        format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
        format.setUnderlineColor(color);
        format.setToolTip(toolTip);
        setFormat(i, 1, format);
    }
}

void QOwnNotesMarkdownHighlighter::highlightLanguageTool(const QString &text) {
    auto *checker = LanguageToolChecker::instance();
    if ((checker == nullptr) || !checker->isEnabled()) {
        return;
    }

    auto *languageCache = dynamic_cast<LanguageCache *>(currentBlockUserData());
    if (!languageCache) {
        languageCache = new LanguageCache;
        setCurrentBlockUserData(languageCache);
    }

    const auto matches = checker->matchesForBlock(currentBlock().blockNumber(), text);
    QVector<LanguageToolMatch> blockMatches;
    blockMatches.reserve(matches.size());

    for (const auto &blockMatch : matches) {
        if (checker->isRuleIgnored(blockMatch.match.ruleId)) {
            continue;
        }

        // Skip matches where the matched word is in the ignored words list
        const QString matchedText = text.mid(blockMatch.match.offset, blockMatch.match.length);
        if (checker->isWordIgnored(matchedText)) {
            continue;
        }

        if (blockMatch.match.length <= 0) {
            continue;
        }

        const QString category = blockMatch.match.ruleCategory.toUpper();
        if ((category == QStringLiteral("TYPOS")) && QOwnSpellChecker::instance()->isActive() &&
            checker->hasTypoMatchCovering(currentBlock().blockNumber(), blockMatch.match.offset,
                                          blockMatch.match.length, text)) {
            // LanguageTool suggestions win over Sonnet, but we still avoid double-underlining
        }

        QColor color(Qt::blue);
        if (category == QStringLiteral("TYPOS")) {
            color = Qt::red;
        } else if (category == QStringLiteral("GRAMMAR")) {
            color = QColor(QStringLiteral("#2a6fdb"));
        } else if ((category == QStringLiteral("STYLE")) ||
                   (category == QStringLiteral("REDUNDANCY"))) {
            color = QColor(QStringLiteral("#c99500"));
        } else if (category == QStringLiteral("PUNCTUATION")) {
            color = QColor(QStringLiteral("#1c8f47"));
        } else if (category == QStringLiteral("TYPOGRAPHY")) {
            color = QColor(QStringLiteral("#7a3db8"));
        }

        setLanguageToolUnderline(blockMatch.match.offset, blockMatch.match.length, color,
                                 blockMatch.match.message);
        blockMatches.append(blockMatch.match);
    }

    languageCache->setLanguageToolMatches(blockMatches);
}
#endif

#ifdef HARPER_ENABLED
void QOwnNotesMarkdownHighlighter::setHarperUnderline(int start, int count, const QColor &color,
                                                      const QString &toolTip) {
    if (MarkdownHighlighter::isPosInACodeSpan(currentBlock().blockNumber(), start)) {
        return;
    }

    for (int i = start; i < start + count; ++i) {
        QTextCharFormat format = QSyntaxHighlighter::format(i);
        format.setFontUnderline(true);
        format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
        format.setUnderlineColor(color);
        format.setToolTip(toolTip);
        setFormat(i, 1, format);
    }
}

void QOwnNotesMarkdownHighlighter::highlightHarper(const QString &text) {
    auto *checker = HarperChecker::instance();
    if ((checker == nullptr) || !checker->isEnabled()) {
        return;
    }

    auto *languageCache = dynamic_cast<LanguageCache *>(currentBlockUserData());
    if (!languageCache) {
        languageCache = new LanguageCache;
        setCurrentBlockUserData(languageCache);
    }

    const auto matches = checker->matchesForBlock(currentBlock().blockNumber(), text);
    QVector<HarperMatch> blockMatches;
    blockMatches.reserve(matches.size());

    for (const auto &blockMatch : matches) {
        if (checker->isRuleIgnored(blockMatch.match.ruleId)) {
            continue;
        }

        const QString matchedText = text.mid(blockMatch.match.offset, blockMatch.match.length);
        if (checker->isWordIgnored(matchedText) || (blockMatch.match.length <= 0)) {
            continue;
        }

        QColor color = QColor(QStringLiteral("#2a6fdb"));
        const QString category = blockMatch.match.ruleCategory;
        if (category == QStringLiteral("Spelling")) {
            color = Qt::red;
        } else if (category == QStringLiteral("Style")) {
            color = QColor(QStringLiteral("#c99500"));
        } else if (category == QStringLiteral("Punctuation")) {
            color = QColor(QStringLiteral("#1c8f47"));
        } else if (category == QStringLiteral("Typography")) {
            color = QColor(QStringLiteral("#7a3db8"));
        }

        setHarperUnderline(blockMatch.match.offset, blockMatch.match.length, color,
                           blockMatch.match.message);
        blockMatches.append(blockMatch.match);
    }

    languageCache->setHarperMatches(blockMatches);
}
#endif

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

/**
 * Stores LSP diagnostics in the block-keyed cache and triggers a rehighlight
 * of every affected block so the underlines appear via QSyntaxHighlighter::setFormat(),
 * exactly like the LanguageTool and Harper integrations.
 */
void QOwnNotesMarkdownHighlighter::setMarkdownLspDiagnostics(
    const QVector<MarkdownLspClient::Diagnostic> &diagnostics) {
    _lspDiagnosticsCache.clear();

    for (const MarkdownLspClient::Diagnostic &diag : diagnostics) {
        if (diag.message.isEmpty()) {
            continue;
        }

        // Choose underline color based on LSP severity:
        // 1 = Error, 2 = Warning, 3 = Information, 4 = Hint
        QColor color;
        switch (diag.severity) {
            case 1:
                color = QColor(214, 68, 68);    // Red for errors
                break;
            case 2:
                color = QColor(210, 140, 30);    // Orange for warnings
                break;
            case 3:
                color = QColor(80, 140, 210);    // Blue for information
                break;
            case 4:
                color = QColor(100, 170, 100);    // Green for hints
                break;
            default:
                color = QColor(214, 68, 68);
                break;
        }

        // A diagnostic may span multiple lines; add an entry for every covered line
        for (int line = diag.range.startLine; line <= diag.range.endLine; ++line) {
            LspBlockDiagnostic entry;
            entry.color = color;
            entry.toolTip = diag.message;

            if (line == diag.range.startLine && line == diag.range.endLine) {
                entry.startCharacter = diag.range.startCharacter;
                entry.endCharacter = diag.range.endCharacter;
            } else if (line == diag.range.startLine) {
                entry.startCharacter = diag.range.startCharacter;
                entry.endCharacter = INT_MAX;    // Highlight to end of line
            } else if (line == diag.range.endLine) {
                entry.startCharacter = 0;
                entry.endCharacter = diag.range.endCharacter;
            } else {
                entry.startCharacter = 0;
                entry.endCharacter = INT_MAX;    // Highlight entire intermediate line
            }

            _lspDiagnosticsCache[line].append(entry);
        }
    }
}

/**
 * Clears all cached LSP diagnostics.
 */
void QOwnNotesMarkdownHighlighter::clearMarkdownLspDiagnostics() { _lspDiagnosticsCache.clear(); }

/**
 * Applies wave underlines for LSP diagnostics on the current block via
 * QSyntaxHighlighter::setFormat(), so they behave identically to spell-check
 * and grammar-checker underlines (document-level, not view-level).
 */
void QOwnNotesMarkdownHighlighter::highlightMarkdownLsp(const QString &text) {
    const int blockNumber = currentBlock().blockNumber();
    const auto it = _lspDiagnosticsCache.constFind(blockNumber);
    if (it == _lspDiagnosticsCache.constEnd()) {
        return;
    }

    for (const LspBlockDiagnostic &entry : *it) {
        const int start = entry.startCharacter;
        const int end = (entry.endCharacter == INT_MAX) ? text.length() : entry.endCharacter;
        const int count = end - start;
        if (count <= 0 || start >= text.length()) {
            continue;
        }
        setMarkdownLspUnderline(start, qMin(count, text.length() - start), entry.color,
                                entry.toolTip);
    }
}

/**
 * Applies a wave underline to a character range in the current block, merging
 * with the existing per-character format to preserve syntax highlighting colors.
 *
 * Batches runs of characters that share the same base format into single
 * setFormat() calls so the text layout sees O(runs) format ranges instead
 * of O(characters), which matters for long diagnostic ranges.
 */
void QOwnNotesMarkdownHighlighter::setMarkdownLspUnderline(int start, int count,
                                                           const QColor &color,
                                                           const QString &toolTip) {
    if (count <= 0) {
        return;
    }

    const int end = start + count;
    int runStart = start;

    // Read the base format at the first character and build the merged format
    QTextCharFormat baseFormat = QSyntaxHighlighter::format(runStart);
    QTextCharFormat merged = baseFormat;
    merged.setFontUnderline(true);
    merged.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
    merged.setUnderlineColor(color);
    merged.setToolTip(toolTip);

    for (int i = start + 1; i < end; ++i) {
        const QTextCharFormat fmt = QSyntaxHighlighter::format(i);
        if (fmt == baseFormat) {
            // Same base format — extend the current run
            continue;
        }

        // Flush the current run
        setFormat(runStart, i - runStart, merged);

        // Start a new run with the new base format
        runStart = i;
        baseFormat = fmt;
        merged = baseFormat;
        merged.setFontUnderline(true);
        merged.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
        merged.setUnderlineColor(color);
        merged.setToolTip(toolTip);
    }

    // Flush the last run
    setFormat(runStart, end - runStart, merged);
}
