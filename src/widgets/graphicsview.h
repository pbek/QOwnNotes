#pragma once

#include <QGraphicsView>
#include <QResizeEvent>

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphicsView(QWidget *parent = nullptr);

protected:
    void scrollContentsBy(int dx, int dy) override;

    void resizeEvent(QResizeEvent *event) override;

Q_SIGNALS:
    void scrolledContentsBy(int dx, int dy);

    void resizedBy(int dw, int dh);
};
