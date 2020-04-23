#include "tagapi.h"

#include <utility>

TagApi* TagApi::fetch(int Id) {
    Tag tag = Tag::fetch(Id);

    if (tag.isFetched()) {
        this->id = tag.getId();
        name = tag.getName();
        parentId = tag.getParentId();
        priority = tag.getPriority();
    }

    return this;
}

TagApi* TagApi::fetchByName(const QString& Name, int ParentId) {
    Tag tag = Tag::fetchByName(Name, ParentId);

    if (tag.isFetched()) {
        this->id = tag.getId();
        this->name = tag.getName();
        this->parentId = tag.getParentId();
        this->priority = tag.getPriority();
    }

    return this;
}

TagApi* TagApi::fromTag(const Tag& tag) {
    auto* tagApi = new TagApi();
    tagApi->fetch(tag.getId());
    return tagApi;
}
