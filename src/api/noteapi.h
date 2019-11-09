#pragma once

#include <QObject>
#include <QStringList>
#include <QQmlListProperty>
#include "entities/note.h"
#include "tagapi.h"

class NoteApi : public QObject, public Note {
    Q_OBJECT

    Q_PROPERTY(int id READ getId())
    Q_PROPERTY(QString name READ getName() WRITE setName)
    Q_PROPERTY(QString fileName READ getFileName())
    Q_PROPERTY(QString fullNoteFilePath READ fullNoteFilePath())
    Q_PROPERTY(QString fullNoteFileDirPath READ fullNoteFileDirPath())
    Q_PROPERTY(int noteSubFolderId READ getNoteSubFolderId())
    Q_PROPERTY(QString noteText READ getNoteText() WRITE setNoteText())
    Q_PROPERTY(QString decryptedNoteText READ getDecryptedNoteText()
                       WRITE setDecryptedNoteText())
    Q_PROPERTY(QDateTime fileCreated READ getFileCreated())
    Q_PROPERTY(QDateTime fileLastModified READ getFileLastModified())
    Q_PROPERTY(bool hasDirtyData READ getHasDirtyData())
    Q_PROPERTY(QQmlListProperty<TagApi> tags READ tags)

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
