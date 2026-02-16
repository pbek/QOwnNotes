// Copyright Waqar Ahmed <waqar.17a@gmail.com>

#pragma once

#include <functional>
#include <litehtml.h>
#include <QPalette>

class checkbox : public litehtml::html_tag
{
public:
    using litehtml::html_tag::html_tag;

    void parse_attributes() override;

    void draw(litehtml::uint_ptr hdc, int x, int y, const litehtml::position *clip) override;

    void set_checked(bool checked) { m_checked = checked; }

    void set_index(int index) { m_index = index; }

    int index() const { return m_index; }

    using ClickCallback = std::function<void(int)>;
    void set_click_callback(const ClickCallback &callback) { m_clickCallback = callback; }

    void on_click() override;

private:
    void addProperty(const litehtml::tchar_t *name, const litehtml::tchar_t *defaultValue);

    bool m_checked = false;
    int m_index = -1;
    ClickCallback m_clickCallback;
};
