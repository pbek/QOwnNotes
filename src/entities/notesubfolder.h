#pragma once

#include <QDateTime>
#include <QSqlQuery>
#include <QDebug>


class NoteSubFolder {
public:
    explicit NoteSubFolder();

    int getId();

    QString getName();

    void setName(QString text);

    static NoteSubFolder fetch(int id);

    static QList<NoteSubFolder> fetchAll(int limit = -1);

    static NoteSubFolder noteSubFolderFromQuery(QSqlQuery query);

    bool store();

    friend QDebug operator<<(QDebug dbg, const NoteSubFolder &note);

    static bool deleteAll();

    bool exists();

    bool fillFromQuery(QSqlQuery query);

    bool remove();

    bool isFetched();

    QDateTime getFileLastModified();

    QDateTime getModified();

    static int countAll();

    int getParentId();

    void setParentId(int parentId);

    QString relativePath();

    NoteSubFolder getParent();

    static QList<NoteSubFolder> fetchAllByParentId(int parentId);

    bool isActive();

    static int activeSubNoteFolderId();

    static NoteSubFolder activeNoteFolder();

    static void setAsActive(int tagId);

    void setAsActive();

protected:
    int id;
    int parentId;
    QString name;
    QDateTime fileLastModified;
    QDateTime created;
    QDateTime modified;

signals:

public slots:
};
