#ifndef LH_BORDERS_H
#define LH_BORDERS_H

#include "css_length.h"
#include "types.h"
#include "web_color.h"

namespace litehtml
{
	struct css_border
	{
		css_length	 width;
		border_style style;
		web_color	 color;

		css_border() { style = border_style_none; }

		css_border(const css_border& val)
		{
			width = val.width;
			style = val.style;
			color = val.color;
		}

		css_border& operator=(const css_border& val) = default;

		string to_string() const;
	};

	struct border
	{
		pixel_t		 width;
		border_style style;
		web_color	 color;

		border()
		{
			width = 0;
			style = border_style_none;
		}
		border(const border& val)
		{
			width = val.width;
			style = val.style;
			color = val.color;
		}
		border(const css_border& val)
		{
			width = (pixel_t) val.width.val();
			style = val.style;
			color = val.color;
		}
		border& operator=(const border& val) = default;
		border& operator=(const css_border& val)
		{
			width = (pixel_t) val.width.val();
			style = val.style;
			color = val.color;
			return *this;
		}
	};

	struct border_radiuses
	{
		pixel_t top_left_x;
		pixel_t top_left_y;

		pixel_t top_right_x;
		pixel_t top_right_y;

		pixel_t bottom_right_x;
		pixel_t bottom_right_y;

		pixel_t bottom_left_x;
		pixel_t bottom_left_y;

		border_radiuses()
		{
			top_left_x	   = 0;
			top_left_y	   = 0;
			top_right_x	   = 0;
			top_right_y	   = 0;
			bottom_right_x = 0;
			bottom_right_y = 0;
			bottom_left_x  = 0;
			bottom_left_y  = 0;
		}
		border_radiuses(const border_radiuses& val)
		{
			top_left_x	   = val.top_left_x;
			top_left_y	   = val.top_left_y;
			top_right_x	   = val.top_right_x;
			top_right_y	   = val.top_right_y;
			bottom_right_x = val.bottom_right_x;
			bottom_right_y = val.bottom_right_y;
			bottom_left_x  = val.bottom_left_x;
			bottom_left_y  = val.bottom_left_y;
		}
		border_radiuses& operator=(const border_radiuses& val) = default;
		void			 operator+=(const margins& mg)
		{
			top_left_x	   += mg.left;
			top_left_y	   += mg.top;
			top_right_x	   += mg.right;
			top_right_y	   += mg.top;
			bottom_right_x += mg.right;
			bottom_right_y += mg.bottom;
			bottom_left_x  += mg.left;
			bottom_left_y  += mg.bottom;
			fix_values();
		}
		void operator-=(const margins& mg)
		{
			top_left_x	   -= mg.left;
			top_left_y	   -= mg.top;
			top_right_x	   -= mg.right;
			top_right_y	   -= mg.top;
			bottom_right_x -= mg.right;
			bottom_right_y -= mg.bottom;
			bottom_left_x  -= mg.left;
			bottom_left_y  -= mg.bottom;
			fix_values();
		}
		void fix_values()
		{
			if(top_left_x < 0)
				top_left_x = 0;
			if(top_left_y < 0)
				top_left_y = 0;
			if(top_right_x < 0)
				top_right_x = 0;
			if(top_right_y < 0)
				top_right_y = 0;
			if(bottom_right_x < 0)
				bottom_right_x = 0;
			if(bottom_right_y < 0)
				bottom_right_y = 0;
			if(bottom_left_x < 0)
				bottom_left_x = 0;
			if(bottom_left_y < 0)
				bottom_left_y = 0;
		}
		void fix_values(pixel_t width, pixel_t height)
		{
			fix_values();
			pixel_t half_width	= width / 2;
			pixel_t half_height = height / 2;
			auto	fix_one		= [&](pixel_t& radii_x, pixel_t& radii_y) {
				   pixel_t factor  = std::min(half_width / radii_x, half_height / radii_y);
				   radii_x		  *= factor;
				   radii_y		  *= factor;
			};

			if(top_left_x > half_width || top_left_y > half_height)
			{
				fix_one(top_left_x, top_left_y);
			}
			if(top_right_x > half_width || top_right_y > half_height)
			{
				fix_one(top_right_x, top_right_y);
			}
			if(bottom_right_x > half_width || bottom_right_y > half_height)
			{
				fix_one(bottom_right_x, bottom_right_y);
			}
			if(bottom_left_x > half_width || bottom_left_y > half_height)
			{
				fix_one(bottom_left_x, bottom_left_y);
			}
		}
	};

	struct css_border_radius
	{
		css_length top_left_x;
		css_length top_left_y;

		css_length top_right_x;
		css_length top_right_y;

		css_length bottom_right_x;
		css_length bottom_right_y;

		css_length bottom_left_x;
		css_length bottom_left_y;

		css_border_radius() = default;

		css_border_radius(const css_border_radius& val)
		{
			top_left_x	   = val.top_left_x;
			top_left_y	   = val.top_left_y;
			top_right_x	   = val.top_right_x;
			top_right_y	   = val.top_right_y;
			bottom_left_x  = val.bottom_left_x;
			bottom_left_y  = val.bottom_left_y;
			bottom_right_x = val.bottom_right_x;
			bottom_right_y = val.bottom_right_y;
		}

		css_border_radius& operator=(const css_border_radius& val) = default;

		border_radiuses calc_percents(pixel_t width, pixel_t height) const
		{
			border_radiuses ret;
			ret.bottom_left_x  = bottom_left_x.calc_percent(width);
			ret.bottom_left_y  = bottom_left_y.calc_percent(height);
			ret.top_left_x	   = top_left_x.calc_percent(width);
			ret.top_left_y	   = top_left_y.calc_percent(height);
			ret.top_right_x	   = top_right_x.calc_percent(width);
			ret.top_right_y	   = top_right_y.calc_percent(height);
			ret.bottom_right_x = bottom_right_x.calc_percent(width);
			ret.bottom_right_y = bottom_right_y.calc_percent(height);
			ret.fix_values(width, height);
			return ret;
		}
	};

	struct css_borders
	{
		css_border		  left;
		css_border		  top;
		css_border		  right;
		css_border		  bottom;
		css_border_radius radius;

		css_borders() = default;

		bool is_visible() const
		{
			return left.width.val() != 0 || right.width.val() != 0 || top.width.val() != 0 || bottom.width.val() != 0;
		}

		css_borders(const css_borders& val)
		{
			left   = val.left;
			right  = val.right;
			top	   = val.top;
			bottom = val.bottom;
			radius = val.radius;
		}

		css_borders& operator=(const css_borders& val) = default;
		string		 to_string() const
		{
			return "left: " + left.to_string() + ", top: " + top.to_string() + ", right: " + top.to_string() +
				   ", bottom: " + bottom.to_string();
		}
	};

	struct borders
	{
		border			left;
		border			top;
		border			right;
		border			bottom;
		border_radiuses radius;

		borders() = default;

		borders(const borders& val)
		{
			left   = val.left;
			right  = val.right;
			top	   = val.top;
			bottom = val.bottom;
			radius = val.radius;
		}

		borders(const css_borders& val)
		{
			left   = val.left;
			right  = val.right;
			top	   = val.top;
			bottom = val.bottom;
		}

		bool is_visible() const { return left.width != 0 || right.width != 0 || top.width != 0 || bottom.width != 0; }

		borders& operator=(const borders& val) = default;

		borders& operator=(const css_borders& val)
		{
			left   = val.left;
			right  = val.right;
			top	   = val.top;
			bottom = val.bottom;
			return *this;
		}
	};
} // namespace litehtml

#endif // LH_BORDERS_H
