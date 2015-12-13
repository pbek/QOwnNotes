#include "calendaritem.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include <QApplication>
#include <QSqlError>
#include <QRegularExpression>


CalendarItem::CalendarItem()
{
    this->id = 0;
    this->hasDirtyData = false;
}

int CalendarItem::getId()
{
    return this->id;
}

QString CalendarItem::getSummary()
{
    return this->summary;
}

QString CalendarItem::getUrl()
{
    return this->url;
}

QString CalendarItem::getUid()
{
    return this->uid;
}

QString CalendarItem::getDescription()
{
    return this->description;
}

bool CalendarItem::getHasDirtyData()
{
    return this->hasDirtyData;
}

void CalendarItem::setSummary(QString text)
{
    this->summary = text;
}

void CalendarItem::setDescription(QString text)
{
    this->description = text;
}

bool CalendarItem::setupTables()
{
    QSqlQuery query;
    query.exec("create table calendarItem (id integer primary key, "
            "summary varchar(255), url varchar(255), description text,"
            "has_dirty_data integer default 0,"
            "priority integer,"
            "calendar varchar(255),"
            "uid varchar(255),"
            "ics_data text,"
            "alarm_date datetime,"
            "created datetime default current_timestamp,"
            "modified datetime default current_timestamp)");

    return true;
}

bool CalendarItem::addCalendarItemForRequest( QString calendar, QString url )
{
    QSqlQuery query;
    query.prepare( "INSERT INTO calendarItem ( calendar, url ) VALUES ( :calendar, :url )" );
    query.bindValue( ":calendar", calendar );
    query.bindValue( ":url", url );
    return query.exec();
}

bool CalendarItem::addCalendarItem( QString summary, QString url, QString text )
{
    QSqlQuery query;
    query.prepare( "INSERT INTO calendarItem ( summary, url, description ) VALUES ( :summary, :url, :description )" );
    query.bindValue( ":summary", summary );
    query.bindValue( ":url", url );
    query.bindValue( ":description", text );
    return query.exec();
}

CalendarItem CalendarItem::fetch( int id )
{
    QSqlQuery query;
    CalendarItem calendarItem;

    query.prepare( "SELECT * FROM calendarItem WHERE id = :id" );
    query.bindValue( ":id", id );

    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
    }
    else if ( query.first() )
    {
        calendarItem.fillFromQuery( query );
    }

    return calendarItem;
}

CalendarItem CalendarItem::fetchByUrlAndCalendar( QString url, QString calendar )
{
    CalendarItem calendarItem;
    QSqlQuery query;

    query.prepare( "SELECT * FROM calendarItem WHERE url = :url AND calendar = :calendar" );
    query.bindValue( ":url", url );
    query.bindValue( ":calendar", calendar );

    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
    }
    else if ( query.first() )
    {
        calendarItem.fillFromQuery( query );
    }

    return calendarItem;
}

bool CalendarItem::remove()
{
    QSqlQuery query;

    query.prepare( "DELETE FROM calendarItem WHERE id = :id" );
    query.bindValue( ":id", this->id );

    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
        return false;
    }
    else
    {
        return true;
    }
}


CalendarItem CalendarItem::calendarItemFromQuery( QSqlQuery query )
{
    CalendarItem calendarItem;
    calendarItem.fillFromQuery( query );
    return calendarItem;
}

bool CalendarItem::fillFromQuery( QSqlQuery query )
{
    this->id = query.value("id").toInt();
    this->summary = query.value("summary").toString();
    this->url = query.value("url").toString();
    this->description = query.value("description").toString();
    this->hasDirtyData = query.value("has_dirty_data").toInt() == 1;
    this->priority = query.value("priority").toInt();
    this->uid = query.value("uid").toString();
    this->calendar = query.value("calendar").toString();
    this->icsData = query.value("ics_data").toString();
    this->alarmDate = query.value("alarm_date").toDateTime();
    this->created = query.value("created").toDateTime();
    this->modified = query.value("modified").toDateTime();

    return true;
}

QList<CalendarItem> CalendarItem::fetchAllByCalendar( QString calendar )
{
    QSqlQuery query;
    QList<CalendarItem> calendarItemList;

    query.prepare( "SELECT * FROM calendarItem WHERE calendar = :calendar ORDER BY priority DESC, modified DESC" );
    query.bindValue( ":calendar", calendar );
    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
    }
    else
    {
        for( int r=0; query.next(); r++ )
        {
            CalendarItem calendarItem = calendarItemFromQuery( query );
            calendarItemList.append( calendarItem );
        }
    }

    return calendarItemList;
}

QList<CalendarItem> CalendarItem::search( QString text )
{
    QSqlQuery query;
    QList<CalendarItem> calendarItemList;

    query.prepare( "SELECT * FROM calendarItem WHERE description LIKE :text ORDER BY priority DESC" );
    query.bindValue( ":text", "%" + text + "%"  );

    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
    }
    else
    {
        for( int r=0; query.next(); r++ )
        {
            CalendarItem calendarItem = calendarItemFromQuery( query );
            calendarItemList.append( calendarItem );
        }
    }

    return calendarItemList;
}


//
// inserts or updates a CalendarItem object in the database
//
bool CalendarItem::store() {
    QSqlQuery query;

    if ( this->id > 0 )
    {
        query.prepare( "UPDATE calendarItem SET "
                       "summary = :summary, url = :url, description = :description, has_dirty_data = :has_dirty_data, "
                       "calendar = :calendar, uid = :uid, ics_data = :ics_data, "
                       "alarm_date = :alarm_date, priority = :priority, created = :created, modified = :modified "
                       "WHERE id = :id" );
        query.bindValue( ":id", this->id );
    }
    else
    {
        query.prepare( "INSERT INTO calendarItem"
                       "( summary, url, description, calendar, uid, ics_data, has_dirty_data, alarm_date, priority, created, modified ) "
                       "VALUES ( :summary, :url, :description, :calendar, :uid, :ics_data, :has_dirty_data, :alarm_date, :priority, :created, :modified )");
    }

    query.bindValue( ":summary", this->summary );
    query.bindValue( ":url", this->url );
    query.bindValue( ":description", this->description );
    query.bindValue( ":has_dirty_data", this->hasDirtyData ? 1 : 0 );
    query.bindValue( ":calendar", this->calendar );
    query.bindValue( ":uid", this->uid );
    query.bindValue( ":ics_data", this->icsData );
    query.bindValue( ":alarm_date", this->alarmDate );
    query.bindValue( ":priority", this->priority );
    query.bindValue( ":created", this->created );
    query.bindValue( ":modified", this->modified );

    // on error
    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
        return false;
    }
    // on insert
    else if ( this->id == 0 )
    {
        this->id = query.lastInsertId().toInt();
    }

    return true;
}

//
// deletes all calendarItems in the database
//
bool CalendarItem::deleteAllByCalendar( QString calendar ) {
    QSqlQuery query;

    query.prepare( "DELETE FROM calendarItem WHERE calendar = :calendar" );
    query.bindValue( ":calendar", calendar );
    if( !query.exec() )
    {
        qDebug() << __func__ << ": " << query.lastError();
        return false;
    }
    else
    {
        return true;
    }
}

//
// checks if the current calendarItem still exists in the database
//
bool CalendarItem::exists() {
    CalendarItem calendarItem = CalendarItem::fetch( this->id );
    return calendarItem.id > 0;
}

bool CalendarItem::isFetched() {
    return ( this->id > 0 );
}

bool CalendarItem::updateWithICSData( QString icsData )
{
    this->icsData = icsData;

    QRegularExpression regex;
    regex.setPatternOptions( QRegularExpression::MultilineOption );
    QRegularExpressionMatch match;
    QString dateFormat = "yyyyMMddThhmmssZ";

    // set the summary
    regex.setPattern( "^SUMMARY:(.+)$" );
    match = regex.match( icsData );
    this->summary = match.captured(1).trimmed();

    // set the UID
    regex.setPattern( "^UID:(.+)$" );
    match = regex.match( icsData );
    this->uid = match.captured(1).trimmed();

    // set the created date
    regex.setPattern( "^CREATED:(.+)$" );
    match = regex.match( icsData );
    this->created = QDateTime::fromString( match.captured(1).trimmed(), dateFormat );

    // set the last modified date
    regex.setPattern( "^LAST-MODIFIED:(.+)$" );
    match = regex.match( icsData );
    this->modified = QDateTime::fromString( match.captured(1).trimmed(), dateFormat );

    // TODO: parse and set more attributes

    return this->store();
}

QDebug operator<<(QDebug dbg, const CalendarItem &calendarItem)
{
    dbg.nospace() << "CalendarItem: <id>" << calendarItem.id << " <summary>" << calendarItem.summary << " <url>" << calendarItem.url << " <calendar>" << calendarItem.calendar;
    return dbg.space();
}
