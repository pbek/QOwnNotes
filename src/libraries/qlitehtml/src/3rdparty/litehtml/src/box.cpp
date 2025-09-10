#include "box.h"

#include "html.h"
#include "html_tag.h"

litehtml::box_type litehtml::block_box::get_type() const { return box_block; }

int litehtml::block_box::height() const { return m_element->height(); }

int litehtml::block_box::width() const { return m_element->width(); }

void litehtml::block_box::add_element(const element::ptr& el) {
  m_element = el;
  el->m_box = this;
}

void litehtml::block_box::finish(bool last_box) {
  if (!m_element) return;
  m_element->apply_relative_shift(m_box_right - m_box_left);
}

bool litehtml::block_box::can_hold(const element::ptr& el, white_space ws) const {
  if (m_element || el->is_inline_box()) {
    return false;
  }
  return true;
}

bool litehtml::block_box::is_empty() const {
  if (m_element) {
    return false;
  }
  return true;
}

int litehtml::block_box::baseline() const {
  if (m_element) {
    return m_element->get_base_line();
  }
  return 0;
}

void litehtml::block_box::get_elements(elements_vector& els) { els.push_back(m_element); }

int litehtml::block_box::top_margin() const {
  if (m_element && m_element->collapse_top_margin()) {
    return m_element->m_margins.top;
  }
  return 0;
}

int litehtml::block_box::bottom_margin() const {
  if (m_element && m_element->collapse_bottom_margin()) {
    return m_element->m_margins.bottom;
  }
  return 0;
}

void litehtml::block_box::y_shift(int shift) {
  m_box_top += shift;
  if (m_element) {
    m_element->m_pos.y += shift;
  }
}

void litehtml::block_box::new_width(int left, int right, elements_vector& els) {}

//////////////////////////////////////////////////////////////////////////

litehtml::box_type litehtml::line_box::get_type() const { return box_line; }

int litehtml::line_box::height() const { return m_height; }

int litehtml::line_box::width() const { return m_width; }

void litehtml::line_box::add_element(const element::ptr& el) {
  el->m_skip = false;
  el->m_box = nullptr;
  bool add = true;
  if ((m_items.empty() && el->is_white_space()) || el->is_break()) {
    el->m_skip = true;
  } else if (el->is_white_space()) {
    if (have_last_space()) {
      add = false;
      el->m_skip = true;
    }
  }

  if (add) {
    el->m_box = this;
    m_items.push_back(el);

    if (!el->m_skip) {
      int el_shift_left = el->get_inline_shift_left();
      int el_shift_right = el->get_inline_shift_right();

      el->m_pos.x = m_box_left + m_width + el_shift_left + el->content_margins_left();
      el->m_pos.y = m_box_top + el->content_margins_top();
      m_width += el->width() + el_shift_left + el_shift_right;
    }
  }
}

void litehtml::line_box::finish(bool last_box) {
  if (is_empty() || (!is_empty() && last_box && is_break_only())) {
    m_height = 0;
    return;
  }

  for (auto i = m_items.rbegin(); i != m_items.rend(); i++) {
    if ((*i)->is_white_space() || (*i)->is_break()) {
      if (!(*i)->m_skip) {
        (*i)->m_skip = true;
        m_width -= (*i)->width();
      }
    } else {
      break;
    }
  }

  int base_line = m_font_metrics.base_line();
  int line_height = m_line_height;

  int add_x = 0;
  switch (m_text_align) {
    case text_align_right:
      if (m_width < (m_box_right - m_box_left)) {
        add_x = (m_box_right - m_box_left) - m_width;
      }
      break;
    case text_align_center:
      if (m_width < (m_box_right - m_box_left)) {
        add_x = ((m_box_right - m_box_left) - m_width) / 2;
      }
      break;
    default:
      add_x = 0;
  }

  m_height = 0;
  // find line box baseline and line-height
  for (const auto& el : m_items) {
    if (el->get_display() == display_inline_text) {
      font_metrics fm;
      el->get_font(&fm);
      base_line = std::max(base_line, fm.base_line());
      line_height = std::max(line_height, el->line_height());
      m_height = std::max(m_height, fm.height);
    }
    el->m_pos.x += add_x;
  }

  if (m_height) {
    base_line += (line_height - m_height) / 2;
  }

  m_height = line_height;

  int y1 = 0;
  int y2 = m_height;

  for (const auto& el : m_items) {
    if (el->get_display() == display_inline_text) {
      font_metrics fm;
      el->get_font(&fm);
      el->m_pos.y = m_height - base_line - fm.ascent;
    } else {
      switch (el->get_vertical_align()) {
        case va_super:
        case va_sub:
        case va_baseline:
          el->m_pos.y = m_height - base_line - el->height() + el->get_base_line() + el->content_margins_top();
          break;
        case va_top:
          el->m_pos.y = y1 + el->content_margins_top();
          break;
        case va_text_top:
          el->m_pos.y = m_height - base_line - m_font_metrics.ascent + el->content_margins_top();
          break;
        case va_middle:
          el->m_pos.y = m_height - base_line - m_font_metrics.x_height / 2 - el->height() / 2 + el->content_margins_top();
          break;
        case va_bottom:
          el->m_pos.y = y2 - el->height() + el->content_margins_top();
          break;
        case va_text_bottom:
          el->m_pos.y = m_height - base_line + m_font_metrics.descent - el->height() + el->content_margins_top();
          break;
      }
      y1 = std::min(y1, el->top());
      y2 = std::max(y2, el->bottom());
    }
  }

  for (const auto& el : m_items) {
    el->m_pos.y -= y1;
    el->m_pos.y += m_box_top;
    if (el->get_display() != display_inline_text) {
      switch (el->get_vertical_align()) {
        case va_top:
          el->m_pos.y = m_box_top + el->content_margins_top();
          break;
        case va_bottom:
          el->m_pos.y = m_box_top + (y2 - y1) - el->height() + el->content_margins_top();
          break;
        case va_baseline:
          // TODO: process vertical align "baseline"
          break;
        case va_middle:
          // TODO: process vertical align "middle"
          break;
        case va_sub:
          // TODO: process vertical align "sub"
          break;
        case va_super:
          // TODO: process vertical align "super"
          break;
        case va_text_bottom:
          // TODO: process vertical align "text-bottom"
          break;
        case va_text_top:
          // TODO: process vertical align "text-top"
          break;
      }
    }

    el->apply_relative_shift(m_box_right - m_box_left);
  }
  m_height = y2 - y1;
  m_baseline = (base_line - y1) - (m_height - line_height);
}

bool litehtml::line_box::can_hold(const element::ptr& el, white_space ws) const {
  if (!el->is_inline_box()) return false;

  if (el->is_break()) {
    return false;
  }

  if (ws == white_space_nowrap || ws == white_space_pre) {
    return true;
  }

  if (m_box_left + m_width + el->width() + el->get_inline_shift_left() + el->get_inline_shift_right() > m_box_right) {
    return false;
  }

  return true;
}

bool litehtml::line_box::have_last_space() const {
  bool ret = false;
  for (auto i = m_items.rbegin(); i != m_items.rend() && !ret; i++) {
    if ((*i)->is_white_space() || (*i)->is_break()) {
      ret = true;
    } else {
      break;
    }
  }
  return ret;
}

bool litehtml::line_box::is_empty() const {
  if (m_items.empty()) return true;
  for (auto i = m_items.rbegin(); i != m_items.rend(); i++) {
    if (!(*i)->m_skip || (*i)->is_break()) {
      return false;
    }
  }
  return true;
}

int litehtml::line_box::baseline() const { return m_baseline; }

void litehtml::line_box::get_elements(elements_vector& els) { els.insert(els.begin(), m_items.begin(), m_items.end()); }

int litehtml::line_box::top_margin() const { return 0; }

int litehtml::line_box::bottom_margin() const { return 0; }

void litehtml::line_box::y_shift(int shift) {
  m_box_top += shift;
  for (auto& el : m_items) {
    el->m_pos.y += shift;
  }
}

bool litehtml::line_box::is_break_only() const {
  if (m_items.empty()) return true;

  if (m_items.front()->is_break()) {
    for (auto& el : m_items) {
      if (!el->m_skip) {
        return false;
      }
    }
    return true;
  }
  return false;
}

void litehtml::line_box::new_width(int left, int right, elements_vector& els) {
  int add = left - m_box_left;
  if (add) {
    m_box_left = left;
    m_box_right = right;
    m_width = 0;
    auto remove_begin = m_items.end();
    for (auto i = m_items.begin() + 1; i != m_items.end(); i++) {
      element::ptr el = (*i);

      if (!el->m_skip) {
        if (m_box_left + m_width + el->width() + el->get_inline_shift_right() + el->get_inline_shift_left() > m_box_right) {
          remove_begin = i;
          break;
        } else {
          el->m_pos.x += add;
          m_width += el->width() + el->get_inline_shift_right() + el->get_inline_shift_left();
        }
      }
    }
    if (remove_begin != m_items.end()) {
      els.insert(els.begin(), remove_begin, m_items.end());
      m_items.erase(remove_begin, m_items.end());

      for (const auto& el : els) {
        el->m_box = nullptr;
      }
    }
  }
}
