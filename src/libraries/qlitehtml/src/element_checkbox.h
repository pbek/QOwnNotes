// Copyright Waqar Ahmed <waqar.17a@gmail.com>

#pragma once

#include <litehtml.h>
#include <QPalette>

class checkbox : public litehtml::html_tag
{
public:
    using litehtml::html_tag::html_tag;

    void parse_attributes() override;

    void draw(litehtml::uint_ptr hdc,
              litehtml::pixel_t x,
              litehtml::pixel_t y,
              const litehtml::position *clip,
              const std::shared_ptr<litehtml::render_item> &ri) override;

    void set_checked(bool checked) { m_checked = checked; }

private:
    void addProperty(const char *name, const char *defaultValue);

    bool m_checked = false;
};
