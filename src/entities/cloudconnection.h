#pragma once

#include <QSqlQuery>

class CloudConnection
{
public:
    explicit CloudConnection();

    int getId();
    static bool create(QString name, QString serverUrl,
                       QString username, QString password);
    static CloudConnection fetch(int id, bool ignoreTableWarning = false);
    static CloudConnection cloudConnectionFromQuery(const QSqlQuery& query);
    bool store();
    friend QDebug operator<<(QDebug dbg, const CloudConnection &CloudConnection);
    bool exists();
    bool fillFromQuery(const QSqlQuery& query);
    bool remove();
    bool isFetched();
    static QList<CloudConnection> fetchAll();
    QString getName();
    QString getServerUrl();
    QString getUsername();
    QString getPassword();
    int getPriority();
    void setName(QString text);
    void setServerUrl(QString text);
    void setPriority(int value);
    void setUsername(QString text);
    void setPassword(QString text);
    static int countAll();
    static bool migrateToCloudConnections();
    bool isCurrent();
    static CloudConnection currentCloudConnection(bool ignoreTableWarning = false);
    static CloudConnection firstCloudConnection();
    static CloudConnection currentTodoCalendarCloudConnection();
    QString getServerUrlPath();
    QString getServerUrlWithoutPath();
    static QList<int> fetchUsedCloudConnectionsIds();

private:
    int id;
    QString name;
    QString serverUrl;
    QString username;
    QString password;
    int priority = 0;
};
