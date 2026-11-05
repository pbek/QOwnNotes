#ifndef QOWNNOTES_NEXTCLOUDDECKSERVICE_H
#define QOWNNOTES_NEXTCLOUDDECKSERVICE_H

#include <QDateTime>
#include <QDebug>
#include <QJsonObject>
#include <QJsonValue>
#include <QList>
#include <QObject>
#include <QtNetwork/QNetworkRequest>

#include "entities/cloudconnection.h"

class NextcloudDeckService : public QObject {
    Q_OBJECT

   public:
    // Labels (shown as tags in QOwnNotes) are defined per board and can be assigned to cards
    struct Label {
        int id = 0;
        QString title;
        // Hexadecimal color without leading "#", like Deck stores it (e.g. "31CC7C")
        QString color;

        bool operator==(const Label& other) const {
            return id == other.id && title == other.title && color == other.color;
        }

        friend QDebug operator<<(QDebug dbg, const Label& label) {
            dbg.nospace() << "Label: <id>" << label.id << " <title>" << label.title << " <color>"
                          << label.color;
            return dbg.space();
        }
    };

    struct Board {
        int id;
        QString title;
        QHash<int, QString> stacks;
        QList<Label> labels;

        bool hasStacks() { return !stacks.isEmpty(); }
    };

    struct Card {
        int id;
        int stackId;
        int boardId;
        QString title;
        QString description;
        QString owner;
        QDateTime duedate;
        int order;
        QString type;
        QDateTime createdAt;
        QDateTime lastModified;
        bool archived;
        QList<Label> labels;

        QList<int> labelIds() const {
            QList<int> ids;
            for (const auto& label : labels) {
                ids.append(label.id);
            }
            return ids;
        }

        friend QDebug operator<<(QDebug dbg, const Card& card) {
            dbg.nospace() << "Card: <id>" << card.id << " <title>" << card.title << " <description>"
                          << card.description << " <duedate>" << card.duedate.toString(Qt::ISODate)
                          << " <order>" << card.order << " <type>" << card.type << " <createdAt>"
                          << card.createdAt.toString(Qt::ISODate) << " <lastModified>"
                          << card.lastModified.toString(Qt::ISODate) << " <archived>"
                          << card.archived << " <labels>" << card.labels;
            return dbg.space();
        }
    };

    explicit NextcloudDeckService(QObject* parent, int cloudConnectionId = -1);
    Label createLabel(const QString& title, const QString& color);
    bool assignLabel(int cardId, int labelId);
    bool removeLabel(int cardId, int labelId);
    bool setCardLabels(int cardId, const QList<int>& currentLabelIds,
                       const QList<int>& wantedLabelIds);
    static Label labelFromJson(const QJsonObject& object);
    static QList<Label> labelsFromJson(const QJsonValue& value);
    static QStringList defaultLabelColors();
    int storeCard(const QString& title, const QString& description = "",
                  QDateTime* dueDateTime = nullptr, int cardId = -1, int cardStackId = -1,
                  int cardOrder = 0, const QString& cardOwner = "");
    QString getCardLinkForId(int cardId);
    bool isEnabledAndValid();
    bool isEnabled();
    QList<Board> getBoards();
    QHash<int, Card> getCards(bool includeArchived = false);
    bool archiveCard(int cardId);
    bool deleteCard(int cardId);
    bool moveCard(int cardId, int targetBoardId, int targetStackId, int order = 0);
    int parseCardIdFromUrl(const QString& url) const;
    static bool isCardUrl(const QString& url);
    static int parseBoardIdFromUrlDirectly(const QString& url);
    static int parseCardIdFromUrlDirectly(const QString& url);
    QString getCardUrlPattern() const;
    void setBoardAndStackIds(int boardId, int stackId);
    void cancelRequests();

   signals:
    void requestsCanceled();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

   private:
    bool _canceled = false;
    CloudConnection cloudConnection;
    QString serverUrl;
    int boardId;
    int stackId;
    void addAuthHeader(QNetworkRequest& networkRequest);
    bool sendJsonRequest(const QByteArray& verb, const QUrl& url, const QJsonObject& body,
                         int* statusCode, QByteArray* responseData, bool* timedOut);
    QString errorMessageForResponse(int statusCode, const QByteArray& responseData,
                                    bool timedOut) const;
    QUrl cardUrl(int cardId, const QString& action = QString()) const;
    Card cardFromJson(const QJsonObject& object, int cardStackId, int cardBoardId) const;
};

#endif    // QOWNNOTES_NEXTCLOUDDECKSERVICE_H
