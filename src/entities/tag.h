#pragma once

#include <QColor>
#include <QVector>
#include <QSqlQuery>

class Note;
class NoteSubFolder;

class Tag {
   public:
    enum SpecialTag {
        AllNotesId = -1,
        AllUntaggedNotesId = -2,
    };

    Tag() noexcept;

    bool operator==(const Tag &tag) const;

    bool operator<(const Tag &tag) const;

    friend QDebug operator<<(QDebug dbg, const Tag &tag);

    int getId() const;

    bool store();

    bool exists() const;

    Tag fillFromQuery(const QSqlQuery &query);

    bool remove() const;

    bool isFetched() const;

    QString getName() const;

    void setName(const QString &text);

    int getPriority() const;

    void setPriority(const int value);

    void setAsActive();

    bool isActive() const;

    bool linkToNote(const Note &note) const;

    bool removeLinkToNote(const Note &note) const;

    QVector<int> fetchAllLinkedNoteIds(const bool fromAllSubfolders,
                                       const bool recursive = true) const;

    QVector<int> fetchAllLinkedNoteIdsForFolder(
        const NoteSubFolder &noteSubFolder, bool fromAllSubfolders,
        const bool recursive = true) const;

    QVector<Note> fetchAllLinkedNotes() const;

    bool isLinkedToNote(const Note &note) const;

    int countLinkedNoteFileNames(const bool fromAllSubfolder,
                                 const bool recursive) const;

    int countLinkedNoteFileNamesForNoteSubFolder(
        const NoteSubFolder &noteSubFolder, const bool recursive) const;

    int getParentId() const;

    void setParentId(const int id);

    bool hasChild(const int tagId) const;

    QColor getColor() const;

    void setColor(const QColor &color);

    /*
     * Tag
     * Public static functions
     */

    static QVector<Tag> fetchAll();

    static int countAll();

    static int activeTagId();

    static Tag activeTag();

    static Tag fetchByName(QString name, const bool startsWith = false);

    static Tag fetchByName(const QString &name, const int parentId);

    static QVector<Tag> fetchAllOfNote(const Note &note);

    static QStringList fetchAllNames();

    static bool removeAllLinksToNote(const Note &note);

    static bool renameNoteFileNamesOfLinks(const QString &oldFileName,
                                           const QString &newFileName,
                                           const NoteSubFolder &noteSubFolder);

    static bool renameNoteSubFolderPathsOfLinks(const QString &oldPath,
                                                const QString &newPath);

    static Tag fetch(const int id);

    static Tag tagFromQuery(const QSqlQuery &query);

    static QVector<Tag> fetchAllWithLinkToNoteNames(
        const QStringList &noteNameList);

    static QVector<Tag> fetchAllByParentId(
        const int parentId,
        const QString &sortBy = QStringLiteral("created DESC"));

    static int countAllParentId(const int parentId);

    static int countAllOfNote(const Note &note);

    static void setAsActive(const int tagId);

    static void convertDirSeparator();

    static Tag fetchOneOfNoteWithColor(const Note &note);

    static void migrateDarkColors();

    static void removeBrokenLinks();

    static QStringList fetchAllNamesOfNote(const Note &note);

    static QSet<int> fetchAllIdsByNote(const Note &note);

    static QStringList searchAllNamesByName(const QString &name);

    static QVector<Tag> fetchRecursivelyByParentId(const int parentId);

    QStringList getParentTagNames();

    static bool isTaggingShowNotesRecursively();

    static QVector<Tag> fetchAllOfNotes(const QVector<Note> &notes);

    static bool mergeFromDatabase(QSqlDatabase &db);

    static Tag getTagByNameBreadcrumbList(const QStringList &nameList,
                                          bool createMissing);

   protected:
    int _id;
    int _parentId;
    int _priority;
    QColor _color;
    QString _name;

    QString colorFieldName() const;

    static bool removeNoteLinkById(const int id);
};

Q_DECLARE_TYPEINFO(Tag, Q_MOVABLE_TYPE);
