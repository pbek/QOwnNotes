#include "html.h"
#include "borders.h"

litehtml::string litehtml::css_border::to_string() const
{
	return width.to_string() + "/" + index_value(style, border_style_strings) + "/" + color.to_string();
}
