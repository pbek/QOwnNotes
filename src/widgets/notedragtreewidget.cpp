#include "notedragtreewidget.h"

#include <QDrag>
#include <QDragEnterEvent>
#include <QMenu>
#include <QMimeData>
#include <QPainter>
#include <QTimer>
#include <QUrl>

#include "entities/note.h"
#include "entities/notefolder.h"
#include "entities/notesubfolder.h"
#include "mainwindow.h"

namespace {
class NoteDragMimeData : public QMimeData {
   public:
    QVector<int> noteIds;
    QVector<int> folderIds;
    int noteFolderId = 0;
};
}    // namespace

NoteDragTreeWidget::NoteDragTreeWidget(QWidget *parent) : QTreeWidget(parent) {
    setDragEnabled(true);
    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    setDefaultDropAction(Qt::CopyAction);
    setDropIndicatorShown(true);
}

QStringList NoteDragTreeWidget::mimeTypes() const {
    return {QStringLiteral("application/x-qownnotes-items")};
}

Qt::DropActions NoteDragTreeWidget::supportedDropActions() const { return Qt::CopyAction; }

bool NoteDragTreeWidget::dropMimeData(QTreeWidgetItem *, int, const QMimeData *, Qt::DropAction) {
    // Filesystem moves must only go through the confirmed moveRequested workflow.
    return false;
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
QMimeData *NoteDragTreeWidget::mimeData(const QList<QTreeWidgetItem *> &items) const {
#else
QMimeData *NoteDragTreeWidget::mimeData(const QList<QTreeWidgetItem *> items) const {
#endif
    auto *data = new NoteDragMimeData;
    data->noteFolderId = NoteFolder::currentNoteFolderId();
    for (const auto *item : items) {
        const int id = item->data(0, Qt::UserRole).toInt();
        if (id <= 0) {
            continue;
        }
        if (item->data(0, Qt::UserRole + 1).toInt() == MainWindow::FolderType) {
            if (NoteSubFolder::fetch(id).isFetched()) {
                data->folderIds.append(id);
            }
        } else if (item->data(0, Qt::UserRole + 1).toInt() == MainWindow::NoteType) {
            if (Note::fetch(id).isFetched()) {
                data->noteIds.append(id);
            }
        }
    }

    // A selected folder already includes all its descendants, even in a flat folder list.
    QVector<int> includedFolderIds;
    for (int id : data->folderIds) {
        auto descendantIds = NoteSubFolder::fetchIdsRecursivelyByParentId(id);
        descendantIds.removeAll(id);
        includedFolderIds += descendantIds;
    }
    QList<QUrl> urls;
    QVector<int> folderIds;
    for (int id : data->folderIds) {
        if (!includedFolderIds.contains(id)) {
            folderIds.append(id);
            urls.append(QUrl::fromLocalFile(NoteSubFolder::fetch(id).fullPath()));
        }
    }
    QVector<int> noteIds;
    for (int id : data->noteIds) {
        const Note note = Note::fetch(id);
        const int folderId = note.getNoteSubFolderId();
        if (!data->folderIds.contains(folderId) && !includedFolderIds.contains(folderId)) {
            noteIds.append(id);
            urls.append(QUrl::fromLocalFile(note.fullNoteFilePath()));
        }
    }
    data->folderIds = folderIds;
    data->noteIds = noteIds;
    data->setUrls(urls);
    data->setData(QStringLiteral("application/x-qownnotes-items"), QByteArrayLiteral("1"));
    return data;
}

void NoteDragTreeWidget::startDrag(Qt::DropActions) {
    auto *data = static_cast<NoteDragMimeData *>(mimeData(selectedItems()));
    if (data->urls().isEmpty()) {
        delete data;
        return;
    }

    // Save edits before another application reads the files.
    emit saveRequested();
    if (data->noteFolderId != NoteFolder::currentNoteFolderId()) {
        delete data;
        return;
    }
    // Saving may rename a note, so resolve the paths again after saving.
    QList<QUrl> urls;
    for (int id : data->folderIds) {
        const NoteSubFolder folder = NoteSubFolder::fetch(id);
        if (folder.isFetched()) {
            urls.append(QUrl::fromLocalFile(folder.fullPath()));
        }
    }
    for (int id : data->noteIds) {
        const Note note = Note::fetch(id);
        if (note.isFetched()) {
            urls.append(QUrl::fromLocalFile(note.fullNoteFilePath()));
        }
    }
    if (urls.isEmpty()) {
        delete data;
        return;
    }
    data->setUrls(urls);
    // Qt may delete the drag during cancellation or defer deletion after exec().
    // It must be heap-allocated; it also owns and cleans up the MIME data.
    auto *drag = new QDrag(this);
    drag->setMimeData(data);
    // Never advertise Move externally: file managers could otherwise remove the originals.
    // Internal moves are performed explicitly after confirmation, not by Qt's item model.
    drag->exec(Qt::CopyAction, Qt::CopyAction);
}

Qt::DropAction NoteDragTreeWidget::chooseDropAction(Qt::KeyboardModifiers modifiers,
                                                    const QPoint &globalPosition) {
    // Ctrl takes precedence if both modifiers are held.
    if (modifiers.testFlag(Qt::ControlModifier)) {
        return Qt::CopyAction;
    }
    if (modifiers.testFlag(Qt::ShiftModifier)) {
        return Qt::MoveAction;
    }
    QMenu menu(this);
    auto *copy = menu.addAction(QIcon::fromTheme(QStringLiteral("edit-copy")), tr("Copy"));
    auto *move = menu.addAction(QIcon::fromTheme(QStringLiteral("edit-move")), tr("Move"));
    menu.addSeparator();
    menu.addAction(tr("Cancel"));
    const auto *choice = menu.exec(globalPosition);
    if (choice == copy) {
        return Qt::CopyAction;
    }
    return choice == move ? Qt::MoveAction : Qt::IgnoreAction;
}

bool NoteDragTreeWidget::acceptsDrag(const QDropEvent *event) const {
    const auto *data = dynamic_cast<const NoteDragMimeData *>(event->mimeData());
    return qobject_cast<NoteDragTreeWidget *>(event->source()) && data &&
           data->noteFolderId == NoteFolder::currentNoteFolderId();
}

void NoteDragTreeWidget::setInternalDragCursor(QDrag *drag, Qt::KeyboardModifiers modifiers) {
    _activeDrag = drag;
    const bool move =
        modifiers.testFlag(Qt::ShiftModifier) && !modifiers.testFlag(Qt::ControlModifier);
    // Advertise Copy to native targets for safety, but show the intended internal operation.
    // Qt's default dragCursor(MoveAction) can be null; passing it would reset to Copy.
    if (move && (_moveCursor.isNull() || _moveCursor.devicePixelRatio() != devicePixelRatioF())) {
        const qreal ratio = devicePixelRatioF();
        _moveCursor = QPixmap(qRound(32 * ratio), qRound(32 * ratio));
        _moveCursor.setDevicePixelRatio(ratio);
        _moveCursor.fill(Qt::transparent);
        QPainter painter(&_moveCursor);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(Qt::black, 1));
        painter.setBrush(Qt::white);
        // QCursor(pixmap) uses the center as its default hotspot, so put the tip there.
        painter.drawPolygon(
            QPolygonF({QPointF(16, 16), QPointF(16, 30), QPointF(20, 26), QPointF(23, 31),
                       QPointF(26, 29), QPointF(22, 24), QPointF(28, 24)}));
    }
    drag->setDragCursor(move ? _moveCursor : QPixmap(), Qt::CopyAction);
}

void NoteDragTreeWidget::updateDragCursor(const QDropEvent *event) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const Qt::KeyboardModifiers modifiers = event->modifiers();
#else
    const Qt::KeyboardModifiers modifiers = event->keyboardModifiers();
#endif
    // A previous drag may still await deferred cleanup, so match the MIME data too.
    for (auto *drag : event->source()->findChildren<QDrag *>()) {
        if (drag->mimeData() == event->mimeData()) {
            setInternalDragCursor(drag, modifiers);
            break;
        }
    }
}

int NoteDragTreeWidget::destinationFolderId(const QPoint &position) const {
    const auto *item = itemAt(position);
    if (!item) {
        return objectName() == QStringLiteral("noteTreeWidget") &&
                       !NoteFolder::isCurrentNoteTreeEnabled()
                   ? NoteSubFolder::activeNoteSubFolderId()
                   : 0;
    }
    const int id = item->data(0, Qt::UserRole).toInt();
    if (item->data(0, Qt::UserRole + 1).toInt() == MainWindow::FolderType) {
        return id == 0 || NoteSubFolder::fetch(id).isFetched() ? id : -1;
    }
    if (item->data(0, Qt::UserRole + 1).toInt() == MainWindow::NoteType) {
        const Note note = Note::fetch(id);
        return note.isFetched() ? note.getNoteSubFolderId() : -1;
    }
    return -1;
}

void NoteDragTreeWidget::dragEnterEvent(QDragEnterEvent *event) {
    if (acceptsDrag(event)) {
        updateDragCursor(event);
        event->setDropAction(Qt::CopyAction);
        event->accept();
    } else {
        event->ignore();
    }
}

bool NoteDragTreeWidget::canDropItems(const QMimeData *mimeData, int destinationFolderId) const {
    const auto *data = dynamic_cast<const NoteDragMimeData *>(mimeData);
    if (!data || destinationFolderId < 0) {
        return false;
    }
    for (int id : data->folderIds) {
        if (NoteSubFolder::fetchIdsRecursivelyByParentId(id).contains(destinationFolderId)) {
            return false;
        }
    }
    if (!data->folderIds.isEmpty()) {
        return true;
    }
    for (int id : data->noteIds) {
        const Note note = Note::fetch(id);
        if (note.isFetched() && note.getNoteSubFolderId() != destinationFolderId) {
            return true;
        }
    }
    return false;
}

void NoteDragTreeWidget::dragMoveEvent(QDragMoveEvent *event) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QPoint position = event->position().toPoint();
#else
    const QPoint position = event->pos();
#endif
    if (acceptsDrag(event) && canDropItems(event->mimeData(), destinationFolderId(position))) {
        updateDragCursor(event);
        QTreeWidget::dragMoveEvent(event);
        event->setDropAction(Qt::CopyAction);
        event->accept();
    } else {
        event->ignore();
    }
}

void NoteDragTreeWidget::dragLeaveEvent(QDragLeaveEvent *event) {
    if (_activeDrag) {
        _activeDrag->setDragCursor(QPixmap(), Qt::CopyAction);
        _activeDrag.clear();
    }
    QTreeWidget::dragLeaveEvent(event);
}

void NoteDragTreeWidget::dropEvent(QDropEvent *event) {
    if (!acceptsDrag(event)) {
        event->ignore();
        return;
    }
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QPoint position = event->position().toPoint();
#else
    const QPoint position = event->pos();
#endif
    const int destination = destinationFolderId(position);
    const QPoint globalPosition = viewport()->mapToGlobal(position);
    const auto *data = static_cast<const NoteDragMimeData *>(event->mimeData());
    if (!canDropItems(data, destination)) {
        event->ignore();
        return;
    }
    // Snapshot IDs before dialogs or index refreshes can invalidate the source items.
    QVector<int> noteIds;
    for (int id : data->noteIds) {
        const Note note = Note::fetch(id);
        if (note.isFetched() && note.getNoteSubFolderId() != destination) {
            noteIds.append(id);
        }
    }
    const QVector<int> folderIds = data->folderIds;
    const int noteFolderId = data->noteFolderId;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const Qt::KeyboardModifiers modifiers = event->modifiers();
#else
    const Qt::KeyboardModifiers modifiers = event->keyboardModifiers();
#endif
    event->setDropAction(Qt::CopyAction);
    event->accept();
    // Finish the native drag before opening dialogs or refreshing either tree.
    QTimer::singleShot(
        0, this,
        [this, noteIds, folderIds, destination, noteFolderId, modifiers, globalPosition]() {
            if (NoteFolder::currentNoteFolderId() != noteFolderId) {
                return;
            }
            const Qt::DropAction action = chooseDropAction(modifiers, globalPosition);
            if (NoteFolder::currentNoteFolderId() != noteFolderId) {
                return;
            }
            if (action == Qt::CopyAction) {
                emit copyRequested(noteIds, folderIds, destination);
            } else if (action == Qt::MoveAction) {
                emit moveRequested(noteIds, folderIds, destination);
            }
        });
}
