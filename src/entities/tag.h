#pragma once

#include <QColor>
#include <QDateTime>
#include <QSqlQuery>
#include <QVector>

class Note;
class NoteSubFolder;

struct TagHeader {
    TagHeader() = default;
    TagHeader(int id, QString name) : _id{id}, _name{std::move(name)} {}
    int _id = 0;
    QString _name{QLatin1String("")};

    bool operator==(const TagHeader &tag) const { return _id == tag._id; }
    bool operator<(const TagHeader &tag) const { return _name < tag._name; }
};

class Tag : protected TagHeader {
   public:
    enum SpecialTag {
        AllNotesId = -1,
        AllUntaggedNotesId = -2,
    };

    Tag() noexcept;
    explicit Tag(int id) noexcept : TagHeader{id, QString()}, _parentId{0} {}

    bool operator==(const Tag &tag) const;

    bool operator<(const Tag &tag) const;

    friend QDebug operator<<(QDebug dbg, const Tag &tag);

    inline int getId() const { return _id; }

    inline const QString &getName() const { return _name; }

    inline void setName(QString text) { _name = text; }

    bool store();

    bool exists() const;

    Tag fillFromQuery(const QSqlQuery &query);

    bool remove() const;

    bool isFetched() const;

    int getPriority() const;

    void setPriority(const int value);

    void setAsActive();

    bool isActive() const;

    bool linkToNote(const Note &note) const;

    bool removeLinkToNote(const Note &note) const;

    static QVector<int> fetchAllLinkedNoteIds(int tagId, const bool fromAllSubfolders,
                                              const bool recursive = true);

    static QVector<int> fetchAllLinkedNoteIdsForFolder(int tagId,
                                                       const NoteSubFolder &noteSubFolder,
                                                       bool fromAllSubfolders,
                                                       const bool recursive = true);

    QVector<Note> fetchAllLinkedNotes() const;

    bool isLinkedToNote(const Note &note) const;

    static int countLinkedNoteFileNames(int tagId, bool fromAllSubfolders, bool recursive);

    static int countLinkedNoteFileNamesForNoteSubFolder(int tagId,
                                                        const NoteSubFolder &noteSubFolder,
                                                        bool fromAllSubfolders, bool recursive);

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

    static bool renameNoteFileNamesOfLinks(const QString &oldFileName, const QString &newFileName,
                                           const NoteSubFolder &noteSubFolder);

    static bool renameNoteSubFolderPathsOfLinks(const QString &oldPath, const QString &newPath);

    static Tag fetch(const int id);

    static Tag tagFromQuery(const QSqlQuery &query);

    static QVector<Tag> fetchAllWithLinkToNoteNames(const QStringList &noteNameList);

    static QVector<TagHeader> fetchAllTagHeadersByParentId(const int parentId);

    static QVector<Tag> fetchAllByParentId(const int parentId,
                                           const QString &sortBy = QStringLiteral("created DESC"));

    static QVector<int> fetchAllIdsByParentId(const int parentId);

    static int countAllParentId(const int parentId);

    static bool noteHasTags(const Note &note, const QString &path);

    static void setAsActive(const int tagId);

    static void convertDirSeparator();

    static Tag fetchOneOfNoteWithColor(const Note &note);

    static void migrateDarkColors();

    static void removeBrokenLinks();

    static QStringList fetchAllNamesOfNote(const Note &note);

    static QSet<int> fetchAllIdsByNote(const Note &note);

    static QStringList searchAllNamesByName(const QString &name);

    static QVector<Tag> fetchRecursivelyByParentId(const int parentId);

    static QVector<int> fetchTagIdsRecursivelyByParentId(const int parentId);

    QStringList getParentTagNames();

    static bool isTaggingShowNotesRecursively();

    static QVector<Tag> fetchAllOfNotes(const QVector<Note> &notes);

    static bool mergeFromDatabase(QSqlDatabase &db);

    static Tag getTagByNameBreadcrumbList(const QStringList &nameList, bool createMissing);

   protected:
    int _parentId;
    int _priority;
    QColor _color;

    QString colorFieldName() const;

    static bool removeNoteLinkById(int id);
    static bool setNoteLinkByIdStale(int id);
    static bool setNoteLinkByIdNotStale(int id);
    static bool removeExpiredStaleNoteLinkBy();
    static Note getNoteFromNoteTagLinkQuery(const QSqlQuery &query);
};

Q_DECLARE_TYPEINFO(Tag, Q_MOVABLE_TYPE);
