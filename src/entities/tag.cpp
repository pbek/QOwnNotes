#include "tag.h"

#include <services/databaseservice.h>
#include <utils/misc.h>

#include <QDebug>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringBuilder>
#include <algorithm>

#include "note.h"
#include "notefolder.h"
#include "notesubfolder.h"

Tag::Tag() noexcept : _parentId(0), _priority(0) {}

int Tag::getParentId() const { return _parentId; }

void Tag::setParentId(const int id) { _parentId = id; }

QColor Tag::getColor() const { return _color; }

void Tag::setColor(const QColor &color) { _color = color; }

int Tag::getPriority() const { return _priority; }

void Tag::setPriority(const int value) { _priority = value; }

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
                        QString(startsWith ? QStringLiteral("LIKE") : QStringLiteral("=")) %
                        QStringLiteral(" :name ORDER BY name");
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

    query.prepare(
        QStringLiteral("SELECT * FROM tag WHERE name = :name AND "
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
    query.bindValue(QStringLiteral(":id"), _id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
        DatabaseService::closeDatabaseConnection(db, query);
        return false;
    } else {
        // remove all children tags
        const auto tagList = fetchAllByParentId(_id);
        for (const Tag &tag : tagList) {
            tag.remove();
        }

        QSqlDatabase db2 = DatabaseService::getNoteFolderDatabase();
        QSqlQuery query2(db2);

        // remove the note tag links
        query2.prepare(QStringLiteral("DELETE FROM noteTagLink WHERE tag_id = :id"));
        query2.bindValue(QStringLiteral(":id"), _id);

        if (!query2.exec()) {
            qWarning() << __func__ << ": " << query2.lastError();
            DatabaseService::closeDatabaseConnection(db2, query2);

            return false;
        } else {
            DatabaseService::closeDatabaseConnection(db2, query2);
            return true;
        }
    }
}

Tag Tag::tagFromQuery(const QSqlQuery &query) { return Tag().fillFromQuery(query); }

Tag Tag::fillFromQuery(const QSqlQuery &query) {
    _id = query.value(QStringLiteral("id")).toInt();
    _name = query.value(QStringLiteral("name")).toString();
    _priority = query.value(QStringLiteral("priority")).toInt();
    _parentId = query.value(QStringLiteral("parent_id")).toInt();

    const QString colorName = query.value(colorFieldName()).toString();
    _color = colorName.isEmpty() ? QColor() : QColor(colorName);

    return *this;
}

QVector<Tag> Tag::fetchAll() {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);

    QVector<Tag> tagList;

    // query.prepare("SELECT * FROM tag ORDER BY priority ASC, name ASC");
    /*
     * We want to select all relevant fields from the tag table. (AS renames the
     * columns in the output table -- to be sure they have the same names as in
     * the original query, e.g. t.id = id). Since we want them ordered by time
     * of use, also consider the noteTagLink table and join it on the tag id.
     * (Now we get rows | tag.id | tag.updated | link.created | ... | for each
     * tag (with link.created = NULL, if it is not yet linked to a note) and
     * additional rows for each link of a tag after the first as an intermediate
     * result.) Now the CASE ... END selects only the highest created / updated
     * date. At this point there can still be multiple lines with the same
     * tag.id, hence we GROUP BY that to get rid of them and are only interested
     * in the latest / max() created time of every group, which can now be used
     * to sort the result by.
     *
     */
    query.prepare(
        QStringLiteral("SELECT t.id as id, t.name as name, t.priority as priority, max( "
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
            tagList.append(tagFromQuery(query));
        }
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return tagList;
}

QVector<TagHeader> Tag::fetchAllTagHeadersByParentId(const int parentId) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    QVector<TagHeader> tagList;

    query.prepare(
        QStringLiteral("SELECT t.id as id, t.name as name "
                       "FROM tag t LEFT JOIN noteTagLink l ON t.id = l.tag_id "
                       "WHERE parent_id = :parentId "
                       "GROUP BY t.name"));
    query.bindValue(QStringLiteral(":parentId"), parentId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            int id = query.value(QStringLiteral("id")).toInt();
            QString name = query.value(QStringLiteral("name")).toString();
            tagList.append(TagHeader{id, name});
        }
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return tagList;
}

QVector<Tag> Tag::fetchAllByParentId(const int parentId, const QString &sortBy) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    QVector<Tag> tagList;

    // query.prepare("SELECT * FROM tag WHERE parent_id = :parentId ORDER BY "
    //                      "priority ASC, name ASC");
    /*
     * See fetchAll(), except we are only interested in tags with a specific
     * parent_id.
     */
    query.prepare(
        QStringLiteral("SELECT t.id as id, t.name as name, t.priority as priority, max( "
                       "CASE "
                       "WHEN l.created > t.updated THEN l.created "
                       "ELSE t.updated "
                       "END "
                       ") AS created, t.parent_id as parent_id, "
                       "t.color as color, t.dark_color as dark_color "
                       "FROM tag t LEFT JOIN noteTagLink l ON t.id = l.tag_id "
                       "WHERE parent_id = :parentId "
                       "GROUP BY t.name "
                       "ORDER BY :sortBy"));
    query.bindValue(QStringLiteral(":parentId"), parentId);
    query.bindValue(QStringLiteral(":sortBy"), sortBy);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            tagList.append(tagFromQuery(query));
        }
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return tagList;
}

QVector<int> Tag::fetchAllIdsByParentId(const int parentId) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);

    query.prepare(
        QStringLiteral("SELECT t.id as id "
                       "FROM tag t LEFT JOIN noteTagLink l ON t.id = l.tag_id "
                       "WHERE parent_id = :parentId"));

    query.bindValue(QStringLiteral(":parentId"), parentId);

    QVector<int> tagList;
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            int id = query.value(QStringLiteral("id")).toInt();
            tagList.append(id);
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
QVector<Tag> Tag::fetchRecursivelyByParentId(const int parentId) {
    QVector<Tag> tagList = QVector<Tag>{fetch(parentId)};
    const auto tags = fetchAllByParentId(parentId);
    tagList.reserve(tags.size());

    for (const Tag &tag : tags) {
        tagList << fetchRecursivelyByParentId(tag.getId());
    }

    return tagList;
}

QVector<int> Tag::fetchTagIdsRecursivelyByParentId(const int parentId) {
    QVector<int> tagIdList{parentId};
    const auto tagIds = fetchAllIdsByParentId(parentId);
    tagIdList.reserve(tagIds.size());

    for (const int tagId : tagIds) {
        tagIdList << fetchTagIdsRecursivelyByParentId(tagId);
    }

    return tagIdList;
}

QStringList Tag::getParentTagNames() {
    const int parentId = _parentId;
    if (parentId == 0) {
        return QStringList();
    }

    Tag parentTag = Tag::fetch(parentId);

    if (!parentTag.isFetched()) {
        return QStringList();
    }

    const QString tagName = parentTag.getName();
    QStringList tagNames = {tagName};
    tagNames << parentTag.getParentTagNames();

    return tagNames;
}

/**
 * Checks if taggingShowNotesRecursively is set
 */
bool Tag::isTaggingShowNotesRecursively() {
    return QSettings().value(QStringLiteral("taggingShowNotesRecursively")).toBool();
}

int Tag::countAllParentId(const int parentId) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);

    query.prepare(
        QStringLiteral("SELECT COUNT(*) AS cnt FROM tag "
                       "WHERE parent_id = :parentId "));
    query.bindValue(QStringLiteral(":parentId"), parentId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        const int result = query.value(QStringLiteral("cnt")).toInt();
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
    const auto tagList = fetchAllByParentId(_id);
    for (const Tag &tag : tagList) {
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
QVector<Tag> Tag::fetchAllOfNote(const Note &note) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);

    QVector<Tag> tagList;

    query.prepare(
        QStringLiteral("SELECT t.* FROM tag t "
                       "JOIN noteTagLink l ON t.id = l.tag_id "
                       "WHERE l.note_file_name = :fileName AND "
                       "l.note_sub_folder_path = :noteSubFolderPath "
                       "ORDER BY t.priority ASC, t.name ASC"));
    query.bindValue(QStringLiteral(":fileName"), note.getName());
    query.bindValue(QStringLiteral(":noteSubFolderPath"), note.getNoteSubFolder().relativePath());

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            tagList.append(tagFromQuery(query));
        }
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return tagList;
}

/**
 * Fetches all linked tags of a list of notes
 */
QVector<Tag> Tag::fetchAllOfNotes(const QVector<Note> &notes) {
    QVector<Tag> notesTagList;

    // get all tags for the notes list
    for (const Note &note : notes) {
        notesTagList.append(Tag::fetchAllOfNote(note));
    }
    // sort
    std::sort(notesTagList.begin(), notesTagList.end());
    // remove duplicates
    notesTagList.erase(std::unique(notesTagList.begin(), notesTagList.end()), notesTagList.end());

    return notesTagList;
}

/**
 * Fetches the names of all linked tags of a note
 */
QStringList Tag::fetchAllNamesOfNote(const Note &note) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    QStringList tagNameList;

    query.prepare(
        QStringLiteral("SELECT t.name FROM tag t "
                       "JOIN noteTagLink l ON t.id = l.tag_id "
                       "WHERE l.note_file_name = :fileName AND "
                       "l.note_sub_folder_path = :noteSubFolderPath "
                       "ORDER BY t.priority ASC, t.name ASC"));
    query.bindValue(QStringLiteral(":fileName"), note.getName());
    query.bindValue(QStringLiteral(":noteSubFolderPath"), note.getNoteSubFolder().relativePath());

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
 * Fetches the ids of all linked tags of a note
 */
QSet<int> Tag::fetchAllIdsByNote(const Note &note) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    QSet<int> tagIdList;

    query.prepare(
        QStringLiteral("SELECT tag_id FROM noteTagLink "
                       "WHERE note_file_name = :fileName AND "
                       "note_sub_folder_path = :noteSubFolderPath"));
    query.bindValue(QStringLiteral(":fileName"), note.getName());
    query.bindValue(QStringLiteral(":noteSubFolderPath"), note.getNoteSubFolder().relativePath());

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            tagIdList << query.value(QStringLiteral("tag_id")).toInt();
        }
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return tagIdList;
}

/**
 * Fetches the names by substring searching for the name
 */
QStringList Tag::searchAllNamesByName(const QString &name) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    QStringList tagNameList;

    query.prepare(
        QStringLiteral("SELECT name FROM tag "
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
    const auto tagList = fetchAllOfNote(note);
    for (const Tag &tag : tagList) {
        if (tag.getColor().isValid()) {
            return tag;
        }
    }

    return Tag();
}

/**
 * Checks if a note has tags
 */
bool Tag::noteHasTags(const Note &note, const QString &path) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);

    query.prepare(
        QStringLiteral("SELECT "
                       "EXISTS (SELECT tag_id FROM noteTagLink "
                       "WHERE note_file_name=:fileName AND "
                       "note_sub_folder_path=:noteSubFolderPath) AS cnt"));
    query.bindValue(QStringLiteral(":fileName"), note.getName());
    query.bindValue(QStringLiteral(":noteSubFolderPath"),
                    path.isEmpty() ? note.getNoteSubFolder().relativePath() : path);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        int result = query.value(QStringLiteral("cnt")).toInt();
        DatabaseService::closeDatabaseConnection(db, query);

        return result == 1;
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

    query.prepare(
        QStringLiteral("SELECT COUNT(*) AS cnt FROM noteTagLink "
                       "WHERE note_file_name = :fileName AND "
                       "note_sub_folder_path = :noteSubFolderPath "
                       "AND tag_id = :tagId"));
    query.bindValue(QStringLiteral(":fileName"), note.getName());
    query.bindValue(QStringLiteral(":noteSubFolderPath"), note.getNoteSubFolder().relativePath());
    query.bindValue(QStringLiteral(":tagId"), _id);

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
QVector<Tag> Tag::fetchAllWithLinkToNoteNames(const QStringList &noteNameList) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    QVector<Tag> tagList;
    const QString noteIdListString = noteNameList.join(QStringLiteral("','"));

    const QString sql = QStringLiteral(
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
            tagList.append(tagFromQuery(query));
        }
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return tagList;
}

/**
 * Fetches all linked note ids
 */
QVector<int> Tag::fetchAllLinkedNoteIds(int tagId, const bool fromAllSubfolders,
                                        const bool recursive) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    QVector<int> noteIdList;

    if (fromAllSubfolders) {
        // 'All notes' selected in note subfolder panel
        query.prepare(
            QStringLiteral("SELECT note_file_name, note_sub_folder_path "
                           "FROM noteTagLink WHERE tag_id = :id"));
    } else if (recursive) {
        query.prepare(
            QStringLiteral("SELECT note_file_name, note_sub_folder_path "
                           "FROM noteTagLink WHERE tag_id = :id "
                           "AND note_sub_folder_path LIKE :noteSubFolderPath"));
        query.bindValue(QStringLiteral(":noteSubFolderPath"),
                        NoteSubFolder::activeNoteSubFolder().relativePath() + "%");
    } else {
        query.prepare(
            QStringLiteral("SELECT note_file_name, note_sub_folder_path "
                           "FROM noteTagLink WHERE tag_id = :id "
                           "AND note_sub_folder_path = :noteSubFolderPath"));
        query.bindValue(QStringLiteral(":noteSubFolderPath"),
                        NoteSubFolder::activeNoteSubFolder().relativePath());
    }

    query.bindValue(QStringLiteral(":id"), tagId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            // always keep in mind that note_file_name is no file name,
            // but the base name (so "my-note", instead of "my-note.md")
            const QString name = query.value(QStringLiteral("note_file_name")).toString();
            const QString noteSubFolderPathData =
                query.value(QStringLiteral("note_sub_folder_path")).toString();
            int noteSubFolderId =
                NoteSubFolder::fetchByPathData(noteSubFolderPathData, QStringLiteral("/")).getId();
            int noteId = Note::fetchNoteIdByName(name, noteSubFolderId);
            noteIdList.append(noteId);
        }
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return noteIdList;
}

/**
 * Fetches all linked note ids for a given subfolder
 */
QVector<int> Tag::fetchAllLinkedNoteIdsForFolder(int tagId, const NoteSubFolder &noteSubFolder,
                                                 bool fromAllSubfolders, const bool recursive) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    QVector<int> noteIdList;

    if (fromAllSubfolders) {
        // 'All notes' selected in note subfolder panel
        query.prepare(
            QStringLiteral("SELECT note_file_name, note_sub_folder_path "
                           "FROM noteTagLink WHERE tag_id = :id"));
    } else if (recursive) {
        query.prepare(
            QStringLiteral("SELECT note_file_name, note_sub_folder_path "
                           "FROM noteTagLink WHERE tag_id = :id "
                           "AND note_sub_folder_path LIKE :noteSubFolderPath"));
        query.bindValue(QStringLiteral(":noteSubFolderPath"), noteSubFolder.relativePath() + "%");
    } else {
        query.prepare(
            QStringLiteral("SELECT note_file_name, note_sub_folder_path "
                           "FROM noteTagLink WHERE tag_id = :id "
                           "AND note_sub_folder_path = :noteSubFolderPath"));
        query.bindValue(QStringLiteral(":noteSubFolderPath"), noteSubFolder.relativePath());
    }

    query.bindValue(QStringLiteral(":id"), tagId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            // always keep in mind that note_file_name is no file name,
            // but the base name (so "my-note", instead of "my-note.md")
            const QString name = query.value(QStringLiteral("note_file_name")).toString();
            const QString noteSubFolderPathData =
                query.value(QStringLiteral("note_sub_folder_path")).toString();
            int noteSubFolderId =
                NoteSubFolder::fetchByPathData(noteSubFolderPathData, QStringLiteral("/")).getId();
            int noteId = Note::fetchNoteIdByName(name, noteSubFolderId);
            noteIdList.append(noteId);
        }
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return noteIdList;
}

/**
 * Fetches all linked notes
 */
QVector<Note> Tag::fetchAllLinkedNotes() const {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    QVector<Note> noteList;

    query.prepare(
        QStringLiteral("SELECT note_file_name, note_sub_folder_path FROM "
                       "noteTagLink WHERE tag_id = :id"));
    query.bindValue(QStringLiteral(":id"), _id);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            const QString fileName = query.value(QStringLiteral("note_file_name")).toString();
            const QString noteSubFolderPath =
                query.value(QStringLiteral("note_sub_folder_path")).toString();
            const auto noteSubFolder =
                NoteSubFolder::fetchByPathData(std::move(noteSubFolderPath), QStringLiteral("/"));
            const auto note = Note::fetchByName(fileName, noteSubFolder.getId());

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

    query.prepare(
        QStringLiteral("UPDATE noteTagLink SET note_sub_folder_path = replace("
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
 * Count the linked note file names for a note sub folder
 */
int Tag::countLinkedNoteFileNamesForNoteSubFolder(int tagId, const NoteSubFolder &noteSubFolder,
                                                  bool fromAllSubfolders, const bool recursive) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);

    if (fromAllSubfolders) {
        query.prepare(
            QStringLiteral("SELECT COUNT(note_file_name) AS cnt FROM noteTagLink "
                           "WHERE tag_id = :id"));
    } else if (recursive) {
        query.prepare(
            QStringLiteral("SELECT COUNT(note_file_name) AS cnt FROM noteTagLink "
                           "WHERE tag_id = :id AND "
                           "note_sub_folder_path LIKE :noteSubFolderPath"));
        query.bindValue(QStringLiteral(":noteSubFolderPath"),
                        noteSubFolder.relativePath() + QLatin1Char('%'));
    } else {
        query.prepare(
            QStringLiteral("SELECT COUNT(note_file_name) AS cnt FROM noteTagLink "
                           "WHERE tag_id = :id AND "
                           "note_sub_folder_path = :noteSubFolderPath"));
        query.bindValue(QStringLiteral(":noteSubFolderPath"),
                        noteSubFolder.relativePath() + QLatin1Char('%'));
    }
    query.bindValue(QStringLiteral(":id"), tagId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        const int result = query.value(QStringLiteral("cnt")).toInt();
        DatabaseService::closeDatabaseConnection(db, query);

        return result;
    }

    DatabaseService::closeDatabaseConnection(db, query);

    return 0;
}

/**
 * Count the linked note file names
 */
int Tag::countLinkedNoteFileNames(int tagId, bool fromAllSubfolders, bool recursive) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);

    if (fromAllSubfolders) {
        query.prepare(
            QStringLiteral("SELECT COUNT(note_file_name) AS cnt FROM noteTagLink "
                           "WHERE tag_id = :id"));
    } else if (recursive) {
        query.prepare(
            QStringLiteral("SELECT COUNT(note_file_name) AS cnt FROM noteTagLink "
                           "WHERE tag_id = :id AND "
                           "note_sub_folder_path LIKE :noteSubFolderPath"));
        query.bindValue(QStringLiteral(":noteSubFolderPath"),
                        NoteSubFolder::activeNoteSubFolder().relativePath() + "%");
    } else {
        query.prepare(
            QStringLiteral("SELECT COUNT(note_file_name) AS cnt FROM noteTagLink "
                           "WHERE tag_id = :id AND "
                           "note_sub_folder_path = :noteSubFolderPath"));
        query.bindValue(QStringLiteral(":noteSubFolderPath"),
                        NoteSubFolder::activeNoteSubFolder().relativePath());
    }
    query.bindValue(QStringLiteral(":id"), tagId);

    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else if (query.first()) {
        const int result = query.value(QStringLiteral("cnt")).toInt();
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
    const QString colorField = colorFieldName();

    const int id = _id;
    if (id > 0) {
        query.prepare(QStringLiteral("UPDATE tag SET name = :name, priority = :priority, "
                                     "parent_id = :parentId, ") %
                      colorField %
                      QStringLiteral(" = :color, "
                                     "updated = datetime('now') "
                                     "WHERE id = :id"));
        query.bindValue(QStringLiteral(":id"), id);
    } else {
        query.prepare(QStringLiteral("INSERT INTO tag (name, priority, parent_id, ") % colorField %
                      QStringLiteral(") VALUES (:name, :priority, :parentId, :color)"));
    }

    const int parentId = _parentId;

    query.bindValue(QStringLiteral(":name"), _name);
    query.bindValue(QStringLiteral(":priority"), _priority);
    query.bindValue(QStringLiteral(":parentId"), parentId);
    query.bindValue(QStringLiteral(":color"), _color.isValid() ? _color.name() : QLatin1String(""));

    if (!query.exec()) {
        // on error
        qWarning() << __func__ << ": " << query.lastError();

        DatabaseService::closeDatabaseConnection(db, query);
        return false;
    } else if (id == 0) {
        // on insert
        _id = query.lastInsertId().toInt();
    }

    // update the parent tag for correct sorting by last use
    if (parentId > 0) {
        QSqlQuery parentQuery(db);
        parentQuery.prepare(QStringLiteral("SELECT * FROM tag WHERE id = :parentId"));
        parentQuery.bindValue(QStringLiteral(":parentId"), parentId);

        if (!parentQuery.exec()) {
            qWarning() << __func__ << ": " << query.lastError();
        } else {
            if (parentQuery.next()) {
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
QString Tag::colorFieldName() const {
    return QSettings().value(QStringLiteral("darkMode")).toBool() ? QStringLiteral("dark_color")
                                                                  : QStringLiteral("color");
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
    query.prepare(
        QStringLiteral("INSERT INTO noteTagLink (tag_id, note_file_name, "
                       "note_sub_folder_path) "
                       "VALUES (:tagId, :noteFileName, "
                       ":noteSubFolderPath)"));

    query.bindValue(QStringLiteral(":tagId"), _id);
    query.bindValue(QStringLiteral(":noteFileName"), note.getName());
    query.bindValue(QStringLiteral(":noteSubFolderPath"), note.getNoteSubFolder().relativePath());

    if (!query.exec()) {
        // we should not show this warning, because we don't check if a
        // link to a note already exists before we try to create another link
        //        qWarning() << __func__ << ": " << query.lastError();

        DatabaseService::closeDatabaseConnection(db, query);

        return false;
    }

    // update the parent tag for correct sorting by last use
    if (_parentId > 0) {
        QSqlQuery parentQuery(db);
        parentQuery.prepare(QStringLiteral("SELECT * FROM tag WHERE id = :parentId"));
        parentQuery.bindValue(QStringLiteral(":parentId"), _parentId);

        if (!parentQuery.exec()) {
            qWarning() << __func__ << ": " << query.lastError();
        } else {
            if (parentQuery.next()) {
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
    query.prepare(
        QStringLiteral("DELETE FROM noteTagLink WHERE tag_id = :tagId AND "
                       "note_file_name = :noteFileName AND "
                       "note_sub_folder_path = :noteSubFolderPath"));

    query.bindValue(QStringLiteral(":tagId"), _id);
    query.bindValue(QStringLiteral(":noteFileName"), note.getName());
    query.bindValue(QStringLiteral(":noteSubFolderPath"), note.getNoteSubFolder().relativePath());

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
    query.prepare(
        QStringLiteral("DELETE FROM noteTagLink WHERE "
                       "note_file_name = :noteFileName AND "
                       "note_sub_folder_path = :noteSubFolderPath"));

    query.bindValue(QStringLiteral(":noteFileName"), note.getName());
    query.bindValue(QStringLiteral(":noteSubFolderPath"), note.getNoteSubFolder().relativePath());

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

    // Check all note tag links if note is missing to set the link stale
    query.prepare(QStringLiteral("SELECT * FROM notetaglink"));
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            Note note = getNoteFromNoteTagLinkQuery(query);

            // set note tag link stale if note doesn't exist
            if (!note.isFetched()) {
                const int id = query.value(QStringLiteral("id")).toInt();
                setNoteLinkByIdStale(id);
            }
        }
    }

    // Check all stale note tag links if note is still missing to remove the
    // stale date if the note came back
    query.prepare(QStringLiteral("SELECT * FROM notetaglink WHERE stale_DATE IS NOT NULL"));
    if (!query.exec()) {
        qWarning() << __func__ << ": " << query.lastError();
    } else {
        for (int r = 0; query.next(); r++) {
            Note note = getNoteFromNoteTagLinkQuery(query);

            // set note tag link to not stale if note exists again
            if (note.isFetched()) {
                const int id = query.value(QStringLiteral("id")).toInt();
                setNoteLinkByIdNotStale(id);
            }
        }
    }

    // Remove all note links that were stale for too long
    removeExpiredStaleNoteLinkBy();

    DatabaseService::closeDatabaseConnection(db, query);
}

Note Tag::getNoteFromNoteTagLinkQuery(const QSqlQuery &query) {
    const QString noteFileName = query.value(QStringLiteral("note_file_name")).toString();
    const QString noteSubFolderPath =
        query.value(QStringLiteral("note_sub_folder_path")).toString();

    const NoteSubFolder noteSubFolder =
        NoteSubFolder::fetchByPathData(noteSubFolderPath, QStringLiteral("/"));

    return Note::fetchByName(noteFileName, noteSubFolder.getId());
}

/**
 * Removes a note tag link by its id
 *
 * @param id
 * @return
 */
bool Tag::removeNoteLinkById(int id) {
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
 * Sets a note tag link stale by its id (if it wasn't already stale)
 *
 * @param id
 * @return
 */
bool Tag::setNoteLinkByIdStale(int id) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "UPDATE noteTagLink SET stale_date = :date WHERE id = :id AND stale_date IS NULL"));
    query.bindValue(QStringLiteral(":id"), id);
    query.bindValue(QStringLiteral(":date"), QDateTime::currentDateTime());

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
 * Sets a note tag link to not stale by its id
 *
 * @param id
 * @return
 */
bool Tag::setNoteLinkByIdNotStale(int id) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    query.prepare(QStringLiteral("UPDATE noteTagLink SET stale_date = NULL WHERE id = :id"));
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
 * Remove all note links that were stale for too long (10 days)
 *
 * @return
 */
bool Tag::removeExpiredStaleNoteLinkBy() {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    query.prepare(QStringLiteral("DELETE FROM noteTagLink WHERE stale_date < :date"));
    query.bindValue(QStringLiteral(":date"), QDateTime::currentDateTime().addDays(-10));

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
bool Tag::renameNoteFileNamesOfLinks(const QString &oldFileName, const QString &newFileName,
                                     const NoteSubFolder &noteSubFolder) {
    QSqlDatabase db = DatabaseService::getNoteFolderDatabase();
    QSqlQuery query(db);
    query.prepare(
        QStringLiteral("UPDATE noteTagLink SET note_file_name = :newFileName WHERE "
                       "note_file_name = :oldFileName AND "
                       "note_sub_folder_path = :noteSubFolderPath"));

    query.bindValue(QStringLiteral(":oldFileName"), oldFileName);
    query.bindValue(QStringLiteral(":newFileName"), newFileName);
    query.bindValue(QStringLiteral(":noteSubFolderPath"), noteSubFolder.relativePath());

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
    query.prepare(
        QStringLiteral("UPDATE noteTagLink SET note_sub_folder_path = "
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
bool Tag::exists() const { return Tag::fetch(_id)._id > 0; }

bool Tag::isFetched() const { return (_id > 0); }

void Tag::setAsActive() { Tag::setAsActive(_id); }

void Tag::setAsActive(const int tagId) {
    NoteFolder noteFolder = NoteFolder::currentNoteFolder();
    noteFolder.setActiveTagId(tagId);
    noteFolder.store();
}

/**
 * Checks if this note folder is the active one
 */
bool Tag::isActive() const { return activeTagId() == _id; }

/**
 * Returns the id of the active note folder in the settings
 */
int Tag::activeTagId() {
    const NoteFolder noteFolder = NoteFolder::currentNoteFolder();
    return noteFolder.getActiveTagId();
}

/**
 * Returns the active note folder
 */
Tag Tag::activeTag() { return Tag::fetch(activeTagId()); }

/**
 * Sets the non-darkMode colors as darkMode colors for all tags
 */
void Tag::migrateDarkColors() {
    QSettings settings;
    const bool darkMode = settings.value(QStringLiteral("darkMode")).toBool();

    // disable dark mode to get the light color
    settings.setValue(QStringLiteral("darkMode"), false);

    // fetch all tags with non-dark mode colors
    const QVector<Tag> tags = fetchAll();

    // enable dark mode to later set the dark color
    settings.setValue(QStringLiteral("darkMode"), true);

    for (Tag tag : tags) {
        // get the non-dark mode color (because the fetch was made while
        // "darkMode" was off)
        const QColor color = tag.getColor();

        // set the non-dark mode color as dark mode color (because now
        // "darkMode" is enabled)
        tag.setColor(color);
        tag.store();
    }

    // set the dark mode to the old value
    settings.setValue(QStringLiteral("darkMode"), darkMode);
}

bool Tag::mergeFromDatabase(QSqlDatabase &db) {
    QSqlDatabase noteFolderDB = DatabaseService::getNoteFolderDatabase();

    const bool isSameTagTable =
        DatabaseService::generateDatabaseTableSha1Signature(db, "tag") ==
        DatabaseService::generateDatabaseTableSha1Signature(noteFolderDB, "tag");

    const bool isSameNoteTagLinkTable =
        DatabaseService::generateDatabaseTableSha1Signature(db, "noteTagLink") ==
        DatabaseService::generateDatabaseTableSha1Signature(noteFolderDB, "noteTagLink");

    // if those tables are the same everything is ok
    if (isSameTagTable && isSameNoteTagLinkTable) {
        qDebug() << "Tag and tagLinkTable were the same in conflicting note "
                    "folder database";

        return true;
    }

    // TODO: try to merge

    return false;
}

/**
 * Fetches a tag by its "breadcrumb list" of tag names.
 * Element nameList[0] would be highest in the tree (with parentId: 0)
 *
 * @param nameList
 * @param createMissing {bool} if true (default) all missing tags will be
 * created
 * @return Tag object of deepest tag of the name breadcrumb list
 */
Tag Tag::getTagByNameBreadcrumbList(const QStringList &nameList, bool createMissing) {
    int parentId = 0;
    Tag tag;

    for (const QString &tagName : nameList) {
        tag = Tag::fetchByName(tagName, parentId);

        if (!tag.isFetched()) {
            if (!createMissing) {
                return Tag();
            }

            tag.setName(tagName);
            tag.setParentId(parentId);
            tag.store();

            if (!tag.isFetched()) {
                return Tag();
            }
        }
        parentId = tag.getId();
    }
    return tag;
}

bool Tag::operator==(const Tag &tag) const { return _id == tag._id; }

bool Tag::operator<(const Tag &tag) const { return _name < tag._name; }

QDebug operator<<(QDebug dbg, const Tag &tag) {
    dbg.nospace() << "Tag: <id>" << tag._id << " <name>" << tag._name << " <parentId>"
                  << tag._parentId;
    return dbg.space();
}
