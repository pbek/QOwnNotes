#pragma once

#include <QObject>

#include "entities/tag.h"

class TagApi : public QObject, public Tag {
    Q_OBJECT
    Q_PROPERTY(int id READ getId CONSTANT)
    Q_PROPERTY(QString name READ getName WRITE setName)
    Q_PROPERTY(int parentId READ getParentId WRITE setParentId)

   public:
    TagApi *fetch(int id);
    Q_INVOKABLE TagApi *fetchByName(const QString &name, int parentId = 0);
    Q_INVOKABLE QStringList getParentTagNames();
    static TagApi* fromTag(const Tag& tag);
};
