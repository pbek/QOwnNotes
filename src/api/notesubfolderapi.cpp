#include "notesubfolderapi.h"

#include <entities/note.h>

#include <QVector>

#include "noteapi.h"

NoteSubFolderApi *NoteSubFolderApi::fetch(int id) {
    _noteSubFolder = NoteSubFolder::fetch(id);

    if (_noteSubFolder.isFetched()) {
        _id = _noteSubFolder.getId();
        _name = _noteSubFolder.getName();
    }

    return this;
}

NoteSubFolderApi *NoteSubFolderApi::fromNoteSubFolder(const NoteSubFolder &noteSubFolder) {
    auto* noteSubFolderApi = new NoteSubFolderApi();
    noteSubFolderApi->fetch(noteSubFolder.getId());
    return noteSubFolderApi;
}

/**
 * Returns all linked notes
 */
QQmlListProperty<NoteApi> NoteSubFolderApi::notes() {
    _notes.clear();

    NoteSubFolder noteSubFolder = NoteSubFolder::fetch(_id);
    QVector<Note> notes = Note::fetchAllByNoteSubFolderId(_id);

    QVectorIterator<Note> itr(notes);
    while (itr.hasNext()) {
        Note note = itr.next();

        auto* noteApi = new NoteApi();
        noteApi->fetch(note.getId());
        _notes.append(noteApi);
    }

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    return {this, _notes};
#else
    return {this, &_notes};
#endif
}

