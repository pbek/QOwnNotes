#pragma once

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QSqlQuery>

// note subfolders that should be ignored by default
// regular expression, separated by ";"
#define IGNORED_NOTE_SUBFOLDERS_DEFAULT "^\\."

class NoteSubFolder {
   public:
    NoteSubFolder();

    int getId() const;

    QString getName() const;

    void setName(QString text);

    static NoteSubFolder fetch(int id, const QString& connectionName = QStringLiteral("memory"));

    static QVector<NoteSubFolder> fetchAll(int limit = -1);

    static NoteSubFolder noteSubFolderFromQuery(const QSqlQuery& query);

    bool store();

    friend QDebug operator<<(QDebug dbg, const NoteSubFolder& note);

    static bool deleteAll();

    bool exists() const;

    NoteSubFolder fillFromQuery(const QSqlQuery& query);

    bool remove();

    bool isFetched() const;

    QDateTime getFileLastModified() const;

    QDateTime getModified() const;

    static int countAll();

    int getParentId() const;

    void setParentId(int parentId);

    QString relativePath(char separator = '/',
                         const QString& connectionName = QStringLiteral("memory")) const;

    QString fullPath() const;

    NoteSubFolder getParent(const QString& connectionName = QStringLiteral("memory")) const;

    static QVector<NoteSubFolder> fetchAllByParentId(
        int parentId, const QString& sortBy = QStringLiteral("file_last_modified DESC"));

    static QVector<int> fetchIdsRecursivelyByParentId(int parentId);

    bool isActive() const;

    static int activeNoteSubFolderId();

    static NoteSubFolder activeNoteSubFolder();

    static bool setAsActive(int noteSubFolderId);

    void setAsActive();

    QString pathData() const;

    static NoteSubFolder fetchByPathData(QString pathData,
                                         const QString& separator = QStringLiteral("\n"));

    static NoteSubFolder fetchByNameAndParentId(const QString& name, int parentId);

    void saveTreeWidgetExpandState(bool expanded) const;

    bool treeWidgetExpandState() const;

    static QString treeWidgetExpandStateSettingsKey(int noteFolderId = 0);

    bool removeFromFileSystem() const;

    QDir dir() const;

    bool rename(const QString& newName);

    static int countAllParentId(int parentId);

    static QVector<int> fetchAllIds();

    static bool isNoteSubfoldersPanelShowNotesRecursively();

    int depth() const;

    static bool willFolderBeIgnored(const QString& folderName, bool showWarning = false);

   protected:
    int _id;
    int _parentId;
    QString _name;
    QDateTime _fileLastModified;
    QDateTime _created;
    QDateTime _modified;
};

Q_DECLARE_TYPEINFO(NoteSubFolder, Q_MOVABLE_TYPE);
