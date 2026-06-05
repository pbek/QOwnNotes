#ifdef LANGUAGETOOL_ENABLED

#include "services/languagetoolchecker.h"

#include <QCryptographicHash>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QTextBlock>
#include <QTimer>

#include "helpers/qownspellchecker.h"
#include "libraries/qmarkdowntextedit/markdownhighlighter.h"
#include "mainwindow.h"
#include "services/languagetoolclient.h"
#include "services/settingsservice.h"

LanguageToolChecker *LanguageToolChecker::instance() {
    static LanguageToolChecker checker;
    return &checker;
}

LanguageToolChecker::LanguageToolChecker() {
    _client = new LanguageToolClient(this);
    _debounceTimer = new QTimer(this);
    _debounceTimer->setSingleShot(true);

    connect(_debounceTimer, &QTimer::timeout, this, &LanguageToolChecker::recheckNow);
    connect(_client, &LanguageToolClient::checkFinished, this,
            &LanguageToolChecker::handleRequestFinished);
    connect(_client, &LanguageToolClient::checkError, this,
            &LanguageToolChecker::handleRequestError);

    qRegisterMetaType<LanguageToolMatch>("LanguageToolMatch");
    qRegisterMetaType<QVector<LanguageToolMatch>>("QVector<LanguageToolMatch>");
    readSettings();
}

void LanguageToolChecker::setTextEdit(QPlainTextEdit *textEdit) {
    if (_textEdit == textEdit) {
        return;
    }

    if (_textEdit) {
        disconnect(_textEdit, nullptr, this, nullptr);
        disconnect(_textEdit->document(), nullptr, this, nullptr);
    }

    clearCurrentState();
    _textEdit = textEdit;
    _document = textEdit ? textEdit->document() : nullptr;

    if (_textEdit == nullptr || _document == nullptr) {
        return;
    }

    connect(_textEdit, &QPlainTextEdit::textChanged, this, &LanguageToolChecker::handleTextChanged);
    connect(_textEdit->verticalScrollBar(), &QScrollBar::valueChanged, this,
            [this]() { scheduleCheck(); });
    connect(_document, &QTextDocument::contentsChange, this, [this](int position, int, int) {
        if (_document == nullptr) {
            return;
        }

        const QTextBlock block = _document->findBlock(position);
        if (block.isValid()) {
            invalidateBlock(block.blockNumber());
        }
    });

    scheduleCheck(true);
}

void LanguageToolChecker::clearForTextEdit(QPlainTextEdit *textEdit, bool updateBlocks) {
    if (_textEdit == textEdit) {
        clearCurrentState(updateBlocks);
        _textEdit.clear();
        _document.clear();
    }
}

void LanguageToolChecker::clearCurrentState(bool updateBlocks) {
    _debounceTimer->stop();
    _client->cancelAllRequests();
    _pendingRequests.clear();
    _pendingRequestId = 0;
    _cache.clear();
    _warningShown = false;
    if (updateBlocks) {
        Q_EMIT blockMatchesUpdated(QVector<int>());
    }
}

void LanguageToolChecker::invalidateBlock(int blockNumber) { _cache.remove(blockNumber); }

void LanguageToolChecker::scheduleCheck(bool immediate) {
    readSettings();
    if (!_enabled || _textEdit == nullptr || _document == nullptr) {
        if (_pendingRequestId > 0) {
            _client->cancelRequest(_pendingRequestId);
            _pendingRequests.remove(_pendingRequestId);
            _pendingRequestId = 0;
        }

        if (!_cache.isEmpty()) {
            _cache.clear();
            Q_EMIT blockMatchesUpdated(QVector<int>());
        }
        return;
    }

    if (immediate) {
        _debounceTimer->stop();
        recheckNow();
        return;
    }

    _debounceTimer->start(_checkDelayMs);
}

void LanguageToolChecker::recheckNow() {
    readSettings();
    if (!_enabled || _textEdit == nullptr || _document == nullptr) {
        if (_pendingRequestId > 0) {
            _client->cancelRequest(_pendingRequestId);
            _pendingRequests.remove(_pendingRequestId);
            _pendingRequestId = 0;
        }
        return;
    }

    const QVector<QTextBlock> blocks = visibleBlocks();
    if (blocks.isEmpty()) {
        return;
    }

    QString combinedText;
    QVector<int> blockNumbers;
    QVector<int> blockStartOffsets;
    QHash<int, QString> blockHashes;
    blockNumbers.reserve(blocks.size());
    blockStartOffsets.reserve(blocks.size());

    for (const QTextBlock &block : blocks) {
        const QString text = block.text();
        const QString hash = blockHash(text);
        const auto cached = _cache.value(block.blockNumber());
        const bool cacheValid =
            cached.textHash == hash &&
            cached.createdAt.secsTo(QDateTime::currentDateTimeUtc()) <= _cacheTtlSeconds;
        if (cacheValid) {
            continue;
        }

        blockNumbers.append(block.blockNumber());
        blockStartOffsets.append(combinedText.size());
        blockHashes.insert(block.blockNumber(), hash);
        combinedText += text;
        combinedText += QLatin1Char('\n');
    }

    if (blockNumbers.isEmpty()) {
        return;
    }

    if (_pendingRequestId > 0) {
        _client->cancelRequest(_pendingRequestId);
        _pendingRequests.remove(_pendingRequestId);
    }

    PendingRequestContext context;
    context.requestId = _nextRequestId++;
    context.documentRevisionHash = QString::fromLatin1(
        QCryptographicHash::hash(_document->toPlainText().toUtf8(), QCryptographicHash::Sha1)
            .toHex());
    context.submittedText = combinedText;
    context.document = _document;
    context.blockNumbers = blockNumbers;
    context.blockStartOffsets = blockStartOffsets;
    context.blockHashes = blockHashes;
    _pendingRequests.insert(context.requestId, context);
    _pendingRequestId = context.requestId;

    LanguageToolClient::RequestOptions options;
    options.requestId = context.requestId;
    options.timeoutMs = _timeoutMs;
    options.text = combinedText;
    options.language = _language;
    options.serverUrl = _serverUrl;
    options.apiKey = _apiKey;
    options.enabledCategories = _enabledCategories;
    options.disabledCategories = _disabledCategories;
    _client->checkText(options);
}

bool LanguageToolChecker::isEnabled() const { return _enabled; }

bool LanguageToolChecker::hasWarnedAboutConnection() const { return _warningShown; }

QVector<LanguageToolChecker::BlockMatch> LanguageToolChecker::matchesForBlock(
    int blockNumber, const QString &blockText) const {
    const auto cached = _cache.value(blockNumber);
    if (cached.textHash != blockHash(blockText)) {
        return {};
    }

    auto *self = const_cast<LanguageToolChecker *>(this);
    if (self->_cache.contains(blockNumber)) {
        self->_cache[blockNumber].lastAccessed = QDateTime::currentDateTimeUtc();
    }

    return cached.matches;
}

LanguageToolChecker::BlockMatch LanguageToolChecker::matchAtPosition(const QTextCursor &cursor,
                                                                     int positionInBlock) const {
    const QTextBlock block = cursor.block();
    const auto matches = matchesForBlock(block.blockNumber(), block.text());
    for (const BlockMatch &blockMatch : matches) {
        if (blockMatch.match.containsPosition(positionInBlock) &&
            !isRuleIgnored(blockMatch.match.ruleId)) {
            // Also check if the matched word is in the ignored words list
            const QString matchedText =
                block.text().mid(blockMatch.match.offset, blockMatch.match.length);
            if (isWordIgnored(matchedText)) {
                continue;
            }
            return blockMatch;
        }
    }

    return {};
}

bool LanguageToolChecker::hasTypoMatchCovering(int blockNumber, int start, int length,
                                               const QString &blockText) const {
    const auto matches = matchesForBlock(blockNumber, blockText);
    const int end = start + length;
    for (const BlockMatch &blockMatch : matches) {
        if (blockMatch.match.ruleCategory.compare(QStringLiteral("TYPOS"), Qt::CaseInsensitive) !=
            0) {
            continue;
        }

        const int matchStart = blockMatch.match.offset;
        const int matchEnd = matchStart + blockMatch.match.length;
        if (matchStart < end && matchEnd > start && !isRuleIgnored(blockMatch.match.ruleId)) {
            const QString matchedText =
                blockText.mid(blockMatch.match.offset, blockMatch.match.length);
            if (!isWordIgnored(matchedText)) {
                return true;
            }
        }
    }

    return false;
}

void LanguageToolChecker::ignoreRule(const QString &ruleId) {
    if (ruleId.isEmpty()) {
        return;
    }

    _ignoredRules.insert(ruleId);

    // Persist ignored rules to settings
    SettingsService settings;
    settings.setValue(QStringLiteral("languageToolIgnoredRules"),
                      QStringList(_ignoredRules.values()));

    // Emit empty list to trigger a full rehighlight of the entire document
    Q_EMIT blockMatchesUpdated(QVector<int>());
}

bool LanguageToolChecker::isRuleIgnored(const QString &ruleId) const {
    return _ignoredRules.contains(ruleId);
}

void LanguageToolChecker::clearIgnoredRules() {
    _ignoredRules.clear();

    SettingsService settings;
    settings.setValue(QStringLiteral("languageToolIgnoredRules"), QStringList());

    Q_EMIT blockMatchesUpdated(QVector<int>());
}

QSet<QString> LanguageToolChecker::ignoredRules() const { return _ignoredRules; }

void LanguageToolChecker::ignoreWord(const QString &word) {
    if (word.isEmpty()) {
        return;
    }

    _ignoredWords.insert(word.toLower());

    // Also tell the regular spell checker to ignore this word
    auto *spellChecker = QOwnSpellChecker::instance();
    if (spellChecker != nullptr) {
        spellChecker->addWordToDictionary(word);
    }

    // Persist ignored words to settings
    SettingsService settings;
    settings.setValue(QStringLiteral("languageToolIgnoredWords"),
                      QStringList(_ignoredWords.values()));

    // Emit empty list to trigger a full rehighlight of the entire document
    Q_EMIT blockMatchesUpdated(QVector<int>());
}

bool LanguageToolChecker::isWordIgnored(const QString &word) const {
    return _ignoredWords.contains(word.toLower());
}

void LanguageToolChecker::clearIgnoredWords() {
    _ignoredWords.clear();

    SettingsService settings;
    settings.setValue(QStringLiteral("languageToolIgnoredWords"), QStringList());

    Q_EMIT blockMatchesUpdated(QVector<int>());
}

QSet<QString> LanguageToolChecker::ignoredWords() const { return _ignoredWords; }

QStringList LanguageToolChecker::enabledCategories() const { return _enabledCategories; }

void LanguageToolChecker::readSettings() {
    SettingsService settings;
    _enabled = settings.value(QStringLiteral("languageToolEnabled"), false).toBool();
    _serverUrl =
        settings
            .value(QStringLiteral("languageToolServerUrl"), QStringLiteral("http://localhost:8081"))
            .toString()
            .trimmed();
    _language =
        settings.value(QStringLiteral("languageToolLanguage"), QStringLiteral("auto")).toString();
    _apiKey = settings.value(QStringLiteral("languageToolApiKey")).toString().trimmed();
    _checkDelayMs = settings.value(QStringLiteral("languageToolCheckDelay"), 1500).toInt();
    _timeoutMs = settings.value(QStringLiteral("languageToolTimeout"), 5000).toInt();
    _enabledCategories =
        settings
            .value(QStringLiteral("languageToolEnabledCategories"),
                   QStringList() << QStringLiteral("TYPOS") << QStringLiteral("GRAMMAR")
                                 << QStringLiteral("STYLE") << QStringLiteral("REDUNDANCY")
                                 << QStringLiteral("PUNCTUATION") << QStringLiteral("TYPOGRAPHY"))
            .toStringList();

    _disabledCategories.clear();
    const QStringList allCategories = {QStringLiteral("TYPOS"),       QStringLiteral("GRAMMAR"),
                                       QStringLiteral("STYLE"),       QStringLiteral("REDUNDANCY"),
                                       QStringLiteral("PUNCTUATION"), QStringLiteral("TYPOGRAPHY")};
    for (const QString &category : allCategories) {
        if (!_enabledCategories.contains(category)) {
            _disabledCategories.append(category);
        }
    }

    // Load persisted ignored rules
    const QStringList ignoredRulesList =
        settings.value(QStringLiteral("languageToolIgnoredRules")).toStringList();
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    _ignoredRules = QSet<QString>(ignoredRulesList.begin(), ignoredRulesList.end());
#else
    _ignoredRules.clear();
    for (const QString &ignoredRule : ignoredRulesList) {
        _ignoredRules.insert(ignoredRule);
    }
#endif
    _ignoredRules.remove(QString());

    // Load persisted ignored words
    const QStringList ignoredWordsList =
        settings.value(QStringLiteral("languageToolIgnoredWords")).toStringList();
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    _ignoredWords = QSet<QString>(ignoredWordsList.begin(), ignoredWordsList.end());
#else
    _ignoredWords.clear();
    for (const QString &ignoredWord : ignoredWordsList) {
        _ignoredWords.insert(ignoredWord);
    }
#endif
    _ignoredWords.remove(QString());

    _debounceTimer->setInterval(_checkDelayMs);
}

bool LanguageToolChecker::shouldCheckBlock(const QTextBlock &block) const {
    if (!block.isValid() || !block.isVisible()) {
        return false;
    }

    if (block.text().trimmed().isEmpty()) {
        return false;
    }

    const int state = block.userState();
    if (state == MarkdownHighlighter::HeadlineEnd || state == MarkdownHighlighter::CodeBlock ||
        state >= MarkdownHighlighter::CodeCpp) {
        return false;
    }

    return true;
}

QVector<QTextBlock> LanguageToolChecker::visibleBlocks() const {
    QVector<QTextBlock> blocks;
    if (_textEdit == nullptr) {
        return blocks;
    }

    const QRect viewportRect = _textEdit->viewport()->rect();

    const QTextCursor topCursor = _textEdit->cursorForPosition(viewportRect.topLeft());
    const QTextCursor bottomCursor =
        _textEdit->cursorForPosition(QPoint(viewportRect.right(), viewportRect.bottom()));

    QTextBlock block = topCursor.block();
    const int lastBlockNumber = bottomCursor.blockNumber();

    while (block.isValid() && block.blockNumber() <= lastBlockNumber) {
        if (shouldCheckBlock(block)) {
            blocks.append(block);
        }

        block = block.next();
    }

    return blocks;
}

void LanguageToolChecker::handleTextChanged() { scheduleCheck(); }

void LanguageToolChecker::handleRequestFinished(int requestId,
                                                const QVector<LanguageToolMatch> &matches) {
    const PendingRequestContext context = _pendingRequests.take(requestId);
    if (_pendingRequestId == requestId) {
        _pendingRequestId = 0;
    }

    if (context.requestId == 0 || context.document == nullptr || context.document != _document) {
        return;
    }

    if (_document == nullptr) {
        return;
    }

    const QString currentRevisionHash = QString::fromLatin1(
        QCryptographicHash::hash(_document->toPlainText().toUtf8(), QCryptographicHash::Sha1)
            .toHex());
    if (currentRevisionHash != context.documentRevisionHash) {
        return;
    }

    _warningShown = false;
    storeMatches(context, matches);
}

void LanguageToolChecker::handleRequestError(int requestId, const QString &errorMessage) {
    _pendingRequests.remove(requestId);
    if (_pendingRequestId == requestId) {
        _pendingRequestId = 0;
    }

    if (!_warningShown) {
        _warningShown = true;
        if (auto *mainWindow = MainWindow::instance()) {
            mainWindow->showStatusBarMessage(
                tr("LanguageTool is unavailable: %1").arg(errorMessage), QStringLiteral("⚠️"), 5000);
        }
        Q_EMIT connectionError(errorMessage);
    }
}

void LanguageToolChecker::storeMatches(const PendingRequestContext &context,
                                       const QVector<LanguageToolMatch> &matches) {
    QHash<int, QVector<BlockMatch>> perBlockMatches;
    perBlockMatches.reserve(context.blockNumbers.size());

    for (const LanguageToolMatch &match : matches) {
        for (int i = 0; i < context.blockNumbers.size(); ++i) {
            const int blockStart = context.blockStartOffsets.at(i);
            const int nextBlockStart = (i + 1 < context.blockStartOffsets.size())
                                           ? context.blockStartOffsets.at(i + 1)
                                           : context.submittedText.size();
            const int blockEnd = nextBlockStart - 1;
            if (match.offset < blockStart || match.offset >= blockEnd) {
                continue;
            }

            BlockMatch blockMatch;
            blockMatch.blockNumber = context.blockNumbers.at(i);
            blockMatch.blockOffset = blockStart;
            blockMatch.match = match;
            blockMatch.match.offset = match.offset - blockStart;

            const QTextBlock block = _document->findBlockByNumber(blockMatch.blockNumber);
            if (!block.isValid()) {
                break;
            }

            if (MarkdownHighlighter::isCodeBlock(block.userState())) {
                break;
            }

            perBlockMatches[blockMatch.blockNumber].append(blockMatch);
            break;
        }
    }

    QVector<int> changedBlocks;
    changedBlocks.reserve(context.blockNumbers.size());
    const QDateTime now = QDateTime::currentDateTimeUtc();
    for (const int blockNumber : context.blockNumbers) {
        CachedBlockResult result;
        result.textHash = context.blockHashes.value(blockNumber);
        result.matches = perBlockMatches.value(blockNumber);
        result.createdAt = now;
        result.lastAccessed = now;
        _cache.insert(blockNumber, result);
        changedBlocks.append(blockNumber);
    }

    cleanupExpiredCache();
    enforceCacheLimit();
    Q_EMIT blockMatchesUpdated(changedBlocks);
}

void LanguageToolChecker::cleanupExpiredCache() {
    const QDateTime now = QDateTime::currentDateTimeUtc();
    QList<int> expiredBlocks;
    for (auto it = _cache.constBegin(); it != _cache.constEnd(); ++it) {
        if (it.value().createdAt.secsTo(now) > _cacheTtlSeconds) {
            expiredBlocks.append(it.key());
        }
    }

    for (const int blockNumber : expiredBlocks) {
        _cache.remove(blockNumber);
    }
}

void LanguageToolChecker::enforceCacheLimit() {
    while (_cache.size() > _cacheLimit) {
        int oldestBlock = -1;
        QDateTime oldestAccess = QDateTime::currentDateTimeUtc();
        for (auto it = _cache.constBegin(); it != _cache.constEnd(); ++it) {
            if (oldestBlock < 0 || it.value().lastAccessed < oldestAccess) {
                oldestBlock = it.key();
                oldestAccess = it.value().lastAccessed;
            }
        }

        if (oldestBlock < 0) {
            break;
        }

        _cache.remove(oldestBlock);
    }
}

QString LanguageToolChecker::blockHash(const QString &text) const {
    return QString::fromLatin1(
        QCryptographicHash::hash(text.toUtf8(), QCryptographicHash::Sha1).toHex());
}

#endif
