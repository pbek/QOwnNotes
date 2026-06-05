#pragma once

#ifdef HARPER_ENABLED

#include <QDateTime>
#include <QHash>
#include <QObject>
#include <QPointer>
#include <QSet>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QVector>

#include "services/harpertypes.h"

class HarperClient;
class QPlainTextEdit;
class QTimer;

class HarperChecker : public QObject {
    Q_OBJECT

   public:
    struct BlockMatch {
        HarperMatch match;
        int blockNumber = -1;
        int blockOffset = 0;
    };

    static HarperChecker *instance();

    void setTextEdit(QPlainTextEdit *textEdit);
    void clearForTextEdit(QPlainTextEdit *textEdit, bool updateBlocks = true);
    void clearCurrentState(bool updateBlocks = true);
    void invalidateBlock(int blockNumber);
    void scheduleCheck(bool immediate = false);
    void recheckNow();
    bool isEnabled() const;
    bool isAvailable() const;
    QVector<BlockMatch> matchesForBlock(int blockNumber, const QString &blockText) const;
    BlockMatch matchAtPosition(const QTextCursor &cursor, int positionInBlock) const;
    void ignoreRule(const QString &ruleId);
    bool isRuleIgnored(const QString &ruleId) const;
    void clearIgnoredRules();
    QSet<QString> ignoredRules() const;
    void ignoreWord(const QString &word);
    bool isWordIgnored(const QString &word) const;
    void clearIgnoredWords();
    QSet<QString> ignoredWords() const;
    QStringList enabledLinters() const;

   Q_SIGNALS:
    void blockMatchesUpdated(QVector<int> blockNumbers);
    void availabilityChanged(bool available);
    void connectionError(QString errorMessage);

   private:
    struct CachedBlockResult {
        QString textHash;
        QVector<BlockMatch> matches;
        QDateTime createdAt;
        QDateTime lastAccessed;
    };

    struct PendingRequestContext {
        int requestId = 0;
        QString documentRevisionHash;
        QString submittedText;
        QTextDocument *document = nullptr;
        QVector<int> blockNumbers;
        QVector<int> blockStartOffsets;
        QHash<int, QString> blockHashes;
    };

    HarperChecker();

    void readSettings();
    bool shouldCheckBlock(const QTextBlock &block) const;
    QVector<QTextBlock> visibleBlocks() const;
    void handleTextChanged();
    void handleRequestFinished(int requestId, const QVector<HarperMatch> &matches);
    void handleRequestError(int requestId, const QString &errorMessage);
    void storeMatches(const PendingRequestContext &context, const QVector<HarperMatch> &matches);
    void cleanupExpiredCache();
    void enforceCacheLimit();
    QString blockHash(const QString &text) const;

    QPointer<QPlainTextEdit> _textEdit;
    QPointer<QTextDocument> _document;
    HarperClient *_client = nullptr;
    QTimer *_debounceTimer = nullptr;
    int _nextRequestId = 1;
    int _pendingRequestId = 0;
    int _cacheTtlSeconds = 300;
    int _cacheLimit = 500;
    bool _enabled = false;
    bool _available = false;
    bool _warningShown = false;
    int _transportMode = 0;
    QString _binaryPath;
    QString _tcpAddress;
    int _tcpPort = 4000;
    QString _dialect;
    QStringList _enabledLinters;
    int _checkDelayMs = 1500;
    int _timeoutMs = 5000;
    QSet<QString> _ignoredRules;
    QSet<QString> _ignoredWords;
    QHash<int, CachedBlockResult> _cache;
    QHash<int, PendingRequestContext> _pendingRequests;
};

#endif
