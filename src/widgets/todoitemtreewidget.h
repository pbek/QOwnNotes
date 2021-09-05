#pragma once

#include <entities/calendaritem.h>

#include <QDropEvent>
#include <QTreeWidget>

class TodoItemTreeWidget : public QTreeWidget {
    Q_OBJECT
   public:
    explicit TodoItemTreeWidget(QWidget *parent = Q_NULLPTR);
   protected:
    void dropEvent(QDropEvent *e);

   Q_SIGNALS:
    void calendarItemUpdated(CalendarItem item);
};

