#include "nextclouddeckservice.h"

#include <QEventLoop>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>

#include "entities/cloudconnection.h"
#include "utils/misc.h"

NextcloudDeckService::NextcloudDeckService(int cloudConnectionId, QObject *parent) : QObject(parent) {
    this->cloudConnection = CloudConnection::fetch(cloudConnectionId);
    this->boardId = this->cloudConnection.getNextcloudDeckBoardId();
    this->stackId = this->cloudConnection.getNextcloudDeckStackId();
}

void NextcloudDeckService::createCard(const QString& title,
                                      const QString& description,
                                      QDateTime* dueDateTime) {
    auto *manager = new QNetworkAccessManager();
    QEventLoop loop;
    QTimer timer;

    timer.setSingleShot(true);

    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

    // 10 sec timeout for the request
    timer.start(10000);

    QUrl url(this->cloudConnection.getServerUrl() + "/apps/deck/api/v1.1/boards/" +
             QString::number(this->boardId) + "/stacks/" +
             QString::number(this->stackId) + "/cards");

    QJsonObject bodyJson;
    bodyJson["title"] = title;
    bodyJson["type"] = "plain";
    bodyJson["order"] = 0;

    if (description != "") {
        bodyJson["description"] = description;
    }

    if (dueDateTime != nullptr) {
        bodyJson["dueDateTime"] = dueDateTime->toSecsSinceEpoch();
    }

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

    auto authString = this->cloudConnection.getUsername() + QStringLiteral(":") + this->cloudConnection.getPassword();
    QByteArray authStringBase64 = authString.toLocal8Bit().toBase64();
    QString headerData = QStringLiteral("Basic ") + authStringBase64;
    networkRequest.setRawHeader("Authorization", headerData.toLocal8Bit());

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
            int id = jsonObject["id"].toInt();

            qDebug() << __func__ << " - 'id': " << id;
        } else {
            qDebug() << __func__ << " - error: " << returnStatusCode;
            qDebug() << __func__ << " - 'data': " << data;
        }
    }

    reply->deleteLater();
    delete (manager);
}
