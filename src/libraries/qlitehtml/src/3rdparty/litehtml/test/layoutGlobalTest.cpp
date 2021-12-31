#include <gtest/gtest.h>

#include "litehtml.h"
#include "test/container_test.h"

using namespace litehtml;

TEST(LayoutGlobal, Smoke) {
  context ctx;
  container_test container;
  litehtml::document::ptr doc = document::createFromString(_t("<html>Body</html>"), &container, &ctx);
  doc->render(50, render_all);
}
