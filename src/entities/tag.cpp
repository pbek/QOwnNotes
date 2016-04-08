#include "tag.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSettings>


Tag::Tag() {
    id = 0;
    name = "";
}

int Tag::getId() {
    return this->id;
}

QString Tag::getName() {
    return this->name;
}

void Tag::setName(QString text) {
    this->name = text;
}

int Tag::getPriority() {
    return this->priority;
}

void Tag::setPriority(int value) {
    this->priority = value;
}

Tag Tag::fetch(int id) {
    QSqlDatabase db = QSqlDatabase::database("note-folder");
    QSqlQuery query(db);

    Tag tag;

    query.prepare("SELECT * FROM tag WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        tag.fillFromQuery(query);
    }

    return tag;
}

int Tag::countAll() {
    QSqlDatabase db = QSqlDatabase::database("note-folder");
    QSqlQuery query(db);

    query.prepare("SELECT COUNT(*) AS cnt FROM tag");

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        return query.value("cnt").toInt();
    }

    return 0;
}

bool Tag::remove() {
    QSqlDatabase db = QSqlDatabase::database("note-folder");
    QSqlQuery query(db);

    query.prepare("DELETE FROM tag WHERE id = :id");
    query.bindValue(":id", this->id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else {
        return true;
    }
}

Tag Tag::tagFromQuery(QSqlQuery query) {
    Tag tag;
    tag.fillFromQuery(query);
    return tag;
}

bool Tag::fillFromQuery(QSqlQuery query) {
    this->id = query.value("id").toInt();
    this->name = query.value("name").toString();

    return true;
}

QList<Tag> Tag::fetchAll() {
    QSqlDatabase db = QSqlDatabase::database("note-folder");
    QSqlQuery query(db);

    QList<Tag> tagList;

    query.prepare("SELECT * FROM tag ORDER BY priority ASC, id ASC");
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            Tag tag = tagFromQuery(query);
            tagList.append(tag);
        }
    }

    return tagList;
}

/**
 * Inserts or updates a Tag object in the database
 */
bool Tag::store() {
    QSqlDatabase db = QSqlDatabase::database("note-folder");
    QSqlQuery query(db);

    if (this->id > 0) {
        query.prepare(
                "UPDATE tag SET name = :name, priority = :priority "
                        "WHERE id = :id");
        query.bindValue(":id", this->id);
    } else {
        query.prepare(
                "INSERT INTO tag (name, priority) VALUES (:name, :priority)");
    }

    query.bindValue(":name", this->name);
    query.bindValue(":priority", this->priority);

    if (!query.exec()) {
        // on error
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else if (this->id == 0) {
        // on insert
        this->id = query.lastInsertId().toInt();
    }

    return true;
}

/**
 * Checks if the active tag still exists in the database
 */
bool Tag::exists() {
    Tag tag = Tag::fetch(this->id);
    return tag.id > 0;
}

bool Tag::isFetched() {
    return (this->id > 0);
}

void Tag::setAsActive() {
    QSettings settings;
    settings.setValue("activeTagId", id);
}

/**
 * Checks if this note folder is the active one
 */
bool Tag::isActive() {
    return activeTagId() == id;
}

/**
 * Returns the id of the active note folder in the settings
 */
int Tag::activeTagId() {
    QSettings settings;
    return settings.value("activeTagId").toInt();
}

/**
 * Returns the active note folder
 */
Tag Tag::activeTag() {
    return Tag::fetch(activeTagId());
}

QDebug operator<<(QDebug dbg, const Tag &tag) {
    dbg.nospace() << "Tag: <id>" << tag.id << " <name>" <<
            " <priority>" << tag.priority;
    return dbg.space();
}
