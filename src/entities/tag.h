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

    int getId();

    bool store();

    bool exists();

    bool fillFromQuery(const QSqlQuery& query);

    bool remove();

    bool isFetched();

    QString getName();

    int getPriority();

    void setName(QString text);

    void setPriority(int value);

    void setAsActive();

    bool isActive();

    bool linkToNote(Note note);

    bool removeLinkToNote(Note note);

    QStringList fetchAllLinkedNoteFileNames(bool fromAllSubfolders);

    QList<Note> fetchAllLinkedNotes();

    bool isLinkedToNote(Note note);

    int countLinkedNoteFileNames(bool fromAllSubfolder, bool recursive);

    int getParentId();

    void setParentId(int id);

    bool hasChild(int tagId);

    QColor getColor();

    void setColor(QColor color);

/*
 * Tag
 * Public static functions
*/

    static QList<Tag> fetchAll();

    static int countAll();

    static int activeTagId();

    static Tag activeTag();

    static Tag fetchByName(QString name, bool startsWith = false);

    static Tag fetchByName(QString name, int parentId);

    static QList<Tag> fetchAllOfNote(Note note);

    static QStringList fetchAllNames();

    static bool removeAllLinksToNote(Note note);

    static bool renameNoteFileNamesOfLinks(const QString& oldFileName,
                                           const QString& newFileName);

    static bool renameNoteSubFolderPathsOfLinks(QString &oldPath,
                                                QString &newPath);

    static Tag fetch(int id);

    static Tag tagFromQuery(const QSqlQuery& query);

    static QList<Tag> fetchAllWithLinkToNoteNames(const QStringList& noteNameList);

    static QList<Tag> fetchAllByParentId(int parentId, const QString& sortBy = "created DESC");

    static int countAllParentId(int parentId);

    static int countAllOfNote(Note note);

    static void setAsActive(int tagId);

    static void convertDirSeparator();

    static Tag fetchOneOfNoteWithColor(const Note& note);

    static void migrateDarkColors();

    static void removeBrokenLinks();

    static QStringList fetchAllNamesOfNote(Note note);

    static QStringList searchAllNamesByName(const QString& name);

    static QList<Tag> fetchRecursivelyByParentId(int parentId);

    static bool isTaggingShowNotesRecursively();

    static QList<Tag> fetchAllOfNotes(QList<Note> notes);

protected:
    int id;
    int priority;
    int parentId;
    QColor _color;
    QString name;

    QString colorFieldName();

    static bool removeNoteLinkById(int id);
};
