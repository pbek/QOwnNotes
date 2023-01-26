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
    auto *noteSubFolderApi = new NoteSubFolderApi();
    noteSubFolderApi->fetch(noteSubFolder.getId());
    return noteSubFolderApi;
}

/**
 * Returns all notes of the note subfolder
 */
QQmlListProperty<NoteApi> NoteSubFolderApi::notes() {
    _notes.clear();

    NoteSubFolder noteSubFolder = NoteSubFolder::fetch(_id);
    QVector<Note> notes = Note::fetchAllByNoteSubFolderId(_id);

    QVectorIterator<Note> itr(notes);
    while (itr.hasNext()) {
        Note note = itr.next();

        auto *noteApi = new NoteApi();
        noteApi->fetch(note.getId());
        _notes.append(noteApi);
    }

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    return {this, _notes};
#else
    return {this, &_notes};
#endif
}

/**
 * Fetches a note subfolder by its id
 *
 * @param id int the id of the note subfolder
 * @return NoteSubFolderApi*
 */
NoteSubFolderApi *NoteSubFolderApi::fetchNoteSubFolderById(int id) {
    auto *noteSubFolder = new NoteSubFolderApi();
    noteSubFolder->fetch(id);
    return noteSubFolder;
}

/**
 * Fetches note subfolders that are children of parentId
 *
 * @param parentId int the id of the parent note subfolder
 * @return QList<QObject*>
 */
QList<QObject *> NoteSubFolderApi::fetchNoteSubFoldersByParentId(int parentId) {
    QList<QObject *> noteSubFolderApis;

    const auto noteSubFolders = NoteSubFolder::fetchAllByParentId(parentId);
    for (const auto &noteSubFolder : noteSubFolders) {
        noteSubFolderApis.append(NoteSubFolderApi::fromNoteSubFolder(noteSubFolder));
    }

    return noteSubFolderApis;
}

NoteSubFolderApi *NoteSubFolderApi::activeNoteSubFolder() {
    return fetchNoteSubFolderById(NoteSubFolder::activeNoteSubFolderId());
}

QString NoteSubFolderApi::relativePath() { return _noteSubFolder.relativePath(); }

QString NoteSubFolderApi::fullPath() { return _noteSubFolder.fullPath(); }
