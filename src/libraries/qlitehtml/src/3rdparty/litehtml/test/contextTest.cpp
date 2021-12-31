#include <gtest/gtest.h>

#include "litehtml.h"

using namespace litehtml;

extern const litehtml::tchar_t master_css[] =
{
#include "master.css.inc"
, 0
};


TEST(ContextTest, LoadMasterStylesheet)
{
    context ctx;
    ctx.load_master_stylesheet(master_css);
}
