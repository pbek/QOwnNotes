#pragma once

#include <QSqlQuery>
#include <QStringList>
#include "notesubfolder.h"

class NoteFolder
{
public:
    explicit NoteFolder();

    friend QDebug operator<<(QDebug dbg, const NoteFolder &noteFolder);

    int getId() const;
    bool store();
    bool exists() const;
    bool fillFromQuery(const QSqlQuery& query);
    bool remove();
    bool isFetched() const;
    QString getName() const;
    int getCloudConnectionId() const;
    QString getLocalPath() const;
    QString getRemotePath() const;
    int getPriority() const;
    void setName(const QString &text);
    void setLocalPath(const QString &text);
    void setPriority(int value);
    void setCloudConnectionId(int id);
    void setRemotePath(const QString &text);
    void setAsCurrent();
    bool isCurrent() const;
    bool localPathExists() const;
    QString suggestRemotePath();
    QString fixRemotePath();
    void setActiveTagId(int value);
    int getActiveTagId() const;
    bool isShowSubfolders() const;
    void setShowSubfolders(bool value);
    void setActiveNoteSubFolder(const NoteSubFolder &noteSubFolder);
    NoteSubFolder getActiveNoteSubFolder() const;
    void resetActiveNoteSubFolder();
    bool isUseGit() const;
    void setUseGit(bool value);
    QJsonObject jsonObject() const;

    static bool create(const QString& name, const QString& localPath,
                       int cloudConnectionId, const QString& remotePath);
    static NoteFolder fetch(int id);
    static NoteFolder noteFolderFromQuery(const QSqlQuery& query);
    static QList<NoteFolder> fetchAll();
    static int countAll();
    static bool migrateToNoteFolders();
    static int currentNoteFolderId();
    static NoteFolder currentNoteFolder();
    static QString currentRemotePath(bool addTrailingSlash = true);
    static QString currentLocalPath();
    static QString currentRootFolderName(bool fullPath = false);
    static bool isCurrentHasSubfolders();
    static bool isCurrentShowSubfolders();
    static QString currentTrashPath();
    static QString currentMediaPath();
    static QString currentAttachmentsPath();
    static QString noteFoldersWebServiceJsonText();
    static bool isPathNoteFolder(const QString &path);
    static bool isCurrentNoteTreeEnabled();

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
