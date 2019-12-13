#ifndef CALENDAR_ITEM_H
#define CALENDAR_ITEM_H

#include <QDateTime>
#include <QSqlQuery>
#include <QStringList>
#include <QUrl>

#define ICS_DATETIME_FORMAT "yyyyMMddThhmmss"

class CalendarItem
{
public:
    explicit CalendarItem();

    int getId();
    bool getHasDirtyData();
    static bool addCalendarItem( QString name, QString fileName, QString text );
    static CalendarItem fetch( int id );
    static QList<CalendarItem> search(QString text);
    static CalendarItem calendarItemFromQuery( QSqlQuery query );
    bool store();
    friend QDebug operator<<(QDebug dbg, const CalendarItem &calendarItem);
    bool fileExists();
    bool exists();
    bool fillFromQuery(QSqlQuery query);
    bool fillByFileName(QString fileName);
    bool remove();
    bool isFetched();
    bool isCompleted();
    static CalendarItem fetchByUrlAndCalendar(QString url, QString calendar);
    static bool addCalendarItemForRequest(QString calendar, QUrl url, QString etag, QString lastModifiedString);
    static QList<CalendarItem> fetchAllByCalendar(QString calendar);
    static bool deleteAllByCalendar(QString calendar);
    QString getUrl();
    QString getCalendar();
    QString getSummary();
    QString getDescription();
    void setSummary(QString text);
    void setDescription(QString text);
    bool updateWithICSData(QString icsData);
    QString getUid() const;
    static CalendarItem fetchByUid(QString uid);
    int getPriority();
    QString generateNewICSData();
    QString getICSData();
    void setPriority(int value);
    void setICSData(QString text);
    QString getETag();
    QString getLastModifiedString();
    static CalendarItem fetchByUrl(QUrl url);
    void setLastModifiedString(QString text);
    void setETag(QString text);
    static QList<QUrl> fetchAllUrlsByCalendar(QString calendar);
    void setCalendar(QString text);
    static CalendarItem createNewTodoItem(QString summary, QString calendar);
    void setUrl(QUrl url);
    void setUid(QString text);
    void setModified(QDateTime dateTime);
    void setCreated(QDateTime dateTime);
    void setCompleted(bool value);
    void updateCompleted(bool value);
    static QList<CalendarItem> fetchAll();
    static void updateAllSortPriorities();
    QDateTime getAlarmDate();
    static int getCurrentCalendarIndex();
    static QString getCurrentCalendarUrl();
    void setAlarmDate(QDateTime dateTime);
    static QList<CalendarItem> fetchAllForReminderAlert();
    static void alertTodoReminders();
    static QList<QString> searchAsUidList(QString text, QString calendar);
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
    static QString findFreeHashKey(QHash<QString, QString> *hash, QString key, int number = 0 );
    void generateICSDataHash();
    void updateICSDataKeyListFromHash();
    void updateSortPriority();
    QString getICSDataAttributeInBlock(QString block, QString attributeName);
    bool removeICSDataBlock(QString block);
    bool addVALARMBlockToICS();
};

#endif // CALENDAR_ITEM_H
