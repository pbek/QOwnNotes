#ifndef NOMENUICONSTYLE_H
#define NOMENUICONSTYLE_H

#include <QProxyStyle>
#include <QStyleOptionMenuItem>
#include <QMenu>
#include <QPainter>

class NoMenuIconStyle : public QProxyStyle {
public:
    NoMenuIconStyle() = default;  // Explicitly define constructor
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter,
                     const QWidget *widget) const override;
};

#endif // NOMENUICONSTYLE_H
