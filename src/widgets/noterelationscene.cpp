/*
 * Copyright (c) 2014-2026 Patrizio Bekerle -- <patrizio@bekerle.com>
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
#include <QGraphicsView>
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
#include <QRandomGenerator>
#endif
#include <QtConcurrent/QtConcurrentRun>
#include <cmath>

#include "mainwindow.h"
#include "services/databaseservice.h"

// NoteItem Implementation
NoteItem::NoteItem(Note &note, qreal x, qreal y, qreal width, qreal height, int level,
                   QGraphicsItem *parent)
    : QGraphicsRectItem(x, y, width, height, parent) {
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);

    setBrush(QBrush(Qt::white));
    const int penWidth = std::max(5 - level, 2);
    setPen(QPen(Qt::black, penWidth));
    setToolTip(QStringLiteral("<b>") + note.getName() + QStringLiteral("</b><br>") +
               QObject::tr("Double-click to open note"));
    _noteName = note.getName();
    _noteId = note.getId();
}

QVariant NoteItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionChange && scene()) {
        emit_position_changed();
    }
    return QGraphicsRectItem::itemChange(change, value);
}

void NoteItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QGraphicsRectItem::paint(painter, option, widget);
    painter->drawText(rect().adjusted(5, 5, -5, -5), Qt::AlignCenter, _noteName);
}

void NoteItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        MainWindow *mainWindow = MainWindow::instance();
        if (mainWindow != nullptr) {
            mainWindow->setCurrentNoteFromNoteId(_noteId);
            return;
        }
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
    QPointF endCenter = m_endItem->rect().center() + m_endItem->pos();

    setLine(QLineF(startCenter, endCenter));
}

// NoteRelationScene Implementation
NoteRelationScene::NoteRelationScene(QObject *parent)
    : QGraphicsScene(parent), m_connecting(false), m_tempLine(nullptr), m_startItem(nullptr) {
    // Connect the signal to the slot with a queued connection
    connect(this, &NoteRelationScene::addItemRequested, this, &NoteRelationScene::addItemToScene,
            Qt::QueuedConnection);
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

NoteItem *NoteRelationScene::createNoteItem(const QPointF &pos, Note note, int level) {
    qreal xpos = fmax(140 - level * 20, 80);
    qreal ypos = fmax(90 - level * 15, 40);
    auto posPoint = QPointF(pos - QPointF(xpos / 2, ypos / 2));
    auto rect = QRectF(pos, QSizeF(xpos, ypos));

    int tries = 0;
    while (!items(rect).empty() && tries++ < 10) {
#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
        int randomX = qrand();
        int randomY = qrand();
#else
        auto randomX = QRandomGenerator::global()->generate();
        auto randomY = QRandomGenerator::global()->generate();
#endif

        int randX = static_cast<int>(randomX % 150) - 75;
        int randY = static_cast<int>(randomY % 150) - 75;
        posPoint = QPointF(pos - QPointF(randX, randY));
        rect = QRectF(posPoint - QPointF(xpos / 2, ypos / 2), QSizeF(xpos, ypos));
        // qDebug() << __func__ << " - 'tries': " << tries;
    }

    auto *noteItem = new NoteItem(note, 0, 0, xpos, ypos, level);
    noteItem->setPos(posPoint);
    // The scene is taking ownership over the note item
    //    addItem(noteItem);
    emit addItemRequested(noteItem);

    return noteItem;
}

void NoteRelationScene::createConnection(NoteItem *startItem, NoteItem *endItem) {
    auto *connection = new ConnectionLine(startItem, endItem);
    //    addItem(connection);
    emit addItemRequested(connection);
    m_connections.push_back(connection);
}

void NoteRelationScene::addItemToScene(QGraphicsItem *item) {
    // This will run in the GUI thread
    addItem(item);
}

void NoteRelationScene::drawForNote(const Note &note) {
    qDebug() << __func__
             << " - 'this->m_drawFuture.isRunning()': " << this->m_drawFuture.isRunning();
    // Disallow more drawing and cancel the current thread
    stopDrawing();

    // Fetch all notes while waiting for the previous thread to finish
    const auto noteList = Note::fetchAll();

    // Wait a little more for if the thread is still running
    if (this->m_drawFuture.isRunning()) {
        this->m_drawFuture.waitForFinished();
    }

    // Allow drawing again and clear the scene
    setAllowDrawing();
    m_connections.clear();
    m_noteItems.clear();
    clear();

    // This runs the gathering of note relations in a different thread
    // The drawing of the note items and connections will be done in the GUI thread, because the
    // QGraphicsView framework is not designed for multithreading (QTimer errors)
    // A different database connection to the memory database will also be use, because you cannot
    // use the same connection in different threads
    this->m_drawFuture = QtConcurrent::run([this, note, noteList]() {
        const QString connectionName = DatabaseService::generateConnectionName();

        {
            QSqlDatabase db = DatabaseService::createSharedMemoryDatabase(connectionName);

            auto rootNoteItem = createNoteItem(QPointF(100, 100), note);
            m_noteItems[note.getId()] = rootNoteItem;

            createLinkedNoteItems(noteList, connectionName, note, rootNoteItem);

            // Update all connections
            for (auto connection : m_connections) {
                connection->updatePosition();
            }

            // Update the scene
            update();

            db.close();
        }    // db goes out of scope and is removed, so the database can be removed

        // Remove database connection after the database is closed
        QSqlDatabase::removeDatabase(connectionName);
    });
}

void NoteRelationScene::stopDrawing() {
    if (m_drawFuture.isRunning()) {
        // Disallow more drawing and cancel the thread
        setAllowDrawing(false);
        m_drawFuture.cancel();
    }
}

void NoteRelationScene::setAllowDrawing(bool allow) { m_allowDrawing = allow; }

/**
 * Recursively creates linked note items around the root note item.
 * The position of the linked note items is calculated in a circle around the
 * root note item, with a radius that increases based on the number of linked
 * notes and decreases based on the level of recursion.
 * The function also creates connections between the root note item and the
 * linked note items, and continues to create linked note items for each linked
 * note until a certain recursion level is reached.
 *
 * @param noteList
 * @param connectionName
 * @param note
 * @param rootNoteItem
 * @param level
 */
void NoteRelationScene::createLinkedNoteItems(const QVector<Note> &noteList,
                                              const QString &connectionName, Note note,
                                              NoteItem *rootNoteItem, int level) {
    if (!this->m_allowDrawing) {
        return;
    }

    // Get linked notes for the current note
    auto linkedNotes = note.findLinkedNotes(noteList, connectionName);

    // Get root note position (center)
    QPointF rootCenter = rootNoteItem->pos() + rootNoteItem->rect().center();

    // Calculate radius based on number of notes
    qreal radius = 250.0;    // Base radius
    if (linkedNotes.size() > 5) {
        radius += (linkedNotes.size() - 5) * 20.0;    // Increase radius for more notes
    }

    // Decrease radius based on level
    radius = radius - level * 60;

    // Create linked notes around the root
    int index = 0;
    level = level + 1;
    for (auto it = linkedNotes.begin(); it != linkedNotes.end(); ++it) {
        if (!this->m_allowDrawing) {
            break;
        }

        // Calculate position in a circle around the root note item
        QPointF notePos = calculateRadialPosition(rootCenter, index, linkedNotes.size(), radius);
        const Note &linkedNote = it.key();
        int linkedNoteId = linkedNote.getId();

        // Check if note item already exists
        NoteItem *linkedNoteItem = m_noteItems[linkedNoteId];

        if (linkedNoteItem == nullptr) {
            // Create note item at calculated position
            linkedNoteItem = createNoteItem(notePos, linkedNote, level);
            m_noteItems[linkedNoteId] = linkedNoteItem;
        }

        // Create connection between root and this note
        createConnection(rootNoteItem, linkedNoteItem);

        if (level < 3) {
            createLinkedNoteItems(noteList, connectionName, linkedNote, linkedNoteItem, level);
        }

        index++;
    }
}

QPointF NoteRelationScene::calculateRadialPosition(QPointF center, int index, int total,
                                                   qreal radius) {
    if (total <= 0) return center;

    // Calculate angle
    qreal angleStep = 2.0 * M_PI / total;
    qreal angle = index * angleStep;

    // Calculate position using polar coordinates
    qreal x = center.x() + radius * cos(angle);
    qreal y = center.y() + radius * sin(angle);

    return {x, y};
}

ZoomableGraphicsView::ZoomableGraphicsView(QWidget *parent) : QGraphicsView(parent) {
    // Set view properties
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
}

void ZoomableGraphicsView::wheelEvent(QWheelEvent *event) {
    // Zoom in or out depending on wheel direction
    if (event->angleDelta().y() > 0) {
        // Zoom in
        scale(scaleFactor, scaleFactor);
    } else {
        // Zoom out
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}
