#include "graphicsview.h"

#include <QResizeEvent>

GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent) {}

void GraphicsView::scrollContentsBy(int dx, int dy) {
    QGraphicsView::scrollContentsBy(dx, dy);
    emit scrolledContentsBy(dx, dy);
}

void GraphicsView::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);

    const int dw = event->size().width() - event->oldSize().width();
    const int dh = event->size().height() - event->oldSize().height();
    emit resizedBy(dw, dh);
}
