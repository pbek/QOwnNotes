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

    int getId();

    static Tag fetch(int id);

    static Tag tagFromQuery(QSqlQuery query);

    bool store();

    friend QDebug operator<<(QDebug dbg, const Tag &tag);

    bool exists();

    bool fillFromQuery(QSqlQuery query);

    bool remove();

    bool isFetched();

    static QList<Tag> fetchAll();

    QString getName();

    int getPriority();

    void setName(QString text);

    void setPriority(int value);

    static int countAll();

    void setAsActive();

    bool isActive();

    static int activeTagId();

    static Tag activeTag();

    static Tag fetchByName(QString name, bool startsWith = false);

    static Tag fetchByName(QString name, int parentId);

    bool linkToNote(Note note);

    static QList<Tag> fetchAllOfNote(Note note);

    bool removeLinkToNote(Note note);

    QStringList fetchAllLinkedNoteFileNames(bool fromAllSubfolders);

    static QStringList fetchAllNames();

    bool isLinkedToNote(Note note);

    static bool removeAllLinksToNote(Note note);

    static bool renameNoteFileNamesOfLinks(
            QString oldFileName, QString newFileName);

    int countLinkedNoteFileNames(bool fromAllSubfolder);

    static QList<Tag> fetchAllWithLinkToNoteNames(QStringList noteNameList);

    int getParentId();

    void setParentId(int id);

    static QList<Tag> fetchAllByParentId(int parentId);

    static int countAllParentId(int parentId);

    bool hasChild(int tagId);

    static int countAllOfNote(Note note);

    static void setAsActive(int tagId);

    static void convertDirSeparator();

    QColor getColor();

    void setColor(QColor color);

    static Tag fetchOneOfNoteWithColor(Note note);

    static void migrateDarkColors();

    static void removeBrokenLinks();

    static QStringList fetchAllNamesOfNote(Note note);

protected:
    int id;
    QString name;
    int priority;
    int parentId;
    QColor _color;

    QString colorFieldName();

    static bool removeNoteLinkById(int id);
};
