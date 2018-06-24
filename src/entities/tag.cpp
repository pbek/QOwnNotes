#include "tag.h"
#include "notefolder.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSettings>
#include <utils/misc.h>


Tag::Tag() {
    id = 0;
    name = "";
    priority = 0;
    parentId = 0;
    _color = QColor();
}

int Tag::getId() {
    return this->id;
}

int Tag::getParentId() {
    return this->parentId;
}

void Tag::setParentId(int id) {
    this->parentId = id;
}

QString Tag::getName() {
    return this->name;
}

void Tag::setName(QString text) {
    this->name = text;
}

QColor Tag::getColor() {
    return this->_color;
}

void Tag::setColor(QColor color) {
    this->_color = color;
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

    Utils::Misc::closeDatabaseConnection(db);

    return tag;
}

/**
 * Fetches a tag by name
 *
 * @param name
 * @param startsWith if true the tag only has to start with name
 * @return
 */
Tag Tag::fetchByName(QString name, bool startsWith) {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);
    Tag tag;
    QString sql = "SELECT * FROM tag WHERE name " +
            QString(startsWith ? "LIKE" : "=") + " :name ORDER BY name";
    query.prepare(sql);

    if (startsWith) {
        name += "%";
    }

    query.bindValue(":name", name);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        tag.fillFromQuery(query);
    }

    Utils::Misc::closeDatabaseConnection(db);

    return tag;
}

Tag Tag::fetchByName(QString name, int parentId) {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);
    Tag tag;

    query.prepare("SELECT * FROM tag WHERE name = :name AND "
                          "parent_id = :parent_id");
    query.bindValue(":name", name.toLower());
    query.bindValue(":parent_id", parentId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        tag.fillFromQuery(query);
    }

    Utils::Misc::closeDatabaseConnection(db);

    return tag;
}

int Tag::countAll() {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);

    query.prepare("SELECT COUNT(*) AS cnt FROM tag");

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        Utils::Misc::closeDatabaseConnection(db);

        return query.value("cnt").toInt();
    }

    Utils::Misc::closeDatabaseConnection(db);

    return 0;
}

/**
 * Removes the tag, their children and its note link items
 */
bool Tag::remove() {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);

    // remove the tag
    query.prepare("DELETE FROM tag WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        return false;
    } else {
        // remove all children tags
        Q_FOREACH(Tag tag, fetchAllByParentId(id)) {
                tag.remove();
            }

        QSqlDatabase db = QSqlDatabase::database("note_folder");
        QSqlQuery query(db);

        // remove the note tag links
        query.prepare("DELETE FROM noteTagLink WHERE tag_id = :id");
        query.bindValue(":id", id);

        if (!query.exec()) {
            qWarning() << __func__ << ": " << query.lastError();
            Utils::Misc::closeDatabaseConnection(db);

            return false;
        } else {
            Utils::Misc::closeDatabaseConnection(db);
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
    this->priority = query.value("priority").toInt();
    this->parentId = query.value("parent_id").toInt();

    QString colorName = query.value(colorFieldName()).toString();
    this->_color = colorName.isEmpty() ? QColor() : QColor(colorName);

    return true;
}

QList<Tag> Tag::fetchAll() {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);

    QList<Tag> tagList;

    //query.prepare("SELECT * FROM tag ORDER BY priority ASC, name ASC");
    /*
     * We want to select all relevant fields from the tag table. (AS renames the
     * columns in the output table -- to be sure they have the same names as in the
     * original query, e.g. t.id = id). Since we want them ordered by time of use,
     * also consider the noteTagLink table and join it on the tag id. (Now we get rows
     * | tag.id | tag.updated | link.created | ... | for each tag (with
     * link.created = NULL, if it is not yet linked to a note) and additional rows
     * for each link of a tag after the first as an intermediate result.) Now the
     * CASE ... END selects only the highest created / updated date. At this
     * point there can still be multiple lines with the same tag.id, hence we GROUP BY
     * that to get rid of them and are only interested in the latest / max() created
     * time of every group, which can now be used to sort the result by.
     *
    */
    query.prepare("SELECT t.id as id, t.name as name, t.priority as priority, max( "
                      "CASE "
                          "WHEN l.created > t.updated THEN l.created "
                          "ELSE t.updated "
                      "END "
                  ") AS created, t.parent_id as parent_id, "
                  "t.color as color, t.dark_color as dark_color "
                  "FROM tag t LEFT JOIN noteTagLink l ON t.id = l.tag_id "
                  "GROUP BY t.name "
                  "ORDER BY created DESC");
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            Tag tag = tagFromQuery(query);
            tagList.append(tag);
        }
    }

    Utils::Misc::closeDatabaseConnection(db);

    return tagList;
}

QList<Tag> Tag::fetchAllByParentId(int parentId, QString sortBy) {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);
    QList<Tag> tagList;

    //query.prepare("SELECT * FROM tag WHERE parent_id = :parentId ORDER BY "
    //                      "priority ASC, name ASC");
    /*
     * See fetchAll(), except we are only interested in tags with a specific parent_id.
    */
    query.prepare("SELECT t.id as id, t.name as name, t.priority as priority, max( "
                      "CASE "
                          "WHEN l.created > t.updated THEN l.created "
                          "ELSE t.updated "
                      "END "
                  ") AS created, t.parent_id as parent_id, "
                  "t.color as color, t.dark_color as dark_color "
                  "FROM tag t LEFT JOIN noteTagLink l ON t.id = l.tag_id "
                  "WHERE parent_id = :parentId "
                  "GROUP BY t.name "
                  "ORDER BY " + sortBy);
    query.bindValue(":parentId", parentId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            Tag tag = tagFromQuery(query);
            tagList.append(tag);
        }
    }

    Utils::Misc::closeDatabaseConnection(db);

    return tagList;
}

/**
 * Fetches a list of all tags recursively by a parent id
 * The tag of the parent id is also included in the list
 *
 * @param parentId
 * @return
 */
QList<Tag> Tag::fetchRecursivelyByParentId(int parentId) {
    QList<Tag> tagList = QList<Tag>() << fetch(parentId);

    Q_FOREACH(Tag tag, fetchAllByParentId(parentId)) {
            tagList << fetchRecursivelyByParentId(tag.getId());
        }

    return tagList;
}

/**
 * Checks if taggingShowNotesRecursively is set
 */
bool Tag::isTaggingShowNotesRecursively() {
    QSettings settings;
    return settings.value("taggingShowNotesRecursively").toBool();
}

int Tag::countAllParentId(int parentId) {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);

    query.prepare("SELECT COUNT(*) AS cnt FROM tag "
                          "WHERE parent_id = :parentId ");
    query.bindValue(":parentId", parentId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        Utils::Misc::closeDatabaseConnection(db);
        return query.value("cnt").toInt();
    }

    Utils::Misc::closeDatabaseConnection(db);

    return 0;
}

/**
 * Checks if the current tag has a child with tagId
 */
bool Tag::hasChild(int tagId) {
    Q_FOREACH(Tag tag, fetchAllByParentId(id)) {
            qDebug() << __func__ << " - 'tag': " << tag;

            if ((tag.getId() == tagId) || tag.hasChild(tagId)) {
                return true;
            }
        }

    return false;
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
                          "WHERE l.note_file_name = :fileName AND "
                          "l.note_sub_folder_path = :noteSubFolderPath "
                          "ORDER BY t.priority ASC, t.name ASC");
    query.bindValue(":fileName", note.getName());
    query.bindValue(":noteSubFolderPath",
                    note.getNoteSubFolder().relativePath());

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            Tag tag = tagFromQuery(query);
            tagList.append(tag);
        }
    }

    Utils::Misc::closeDatabaseConnection(db);

    return tagList;
}

/**
 * Fetches the names of all linked tags of a note
 */
QStringList Tag::fetchAllNamesOfNote(Note note) {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);
    QStringList tagNameList;

    query.prepare("SELECT t.name FROM tag t "
                          "JOIN noteTagLink l ON t.id = l.tag_id "
                          "WHERE l.note_file_name = :fileName AND "
                          "l.note_sub_folder_path = :noteSubFolderPath "
                          "ORDER BY t.priority ASC, t.name ASC");
    query.bindValue(":fileName", note.getName());
    query.bindValue(":noteSubFolderPath",
                    note.getNoteSubFolder().relativePath());

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            tagNameList << query.value("name").toString();
        }
    }

    Utils::Misc::closeDatabaseConnection(db);

    return tagNameList;
}

/**
 * Fetches the names by substring searching for the name
 */
QStringList Tag::searchAllNamesByName(QString name) {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);
    QStringList tagNameList;

    query.prepare("SELECT name FROM tag "
                          "WHERE name LIKE :name "
                          "ORDER BY priority ASC, name ASC");
    query.bindValue(":name", "%" + name + "%");

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            tagNameList << query.value("name").toString();
        }
    }

    Utils::Misc::closeDatabaseConnection(db);

    return tagNameList;
}

/**
 * Fetches one Tag of a note that has a color
 */
Tag Tag::fetchOneOfNoteWithColor(Note note) {
    Q_FOREACH(Tag tag, fetchAllOfNote(note)) {
            if (tag.getColor().isValid()) {
                return tag;
            }
        }

    return Tag();
}

/**
 * Count all linked tags of a note
 */
int Tag::countAllOfNote(Note note) {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);

    query.prepare("SELECT COUNT(*) AS cnt FROM noteTagLink "
                          "WHERE note_file_name = :fileName AND "
                          "note_sub_folder_path = :noteSubFolderPath");
    query.bindValue(":fileName", note.getName());
    query.bindValue(":noteSubFolderPath",
                    note.getNoteSubFolder().relativePath());

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        Utils::Misc::closeDatabaseConnection(db);

        return query.value("cnt").toInt();
    }

    Utils::Misc::closeDatabaseConnection(db);

    return 0;
}

/**
 * Checks if tag is linked to a note
 */
bool Tag::isLinkedToNote(Note note) {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);

    query.prepare("SELECT COUNT(*) AS cnt FROM noteTagLink "
                          "WHERE note_file_name = :fileName AND "
                          "note_sub_folder_path = :noteSubFolderPath "
                          "AND tag_id = :tagId");
    query.bindValue(":fileName", note.getName());
    query.bindValue(":noteSubFolderPath",
                    note.getNoteSubFolder().relativePath());
    query.bindValue(":tagId", id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        Utils::Misc::closeDatabaseConnection(db);

        return query.value("cnt").toInt() > 0;
    }

    Utils::Misc::closeDatabaseConnection(db);

    return false;
}

/**
 * Returns all tags that are linked to certain note names
 */
QList<Tag> Tag::fetchAllWithLinkToNoteNames(QStringList noteNameList) {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);
    QList<Tag> tagList;
    QString noteIdListString = noteNameList.join("','");

    QString sql = QString(
            "SELECT t.* FROM tag t "
                "JOIN noteTagLink l ON t.id = l.tag_id "
                "WHERE l.note_file_name IN ('%1') AND "
                    "l.note_sub_folder_path = :noteSubFolderPath "
                "GROUP BY t.id "
                "ORDER BY t.priority ASC, t.name ASC")
            .arg(noteIdListString);
    query.prepare(sql);
    query.bindValue(":noteSubFolderPath",
                    NoteSubFolder::activeNoteSubFolder().relativePath());

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            Tag tag = tagFromQuery(query);
            tagList.append(tag);
        }
    }

    Utils::Misc::closeDatabaseConnection(db);

    return tagList;
}

/**
 * Fetches all linked note file names
 */
QStringList Tag::fetchAllLinkedNoteFileNames(bool fromAllSubfolders) {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);
    QStringList fileNameList;

    if (fromAllSubfolders) {
         // 'All notes' selected in note subfolder panel
        query.prepare("SELECT note_file_name FROM noteTagLink WHERE tag_id = :id");
    } else {
        query.prepare("SELECT note_file_name FROM noteTagLink WHERE tag_id = :id "
                              "AND note_sub_folder_path LIKE :noteSubFolderPath");
        query.bindValue(":noteSubFolderPath",
                    NoteSubFolder::activeNoteSubFolder().relativePath() + "%");
    }

    query.bindValue(":id", this->id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            fileNameList.append(query.value("note_file_name").toString());
        }
    }

    Utils::Misc::closeDatabaseConnection(db);

    return fileNameList;
}

/**
 * Converts backslashes to slashes in the noteTagLink table to fix
 * problems with Windows
 */
void Tag::convertDirSeparator() {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);

    query.prepare("UPDATE noteTagLink SET note_sub_folder_path = replace("
                          "note_sub_folder_path, '\\', '/')");

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    }

    Utils::Misc::closeDatabaseConnection(db);
}

/**
 * Fetches all tag names
 */
QStringList Tag::fetchAllNames() {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);

    QStringList nameList;

    query.prepare("SELECT name FROM tag ORDER BY name");

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            nameList.append(query.value("name").toString());
        }
    }

    Utils::Misc::closeDatabaseConnection(db);

    return nameList;
}

/**
 * Count the linked note file names
 */
int Tag::countLinkedNoteFileNames(bool fromAllSubfolders, bool recursive) {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);

    if (fromAllSubfolders) {
    query.prepare("SELECT COUNT(note_file_name) AS cnt FROM noteTagLink "
                          "WHERE tag_id = :id");
    } else if (recursive) {
        query.prepare("SELECT COUNT(note_file_name) AS cnt FROM noteTagLink "
                              "WHERE tag_id = :id AND "
                              "note_sub_folder_path LIKE :noteSubFolderPath");
        query.bindValue(":noteSubFolderPath",
                        NoteSubFolder::activeNoteSubFolder().relativePath() + "%");
    } else {
        query.prepare("SELECT COUNT(note_file_name) AS cnt FROM noteTagLink "
                      "WHERE tag_id = :id AND "
                      "note_sub_folder_path = :noteSubFolderPath");
        query.bindValue(":noteSubFolderPath",
                        NoteSubFolder::activeNoteSubFolder().relativePath());
    }
    query.bindValue(":id", this->id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        Utils::Misc::closeDatabaseConnection(db);

        return query.value("cnt").toInt();
    }

    Utils::Misc::closeDatabaseConnection(db);

    return 0;
}

/**
 * Inserts or updates a Tag object in the database
 */
bool Tag::store() {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);
    QString colorField = colorFieldName();

    if (this->id > 0) {
        query.prepare(
                "UPDATE tag SET name = :name, priority = :priority, "
                        "parent_id = :parentId, " + colorField + " = :color, "
                        "updated = datetime('now') "
                        "WHERE id = :id");
        query.bindValue(":id", this->id);
    } else {
        query.prepare(
                "INSERT INTO tag (name, priority, parent_id, " + colorField +
                        ") VALUES (:name, :priority, :parentId, :color)");
    }

    query.bindValue(":name", this->name);
    query.bindValue(":priority", this->priority);
    query.bindValue(":parentId", this->parentId);
    query.bindValue(":color", _color.isValid() ? _color.name() : "");

    if (!query.exec()) {
        // on error
        qWarning() << __func__ << ": " << query.lastError();

        Utils::Misc::closeDatabaseConnection(db);
        return false;
    } else if (this->id == 0) {
        // on insert
        this->id = query.lastInsertId().toInt();
    }

    // update the parent tag for correct sorting by last use
    if (this->parentId > 0) {
        QSqlQuery parentQuery(db);
        parentQuery.prepare("SELECT * FROM tag WHERE id = :parentId");
        parentQuery.bindValue(":parentId", this->parentId);

        if (!parentQuery.exec()) {
            qWarning() << __func__ << ": " << query.lastError();
        } else {
            if(parentQuery.next()) {
                Tag parent = tagFromQuery(parentQuery);
                parent.store();
            }
        }
    }

    Utils::Misc::closeDatabaseConnection(db);

    return true;
}

/**
 * Returns the name of the color database field that should be used to load
 * and store tag colors
 *
 * @return
 */
QString Tag::colorFieldName() {
    QSettings settings;
    return settings.value("darkMode").toBool() ? "dark_color" : "color";
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
    query.prepare("INSERT INTO noteTagLink (tag_id, note_file_name, "
                          "note_sub_folder_path) "
                          "VALUES (:tagId, :noteFileName, "
                          ":noteSubFolderPath)");

    query.bindValue(":tagId", this->id);
    query.bindValue(":noteFileName", note.getName());
    query.bindValue(":noteSubFolderPath",
                    note.getNoteSubFolder().relativePath());

    if (!query.exec()) {
        // we should not show this warning, because we don't check if a
        // link to a note already exists before we try to create an other link
//        qWarning() << __func__ << ": " << query.lastError();

        Utils::Misc::closeDatabaseConnection(db);

        return false;
    }

    // update the parent tag for correct sorting by last use
    if (this->parentId > 0) {
        QSqlQuery parentQuery(db);
        parentQuery.prepare("SELECT * FROM tag WHERE id = :parentId");
        parentQuery.bindValue(":parentId", this->parentId);

        if (!parentQuery.exec()) {
            qWarning() << __func__ << ": " << query.lastError();
        } else {
            if(parentQuery.next()) {
                Tag parent = tagFromQuery(parentQuery);
                parent.store();
            }
        }
    }

    Utils::Misc::closeDatabaseConnection(db);

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
                          "note_file_name = :noteFileName AND "
                          "note_sub_folder_path = :noteSubFolderPath");

    query.bindValue(":tagId", this->id);
    query.bindValue(":noteFileName", note.getName());
    query.bindValue(":noteSubFolderPath",
                    note.getNoteSubFolder().relativePath());

    if (!query.exec()) {
        // on error
        qWarning() << __func__ << ": " << query.lastError();

        Utils::Misc::closeDatabaseConnection(db);

        return false;
    }

    Utils::Misc::closeDatabaseConnection(db);

    return true;
}

/**
 * Removes all links to a note
 */
bool Tag::removeAllLinksToNote(Note note) {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);
    query.prepare("DELETE FROM noteTagLink WHERE "
                          "note_file_name = :noteFileName AND "
                          "note_sub_folder_path = :noteSubFolderPath");

    query.bindValue(":noteFileName", note.getName());
    query.bindValue(":noteSubFolderPath",
                    note.getNoteSubFolder().relativePath());

    if (!query.exec()) {
        // on error
        qWarning() << __func__ << ": " << query.lastError();

        Utils::Misc::closeDatabaseConnection(db);

        return false;
    }

    Utils::Misc::closeDatabaseConnection(db);

    return true;
}

/**
 * Removes all broken note tag links
 */
void Tag::removeBrokenLinks() {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);

    query.prepare("SELECT * FROM notetaglink");
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            QString noteFileName = query.value("note_file_name").toString();
            QString noteSubFolderPath = query.value(
                    "note_sub_folder_path").toString();

            NoteSubFolder noteSubFolder = NoteSubFolder::fetchByPathData(
                    noteSubFolderPath, "/");
            Note note = Note::fetchByName(noteFileName, noteSubFolder.getId());

            // remove note tag link if note doesn't exist
            if (!note.exists()) {
                int id = query.value("id").toInt();
                removeNoteLinkById(id);
            }
        }
    }

    Utils::Misc::closeDatabaseConnection(db);
}

/**
 * Removes a note tag link by its id
 *
 * @param id
 * @return
 */
bool Tag::removeNoteLinkById(int id) {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);
    query.prepare("DELETE FROM noteTagLink WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        // on error
        qWarning() << __func__ << ": " << query.lastError();

        Utils::Misc::closeDatabaseConnection(db);
        return false;
    }

    Utils::Misc::closeDatabaseConnection(db);
    return true;
}


/**
 * Renames the note file name of note links
 */
bool Tag::renameNoteFileNamesOfLinks(QString oldFileName, QString newFileName) {
    QSqlDatabase db = QSqlDatabase::database("note_folder");
    QSqlQuery query(db);
    query.prepare("UPDATE noteTagLink SET note_file_name = :newFileName WHERE "
                          "note_file_name = :oldFileName AND "
                          "note_sub_folder_path = :noteSubFolderPath");

    query.bindValue(":oldFileName", oldFileName);
    query.bindValue(":newFileName", newFileName);
    query.bindValue(":noteSubFolderPath",
                    NoteSubFolder::activeNoteSubFolder().relativePath());

    if (!query.exec()) {
        // on error
        qWarning() << __func__ << ": " << query.lastError();

        Utils::Misc::closeDatabaseConnection(db);
        return false;
    }

    Utils::Misc::closeDatabaseConnection(db);

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
    Tag::setAsActive(id);
}

void Tag::setAsActive(int tagId) {
    NoteFolder noteFolder = NoteFolder::currentNoteFolder();
    noteFolder.setActiveTagId(tagId);
    noteFolder.store();
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
    NoteFolder noteFolder = NoteFolder::currentNoteFolder();
    return noteFolder.getActiveTagId();
}

/**
 * Returns the active note folder
 */
Tag Tag::activeTag() {
    return Tag::fetch(activeTagId());
}

/**
 * Sets the non-darkMode colors as darkMode colors for all tags
 */
void Tag::migrateDarkColors() {
    QSettings settings;
    bool darkMode = settings.value("darkMode").toBool();

    // disable dark mode to get the light color
    settings.setValue("darkMode", false);

    // fetch all tags with non-dark mode colors
    QList <Tag> tags = fetchAll();

    // enable dark mode to later set the dark color
    settings.setValue("darkMode", true);

    Q_FOREACH(Tag tag, tags) {
            // get the non-dark mode color (because the fetch was made while
            // "darkMode" was off)
            QColor color = tag.getColor();

            // set the non-dark mode color as dark mode color (because now
            // "darkMode" is enabled)
            tag.setColor(color);
            tag.store();
        }

    // set the dark mode to the old value
    settings.setValue("darkMode", darkMode);
}

QDebug operator<<(QDebug dbg, const Tag &tag) {
    dbg.nospace() << "Tag: <id>" << tag.id << " <name>" << tag.name <<
            " <parentId>" << tag.parentId;
    return dbg.space();
}
