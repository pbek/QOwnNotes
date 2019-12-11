#include "tag.h"
#include "notefolder.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSettings>
#include <algorithm>
#include <utils/misc.h>
#include <QStringBuilder>
#include <services/databaseservice.h>


Tag::Tag() :
    id(0), priority(0), parentId(0),
    _color(QColor()), name(QString()) {

  //  id = 0;
  //  name = "";
  //  priority = 0;
  //  parentId = 0;
  //  _color = QColor();
}

int Tag::getId() const {
    return this->id;
}

int Tag::getParentId() const {
    return this->parentId;
}

void Tag::setParentId(const int id) {
    this->parentId = id;
}

QString Tag::getName() const {
    return this->name;
}

void Tag::setName(const QString &text) {
    this->name = text;
}

QColor Tag::getColor() const {
    return this->_color;
}

void Tag::setColor(const QColor &color) {
    this->_color = color;
}

int Tag::getPriority() const {
    return this->priority;
}

void Tag::setPriority(const int value) {
    this->priority = value;
}

Tag Tag::fetch(const int id) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);

    Tag tag;

    query.prepare(QStringLiteral("SELECT * FROM tag WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        tag.fillFromQuery(query);
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return tag;
}

/**
 * Fetches a tag by name
 *
 * @param name
 * @param startsWith if true the tag only has to start with name
 * @return
 */
Tag Tag::fetchByName(QString name, const bool startsWith) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    Tag tag;
    const QString sql = QStringLiteral("SELECT * FROM tag WHERE name ") %
            QString(startsWith ? QStringLiteral("LIKE") : QStringLiteral("="))
                    % QStringLiteral(" :name ORDER BY name");
    query.prepare(sql);

    if (startsWith) {
        name += QStringLiteral("%");
    }

    query.bindValue(QStringLiteral(":name"), name);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        tag.fillFromQuery(query);
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return tag;
}

Tag Tag::fetchByName(const QString &name, const int parentId) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    Tag tag;

    query.prepare(QStringLiteral("SELECT * FROM tag WHERE name = :name AND "
                          "parent_id = :parent_id"));
    query.bindValue(QStringLiteral(":name"), name.toLower());
    query.bindValue(QStringLiteral(":parent_id"), parentId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        tag.fillFromQuery(query);
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return tag;
}

int Tag::countAll() {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);

    query.prepare(QStringLiteral("SELECT COUNT(*) AS cnt FROM tag"));

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        int result = query.value(QStringLiteral("cnt")).toInt();
        DatabaseService::closeDatabaseConnection(db, query);

        return result;
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return 0;
}

/**
 * Removes the tag, their children and its note link items
 */
bool Tag::remove() const {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);

    // remove the tag
    query.prepare(QStringLiteral("DELETE FROM tag WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        DatabaseService::closeDatabaseConnection(db, query);
        return false;
    } else {
        // remove all children tags
        Q_FOREACH(const Tag &tag, fetchAllByParentId(id)) {
                tag.remove();
            }

        QSqlDatabase _db = DatabaseService::getNoteFolderDatabase();
        QSqlQuery _query(_db);

        // remove the note tag links
        _query.prepare(QStringLiteral("DELETE FROM noteTagLink WHERE tag_id = :id"));
        _query.bindValue(QStringLiteral(":id"), id);

        if (!_query.exec()) {
            qWarning() << __func__ << ": " << _query.lastError();
            DatabaseService::closeDatabaseConnection(_db, _query);

            return false;
        } else {
            DatabaseService::closeDatabaseConnection(_db, _query);
            return true;
        }
    }
}

Tag Tag::tagFromQuery(const QSqlQuery& query) {
    Tag tag;
    tag.fillFromQuery(query);
    return tag;
}

bool Tag::fillFromQuery(const QSqlQuery& query) {
    this->id = query.value(QStringLiteral("id")).toInt();
    this->name = query.value(QStringLiteral("name")).toString();
    this->priority = query.value(QStringLiteral("priority")).toInt();
    this->parentId = query.value(QStringLiteral("parent_id")).toInt();

    QString colorName = query.value(colorFieldName()).toString();
    this->_color = colorName.isEmpty() ? QColor() : QColor(colorName);

    return true;
}

QList<Tag> Tag::fetchAll() {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
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
    query.prepare(QStringLiteral("SELECT t.id as id, t.name as name, t.priority as priority, max( "
                      "CASE "
                          "WHEN l.created > t.updated THEN l.created "
                          "ELSE t.updated "
                      "END "
                  ") AS created, t.parent_id as parent_id, "
                  "t.color as color, t.dark_color as dark_color "
                  "FROM tag t LEFT JOIN noteTagLink l ON t.id = l.tag_id "
                  "GROUP BY t.name "
                  "ORDER BY created DESC"));
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            Tag tag = tagFromQuery(query);
            tagList.append(tag);
        }
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return tagList;
}

QList<Tag> Tag::fetchAllByParentId(const int parentId, const QString& sortBy) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    QList<Tag> tagList;

    //query.prepare("SELECT * FROM tag WHERE parent_id = :parentId ORDER BY "
    //                      "priority ASC, name ASC");
    /*
     * See fetchAll(), except we are only interested in tags with a specific parent_id.
    */
    query.prepare(QStringLiteral("SELECT t.id as id, t.name as name, t.priority as priority, max( "
                      "CASE "
                          "WHEN l.created > t.updated THEN l.created "
                          "ELSE t.updated "
                      "END "
                  ") AS created, t.parent_id as parent_id, "
                  "t.color as color, t.dark_color as dark_color "
                  "FROM tag t LEFT JOIN noteTagLink l ON t.id = l.tag_id "
                  "WHERE parent_id = :parentId "
                  "GROUP BY t.name "
                  "ORDER BY ") % sortBy);
    query.bindValue(QStringLiteral(":parentId"), parentId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            Tag tag = tagFromQuery(query);
            tagList.append(tag);
        }
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return tagList;
}

/**
 * Fetches a list of all tags recursively by a parent id
 * The tag of the parent id is also included in the list
 *
 * @param parentId
 * @return
 */
QList<Tag> Tag::fetchRecursivelyByParentId(const int parentId) {
    QList<Tag> tagList = QList<Tag>() << fetch(parentId);

    Q_FOREACH(const Tag &tag, fetchAllByParentId(parentId)) {
            tagList << fetchRecursivelyByParentId(tag.getId());
        }

    return tagList;
}

/**
 * Checks if taggingShowNotesRecursively is set
 */
bool Tag::isTaggingShowNotesRecursively() {
    QSettings settings;
    return settings.value(QStringLiteral("taggingShowNotesRecursively")).toBool();
}

int Tag::countAllParentId(const int parentId) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);

    query.prepare(QStringLiteral("SELECT COUNT(*) AS cnt FROM tag "
                          "WHERE parent_id = :parentId "));
    query.bindValue(QStringLiteral(":parentId"), parentId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        int result = query.value(QStringLiteral("cnt")).toInt();
        DatabaseService::closeDatabaseConnection(db, query);
        return result;
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return 0;
}

/**
 * Checks if the current tag has a child with tagId
 */
bool Tag::hasChild(const int tagId) const {
    Q_FOREACH(const Tag &tag, fetchAllByParentId(id)) {
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
QList<Tag> Tag::fetchAllOfNote(const Note &note) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);

    QList<Tag> tagList;

    query.prepare(QStringLiteral("SELECT t.* FROM tag t "
                          "JOIN noteTagLink l ON t.id = l.tag_id "
                          "WHERE l.note_file_name = :fileName AND "
                          "l.note_sub_folder_path = :noteSubFolderPath "
                          "ORDER BY t.priority ASC, t.name ASC"));
    query.bindValue(QStringLiteral(":fileName"), note.getName());
    query.bindValue(QStringLiteral(":noteSubFolderPath"),
                    note.getNoteSubFolder().relativePath());

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            Tag tag = tagFromQuery(query);
            tagList.append(tag);
        }
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return tagList;
}

/**
 * Fetches all linked tags of a list of notes
 */
QList<Tag> Tag::fetchAllOfNotes(const QList<Note> &notes) {
    QList<Tag> resultTagList;

    Q_FOREACH (const Note &note, notes) {
            QList<Tag> tagList = Tag::fetchAllOfNote(note);

            Q_FOREACH (const Tag &tag, tagList) {
                    if (!resultTagList.contains(tag)) {
                        resultTagList.append(tag);
                    }
            }
    }

    std::sort(resultTagList.begin(), resultTagList.end());

    return resultTagList;
}

/**
 * Fetches the names of all linked tags of a note
 */
QStringList Tag::fetchAllNamesOfNote(const Note &note) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    QStringList tagNameList;

    query.prepare(QStringLiteral("SELECT t.name FROM tag t "
                          "JOIN noteTagLink l ON t.id = l.tag_id "
                          "WHERE l.note_file_name = :fileName AND "
                          "l.note_sub_folder_path = :noteSubFolderPath "
                          "ORDER BY t.priority ASC, t.name ASC"));
    query.bindValue(QStringLiteral(":fileName"), note.getName());
    query.bindValue(QStringLiteral(":noteSubFolderPath"),
                    note.getNoteSubFolder().relativePath());

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            tagNameList << query.value(QStringLiteral("name")).toString();
        }
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return tagNameList;
}

/**
 * Fetches the names by substring searching for the name
 */
QStringList Tag::searchAllNamesByName(const QString& name) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    QStringList tagNameList;

    query.prepare(QStringLiteral("SELECT name FROM tag "
                          "WHERE name LIKE :name "
                          "ORDER BY priority ASC, name ASC"));
    query.bindValue(QStringLiteral(":name"), "%" + name + "%");

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            tagNameList << query.value(QStringLiteral("name")).toString();
        }
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return tagNameList;
}

/**
 * Fetches one Tag of a note that has a color
 */
Tag Tag::fetchOneOfNoteWithColor(const Note &note) {
    Q_FOREACH(const Tag &tag, fetchAllOfNote(note)) {
            if (tag.getColor().isValid()) {
                return tag;
            }
        }

    return Tag();
}

/**
 * Count all linked tags of a note
 */
int Tag::countAllOfNote(const Note &note) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);

    query.prepare(QStringLiteral("SELECT COUNT(*) AS cnt FROM noteTagLink "
                          "WHERE note_file_name = :fileName AND "
                          "note_sub_folder_path = :noteSubFolderPath"));
    query.bindValue(QStringLiteral(":fileName"), note.getName());
    query.bindValue(QStringLiteral(":noteSubFolderPath"),
                    note.getNoteSubFolder().relativePath());

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        int result = query.value(QStringLiteral("cnt")).toInt();
        DatabaseService::closeDatabaseConnection(db, query);

        return result;
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return 0;
}

/**
 * Checks if tag is linked to a note
 */
bool Tag::isLinkedToNote(const Note &note) const {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);

    query.prepare(QStringLiteral("SELECT COUNT(*) AS cnt FROM noteTagLink "
                          "WHERE note_file_name = :fileName AND "
                          "note_sub_folder_path = :noteSubFolderPath "
                          "AND tag_id = :tagId"));
    query.bindValue(QStringLiteral(":fileName"), note.getName());
    query.bindValue(QStringLiteral(":noteSubFolderPath"),
                    note.getNoteSubFolder().relativePath());
    query.bindValue(QStringLiteral(":tagId"), id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        bool result = query.value(QStringLiteral("cnt")).toInt() > 0;
        DatabaseService::closeDatabaseConnection(db, query);

        return result;
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return false;
}

/**
 * Returns all tags that are linked to certain note names
 */
QList<Tag> Tag::fetchAllWithLinkToNoteNames(const QStringList& noteNameList) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    QList<Tag> tagList;
    QString noteIdListString = noteNameList.join("','");

    QString sql = QStringLiteral(
            "SELECT t.* FROM tag t "
                "JOIN noteTagLink l ON t.id = l.tag_id "
                "WHERE l.note_file_name IN ('%1') AND "
                    "l.note_sub_folder_path = :noteSubFolderPath "
                "GROUP BY t.id "
                "ORDER BY t.priority ASC, t.name ASC")
            .arg(noteIdListString);
    query.prepare(sql);
    query.bindValue(QStringLiteral(":noteSubFolderPath"),
                    NoteSubFolder::activeNoteSubFolder().relativePath());

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            Tag tag = tagFromQuery(query);
            tagList.append(tag);
        }
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return tagList;
}

/**
 * Fetches all linked note file names
 */

QStringList Tag::fetchAllLinkedNoteFileNames(const bool fromAllSubfolders) const {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    QStringList fileNameList;

    if (fromAllSubfolders) {
         // 'All notes' selected in note subfolder panel
        query.prepare(QStringLiteral("SELECT note_file_name FROM noteTagLink WHERE tag_id = :id"));
    } else {
        query.prepare(QStringLiteral("SELECT note_file_name FROM noteTagLink WHERE tag_id = :id "
                              "AND note_sub_folder_path LIKE :noteSubFolderPath"));
        query.bindValue(QStringLiteral(":noteSubFolderPath"),
                    NoteSubFolder::activeNoteSubFolder().relativePath() + "%");
    }

    query.bindValue(QStringLiteral(":id"), this->id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            fileNameList.append(query.value(QStringLiteral("note_file_name")).toString());
        }
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return fileNameList;
}

/**
 * Fetches all linked note file names for a given subfolder
 */

QStringList Tag::fetchAllLinkedNoteFileNamesForFolder(const NoteSubFolder &noteSubFolder, bool fromAllSubfolders) const {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    QStringList fileNameList;

    if (fromAllSubfolders) {
         // 'All notes' selected in note subfolder panel
        query.prepare(QStringLiteral("SELECT note_file_name FROM noteTagLink WHERE tag_id = :id"));
    } else {
        query.prepare(QStringLiteral("SELECT note_file_name FROM noteTagLink WHERE tag_id = :id "
                              "AND note_sub_folder_path LIKE :noteSubFolderPath"));
        query.bindValue(QStringLiteral(":noteSubFolderPath"), noteSubFolder.relativePath() + "%");
    }

    query.bindValue(QStringLiteral(":id"), this->id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            fileNameList.append(query.value(QStringLiteral("note_file_name")).toString());
        }
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return fileNameList;
}

/**
 * Fetches all linked notes
 */
QList<Note> Tag::fetchAllLinkedNotes() const {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    QList<Note> noteList;

    query.prepare(QStringLiteral("SELECT note_file_name, note_sub_folder_path FROM "
                  "noteTagLink WHERE tag_id = :id"));
    query.bindValue(QStringLiteral(":id"), this->id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            QString fileName = query.value(QStringLiteral("note_file_name")).toString();
            QString noteSubFolderPath = query.value(QStringLiteral("note_sub_folder_path")).toString();
            auto noteSubFolder = NoteSubFolder::fetchByPathData(noteSubFolderPath, QStringLiteral("/"));
            auto note = Note::fetchByName(fileName, noteSubFolder.getId());

            noteList << note;
        }
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return noteList;
}

/**
 * Converts backslashes to slashes in the noteTagLink table to fix
 * problems with Windows
 */
void Tag::convertDirSeparator() {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);

    query.prepare(QStringLiteral("UPDATE noteTagLink SET note_sub_folder_path = replace("
                          "note_sub_folder_path, '\\', '/')"));

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    }

    DatabaseService::closeDatabaseConnection(db, query);
}

/**
 * Fetches all tag names
 */
QStringList Tag::fetchAllNames() {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);

    QStringList nameList;

    query.prepare(QStringLiteral("SELECT name FROM tag ORDER BY name"));

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            nameList.append(query.value(QStringLiteral("name")).toString());
        }
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return nameList;
}

/**
 * Count the linked note file names for a note folder
 */
int Tag::countLinkedNoteFileNamesForNoteFolder(const NoteSubFolder &noteSubFolder, const bool recursive) const {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);

    if (recursive) {
        query.prepare(QStringLiteral("SELECT COUNT(note_file_name) AS cnt FROM noteTagLink "
                              "WHERE tag_id = :id AND "
                              "note_sub_folder_path LIKE :noteSubFolderPath"));
        query.bindValue(QStringLiteral(":noteSubFolderPath"),
                        noteSubFolder.relativePath() + QLatin1Char('%'));
    } else {
        query.prepare(QStringLiteral("SELECT COUNT(note_file_name) AS cnt FROM noteTagLink "
                      "WHERE tag_id = :id AND "
                      "note_sub_folder_path = :noteSubFolderPath"));
        query.bindValue(QStringLiteral(":noteSubFolderPath"),
                        noteSubFolder.relativePath() + QLatin1Char('%'));
    }
    query.bindValue(QStringLiteral(":id"), this->id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        int result = query.value(QStringLiteral("cnt")).toInt();
        DatabaseService::closeDatabaseConnection(db, query);

        return result;
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return 0;
}

/**
 * Count the linked note file names
 */
int Tag::countLinkedNoteFileNames(const bool fromAllSubfolders, const bool recursive) const {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);

    if (fromAllSubfolders) {
    query.prepare(QStringLiteral("SELECT COUNT(note_file_name) AS cnt FROM noteTagLink "
                          "WHERE tag_id = :id"));
    } else if (recursive) {
        query.prepare(QStringLiteral("SELECT COUNT(note_file_name) AS cnt FROM noteTagLink "
                              "WHERE tag_id = :id AND "
                              "note_sub_folder_path LIKE :noteSubFolderPath"));
        query.bindValue(QStringLiteral(":noteSubFolderPath"),
                        NoteSubFolder::activeNoteSubFolder().relativePath() + "%");
    } else {
        query.prepare(QStringLiteral("SELECT COUNT(note_file_name) AS cnt FROM noteTagLink "
                      "WHERE tag_id = :id AND "
                      "note_sub_folder_path = :noteSubFolderPath"));
        query.bindValue(QStringLiteral(":noteSubFolderPath"),
                        NoteSubFolder::activeNoteSubFolder().relativePath());
    }
    query.bindValue(QStringLiteral(":id"), this->id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        int result = query.value(QStringLiteral("cnt")).toInt();
        DatabaseService::closeDatabaseConnection(db, query);

        return result;
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return 0;
}

/**
 * Inserts or updates a Tag object in the database
 */
bool Tag::store() {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    QString colorField = colorFieldName();

    if (this->id > 0) {
        query.prepare(QStringLiteral("UPDATE tag SET name = :name, priority = :priority, "
                        "parent_id = :parentId, ") % colorField % QStringLiteral(" = :color, "
                        "updated = datetime('now') "
                        "WHERE id = :id"));
        query.bindValue(QStringLiteral(":id"), this->id);
    } else {
        query.prepare(
                QStringLiteral("INSERT INTO tag (name, priority, parent_id, ") % colorField %
                        QStringLiteral(") VALUES (:name, :priority, :parentId, :color)"));
    }

    query.bindValue(QStringLiteral(":name"), this->name);
    query.bindValue(QStringLiteral(":priority"), this->priority);
    query.bindValue(QStringLiteral(":parentId"), this->parentId);
    query.bindValue(QStringLiteral(":color"), _color.isValid() ? _color.name() : QString());

    if (!query.exec()) {
        // on error
        qWarning() << __func__ << ": " << query.lastError();

        DatabaseService::closeDatabaseConnection(db, query);
        return false;
    } else if (this->id == 0) {
        // on insert
        this->id = query.lastInsertId().toInt();
    }

    // update the parent tag for correct sorting by last use
    if (this->parentId > 0) {
        QSqlQuery parentQuery(db);
        parentQuery.prepare(QStringLiteral("SELECT * FROM tag WHERE id = :parentId"));
        parentQuery.bindValue(QStringLiteral(":parentId"), this->parentId);

        if (!parentQuery.exec()) {
            qWarning() << __func__ << ": " << query.lastError();
        } else {
            if(parentQuery.next()) {
                Tag parent = tagFromQuery(parentQuery);
                parent.store();
            }
        }
    }

    DatabaseService::closeDatabaseConnection(db, query);

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
    return settings.value(QStringLiteral("darkMode")).toBool() ?
                QStringLiteral("dark_color") : QStringLiteral("color");
}

/**
 * Links a note to a tag
 */
bool Tag::linkToNote(const Note &note) const {
    if (!isFetched()) {
        return false;
    }

    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    query.prepare(QStringLiteral("INSERT INTO noteTagLink (tag_id, note_file_name, "
                          "note_sub_folder_path) "
                          "VALUES (:tagId, :noteFileName, "
                          ":noteSubFolderPath)"));

    query.bindValue(QStringLiteral(":tagId"), this->id);
    query.bindValue(QStringLiteral(":noteFileName"), note.getName());
    query.bindValue(QStringLiteral(":noteSubFolderPath"),
                    note.getNoteSubFolder().relativePath());

    if (!query.exec()) {
        // we should not show this warning, because we don't check if a
        // link to a note already exists before we try to create an other link
//        qWarning() << __func__ << ": " << query.lastError();

        DatabaseService::closeDatabaseConnection(db, query);

        return false;
    }

    // update the parent tag for correct sorting by last use
    if (this->parentId > 0) {
        QSqlQuery parentQuery(db);
        parentQuery.prepare(QStringLiteral("SELECT * FROM tag WHERE id = :parentId"));
        parentQuery.bindValue(QStringLiteral(":parentId"), this->parentId);

        if (!parentQuery.exec()) {
            qWarning() << __func__ << ": " << query.lastError();
        } else {
            if(parentQuery.next()) {
                Tag parent = tagFromQuery(parentQuery);
                parent.store();
            }
        }
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return true;
}

/**
 * Removes the link to a note
 */
bool Tag::removeLinkToNote(const Note &note) const {
    if (!isFetched()) {
        return false;
    }

    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    query.prepare(QStringLiteral("DELETE FROM noteTagLink WHERE tag_id = :tagId AND "
                          "note_file_name = :noteFileName AND "
                          "note_sub_folder_path = :noteSubFolderPath"));

    query.bindValue(QStringLiteral(":tagId"), this->id);
    query.bindValue(QStringLiteral(":noteFileName"), note.getName());
    query.bindValue(QStringLiteral(":noteSubFolderPath"),
                    note.getNoteSubFolder().relativePath());

    if (!query.exec()) {
        // on error
        qWarning() << __func__ << ": " << query.lastError();

        DatabaseService::closeDatabaseConnection(db, query);

        return false;
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return true;
}

/**
 * Removes all links to a note
 */
bool Tag::removeAllLinksToNote(const Note &note) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    query.prepare(QStringLiteral("DELETE FROM noteTagLink WHERE "
                          "note_file_name = :noteFileName AND "
                          "note_sub_folder_path = :noteSubFolderPath"));

    query.bindValue(QStringLiteral(":noteFileName"), note.getName());
    query.bindValue(QStringLiteral(":noteSubFolderPath"),
                    note.getNoteSubFolder().relativePath());

    if (!query.exec()) {
        // on error
        qWarning() << __func__ << ": " << query.lastError();

        DatabaseService::closeDatabaseConnection(db, query);

        return false;
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return true;
}

/**
 * Removes all broken note tag links
 */
void Tag::removeBrokenLinks() {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);

    query.prepare(QStringLiteral("SELECT * FROM notetaglink"));
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            QString noteFileName = query.value(QStringLiteral("note_file_name")).toString();
            QString noteSubFolderPath = query.value(
                    QStringLiteral("note_sub_folder_path")).toString();

            NoteSubFolder noteSubFolder = NoteSubFolder::fetchByPathData(
                    noteSubFolderPath, QStringLiteral("/"));
            Note note = Note::fetchByName(noteFileName, noteSubFolder.getId());

            // remove note tag link if note doesn't exist
            if (!note.exists()) {
                int id = query.value(QStringLiteral("id")).toInt();
                removeNoteLinkById(id);
            }
        }
    }

    DatabaseService::closeDatabaseConnection(db, query);
}

/**
 * Removes a note tag link by its id
 *
 * @param id
 * @return
 */
bool Tag::removeNoteLinkById(const int id) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    query.prepare(QStringLiteral("DELETE FROM noteTagLink WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), id);

    if (!query.exec()) {
        // on error
        qWarning() << __func__ << ": " << query.lastError();

        DatabaseService::closeDatabaseConnection(db, query);
        return false;
    }

    DatabaseService::closeDatabaseConnection(db, query);
    return true;
}


/**
 * Renames the note file name of note links
 */
bool Tag::renameNoteFileNamesOfLinks(const QString& oldFileName, const QString& newFileName) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    query.prepare(QStringLiteral("UPDATE noteTagLink SET note_file_name = :newFileName WHERE "
                          "note_file_name = :oldFileName AND "
                          "note_sub_folder_path = :noteSubFolderPath"));

    query.bindValue(QStringLiteral(":oldFileName"), oldFileName);
    query.bindValue(QStringLiteral(":newFileName"), newFileName);
    // TODO: relying on NoteSubFolder::activeNoteSubFolder() can make troubles if "All notes" are selected as current tag
    query.bindValue(QStringLiteral(":noteSubFolderPath"),
                    NoteSubFolder::activeNoteSubFolder().relativePath());

    if (!query.exec()) {
        // on error
        qWarning() << __func__ << ": " << query.lastError();

        DatabaseService::closeDatabaseConnection(db, query);
        return false;
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return true;
}

/**
 * Renames the note sub folder paths of note links
 */
bool Tag::renameNoteSubFolderPathsOfLinks(const QString &oldPath, const QString &newPath) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    query.prepare(QStringLiteral("UPDATE noteTagLink SET note_sub_folder_path = "
                  "replace(note_sub_folder_path, :oldPath, :newPath) WHERE "
                          "note_sub_folder_path LIKE :oldPathLike"));

    query.bindValue(QStringLiteral(":oldPath"), oldPath);
    query.bindValue(QStringLiteral(":oldPathLike"), oldPath + "%");
    query.bindValue(QStringLiteral(":newPath"), newPath);

    if (!query.exec()) {
        // on error
        qWarning() << __func__ << ": " << query.lastError();

        DatabaseService::closeDatabaseConnection(db, query);
        return false;
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return true;
}

/**
 * Checks if the active tag still exists in the database
 */
bool Tag::exists() const {
    Tag tag = Tag::fetch(this->id);
    return tag.id > 0;
}

bool Tag::isFetched() const {
    return (this->id > 0);
}

void Tag::setAsActive() {
    Tag::setAsActive(id);
}

void Tag::setAsActive(const int tagId) {
    NoteFolder noteFolder = NoteFolder::currentNoteFolder();
    noteFolder.setActiveTagId(tagId);
    noteFolder.store();
}

/**
 * Checks if this note folder is the active one
 */
bool Tag::isActive() const {
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
    bool darkMode = settings.value(QStringLiteral("darkMode")).toBool();

    // disable dark mode to get the light color
    settings.setValue(QStringLiteral("darkMode"), false);

    // fetch all tags with non-dark mode colors
    QList <Tag> tags = fetchAll();

    // enable dark mode to later set the dark color
    settings.setValue(QStringLiteral("darkMode"), true);

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
    settings.setValue(QStringLiteral("darkMode"), darkMode);
}

bool Tag::operator==(const Tag &tag) const {
    return id == tag.id;
}

bool Tag::operator<(const Tag &tag) const {
    return name < tag.name;
}

QDebug operator<<(QDebug dbg, const Tag &tag) {
    dbg.nospace() << "Tag: <id>" << tag.id << " <name>" << tag.name <<
            " <parentId>" << tag.parentId;
    return dbg.space();
}
