#pragma once

#include <QByteArray>
#include <QJsonObject>
#include <QObject>
#include <QProcess>
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

   Q_SIGNALS:
    void completionReceived(int requestId, const QStringList &items);
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

    QProcess *_process = nullptr;
    QByteArray _readBuffer;
    QString _command;
    QStringList _arguments;
    int _nextRequestId = 1;
    int _initializeRequestId = -1;
    bool _initialized = false;
    PendingDocument _pendingDocument;
};
