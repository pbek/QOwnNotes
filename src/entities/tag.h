#pragma once

#include <QSqlQuery>
#include <QList>
#include "note.h"

class Tag {
public:
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

    static Tag fetchByName(QString name);

    bool linkToNote(Note note);

    static QList<Tag> fetchAllOfNote(Note note);

    bool removeLinkToNote(Note note);

    QStringList fetchAllLinkedNoteFileNames();

    bool isLinkedToNote(Note note);

private:
    int id;
    QString name;
    int priority;
};
