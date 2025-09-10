#ifndef NOMENUICONSTYLE_H
#define NOMENUICONSTYLE_H

#include <QMenu>
#include <QPainter>
#include <QProxyStyle>
#include <QStyleOptionMenuItem>

class NoMenuIconStyle : public QProxyStyle {
   public:
    explicit NoMenuIconStyle(QStyle *style = nullptr) : QProxyStyle(style) {}
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter,
                     const QWidget *widget) const override;
};

#endif    // NOMENUICONSTYLE_H
