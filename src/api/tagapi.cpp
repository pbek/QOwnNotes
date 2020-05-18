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
