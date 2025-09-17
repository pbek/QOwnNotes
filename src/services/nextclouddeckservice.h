#ifndef QOWNNOTES_NEXTCLOUDDECKSERVICE_H
#define QOWNNOTES_NEXTCLOUDDECKSERVICE_H

#include <QDateTime>
#include <QDebug>
#include <QObject>
#include <QtNetwork/QNetworkRequest>

#include "entities/cloudconnection.h"

class NextcloudDeckService : public QObject {
    Q_OBJECT

    struct Board {
        int id;
        QString title;
        QHash<int, QString> stacks;

        bool hasStacks() { return !stacks.isEmpty(); }

        //        QDebug operator<<(QDebug dbg, const Board & board) {
        //            dbg.nospace() << "Bookmark: <title>" << board.title << " <id>" << board.id <<
        //            " <stacks>"
        //                          << board.stacks;
        //            return dbg.space();
        //        }
    };

   public:
    struct Card {
        int id;
        QString title;
        QString description;
        QDateTime duedate;
        int order;
        QString type;
        QDateTime createdAt;
        QDateTime lastModified;

        friend QDebug operator<<(QDebug dbg, const Card& card) {
            dbg.nospace() << "Card: <id>" << card.id << " <title>" << card.title << " <description>"
                          << card.description << " <duedate>" << card.duedate.toString(Qt::ISODate)
                          << " <order>" << card.order << " <type>" << card.type << " <createdAt>"
                          << card.createdAt.toString(Qt::ISODate) << " <lastModified>"
                          << card.lastModified.toString(Qt::ISODate);
            return dbg.space();
        }
    };

    explicit NextcloudDeckService(QObject* parent, int cloudConnectionId = -1);
    int storeCard(const QString& title, const QString& description = "",
                  QDateTime* dueDateTime = nullptr, int cardId = -1);
    QString getCardLinkForId(int cardId);
    bool isEnabledAndValid();
    bool isEnabled();
    QList<Board> getBoards();
    QHash<int, Card> getCards();
    bool archiveCard(int cardId);
    bool deleteCard(int cardId);
    int parseCardIdFromUrl(const QString& url) const;
    static bool isCardUrl(const QString& url);

   private:
    CloudConnection cloudConnection;
    QString serverUrl;
    int boardId;
    int stackId;
    void addAuthHeader(QNetworkRequest& networkRequest);
};

#endif    // QOWNNOTES_NEXTCLOUDDECKSERVICE_H
