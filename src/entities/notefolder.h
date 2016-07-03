#pragma once

#include <QSqlQuery>
#include <QStringList>
#include "notesubfolder.h"

class NoteFolder
{
public:
    explicit NoteFolder();

    int getId();
    static bool create(QString name, QString localPath,
                       int ownCloudServerId, QString remotePath);
    static NoteFolder fetch(int id);
    static NoteFolder noteFolderFromQuery(QSqlQuery query);
    bool store();
    friend QDebug operator<<(QDebug dbg, const NoteFolder &noteFolder);
    bool exists();
    bool fillFromQuery(QSqlQuery query);
    bool remove();
    bool isFetched();
    static QList<NoteFolder> fetchAll();
    QString getName();
    int getOwnCloudServerId();
    QString getLocalPath();
    QString getRemotePath();
    int getPriority();
    void setName(QString text);
    void setLocalPath(QString text);
    void setPriority(int value);
    void setOwnCloudServerId(int id);
    void setRemotePath(QString text);
    static int countAll();
    static bool migrateToNoteFolders();
    void setAsCurrent();
    bool isCurrent();
    static int currentNoteFolderId();
    static NoteFolder currentNoteFolder();
    bool localPathExists();
    QString suggestRemotePath();
    QString fixRemotePath();
    static QString currentRemotePath();
    static QString currentLocalPath();
    void setActiveTagId(int value);
    int getActiveTagId();
    bool isShowSubfolders();
    void setShowSubfolders(bool value);
    static bool isCurrentShowSubfolders();
    void setActiveNoteSubFolder(NoteSubFolder noteSubFolder);
    NoteSubFolder getActiveNoteSubFolder();

private:
    int id;
    QString name;
    QString localPath;
    int ownCloudServerId;
    QString remotePath;
    int priority;
    int activeTagId;
    QString activeNoteSubFolderData;
    bool showSubfolders;
};
