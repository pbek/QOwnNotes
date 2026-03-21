#pragma once

#ifdef LANGUAGETOOL_ENABLED

#include <QDateTime>
#include <QHash>
#include <QObject>
#include <QPointer>
#include <QSet>
#include <QTextBlock>
#include <QTextDocument>
#include <QVector>

#include "services/languagetooltypes.h"

class LanguageToolClient;
class QPlainTextEdit;
class QTimer;

class LanguageToolChecker : public QObject {
    Q_OBJECT

   public:
    struct BlockMatch {
        LanguageToolMatch match;
        int blockNumber = -1;
        int blockOffset = 0;
    };

    static LanguageToolChecker *instance();

    void setTextEdit(QPlainTextEdit *textEdit);
    void clearForTextEdit(QPlainTextEdit *textEdit);
    void clearCurrentState();
    void invalidateBlock(int blockNumber);
    void scheduleCheck(bool immediate = false);
    void recheckNow();
    bool isEnabled() const;
    bool hasWarnedAboutConnection() const;
    QVector<BlockMatch> matchesForBlock(int blockNumber, const QString &blockText) const;
    BlockMatch matchAtPosition(const QTextCursor &cursor, int positionInBlock) const;
    bool hasTypoMatchCovering(int blockNumber, int start, int length,
                              const QString &blockText) const;
    void ignoreRule(const QString &ruleId);
    bool isRuleIgnored(const QString &ruleId) const;
    QStringList enabledCategories() const;

   signals:
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

    LanguageToolChecker();

    void readSettings();
    bool shouldCheckBlock(const QTextBlock &block) const;
    QVector<QTextBlock> visibleBlocks() const;
    void handleTextChanged();
    void handleRequestFinished(int requestId, const QVector<LanguageToolMatch> &matches);
    void handleRequestError(int requestId, const QString &errorMessage);
    void storeMatches(const PendingRequestContext &context,
                      const QVector<LanguageToolMatch> &matches);
    void cleanupExpiredCache();
    void enforceCacheLimit();
    QString blockHash(const QString &text) const;
    QPointer<QPlainTextEdit> _textEdit;
    QPointer<QTextDocument> _document;
    LanguageToolClient *_client = nullptr;
    QTimer *_debounceTimer = nullptr;
    int _nextRequestId = 1;
    int _pendingRequestId = 0;
    int _cacheTtlSeconds = 300;
    int _cacheLimit = 500;
    bool _enabled = false;
    bool _warningShown = false;
    QString _serverUrl;
    QString _language;
    QString _apiKey;
    QStringList _enabledCategories;
    QStringList _disabledCategories;
    int _checkDelayMs = 1500;
    int _timeoutMs = 5000;
    QSet<QString> _ignoredRules;
    QHash<int, CachedBlockResult> _cache;
    QHash<int, PendingRequestContext> _pendingRequests;
};

#endif
