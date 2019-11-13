#include <utility>
#include "tagapi.h"

TagApi* TagApi::fetch(int _id) {
    Tag tag = Tag::fetch(_id);

    if (tag.isFetched()) {
        this->id = tag.getId();
        name = tag.getName();
        parentId = tag.getParentId();
        priority = tag.getPriority();
    }

    return this;
}

TagApi* TagApi::fetchByName(QString _name, int _parentId) {
    Tag tag = Tag::fetchByName(std::move(_name), _parentId);

    if (tag.isFetched()) {
        this->id = tag.getId();
        this->name = tag.getName();
        this->parentId = tag.getParentId();
        this->priority = tag.getPriority();
    }

    return this;
}
