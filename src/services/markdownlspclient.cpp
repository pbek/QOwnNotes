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

    QJsonObject formatting;
    formatting.insert(QStringLiteral("dynamicRegistration"), false);

    QJsonObject rangeFormatting;
    rangeFormatting.insert(QStringLiteral("dynamicRegistration"), false);

    QJsonObject codeAction;
    codeAction.insert(QStringLiteral("dynamicRegistration"), false);

    // Declare support for receiving publishDiagnostics notifications
    QJsonObject publishDiagnostics;
    publishDiagnostics.insert(QStringLiteral("relatedInformation"), false);

    QJsonObject textDocument;
    textDocument.insert(QStringLiteral("completion"), completion);
    textDocument.insert(QStringLiteral("formatting"), formatting);
    textDocument.insert(QStringLiteral("rangeFormatting"), rangeFormatting);
    textDocument.insert(QStringLiteral("codeAction"), codeAction);
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
    _completionRequestIds.insert(requestId);
    obj.insert(QStringLiteral("jsonrpc"), QStringLiteral("2.0"));
    obj.insert(QStringLiteral("id"), requestId);
    obj.insert(QStringLiteral("method"), QStringLiteral("textDocument/completion"));
    obj.insert(QStringLiteral("params"), params);
    sendMessage(obj);
    return requestId;
}

int MarkdownLspClient::requestDocumentFormatting(const QString &uri, int tabSize,
                                                 bool insertSpaces) {
    if (!_initialized) {
        return -1;
    }

    QJsonObject doc;
    doc.insert(QStringLiteral("uri"), uri);

    QJsonObject options;
    options.insert(QStringLiteral("tabSize"), tabSize);
    options.insert(QStringLiteral("insertSpaces"), insertSpaces);

    QJsonObject params;
    params.insert(QStringLiteral("textDocument"), doc);
    params.insert(QStringLiteral("options"), options);

    QJsonObject obj;
    const int requestId = _nextRequestId++;
    _formattingUriByRequest.insert(requestId, uri);
    obj.insert(QStringLiteral("jsonrpc"), QStringLiteral("2.0"));
    obj.insert(QStringLiteral("id"), requestId);
    obj.insert(QStringLiteral("method"), QStringLiteral("textDocument/formatting"));
    obj.insert(QStringLiteral("params"), params);
    sendMessage(obj);
    return requestId;
}

int MarkdownLspClient::requestRangeFormatting(const QString &uri, const DiagnosticRange &range,
                                              int tabSize, bool insertSpaces) {
    if (!_initialized) {
        return -1;
    }

    QJsonObject doc;
    doc.insert(QStringLiteral("uri"), uri);

    QJsonObject start;
    start.insert(QStringLiteral("line"), range.startLine);
    start.insert(QStringLiteral("character"), range.startCharacter);

    QJsonObject end;
    end.insert(QStringLiteral("line"), range.endLine);
    end.insert(QStringLiteral("character"), range.endCharacter);

    QJsonObject rangeObject;
    rangeObject.insert(QStringLiteral("start"), start);
    rangeObject.insert(QStringLiteral("end"), end);

    QJsonObject options;
    options.insert(QStringLiteral("tabSize"), tabSize);
    options.insert(QStringLiteral("insertSpaces"), insertSpaces);

    QJsonObject params;
    params.insert(QStringLiteral("textDocument"), doc);
    params.insert(QStringLiteral("range"), rangeObject);
    params.insert(QStringLiteral("options"), options);

    QJsonObject obj;
    const int requestId = _nextRequestId++;
    _rangeFormattingUriByRequest.insert(requestId, uri);
    obj.insert(QStringLiteral("jsonrpc"), QStringLiteral("2.0"));
    obj.insert(QStringLiteral("id"), requestId);
    obj.insert(QStringLiteral("method"), QStringLiteral("textDocument/rangeFormatting"));
    obj.insert(QStringLiteral("params"), params);
    sendMessage(obj);
    return requestId;
}

int MarkdownLspClient::requestCodeActions(const QString &uri, const DiagnosticRange &range,
                                          const QVector<Diagnostic> &diagnostics) {
    if (!_initialized) {
        return -1;
    }

    QJsonObject doc;
    doc.insert(QStringLiteral("uri"), uri);

    QJsonObject start;
    start.insert(QStringLiteral("line"), range.startLine);
    start.insert(QStringLiteral("character"), range.startCharacter);

    QJsonObject end;
    end.insert(QStringLiteral("line"), range.endLine);
    end.insert(QStringLiteral("character"), range.endCharacter);

    QJsonObject rangeObject;
    rangeObject.insert(QStringLiteral("start"), start);
    rangeObject.insert(QStringLiteral("end"), end);

    QJsonArray diagnosticsArray;
    for (const Diagnostic &diagnostic : diagnostics) {
        QJsonObject diagRangeStart;
        diagRangeStart.insert(QStringLiteral("line"), diagnostic.range.startLine);
        diagRangeStart.insert(QStringLiteral("character"), diagnostic.range.startCharacter);

        QJsonObject diagRangeEnd;
        diagRangeEnd.insert(QStringLiteral("line"), diagnostic.range.endLine);
        diagRangeEnd.insert(QStringLiteral("character"), diagnostic.range.endCharacter);

        QJsonObject diagRange;
        diagRange.insert(QStringLiteral("start"), diagRangeStart);
        diagRange.insert(QStringLiteral("end"), diagRangeEnd);

        QJsonObject diagObject;
        diagObject.insert(QStringLiteral("range"), diagRange);
        if (diagnostic.severity > 0) {
            diagObject.insert(QStringLiteral("severity"), diagnostic.severity);
        }
        if (!diagnostic.message.isEmpty()) {
            diagObject.insert(QStringLiteral("message"), diagnostic.message);
        }
        diagnosticsArray.append(diagObject);
    }

    QJsonObject context;
    context.insert(QStringLiteral("diagnostics"), diagnosticsArray);

    QJsonObject params;
    params.insert(QStringLiteral("textDocument"), doc);
    params.insert(QStringLiteral("range"), rangeObject);
    params.insert(QStringLiteral("context"), context);

    QJsonObject obj;
    const int requestId = _nextRequestId++;
    _codeActionUriByRequest.insert(requestId, uri);
    obj.insert(QStringLiteral("jsonrpc"), QStringLiteral("2.0"));
    obj.insert(QStringLiteral("id"), requestId);
    obj.insert(QStringLiteral("method"), QStringLiteral("textDocument/codeAction"));
    obj.insert(QStringLiteral("params"), params);
    sendMessage(obj);
    return requestId;
}

int MarkdownLspClient::executeCommand(const QJsonObject &command) {
    if (!_initialized || command.isEmpty()) {
        return -1;
    }

    if (!command.contains(QStringLiteral("command"))) {
        return -1;
    }

    QJsonObject params = command;

    QJsonObject obj;
    const int requestId = _nextRequestId++;
    obj.insert(QStringLiteral("jsonrpc"), QStringLiteral("2.0"));
    obj.insert(QStringLiteral("id"), requestId);
    obj.insert(QStringLiteral("method"), QStringLiteral("workspace/executeCommand"));
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
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
        const QStringList lines = text.split(QLatin1Char('\n'), QString::SkipEmptyParts);
#else
        const QStringList lines = text.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
#endif
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
        emit serverInitialized();
        sendInitializedNotification();
        flushPendingDocument();
        return;
    }

    if (!object.contains(QStringLiteral("result"))) {
        return;
    }

    const QJsonValue result = object.value(QStringLiteral("result"));

    if (_completionRequestIds.contains(id)) {
        _completionRequestIds.remove(id);
        QStringList items;
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
        return;
    }

    if (_formattingUriByRequest.contains(id)) {
        const QString uri = _formattingUriByRequest.take(id);
        QVector<TextEdit> edits;
        if (result.isArray()) {
            edits = parseTextEdits(result);
        } else if (result.isObject()) {
            edits = parseWorkspaceEdits(result.toObject(), uri);
        }
        emit formattingReceived(id, edits);
        return;
    }

    if (_rangeFormattingUriByRequest.contains(id)) {
        const QString uri = _rangeFormattingUriByRequest.take(id);
        QVector<TextEdit> edits;
        if (result.isArray()) {
            edits = parseTextEdits(result);
        } else if (result.isObject()) {
            edits = parseWorkspaceEdits(result.toObject(), uri);
        }
        emit formattingReceived(id, edits);
        return;
    }

    if (_codeActionUriByRequest.contains(id)) {
        const QString uri = _codeActionUriByRequest.take(id);
        QVector<CodeAction> actions;
        if (result.isArray()) {
            const QJsonArray array = result.toArray();
            actions.reserve(array.size());
            for (const QJsonValue &value : array) {
                const QJsonObject actionObject = value.toObject();
                if (actionObject.isEmpty()) {
                    continue;
                }

                QString title = actionObject.value(QStringLiteral("title")).toString();
                if (title.isEmpty()) {
                    title = actionObject.value(QStringLiteral("command")).toString();
                }

                CodeAction action;
                action.title = title;

                const QJsonValue editValue = actionObject.value(QStringLiteral("edit"));
                if (editValue.isObject()) {
                    action.edits = parseWorkspaceEdits(editValue.toObject(), uri);
                }

                const QJsonValue commandValue = actionObject.value(QStringLiteral("command"));
                if (commandValue.isObject()) {
                    action.command = commandValue.toObject();
                } else if (actionObject.contains(QStringLiteral("command")) &&
                           actionObject.contains(QStringLiteral("arguments"))) {
                    action.command = actionObject;
                }

                if (action.title.isEmpty() && action.command.contains(QStringLiteral("title"))) {
                    action.title = action.command.value(QStringLiteral("title")).toString();
                }

                if (!action.title.isEmpty()) {
                    actions.append(action);
                }
            }
        }

        emit codeActionsReceived(id, actions);
        return;
    }
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

QVector<MarkdownLspClient::TextEdit> MarkdownLspClient::parseTextEdits(
    const QJsonValue &value) const {
    QVector<TextEdit> edits;
    if (!value.isArray()) {
        return edits;
    }

    const QJsonArray array = value.toArray();
    edits.reserve(array.size());
    for (const QJsonValue &itemValue : array) {
        const QJsonObject item = itemValue.toObject();
        if (item.isEmpty()) {
            continue;
        }

        const QJsonObject rangeObject = item.value(QStringLiteral("range")).toObject();
        const QJsonObject startObject = rangeObject.value(QStringLiteral("start")).toObject();
        const QJsonObject endObject = rangeObject.value(QStringLiteral("end")).toObject();

        TextEdit edit;
        edit.range.startLine = startObject.value(QStringLiteral("line")).toInt();
        edit.range.startCharacter = startObject.value(QStringLiteral("character")).toInt();
        edit.range.endLine = endObject.value(QStringLiteral("line")).toInt();
        edit.range.endCharacter = endObject.value(QStringLiteral("character")).toInt();
        edit.newText = item.value(QStringLiteral("newText")).toString();
        edits.append(edit);
    }

    return edits;
}

QVector<MarkdownLspClient::TextEdit> MarkdownLspClient::parseWorkspaceEdits(
    const QJsonObject &workspaceEdit, const QString &uri) const {
    QVector<TextEdit> edits;
    if (workspaceEdit.isEmpty() || uri.isEmpty()) {
        return edits;
    }

    const QJsonValue changesValue = workspaceEdit.value(QStringLiteral("changes"));
    if (changesValue.isObject()) {
        const QJsonObject changesObject = changesValue.toObject();
        const QJsonValue uriEdits = changesObject.value(uri);
        if (uriEdits.isArray()) {
            edits = parseTextEdits(uriEdits);
            if (!edits.isEmpty()) {
                return edits;
            }
        }
    }

    const QJsonValue documentChangesValue = workspaceEdit.value(QStringLiteral("documentChanges"));
    if (documentChangesValue.isArray()) {
        const QJsonArray docChanges = documentChangesValue.toArray();
        for (const QJsonValue &docChangeValue : docChanges) {
            const QJsonObject docChange = docChangeValue.toObject();
            if (docChange.isEmpty()) {
                continue;
            }

            const QJsonObject textDocument =
                docChange.value(QStringLiteral("textDocument")).toObject();
            const QString changeUri = textDocument.value(QStringLiteral("uri")).toString();
            if (changeUri != uri) {
                continue;
            }

            const QJsonValue editValues = docChange.value(QStringLiteral("edits"));
            if (editValues.isArray()) {
                edits = parseTextEdits(editValues);
                return edits;
            }
        }
    }

    return edits;
}
