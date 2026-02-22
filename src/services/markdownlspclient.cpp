#include "markdownlspclient.h"

#include <QCoreApplication>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QRegularExpression>
#include <QUrl>

MarkdownLspClient::MarkdownLspClient(QObject *parent)
    : QObject(parent), _process(new QProcess(this)) {
    connect(_process, &QProcess::readyReadStandardOutput, this,
            &MarkdownLspClient::onReadyReadStandardOutput);
    connect(_process, &QProcess::readyReadStandardError, this,
            &MarkdownLspClient::onReadyReadStandardError);
    connect(_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            [this](int, QProcess::ExitStatus) {
                _initialized = false;
                _initializeRequestId = -1;
            });
}

MarkdownLspClient::~MarkdownLspClient() { shutdown(); }

void MarkdownLspClient::setServerCommand(const QString &command, const QStringList &arguments) {
    _command = command;
    _arguments = arguments;
}

bool MarkdownLspClient::start() {
    if (_command.isEmpty()) {
        emit errorMessage(tr("Markdown LSP server command is empty"));
        return false;
    }

    if (_process->state() != QProcess::NotRunning) {
        return true;
    }

    _readBuffer.clear();
    _initialized = false;
    _initializeRequestId = -1;

    _process->start(_command, _arguments, QIODevice::ReadWrite);
    if (!_process->waitForStarted(2000)) {
        emit errorMessage(tr("Failed to start Markdown LSP server: %1").arg(_command));
        return false;
    }

    return true;
}

void MarkdownLspClient::shutdown() {
    if (!_process || _process->state() == QProcess::NotRunning) {
        return;
    }

    if (_initialized) {
        QJsonObject params;
        QJsonObject obj;
        obj.insert(QStringLiteral("jsonrpc"), QStringLiteral("2.0"));
        obj.insert(QStringLiteral("id"), _nextRequestId++);
        obj.insert(QStringLiteral("method"), QStringLiteral("shutdown"));
        obj.insert(QStringLiteral("params"), params);
        sendMessage(obj);

        QJsonObject exitObj;
        exitObj.insert(QStringLiteral("jsonrpc"), QStringLiteral("2.0"));
        exitObj.insert(QStringLiteral("method"), QStringLiteral("exit"));
        sendMessage(exitObj);
    }

    _process->terminate();
    _process->waitForFinished(1500);
    if (_process->state() != QProcess::NotRunning) {
        _process->kill();
    }
}

bool MarkdownLspClient::isRunning() const {
    return _process && _process->state() != QProcess::NotRunning;
}

void MarkdownLspClient::initialize(const QString &rootPath, const QString &clientName,
                                   const QString &clientVersion) {
    if (!isRunning()) {
        return;
    }

    QJsonObject clientInfo;
    clientInfo.insert(QStringLiteral("name"), clientName);
    clientInfo.insert(QStringLiteral("version"), clientVersion);

    QJsonObject completionItem;
    completionItem.insert(QStringLiteral("snippetSupport"), false);

    QJsonObject completion;
    completion.insert(QStringLiteral("completionItem"), completionItem);

    // Declare support for receiving publishDiagnostics notifications
    QJsonObject publishDiagnostics;
    publishDiagnostics.insert(QStringLiteral("relatedInformation"), false);

    QJsonObject textDocument;
    textDocument.insert(QStringLiteral("completion"), completion);
    textDocument.insert(QStringLiteral("publishDiagnostics"), publishDiagnostics);

    QJsonObject capabilities;
    capabilities.insert(QStringLiteral("textDocument"), textDocument);

    QJsonObject params;
    params.insert(QStringLiteral("processId"), QCoreApplication::applicationPid());
    params.insert(QStringLiteral("rootPath"), rootPath);
    params.insert(QStringLiteral("rootUri"), QUrl::fromLocalFile(rootPath).toString());
    params.insert(QStringLiteral("clientInfo"), clientInfo);
    params.insert(QStringLiteral("capabilities"), capabilities);

    QJsonObject obj;
    _initializeRequestId = _nextRequestId++;
    obj.insert(QStringLiteral("jsonrpc"), QStringLiteral("2.0"));
    obj.insert(QStringLiteral("id"), _initializeRequestId);
    obj.insert(QStringLiteral("method"), QStringLiteral("initialize"));
    obj.insert(QStringLiteral("params"), params);
    sendMessage(obj);
}

void MarkdownLspClient::didOpen(const QString &uri, const QString &languageId, const QString &text,
                                int version) {
    if (!_initialized) {
        _pendingDocument.uri = uri;
        _pendingDocument.languageId = languageId;
        _pendingDocument.text = text;
        _pendingDocument.version = version;
        _pendingDocument.valid = true;
        return;
    }

    QJsonObject doc;
    doc.insert(QStringLiteral("uri"), uri);
    doc.insert(QStringLiteral("languageId"), languageId);
    doc.insert(QStringLiteral("version"), version);
    doc.insert(QStringLiteral("text"), text);

    QJsonObject params;
    params.insert(QStringLiteral("textDocument"), doc);

    QJsonObject obj;
    obj.insert(QStringLiteral("jsonrpc"), QStringLiteral("2.0"));
    obj.insert(QStringLiteral("method"), QStringLiteral("textDocument/didOpen"));
    obj.insert(QStringLiteral("params"), params);
    sendMessage(obj);
}

void MarkdownLspClient::didChange(const QString &uri, const QString &text, int version) {
    if (!_initialized) {
        _pendingDocument.uri = uri;
        _pendingDocument.text = text;
        _pendingDocument.version = version;
        _pendingDocument.valid = true;
        return;
    }

    QJsonObject doc;
    doc.insert(QStringLiteral("uri"), uri);
    doc.insert(QStringLiteral("version"), version);

    QJsonObject change;
    change.insert(QStringLiteral("text"), text);
    QJsonArray changes;
    changes.append(change);

    QJsonObject params;
    params.insert(QStringLiteral("textDocument"), doc);
    params.insert(QStringLiteral("contentChanges"), changes);

    QJsonObject obj;
    obj.insert(QStringLiteral("jsonrpc"), QStringLiteral("2.0"));
    obj.insert(QStringLiteral("method"), QStringLiteral("textDocument/didChange"));
    obj.insert(QStringLiteral("params"), params);
    sendMessage(obj);
}

void MarkdownLspClient::didClose(const QString &uri) {
    if (!isRunning()) {
        return;
    }

    QJsonObject doc;
    doc.insert(QStringLiteral("uri"), uri);

    QJsonObject params;
    params.insert(QStringLiteral("textDocument"), doc);

    QJsonObject obj;
    obj.insert(QStringLiteral("jsonrpc"), QStringLiteral("2.0"));
    obj.insert(QStringLiteral("method"), QStringLiteral("textDocument/didClose"));
    obj.insert(QStringLiteral("params"), params);
    sendMessage(obj);
}

int MarkdownLspClient::requestCompletion(const QString &uri, int line, int character) {
    if (!_initialized) {
        return -1;
    }

    QJsonObject position;
    position.insert(QStringLiteral("line"), line);
    position.insert(QStringLiteral("character"), character);

    QJsonObject doc;
    doc.insert(QStringLiteral("uri"), uri);

    QJsonObject params;
    params.insert(QStringLiteral("textDocument"), doc);
    params.insert(QStringLiteral("position"), position);

    QJsonObject obj;
    const int requestId = _nextRequestId++;
    obj.insert(QStringLiteral("jsonrpc"), QStringLiteral("2.0"));
    obj.insert(QStringLiteral("id"), requestId);
    obj.insert(QStringLiteral("method"), QStringLiteral("textDocument/completion"));
    obj.insert(QStringLiteral("params"), params);
    sendMessage(obj);
    return requestId;
}

void MarkdownLspClient::sendMessage(const QJsonObject &object) {
    if (!isRunning()) {
        return;
    }

    const QJsonDocument doc(object);
    const QByteArray payload = doc.toJson(QJsonDocument::Compact);
    const QByteArray header = QByteArray("Content-Length: ") + QByteArray::number(payload.size()) +
                              QByteArray("\r\n\r\n");
    _process->write(header);
    _process->write(payload);
}

void MarkdownLspClient::onReadyReadStandardOutput() {
    _readBuffer.append(_process->readAllStandardOutput());
    parseMessages();
}

void MarkdownLspClient::onReadyReadStandardError() {
    const QByteArray data = _process->readAllStandardError();
    if (!data.isEmpty()) {
        const QString text = QString::fromUtf8(data);
        const QStringList lines = text.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
        static const QRegularExpression errorPattern(QStringLiteral("\\bERR\\b|\\bERROR\\b"),
                                                     QRegularExpression::CaseInsensitiveOption);
        for (const QString &line : lines) {
            const QString trimmed = line.trimmed();
            if (trimmed.isEmpty()) {
                continue;
            }
            if (errorPattern.match(trimmed).hasMatch()) {
                emit errorMessage(trimmed);
            }
        }
    }
}

void MarkdownLspClient::parseMessages() {
    while (true) {
        const int headerEnd = _readBuffer.indexOf("\r\n\r\n");
        if (headerEnd == -1) {
            return;
        }

        const QByteArray header = _readBuffer.left(headerEnd);
        const QByteArray bodyStart = _readBuffer.mid(headerEnd + 4);

        QRegularExpression contentLengthRe(QStringLiteral("Content-Length: (\\d+)"));
        QRegularExpressionMatch match = contentLengthRe.match(QString::fromLatin1(header));
        if (!match.hasMatch()) {
            _readBuffer.remove(0, headerEnd + 4);
            continue;
        }

        const int contentLength = match.captured(1).toInt();
        if (bodyStart.size() < contentLength) {
            return;
        }

        const QByteArray body = bodyStart.left(contentLength);
        _readBuffer.remove(0, headerEnd + 4 + contentLength);

        QJsonParseError parseError;
        const QJsonDocument doc = QJsonDocument::fromJson(body, &parseError);
        if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
            continue;
        }

        const QJsonObject object = doc.object();
        if (object.contains(QStringLiteral("id"))) {
            handleResponse(object);
        } else {
            handleNotification(object);
        }
    }
}

void MarkdownLspClient::handleResponse(const QJsonObject &object) {
    const int id = object.value(QStringLiteral("id")).toInt(-1);
    if (id == _initializeRequestId) {
        _initialized = true;
        _initializeRequestId = -1;
        sendInitializedNotification();
        flushPendingDocument();
        return;
    }

    if (!object.contains(QStringLiteral("result"))) {
        return;
    }

    QStringList items;
    const QJsonValue result = object.value(QStringLiteral("result"));
    if (result.isObject()) {
        const QJsonObject resultObj = result.toObject();
        const QJsonValue list = resultObj.value(QStringLiteral("items"));
        if (list.isArray()) {
            const QJsonArray array = list.toArray();
            for (const QJsonValue &value : array) {
                const QJsonObject item = value.toObject();
                const QString label = item.value(QStringLiteral("label")).toString();
                if (!label.isEmpty()) {
                    items.append(label);
                }
            }
        }
    } else if (result.isArray()) {
        const QJsonArray array = result.toArray();
        for (const QJsonValue &value : array) {
            const QJsonObject item = value.toObject();
            const QString label = item.value(QStringLiteral("label")).toString();
            if (!label.isEmpty()) {
                items.append(label);
            }
        }
    }

    emit completionReceived(id, items);
}

void MarkdownLspClient::handleNotification(const QJsonObject &object) {
    const QString method = object.value(QStringLiteral("method")).toString();
    if (method != QStringLiteral("textDocument/publishDiagnostics")) {
        return;
    }

    const QJsonObject params = object.value(QStringLiteral("params")).toObject();
    const QString uri = params.value(QStringLiteral("uri")).toString();
    const QJsonArray diagnosticsArray = params.value(QStringLiteral("diagnostics")).toArray();
    if (uri.isEmpty()) {
        return;
    }

    QVector<MarkdownLspClient::Diagnostic> diagnostics;
    diagnostics.reserve(diagnosticsArray.size());
    for (const QJsonValue &diagnosticValue : diagnosticsArray) {
        const QJsonObject diagnosticObject = diagnosticValue.toObject();
        const QJsonObject rangeObject = diagnosticObject.value(QStringLiteral("range")).toObject();
        const QJsonObject startObject = rangeObject.value(QStringLiteral("start")).toObject();
        const QJsonObject endObject = rangeObject.value(QStringLiteral("end")).toObject();

        MarkdownLspClient::Diagnostic diagnostic;
        diagnostic.range.startLine = startObject.value(QStringLiteral("line")).toInt();
        diagnostic.range.startCharacter = startObject.value(QStringLiteral("character")).toInt();
        diagnostic.range.endLine = endObject.value(QStringLiteral("line")).toInt();
        diagnostic.range.endCharacter = endObject.value(QStringLiteral("character")).toInt();
        diagnostic.severity = diagnosticObject.value(QStringLiteral("severity")).toInt();
        diagnostic.message = diagnosticObject.value(QStringLiteral("message")).toString();
        diagnostics.push_back(diagnostic);
    }

    emit diagnosticsReceived(uri, diagnostics);
}

void MarkdownLspClient::sendInitializedNotification() {
    QJsonObject params;
    QJsonObject obj;
    obj.insert(QStringLiteral("jsonrpc"), QStringLiteral("2.0"));
    obj.insert(QStringLiteral("method"), QStringLiteral("initialized"));
    obj.insert(QStringLiteral("params"), params);
    sendMessage(obj);
}

void MarkdownLspClient::flushPendingDocument() {
    if (!_pendingDocument.valid) {
        return;
    }

    didOpen(_pendingDocument.uri, _pendingDocument.languageId, _pendingDocument.text,
            _pendingDocument.version);
    _pendingDocument.valid = false;
}
