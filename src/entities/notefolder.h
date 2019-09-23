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
                       int cloudConnectionId, QString remotePath);
    static NoteFolder fetch(int id);
    static NoteFolder noteFolderFromQuery(const QSqlQuery& query);
    bool store();
    friend QDebug operator<<(QDebug dbg, const NoteFolder &noteFolder);
    bool exists();
    bool fillFromQuery(const QSqlQuery& query);
    bool remove();
    bool isFetched();
    static QList<NoteFolder> fetchAll();
    QString getName();
    int getCloudConnectionId();
    QString getLocalPath();
    QString getRemotePath();
    int getPriority();
    void setName(QString text);
    void setLocalPath(QString text);
    void setPriority(int value);
    void setCloudConnectionId(int id);
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
    static QString currentRemotePath(bool addTrailingSlash = true);
    static QString currentLocalPath();
    static QString currentRootFolderName(bool fullPath = false);
    void setActiveTagId(int value);
    int getActiveTagId();
    bool isShowSubfolders();
    void setShowSubfolders(bool value);
    static bool isCurrentShowSubfolders();
    void setActiveNoteSubFolder(NoteSubFolder noteSubFolder);
    NoteSubFolder getActiveNoteSubFolder();
    void resetActiveNoteSubFolder();
    static QString currentTrashPath();
    static QString currentMediaPath();
    static QString currentAttachmentsPath();
    bool isUseGit();
    void setUseGit(bool value);
    QJsonObject jsonObject();
    static QString noteFoldersWebServiceJsonText();
    static bool isPathNoteFolder(const QString &path);

private:
    int id;
    QString name;
    QString localPath;
    int cloudConnectionId;
    QString remotePath;
    int priority;
    int activeTagId;
    QString activeNoteSubFolderData;
    bool showSubfolders;
    bool useGit;
};
