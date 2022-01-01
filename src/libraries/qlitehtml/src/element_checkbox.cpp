#include "element_checkbox.h"

#include <QDebug>
#include <QPainter>
#include <QStylePainter>
#include <QStyleOptionButton>
#include <QCheckBox>
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "qlitehtml_checkbox", QtWarningMsg)

void checkbox::addProperty(const litehtml::tchar_t *name, const litehtml::tchar_t *defaultValue)
{
    const litehtml::tchar_t* attr_width = get_attr(name);
    if(attr_width) {
        qDebug(log) << name << attr_width;
        m_style.add_property(name, attr_width, nullptr, true);
    } else {
        qDebug(log) << "using default " << name << 10;
        m_style.add_property(name, defaultValue, nullptr, true);
    }
}

void checkbox::parse_attributes()
{
    addProperty(_t("width"), _t("10"));
    addProperty(_t("height"), _t("10"));
    addProperty(_t("margin-right"), _t("2"));
}

void checkbox::draw(litehtml::uint_ptr hdc, int x, int y, const litehtml::position* clip)
{
    auto pos = m_pos;
    pos.x += x;
    pos.y += y;

//     qDebug(log) << "draw checkbox " << QRect(pos.x, pos.y, pos.width, pos.height);

    auto *paint = reinterpret_cast<QPainter*>(hdc);
    const QRectF checkboxRect(pos.x, pos.y, pos.width, pos.height);
    paint->drawRoundedRect(checkboxRect, 1., 1.);

    if (m_checked) {
        QRect check = checkboxRect.toRect().adjusted(1, 1, -1, -1);
        auto brush = paint->brush();
        paint->setBrush(Qt::gray);
        paint->drawRect(check);
        paint->setBrush(brush);
    }
}
