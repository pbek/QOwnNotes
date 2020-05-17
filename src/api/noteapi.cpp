#include "noteapi.h"

#include <entities/notefolder.h>
#include <entities/tag.h>

#include <QVector>
#include <utility>

#include "tagapi.h"

NoteApi* NoteApi::fetch(int _id) {
    _note = Note::fetch(_id);

    if (_note.isFetched()) {
        this->_id = _note.getId();
        _name = _note.getName();
        _fileName = _note.getFileName();
        _noteText = _note.getNoteText();
        _hasDirtyData = _note.getHasDirtyData();
        _noteSubFolderId = _note.getNoteSubFolderId();
        _decryptedNoteText = _note.getDecryptedNoteText();
        _fileCreated = _note.getFileCreated();
        _fileLastModified = _note.getFileLastModified();
    }

    return this;
}

NoteApi* NoteApi::fromNote(const Note& note) {
    auto* noteApi = new NoteApi();
    noteApi->fetch(note.getId());
    return noteApi;
}

// NoteApi NoteApi::fromNote(Note note) {
//    NoteApi noteApi;
//    noteApi.fetch(note.getId());
//    return noteApi;
//}

/**
 * Returns all linked tags
 */
QQmlListProperty<TagApi> NoteApi::tags() {
    _tags.clear();

    Note note = Note::fetch(_id);
    QList<Tag> tags = Tag::fetchAllOfNote(note);
    QListIterator<Tag> itr(tags);
    while (itr.hasNext()) {
        Tag tag = itr.next();

        auto* tagApi = new TagApi();
        tagApi->fetch(tag.getId());
        _tags.append(tagApi);
    }

    return {this, _tags};
}

/**
 * Returns the names of all linked tags
 */
QStringList NoteApi::tagNames() const {
    QStringList tagNameList;
    Note note = Note::fetch(_id);
    QList<Tag> tags = Tag::fetchAllOfNote(note);
    QListIterator<Tag> itr(tags);
    while (itr.hasNext()) {
        Tag tag = itr.next();
        tagNameList.append(tag.getName());
    }

    return tagNameList;
}

/**
 * Adds a tag to the note
 *
 * @param tagName
 * @return true if the note was tagged
 */
bool NoteApi::addTag(const QString& tagName) {
    if (tagName.isEmpty()) {
        return false;
    }

    Note note = Note::fetch(_id);
    if (!note.exists()) {
        return false;
    }

    // create a new tag if it doesn't exist
    Tag tag = Tag::fetchByName(tagName);
    if (!tag.isFetched()) {
        tag.setName(tagName);
        tag.store();
    }

    return tag.linkToNote(note);
}

/**
 * Removes a tag from the note
 *
 * @param tagName
 * @return true if the tag was removed from the note
 */
bool NoteApi::removeTag(QString tagName) {
    Tag tag = Tag::fetchByName(std::move(tagName));
    if (!tag.exists()) {
        return false;
    }

    Note note = Note::fetch(_id);
    if (!note.exists()) {
        return false;
    }

    return tag.removeLinkToNote(note);
}

/**
 * Renames a note file
 *
 * @param newName new file name (without file-extension)
 * @return true if the note was renamed
 */
bool NoteApi::renameNoteFile(const QString &newName) {
    Note note = Note::fetch(_id);

    if (note.exists()) {
        return note.renameNoteFile(newName);
    }

    return false;
}

/**
 * Checks if it is allowed to have a different note file name than the headline
 *
 * @return bool
 */
bool NoteApi::allowDifferentFileName() {
    return Note::allowDifferentFileName();
}

/**
 * Fetches all notes
 * Disclaimer: not tested, might not work yet
 *
 * @param limit
 * @param offset
 * @return
 */
QQmlListProperty<NoteApi> NoteApi::fetchAll(int limit, int offset) {
    QVector<int> noteIds = Note::fetchAllIds(limit, offset);
    QList<NoteApi*> notes;

    Q_FOREACH (int noteId, noteIds) {
        NoteApi* note = NoteApi::fetch(noteId);
        notes.append(note);
    }

    return {this, notes};
}

/**
 * Returns the generated html for a note
 *
 * @param forExport if true (default) the export-html will be generated
 * @return
 */
QString NoteApi::toMarkdownHtml(bool forExport) {
    return _note.toMarkdownHtml(NoteFolder::currentLocalPath(), 980, forExport,
                                true, true);
}

/**
 * Returns the absolute file url from a relative file name
 *
 * @param fileName
 * @return
 */
QString NoteApi::getFileURLFromFileName(const QString& localFileName) {
    return _note.getFileURLFromFileName(localFileName);
}
