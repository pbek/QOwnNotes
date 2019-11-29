#pragma once

#include <QSqlQuery>
#include <QList>
#include <QColor>
#include "note.h"

class Tag {
public:
    enum SpecialTag {
        AllNotesId = -1,
        AllUntaggedNotesId = -2,
    };

    explicit Tag();

    bool operator ==(const Tag &tag) const;

    bool operator <(const Tag &tag) const;

    friend QDebug operator<<(QDebug dbg, const Tag &tag);

    int getId() const;

    bool store();

    bool exists() const;

    bool fillFromQuery(const QSqlQuery& query);

    bool remove() const;

    bool isFetched() const;

    QString getName() const;

    void setName(const QString &text);

    int getPriority() const;

    void setPriority(int value);

    void setAsActive();

    bool isActive() const;

    bool linkToNote(const Note &note) const;

    bool removeLinkToNote(const Note &note) const;

    QStringList fetchAllLinkedNoteFileNames(bool fromAllSubfolders) const;

    QList<Note> fetchAllLinkedNotes();

    bool isLinkedToNote(const Note &note) const;

    int countLinkedNoteFileNames(bool fromAllSubfolder, bool recursive) const;

    int countLinkedNoteFileNamesForNoteFolder(NoteSubFolder noteSubFolder, bool recursive) const;

    int getParentId() const;

    void setParentId(int id);

    bool hasChild(int tagId) const;

    QColor getColor() const;

    void setColor(const QColor &color);

/*
 * Tag
 * Public static functions
*/

    static QList<Tag> fetchAll();

    static int countAll();

    static int activeTagId();

    static Tag activeTag();

    static Tag fetchByName(QString name, bool startsWith = false);

    static Tag fetchByName(const QString &name, int parentId);

    static QList<Tag> fetchAllOfNote(const Note &note);

    static QStringList fetchAllNames();

    static bool removeAllLinksToNote(const Note &note);

    static bool renameNoteFileNamesOfLinks(const QString& oldFileName,
                                           const QString& newFileName);

    static bool renameNoteSubFolderPathsOfLinks(QString &oldPath,
                                                QString &newPath);

    static Tag fetch(int id);

    static Tag tagFromQuery(const QSqlQuery& query);

    static QList<Tag> fetchAllWithLinkToNoteNames(const QStringList& noteNameList);

    static QList<Tag> fetchAllByParentId(int parentId, const QString& sortBy = "created DESC");

    static int countAllParentId(int parentId);

    static int countAllOfNote(const Note &note);

    static void setAsActive(int tagId);

    static void convertDirSeparator();

    static Tag fetchOneOfNoteWithColor(const Note& note);

    static void migrateDarkColors();

    static void removeBrokenLinks();

    static QStringList fetchAllNamesOfNote(const Note &note);

    static QStringList searchAllNamesByName(const QString& name);

    static QList<Tag> fetchRecursivelyByParentId(int parentId);

    static bool isTaggingShowNotesRecursively();

    static QList<Tag> fetchAllOfNotes(const QList<Note>& notes);

protected:
    int id;
    int priority;
    int parentId;
    QColor _color;
    QString name;

    QString colorFieldName();

    static bool removeNoteLinkById(int id);
};
