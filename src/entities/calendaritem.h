#ifndef CALENDAR_ITEM_H
#define CALENDAR_ITEM_H

#include <QDateTime>
#include <QSqlQuery>


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
    static bool addCalendarItemForRequest(QString calendar, QString url);
    static QList<CalendarItem> fetchAllByCalendar(QString calendar);
    static bool deleteAllByCalendar(QString calendar);
    QString getUrl();
    QString getSummary();
    QString getDescription();
    void setSummary(QString text);
    void setDescription(QString text);
    static bool setupTables();
    bool updateWithICSData(QString icsData);
    QString getUid();
    static CalendarItem fetchByUid(QString uid);
    int getPriority();
    QString generateNewICSData();
private:
    int id;
    QString summary;
    QString url;
    QString description;
    QString calendar;
    QString uid;
    QString icsData;
    int priority;
    bool hasDirtyData;
    bool completed;
    QDateTime alarmDate;
    QDateTime created;
    QDateTime modified;
    static QHash<QString, QString> icsDataToHash(QString icsData);
    static QString decodeICSDataLine(QString line);
    static QString findFreeHashKey(QHash<QString, QString> *hash, QString key, int number = 0 );
};

#endif // CALENDAR_ITEM_H
