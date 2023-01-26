#include "tagapi.h"

#include <utility>

TagApi* TagApi::fetch(int Id) {
    Tag tag = Tag::fetch(Id);

    if (tag.isFetched()) {
        this->_id = tag.getId();
        _name = tag.getName();
        _parentId = tag.getParentId();
        _priority = tag.getPriority();
    }

    return this;
}

TagApi* TagApi::fetchByName(const QString& Name, int ParentId) {
    Tag tag = Tag::fetchByName(Name, ParentId);

    if (tag.isFetched()) {
        this->_id = tag.getId();
        this->_name = tag.getName();
        this->_parentId = tag.getParentId();
        this->_priority = tag.getPriority();
    }

    return this;
}

QStringList TagApi::getParentTagNames() {
    Tag tag = Tag::fetch(this->_id);

    return tag.getParentTagNames();
}

TagApi* TagApi::fromTag(const Tag& tag) {
    auto* tagApi = new TagApi();
    tagApi->fetch(tag.getId());
    return tagApi;
}

void TagApi::copy(const Tag& tag) {
    this->_id = tag.getId();
    _name = tag.getName();
    _parentId = tag.getParentId();
    _priority = tag.getPriority();
}

/**
 * Returns all linked notes
 */
QQmlListProperty<NoteApi> TagApi::notes() {
    _notes.clear();

    Tag tag = Tag::fetch(_id);
    QVector<Note> notes = tag.fetchAllLinkedNotes();

    QVectorIterator<Note> itr(notes);
    while (itr.hasNext()) {
        Note note = itr.next();

        auto* noteApi = new NoteApi();
        noteApi->copy(note);
        _notes.append(noteApi);
    }

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    return {this, _notes};
#else
    return {this, &_notes};
#endif
}
