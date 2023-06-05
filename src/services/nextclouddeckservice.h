#ifndef QOWNNOTES_NEXTCLOUDDECKSERVICE_H
#define QOWNNOTES_NEXTCLOUDDECKSERVICE_H

#include <QDateTime>
#include <QObject>
#include <QtNetwork/QNetworkRequest>

#include "entities/cloudconnection.h"
class NextcloudDeckService : public QObject {
    Q_OBJECT

   public:
    NextcloudDeckService(int cloudConnectionId, QObject* parent);
    int createCard(const QString& title, const QString& description = "", QDateTime* dueDateTime = nullptr);
    QString getCardLinkForId(int cardId);

   private:
    CloudConnection cloudConnection;
    QString serverUrl;
    int boardId;
    int stackId;
    void addAuthHeader(QNetworkRequest& networkRequest);
};

#endif    // QOWNNOTES_NEXTCLOUDDECKSERVICE_H
