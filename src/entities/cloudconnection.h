#pragma once

#include <QSqlQuery>

class CloudConnection {
   public:
    explicit CloudConnection();

    int getId();
    static bool create(const QString &name, const QString &serverUrl, const QString &username,
                       const QString &password);
    static CloudConnection fetch(int id, bool ignoreTableWarning = false);
    static CloudConnection cloudConnectionFromQuery(const QSqlQuery &query);
    bool store();
    bool storeMigratedCloudConnection();
    friend QDebug operator<<(QDebug dbg, const CloudConnection &CloudConnection);
    bool exists();
    bool fillFromQuery(const QSqlQuery &query);
    bool remove();
    bool isFetched();
    static QList<CloudConnection> fetchAll();
    QString getName();
    QString getServerUrl();
    QString getUsername();
    QString getAccountId();
    QString getPassword();
    bool getAppQOwnNotesAPIEnabled();
    int getPriority();
    void setName(const QString &text);
    void setServerUrl(const QString &text);
    void setPriority(int value);
    void setAppQOwnNotesAPIEnabled(bool value);
    void setUsername(const QString &text);
    void setAccountId(const QString &text);
    void setPassword(const QString &text);
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
    QString accountId;
    QString password;
    int priority = 0;
    bool appQOwnNotesAPIEnabled = true;
};
