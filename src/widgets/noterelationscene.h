/*
 * Copyright (c) 2014-2025 Patrizio Bekerle -- <patrizio@bekerle.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

#pragma once

#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <vector>

#include "entities/note.h"

class NoteItem;
class ConnectionLine;

// Note rectangle item representing a note
class NoteItem : public QGraphicsRectItem {
   public:
    NoteItem(Note &note, qreal x, qreal y, qreal width, qreal height, int level = 0,
             QGraphicsItem *parent = nullptr);

   protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    QString _noteName;
    int _noteId;

   private:
    void emit_position_changed();
};

// Line item representing a connection/rubberband
class ConnectionLine : public QGraphicsLineItem {
   public:
    ConnectionLine(NoteItem *startItem, NoteItem *endItem, QGraphicsItem *parent = nullptr);
    void updatePosition();

   private:
    NoteItem *m_startItem;
    NoteItem *m_endItem;
};

// Main scene class handling connections
class NoteRelationScene : public QGraphicsScene {
    Q_OBJECT

   public:
    explicit NoteRelationScene(QObject *parent = nullptr);
    void drawForNote(const Note& note);

   protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

   private:
    NoteItem *createNoteItem(const QPointF &pos, Note note, int level = 0);
    void createConnection(NoteItem *startItem, NoteItem *endItem);

    bool m_connecting;
    QGraphicsLineItem *m_tempLine;
    NoteItem *m_startItem;
    std::vector<ConnectionLine *> m_connections;
    static QPointF calculateRadialPosition(QPointF center, int index, int total, qreal radius);
    void createLinkedNoteItems(const QVector<Note>& noteList, Note note, NoteItem *rootNoteItem, int level = 0);
};
