#include "tag.h"
#include "note.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSettings>


Tag::Tag() {
    id = 0;
    name = "";
    priority = 0;
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
    QSqlDatabase db = QSqlDatabase::database("note_folder");
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

Tag Tag::fetchByName(QString name) {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);

    Tag tag;

    query.prepare("SELECT * FROM tag WHERE LOWER(name) = :name");
    query.bindValue(":name", name.toLower());

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        tag.fillFromQuery(query);
    }

    return tag;
}

int Tag::countAll() {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);

    query.prepare("SELECT COUNT(*) AS cnt FROM tag");

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        return query.value("cnt").toInt();
    }

    return 0;
}

/**
 * Removes the tag and its note link items
 */
bool Tag::remove() {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);

    // remove the tag
    query.prepare("DELETE FROM tag WHERE id = :id");
    query.bindValue(":id", this->id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else {
        // remove the note tag links
        query.prepare("DELETE FROM noteTagLink WHERE tag_id = :id");
        query.bindValue(":id", this->id);

        if (!query.exec()) {
            qWarning() << __func__ << ": " << query.lastError();
            return false;
        } else {
            return true;
        }
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
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);

    QList<Tag> tagList;

    query.prepare("SELECT * FROM tag ORDER BY priority ASC, name ASC");
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
 * Fetches all linked tags of a note
 */
QList<Tag> Tag::fetchAllOfNote(Note note) {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);

    QList<Tag> tagList;

    query.prepare("SELECT t.* FROM tag t "
                          "JOIN noteTagLink l ON t.id = l.tag_id "
                          "WHERE l.note_file_name = :fileName "
                          "ORDER BY t.priority ASC, t.name ASC");
    query.bindValue(":fileName", note.getName());

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
 * Checks if tag is linked to a note
 */
bool Tag::isLinkedToNote(Note note) {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);

    query.prepare("SELECT COUNT(*) AS cnt FROM noteTagLink "
                          "WHERE note_file_name = :fileName "
                          "AND tag_id = :tagId");
    query.bindValue(":fileName", note.getName());
    query.bindValue(":fileName", id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        return query.value("cnt").toInt() > 0;
    }

    return false;
}

/**
 * Fetches all linked note file names
 */
QStringList Tag::fetchAllLinkedNoteFileNames() {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);

    QStringList fileNameList;

    query.prepare("SELECT note_file_name FROM noteTagLink WHERE tag_id = :id");
    query.bindValue(":id", this->id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            fileNameList.append(query.value("note_file_name").toString());
        }
    }

    return fileNameList;
}

/**
 * Count the linked note file names
 */
int Tag::countLinkedNoteFileNames() {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);

    query.prepare("SELECT COUNT(note_file_name) AS cnt FROM noteTagLink "
                          "WHERE tag_id = :id");
    query.bindValue(":id", this->id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        return query.value("cnt").toInt();
    }

    return 0;
}

/**
 * Inserts or updates a Tag object in the database
 */
bool Tag::store() {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
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
 * Links a note to a tag
 */
bool Tag::linkToNote(Note note) {
    if (!isFetched()) {
        return false;
    }

    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);
    query.prepare("INSERT INTO noteTagLink (tag_id, note_file_name) "
                           "VALUES (:tagId, :noteFileName)");

    query.bindValue(":tagId", this->id);
    query.bindValue(":noteFileName", note.getName());

    if (!query.exec()) {
        // on error
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    }

    return true;
}

/**
 * Removes the link to a note
 */
bool Tag::removeLinkToNote(Note note) {
    if (!isFetched()) {
        return false;
    }

    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);
    query.prepare("DELETE FROM noteTagLink WHERE tag_id = :tagId AND "
                          "note_file_name = :noteFileName");

    query.bindValue(":tagId", this->id);
    query.bindValue(":noteFileName", note.getName());

    if (!query.exec()) {
        // on error
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    }

    return true;
}

/**
 * Removes all links to a note
 */
bool Tag::removeAllLinksToNote(Note note) {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);
    query.prepare("DELETE FROM noteTagLink WHERE "
                          "note_file_name = :noteFileName");

    query.bindValue(":noteFileName", note.getName());

    if (!query.exec()) {
        // on error
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    }

    return true;
}

/**
 * Renames the note file name of note links
 */
bool Tag::renameNoteFileNamesOfLinks(QString oldFileName, QString newFileName) {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);
    query.prepare("UPDATE noteTagLink SET note_file_name = :newFileName WHERE "
                          "note_file_name = :oldFileName");

    query.bindValue(":oldFileName", oldFileName);
    query.bindValue(":newFileName", newFileName);

    if (!query.exec()) {
        // on error
        qWarning() << __func__ << ": " << query.lastError();
        return false;
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
