#pragma once

#include <QObject>
#include <QString>
#include <QTimer>
#include <QVector>

#include "markdownlspclient.h"

class QTextDocument;

/**
 * Tracks edits to a QTextDocument via contentsChange() and produces LSP
 * didChange payloads — either incremental (TextDocumentSyncKind=2) or
 * full-text (TextDocumentSyncKind=1) — without ever calling toPlainText()
 * on the per-keystroke hot path.
 *
 * Lifecycle:
 *   1. Construct with a parent and debounce interval.
 *   2. Call open() when a note is loaded.
 *   3. The tracker connects to contentsChange automatically.
 *   4. Call close() when switching away from the note.
 */
class MarkdownLspDocumentTracker : public QObject {
    Q_OBJECT

   public:
    /// LSP TextDocumentSyncKind
    enum SyncKind { SyncNone = 0, SyncFull = 1, SyncIncremental = 2 };

    explicit MarkdownLspDocumentTracker(QObject *parent = nullptr);

    void setClient(MarkdownLspClient *client);
    void setDocument(QTextDocument *document);
    void setDebounceInterval(int ms);
    void setSyncKind(SyncKind kind);

    void open(const QString &uri, const QString &text);
    void close();
    bool isOpen() const;
    const QString &uri() const;
    int version() const;

    /// Force an immediate full sync (e.g. after applying LSP edits).
    void flushFullSync();

    /// Temporarily suppress change tracking (e.g. while applying LSP edits).
    void setSuppressed(bool suppressed);

   Q_SIGNALS:
    /// Emitted when the debounce timer fires and a didChange was sent.
    void changeSent();

   private Q_SLOTS:
    void onContentsChange(int position, int charsRemoved, int charsAdded);
    void onDebounceTimeout();

   private:
    using IncrementalChange = MarkdownLspClient::IncrementalChange;

    /// Maintain a line-lengths snapshot so we can resolve old end positions.
    void rebuildLineLengths();
    void positionToLineCharacterFromSnapshot(int position, int &line, int &character) const;

    MarkdownLspClient *_client = nullptr;
    QTextDocument *_document = nullptr;
    QTimer _debounceTimer;
    QString _uri;
    int _version = 0;
    bool _isOpen = false;
    bool _suppressed = false;
    SyncKind _syncKind = SyncFull;

    /// Pending incremental changes accumulated between debounce fires.
    QVector<IncrementalChange> _pendingChanges;
    bool _pendingFullSync = false;

    /// Line-lengths snapshot from the *previous* document state so we can
    /// convert (position + charsRemoved) to old (line, character).
    QVector<int> _lineLengths;
};
