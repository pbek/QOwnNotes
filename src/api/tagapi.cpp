#include "tagapi.h"

TagApi* TagApi::fetch(int id) {
    Tag tag = Tag::fetch(id);

    if (tag.isFetched()) {
        this->id = tag.getId();
        name = tag.getName();
        parentId = tag.getParentId();
        priority = tag.getPriority();
    }

    return this;
}
