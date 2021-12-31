#ifndef LH_CSS_OFFSETS_H
#define LH_CSS_OFFSETS_H

#include "css_length.h"

namespace litehtml
{
	struct css_offsets
	{
		css_length	left;
		css_length	top;
		css_length	right;
		css_length	bottom;

		css_offsets() = default;

		css_offsets(const css_offsets& val)
		{
			left	= val.left;
			top		= val.top;
			right	= val.right;
			bottom	= val.bottom;
		}

		css_offsets& operator=(const css_offsets& val)
		{
			left	= val.left;
			top		= val.top;
			right	= val.right;
			bottom	= val.bottom;
			return *this;
		}
	};
}

#endif  // LH_CSS_OFFSETS_H
