#ifndef NOMENUICONSTYLE_H
#define NOMENUICONSTYLE_H

#include <QMenu>
#include <QPainter>
#include <QProxyStyle>
#include <QStyleOptionMenuItem>

class NoMenuIconStyle : public QProxyStyle {
   public:
    NoMenuIconStyle() = default;    // Explicitly define constructor
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter,
                     const QWidget *widget) const override;
};

#endif    // NOMENUICONSTYLE_H
