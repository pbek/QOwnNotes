
#include "todoitemtreewidget.h"

#include <entities/calendaritem.h>

#include <QDebug>

TodoItemTreeWidget::TodoItemTreeWidget(QWidget *parent) : QTreeWidget(parent) { Q_UNUSED(parent); }

void TodoItemTreeWidget::dropEvent(QDropEvent *e) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QModelIndex droppedIndex = indexAt(e->position().toPoint());
#else
    QModelIndex droppedIndex = indexAt(e->pos());
#endif

    if (!droppedIndex.isValid()) {
        return;
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    auto destItem = itemAt(e->position().toPoint());
#else
    auto destItem = itemAt(e->pos());
#endif
    auto destUid = destItem->data(0, Qt::UserRole).toString();

    qDebug() << __func__ << " - 'destUid': " << destUid;

    auto uid = currentItem()->data(0, Qt::UserRole).toString();
    CalendarItem calItem = CalendarItem::fetchByUid(uid);

    if (!calItem.isFetched()) {
        return;
    }

    // update relate item
    calItem.setRelatedUid(destUid);
    calItem.store();

    emit calendarItemUpdated(calItem);

    // finish the move event
    QTreeWidget::dropEvent(e);
}
