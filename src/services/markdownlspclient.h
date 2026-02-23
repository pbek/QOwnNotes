#pragma once

#include <QByteArray>
#include <QHash>
#include <QJsonObject>
#include <QObject>
#include <QProcess>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QVector>

class MarkdownLspClient : public QObject {
    Q_OBJECT

   public:
    struct DiagnosticRange {
        int startLine = 0;
        int startCharacter = 0;
        int endLine = 0;
        int endCharacter = 0;
    };

    struct TextEdit {
        DiagnosticRange range;
        QString newText;
    };

    struct CodeAction {
        QString title;
        QVector<TextEdit> edits;
        QJsonObject command;

        bool hasEdits() const { return !edits.isEmpty(); }
        bool hasCommand() const { return !command.isEmpty(); }
    };

    struct Diagnostic {
        DiagnosticRange range;
        int severity = 0;
        QString message;
    };

    explicit MarkdownLspClient(QObject *parent = nullptr);
    ~MarkdownLspClient() override;

    void setServerCommand(const QString &command, const QStringList &arguments);
    bool start();
    void shutdown();
    bool isRunning() const;

    void initialize(const QString &rootPath, const QString &clientName,
                    const QString &clientVersion);

    void didOpen(const QString &uri, const QString &languageId, const QString &text, int version);
    void didChange(const QString &uri, const QString &text, int version);
    void didClose(const QString &uri);

    int requestCompletion(const QString &uri, int line, int character);
    int requestDocumentFormatting(const QString &uri, int tabSize, bool insertSpaces);
    int requestRangeFormatting(const QString &uri, const DiagnosticRange &range, int tabSize,
                               bool insertSpaces);
    int requestCodeActions(const QString &uri, const DiagnosticRange &range,
                           const QVector<Diagnostic> &diagnostics);
    int executeCommand(const QJsonObject &command);

   Q_SIGNALS:
    void completionReceived(int requestId, const QStringList &items);
    void formattingReceived(int requestId, const QVector<TextEdit> &edits);
    void codeActionsReceived(int requestId, const QVector<CodeAction> &actions);
    void diagnosticsReceived(const QString &uri, const QVector<Diagnostic> &diagnostics);
    void errorMessage(const QString &message);
    void serverInitialized();

   private Q_SLOTS:
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();

   private:
    struct PendingDocument {
        QString uri;
        QString languageId;
        QString text;
        int version = 0;
        bool valid = false;
    };

    void sendMessage(const QJsonObject &object);
    void parseMessages();
    void handleResponse(const QJsonObject &object);
    void handleNotification(const QJsonObject &object);
    void sendInitializedNotification();
    void flushPendingDocument();
    QVector<TextEdit> parseTextEdits(const QJsonValue &value) const;
    QVector<TextEdit> parseWorkspaceEdits(const QJsonObject &workspaceEdit,
                                          const QString &uri) const;

    QProcess *_process = nullptr;
    QByteArray _readBuffer;
    QString _command;
    QStringList _arguments;
    int _nextRequestId = 1;
    int _initializeRequestId = -1;
    bool _initialized = false;
    PendingDocument _pendingDocument;
    QSet<int> _completionRequestIds;
    QHash<int, QString> _codeActionUriByRequest;
    QHash<int, QString> _formattingUriByRequest;
    QHash<int, QString> _rangeFormattingUriByRequest;
};
