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

#include "noterelationscene.h"

#include <QDebug>

// NoteItem Implementation
NoteItem::NoteItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent)
    : QGraphicsRectItem(x, y, width, height, parent) {
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);

    setBrush(QBrush(Qt::white));
    setPen(QPen(Qt::black, 2));
}

QVariant NoteItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionChange && scene()) {
        emit_position_changed();
    }
    return QGraphicsRectItem::itemChange(change, value);
}

void NoteItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QGraphicsRectItem::paint(painter, option, widget);
    painter->drawText(rect().adjusted(5, 5, -5, -5), Qt::AlignCenter, "Note");
}

void NoteItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        setSelected(true);
    }
    QGraphicsRectItem::mousePressEvent(event);
}

void NoteItem::emit_position_changed() {
    if (scene()) {
        scene()->update();
    }
}

// ConnectionLine Implementation
ConnectionLine::ConnectionLine(NoteItem *startItem, NoteItem *endItem, QGraphicsItem *parent)
    : QGraphicsLineItem(parent), m_startItem(startItem), m_endItem(endItem) {
    setPen(QPen(Qt::darkGray, 2, Qt::DashLine));
    updatePosition();
}

void ConnectionLine::updatePosition() {
    if (!m_startItem || !m_endItem) return;

    QPointF startCenter = m_startItem->rect().center() + m_startItem->pos();
    // TODO: Fix crash after scene was cleared and new items were created
    QPointF endCenter = m_endItem->rect().center() + m_endItem->pos();

    setLine(QLineF(startCenter, endCenter));
}

// NoteRelationScene Implementation
NoteRelationScene::NoteRelationScene(QObject *parent)
    : QGraphicsScene(parent), m_connecting(false), m_tempLine(nullptr), m_startItem(nullptr) {}

void NoteRelationScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QGraphicsItem *item = itemAt(event->scenePos(), QTransform());
        if (item && item->type() == QGraphicsRectItem::Type) {
            if (event->modifiers() & Qt::AltModifier) {
                m_connecting = true;
                m_startItem = dynamic_cast<NoteItem *>(item);

                m_tempLine = new QGraphicsLineItem(QLineF(event->scenePos(), event->scenePos()));
                m_tempLine->setPen(QPen(Qt::red, 2, Qt::DashLine));
                addItem(m_tempLine);

                return;
            }
        } else if (!item) {
            createNote(event->scenePos());
            return;
        }
    }

    QGraphicsScene::mousePressEvent(event);
}

void NoteRelationScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (m_connecting && m_tempLine && m_startItem) {
        QLineF newLine(m_startItem->rect().center() + m_startItem->pos(), event->scenePos());
        m_tempLine->setLine(newLine);
    } else {
        QGraphicsScene::mouseMoveEvent(event);
    }

    for (auto connection : m_connections) {
        connection->updatePosition();
    }
}

void NoteRelationScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (m_connecting && m_startItem && m_tempLine) {
        QGraphicsItem *endItem = itemAt(event->scenePos(), QTransform());

        if (endItem && endItem->type() == QGraphicsRectItem::Type && endItem != m_startItem) {
            auto *targetNote = dynamic_cast<NoteItem *>(endItem);
            if (targetNote) {
                createConnection(m_startItem, targetNote);
            }
        }

        removeItem(m_tempLine);
        delete m_tempLine;
        m_tempLine = nullptr;
        m_connecting = false;
    } else {
        QGraphicsScene::mouseReleaseEvent(event);
    }
}

void NoteRelationScene::createNote(const QPointF &pos, const QString &noteName) {
    // TODO: Handle memory leak
    auto *note = new NoteItem(0, 0, 100, 60);
    note->setPos(pos - QPointF(50, 30));
    addItem(note);
}

void NoteRelationScene::createConnection(NoteItem *startItem, NoteItem *endItem) {
    // TODO: Handle memory leak
    auto *connection = new ConnectionLine(startItem, endItem);
    addItem(connection);
    m_connections.push_back(connection);
}

void NoteRelationScene::drawForNote(const Note &note) {
    clear();
    qDebug() << __func__ << " - 'note': " << note;

    // TODO: Draw note item from note data
    // TODO: Write note name on note item
    createNote(QPointF(100, 100));
    createNote(QPointF(300, 100));
    createNote(QPointF(200, 200));
    createConnection(dynamic_cast<NoteItem *>(items().at(0)),
                     dynamic_cast<NoteItem *>(items().at(1)));
    createConnection(dynamic_cast<NoteItem *>(items().at(1)),
                     dynamic_cast<NoteItem *>(items().at(2)));
}
