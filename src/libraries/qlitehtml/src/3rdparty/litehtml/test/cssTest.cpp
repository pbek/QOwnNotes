#include <gtest/gtest.h>

#include <assert.h>
#include "litehtml.h"
#include "test/container_test.h"
using namespace litehtml;

TEST(CSSTest, Parse) {
  container_test container;
  litehtml::document::ptr doc = std::make_shared<litehtml::document>(&container, nullptr);
  media_query_list::ptr media = media_query_list::ptr();
  css c;
  c.parse_stylesheet(_t("/*Comment*/"), nullptr, doc, nullptr);
  c.parse_stylesheet(_t("html { display: none }"), nullptr, doc, nullptr);
  // https://www.w3schools.com/cssref/pr_import_rule.asp
  c.parse_stylesheet(_t("@import \"navigation.css\"; /* Using a string */"), nullptr, doc, nullptr);
  c.parse_stylesheet(_t("@import url(\"navigation.css\"); /* Using a url */"), nullptr, doc, nullptr);
  c.parse_stylesheet(_t("@import \"navigation.css\""), nullptr, doc, nullptr);
  c.parse_stylesheet(_t("@import \"printstyle.css\" print;"), nullptr, doc, nullptr);
  c.parse_stylesheet(_t("@import \"mobstyle.css\" screen and (max-width: 768px);"), nullptr, doc, nullptr);
  // https://www.w3schools.com/cssref/css3_pr_mediaquery.asp
  c.parse_stylesheet(_t("@media only screen and (max-width: 600px) { body { background-color: lightblue; } }"), nullptr, doc, nullptr);
}

TEST(CSSTest, Url) {
  tstring url;

  css::parse_css_url(_t(""), url);
  EXPECT_TRUE(url.empty());

  css::parse_css_url(_t("value"), url);
  EXPECT_TRUE(url.empty());

  css::parse_css_url(_t("url()"), url);
  EXPECT_TRUE(url.empty());

  css::parse_css_url(_t("url(value)"), url);
  EXPECT_TRUE(!t_strcmp(url.c_str(), _t("value")));

  css::parse_css_url(_t("url('value')"), url);
  EXPECT_TRUE(!t_strcmp(url.c_str(), _t("value")));

  css::parse_css_url(_t("url(\"value\")"), url);
  EXPECT_TRUE(!t_strcmp(url.c_str(), _t("value")));
}

TEST(CSSTest, LengthParse) {
  css_length length;

  length.fromString(_t("calc(todo)"));
  assert(length.is_predefined() == true);
  assert(length.predef() == 0);
  assert(length.val() == 0);
  assert(length.units() == css_units_none);

  length.fromString(_t("top"), _t("top;bottom"), -1);
  assert(length.is_predefined() == true);
  assert(length.predef() == 0);
  assert(length.val() == 0);
  assert(length.units() == css_units_none);

  length.fromString(_t("bottom"), _t("top;bottom"), -1);
  assert(length.is_predefined() == true);
  assert(length.predef() == 1);
  assert(length.val() == 0);
  assert(length.units() == css_units_none);

  length.fromString(_t("bad"), _t("top;bottom"), -1);
  assert(length.is_predefined() == true);
  assert(length.predef() == -1);
  assert(length.val() == 0);
  assert(length.units() == css_units_none);

  length.fromString(_t("123"), _t("top;bottom"), -1);
  assert(length.is_predefined() == false);
  assert(length.predef() == 0);
  assert(length.val() == 123);
  assert(length.units() == css_units_none);

  length.fromString(_t("123px"), _t("top;bottom"), -1);
  assert(length.is_predefined() == false);
  assert(length.predef() == 0);
  assert(length.val() == 123);
  assert(length.units() == css_units_px);
}

TEST(CSSTest, ElementSelectorParse) {
  css_element_selector selector;
  // https://www.w3schools.com/cssref/css_selectors.asp
  selector.parse(_t(".class"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("class")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("class")));

  selector.parse(_t(".class1.class2"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 2);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("class1")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[1].val.c_str(), _t("class2")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("class")));

  selector.parse(_t("#id"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("id")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("id")));

  selector.parse(_t("*"));
  EXPECT_TRUE(!t_strcmp(selector.m_tag.c_str(), _t("*")));
  EXPECT_TRUE(selector.m_attrs.empty());

  selector.parse(_t("element"));
  EXPECT_TRUE(!t_strcmp(selector.m_tag.c_str(), _t("element")));
  EXPECT_TRUE(selector.m_attrs.empty());

  selector.parse(_t("[attribute]"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("attribute")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_exists);

  selector.parse(_t("[attribute=value]"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("value")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("attribute")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_equal);

  selector.parse(_t("[attribute~=value]"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("value")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("attribute")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_contain_str);

  selector.parse(_t("[attribute|=value]"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("value")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("attribute")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_start_str);

  selector.parse(_t("[attribute^=value]"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("value")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("attribute")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_start_str);

  selector.parse(_t("[attribute$=value]"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("value")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("attribute")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_end_str);

  selector.parse(_t("[attribute*=value]"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("value")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("attribute")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_contain_str);

  selector.parse(_t(":active"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("active")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t("::after"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("after")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo-el")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_element);

  selector.parse(_t("::before"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("before")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo-el")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_element);

  selector.parse(_t(":checked"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("checked")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":default"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("default")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":disabled"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("disabled")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":empty"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("empty")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":enabled"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("enabled")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":first-child"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("first-child")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t("::first-letter"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("first-letter")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo-el")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_element);

  selector.parse(_t("::first-line"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("first-line")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo-el")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_element);

  selector.parse(_t(":first-of-type"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("first-of-type")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":focus"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("focus")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":hover"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("hover")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":in-range"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("in-range")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":indeterminate"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("indeterminate")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":invalid"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("invalid")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":lang(language)"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("lang(language)")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":last-child"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("last-child")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":last-of-type"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("last-of-type")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":link"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("link")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":not(selector)"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("not(selector)")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":nth-child(n)"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("nth-child(n)")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":nth-last-child(n)"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("nth-last-child(n)")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":nth-last-of-type(n)"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("nth-last-of-type(n)")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":nth-of-type(n)"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("nth-of-type(n)")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":only-of-type"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("only-of-type")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":only-child"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("only-child")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":optional"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("optional")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":out-of-range"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("out-of-range")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t("::placeholder"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("placeholder")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo-el")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_element);

  selector.parse(_t(":read-only"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("read-only")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":read-write"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("read-write")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":required"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("required")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":root"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("root")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t("::selection"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("selection")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo-el")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_element);

  selector.parse(_t(":target"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("target")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":valid"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("valid")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  selector.parse(_t(":visited"));
  EXPECT_TRUE(selector.m_tag.empty());
  EXPECT_TRUE(selector.m_attrs.size() == 1);
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].val.c_str(), _t("visited")));
  EXPECT_TRUE(!t_strcmp(selector.m_attrs[0].attribute.c_str(), _t("pseudo")));
  EXPECT_TRUE(selector.m_attrs[0].condition == select_pseudo_class);

  // other
  selector.parse(_t("tag:psudo#anchor"));
  EXPECT_TRUE(!t_strcmp(selector.m_tag.c_str(), _t("tag")));
  EXPECT_TRUE(selector.m_attrs.size() == 2);
}

TEST(CSSTest, DISABLED_SelectorParse) {
  css_selector selector(nullptr);
  // https://www.w3schools.com/cssref/css_selectors.asp
  assert(!selector.parse(_t("")));
  EXPECT_TRUE(selector.parse(_t("element")));
  EXPECT_TRUE(selector.m_combinator == combinator_descendant);
  EXPECT_TRUE(!t_strcmp(selector.m_right.m_tag.c_str(), _t("element")));
  EXPECT_TRUE(selector.m_right.m_attrs.empty());
  EXPECT_TRUE(selector.m_left == nullptr);

  // assert(selector.parse(_t("element,element")));
  // EXPECT_TRUE(selector.m_combinator == combinator_descendant);
  // EXPECT_TRUE(selector.m_right.m_tag.c_str(), _t("element"));
  // EXPECT_TRUE(selector.m_right.m_attrs.empty());

  EXPECT_TRUE(selector.parse(_t(".class1 .class2")));
  EXPECT_TRUE(selector.m_combinator == combinator_descendant);
  EXPECT_TRUE(selector.m_right.m_tag.empty());
  EXPECT_TRUE(selector.m_right.m_attrs.size() == 1);
  EXPECT_TRUE(selector.m_left->m_right.m_attrs.size() == 1);

  assert(selector.parse(_t("element element")));
  EXPECT_TRUE(selector.m_combinator == combinator_descendant);
  EXPECT_TRUE(!t_strcmp(selector.m_right.m_tag.c_str(), _t("element")));
  EXPECT_TRUE(selector.m_right.m_attrs.empty());
  EXPECT_TRUE(!t_strcmp(selector.m_left->m_right.m_tag.c_str(), _t("element")));

  assert(selector.parse(_t("element>element")));
  EXPECT_TRUE(selector.m_combinator == combinator_child);
  EXPECT_TRUE(!t_strcmp(selector.m_right.m_tag.c_str(), _t("element")));
  EXPECT_TRUE(selector.m_right.m_attrs.empty());
  EXPECT_TRUE(!t_strcmp(selector.m_left->m_right.m_tag.c_str(), _t("element")));

  assert(selector.parse(_t("element+element")));
  EXPECT_TRUE(selector.m_combinator == combinator_adjacent_sibling);
  EXPECT_TRUE(!t_strcmp(selector.m_right.m_tag.c_str(), _t("element")));
  EXPECT_TRUE(selector.m_right.m_attrs.empty());
  EXPECT_TRUE(!t_strcmp(selector.m_left->m_right.m_tag.c_str(), _t("element")));

  assert(selector.parse(_t("element1~element2")));
  EXPECT_TRUE(selector.m_combinator == combinator_general_sibling);
  EXPECT_TRUE(!t_strcmp(selector.m_right.m_tag.c_str(), _t("element2")));
  EXPECT_TRUE(selector.m_right.m_attrs.empty());
  EXPECT_TRUE(!t_strcmp(selector.m_left->m_right.m_tag.c_str(), _t("element1")));
}

TEST(CSSTest, StyleAdd) {
  style style;
  style.add(_t("border: 5px solid red; background-image: value"), _t("base"));
  style.add(_t("border: 5px solid red!important; background-image: value"), _t("base"));
}

TEST(CSSTest, StyleAddProperty) {
  style style;
  style.add_property(_t("background-image"), _t("value"), _t("base"), false);
  style.add_property(_t("border-spacing"), _t("1"), nullptr, false);
  style.add_property(_t("border-spacing"), _t("1 2"), nullptr, false);
  style.add_property(_t("border"), _t("5px solid red"), nullptr, false);
  style.add_property(_t("border-left"), _t("5px solid red"), nullptr, false);
  style.add_property(_t("border-right"), _t("5px solid red"), nullptr, false);
  style.add_property(_t("border-top"), _t("5px solid red"), nullptr, false);
  style.add_property(_t("border-bottom"), _t("5px solid red"), nullptr, false);
  style.add_property(_t("border-bottom-left-radius"), _t("1"), nullptr, false);
  style.add_property(_t("border-bottom-left-radius"), _t("1 2"), nullptr, false);
  style.add_property(_t("border-bottom-right-radius"), _t("1"), nullptr, false);
  style.add_property(_t("border-bottom-right-radius"), _t("1 2"), nullptr, false);
  style.add_property(_t("border-top-right-radius"), _t("1"), nullptr, false);
  style.add_property(_t("border-top-right-radius"), _t("1 2"), nullptr, false);
  style.add_property(_t("border-top-left-radius"), _t("1"), nullptr, false);
  style.add_property(_t("border-top-left-radius"), _t("1 2"), nullptr, false);
  style.add_property(_t("border-radius"), _t("1"), nullptr, false);
  style.add_property(_t("border-radius"), _t("1 2"), nullptr, false);
  style.add_property(_t("border-radius-x"), _t("1"), nullptr, false);
  style.add_property(_t("border-radius-x"), _t("1 2"), nullptr, false);
  style.add_property(_t("border-radius-x"), _t("1 2 3"), nullptr, false);
  style.add_property(_t("border-radius-x"), _t("1 2 3 4"), nullptr, false);
  style.add_property(_t("border-radius-y"), _t("1"), nullptr, false);
  style.add_property(_t("border-radius-y"), _t("1 2"), nullptr, false);
  style.add_property(_t("border-radius-y"), _t("1 2 3"), nullptr, false);
  style.add_property(_t("border-radius-y"), _t("1 2 3 4"), nullptr, false);
  style.add_property(_t("list-style-image"), _t("value"), _t("base"), false);
  style.add_property(_t("background"), _t("url(value)"), _t("base"), false);
  style.add_property(_t("background"), _t("repeat"), nullptr, false);
  style.add_property(_t("background"), _t("fixed"), nullptr, false);
  style.add_property(_t("background"), _t("border-box"), nullptr, false);
  style.add_property(_t("background"), _t("border-box border-box"), nullptr, false);
  style.add_property(_t("background"), _t("left"), nullptr, false);
  style.add_property(_t("background"), _t("1"), nullptr, false);
  style.add_property(_t("background"), _t("-1"), nullptr, false);
  style.add_property(_t("background"), _t("-1"), nullptr, false);
  style.add_property(_t("background"), _t("+1"), nullptr, false);
  style.add_property(_t("background"), _t("left 1"), nullptr, false);
  style.add_property(_t("background"), _t("red"), nullptr, false);
  style.add_property(_t("margin"), _t("1"), nullptr, false);
  style.add_property(_t("margin"), _t("1 2"), nullptr, false);
  style.add_property(_t("margin"), _t("1 2 3"), nullptr, false);
  style.add_property(_t("margin"), _t("1 2 3 4"), nullptr, false);
  style.add_property(_t("padding"), _t("1"), nullptr, false);
  style.add_property(_t("padding"), _t("1 2"), nullptr, false);
  style.add_property(_t("padding"), _t("1 2 3"), nullptr, false);
  style.add_property(_t("padding"), _t("1 2 3 4"), nullptr, false);
  style.add_property(_t("border-left"), _t("TBD"), nullptr, false);
  style.add_property(_t("border-left"), _t("TBD"), nullptr, false);
  style.add_property(_t("border-left"), _t("TBD"), nullptr, false);
  style.add_property(_t("border-left"), _t("TBD"), nullptr, false);
  style.add_property(_t("border-right"), _t("TBD"), nullptr, false);
  style.add_property(_t("border-right"), _t("TBD"), nullptr, false);
  style.add_property(_t("border-right"), _t("TBD"), nullptr, false);
  style.add_property(_t("border-right"), _t("TBD"), nullptr, false);
  style.add_property(_t("border-top"), _t("TBD"), nullptr, false);
  style.add_property(_t("border-top"), _t("TBD"), nullptr, false);
  style.add_property(_t("border-top"), _t("TBD"), nullptr, false);
  style.add_property(_t("border-top"), _t("TBD"), nullptr, false);
  style.add_property(_t("border-bottom"), _t("TBD"), nullptr, false);
  style.add_property(_t("border-bottom"), _t("TBD"), nullptr, false);
  style.add_property(_t("border-bottom"), _t("TBD"), nullptr, false);
  style.add_property(_t("border-bottom"), _t("TBD"), nullptr, false);
  style.add_property(_t("border-width"), _t("1"), nullptr, false);
  style.add_property(_t("border-width"), _t("1 2"), nullptr, false);
  style.add_property(_t("border-width"), _t("1 2 3"), nullptr, false);
  style.add_property(_t("border-width"), _t("1 2 3 4"), nullptr, false);
  style.add_property(_t("border-style"), _t("1"), nullptr, false);
  style.add_property(_t("border-style"), _t("1 2"), nullptr, false);
  style.add_property(_t("border-style"), _t("1 2 3"), nullptr, false);
  style.add_property(_t("border-style"), _t("1 2 3 4"), nullptr, false);
  style.add_property(_t("border-color"), _t("1"), nullptr, false);
  style.add_property(_t("border-color"), _t("1 2"), nullptr, false);
  style.add_property(_t("border-color"), _t("1 2 3"), nullptr, false);
  style.add_property(_t("border-color"), _t("1 2 3 4"), nullptr, false);
  style.add_property(_t("font"), _t("TBD"), nullptr, false);
  style.add_property(_t("font"), _t("TBD"), nullptr, false);
  style.add_property(_t("font"), _t("TBD"), nullptr, false);
  style.add_property(_t("font"), _t("TBD"), nullptr, false);
  style.add_property(_t("unknown"), _t("value"), nullptr, false);
}
