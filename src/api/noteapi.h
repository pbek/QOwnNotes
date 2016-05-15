#pragma once

#include <QObject>
#include <QStringList>
#include "entities/note.h"
#include "tagapi.h"

class NoteApi : public QObject, public Note {
    Q_OBJECT

    Q_PROPERTY(int id READ getId())
    Q_PROPERTY(QString name READ getName() WRITE setName)
    Q_PROPERTY(QString fileName READ getFileName())
    Q_PROPERTY(QString noteText READ getNoteText() WRITE setNoteText())
    Q_PROPERTY(QString decryptedNoteText READ getDecryptedNoteText()
    WRITE setDecryptedNoteText())
    Q_PROPERTY(bool hasDirtyData READ getHasDirtyData())

public:
    NoteApi *fetch(int id);
    static NoteApi *fromNote(Note note);
//    static NoteApi fromNote(Note note);

    Q_INVOKABLE QList<TagApi*> tags();
    Q_INVOKABLE QStringList tagNames() const;
};
