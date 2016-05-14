#include <entities/tag.h>
#include "noteapi.h"

NoteApi* NoteApi::fetch(int id) {
    Note note = Note::fetch(id);

    if (note.isFetched()) {
        this->id = note.getId();
        name = note.getName();
        fileName = note.getFileName();
        noteText = note.getNoteText();
        hasDirtyData = note.getHasDirtyData();
        decryptedNoteText = note.getDecryptedNoteText();
    }

    return this;
}

/**
 * Returns all linked tags
 */
QList<TagApi*> NoteApi::tags() {
    QList<TagApi*> tagApiList;
    Note note = Note::fetch(id);
    QList<Tag> tags = Tag::fetchAllOfNote(note);
    QListIterator<Tag> itr(tags);
    while (itr.hasNext()) {
        Tag tag = itr.next();

        TagApi* tagApi = new TagApi();
        tagApi->fetch(tag.getId());
        tagApiList << tagApi;
    }

    return tagApiList;
}

/**
 * Returns the names of all linked tags
 */
QStringList NoteApi::tagNames() {
    QStringList tagNameList;
    Note note = Note::fetch(id);
    QList<Tag> tags = Tag::fetchAllOfNote(note);
    QListIterator<Tag> itr(tags);
    while (itr.hasNext()) {
        Tag tag = itr.next();
        tagNameList << tag.getName();
    }

    return tagNameList;
}
