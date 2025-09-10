#include "noteapi.h"

#include <entities/notefolder.h>
#include <entities/tag.h>

#include <QVector>
#include <utility>

#include "tagapi.h"

NoteApi* NoteApi::fetch(int id) {
    _note = Note::fetch(id);

    if (_note.isFetched()) {
        _id = _note.getId();
        _name = _note.getName();
        _fileName = _note.getFileName();
        _noteText = _note.getNoteText();
        _hasDirtyData = _note.getHasDirtyData();
        _noteSubFolderId = _note.getNoteSubFolderId();
        _fileCreated = _note.getFileCreated();
        _fileLastModified = _note.getFileLastModified();

        // we'll try not to fetch the decrypted note text, because it
        // would be done every time the current note changes
        _decryptedNoteText = _note.getDecryptedNoteText();
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
    QVector<Tag> tags = Tag::fetchAllOfNote(note);
    QVectorIterator<Tag> itr(tags);
    while (itr.hasNext()) {
        Tag tag = itr.next();

        auto* tagApi = new TagApi();
        tagApi->copy(tag);
        _tags.append(tagApi);
    }
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    return {this, _tags};
#else
    return {this, &_tags};
#endif
}

/**
 * Returns the names of all linked tags
 */
QStringList NoteApi::tagNames() const {
    QStringList tagNameList;
    Note note = Note::fetch(_id);
    QVector<Tag> tags = Tag::fetchAllOfNote(note);
    QVectorIterator<Tag> itr(tags);
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
    if (!note.isFetched()) {
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
    if (!tag.isFetched()) {
        return false;
    }

    Note note = Note::fetch(_id);
    if (!note.isFetched()) {
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
bool NoteApi::renameNoteFile(const QString& newName) {
    Note note = Note::fetch(_id);

    if (note.isFetched()) {
        return note.renameNoteFile(newName);
    }

    return false;
}

/**
 * Checks if it is allowed to have a different note file name than the headline
 *
 * @return bool
 */
bool NoteApi::allowDifferentFileName() { return Note::allowDifferentFileName(); }

/**
 * Fetches all notes
 * Disclaimer: not tested, might not work yet
 *
 * @param limit
 * @param offset
 * @return
 */
QQmlListProperty<NoteApi> NoteApi::fetchAll(int limit, int offset) {
    const QVector<int> noteIds = Note::fetchAllIds(limit, offset);
    QList<NoteApi*> notes;

    for (int noteId : noteIds) {
        NoteApi* note = NoteApi::fetch(noteId);
        notes.append(note);
    }
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    return {this, notes};
#else
    return {this, &notes};
#endif
}

/**
 * Returns the generated html for a note
 *
 * @param forExport if true (default) the export-html will be generated
 * @return
 */
QString NoteApi::toMarkdownHtml(bool forExport) {
    return _note.toMarkdownHtml(NoteFolder::currentLocalPath(), 980, forExport, true, true);
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

/**
 * Returns the Markdown note url for linking to the note with noteId
 *
 * @param noteId
 * @return
 */
QString NoteApi::getNoteUrlForLinkingToNoteId(int noteId) {
    const Note note = Note::fetch(noteId);
    return _note.getNoteUrlForLinkingTo(note);
}

void NoteApi::copy(const Note& note) {
    _note = note;

    if (_note.isFetched()) {
        _id = note.getId();
        _name = note.getName();
        _fileName = note.getFileName();
        _noteText = note.getNoteText();
        _hasDirtyData = note.getHasDirtyData();
        _noteSubFolderId = note.getNoteSubFolderId();
        _fileCreated = note.getFileCreated();
        _fileLastModified = note.getFileLastModified();

        // we'll try not to fetch the decrypted note text, because it
        // would be done every time the current note changes
        _decryptedNoteText = note.getDecryptedNoteText();
    }
}
