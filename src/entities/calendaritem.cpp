#include "calendaritem.h"

#include <qregularexpression.h>
#include <services/owncloudservice.h>
#include <utils/misc.h>

#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QRegularExpression>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QUuid>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QTimeZone>
#endif

#include "services/settingsservice.h"

CalendarItem::CalendarItem() {
    id = 0;
    hasDirtyData = false;
    icsDataKeyList = QStringList();
    priority = 0;
    sortPriority = 0;
    summary = QString();
    description = QString();
    completed = false;
}

int CalendarItem::getId() { return this->id; }

QString CalendarItem::getSummary() { return this->summary; }

QString CalendarItem::getCalendar() { return this->calendar; }

QString CalendarItem::getUrl() { return this->url; }

QString CalendarItem::getICSData() { return this->icsData; }

QString CalendarItem::getUid() const { return this->uid; }

QString CalendarItem::getRelatedUid() const { return this->relatedUid; }

QString CalendarItem::getDescription() { return this->description; }

QString CalendarItem::getTags() { return this->tags; }

int CalendarItem::getPriority() { return this->priority; }

bool CalendarItem::getHasDirtyData() { return this->hasDirtyData; }

QString CalendarItem::getLastModifiedString() { return this->lastModifiedString; }

QString CalendarItem::getETag() { return this->etag; }

QDateTime CalendarItem::getAlarmDate() { return this->alarmDate; }

void CalendarItem::setLastModifiedString(const QString &text) { this->lastModifiedString = text; }

void CalendarItem::setETag(const QString &text) { this->etag = text; }

void CalendarItem::setSummary(const QString &text) { this->summary = text; }

void CalendarItem::setUrl(const QUrl &url) { this->url = url.toString(); }

void CalendarItem::setCalendar(const QString &text) { this->calendar = text; }

void CalendarItem::setDescription(const QString &text) { this->description = text; }

void CalendarItem::setICSData(const QString &text) { this->icsData = text; }

void CalendarItem::setUid(const QString &text) { this->uid = text; }

void CalendarItem::setRelatedUid(const QString &text) { this->relatedUid = text; }

void CalendarItem::setCreated(const QDateTime &dateTime) { this->created = dateTime; }

void CalendarItem::setModified(const QDateTime &dateTime) { this->modified = dateTime; }

void CalendarItem::setAlarmDate(const QDateTime &dateTime) { this->alarmDate = dateTime; }

void CalendarItem::setPriority(int value) { this->priority = value; }

void CalendarItem::setCompleted(bool value) { this->completed = value; }

void CalendarItem::setTags(QString tags) { this->tags = tags; };

void CalendarItem::updateCompleted(bool value) {
    this->completed = value;
    if (value) {
        this->completedDate = QDateTime::currentDateTime();
    }
}

/**
 * CalendarItem::addCalendarItemForRequest
 * @param calendar
 * @param url we are using QUrl because be want special characters in the urls
 * translated
 * @return
 */
bool CalendarItem::addCalendarItemForRequest(const QString &calendar, const QUrl &url,
                                             const QString &etag,
                                             const QString &lastModifiedString) {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    query.prepare(
        "INSERT INTO calendarItem ( calendar, url, etag,"
        " last_modified_string ) VALUES ( :calendar, :url, :etag,"
        " :last_modified_string )");
    query.bindValue(QStringLiteral(":calendar"), calendar);
    query.bindValue(QStringLiteral(":url"), url);
    query.bindValue(QStringLiteral(":etag"), etag);
    query.bindValue(QStringLiteral(":last_modified_string"), lastModifiedString);
    return query.exec();
}

bool CalendarItem::addCalendarItem(const QString &summary, const QString &url,
                                   const QString &text) {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    query.prepare(
        "INSERT INTO calendarItem ( summary, url, description )"
        " VALUES ( :summary, :url, :description )");
    query.bindValue(QStringLiteral(":summary"), summary);
    query.bindValue(QStringLiteral(":url"), url);
    query.bindValue(QStringLiteral(":description"), text);
    return query.exec();
}

CalendarItem CalendarItem::fetch(int id) {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    CalendarItem calendarItem;

    query.prepare(QStringLiteral("SELECT * FROM calendarItem WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        calendarItem.fillFromQuery(query);
    }

    return calendarItem;
}

CalendarItem CalendarItem::fetchByUrlAndCalendar(const QString &url, const QString &calendar) {
    CalendarItem calendarItem;
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    query.prepare(
        "SELECT * FROM calendarItem WHERE url = :url AND "
        "calendar = :calendar");
    query.bindValue(QStringLiteral(":url"), url);
    query.bindValue(QStringLiteral(":calendar"), calendar);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        calendarItem.fillFromQuery(query);
    }

    return calendarItem;
}

CalendarItem CalendarItem::fetchByUid(const QString &uid) {
    CalendarItem calendarItem;
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    query.prepare(QStringLiteral("SELECT * FROM calendarItem WHERE uid = :uid"));
    query.bindValue(QStringLiteral(":uid"), uid);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        calendarItem.fillFromQuery(query);
    }

    return calendarItem;
}

CalendarItem CalendarItem::fetchByUrl(const QUrl &url) {
    CalendarItem calendarItem;
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    query.prepare(QStringLiteral("SELECT * FROM calendarItem WHERE url = :url"));
    query.bindValue(QStringLiteral(":url"), url);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        calendarItem.fillFromQuery(query);
    }

    return calendarItem;
}

bool CalendarItem::remove() {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    query.prepare(QStringLiteral("DELETE FROM calendarItem WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), this->id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else {
        return true;
    }
}

/**
 * Deletes all calendar items
 */
bool CalendarItem::removeAll() {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    query.prepare(QStringLiteral("DELETE FROM calendarItem"));

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else {
        return true;
    }
}

CalendarItem CalendarItem::calendarItemFromQuery(const QSqlQuery &query) {
    CalendarItem calendarItem;
    calendarItem.fillFromQuery(query);
    return calendarItem;
}

bool CalendarItem::fillFromQuery(const QSqlQuery &query) {
    this->id = query.value(QStringLiteral("id")).toInt();
    this->summary = query.value(QStringLiteral("summary")).toString();
    this->url = query.value(QStringLiteral("url")).toString();
    this->description = query.value(QStringLiteral("description")).toString();
    this->hasDirtyData = query.value(QStringLiteral("has_dirty_data")).toInt() == 1;
    this->completed = query.value(QStringLiteral("completed")).toInt() == 1;
    this->priority = query.value(QStringLiteral("priority")).toInt();
    this->uid = query.value(QStringLiteral("uid")).toString();
    this->relatedUid = query.value(QStringLiteral("related_uid")).toString();
    this->calendar = query.value(QStringLiteral("calendar")).toString();
    this->icsData = query.value(QStringLiteral("ics_data")).toString();
    this->etag = query.value(QStringLiteral("etag")).toString();
    this->lastModifiedString = query.value(QStringLiteral("last_modified_string")).toString();
    this->alarmDate = query.value(QStringLiteral("alarm_date")).toDateTime();
    this->created = query.value(QStringLiteral("created")).toDateTime();
    this->modified = query.value(QStringLiteral("modified")).toDateTime();
    this->completedDate = query.value(QStringLiteral("completed_date")).toDateTime();
    this->sortPriority = query.value(QStringLiteral("sort_priority")).toInt();
    this->tags = query.value(QStringLiteral("tags")).toString();
    return true;
}

QList<CalendarItem> CalendarItem::fetchAllByCalendar(const QString &calendar) {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    QList<CalendarItem> calendarItemList;

    query.prepare(
        "SELECT * FROM calendarItem WHERE calendar = :calendar ORDER BY "
        "completed ASC, sort_priority DESC, modified DESC");
    query.bindValue(QStringLiteral(":calendar"), calendar);
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            CalendarItem calendarItem = calendarItemFromQuery(query);
            calendarItemList.append(calendarItem);
        }
    }

    return calendarItemList;
}

QList<CalendarItem> CalendarItem::fetchAll() {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    QList<CalendarItem> calendarItemList;

    query.prepare(QStringLiteral("SELECT * FROM calendarItem"));
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            CalendarItem calendarItem = calendarItemFromQuery(query);
            calendarItemList.append(calendarItem);
        }
    }

    return calendarItemList;
}

/**
 * Fetches tasks for the system tray menu
 */
QList<CalendarItem> CalendarItem::fetchAllForSystemTray(int limit) {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    QList<CalendarItem> calendarItemList;

    query.prepare(
        "SELECT * FROM calendarItem WHERE completed = 0 "
        "ORDER BY priority DESC, modified DESC "
        "LIMIT :limit");
    query.bindValue(QStringLiteral(":limit"), limit);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            CalendarItem calendarItem = calendarItemFromQuery(query);
            calendarItemList.append(calendarItem);
        }
    }

    return calendarItemList;
}

/**
 * Fetches all calendar items with an alarm date in the current minute
 * @return
 */
QList<CalendarItem> CalendarItem::fetchAllForReminderAlert() {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    QList<CalendarItem> calendarItemList;

    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();

    QTime timeFrom = QTime();
    timeFrom.setHMS(time.hour(), time.minute(), 0);

    QTime timeTo = QTime();
    timeTo.setHMS(time.hour(), time.minute(), 59);

    QDateTime dateFrom = QDateTime(date, timeFrom);
    QDateTime dateTo = QDateTime(date, timeTo);

    query.prepare(
        "SELECT * FROM calendarItem WHERE alarm_date >= "
        ":alarm_data_from AND alarm_date <= :alarm_data_to");
    query.bindValue(QStringLiteral(":alarm_data_from"), dateFrom);
    query.bindValue(QStringLiteral(":alarm_data_to"), dateTo);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            CalendarItem calendarItem = calendarItemFromQuery(query);
            calendarItemList.append(calendarItem);
        }
    }

    return calendarItemList;
}

QList<QUrl> CalendarItem::fetchAllUrlsByCalendar(const QString &calendar) {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);
    QList<QUrl> urlList;

    query.prepare(QStringLiteral("SELECT url FROM calendarItem WHERE calendar = :calendar"));
    query.bindValue(QStringLiteral(":calendar"), calendar);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            urlList.append(query.value(QStringLiteral("url")).toString());
        }
    }

    return urlList;
}

QList<CalendarItem> CalendarItem::search(const QString &text) {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    QList<CalendarItem> calendarItemList;

    query.prepare(
        "SELECT * FROM calendarItem"
        "WHERE description LIKE :text"
        "ORDER BY sort_priority DESC");

    query.bindValue(QStringLiteral(":text"), "%" + text + "%");

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            CalendarItem calendarItem = calendarItemFromQuery(query);
            calendarItemList.append(calendarItem);
        }
    }

    return calendarItemList;
}

/**
 * Returns a list of UIDs for the search in the TodoDialog
 */
QList<QString> CalendarItem::searchAsUidList(const QString &text, const QString &calendar) {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);
    QList<QString> resultList;

    query.prepare(
        "SELECT uid FROM calendarItem "
        "WHERE ( description LIKE :text OR summary LIKE :text ) AND "
        "calendar = :calendar "
        "ORDER BY sort_priority DESC");

    query.bindValue(QStringLiteral(":text"), "%" + text + "%");
    query.bindValue(QStringLiteral(":calendar"), calendar);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            resultList.append(query.value(QStringLiteral("uid")).toString());
        }
    }

    return resultList;
}

void CalendarItem::updateSortPriority() {
    if (priority == 0) {
        sortPriority = 50;
    } else {
        sortPriority = (10 - priority) * 10;
    }
}

/**
 * Updates all priorities of calendar items
 */
void CalendarItem::updateAllSortPriorities() {
    QList<CalendarItem> calendarItemList = fetchAll();

    QListIterator<CalendarItem> itr(calendarItemList);
    while (itr.hasNext()) {
        CalendarItem calItem = itr.next();
        calItem.updateSortPriority();
        calItem.store();
    }
}

//
// inserts or updates a CalendarItem object in the database
//
bool CalendarItem::store() {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    this->updateSortPriority();

    if (this->id > 0) {
        query.prepare(
            "UPDATE calendarItem SET summary = :summary, url = :url, "
            "description = :description, "
            "tags = :tags, "
            "has_dirty_data = :has_dirty_data, "
            "completed = :completed, "
            "calendar = :calendar, uid = :uid, related_uid = :related_uid, "
            "ics_data = :ics_data, etag = :etag, "
            "last_modified_string = :last_modified_string, "
            "alarm_date = :alarm_date, priority = :priority, "
            "created = :created, modified = :modified, "
            "completed_date = :completed_date, "
            "sort_priority = :sort_priority WHERE id = :id");
        query.bindValue(QStringLiteral(":id"), this->id);
    } else {
        query.prepare(
            "INSERT INTO calendarItem "
            "(summary, url, description, tags, calendar, uid, related_uid, ics_data, "
            "etag, last_modified_string, has_dirty_data, completed, "
            "alarm_date, priority, created, modified, "
            "completed_date, sort_priority) "
            "VALUES (:summary, :url, :description, :tags, :calendar, :uid,"
            " :related_uid, :ics_data, :etag, :last_modified_string, "
            ":has_dirty_data, :completed, :alarm_date, :priority, "
            ":created, :modified, :completed_date, "
            ":sort_priority)");
    }

    query.bindValue(QStringLiteral(":summary"), this->summary);
    query.bindValue(QStringLiteral(":url"), this->url);
    query.bindValue(QStringLiteral(":description"), this->description);
    query.bindValue(QStringLiteral(":has_dirty_data"), this->hasDirtyData ? 1 : 0);
    query.bindValue(QStringLiteral(":completed"), this->completed ? 1 : 0);
    query.bindValue(QStringLiteral(":calendar"), this->calendar);
    query.bindValue(QStringLiteral(":uid"), this->uid);
    query.bindValue(QStringLiteral(":related_uid"), this->relatedUid);
    query.bindValue(QStringLiteral(":ics_data"), this->icsData);
    query.bindValue(QStringLiteral(":etag"), this->etag);
    query.bindValue(QStringLiteral(":last_modified_string"), this->lastModifiedString);
    query.bindValue(QStringLiteral(":alarm_date"), this->alarmDate);
    query.bindValue(QStringLiteral(":priority"), this->priority);
    query.bindValue(QStringLiteral(":created"), this->created);
    query.bindValue(QStringLiteral(":modified"), this->modified);
    query.bindValue(QStringLiteral(":completed_date"), this->completedDate);
    query.bindValue(QStringLiteral(":sort_priority"), this->sortPriority);
    query.bindValue(QStringLiteral(":tags"), this->tags);

    // on error
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else if (this->id == 0) {    // on insert
        this->id = query.lastInsertId().toInt();
    }

    return true;
}

/**
 * Generates the new ICS data for the CalendarItem to send to the
 * calendar server
 * @return
 */
QString CalendarItem::generateNewICSData() {
    qDebug() << __func__ << " - 'icsData before': " << icsData;

    // generate a new icsDataHash
    generateICSDataHash();

    //    qDebug() << __func__ << " - 'icsDataHash after generateICSDataHash': "
    //    << icsDataHash; qDebug() << __func__ << " - 'icsDataKeyList after
    //    generateICSDataHash': " << icsDataKeyList->join( "," );

    // update the icsDataHash
    icsDataHash[QStringLiteral("SUMMARY")] = summary;
    icsDataHash[QStringLiteral("DESCRIPTION")] = description;
    // If categories are empty, remove the CATEGORIES data
    // otherwise we get empty tag
    if (tags.isEmpty()) {
        icsDataHash.remove(QStringLiteral("CATEGORIES"));
        icsDataKeyList.removeAll(QStringLiteral("CATEGORIES"));
    } else {
        // Turn single backslashes into two for categories when uploading
        icsDataHash[QStringLiteral("CATEGORIES")] = tags;
    }
    icsDataHash[QStringLiteral("UID")] = uid;
    icsDataHash[QStringLiteral("PRIORITY")] = QString::number(priority);
    icsDataHash[QStringLiteral("PERCENT-COMPLETE")] = QString::number(completed ? 100 : 0);
    icsDataHash[QStringLiteral("STATUS")] = completed ? "COMPLETED" : "NEEDS-ACTION";
    //    icsDataHash["CREATED"] =
    //    created.toUTC().toString(ICS_DATETIME_FORMAT);
    //    icsDataHash["LAST-MODIFIED"] =
    //            modified.toUTC().toString(ICS_DATETIME_FORMAT);
    icsDataHash[QStringLiteral("CREATED")] = created.toString(ICS_DATETIME_FORMAT);
    icsDataHash[QStringLiteral("LAST-MODIFIED")] = modified.toString(ICS_DATETIME_FORMAT);

    if (!relatedUid.isEmpty()) {
        icsDataHash[QStringLiteral("RELATED-TO")] = relatedUid;
    }

    if (completed) {
        //        icsDataHash["COMPLETED"] =
        //                completedDate.toUTC().toString(ICS_DATETIME_FORMAT);
        icsDataHash[QStringLiteral("COMPLETED")] = completedDate.toString(ICS_DATETIME_FORMAT);
    } else {
        // remove the "COMPLETED" ics data attribute if the task item is not
        // completed
        icsDataHash.remove(QStringLiteral("COMPLETED"));
        icsDataKeyList.removeAll(QStringLiteral("COMPLETED"));
    }

    removeICSDataBlock(QStringLiteral("VALARM"));

    // set the alarm if needed
    if (alarmDate.isValid()) {
        icsDataHash[QStringLiteral("DUE")] = alarmDate.toString(ICS_DATETIME_FORMAT);

        // Nextcloud seems to have problem with the VALARM block
        //        addVALARMBlockToICS();
    } else {
        icsDataHash.remove(QStringLiteral("DUE"));
        icsDataKeyList.removeAll(QStringLiteral("DUE"));
    }

    // check for new keys so that we can send them to the calendar server
    updateICSDataKeyListFromHash();

    //    qDebug() << __func__ << " - 'icsDataHash after
    //    updateICSDataKeyListFromHash': " << icsDataHash; qDebug() << __func__
    //    << " - 'icsDataKeyList after updateICSDataKeyListFromHash': " <<
    //    icsDataKeyList->join( "," );

    icsData.clear();

    // loop through every line in the icsDataHash in the correct order
    for (int i = 0; i < icsDataKeyList.size(); ++i) {
        QString key = icsDataKeyList.at(i);
        QString realKey = key;
        // cut out the numbers at the end
        realKey.replace(QRegularExpression(QStringLiteral("\\d*$")), QString());

        QString line = icsDataHash.value(key);

        // commas only have one backslash
        if (key != QStringLiteral("CATEGORIES")) {
            line.replace(QLatin1String("\\"), QLatin1String("\\\\"));
        }
        // convert newlines
        line.replace(QLatin1String("\n"), QLatin1String("\\n"));

        // add a new ics data line
        icsData += realKey + ":" + line + "\n";
    }

    qDebug() << __func__ << " - 'icsData after': " << icsData;

    return icsData;
}

/**
 * Checks if there are new entries in the icsDataHash (e.g. when we want to set
 * a new property that wasn't there before) and updates the icsDataKeyList
 */
void CalendarItem::updateICSDataKeyListFromHash() {
    QStringList keysToAdd;

    // look for new keys
    QHashIterator<QString, QString> i(icsDataHash);
    while (i.hasNext()) {
        i.next();
        QString key = i.key();

        // we found a new key
        if (!icsDataKeyList.contains(key)) {
            keysToAdd.append(key);
        }
    }

    if (keysToAdd.size() > 0) {
        int indexToAddKeys = -1;

        // search for the index where we should add the new keys
        for (int i = 0; i < icsDataKeyList.size(); ++i) {
            QString key = icsDataKeyList.at(i);

            if (key.startsWith(QLatin1String("BEGIN")) &&
                (icsDataHash.value(key) == QLatin1String("VTODO"))) {
                // we want to add our new element after the BEGIN:VTODO
                indexToAddKeys = i + 1;
                break;
            }
        }

        // abort if we didn't find an index to add our new keys
        if (indexToAddKeys == -1) {
            return;
        }

        // add the new keys
        for (int i = 0; i < keysToAdd.size(); ++i) {
            QString key = keysToAdd.at(i);

            icsDataKeyList.insert(indexToAddKeys, key);
        }
    }
}

//
// deletes all calendarItems of a calendar in the database
//
bool CalendarItem::deleteAllByCalendar(const QString &calendar) {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    query.prepare(QStringLiteral("DELETE FROM calendarItem WHERE calendar = :calendar"));
    query.bindValue(QStringLiteral(":calendar"), calendar);
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else {
        return true;
    }
}

//
// checks if the current calendarItem still exists in the database
//
bool CalendarItem::exists() {
    CalendarItem calendarItem = CalendarItem::fetch(this->id);
    return calendarItem.id > 0;
}

bool CalendarItem::isFetched() { return (this->id > 0); }

bool CalendarItem::isCompleted() { return this->completed; }

bool CalendarItem::updateWithICSData(const QString &icsData) {
    this->icsData = icsData;

    // parse and transform the ics data to a hash with the data
    generateICSDataHash();

    // we only need VTODO items!
    //    if ( ( icsDataHash["BEGIN1"] != "VTODO" ) &&
    //         ( icsDataHash["BEGIN"] != "VTODO" ) ) {
    //        return false;
    //    }

    summary = icsDataHash.contains(QStringLiteral("SUMMARY"))
                  ? icsDataHash[QStringLiteral("SUMMARY")].trimmed()
                  : QString();
    completed = icsDataHash.contains(QStringLiteral("PERCENT-COMPLETE"))
                    ? icsDataHash[QStringLiteral("PERCENT-COMPLETE")] == QLatin1String("100")
                    : false;

    // also take the completed status into account
    if (!completed) {
        completed = icsDataHash.contains(QStringLiteral("STATUS")) &&
                    icsDataHash[QStringLiteral("STATUS")] == QLatin1String("COMPLETED");
    }

    uid = icsDataHash.contains(QStringLiteral("UID")) ? icsDataHash[QStringLiteral("UID")]
                                                      : QString();
    relatedUid = icsDataHash.contains(QStringLiteral("RELATED-TO"))
                     ? icsDataHash[QStringLiteral("RELATED-TO")]
                     : QString();
    description = icsDataHash.contains(QStringLiteral("DESCRIPTION"))
                      ? icsDataHash[QStringLiteral("DESCRIPTION")]
                      : QString();

    // Turn double backslashes into one for categories
    tags = icsDataHash.contains(QStringLiteral("CATEGORIES"))
               ? icsDataHash[QStringLiteral("CATEGORIES")]
               : QString();

    priority = icsDataHash.contains(QStringLiteral("PRIORITY"))
                   ? icsDataHash[QStringLiteral("PRIORITY")].toInt()
                   : 0;
    created = icsDataHash.contains(QStringLiteral("CREATED"))
                  ? getDateTimeFromString(icsDataHash[QStringLiteral("CREATED")])
                  : QDateTime::currentDateTime();
    modified = icsDataHash.contains(QStringLiteral("LAST-MODIFIED"))
                   ? getDateTimeFromString(icsDataHash[QStringLiteral("LAST-MODIFIED")])
                   : QDateTime::currentDateTime();

    // workaround to check if we have a alarm description, so that on empty
    // descriptions the alarm description isn't taken
    QString alarmDescription =
        getICSDataAttributeInBlock(QStringLiteral("VALARM"), QStringLiteral("DESCRIPTION"));
    if ((!description.isEmpty()) && (!alarmDescription.isEmpty()) &&
        (description == alarmDescription)) {
        description = QString();
    }

    QString alarmDateString = getICSDataAttributeInBlock(QStringLiteral("VALARM"),
                                                         QStringLiteral("TRIGGER;VALUE=DATE-TIME"));

    // Nextcloud seems to store the reminder date in the DUE field
    alarmDate = icsDataHash.contains(QStringLiteral("DUE"))
                    ? getDateTimeFromString(icsDataHash[QStringLiteral("DUE")])
                    : QDateTime();

    if (!alarmDateString.isEmpty()) {
        alarmDate = getDateTimeFromString(alarmDateString);
    }

    //    qDebug() << __func__ << " - 'alarmDate': " << alarmDate;
    //    qDebug() << __func__ << " - 'alarmDate': " << alarmDate.isNull();

    return this->store();
}

QDateTime CalendarItem::getDateTimeFromString(const QString &dateString) {
    // Nextcloud Tasks and QOwnNotes are using local dates
    QDateTime dateTime = QDateTime::fromString(dateString, ICS_DATETIME_FORMAT);

    // if the local ICS_DATETIME_FORMAT can't be parsed try the
    // UTC ICS_DATETIME_FORMAT_UTC (e.g. used by OpenTasks or macOS Reminders)
    // and convert it into a local datetime
    // see: https://github.com/pbek/QOwnNotes/issues/1966
    if (!dateTime.isValid()) {
        dateTime = QDateTime::fromString(dateString, ICS_DATETIME_FORMAT_UTC);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        dateTime = QDateTime(dateTime.date(), dateTime.time(), QTimeZone::utc()).toLocalTime();
#else
        dateTime = QDateTime(dateTime.date(), dateTime.time(), Qt::UTC).toLocalTime();
#endif
    }

    return dateTime;
}

/**
 * Searches for an attribute in a block in the ics data
 * @param block
 * @param attributeName
 * @return
 */
QString CalendarItem::getICSDataAttributeInBlock(const QString &block,
                                                 const QString &attributeName) {
    bool blockFound = false;
    for (int i = 0; i < icsDataKeyList.size(); ++i) {
        QString key = icsDataKeyList.at(i);
        QString value = icsDataHash.value(key);

        if (key.startsWith(QLatin1String("BEGIN")) && (value == block)) {
            blockFound = true;
        }

        if (blockFound) {
            if (key.startsWith(attributeName)) {
                return value;
            }
        }
    }

    return QString();
}

/**
 * Removes a block in the ics data
 * @param block
 */
bool CalendarItem::removeICSDataBlock(const QString &block) {
    bool blockFound = false;
    bool doReturn = false;

    // make a copy of the data
    QHash<QString, QString> icsDataHashCopy = icsDataHash;
    QStringList icsDataKeyListCopy = icsDataKeyList;

    for (int i = 0; i < icsDataKeyList.size(); ++i) {
        QString key = icsDataKeyList.at(i);
        QString value = icsDataHash.value(key);

        // look for the begin block
        if (key.startsWith(QLatin1String("BEGIN")) && (value == block)) {
            blockFound = true;
        }

        if (blockFound) {
            // look for the end block
            if (key.startsWith(QLatin1String("END")) && (value == block)) {
                doReturn = true;
            }

            // remove the attributes
            icsDataHashCopy.remove(key);
            icsDataKeyListCopy.removeOne(key);

            if (doReturn) {
                // write the data back and end
                icsDataHash = icsDataHashCopy;
                icsDataKeyList = icsDataKeyListCopy;
                return true;
            }
        }
    }

    return false;
}

/**
 * Adds the VALARM block to the ics data
 */
bool CalendarItem::addVALARMBlockToICS() {
    // make a copy of the data
    QHash<QString, QString> icsDataHashCopy = icsDataHash;
    QStringList icsDataKeyListCopy = icsDataKeyList;

    bool foundBegin = false;

    for (int i = 0; i < icsDataKeyList.size(); ++i) {
        QString key = icsDataKeyList.at(i);
        QString value = icsDataHash.value(key);

        // look for the VTODO begin block
        if (key.startsWith(QLatin1String("BEGIN")) && (value == QLatin1String("VTODO"))) {
            foundBegin = true;
        }

        // add the VALARM block at the end of the VTODO block
        if (foundBegin && key.startsWith(QLatin1String("END")) &&
            (value == QLatin1String("VTODO"))) {
            QString addKey;
            //            QString alarmDateString =
            //                    alarmDate.toUTC().toString(ICS_DATETIME_FORMAT);
            QString alarmDateString = alarmDate.toString(ICS_DATETIME_FORMAT);

            addKey = findFreeHashKey(&icsDataHashCopy, QStringLiteral("BEGIN"));
            icsDataHashCopy[addKey] = QLatin1String("VALARM");
            icsDataKeyListCopy.insert(i, addKey);

            addKey = findFreeHashKey(&icsDataHashCopy, QStringLiteral("ACTION"));
            icsDataHashCopy[addKey] = QLatin1String("DISPLAY");
            icsDataKeyListCopy.insert(++i, addKey);

            addKey = findFreeHashKey(&icsDataHashCopy, QStringLiteral("DESCRIPTION"));
            icsDataHashCopy[addKey] = QLatin1String("Reminder");
            icsDataKeyListCopy.insert(++i, addKey);

            addKey = findFreeHashKey(&icsDataHashCopy, QStringLiteral("TRIGGER;VALUE=DATE-TIME"));
            icsDataHashCopy[addKey] = alarmDateString;
            icsDataKeyListCopy.insert(++i, addKey);

            addKey = findFreeHashKey(&icsDataHashCopy, QStringLiteral("END"));
            icsDataHashCopy[addKey] = QLatin1String("VALARM");
            icsDataKeyListCopy.insert(++i, addKey);

            addKey = findFreeHashKey(&icsDataHashCopy, QStringLiteral("DUE"));
            icsDataHashCopy[addKey] = alarmDateString;
            icsDataKeyListCopy.insert(++i, addKey);

            // write the data back and end
            icsDataHash = icsDataHashCopy;
            icsDataKeyList = icsDataKeyListCopy;
            return true;
        }
    }

    return false;
}

/**
 * Parses and transforms the ics data to a hash with the data
 * @param icsData
 * @return
 */
void CalendarItem::generateICSDataHash() {
    QRegularExpression regex;
    QRegularExpressionMatch match;
    QString lastKey;
    icsDataKeyList.clear();
    icsDataHash.clear();

    QStringList iscDataLines = icsData.split(QStringLiteral("\n"));

    QListIterator<QString> i(iscDataLines);
    while (i.hasNext()) {
        QString line = i.next();
        line.replace(QLatin1String("\r"), QString());

        if (line.isEmpty()) {
            continue;
        }

        // multi-line text stats with a space
        if (!line.startsWith(QLatin1String(" "))) {
            // remove the trailing \n
            if (line.endsWith(QLatin1String("\n"))) {
                line.chop(1);
            }

            // parse key and value
            regex.setPattern(QStringLiteral("^([A-Z\\-_=;]+):(.*)$"));
            match = regex.match(line);

            // set last key for multi line texts
            lastKey = match.captured(1);

            if (lastKey.isEmpty()) {
                continue;
            }

            // find a free key
            lastKey = findFreeHashKey(&icsDataHash, lastKey);

            // add new key / value pair to the hash
            // Categories/tags wont be cleaned since they can have commas in middle of
            // a category
            if (lastKey == "CATEGORIES") {
                icsDataHash[lastKey] = match.captured(2);
            } else {
                icsDataHash[lastKey] = decodeICSDataLine(match.captured(2));
            }
            //            hash.insert( lastKey, decodeICSDataLine(
            //            match.captured(2) ) );

            // add the key to the key order list
            icsDataKeyList.append(lastKey);
        } else {
            // remove the trailing \n
            if (line.endsWith(QLatin1String("\n"))) {
                line.chop(1);
            }

            // remove leading space
            regex.setPattern(QStringLiteral("^ (.+)$"));
            match = regex.match(line);

            // add text to last line
            icsDataHash[lastKey] += decodeICSDataLine(match.captured(1));
        }
    }
}

/**
 * Finds a free key in a hash
 * @param hash
 * @param key
 * @param number
 * @return
 */
QString CalendarItem::findFreeHashKey(QHash<QString, QString> *hash, QString key, int number) {
    // give up after 1000 tries
    if (number >= 1000) {
        return key;
    }

    QString newKey = key;

    // if number is bigger than 0 add the number to the key
    if (number > 0) {
        newKey = key + QString::number(number);
    }

    // if the key is taken find increase number and try to find a new key
    if (hash->contains(newKey)) {
        return findFreeHashKey(hash, key, ++number);
    }

    return newKey;
}

/**
 * Decodes an ics data line
 * @param line
 * @return
 */
QString CalendarItem::decodeICSDataLine(QString line) {
    //    qDebug() << __func__ << " - 'before line': " << line;

    // replace \n with newlines
    // we have to replace this twice, because of the first character that
    // gets replaces in multiple \n
    line.replace(QRegularExpression(QStringLiteral("([^\\\\])\\\\n")), "\\1\n");
    line.replace(QRegularExpression(QStringLiteral("([^\\\\])\\\\n")), "\\1\n");

    // replace "\\" with "\"
    line.replace(QLatin1String("\\\\"), QLatin1String("\\"));

    // replace "\," with ","
    line.replace(QLatin1String("\\,"), QLatin1String(","));

    //    qDebug() << __func__ << " - 'after line':  " << line;
    return line;
}

CalendarItem CalendarItem::createNewTodoItem(const QString &summary, const QString &calendar,
                                             const QString &relatedUid) {
    QString uuidString = Utils::Misc::createUuidString();
    CalendarItem calItem;
    calItem.setSummary(summary);
    calItem.setCalendar(calendar);
    calItem.setRelatedUid(relatedUid);
    calItem.setUrl(QUrl(getCurrentCalendarUrl() + "qownnotes-" + uuidString + ".ics"));
    calItem.setICSData(
        "BEGIN:VCALENDAR\nVERSION:2.0\nPRODID:ownCloud Calendar"
        "\nCALSCALE:GREGORIAN\nBEGIN:VTODO\nEND:VTODO"
        "\nEND:VCALENDAR");
    calItem.setUid(uuidString);

    QDateTime dateTime = QDateTime::currentDateTime();
    calItem.setCreated(dateTime);
    calItem.setModified(dateTime);

    calItem.generateNewICSData();

    if (calItem.store()) {
        qDebug() << __func__ << " - 'calItem': " << calItem;
        qDebug() << __func__ << " - 'calItem.getICSData()': " << calItem.getICSData();
    }

    return calItem;
}

/**
 * Returns the index of the currently selected task list in the todo dialog
 *
 * @return
 */
int CalendarItem::getCurrentCalendarIndex() {
    SettingsService settings;

    QString todoListSelectorSelectedItem =
        settings.value(QStringLiteral("TodoDialog/todoListSelectorSelectedItem")).toString();
    if (!todoListSelectorSelectedItem.isEmpty()) {
        QStringList todoCalendarEnabledList =
            settings.value(QStringLiteral("ownCloud/todoCalendarEnabledList")).toStringList();

        // search for the text in the todoCalendarEnabledList
        int index = todoCalendarEnabledList.indexOf(todoListSelectorSelectedItem);

        if (index >= 0) {
            // return the index of the task list selector if we found it
            return index;
        }
    }

    return -1;
}

/**
 * Returns the url of the currently selected task list in the todo dialog
 * @return
 */
QString CalendarItem::getCurrentCalendarUrl() {
    SettingsService settings;

    int index = getCurrentCalendarIndex();

    if (index >= 0) {
        QStringList todoCalendarEnabledUrlList =
            settings.value(QStringLiteral("ownCloud/todoCalendarEnabledUrlList")).toStringList();
        return todoCalendarEnabledUrlList.at(index);
    }

    return QString();
}

/**
 * Shows alerts for calendar items with an alarm date in the current minute
 */
void CalendarItem::alertTodoReminders() {
    if (!OwnCloudService::isTodoCalendarSupportEnabled()) {
        return;
    }

    QList<CalendarItem> calendarItemList = fetchAllForReminderAlert();

    QListIterator<CalendarItem> itr(calendarItemList);
    while (itr.hasNext()) {
        CalendarItem calItem = itr.next();
        QMessageBox::information(nullptr, QStringLiteral("Reminder"),
                                 "Reminder: <strong>" + calItem.getSummary() + "</strong>");
    }
}

/**
 * Counts all calendar items
 */
int CalendarItem::countAll() {
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QSqlQuery query(db);

    query.prepare(QStringLiteral("SELECT COUNT(*) AS cnt FROM calendarItem"));

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        return query.value(QStringLiteral("cnt")).toInt();
    }

    return 0;
}

QDebug operator<<(QDebug dbg, const CalendarItem &calendarItem) {
    dbg.nospace() << "CalendarItem: <id>" << calendarItem.id << " <summary>" << calendarItem.summary
                  << " <url>" << calendarItem.url << " <relatedUid>" << calendarItem.relatedUid
                  << " <calendar>" << calendarItem.calendar;
    return dbg.space();
}
