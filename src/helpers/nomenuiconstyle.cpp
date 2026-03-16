#include "nomenuiconstyle.h"

void NoMenuIconStyle::drawControl(ControlElement element, const QStyleOption *option,
                                  QPainter *painter, const QWidget *widget) const {
    if (element == CE_MenuItem && widget && qobject_cast<const QMenu *>(widget)) {
        if (const auto *menuItem = qstyleoption_cast<const QStyleOptionMenuItem *>(option)) {
#ifdef __clang__
#if __has_warning("-Wdeprecated-copy-with-user-provided-copy")
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-copy-with-user-provided-copy"
#endif
#endif
            QStyleOptionMenuItem opt(*menuItem);    // Copy original option
#ifdef __clang__
#if __has_warning("-Wdeprecated-copy-with-user-provided-copy")
#pragma clang diagnostic pop
#endif
#endif
            opt.icon = QIcon();      // Remove the icon
            opt.maxIconWidth = 0;    // Prevent space allocation

            QProxyStyle::drawControl(element, &opt, painter, widget);
            return;
        }
    }
    QProxyStyle::drawControl(element, option, painter, widget);
}
