#include "element_checkbox.h"

#include <QApplication>
#include <QCheckBox>
#include <QDebug>
#include <QLoggingCategory>
#include <QPainter>
#include <QStyleOptionButton>
#include <QStylePainter>

#include <litehtml/render_item.h>

static Q_LOGGING_CATEGORY(log, "qlitehtml_checkbox", QtWarningMsg)

    void checkbox::addProperty(const char *name, const char *defaultValue)
{
    const char *attr_width = get_attr(name);
    if (attr_width) {
        qDebug(log) << name << attr_width;
        m_style.add_property(litehtml::_id(name), attr_width, "", true, nullptr);
    } else {
        qDebug(log) << "using default " << name << 10;
        m_style.add_property(litehtml::_id(name), defaultValue, "", true, nullptr);
    }
}

void checkbox::parse_attributes()
{
    addProperty("width", "12");
    addProperty("height", "12");
    addProperty("margin-right", "5");
}

void checkbox::draw(litehtml::uint_ptr hdc,
                    litehtml::pixel_t x,
                    litehtml::pixel_t y,
                    const litehtml::position *clip,
                    const std::shared_ptr<litehtml::render_item> &ri)
{
    Q_UNUSED(clip)
    auto pos = ri ? ri->get_placement() : litehtml::position();
    pos.x += x;
    pos.y += y;

    //     qDebug(log) << "draw checkbox " << QRect(pos.x, pos.y, pos.width, pos.height);

    auto palette = qApp->palette();

    auto *paint = reinterpret_cast<QPainter *>(hdc);
    const QRectF checkboxRect(pos.x, pos.y, pos.width, pos.height);

    auto savedBrush = paint->brush();
    auto savedPen = paint->pen();

    paint->setPen(palette.windowText().color());
    paint->setBrush(palette.base());

    paint->drawRoundedRect(checkboxRect, 1., 1.);

    if (m_checked) {
        QRect check = checkboxRect.toRect().adjusted(2, 2, -2, -2);
        paint->setBrush(palette.windowText().color());
        paint->drawRect(check);
    }

    // restore
    paint->setPen(savedPen);
    paint->setBrush(savedBrush);
}
