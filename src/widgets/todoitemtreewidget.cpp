
#include "todoitemtreewidget.h"

#include <entities/calendaritem.h>

#include <QDebug>

TodoItemTreeWidget::TodoItemTreeWidget(QWidget *parent) : QTreeWidget(parent) { Q_UNUSED(parent); }

void TodoItemTreeWidget::dropEvent(QDropEvent *e) {
    QModelIndex droppedIndex = indexAt(e->pos());

    if (!droppedIndex.isValid()) {
        return;
    }

    auto destItem = itemAt(e->pos());
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
