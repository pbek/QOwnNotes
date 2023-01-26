#pragma once

#include <QObject>
#include <QQmlListProperty>

#include "entities/notesubfolder.h"

class NoteApi;

class NoteSubFolderApi : public QObject, public NoteSubFolder {
    Q_OBJECT

    Q_PROPERTY(int id READ getId() CONSTANT)
    Q_PROPERTY(QString name READ getName() WRITE setName)
    Q_PROPERTY(QQmlListProperty<NoteApi> notes READ notes CONSTANT)

   private:
    NoteSubFolder _noteSubFolder;
    QList<NoteApi *> _notes;

   public:
    NoteSubFolderApi *fetch(int id);
    static NoteSubFolderApi *fromNoteSubFolder(const NoteSubFolder &noteSubFolder);

    Q_INVOKABLE static NoteSubFolderApi *fetchNoteSubFolderById(int id);
    Q_INVOKABLE static NoteSubFolderApi *activeNoteSubFolder();
    Q_INVOKABLE static QList<QObject *> fetchNoteSubFoldersByParentId(int parentId);

    Q_INVOKABLE QQmlListProperty<NoteApi> notes();
    Q_INVOKABLE QString relativePath();
    Q_INVOKABLE QString fullPath();
};
