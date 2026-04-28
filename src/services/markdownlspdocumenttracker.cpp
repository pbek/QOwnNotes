#include "markdownlspdocumenttracker.h"

#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>

#include "markdownlspclient.h"

MarkdownLspDocumentTracker::MarkdownLspDocumentTracker(QObject *parent) : QObject(parent) {
    _debounceTimer.setSingleShot(true);
    _debounceTimer.setInterval(200);
    connect(&_debounceTimer, &QTimer::timeout, this,
            &MarkdownLspDocumentTracker::onDebounceTimeout);
}

void MarkdownLspDocumentTracker::setClient(MarkdownLspClient *client) { _client = client; }

void MarkdownLspDocumentTracker::setDocument(QTextDocument *document) {
    // Disconnect from old document
    if (_document) {
        disconnect(_document, &QTextDocument::contentsChange, this,
                   &MarkdownLspDocumentTracker::onContentsChange);
    }

    _document = document;

    if (_document) {
        connect(_document, &QTextDocument::contentsChange, this,
                &MarkdownLspDocumentTracker::onContentsChange);
    }
}

void MarkdownLspDocumentTracker::setDebounceInterval(int ms) { _debounceTimer.setInterval(ms); }

void MarkdownLspDocumentTracker::setSyncKind(SyncKind kind) { _syncKind = kind; }

void MarkdownLspDocumentTracker::open(const QString &uri, const QString &text) {
    if (!_client) {
        return;
    }

    // Close previous document if any
    if (_isOpen && !_uri.isEmpty()) {
        _client->didClose(_uri);
    }

    _uri = uri;
    _version = 1;
    _isOpen = true;
    _pendingChanges.clear();
    _pendingFullSync = false;
    _suppressed = false;

    _client->didOpen(_uri, QStringLiteral("markdown"), text, _version);

    // Build the initial line-lengths snapshot from the provided text
    _lineLengths.clear();
    int lineStart = 0;
    for (int i = 0; i < text.size(); ++i) {
        if (text.at(i) == QLatin1Char('\n')) {
            // Length includes the newline character itself
            _lineLengths.append(i - lineStart + 1);
            lineStart = i + 1;
        }
    }
    // Last line (may not end with newline)
    _lineLengths.append(text.size() - lineStart);
}

void MarkdownLspDocumentTracker::close() {
    if (_client && _isOpen && !_uri.isEmpty()) {
        _client->didClose(_uri);
    }

    _debounceTimer.stop();
    _isOpen = false;
    _uri.clear();
    _version = 0;
    _pendingChanges.clear();
    _pendingFullSync = false;
    _lineLengths.clear();
}

bool MarkdownLspDocumentTracker::isOpen() const { return _isOpen && !_uri.isEmpty(); }

const QString &MarkdownLspDocumentTracker::uri() const { return _uri; }

int MarkdownLspDocumentTracker::version() const { return _version; }

void MarkdownLspDocumentTracker::flushFullSync() {
    if (!_client || !_isOpen || !_document) {
        return;
    }

    _debounceTimer.stop();
    _pendingChanges.clear();
    _pendingFullSync = false;

    _version++;
    const QString text = _document->toPlainText();
    _client->didChange(_uri, text, _version);

    // Rebuild line-lengths from scratch after a full sync
    rebuildLineLengths();

    emit changeSent();
}

void MarkdownLspDocumentTracker::setSuppressed(bool suppressed) { _suppressed = suppressed; }

// ─── contentsChange slot ──────────────────────────────────────────────
//
// This is called by QTextDocument on EVERY edit (keystroke, paste, undo).
// We must keep it as cheap as possible.  The document is already in the
// NEW state when this fires.

void MarkdownLspDocumentTracker::onContentsChange(int position, int charsRemoved, int charsAdded) {
    if (!_isOpen || _suppressed || !_client) {
        return;
    }

    if (_syncKind == SyncIncremental && !_pendingFullSync) {
        // Compute the old range end from our line-lengths snapshot, which
        // still reflects the document state *before* this edit.
        IncrementalChange change;

        // Start position — same in both old and new document
        positionToLineCharacterFromSnapshot(position, change.startLine, change.startCharacter);

        // End position of the OLD range — use the snapshot
        const int oldEnd = position + charsRemoved;
        positionToLineCharacterFromSnapshot(oldEnd, change.endLine, change.endCharacter);

        // Read the NEW text from the document (O(charsAdded))
        if (charsAdded > 0) {
            change.text.reserve(charsAdded);
            for (int i = 0; i < charsAdded; ++i) {
                const QChar ch = _document->characterAt(position + i);
                // QTextDocument uses U+2029 (paragraph separator) for newlines
                if (ch == QChar::ParagraphSeparator) {
                    change.text += QLatin1Char('\n');
                } else {
                    change.text += ch;
                }
            }
        }

        _pendingChanges.append(change);
    } else {
        // Full sync mode — just flag that we need a full send
        _pendingFullSync = true;
    }

    // Update the line-lengths snapshot to reflect the new document state
    rebuildLineLengths();

    _debounceTimer.start();
}

// ─── debounce timeout ─────────────────────────────────────────────────

void MarkdownLspDocumentTracker::onDebounceTimeout() {
    if (!_isOpen || !_client || !_document) {
        return;
    }

    _version++;

    if (_syncKind == SyncIncremental && !_pendingFullSync && !_pendingChanges.isEmpty()) {
        _client->didChangeIncremental(_uri, _pendingChanges, _version);
    } else {
        // Full sync fallback
        const QString text = _document->toPlainText();
        _client->didChange(_uri, text, _version);

        // Rebuild after full sync to stay in sync
        rebuildLineLengths();
    }

    _pendingChanges.clear();
    _pendingFullSync = false;

    emit changeSent();
}

// ─── position conversion helpers ──────────────────────────────────────

void MarkdownLspDocumentTracker::positionToLineCharacterFromSnapshot(int position, int &line,
                                                                     int &character) const {
    int offset = 0;
    for (int i = 0; i < _lineLengths.size(); ++i) {
        const int lineLen = _lineLengths.at(i);
        if (offset + lineLen > position) {
            line = i;
            character = position - offset;
            return;
        }
        offset += lineLen;
    }

    // Position is at or past the end of the document
    line = qMax(0, _lineLengths.size() - 1);
    character = position - offset + (_lineLengths.isEmpty() ? 0 : _lineLengths.last());
}

void MarkdownLspDocumentTracker::rebuildLineLengths() {
    _lineLengths.clear();
    if (!_document) {
        return;
    }

    QTextBlock block = _document->begin();
    while (block.isValid()) {
        // Block length includes the trailing newline (except the last block)
        _lineLengths.append(block.length());
        block = block.next();
    }
}
