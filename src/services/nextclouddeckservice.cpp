#include "nextclouddeckservice.h"

#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimeZone>
#include <QTimer>

#include "entities/cloudconnection.h"
#include "owncloudservice.h"
#include "utils/gui.h"
#include "utils/misc.h"

NextcloudDeckService::NextcloudDeckService(QObject* parent, int cloudConnectionId)
    : QObject(parent) {
    if (cloudConnectionId == -1) {
        cloudConnectionId = CloudConnection::currentCloudConnection().getId();
    }

    this->cloudConnection = CloudConnection::fetch(cloudConnectionId);
    this->serverUrl = this->cloudConnection.getServerUrl();
    this->boardId = this->cloudConnection.getNextcloudDeckBoardId();
    this->stackId = this->cloudConnection.getNextcloudDeckStackId();
}

bool NextcloudDeckService::isEnabledAndValid() {
    return isEnabled() && this->cloudConnection.getNextcloudDeckBoardId() > 0 &&
           this->cloudConnection.getNextcloudDeckStackId() > 0;
}

bool NextcloudDeckService::isEnabled() {
    return OwnCloudService::isOwnCloudSupportEnabled() &&
           this->cloudConnection.getNextcloudDeckEnabled();
}

int NextcloudDeckService::storeCard(const QString& title, const QString& description,
                                    QDateTime* dueDateTime, int cardId) {
    int resultCardId = -1;
    auto* manager = new QNetworkAccessManager();
    QEventLoop loop;
    QTimer timer;

    timer.setSingleShot(true);

    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

    // 10 sec timeout for the request
    timer.start(10000);

    bool isUpdate = (cardId > 0);
    QUrl url;

    if (isUpdate) {
        url = QUrl(serverUrl + "/index.php/apps/deck/cards/" + QString::number(cardId));
    } else {
        url = QUrl(serverUrl + "/index.php/apps/deck/api/v1.1/boards/" +
                   QString::number(this->boardId) + "/stacks/" + QString::number(this->stackId) +
                   "/cards");
    }

    qDebug() << __func__ << " - 'url': " << url;
    qDebug() << __func__ << " - 'isUpdate': " << isUpdate;

    QJsonObject bodyJson;
    bodyJson["title"] = title;
    bodyJson["type"] = "plain";

    if (isUpdate) {
        bodyJson["id"] = cardId;
        bodyJson["stackId"] = this->stackId;
        bodyJson["boardId"] = this->boardId;
        // TODO: What do we set the order to?
        bodyJson["order"] = 0;
    } else {
        bodyJson["order"] = 0;
    }

    if (description != "") {
        bodyJson["description"] = description;
    }

    if (dueDateTime != nullptr) {
        // We need to convert the DateTime to UTC to get around Deck's/Nextcloud's timezone issue
        // see: https://github.com/nextcloud/deck/issues/4764
        bodyJson["duedate"] = dueDateTime->toTimeZone(QTimeZone::utc()).toString(Qt::ISODate);
    }

    qDebug() << __func__ << " - 'bodyJson': " << bodyJson;
    QJsonDocument bodyJsonDoc(bodyJson);

    QNetworkRequest networkRequest = QNetworkRequest(url);
    networkRequest.setHeader(QNetworkRequest::UserAgentHeader,
                             Utils::Misc::friendlyUserAgentString());

#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
    networkRequest.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
#else
    networkRequest.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
#endif

    QByteArray data;
    QNetworkReply* reply;

    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    networkRequest.setRawHeader("OCS-APIRequest", "true");
    addAuthHeader(networkRequest);

    if (isUpdate) {
        reply = manager->put(networkRequest, bodyJsonDoc.toJson());
    } else {
        reply = manager->post(networkRequest, bodyJsonDoc.toJson());
    }

    loop.exec();

    // if we didn't get a timeout let us return the content
    if (timer.isActive()) {
        int returnStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        // only get the data if the status code was "success"
        // see: https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
        if (returnStatusCode >= 200 && returnStatusCode < 300) {
            // get the data from the network reply
            data = reply->readAll();

            QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
            QJsonObject jsonObject = jsonDoc.object();
            resultCardId = jsonObject["id"].toInt();

            // If we're updating, use the original cardId if the response doesn't contain one
            if (isUpdate && resultCardId <= 0) {
                resultCardId = cardId;
            }

            qDebug() << __func__ << " - 'resultCardId': " << resultCardId;
            qDebug() << __func__ << " - 'jsonDoc': " << jsonDoc;
        } else {
            QString errorString = reply->errorString();
            QString operation = isUpdate ? tr("updating") : tr("creating");
            Utils::Gui::warning(nullptr, tr("Error while %1 card").arg(operation),
                                tr("%1 a card failed with status code %2 and message: %3")
                                    .arg(operation.left(1).toUpper() + operation.mid(1))
                                    .arg(QString::number(returnStatusCode), errorString),
                                "nextcloud-deck-create-update-failed");

            qDebug() << __func__ << " - error: " << returnStatusCode;
            qDebug() << __func__ << " - 'errorString': " << errorString;
        }
    }

    reply->deleteLater();
    delete (manager);

    return resultCardId;
}

bool NextcloudDeckService::archiveCard(int cardId) {
    auto* manager = new QNetworkAccessManager();
    QEventLoop loop;
    QTimer timer;

    timer.setSingleShot(true);

    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

    // 10 sec timeout for the request
    timer.start(10000);

    QUrl url =
        QUrl(serverUrl + "/index.php/apps/deck/cards/" + QString::number(cardId) + "/archive");

    QNetworkRequest networkRequest = QNetworkRequest(url);
    networkRequest.setHeader(QNetworkRequest::UserAgentHeader,
                             Utils::Misc::friendlyUserAgentString());

#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
    networkRequest.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
#else
    networkRequest.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
#endif

    networkRequest.setRawHeader("OCS-APIRequest", "true");
    addAuthHeader(networkRequest);

    QNetworkReply* reply = manager->put(networkRequest, QByteArray());

    loop.exec();
    bool result = false;

    if (timer.isActive()) {
        int returnStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (returnStatusCode >= 200 && returnStatusCode < 300) {
            result = true;
        } else {
            QString errorString = reply->errorString();
            Utils::Gui::warning(nullptr, tr("Error while archiving card"),
                                tr("Archiving the card failed with status code %2 and message: %3")
                                    .arg(QString::number(returnStatusCode), errorString),
                                "nextcloud-deck-archive-failed");

            qDebug() << __func__ << " - error: " << returnStatusCode;
            qDebug() << __func__ << " - 'errorString': " << errorString;
        }
    } else {
        qDebug() << __func__ << " - 'timer' timed out";
    }

    reply->deleteLater();
    delete (manager);

    return result;
}

bool NextcloudDeckService::deleteCard(int cardId) {
    auto* manager = new QNetworkAccessManager();
    QEventLoop loop;
    QTimer timer;

    timer.setSingleShot(true);

    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

    // 10 sec timeout for the request
    timer.start(10000);

    QUrl url = QUrl(serverUrl + "/index.php/apps/deck/cards/" + QString::number(cardId));

    QNetworkRequest networkRequest = QNetworkRequest(url);
    networkRequest.setHeader(QNetworkRequest::UserAgentHeader,
                             Utils::Misc::friendlyUserAgentString());

#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
    networkRequest.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
#else
    networkRequest.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
#endif

    networkRequest.setRawHeader("OCS-APIRequest", "true");
    addAuthHeader(networkRequest);

    QNetworkReply* reply = manager->sendCustomRequest(networkRequest, "DELETE");

    loop.exec();
    bool result = false;

    if (timer.isActive()) {
        int returnStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (returnStatusCode >= 200 && returnStatusCode < 300) {
            result = true;
        } else {
            QString errorString = reply->errorString();
            Utils::Gui::warning(nullptr, tr("Error while deleting card"),
                                tr("Deleting the card failed with status code %2 and message: %3")
                                    .arg(QString::number(returnStatusCode), errorString),
                                "nextcloud-deck-delete-failed");

            qDebug() << __func__ << " - error: " << returnStatusCode;
            qDebug() << __func__ << " - 'errorString': " << errorString;
        }
    } else {
        qDebug() << __func__ << " - 'timer' timed out";
    }

    reply->deleteLater();
    delete (manager);

    return result;
}

QString NextcloudDeckService::getCardLinkForId(int cardId) {
    qDebug() << __func__ << " - 'boardId': " << this->boardId;

    return QStringLiteral("%1/apps/deck/#/board/%2/card/%3")
        .arg(this->serverUrl, QString::number(this->boardId), QString::number(cardId));
}

void NextcloudDeckService::addAuthHeader(QNetworkRequest& networkRequest) {
    auto authString =
        cloudConnection.getUsername() + QStringLiteral(":") + cloudConnection.getPassword();
    QByteArray authStringBase64 = authString.toLocal8Bit().toBase64();
    QString headerData = QStringLiteral("Basic ") + authStringBase64;
    networkRequest.setRawHeader("Authorization", headerData.toLocal8Bit());
}

QList<NextcloudDeckService::Board> NextcloudDeckService::getBoards() {
    auto* manager = new QNetworkAccessManager();
    QEventLoop loop;
    QTimer timer;

    timer.setSingleShot(true);

    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

    // 10 sec timeout for the request
    timer.start(10000);

    QUrl url(serverUrl + "/index.php/apps/deck/api/v1.1/boards?details=true");
    qDebug() << __func__ << " - 'url': " << url;

    QNetworkRequest networkRequest = QNetworkRequest(url);
    networkRequest.setHeader(QNetworkRequest::UserAgentHeader,
                             Utils::Misc::friendlyUserAgentString());

#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
    networkRequest.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
#else
    networkRequest.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
#endif

    QByteArray data;
    QNetworkReply* reply;

    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    networkRequest.setRawHeader("OCS-APIRequest", "true");
    addAuthHeader(networkRequest);

    reply = manager->get(networkRequest);

    loop.exec();
    QList<NextcloudDeckService::Board> boards;

    // if we didn't get a timeout let us return the content
    if (timer.isActive()) {
        int returnStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        // only get the data if the status code was "success"
        // see: https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
        if (returnStatusCode >= 200 && returnStatusCode < 300) {
            // get the data from the network reply
            data = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

            if (jsonDoc.isArray()) {
                QJsonArray jsonArray = jsonDoc.array();

                for (auto jsonValue : jsonArray) {
                    QJsonObject object = jsonValue.toObject();
                    int bBoardId = object["id"].toInt();

                    NextcloudDeckService::Board board;
                    board.id = bBoardId;
                    board.title = object["title"].toString();

                    auto stacksJsonValue = object["stacks"];
                    QHash<int, QString> stacks;

                    if (stacksJsonValue.isArray()) {
                        QJsonArray stacksArray = stacksJsonValue.toArray();

                        for (auto stackValue : stacksArray) {
                            QJsonObject stackObject = stackValue.toObject();
                            int bStackId = stackObject["id"].toInt();
                            QString stackTitle = stackObject["title"].toString();

                            stacks[bStackId] = stackTitle;
                        }
                    }

                    board.stacks = stacks;
                    boards.append(board);
                }
            }

        } else {
            QString errorString = reply->errorString();
            Utils::Gui::warning(nullptr, tr("Error while loading boards"),
                                tr("Loading the boards failed with status code %1 and message: %2")
                                    .arg(QString::number(returnStatusCode), errorString),
                                "nextcloud-deck-get-boards-failed");

            qDebug() << __func__ << " - error: " << returnStatusCode;
            qDebug() << __func__ << " - 'errorString': " << errorString;
        }
    }

    reply->deleteLater();
    delete (manager);

    return boards;
}

QHash<int, NextcloudDeckService::Card> NextcloudDeckService::getCards() {
    auto* manager = new QNetworkAccessManager();
    QEventLoop loop;
    QTimer timer;

    timer.setSingleShot(true);

    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

    // 10 sec timeout for the request
    timer.start(10000);

    // Use the boards endpoint with details to get all cards from all stacks
    QUrl url(serverUrl + "/index.php/apps/deck/api/v1.1/boards/" + QString::number(this->boardId) +
             "/stacks?details=true");
    // QUrl url(serverUrl + "/index.php/apps/deck/stacks/" + QString::number(this->boardId));
    qDebug() << __func__ << " - 'url': " << url;

    QNetworkRequest networkRequest = QNetworkRequest(url);
    networkRequest.setHeader(QNetworkRequest::UserAgentHeader,
                             Utils::Misc::friendlyUserAgentString());

#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
    networkRequest.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
#else
    networkRequest.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
#endif

    QByteArray data;
    QNetworkReply* reply;

    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    networkRequest.setRawHeader("OCS-APIRequest", "true");
    addAuthHeader(networkRequest);

    reply = manager->get(networkRequest);

    loop.exec();
    QHash<int, NextcloudDeckService::Card> cards;

    // if we didn't get a timeout let us return the content
    if (timer.isActive()) {
        int returnStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        // only get the data if the status code was "success"
        // see: https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
        if (returnStatusCode >= 200 && returnStatusCode < 300) {
            // get the data from the network reply
            data = reply->readAll();
            qDebug() << __func__ << " - 'data': " << data;

            QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

            if (jsonDoc.isArray()) {
                QJsonArray stacksArray = jsonDoc.array();

                for (auto stackValue : stacksArray) {
                    QJsonObject stackObject = stackValue.toObject();
                    int currentStackId = stackObject["id"].toInt();

                    // Only process cards from the configured stack
                    if (currentStackId == this->stackId) {
                        QJsonArray cardsArray = stackObject["cards"].toArray();

                        for (auto cardValue : cardsArray) {
                            QJsonObject object = cardValue.toObject();

                            NextcloudDeckService::Card card;
                            card.id = object["id"].toInt();
                            card.title = object["title"].toString();
                            card.description = object["description"].toString();
                            card.order = object["order"].toInt();
                            card.type = object["type"].toString();

                            // Parse datetime fields from ISO 8601 strings
                            QString duedateString = object["duedate"].toString();
                            if (!duedateString.isEmpty() && duedateString != "null") {
                                card.duedate = QDateTime::fromString(duedateString, Qt::ISODate);
                            }

                            qint64 createdAtTimestamp =
                                object["createdAt"].toVariant().toLongLong();
                            if (createdAtTimestamp > 0) {
#if QT_VERSION < QT_VERSION_CHECK(5, 8, 0)
                                card.createdAt = QDateTime::fromTime_t(createdAtTimestamp);
#else
                                card.createdAt = QDateTime::fromSecsSinceEpoch(createdAtTimestamp);
#endif
                            }

                            qint64 lastModifiedTimestamp =
                                object["lastModified"].toVariant().toLongLong();
                            if (lastModifiedTimestamp > 0) {
#if QT_VERSION < QT_VERSION_CHECK(5, 8, 0)
                                card.lastModified = QDateTime::fromTime_t(lastModifiedTimestamp);
#else
                                card.lastModified =
                                    QDateTime::fromSecsSinceEpoch(lastModifiedTimestamp);
#endif
                            }

                            qDebug() << __func__ << " - found card: " << card;
                            cards[card.id] = card;
                        }

                        // We found our stack, no need to continue
                        break;
                    }
                }
            }

        } else {
            QString errorString = reply->errorString();
            Utils::Gui::warning(nullptr, tr("Error while loading cards"),
                                tr("Loading the cards failed with status code %1 and message: %2")
                                    .arg(QString::number(returnStatusCode), errorString),
                                "nextcloud-deck-get-cards-failed");

            qDebug() << __func__ << " - error: " << returnStatusCode;
            qDebug() << __func__ << " - 'errorString': " << errorString;
        }
    }

    reply->deleteLater();
    delete (manager);

    return cards;
}
