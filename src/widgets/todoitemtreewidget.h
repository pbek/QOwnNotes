#pragma once

#include <entities/calendaritem.h>

#include <QDropEvent>
#include <QTreeWidget>

class TodoItemTreeWidget : public QTreeWidget {
    Q_OBJECT
   public:
    explicit TodoItemTreeWidget(QWidget *parent = nullptr);

   protected:
    void dropEvent(QDropEvent *e) override;

   Q_SIGNALS:
    void calendarItemUpdated(CalendarItem item);
};
