#ifndef QOWNNOTES_NEXTCLOUDDECKSERVICE_H
#define QOWNNOTES_NEXTCLOUDDECKSERVICE_H

#include <QObject>
#include <QDateTime>

#include "entities/cloudconnection.h"
class NextcloudDeckService : public QObject {
    Q_OBJECT

   public:
    NextcloudDeckService(int cloudConnectionId, QObject* parent);
    void createCard(const QString& title, const QString& description = "", QDateTime* dueDateTime = nullptr);

   private:
    CloudConnection cloudConnection;
    int boardId;
    int stackId;
};

#endif    // QOWNNOTES_NEXTCLOUDDECKSERVICE_H
