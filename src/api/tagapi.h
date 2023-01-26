#pragma once

#include <QObject>

#include "entities/tag.h"
#include "noteapi.h"

class TagApi : public QObject, public Tag {
    Q_OBJECT
    Q_PROPERTY(int id READ getId CONSTANT)
    Q_PROPERTY(QString name READ getName WRITE setName)
    Q_PROPERTY(int parentId READ getParentId WRITE setParentId)
    Q_PROPERTY(QQmlListProperty<NoteApi> notes READ notes CONSTANT)

   private:
    QList<NoteApi *> _notes;

   public:
    TagApi *fetch(int id);
    Q_INVOKABLE TagApi *fetchByName(const QString &name, int parentId = 0);
    Q_INVOKABLE QStringList getParentTagNames();
    Q_INVOKABLE QQmlListProperty<NoteApi> notes();
    static TagApi *fromTag(const Tag &tag);
    void copy(const Tag &t);
};
