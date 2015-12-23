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
    this->icsDataKeyList = new QStringList();
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

QString CalendarItem::getICSData()
{
    return this->icsData;
}

QString CalendarItem::getUid()
{
    return this->uid;
}

QString CalendarItem::getDescription()
{
    return this->description;
}

int CalendarItem::getPriority()
{
    return this->priority;
}

bool CalendarItem::getHasDirtyData()
{
    return this->hasDirtyData;
}

QString CalendarItem::getLastModifiedString()
{
    return this->lastModifiedString;
}

QString CalendarItem::getETag()
{
    return this->etag;
}

void CalendarItem::setLastModifiedString(QString text)
{
    this->lastModifiedString = text;
}

void CalendarItem::setETag(QString text)
{
    this->etag = text;
}

void CalendarItem::setSummary(QString text)
{
    this->summary = text;
}

void CalendarItem::setDescription(QString text)
{
    this->description = text;
}

void CalendarItem::setICSData( QString text )
{
    this->icsData = text;
}

void CalendarItem::setPriority( int value )
{
    this->priority = value;
}

bool CalendarItem::setupTables()
{
    QSqlDatabase db = QSqlDatabase::database( "disk" );
    QSqlQuery query( db );

    query.exec("CREATE TABLE calendarItem (id INTEGER PRIMARY KEY,"
            "summary VARCHAR(255), url VARCHAR(255) UNIQUE, description TEXT,"
            "has_dirty_data INTEGER DEFAULT 0,"
            "completed INTEGER DEFAULT 0,"
            "priority INTEGER,"
            "calendar VARCHAR(255),"
            "uid VARCHAR(255),"
            "ics_data TEXT,"
            "alarm_date DATETIME,"
            "etag VARCHAR(255),"
            "last_modified_string VARCHAR(255),"
            "created DATETIME DEFAULT current_timestamp,"
            "modified DATETIME DEFAULT current_timestamp)");

//    query.exec("CREATE UNIQUE INDEX calendarUrl ON calendarItem( calendar, url );");

    return true;
}

/**
 * @brief CalendarItem::addCalendarItemForRequest
 * @param calendar
 * @param url we are using QUrl because be want special characters in the urls translated
 * @return
 */
bool CalendarItem::addCalendarItemForRequest( QString calendar, QUrl url, QString etag, QString lastModifiedString )
{
    QSqlDatabase db = QSqlDatabase::database( "disk" );
    QSqlQuery query( db );

    query.prepare( "INSERT INTO calendarItem ( calendar, url, etag, last_modified_string ) VALUES ( :calendar, :url, :etag, :last_modified_string )" );
    query.bindValue( ":calendar", calendar );
    query.bindValue( ":url", url );
    query.bindValue( ":etag", etag );
    query.bindValue( ":last_modified_string", lastModifiedString );
    return query.exec();
}

bool CalendarItem::addCalendarItem( QString summary, QString url, QString text )
{
    QSqlDatabase db = QSqlDatabase::database( "disk" );
    QSqlQuery query( db );

    query.prepare( "INSERT INTO calendarItem ( summary, url, description ) VALUES ( :summary, :url, :description )" );
    query.bindValue( ":summary", summary );
    query.bindValue( ":url", url );
    query.bindValue( ":description", text );
    return query.exec();
}

CalendarItem CalendarItem::fetch( int id )
{
    QSqlDatabase db = QSqlDatabase::database( "disk" );
    QSqlQuery query( db );

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
    QSqlDatabase db = QSqlDatabase::database( "disk" );
    QSqlQuery query( db );

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

CalendarItem CalendarItem::fetchByUid( QString uid )
{
    CalendarItem calendarItem;
    QSqlDatabase db = QSqlDatabase::database( "disk" );
    QSqlQuery query( db );

    query.prepare( "SELECT * FROM calendarItem WHERE uid = :uid" );
    query.bindValue( ":uid", uid );

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

CalendarItem CalendarItem::fetchByUrl( QUrl url )
{
    CalendarItem calendarItem;
    QSqlDatabase db = QSqlDatabase::database( "disk" );
    QSqlQuery query( db );

    query.prepare( "SELECT * FROM calendarItem WHERE url = :url" );
    query.bindValue( ":url", url );

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
    QSqlDatabase db = QSqlDatabase::database( "disk" );
    QSqlQuery query( db );

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
    this->completed = query.value("completed").toInt() == 1;
    this->priority = query.value("priority").toInt();
    this->uid = query.value("uid").toString();
    this->calendar = query.value("calendar").toString();
    this->icsData = query.value("ics_data").toString();
    this->etag = query.value("etag").toString();
    this->lastModifiedString = query.value("last_modified_string").toString();
    this->alarmDate = query.value("alarm_date").toDateTime();
    this->created = query.value("created").toDateTime();
    this->modified = query.value("modified").toDateTime();

    return true;
}

QList<CalendarItem> CalendarItem::fetchAllByCalendar( QString calendar )
{
    QSqlDatabase db = QSqlDatabase::database( "disk" );
    QSqlQuery query( db );

    QList<CalendarItem> calendarItemList;

    query.prepare( "SELECT * FROM calendarItem WHERE calendar = :calendar ORDER BY completed ASC, priority DESC, modified DESC" );
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
    QSqlDatabase db = QSqlDatabase::database( "disk" );
    QSqlQuery query( db );

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
    QSqlDatabase db = QSqlDatabase::database( "disk" );
    QSqlQuery query( db );

    if ( this->id > 0 )
    {
        query.prepare( "UPDATE calendarItem SET "
                       "summary = :summary, url = :url, description = :description, has_dirty_data = :has_dirty_data, completed = :completed, "
                       "calendar = :calendar, uid = :uid, ics_data = :ics_data, "
                       "etag = :etag, last_modified_string = :last_modified_string, "
                       "alarm_date = :alarm_date, priority = :priority, created = :created, modified = :modified "
                       "WHERE id = :id" );
        query.bindValue( ":id", this->id );
    }
    else
    {
        query.prepare( "INSERT INTO calendarItem"
                       "( summary, url, description, calendar, uid, ics_data, etag, last_modified_string, has_dirty_data, completed, alarm_date, priority, created, modified ) "
                       "VALUES ( :summary, :url, :description, :calendar, :uid, :ics_data, :etag, :last_modified_string, :has_dirty_data, :completed, :alarm_date, :priority, :created, :modified )");
    }

    query.bindValue( ":summary", this->summary );
    query.bindValue( ":url", this->url );
    query.bindValue( ":description", this->description );
    query.bindValue( ":has_dirty_data", this->hasDirtyData ? 1 : 0 );
    query.bindValue( ":completed", this->completed ? 1 : 0 );
    query.bindValue( ":calendar", this->calendar );
    query.bindValue( ":uid", this->uid );
    query.bindValue( ":ics_data", this->icsData );
    query.bindValue( ":etag", this->etag );
    query.bindValue( ":last_modified_string", this->lastModifiedString );
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

/**
 * @brief Generates the new ICS data for the CalendarItem to send to the calendar server
 * @return
 */
QString CalendarItem::generateNewICSData() {
    qDebug() << __func__ << " - 'icsData before': " << icsData;

    // generate a new icsDataHash
    generateICSDataHash();

    // update the icsDataHash
    icsDataHash["SUMMARY"] = summary;
    icsDataHash["DESCRIPTION"] = description;
    icsDataHash["PRIORITY"] = QString::number( priority );

    // check for new keys so that we can send them to the calendar server
    updateICSDataKeyListFromHash();

    icsData.clear();

    // loop through every line in the icsDataHash in the correct order
    for ( int i = 0; i < icsDataKeyList->size(); ++i )
    {
        QString key = icsDataKeyList->at( i );
        QString realKey = key;
        // cut out the numbers at the end
        realKey.replace( QRegularExpression( "\\d*$" ), "" );

        QString line = icsDataHash.value( key );

        // escape "\"s
        line.replace( "\\", "\\\\" );
        // convert newlines
        line.replace( "\n", "\\n" );

        // add a new ics data line
        icsData += realKey + ":" + line + "\n";
    }

    qDebug() << __func__ << " - 'icsData after': " << icsData;

    return icsData;
}

/**
 * @brief Checks if there are new entries in the icsDataHash (e.g. when we want to set a new property that wasn't there before) and updates the icsDataKeyList
 */
void CalendarItem::updateICSDataKeyListFromHash() {

    QStringList keysToAdd;

    // look for new keys
    QHashIterator<QString, QString> i( icsDataHash );
    while ( i.hasNext() )
    {
        i.next();
        QString key = i.key();

        // we found a new key
        if ( !icsDataKeyList->contains( key ) )
        {
            keysToAdd.append( key );
        }
    }

    if ( keysToAdd.size() > 0 )
    {
        int indexToAddKeys = -1;

        // search for the index where we should add the new keys
        for ( int i = 0; i < icsDataKeyList->size(); ++i )
        {
            QString key = icsDataKeyList->at( i );

            if ( key.startsWith( "BEGIN" ) && ( icsDataHash.value( key ) == "VTODO" ) )
            {
                // we want to add our new element after the BEGIN:VTODO
                indexToAddKeys = i + 1;
                break;
            }
        }

        // abort if we didn't find an index to add our new keys
        if ( indexToAddKeys == -1 ) {
            return;
        }

        // add the new keys
        for ( int i = 0; i < keysToAdd.size(); ++i )
        {
            QString key = keysToAdd.at( i );

            icsDataKeyList->insert( indexToAddKeys, key );
        }
    }
}

//
// deletes all calendarItems in the database
//
bool CalendarItem::deleteAllByCalendar( QString calendar ) {
    QSqlDatabase db = QSqlDatabase::database( "disk" );
    QSqlQuery query( db );

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

bool CalendarItem::isCompleted() {
    return this->completed;
}

bool CalendarItem::updateWithICSData( QString icsData )
{
    this->icsData = icsData;

    // parse and transform the ics data to a hash with the data
    generateICSDataHash();

    // we only need VTODO items!
    if ( icsDataHash["BEGIN1"] != "VTODO" ) {
        return false;
    }

    QString dateFormat = "yyyyMMddThhmmssZ";

    this->summary = icsDataHash.contains( "SUMMARY" ) ? icsDataHash["SUMMARY"] : "";
    this->completed = icsDataHash.contains( "PERCENT-COMPLETE" ) ? icsDataHash["PERCENT-COMPLETE"] == "100" : false;
    this->uid = icsDataHash.contains( "UID" ) ? icsDataHash["UID"] : "";
    this->description = icsDataHash.contains( "DESCRIPTION" ) ? icsDataHash["DESCRIPTION"] : "";
    this->priority = icsDataHash.contains( "PRIORITY" ) ? icsDataHash["PRIORITY"].toInt() : 0;
    this->created = icsDataHash.contains( "CREATED" ) ? QDateTime::fromString( icsDataHash["CREATED"], dateFormat ) : QDateTime::currentDateTime();
    this->modified = icsDataHash.contains( "LAST-MODIFIED" ) ? QDateTime::fromString( icsDataHash["LAST-MODIFIED"], dateFormat ) : QDateTime::currentDateTime();

    return this->store();
}

/**
 * @brief Parses and transforms the ics data to a hash with the data
 * @param icsData
 * @return
 */
void CalendarItem::generateICSDataHash()
{
    QRegularExpression regex;
    QRegularExpressionMatch match;
    QString lastKey;
    icsDataKeyList->clear();

    QStringList iscDataLines = icsData.split( "\n" );

    QListIterator<QString> i( iscDataLines );
    while ( i.hasNext() )
    {
        QString line = i.next();

        // mutli-line text stats with a space
        if ( !line.startsWith( " " ) )
        {
            // remove the trailing \n
            line.chop( 1 );
            // parse key and value
            regex.setPattern( "^([A-Z\\-_=;]+):(.+)$" );
            match = regex.match( line );

            // set last key for multi line texts
            lastKey = match.captured(1);

            if ( lastKey == "" ) {
                continue;
            }

            // find a free key
            lastKey = findFreeHashKey( &icsDataHash, lastKey );

            // add new key / value pair to the hash
            icsDataHash[lastKey] = decodeICSDataLine( match.captured(2) );
//            hash.insert( lastKey, decodeICSDataLine( match.captured(2) ) );

            // add the key to the key order list
            icsDataKeyList->append( lastKey );
        }
        else
        {
            // remove the trailing \n
            line.chop( 1 );

            // remove leading space
            regex.setPattern( "^ (.+)$" );
            match = regex.match( line );

            // add text to last line
            icsDataHash[lastKey] += decodeICSDataLine( match.captured(1) );
        }
    }
}

/**
 * @brief Finds a free key in a hash
 * @param hash
 * @param key
 * @param number
 * @return
 */
QString CalendarItem::findFreeHashKey( QHash<QString, QString> *hash, QString key, int number )
{
    // give up after 1000 tries
    if ( number >= 1000 )
    {
        return key;
    }

    QString newKey = key;

    // if number is bigger than 0 add the number to the key
    if ( number > 0 )
    {
        newKey = key + QString::number( number );
    }

    // if the key is taken find increase number and try to find a new key
    if ( hash->value( newKey ) != "" )
    {
        return findFreeHashKey( hash, key, ++number );
    }

    return newKey;
}


/**
 * @brief Decodes an ics data line
 * @param line
 * @return
 */
QString CalendarItem::decodeICSDataLine( QString line )
{
//    qDebug() << __func__ << " - 'before line': " << line;

    // replace \n with newlines
    // we have to replace this twice, because of the first character that gets replaces in multiple \n
    line.replace( QRegularExpression( "([^\\\\])\\\\n" ), "\\1\n" );
    line.replace( QRegularExpression( "([^\\\\])\\\\n" ), "\\1\n" );

    // replace \\ with "\"
    line.replace( "\\\\", "\\" );

//    qDebug() << __func__ << " - 'after line':  " << line;
    return line;
}


QDebug operator<<(QDebug dbg, const CalendarItem &calendarItem)
{
    dbg.nospace() << "CalendarItem: <id>" << calendarItem.id << " <summary>" << calendarItem.summary << " <url>" << calendarItem.url << " <calendar>" << calendarItem.calendar;
    return dbg.space();
}
