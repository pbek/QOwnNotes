#include "graphicsview.h"

GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent) {
}

void GraphicsView::scrollContentsBy(int dx, int dy) {
    QGraphicsView::scrollContentsBy(dx, dy);
    emit(scrolledContentsBy(dx, dy));
}

void GraphicsView::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);

    int dw = event->size().width() - event->oldSize().width();
    int dh = event->size().height() - event->oldSize().height();
    emit(resizedBy(dw, dh));
}
