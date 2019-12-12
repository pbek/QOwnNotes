#pragma once

#include <QObject>
#include <QStringList>
#include <QQmlListProperty>
#include "entities/note.h"
#include "tagapi.h"

class NoteApi : public QObject, public Note {
    Q_OBJECT

    Q_PROPERTY(int id READ getId() CONSTANT)
    Q_PROPERTY(QString name READ getName() WRITE setName)
    Q_PROPERTY(QString fileName READ getFileName() CONSTANT)
    Q_PROPERTY(QString fullNoteFilePath READ fullNoteFilePath() CONSTANT)
    Q_PROPERTY(QString fullNoteFileDirPath READ fullNoteFileDirPath() CONSTANT)
    Q_PROPERTY(int noteSubFolderId READ getNoteSubFolderId() CONSTANT)
    Q_PROPERTY(QString noteText READ getNoteText() WRITE setNoteText())
    Q_PROPERTY(QString decryptedNoteText READ getDecryptedNoteText()
                       WRITE setDecryptedNoteText())
    Q_PROPERTY(QDateTime fileCreated READ getFileCreated() CONSTANT)
    Q_PROPERTY(QDateTime fileLastModified READ getFileLastModified() CONSTANT)
    Q_PROPERTY(bool hasDirtyData READ getHasDirtyData() CONSTANT)
    Q_PROPERTY(QQmlListProperty<TagApi> tags READ tags CONSTANT)

private:
    QList<TagApi *> _tags;
    Note _note;

public:
    NoteApi *fetch(int id);
    static NoteApi *fromNote(const Note &note);
//    static NoteApi fromNote(Note note);

    Q_INVOKABLE QQmlListProperty<TagApi> tags();
    Q_INVOKABLE QStringList tagNames() const;
    Q_INVOKABLE bool addTag(const QString& tagName);
    Q_INVOKABLE bool removeTag(QString tagName);
    Q_INVOKABLE QQmlListProperty<NoteApi> fetchAll(int limit = -1,
                                                   int offset = -1);
    Q_INVOKABLE QString toMarkdownHtml(bool forExport = true);
    Q_INVOKABLE QString getFileURLFromFileName(const QString& localFileName);
};
