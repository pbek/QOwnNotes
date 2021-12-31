#include <gtest/gtest.h>

#include "litehtml.h"
#include "test/container_test.h"

using namespace litehtml;

TEST(DocumentTest, AddFont) {
  container_test container;
  litehtml::document::ptr doc = std::make_shared<litehtml::document>(&container, nullptr);
  font_metrics fm;
  doc->get_font(nullptr, 0, _t("normal"), _t("normal"), _t(""), &fm);
  doc->get_font(_t("inherit"), 0, _t("normal"), _t("normal"), _t(""), &fm);
  doc->get_font(_t("Arial"), 0, _t("bold"), _t("normal"), _t("underline"), &fm);
  doc->get_font(_t("Arial"), 0, _t("bold"), _t("normal"), _t("line-through"), &fm);
  doc->get_font(_t("Arial"), 0, _t("bold"), _t("normal"), _t("overline"), &fm);
}

TEST(DocumentTest, Render) {
  context ctx;
  container_test container;
  litehtml::document::ptr doc = document::createFromString(_t("<html>Body</html>"), &container, &ctx);
  doc->render(100, render_fixed_only);
  doc->render(100, render_no_fixed);
  doc->render(100, render_all);
}

TEST(DocumentTest, Draw) {
  context ctx;
  container_test container;
  litehtml::document::ptr doc = document::createFromString(_t("<html>Body</html>"), &container, &ctx);
  position pos(0, 0, 100, 100);
  doc->draw((uint_ptr)0, 0, 0, &pos);
}

TEST(DocumentTest, CvtUnits) {
  container_test container;
  litehtml::document::ptr doc = std::make_shared<litehtml::document>(&container, nullptr);
  bool is_percent;
  doc->cvt_units(_t(""), 10, &is_percent);
  css_length c;
  c.fromString(_t("10%")), doc->cvt_units(c, 10, 100);
  c.fromString(_t("10em")), doc->cvt_units(c, 10, 100);
  c.fromString(_t("10pt")), doc->cvt_units(c, 10, 100);
  c.fromString(_t("10in")), doc->cvt_units(c, 10, 100);
  c.fromString(_t("10cm")), doc->cvt_units(c, 10, 100);
  c.fromString(_t("10mm")), doc->cvt_units(c, 10, 100);
  c.fromString(_t("10vm")), doc->cvt_units(c, 10, 100);
  c.fromString(_t("10vh")), doc->cvt_units(c, 10, 100);
  c.fromString(_t("10vmin")), doc->cvt_units(c, 10, 100);
  c.fromString(_t("10vmax")), doc->cvt_units(c, 10, 100);
  c.fromString(_t("10")), doc->cvt_units(c, 10, 100);
}

TEST(DocumentTest, MouseEvents) {
  container_test container;
  litehtml::document::ptr doc = std::make_shared<litehtml::document>(&container, nullptr);
  position::vector redraw_boxes;
  doc->on_mouse_over(0, 0, 0, 0, redraw_boxes);
  doc->on_lbutton_down(0, 0, 0, 0, redraw_boxes);
  doc->on_lbutton_up(0, 0, 0, 0, redraw_boxes);
  doc->on_mouse_leave(redraw_boxes);
}

TEST(DocumentTest, CreateElement) {
  container_test container;
  litehtml::document::ptr doc = std::make_shared<litehtml::document>(&container, nullptr);
  string_map map;
  doc->create_element(_t("container"), map);
  doc->create_element(_t("br"), map);
  doc->create_element(_t("p"), map);
  doc->create_element(_t("img"), map);
  doc->create_element(_t("table"), map);
  doc->create_element(_t("td"), map);
  doc->create_element(_t("th"), map);
  doc->create_element(_t("link"), map);
  doc->create_element(_t("title"), map);
  doc->create_element(_t("a"), map);
  doc->create_element(_t("tr"), map);
  doc->create_element(_t("style"), map);
  doc->create_element(_t("base"), map);
  doc->create_element(_t("div"), map);
  doc->create_element(_t("script"), map);
  doc->create_element(_t("font"), map);
  doc->create_element(_t("tag"), map);
}

TEST(DocumentTest, DeviceChange) {
  container_test container;
  litehtml::document::ptr doc = std::make_shared<litehtml::document>(&container, nullptr);
  doc->media_changed();
  doc->lang_changed();
}

TEST(DocumentTest, Parse) {
  context ctx;
  container_test container;
  document::createFromString(_t(""), &container, &ctx);
}
