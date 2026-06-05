#ifdef HARPER_ENABLED

#include "services/harperchecker.h"

#include <QCryptographicHash>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QTextBlock>
#include <QTimer>

#include "helpers/qownspellchecker.h"
#include "libraries/qmarkdowntextedit/markdownhighlighter.h"
#include "mainwindow.h"
#include "services/harperclient.h"
#include "services/settingsservice.h"

HarperChecker *HarperChecker::instance() {
    static HarperChecker checker;
    return &checker;
}

HarperChecker::HarperChecker() {
    _client = new HarperClient(this);
    _debounceTimer = new QTimer(this);
    _debounceTimer->setSingleShot(true);

    connect(_debounceTimer, &QTimer::timeout, this, &HarperChecker::recheckNow);
    connect(_client, &HarperClient::checkFinished, this, &HarperChecker::handleRequestFinished);
    connect(_client, &HarperClient::checkError, this, &HarperChecker::handleRequestError);

    qRegisterMetaType<HarperMatch>("HarperMatch");
    qRegisterMetaType<QVector<HarperMatch>>("QVector<HarperMatch>");
    readSettings();
}

void HarperChecker::setTextEdit(QPlainTextEdit *textEdit) {
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

    if ((_textEdit == nullptr) || (_document == nullptr)) {
        return;
    }

    connect(_textEdit, &QPlainTextEdit::textChanged, this, &HarperChecker::handleTextChanged);
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

void HarperChecker::clearForTextEdit(QPlainTextEdit *textEdit, bool updateBlocks) {
    if (_textEdit == textEdit) {
        clearCurrentState(updateBlocks);
        _textEdit.clear();
        _document.clear();
    }
}

void HarperChecker::clearCurrentState(bool updateBlocks) {
    _debounceTimer->stop();
    _client->cancelAllRequests();
    _pendingRequests.clear();
    _pendingRequestId = 0;
    _cache.clear();
    _warningShown = false;
    const bool oldAvailable = _available;
    _available = false;
    if (oldAvailable != _available) {
        Q_EMIT availabilityChanged(_available);
    }
    if (updateBlocks) {
        Q_EMIT blockMatchesUpdated(QVector<int>());
    }
}

void HarperChecker::invalidateBlock(int blockNumber) { _cache.remove(blockNumber); }

void HarperChecker::scheduleCheck(bool immediate) {
    readSettings();
    if (!_enabled || (_textEdit == nullptr) || (_document == nullptr)) {
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

void HarperChecker::recheckNow() {
    readSettings();
    if (!_enabled || (_textEdit == nullptr) || (_document == nullptr)) {
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
            (cached.textHash == hash) &&
            (cached.createdAt.secsTo(QDateTime::currentDateTimeUtc()) <= _cacheTtlSeconds);
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

    HarperClient::RequestOptions options;
    options.requestId = context.requestId;
    options.timeoutMs = _timeoutMs;
    options.text = combinedText;
    options.dialect = _dialect;
    options.enabledLinters = _enabledLinters;
    options.transportMode = _transportMode;
    options.binaryPath = _binaryPath;
    options.tcpAddress = _tcpAddress;
    options.tcpPort = _tcpPort;
    _client->checkText(options);
}

bool HarperChecker::isEnabled() const { return _enabled; }

bool HarperChecker::isAvailable() const { return _available; }

QVector<HarperChecker::BlockMatch> HarperChecker::matchesForBlock(int blockNumber,
                                                                  const QString &blockText) const {
    const auto cached = _cache.value(blockNumber);
    if (cached.textHash != blockHash(blockText)) {
        return {};
    }

    auto *self = const_cast<HarperChecker *>(this);
    if (self->_cache.contains(blockNumber)) {
        self->_cache[blockNumber].lastAccessed = QDateTime::currentDateTimeUtc();
    }

    return cached.matches;
}

HarperChecker::BlockMatch HarperChecker::matchAtPosition(const QTextCursor &cursor,
                                                         int positionInBlock) const {
    const QTextBlock block = cursor.block();
    const auto matches = matchesForBlock(block.blockNumber(), block.text());
    for (const BlockMatch &blockMatch : matches) {
        if (blockMatch.match.containsPosition(positionInBlock) &&
            !isRuleIgnored(blockMatch.match.ruleId)) {
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

void HarperChecker::ignoreRule(const QString &ruleId) {
    if (ruleId.isEmpty()) {
        return;
    }

    _ignoredRules.insert(ruleId);

    SettingsService settings;
    settings.setValue(QStringLiteral("harperIgnoredRules"), QStringList(_ignoredRules.values()));
    Q_EMIT blockMatchesUpdated(QVector<int>());
}

bool HarperChecker::isRuleIgnored(const QString &ruleId) const {
    return _ignoredRules.contains(ruleId);
}

void HarperChecker::clearIgnoredRules() {
    _ignoredRules.clear();

    SettingsService settings;
    settings.setValue(QStringLiteral("harperIgnoredRules"), QStringList());
    Q_EMIT blockMatchesUpdated(QVector<int>());
}

QSet<QString> HarperChecker::ignoredRules() const { return _ignoredRules; }

void HarperChecker::ignoreWord(const QString &word) {
    if (word.isEmpty()) {
        return;
    }

    _ignoredWords.insert(word.toLower());

    auto *spellChecker = QOwnSpellChecker::instance();
    if (spellChecker != nullptr) {
        spellChecker->addWordToDictionary(word);
    }

    SettingsService settings;
    settings.setValue(QStringLiteral("harperIgnoredWords"), QStringList(_ignoredWords.values()));
    Q_EMIT blockMatchesUpdated(QVector<int>());
}

bool HarperChecker::isWordIgnored(const QString &word) const {
    return _ignoredWords.contains(word.toLower());
}

void HarperChecker::clearIgnoredWords() {
    _ignoredWords.clear();

    SettingsService settings;
    settings.setValue(QStringLiteral("harperIgnoredWords"), QStringList());
    Q_EMIT blockMatchesUpdated(QVector<int>());
}

QSet<QString> HarperChecker::ignoredWords() const { return _ignoredWords; }

QStringList HarperChecker::enabledLinters() const { return _enabledLinters; }

void HarperChecker::readSettings() {
    SettingsService settings;
    _enabled = settings.value(QStringLiteral("harperEnabled"), false).toBool();
    _transportMode = settings.value(QStringLiteral("harperTransportMode"), 0).toInt();
    _binaryPath = settings.value(QStringLiteral("harperBinaryPath"), QStringLiteral("harper-ls"))
                      .toString()
                      .trimmed();
    _tcpAddress = settings.value(QStringLiteral("harperTcpAddress"), QStringLiteral("127.0.0.1"))
                      .toString()
                      .trimmed();
    _tcpPort = settings.value(QStringLiteral("harperTcpPort"), 4000).toInt();
    _dialect = settings.value(QStringLiteral("harperDialect"), QStringLiteral("American"))
                   .toString()
                   .trimmed();
    _checkDelayMs = settings.value(QStringLiteral("harperCheckDelay"), 1500).toInt();
    _timeoutMs = settings.value(QStringLiteral("harperTimeout"), 5000).toInt();
    _enabledLinters =
        settings
            .value(QStringLiteral("harperEnabledLinters"),
                   QStringList() << QStringLiteral("SpellCheck") << QStringLiteral("AnA")
                                 << QStringLiteral("SentenceCapitalization")
                                 << QStringLiteral("RepeatedWords")
                                 << QStringLiteral("LongSentences") << QStringLiteral("Spaces")
                                 << QStringLiteral("QuoteSpacing")
                                 << QStringLiteral("NoFrenchSpaces")
                                 << QStringLiteral("WrongApostrophe")
                                 << QStringLiteral("CorrectNumberSuffix")
                                 << QStringLiteral("UnclosedQuotes"))
            .toStringList();

    const QStringList ignoredRulesList =
        settings.value(QStringLiteral("harperIgnoredRules")).toStringList();
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    _ignoredRules = QSet<QString>(ignoredRulesList.begin(), ignoredRulesList.end());
#else
    _ignoredRules.clear();
    for (const QString &ignoredRule : ignoredRulesList) {
        _ignoredRules.insert(ignoredRule);
    }
#endif
    _ignoredRules.remove(QString());

    const QStringList ignoredWordsList =
        settings.value(QStringLiteral("harperIgnoredWords")).toStringList();
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

bool HarperChecker::shouldCheckBlock(const QTextBlock &block) const {
    if (!block.isValid() || !block.isVisible()) {
        return false;
    }

    if (block.text().trimmed().isEmpty()) {
        return false;
    }

    const int state = block.userState();
    if ((state == MarkdownHighlighter::HeadlineEnd) || (state == MarkdownHighlighter::CodeBlock) ||
        (state >= MarkdownHighlighter::CodeCpp)) {
        return false;
    }

    return true;
}

QVector<QTextBlock> HarperChecker::visibleBlocks() const {
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
    while (block.isValid() && (block.blockNumber() <= lastBlockNumber)) {
        if (shouldCheckBlock(block)) {
            blocks.append(block);
        }

        block = block.next();
    }

    return blocks;
}

void HarperChecker::handleTextChanged() { scheduleCheck(); }

void HarperChecker::handleRequestFinished(int requestId, const QVector<HarperMatch> &matches) {
    const PendingRequestContext context = _pendingRequests.take(requestId);
    if (_pendingRequestId == requestId) {
        _pendingRequestId = 0;
    }

    if ((context.requestId == 0) || (context.document == nullptr) ||
        (context.document != _document) || (_document == nullptr)) {
        return;
    }

    const QString currentRevisionHash = QString::fromLatin1(
        QCryptographicHash::hash(_document->toPlainText().toUtf8(), QCryptographicHash::Sha1)
            .toHex());
    if (currentRevisionHash != context.documentRevisionHash) {
        return;
    }

    _warningShown = false;
    if (!_available) {
        _available = true;
        Q_EMIT availabilityChanged(true);
    }
    storeMatches(context, matches);
}

void HarperChecker::handleRequestError(int requestId, const QString &errorMessage) {
    _pendingRequests.remove(requestId);
    if (_pendingRequestId == requestId) {
        _pendingRequestId = 0;
    }

    if (_available) {
        _available = false;
        Q_EMIT availabilityChanged(false);
    }

    if (!_warningShown) {
        _warningShown = true;
        if (auto *mainWindow = MainWindow::instance()) {
            mainWindow->showStatusBarMessage(tr("Harper is unavailable: %1").arg(errorMessage),
                                             QStringLiteral("⚠️"), 5000);
        }
        Q_EMIT connectionError(errorMessage);
    }
}

void HarperChecker::storeMatches(const PendingRequestContext &context,
                                 const QVector<HarperMatch> &matches) {
    QHash<int, QVector<BlockMatch>> perBlockMatches;
    perBlockMatches.reserve(context.blockNumbers.size());

    for (const HarperMatch &match : matches) {
        for (int i = 0; i < context.blockNumbers.size(); ++i) {
            const int blockStart = context.blockStartOffsets.at(i);
            const int nextBlockStart = (i + 1 < context.blockStartOffsets.size())
                                           ? context.blockStartOffsets.at(i + 1)
                                           : context.submittedText.size();
            const int blockEnd = nextBlockStart - 1;
            if ((match.offset < blockStart) || (match.offset >= blockEnd)) {
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

void HarperChecker::cleanupExpiredCache() {
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

void HarperChecker::enforceCacheLimit() {
    while (_cache.size() > _cacheLimit) {
        int oldestBlock = -1;
        QDateTime oldestAccess = QDateTime::currentDateTimeUtc();
        for (auto it = _cache.constBegin(); it != _cache.constEnd(); ++it) {
            if ((oldestBlock < 0) || (it.value().lastAccessed < oldestAccess)) {
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

QString HarperChecker::blockHash(const QString &text) const {
    return QString::fromLatin1(
        QCryptographicHash::hash(text.toUtf8(), QCryptographicHash::Sha1).toHex());
}

#endif
