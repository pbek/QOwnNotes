#ifndef CALENDAR_ITEM_H
#define CALENDAR_ITEM_H

#include <QDateTime>
#include <QSqlQuery>
#include <QStringList>
#include <QUrl>

#define ICS_DATETIME_FORMAT "yyyyMMddThhmmss"
#define ICS_DATETIME_FORMAT_UTC "yyyyMMddThhmmssZ"

class CalendarItem {
   public:
    explicit CalendarItem();

    int getId();
    bool getHasDirtyData();
    static bool addCalendarItem(const QString &name, const QString &fileName, const QString &text);
    static CalendarItem fetch(int id);
    static QList<CalendarItem> search(const QString &text);
    static CalendarItem calendarItemFromQuery(const QSqlQuery &query);
    bool store();
    friend QDebug operator<<(QDebug dbg, const CalendarItem &calendarItem);
    bool fileExists();
    bool exists();
    bool fillFromQuery(const QSqlQuery &query);
    bool fillByFileName(QString fileName);
    bool remove();
    bool isFetched();
    bool isCompleted();
    static CalendarItem fetchByUrlAndCalendar(const QString &url, const QString &calendar);
    static bool addCalendarItemForRequest(const QString &calendar, const QUrl &url,
                                          const QString &etag, const QString &lastModifiedString);
    static QList<CalendarItem> fetchAllByCalendar(const QString &calendar);
    static bool deleteAllByCalendar(const QString &calendar);
    QString getUrl();
    QString getCalendar();
    QString getSummary();
    QString getDescription();
    void setSummary(const QString &text);
    void setDescription(const QString &text);
    bool updateWithICSData(const QString &icsData);
    QString getUid() const;
    QString getRelatedUid() const;
    static CalendarItem fetchByUid(const QString &uid);
    int getPriority();
    QString generateNewICSData();
    QString getICSData();
    void setPriority(int value);
    void setICSData(const QString &text);
    QString getETag();
    QString getLastModifiedString();
    static CalendarItem fetchByUrl(const QUrl &url);
    void setLastModifiedString(const QString &text);
    void setETag(const QString &text);
    static QList<QUrl> fetchAllUrlsByCalendar(const QString &calendar);
    void setCalendar(const QString &text);
    static CalendarItem createNewTodoItem(const QString &summary, const QString &calendar,
                                          const QString &relatedUid = QLatin1String(""));
    void setUrl(const QUrl &url);
    void setUid(const QString &text);
    void setRelatedUid(const QString &text);
    void setModified(const QDateTime &dateTime);
    void setCreated(const QDateTime &dateTime);
    void setCompleted(bool value);
    void updateCompleted(bool value);
    static QList<CalendarItem> fetchAll();
    static void updateAllSortPriorities();
    QDateTime getAlarmDate();
    static int getCurrentCalendarIndex();
    static QString getCurrentCalendarUrl();
    void setAlarmDate(const QDateTime &dateTime);
    static QList<CalendarItem> fetchAllForReminderAlert();
    static void alertTodoReminders();
    static QList<QString> searchAsUidList(const QString &text, const QString &calendar);
    static QList<CalendarItem> fetchAllForSystemTray(int limit = 10);
    static bool removeAll();
    static int countAll();

   private:
    int id;
    QString summary;
    QString url;
    QString description;
    QString calendar;
    QString uid;
    QString relatedUid;
    QString icsData;
    QString etag;
    QString lastModifiedString;
    int priority;
    int sortPriority;
    bool hasDirtyData;
    bool completed;
    QDateTime alarmDate;
    QDateTime created;
    QDateTime modified;
    QDateTime completedDate;
    QHash<QString, QString> icsDataHash;
    QStringList icsDataKeyList;
    static QString decodeICSDataLine(QString line);
    static QString findFreeHashKey(QHash<QString, QString> *hash, QString key, int number = 0);
    void generateICSDataHash();
    void updateICSDataKeyListFromHash();
    void updateSortPriority();
    QString getICSDataAttributeInBlock(const QString &block, const QString &attributeName);
    bool removeICSDataBlock(const QString &block);
    bool addVALARMBlockToICS();
    static QDateTime getDateTimeFromString(const QString &dateString);
};

#endif    // CALENDAR_ITEM_H
