#pragma once

#include <QByteArray>
#include <QJsonObject>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>

class MarkdownLspClient : public QObject {
    Q_OBJECT

   public:
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
    void errorMessage(const QString &message);

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
