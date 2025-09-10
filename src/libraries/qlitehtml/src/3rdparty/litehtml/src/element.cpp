#include "element.h"

#include "document.h"
#include "html.h"

#define LITEHTML_EMPTY_FUNC \
  {                         \
  }
#define LITEHTML_RETURN_FUNC(ret) \
  {                               \
    return ret;                   \
  }

litehtml::element::element(const std::shared_ptr<litehtml::document>& doc) : m_doc(doc) {
  m_box = nullptr;
  m_skip = false;
}

bool litehtml::element::is_point_inside(int x, int y) {
  if (get_display() != display_inline && get_display() != display_table_row) {
    position pos = m_pos;
    pos += m_padding;
    pos += m_borders;
    if (pos.is_point_inside(x, y)) {
      return true;
    } else {
      return false;
    }
  } else {
    position::vector boxes;
    get_inline_boxes(boxes);
    for (auto& box : boxes) {
      if (box.is_point_inside(x, y)) {
        return true;
      }
    }
  }
  return false;
}

litehtml::web_color litehtml::element::get_color(const tchar_t* prop_name, bool inherited, const litehtml::web_color& def_color) {
  const tchar_t* clrstr = get_style_property(prop_name, inherited, nullptr);
  if (!clrstr) {
    return def_color;
  }
  return web_color::from_string(clrstr, get_document()->container());
}

litehtml::position litehtml::element::get_placement() const {
  litehtml::position pos = m_pos;
  element::ptr cur_el = parent();
  while (cur_el) {
    pos.x += cur_el->m_pos.x;
    pos.y += cur_el->m_pos.y;
    cur_el = cur_el->parent();
  }
  return pos;
}

bool litehtml::element::is_inline_box() const {
  style_display d = get_display();
  if (d == display_inline || d == display_inline_table || d == display_inline_block || d == display_inline_text) {
    return true;
  }
  return false;
}

bool litehtml::element::collapse_top_margin() const {
  if (!m_borders.top && !m_padding.top && in_normal_flow() && get_float() == float_none && m_margins.top >= 0 && have_parent()) {
    return true;
  }
  return false;
}

bool litehtml::element::collapse_bottom_margin() const {
  if (!m_borders.bottom && !m_padding.bottom && in_normal_flow() && get_float() == float_none && m_margins.bottom >= 0 && have_parent()) {
    return true;
  }
  return false;
}

bool litehtml::element::get_predefined_height(int& p_height) const {
  css_length h = get_css_height();
  if (h.is_predefined()) {
    p_height = m_pos.height;
    return false;
  }
  if (h.units() == css_units_percentage) {
    element::ptr el_parent = parent();
    if (!el_parent) {
      position client_pos;
      get_document()->container()->get_client_rect(client_pos);
      p_height = h.calc_percent(client_pos.height);
      return true;
    } else {
      int ph = 0;
      if (el_parent->get_predefined_height(ph)) {
        p_height = h.calc_percent(ph);
        if (is_body()) {
          p_height -= content_margins_height();
        }
        return true;
      } else {
        p_height = m_pos.height;
        return false;
      }
    }
  }
  p_height = get_document()->cvt_units(h, get_font_size());
  return true;
}

void litehtml::element::calc_document_size(litehtml::size& sz, int x /*= 0*/, int y /*= 0*/) {
  if (is_visible()) {
    sz.width = std::max(sz.width, x + right());
    sz.height = std::max(sz.height, y + bottom());
  }
}

void litehtml::element::get_redraw_box(litehtml::position& pos, int x /*= 0*/, int y /*= 0*/) {
  if (is_visible()) {
    int p_left = std::min(pos.left(), x + m_pos.left() - m_padding.left - m_borders.left);
    int p_right = std::max(pos.right(), x + m_pos.right() + m_padding.left + m_borders.left);
    int p_top = std::min(pos.top(), y + m_pos.top() - m_padding.top - m_borders.top);
    int p_bottom = std::max(pos.bottom(), y + m_pos.bottom() + m_padding.bottom + m_borders.bottom);

    pos.x = p_left;
    pos.y = p_top;
    pos.width = p_right - p_left;
    pos.height = p_bottom - p_top;
  }
}

int litehtml::element::calc_width(int defVal) const {
  css_length w = get_css_width();
  if (w.is_predefined()) {
    return defVal;
  }
  if (w.units() == css_units_percentage) {
    element::ptr el_parent = parent();
    if (!el_parent) {
      position client_pos;
      get_document()->container()->get_client_rect(client_pos);
      return w.calc_percent(client_pos.width) - content_margins_width();
    } else {
      int pw = el_parent->calc_width(defVal);
      if (is_body()) {
        pw -= content_margins_width();
      }
      return w.calc_percent(pw);
    }
  }
  return get_document()->cvt_units(w, get_font_size());
}

bool litehtml::element::is_ancestor(const ptr& el) const {
  element::ptr el_parent = parent();
  while (el_parent && el_parent != el) {
    el_parent = el_parent->parent();
  }
  if (el_parent) {
    return true;
  }
  return false;
}

int litehtml::element::get_inline_shift_left() {
  int ret = 0;
  element::ptr el_parent = parent();
  if (el_parent) {
    if (el_parent->get_display() == display_inline) {
      style_display disp = get_display();

      if (disp == display_inline_text || disp == display_inline_block) {
        element::ptr el = shared_from_this();
        while (el_parent && el_parent->get_display() == display_inline) {
          if (el_parent->is_first_child_inline(el)) {
            ret += el_parent->padding_left() + el_parent->border_left() + el_parent->margin_left();
          }
          el = el_parent;
          el_parent = el_parent->parent();
        }
      }
    }
  }

  return ret;
}

int litehtml::element::get_inline_shift_right() {
  int ret = 0;
  element::ptr el_parent = parent();
  if (el_parent) {
    if (el_parent->get_display() == display_inline) {
      style_display disp = get_display();

      if (disp == display_inline_text || disp == display_inline_block) {
        element::ptr el = shared_from_this();
        while (el_parent && el_parent->get_display() == display_inline) {
          if (el_parent->is_last_child_inline(el)) {
            ret += el_parent->padding_right() + el_parent->border_right() + el_parent->margin_right();
          }
          el = el_parent;
          el_parent = el_parent->parent();
        }
      }
    }
  }

  return ret;
}

void litehtml::element::apply_relative_shift(int parent_width) {
  css_offsets offsets;
  if (get_element_position(&offsets) == element_position_relative) {
    element::ptr parent_ptr = parent();
    if (!offsets.left.is_predefined()) {
      m_pos.x += offsets.left.calc_percent(parent_width);
    } else if (!offsets.right.is_predefined()) {
      m_pos.x -= offsets.right.calc_percent(parent_width);
    }
    if (!offsets.top.is_predefined()) {
      int h = 0;

      if (offsets.top.units() == css_units_percentage) {
        element::ptr el_parent = parent();
        if (el_parent) {
          el_parent->get_predefined_height(h);
        }
      }

      m_pos.y += offsets.top.calc_percent(h);
    } else if (!offsets.bottom.is_predefined()) {
      int h = 0;

      if (offsets.top.units() == css_units_percentage) {
        element::ptr el_parent = parent();
        if (el_parent) {
          el_parent->get_predefined_height(h);
        }
      }

      m_pos.y -= offsets.bottom.calc_percent(h);
    }
  }
}

void litehtml::element::calc_auto_margins(int parent_width) LITEHTML_EMPTY_FUNC const litehtml::background* litehtml::element::get_background(bool own_only) LITEHTML_RETURN_FUNC(nullptr) litehtml::element::ptr litehtml::element::get_element_by_point(int x, int y, int client_x, int client_y) LITEHTML_RETURN_FUNC(nullptr) litehtml::element::ptr litehtml::element::get_child_by_point(int x, int y, int client_x, int client_y, draw_flag flag, int zindex) LITEHTML_RETURN_FUNC(nullptr) void litehtml::element::get_line_left_right(int y, int def_right, int& ln_left, int& ln_right) LITEHTML_EMPTY_FUNC void litehtml::element::add_style(const litehtml::style& st) LITEHTML_EMPTY_FUNC void litehtml::element::select_all(const css_selector& selector, litehtml::elements_vector& res) LITEHTML_EMPTY_FUNC litehtml::elements_vector litehtml::element::select_all(const litehtml::css_selector& selector)
    LITEHTML_RETURN_FUNC(litehtml::elements_vector()) litehtml::elements_vector litehtml::element::select_all(const litehtml::tstring& selector) LITEHTML_RETURN_FUNC(litehtml::elements_vector()) litehtml::element::ptr litehtml::element::select_one(const css_selector& selector) LITEHTML_RETURN_FUNC(nullptr) litehtml::element::ptr litehtml::element::select_one(const tstring& selector) LITEHTML_RETURN_FUNC(nullptr) litehtml::element::ptr litehtml::element::find_adjacent_sibling(const element::ptr& el, const css_selector& selector, bool apply_pseudo /*= true*/, bool* is_pseudo /*= 0*/) LITEHTML_RETURN_FUNC(nullptr) litehtml::element::ptr litehtml::element::find_sibling(const element::ptr& el, const css_selector& selector, bool apply_pseudo /*= true*/, bool* is_pseudo /*= 0*/) LITEHTML_RETURN_FUNC(nullptr) bool litehtml::element::is_nth_last_child(const element::ptr& el, int num, int off, bool of_type) const
    LITEHTML_RETURN_FUNC(false) bool litehtml::element::is_nth_child(const element::ptr&, int num, int off, bool of_type) const LITEHTML_RETURN_FUNC(false) bool litehtml::element::is_only_child(const element::ptr& el, bool of_type) const LITEHTML_RETURN_FUNC(false) litehtml::overflow litehtml::element::get_overflow() const LITEHTML_RETURN_FUNC(overflow_visible) void litehtml::element::draw_children(uint_ptr hdc, int x, int y, const position* clip, draw_flag flag, int zindex) LITEHTML_EMPTY_FUNC void litehtml::element::draw_stacking_context(uint_ptr hdc, int x, int y, const position* clip, bool with_positioned) LITEHTML_EMPTY_FUNC void litehtml::element::render_positioned(render_type rt) LITEHTML_EMPTY_FUNC int litehtml::element::get_zindex() const LITEHTML_RETURN_FUNC(0) bool litehtml::element::fetch_positioned() LITEHTML_RETURN_FUNC(false) litehtml::visibility litehtml::element::get_visibility() const
    LITEHTML_RETURN_FUNC(visibility_visible) void litehtml::element::apply_vertical_align() LITEHTML_EMPTY_FUNC void litehtml::element::set_css_width(css_length& w) LITEHTML_EMPTY_FUNC litehtml::element::ptr litehtml::element::get_child(int idx) const LITEHTML_RETURN_FUNC(nullptr) size_t litehtml::element::get_children_count() const LITEHTML_RETURN_FUNC(0) void litehtml::element::calc_outlines(int parent_width) LITEHTML_EMPTY_FUNC litehtml::css_length litehtml::element::get_css_width() const LITEHTML_RETURN_FUNC(css_length()) litehtml::css_length litehtml::element::get_css_height() const LITEHTML_RETURN_FUNC(css_length()) litehtml::element_clear litehtml::element::get_clear() const LITEHTML_RETURN_FUNC(clear_none) litehtml::css_length litehtml::element::get_css_left() const LITEHTML_RETURN_FUNC(css_length()) litehtml::css_length litehtml::element::get_css_right() const LITEHTML_RETURN_FUNC(css_length()) litehtml::css_length litehtml::element::get_css_top() const
    LITEHTML_RETURN_FUNC(css_length()) litehtml::css_length litehtml::element::get_css_bottom() const LITEHTML_RETURN_FUNC(css_length()) litehtml::css_offsets litehtml::element::get_css_offsets() const LITEHTML_RETURN_FUNC(css_offsets()) litehtml::vertical_align litehtml::element::get_vertical_align() const LITEHTML_RETURN_FUNC(va_baseline) int litehtml::element::place_element(const ptr& el, int max_width) LITEHTML_RETURN_FUNC(0) int litehtml::element::render_inline(const ptr& container, int max_width) LITEHTML_RETURN_FUNC(0) void litehtml::element::add_positioned(const ptr& el) LITEHTML_EMPTY_FUNC int litehtml::element::find_next_line_top(int top, int width, int def_right) LITEHTML_RETURN_FUNC(0) litehtml::element_float litehtml::element::get_float() const LITEHTML_RETURN_FUNC(float_none) void litehtml::element::add_float(const ptr& el, int x, int y) LITEHTML_EMPTY_FUNC void litehtml::element::update_floats(int dy, const ptr& parent) LITEHTML_EMPTY_FUNC
    int litehtml::element::get_line_left(int y) LITEHTML_RETURN_FUNC(0) int litehtml::element::get_line_right(int y, int def_right) LITEHTML_RETURN_FUNC(def_right) int litehtml::element::get_left_floats_height() const LITEHTML_RETURN_FUNC(0) int litehtml::element::get_right_floats_height() const LITEHTML_RETURN_FUNC(0) int litehtml::element::get_floats_height(element_float el_float) const LITEHTML_RETURN_FUNC(0) bool litehtml::element::is_floats_holder() const LITEHTML_RETURN_FUNC(false) void litehtml::element::get_content_size(size& sz, int max_width) LITEHTML_EMPTY_FUNC void litehtml::element::init() LITEHTML_EMPTY_FUNC int litehtml::element::render(int x, int y, int max_width, bool second_pass) LITEHTML_RETURN_FUNC(0) bool litehtml::element::appendChild(const ptr& el) LITEHTML_RETURN_FUNC(false) bool litehtml::element::removeChild(const ptr& el)
        LITEHTML_RETURN_FUNC(false) void litehtml::element::clearRecursive() LITEHTML_EMPTY_FUNC const litehtml::tchar_t* litehtml::element::get_tagName() const LITEHTML_RETURN_FUNC(_t("")) void litehtml::element::set_tagName(const tchar_t* tag) LITEHTML_EMPTY_FUNC void litehtml::element::set_data(const tchar_t* data) LITEHTML_EMPTY_FUNC void litehtml::element::set_attr(const tchar_t* name, const tchar_t* val) LITEHTML_EMPTY_FUNC void litehtml::element::apply_stylesheet(const litehtml::css& stylesheet) LITEHTML_EMPTY_FUNC void litehtml::element::refresh_styles() LITEHTML_EMPTY_FUNC void litehtml::element::on_click() LITEHTML_EMPTY_FUNC void litehtml::element::init_font() LITEHTML_EMPTY_FUNC void litehtml::element::get_inline_boxes(position::vector& boxes) LITEHTML_EMPTY_FUNC void litehtml::element::parse_styles(bool is_reparse /*= false*/) LITEHTML_EMPTY_FUNC const litehtml::tchar_t* litehtml::element::get_attr(const tchar_t* name, const tchar_t* def /*= 0*/) const
    LITEHTML_RETURN_FUNC(def) bool litehtml::element::is_white_space() const LITEHTML_RETURN_FUNC(false) bool litehtml::element::is_body() const LITEHTML_RETURN_FUNC(false) bool litehtml::element::is_break() const LITEHTML_RETURN_FUNC(false) int litehtml::element::get_base_line() LITEHTML_RETURN_FUNC(0) bool litehtml::element::on_mouse_over() LITEHTML_RETURN_FUNC(false) bool litehtml::element::on_mouse_leave() LITEHTML_RETURN_FUNC(false) bool litehtml::element::on_lbutton_down() LITEHTML_RETURN_FUNC(false) bool litehtml::element::on_lbutton_up() LITEHTML_RETURN_FUNC(false) bool litehtml::element::find_styles_changes(position::vector& redraw_boxes, int x, int y) LITEHTML_RETURN_FUNC(false) const litehtml::tchar_t* litehtml::element::get_cursor() LITEHTML_RETURN_FUNC(nullptr) litehtml::white_space litehtml::element::get_white_space() const LITEHTML_RETURN_FUNC(white_space_normal) litehtml::style_display litehtml::element::get_display() const
    LITEHTML_RETURN_FUNC(display_none) bool litehtml::element::set_pseudo_class(const tchar_t* pclass, bool add) LITEHTML_RETURN_FUNC(false) bool litehtml::element::set_class(const tchar_t* pclass, bool add) LITEHTML_RETURN_FUNC(false) litehtml::element_position litehtml::element::get_element_position(css_offsets* offsets) const LITEHTML_RETURN_FUNC(element_position_static) bool litehtml::element::is_replaced() const LITEHTML_RETURN_FUNC(false) int litehtml::element::line_height() const LITEHTML_RETURN_FUNC(0) void litehtml::element::draw(uint_ptr hdc, int x, int y, const position* clip) LITEHTML_EMPTY_FUNC void litehtml::element::draw_background(uint_ptr hdc, int x, int y, const position* clip) LITEHTML_EMPTY_FUNC const litehtml::tchar_t* litehtml::element::get_style_property(const tchar_t* name, bool inherited, const tchar_t* def /*= 0*/) LITEHTML_RETURN_FUNC(nullptr) litehtml::uint_ptr litehtml::element::get_font(font_metrics* fm /*= 0*/)
        LITEHTML_RETURN_FUNC(0) int litehtml::element::get_font_size() const LITEHTML_RETURN_FUNC(0) void litehtml::element::get_text(tstring& text) LITEHTML_EMPTY_FUNC void litehtml::element::parse_attributes() LITEHTML_EMPTY_FUNC int litehtml::element::select(const css_selector& selector, bool apply_pseudo) LITEHTML_RETURN_FUNC(select_no_match) int litehtml::element::select(const css_element_selector& selector, bool apply_pseudo /*= true*/) LITEHTML_RETURN_FUNC(select_no_match) litehtml::element::ptr litehtml::element::find_ancestor(const css_selector& selector, bool apply_pseudo, bool* is_pseudo) LITEHTML_RETURN_FUNC(nullptr) bool litehtml::element::is_first_child_inline(const element::ptr& el) const LITEHTML_RETURN_FUNC(false) bool litehtml::element::is_last_child_inline(const element::ptr& el) LITEHTML_RETURN_FUNC(false) bool litehtml::element::have_inline_child() const LITEHTML_RETURN_FUNC(false)
