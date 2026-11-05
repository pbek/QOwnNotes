#include "nextclouddeckservice.h"

#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QTimeZone>
#include <QTimer>

#include "cloudservice.h"
#include "entities/cloudconnection.h"
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

/**
 * Cancels the currently running request of getBoards() or getCards()
 */
void NextcloudDeckService::cancelRequests() {
    _canceled = true;
    emit requestsCanceled();
}

bool NextcloudDeckService::isEnabledAndValid() {
    return isEnabled() && this->cloudConnection.getNextcloudDeckBoardId() > 0 &&
           this->cloudConnection.getNextcloudDeckStackId() > 0;
}

bool NextcloudDeckService::isEnabled() {
    return CloudService::isCloudSupportEnabled() && this->cloudConnection.getNextcloudDeckEnabled();
}

int NextcloudDeckService::storeCard(const QString& title, const QString& description,
                                    QDateTime* dueDateTime, int cardId, int cardStackId,
                                    int cardOrder, const QString& cardOwner) {
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
    // For updates use the card's own stackId if provided, otherwise fall back to configured stack
    int effectiveStackId = (isUpdate && cardStackId > 0) ? cardStackId : this->stackId;
    QUrl url;

    if (isUpdate) {
        url = QUrl(serverUrl + "/index.php/apps/deck/api/v1.1/boards/" +
                   QString::number(this->boardId) + "/stacks/" + QString::number(effectiveStackId) +
                   "/cards/" + QString::number(cardId));
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
    // Use the card's actual order for updates to avoid reordering side-effects
    bodyJson["order"] = isUpdate ? cardOrder : 0;
    // Description is always required by the API; send an empty string if not provided
    bodyJson["description"] = description;
    // Owner is required by the Deck API for card updates
    bodyJson["owner"] = !cardOwner.isEmpty() ? cardOwner : cloudConnection.getUsername();

    if (dueDateTime != nullptr) {
        // We need to convert the DateTime to UTC to get around Deck's/Nextcloud's timezone issue
        // see: https://github.com/nextcloud/deck/issues/4764
        bodyJson["duedate"] = dueDateTime->toTimeZone(QTimeZone::utc()).toString(Qt::ISODate);
    } else {
        // Explicitly send null to clear the due date on updates
        bodyJson["duedate"] = QJsonValue::Null;
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
            QString responseBody = reply->readAll();
            if (isUpdate) {
                Utils::Gui::warning(nullptr, tr("Error while updating card"),
                                    tr("Updating card failed with status code %1 and message: %2")
                                        .arg(QString::number(returnStatusCode), errorString),
                                    "nextcloud-deck-update-failed");
            } else {
                Utils::Gui::warning(nullptr, tr("Error while creating card"),
                                    tr("Creating card failed with status code %1 and message: %2")
                                        .arg(QString::number(returnStatusCode), errorString),
                                    "nextcloud-deck-create-failed");
            }

            qDebug() << __func__ << " - error: " << returnStatusCode;
            qDebug() << __func__ << " - 'errorString': " << errorString;
            qDebug() << __func__ << " - 'responseBody': " << responseBody;
        }
    }

    reply->deleteLater();
    delete (manager);

    return resultCardId;
}

bool NextcloudDeckService::moveCard(int cardId, int targetBoardId, int targetStackId, int order) {
    if (cardId < 1 || boardId < 1 || stackId < 1 || targetBoardId < 1 || targetStackId < 1 ||
        order < 0) {
        return false;
    }

    QNetworkAccessManager manager;
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

    // Route parameters can override JSON parameters in Nextcloud. Use the destination in both
    // places so stackId cannot resolve to the source stack and silently reorder instead of move.
    // Deck resolves the existing card by cardId and checks permissions on it and the target stack.
    const QUrl url(serverUrl + "/index.php/apps/deck/api/v1.1/boards/" +
                   QString::number(targetBoardId) + "/stacks/" + QString::number(targetStackId) +
                   "/cards/" + QString::number(cardId) + "/reorder");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, Utils::Misc::friendlyUserAgentString());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("OCS-APIRequest", "true");
#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
#else
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
#endif
    addAuthHeader(request);

    QJsonObject body;
    body["stackId"] = targetStackId;
    body["order"] = order;
    QNetworkReply* reply = manager.put(request, QJsonDocument(body).toJson());
    timer.start(10000);
    loop.exec();

    const bool timedOut = !timer.isActive();
    timer.stop();
    if (timedOut) {
        reply->abort();
    }

    const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QJsonDocument response = QJsonDocument::fromJson(reply->readAll());
    const bool requestSucceeded = !timedOut && reply->error() == QNetworkReply::NoError &&
                                  statusCode >= 200 && statusCode < 300;
    // Reorder returns the destination stack's cards. HTTP success alone does not prove a move.
    bool success = false;
    if (requestSucceeded && response.isArray()) {
        for (const auto& value : response.array()) {
            const auto card = value.toObject();
            if (card.value(QStringLiteral("id")).toInt() == cardId &&
                card.value(QStringLiteral("stackId")).toInt() == targetStackId) {
                success = true;
                break;
            }
        }
    }
    if (!success) {
        QString message;
        if (timedOut) {
            message = tr("Moving the card timed out. Reload the cards to check its location.");
        } else if (requestSucceeded) {
            message =
                tr("The server did not confirm that the card was moved to the destination "
                   "stack. Reload the cards to check its location.");
        } else {
            const QString serverMessage =
                response.object().value(QStringLiteral("message")).toString();
            message = tr("Moving the card failed with status code %1 and message: %2")
                          .arg(QString::number(statusCode),
                               serverMessage.isEmpty() ? reply->errorString() : serverMessage);
        }
        // Move failures must remain visible even if a previous warning was suppressed.
        QMessageBox::warning(qobject_cast<QWidget*>(parent()), tr("Error while moving card"),
                             message);
    }

    reply->deleteLater();
    return success;
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
                                tr("Archiving the card failed with status code %1 and message: %2")
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

bool NextcloudDeckService::isCardUrl(const QString& url) {
    if (url.isEmpty()) {
        return false;
    }

    static const QRegularExpression re(QStringLiteral(R"(\/apps\/deck\/#\/board\/\d+\/card\/)"));
    QRegularExpressionMatch match = re.match(url);

    return match.hasMatch();
}

int NextcloudDeckService::parseBoardIdFromUrlDirectly(const QString& url) {
    static const QRegularExpression re(QStringLiteral(R"(\/board\/(\d+)\/card\/)"));
    QRegularExpressionMatch match = re.match(url);

    if (match.hasMatch()) {
        return match.captured(1).toInt();
    }

    return -1;
}

QString NextcloudDeckService::getCardUrlPattern() const {
    return serverUrl + QStringLiteral("/apps/deck/#/board/") + QString::number(this->boardId) +
           QStringLiteral("/card/");
}

int NextcloudDeckService::parseCardIdFromUrlDirectly(const QString& url) {
    static const QRegularExpression re(QStringLiteral(R"(\/card\/(\d+))"));
    QRegularExpressionMatch match = re.match(url);

    if (match.hasMatch()) {
        return match.captured(1).toInt();
    }

    return -1;
}

int NextcloudDeckService::parseCardIdFromUrl(const QString& url) const {
    qDebug() << __func__ << "getCardUrlPattern(): " << getCardUrlPattern();
    if (!url.startsWith(getCardUrlPattern())) {
        return -1;
    }

    static const QRegularExpression re(QStringLiteral(R"(\/card\/(\d+))"));
    QRegularExpressionMatch match = re.match(url);

    if (match.hasMatch()) {
        return match.captured(1).toInt();
    }

    return -1;
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
                                tr("Deleting the card failed with status code %1 and message: %2")
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

void NextcloudDeckService::setBoardAndStackIds(int boardId, int stackId) {
    this->boardId = boardId;
    this->stackId = stackId;
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

    QObject::connect(this, &NextcloudDeckService::requestsCanceled, &loop, &QEventLoop::quit);
    QObject::connect(reply, &QNetworkReply::downloadProgress, this,
                     &NextcloudDeckService::downloadProgress);
    loop.exec();
    QList<NextcloudDeckService::Board> boards;

    // if we didn't get a timeout let us return the content
    if (timer.isActive() && !_canceled) {
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
                    board.labels = labelsFromJson(object[QStringLiteral("labels")]);
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

QHash<int, NextcloudDeckService::Card> NextcloudDeckService::getCards(bool includeArchived) {
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

    QObject::connect(this, &NextcloudDeckService::requestsCanceled, &loop, &QEventLoop::quit);
    QObject::connect(reply, &QNetworkReply::downloadProgress, this,
                     &NextcloudDeckService::downloadProgress);
    loop.exec();
    QHash<int, NextcloudDeckService::Card> cards;

    // if we didn't get a timeout let us return the content
    if (timer.isActive() && !_canceled) {
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
                        int currentBoardId = stackObject["boardId"].toInt();

                        for (auto cardValue : cardsArray) {
                            const auto card =
                                cardFromJson(cardValue.toObject(), currentStackId,
                                             currentBoardId > 0 ? currentBoardId : this->boardId);

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

    // If includeArchived is true, fetch archived cards as well
    if (includeArchived && !_canceled) {
        auto* archivedManager = new QNetworkAccessManager();
        QEventLoop archivedLoop;
        QTimer archivedTimer;

        archivedTimer.setSingleShot(true);

        QObject::connect(&archivedTimer, SIGNAL(timeout()), &archivedLoop, SLOT(quit()));
        QObject::connect(archivedManager, SIGNAL(finished(QNetworkReply*)), &archivedLoop,
                         SLOT(quit()));

        // 10 sec timeout for the request
        archivedTimer.start(10000);

        QUrl archivedUrl(serverUrl + "/index.php/apps/deck/api/v1.1/boards/" +
                         QString::number(this->boardId) + "/stacks/archived");
        qDebug() << __func__ << " - 'archivedUrl': " << archivedUrl;

        QNetworkRequest archivedNetworkRequest = QNetworkRequest(archivedUrl);
        archivedNetworkRequest.setHeader(QNetworkRequest::UserAgentHeader,
                                         Utils::Misc::friendlyUserAgentString());

#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
        archivedNetworkRequest.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
#else
        archivedNetworkRequest.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
#endif

        QByteArray archivedData;
        QNetworkReply* archivedReply;

        archivedNetworkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        archivedNetworkRequest.setRawHeader("OCS-APIRequest", "true");
        addAuthHeader(archivedNetworkRequest);

        archivedReply = archivedManager->get(archivedNetworkRequest);

        QObject::connect(this, &NextcloudDeckService::requestsCanceled, &archivedLoop,
                         &QEventLoop::quit);
        QObject::connect(archivedReply, &QNetworkReply::downloadProgress, this,
                         &NextcloudDeckService::downloadProgress);
        archivedLoop.exec();

        // if we didn't get a timeout let us return the content
        if (archivedTimer.isActive() && !_canceled) {
            int archivedReturnStatusCode =
                archivedReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

            // only get the data if the status code was "success"
            // see: https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
            if (archivedReturnStatusCode >= 200 && archivedReturnStatusCode < 300) {
                // get the data from the network reply
                archivedData = archivedReply->readAll();
                qDebug() << __func__ << " - 'archivedData': " << archivedData;

                QJsonDocument archivedJsonDoc = QJsonDocument::fromJson(archivedData);

                if (archivedJsonDoc.isArray()) {
                    QJsonArray archivedStacksArray = archivedJsonDoc.array();

                    for (auto stackValue : archivedStacksArray) {
                        QJsonObject stackObject = stackValue.toObject();
                        int archivedStackId = stackObject["id"].toInt();
                        int archivedBoardId = stackObject["boardId"].toInt();
                        QJsonArray archivedCardsArray = stackObject["cards"].toArray();

                        for (auto cardValue : archivedCardsArray) {
                            auto card =
                                cardFromJson(cardValue.toObject(),
                                             archivedStackId > 0 ? archivedStackId : this->stackId,
                                             archivedBoardId > 0 ? archivedBoardId : this->boardId);
                            card.archived = true;    // These are archived cards

                            qDebug() << __func__ << " - found archived card: " << card;
                            cards[card.id] = card;
                        }
                    }
                }

            } else {
                QString errorString = archivedReply->errorString();
                Utils::Gui::warning(
                    nullptr, tr("Error while loading archived cards"),
                    tr("Loading the archived cards failed with status code %1 and message: %2")
                        .arg(QString::number(archivedReturnStatusCode), errorString),
                    "nextcloud-deck-get-archived-cards-failed");

                qDebug() << __func__ << " - archived error: " << archivedReturnStatusCode;
                qDebug() << __func__ << " - 'archivedErrorString': " << errorString;
            }
        }

        archivedReply->deleteLater();
        delete (archivedManager);
    }

    return cards;
}

NextcloudDeckService::Card NextcloudDeckService::cardFromJson(const QJsonObject& object,
                                                              int cardStackId,
                                                              int cardBoardId) const {
    NextcloudDeckService::Card card;
    card.id = object[QStringLiteral("id")].toInt();
    card.stackId = cardStackId;
    card.boardId = cardBoardId;
    card.title = object[QStringLiteral("title")].toString();
    card.description = object[QStringLiteral("description")].toString();
    card.owner = object[QStringLiteral("owner")].toString();
    card.order = object[QStringLiteral("order")].toInt();
    card.type = object[QStringLiteral("type")].toString();
    card.archived = object[QStringLiteral("archived")].toBool();
    card.labels = labelsFromJson(object[QStringLiteral("labels")]);

    // Parse datetime fields from ISO 8601 strings
    const QString duedateString = object[QStringLiteral("duedate")].toString();
    if (!duedateString.isEmpty() && duedateString != QStringLiteral("null")) {
        card.duedate = QDateTime::fromString(duedateString, Qt::ISODate);
    }

    const qint64 createdAtTimestamp = object[QStringLiteral("createdAt")].toVariant().toLongLong();
    if (createdAtTimestamp > 0) {
        card.createdAt = QDateTime::fromSecsSinceEpoch(createdAtTimestamp);
    }

    const qint64 lastModifiedTimestamp =
        object[QStringLiteral("lastModified")].toVariant().toLongLong();
    if (lastModifiedTimestamp > 0) {
        card.lastModified = QDateTime::fromSecsSinceEpoch(lastModifiedTimestamp);
    }

    return card;
}

NextcloudDeckService::Label NextcloudDeckService::labelFromJson(const QJsonObject& object) {
    Label label;
    label.id = object[QStringLiteral("id")].toVariant().toInt();
    label.title = object[QStringLiteral("title")].toString();
    label.color = object[QStringLiteral("color")].toString().remove(QLatin1Char('#'));
    return label;
}

QList<NextcloudDeckService::Label> NextcloudDeckService::labelsFromJson(const QJsonValue& value) {
    QList<Label> labels;

    // Deck returns null instead of an empty array for cards without labels
    if (!value.isArray()) {
        return labels;
    }

    const QJsonArray array = value.toArray();
    for (const auto& labelValue : array) {
        const Label label = labelFromJson(labelValue.toObject());

        if (label.id > 0) {
            labels.append(label);
        }
    }

    return labels;
}

/**
 * The colors the Deck web interface offers for labels
 */
QStringList NextcloudDeckService::defaultLabelColors() {
    return {QStringLiteral("31CC7C"), QStringLiteral("317CCC"), QStringLiteral("FF7A66"),
            QStringLiteral("F1DB50"), QStringLiteral("7C31CC"), QStringLiteral("CC317C"),
            QStringLiteral("3A3B3D"), QStringLiteral("CACBCD")};
}

QUrl NextcloudDeckService::cardUrl(int cardId, const QString& action) const {
    QString url = serverUrl + QStringLiteral("/index.php/apps/deck/api/v1.1/boards/") +
                  QString::number(this->boardId) + QStringLiteral("/stacks/") +
                  QString::number(this->stackId) + QStringLiteral("/cards/") +
                  QString::number(cardId);

    if (!action.isEmpty()) {
        url += QLatin1Char('/') + action;
    }

    return QUrl(url);
}

/**
 * Sends a JSON request to the Deck API and waits up to 10 seconds for the reply
 *
 * Returns true if the server answered with a success status code
 */
bool NextcloudDeckService::sendJsonRequest(const QByteArray& verb, const QUrl& url,
                                           const QJsonObject& body, int* statusCode,
                                           QByteArray* responseData, bool* timedOut) {
    QNetworkAccessManager manager;
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, Utils::Misc::friendlyUserAgentString());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("OCS-APIRequest", "true");
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
    addAuthHeader(request);

    qDebug() << __func__ << " - " << verb << " " << url << " " << body;

    QNetworkReply* reply = manager.sendCustomRequest(
        request, verb, QJsonDocument(body).toJson(QJsonDocument::Compact));
    timer.start(10000);
    loop.exec();

    const bool isTimedOut = !timer.isActive();
    timer.stop();
    if (isTimedOut) {
        reply->abort();
    }

    const int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QByteArray data = reply->readAll();
    reply->deleteLater();

    if (statusCode != nullptr) {
        *statusCode = code;
    }
    if (responseData != nullptr) {
        *responseData = data;
    }
    if (timedOut != nullptr) {
        *timedOut = isTimedOut;
    }

    return !isTimedOut && code >= 200 && code < 300;
}

QString NextcloudDeckService::errorMessageForResponse(int statusCode,
                                                      const QByteArray& responseData,
                                                      bool timedOut) const {
    if (timedOut) {
        return tr("The request to your Nextcloud server timed out.");
    }

    // Deck reports the reason for a failed request in the "message" field
    const QString serverMessage =
        QJsonDocument::fromJson(responseData).object().value(QStringLiteral("message")).toString();

    return tr("The request failed with status code %1 and message: %2")
        .arg(QString::number(statusCode),
             serverMessage.isEmpty() ? tr("unknown error") : serverMessage);
}

/**
 * Creates a new label in the current board
 *
 * Returns a label with an id of 0 if the label couldn't be created
 */
NextcloudDeckService::Label NextcloudDeckService::createLabel(const QString& title,
                                                              const QString& color) {
    const QString trimmedTitle = title.trimmed();

    if (this->boardId < 1 || trimmedTitle.isEmpty()) {
        return Label();
    }

    const QUrl url(serverUrl + QStringLiteral("/index.php/apps/deck/api/v1.1/boards/") +
                   QString::number(this->boardId) + QStringLiteral("/labels"));
    QJsonObject body;
    body[QStringLiteral("title")] = trimmedTitle;
    body[QStringLiteral("color")] = QString(color).remove(QLatin1Char('#')).toUpper();

    int statusCode = 0;
    QByteArray data;
    bool timedOut = false;

    if (sendJsonRequest("POST", url, body, &statusCode, &data, &timedOut)) {
        const Label label = labelFromJson(QJsonDocument::fromJson(data).object());

        if (label.id > 0) {
            return label;
        }
    }

    Utils::Gui::warning(qobject_cast<QWidget*>(parent()), tr("Error while creating tag"),
                        tr("The tag \"%1\" could not be created. %2")
                            .arg(trimmedTitle, errorMessageForResponse(statusCode, data, timedOut)),
                        QStringLiteral("nextcloud-deck-create-label-failed"));

    return Label();
}

bool NextcloudDeckService::assignLabel(int cardId, int labelId) {
    if (cardId < 1 || labelId < 1 || this->boardId < 1 || this->stackId < 1) {
        return false;
    }

    QJsonObject body;
    body[QStringLiteral("labelId")] = labelId;
    int statusCode = 0;
    QByteArray data;
    bool timedOut = false;

    if (sendJsonRequest("PUT", cardUrl(cardId, QStringLiteral("assignLabel")), body, &statusCode,
                        &data, &timedOut)) {
        return true;
    }

    Utils::Gui::warning(qobject_cast<QWidget*>(parent()), tr("Error while adding tag"),
                        tr("The tag could not be added to the card. %1")
                            .arg(errorMessageForResponse(statusCode, data, timedOut)),
                        QStringLiteral("nextcloud-deck-assign-label-failed"));

    return false;
}

bool NextcloudDeckService::removeLabel(int cardId, int labelId) {
    if (cardId < 1 || labelId < 1 || this->boardId < 1 || this->stackId < 1) {
        return false;
    }

    QJsonObject body;
    body[QStringLiteral("labelId")] = labelId;
    int statusCode = 0;
    QByteArray data;
    bool timedOut = false;

    if (sendJsonRequest("PUT", cardUrl(cardId, QStringLiteral("removeLabel")), body, &statusCode,
                        &data, &timedOut)) {
        return true;
    }

    Utils::Gui::warning(qobject_cast<QWidget*>(parent()), tr("Error while removing tag"),
                        tr("The tag could not be removed from the card. %1")
                            .arg(errorMessageForResponse(statusCode, data, timedOut)),
                        QStringLiteral("nextcloud-deck-remove-label-failed"));

    return false;
}

/**
 * Assigns and removes labels, so the card ends up with exactly the wanted labels
 *
 * Returns false if at least one of the requests failed
 */
bool NextcloudDeckService::setCardLabels(int cardId, const QList<int>& currentLabelIds,
                                         const QList<int>& wantedLabelIds) {
    bool success = true;

    for (const int labelId : currentLabelIds) {
        if (!wantedLabelIds.contains(labelId)) {
            success = removeLabel(cardId, labelId) && success;
        }
    }

    for (const int labelId : wantedLabelIds) {
        if (!currentLabelIds.contains(labelId)) {
            success = assignLabel(cardId, labelId) && success;
        }
    }

    return success;
}
