#ifdef LANGUAGETOOL_ENABLED

#include "services/languagetoolclient.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QUrlQuery>

LanguageToolClient::LanguageToolClient(QObject *parent) : QObject(parent) {
    _networkAccessManager = new QNetworkAccessManager(this);
}

int LanguageToolClient::checkText(const RequestOptions &options) {
    if (options.requestId <= 0) {
        return 0;
    }

    cancelRequest(options.requestId);

    QUrl url = buildCheckUrl(options.serverUrl);
    if (!url.isValid()) {
        Q_EMIT checkError(options.requestId, tr("Invalid LanguageTool server URL"));
        return options.requestId;
    }

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QStringLiteral("application/x-www-form-urlencoded"));

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("text"), options.text);
    query.addQueryItem(QStringLiteral("language"),
                       options.language.isEmpty() ? QStringLiteral("auto") : options.language);

    if (!options.apiKey.trimmed().isEmpty()) {
        query.addQueryItem(QStringLiteral("apiKey"), options.apiKey.trimmed());
    }

    if (!options.enabledCategories.isEmpty()) {
        query.addQueryItem(QStringLiteral("enabledCategories"),
                           options.enabledCategories.join(QLatin1Char(',')));
    }

    if (!options.disabledCategories.isEmpty()) {
        query.addQueryItem(QStringLiteral("disabledCategories"),
                           options.disabledCategories.join(QLatin1Char(',')));
    }

    QNetworkReply *reply =
        _networkAccessManager->post(request, query.toString(QUrl::FullyEncoded).toUtf8());
    auto *timeoutTimer = new QTimer(this);
    timeoutTimer->setSingleShot(true);
    timeoutTimer->setInterval(options.timeoutMs);

    RequestState state;
    state.requestId = options.requestId;
    state.reply = reply;
    state.timeoutTimer = timeoutTimer;
    _requests.insert(options.requestId, state);

    const auto requestId = options.requestId;

    connect(timeoutTimer, &QTimer::timeout, this, [this, requestId]() {
        const auto state = _requests.value(requestId);
        if (state.reply) {
            state.reply->abort();
        }
    });

    connect(reply, &QNetworkReply::finished, this, [this, requestId]() {
        const auto state = _requests.value(requestId);
        QNetworkReply *reply = state.reply;
        if (reply == nullptr) {
            finishRequest(requestId);
            return;
        }

        if (state.timeoutTimer) {
            state.timeoutTimer->stop();
        }

        const QByteArray responseData = reply->readAll();
        const QNetworkReply::NetworkError networkError = reply->error();

        if (networkError != QNetworkReply::NoError) {
            Q_EMIT checkError(requestId, reply->errorString());
            finishRequest(requestId);
            return;
        }

        Q_EMIT checkFinished(requestId, parseMatches(responseData));
        finishRequest(requestId);
    });

    timeoutTimer->start();
    return options.requestId;
}

void LanguageToolClient::cancelRequest(int requestId) {
    const auto state = _requests.take(requestId);
    if (state.timeoutTimer) {
        state.timeoutTimer->stop();
        state.timeoutTimer->deleteLater();
    }

    if (state.reply) {
        state.reply->abort();
        state.reply->deleteLater();
    }
}

void LanguageToolClient::cancelAllRequests() {
    const QList<int> requestIds = _requests.keys();
    for (const int requestId : requestIds) {
        cancelRequest(requestId);
    }
}

void LanguageToolClient::finishRequest(int requestId) {
    const auto state = _requests.take(requestId);
    if (state.timeoutTimer) {
        state.timeoutTimer->deleteLater();
    }

    if (state.reply) {
        state.reply->deleteLater();
    }
}

QUrl LanguageToolClient::buildCheckUrl(const QString &serverUrl) {
    QUrl url(serverUrl.trimmed());
    if (!url.isValid()) {
        return url;
    }

    QString path = url.path();
    if (path.endsWith(QLatin1Char('/'))) {
        path.chop(1);
    }

    if (!path.endsWith(QStringLiteral("/v2/check"))) {
        path += QStringLiteral("/v2/check");
    }

    url.setPath(path);
    return url;
}

QVector<LanguageToolMatch> LanguageToolClient::parseMatches(const QByteArray &responseData) {
    QVector<LanguageToolMatch> matches;

    const QJsonDocument document = QJsonDocument::fromJson(responseData);
    if (!document.isObject()) {
        return matches;
    }

    const QJsonArray matchArray = document.object().value(QStringLiteral("matches")).toArray();
    matches.reserve(matchArray.size());

    for (const QJsonValue &matchValue : matchArray) {
        const QJsonObject object = matchValue.toObject();
        LanguageToolMatch match;
        match.offset = object.value(QStringLiteral("offset")).toInt();
        match.length = object.value(QStringLiteral("length")).toInt();
        match.message = object.value(QStringLiteral("message")).toString();
        match.shortMessage = object.value(QStringLiteral("shortMessage")).toString();

        const QJsonObject contextObject = object.value(QStringLiteral("context")).toObject();
        match.contextText = contextObject.value(QStringLiteral("text")).toString();

        const QJsonObject ruleObject = object.value(QStringLiteral("rule")).toObject();
        match.ruleId = ruleObject.value(QStringLiteral("id")).toString();
        match.ruleUrl = ruleObject.value(QStringLiteral("url")).toString();
        match.ruleCategory = ruleObject.value(QStringLiteral("category"))
                                 .toObject()
                                 .value(QStringLiteral("id"))
                                 .toString();

        const QJsonArray replacementArray = object.value(QStringLiteral("replacements")).toArray();
        for (const QJsonValue &replacementValue : replacementArray) {
            const QString value =
                replacementValue.toObject().value(QStringLiteral("value")).toString();
            if (!value.isEmpty()) {
                match.replacements.append(value);
            }
        }

        matches.append(match);
    }

    return matches;
}

#endif
