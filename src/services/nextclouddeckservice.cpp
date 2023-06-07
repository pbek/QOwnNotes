#include "nextclouddeckservice.h"

#include <QEventLoop>
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

NextcloudDeckService::NextcloudDeckService(QObject* parent, int cloudConnectionId) : QObject(parent) {
    if (cloudConnectionId == -1) {
        cloudConnectionId = CloudConnection::currentCloudConnection().getId();
    }

    this->cloudConnection = CloudConnection::fetch(cloudConnectionId);
    this->serverUrl = this->cloudConnection.getServerUrl();
    this->boardId = this->cloudConnection.getNextcloudDeckBoardId();
    this->stackId = this->cloudConnection.getNextcloudDeckStackId();
}

bool NextcloudDeckService::isEnabled() {
    return OwnCloudService::isOwnCloudSupportEnabled() &&
           this->cloudConnection.getNextcloudDeckEnabled() &&
           this->cloudConnection.getNextcloudDeckBoardId() > 0 &&
           this->cloudConnection.getNextcloudDeckStackId() > 0;
}

int NextcloudDeckService::createCard(const QString& title,
                                      const QString& description,
                                      QDateTime* dueDateTime) {
    int cardId = -1;
    auto *manager = new QNetworkAccessManager();
    QEventLoop loop;
    QTimer timer;

    timer.setSingleShot(true);

    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

    // 10 sec timeout for the request
    timer.start(10000);

    QUrl url(serverUrl + "/apps/deck/api/v1.1/boards/" +
             QString::number(this->boardId) + "/stacks/" +
             QString::number(this->stackId) + "/cards");
    qDebug() << __func__ << " - 'url': " << url;

    QJsonObject bodyJson;
    bodyJson["title"] = title;
    bodyJson["type"] = "plain";
    bodyJson["order"] = 0;

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
    QNetworkReply *reply;

    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    networkRequest.setRawHeader("OCS-APIRequest", "true");
    addAuthHeader(networkRequest);

    reply = manager->post(networkRequest, bodyJsonDoc.toJson());

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
            cardId = jsonObject["id"].toInt();

            qDebug() << __func__ << " - 'cardId': " << cardId;
            qDebug() << __func__ << " - 'jsonDoc': " << jsonDoc;
        } else {
            QString errorString = reply->errorString();
            Utils::Gui::warning(nullptr, tr("Error while creating card"),
                                tr("Creating a card failed with status code %1 and message: %2")
                                    .arg(QString::number(returnStatusCode), errorString),
                                "nextcloud-deck-create-failed");

            qDebug() << __func__ << " - error: " << returnStatusCode;
            qDebug() << __func__ << " - 'errorString': " << errorString;
        }
    }

    reply->deleteLater();
    delete (manager);

    return cardId;
}

QString NextcloudDeckService::getCardLinkForId(int cardId) {
    qDebug() << __func__ << " - 'boardId': " << this->boardId;

    return QStringLiteral("%1/apps/deck/#/board/%2/card/%3").arg(
        this->serverUrl, QString::number(this->boardId), QString::number(cardId));
}

void NextcloudDeckService::addAuthHeader(QNetworkRequest& networkRequest) {
    auto authString =
        cloudConnection.getUsername() + QStringLiteral(":") + cloudConnection.getPassword();
    QByteArray authStringBase64 = authString.toLocal8Bit().toBase64();
    QString headerData = QStringLiteral("Basic ") + authStringBase64;
    networkRequest.setRawHeader("Authorization", headerData.toLocal8Bit());
}
