#pragma once

#ifdef LANGUAGETOOL_ENABLED

#include <QHash>
#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QUrl>
#include <QVector>

#include "services/languagetooltypes.h"

class QNetworkAccessManager;
class QNetworkReply;

class LanguageToolClient : public QObject {
    Q_OBJECT

   public:
    struct RequestOptions {
        int requestId = 0;
        int timeoutMs = 5000;
        QString text;
        QString language;
        QString serverUrl;
        QString apiKey;
        QStringList enabledCategories;
        QStringList disabledCategories;
    };

    explicit LanguageToolClient(QObject *parent = nullptr);

    int checkText(const RequestOptions &options);
    void cancelRequest(int requestId);
    void cancelAllRequests();

   signals:
    void checkFinished(int requestId, QVector<LanguageToolMatch> matches);
    void checkError(int requestId, QString errorMessage);

   private:
    struct RequestState {
        int requestId = 0;
        QPointer<QNetworkReply> reply;
        QPointer<QTimer> timeoutTimer;
    };

    void finishRequest(int requestId);
    static QUrl buildCheckUrl(const QString &serverUrl);
    static QVector<LanguageToolMatch> parseMatches(const QByteArray &responseData);

    QNetworkAccessManager *_networkAccessManager = nullptr;
    QHash<int, RequestState> _requests;
};

#endif
