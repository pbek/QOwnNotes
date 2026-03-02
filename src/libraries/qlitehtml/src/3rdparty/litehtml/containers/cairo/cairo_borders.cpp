#include <litehtml.h>
#include "cairo_borders.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void cairo::add_path_arc(cairo_t* cr, double x, double y, double rx, double ry, double a1, double a2, bool neg)
{
	if(rx > 0 && ry > 0)
	{
		cairo_save(cr);

		cairo_translate(cr, x, y);
		cairo_scale(cr, 1, ry / rx);
		cairo_translate(cr, -x, -y);

		if(neg)
		{
			cairo_arc_negative(cr, x, y, rx, a1, a2);
		} else
		{
			cairo_arc(cr, x, y, rx, a1, a2);
		}

		cairo_restore(cr);
	} else
	{
		cairo_move_to(cr, x, y);
	}
}

/**
 * Draw border at the left side. Use the only function to draw all border by using rotation transfer
 * @param cr cairo context
 * @param left left position of the border
 * @param top top position of the border
 * @param bottom bottom position of the border
 * @param data border data
 */
void cairo::border::draw_border()
{
	cairo_save(cr);

	if(radius_top_x != 0 && radius_top_y != 0)
	{
		double start_angle = M_PI;
		double end_angle   = start_angle + M_PI / 2.0 / ((double) top_border_width / (double) border_width + 1);

		add_path_arc(cr, left + radius_top_x, top + radius_top_y, radius_top_x - border_width,
					 radius_top_y - border_width + (border_width - top_border_width), start_angle, end_angle, false);

		add_path_arc(cr, left + radius_top_x, top + radius_top_y, radius_top_x, radius_top_y, end_angle, start_angle,
					 true);
	} else
	{
		cairo_move_to(cr, left + border_width, top + top_border_width);
		cairo_line_to(cr, left, top);
	}

	if(radius_bottom_x != 0 && radius_bottom_y != 0)
	{
		cairo_line_to(cr, left, bottom - radius_bottom_y);

		double end_angle   = M_PI;
		double start_angle = end_angle - M_PI / 2.0 / ((double) bottom_border_width / (double) border_width + 1);

		add_path_arc(cr, left + radius_bottom_x, bottom - radius_bottom_y, radius_bottom_x, radius_bottom_y, end_angle,
					 start_angle, true);

		add_path_arc(cr, left + radius_bottom_x, bottom - radius_bottom_y, radius_bottom_x - border_width,
					 radius_bottom_y - border_width + (border_width - bottom_border_width), start_angle, end_angle,
					 false);
	} else
	{
		cairo_line_to(cr, left, bottom);
		cairo_line_to(cr, left + border_width, bottom - bottom_border_width);
	}
	cairo_clip(cr);

	switch(style)
	{
	case litehtml::border_style_dotted:
		draw_dotted();
		break;
	case litehtml::border_style_dashed:
		draw_dashed();
		break;
	case litehtml::border_style_double:
		draw_double();
		break;
	case litehtml::border_style_inset:
		draw_inset_outset(true);
		break;
	case litehtml::border_style_outset:
		draw_inset_outset(false);
		break;
	case litehtml::border_style_groove:
		draw_groove_ridge(true);
		break;
	case litehtml::border_style_ridge:
		draw_groove_ridge(false);
		break;
	default:
		draw_solid();
		break;
	}

	cairo_restore(cr);
}

void cairo::border::draw_line(double line_offset, double top_line_offset, double bottom_line_offset)
{
	if(radius_top_x != 0 && radius_top_y != 0)
	{
		double end_angle   = M_PI;
		double start_angle = end_angle + M_PI / 2.0 / ((double) top_border_width / (double) border_width + 1);

		add_path_arc(cr, left + radius_top_x, top + radius_top_y, radius_top_x - line_offset,
					 radius_top_y - line_offset + (line_offset - top_line_offset), start_angle, end_angle, true);
	} else
	{
		cairo_move_to(cr, left + line_offset, top);
	}

	if(radius_bottom_x != 0 && radius_bottom_y != 0)
	{
		cairo_line_to(cr, left + line_offset, bottom - radius_bottom_y);

		double start_angle = M_PI;
		double end_angle   = start_angle - M_PI / 2.0 / ((double) bottom_border_width / (double) border_width + 1);

		add_path_arc(cr, left + radius_bottom_x, bottom - radius_bottom_y, radius_bottom_x - line_offset,
					 radius_bottom_y - line_offset + (line_offset - bottom_line_offset), start_angle, end_angle, true);
	} else
	{
		cairo_line_to(cr, left + line_offset, bottom);
	}
}

void cairo::border::draw_inset_outset(bool is_inset)
{
	litehtml::web_color line_color;
	litehtml::web_color light_color = color;
	litehtml::web_color dark_color	= color.darken(0.33);
	if(color.red == 0 && color.green == 0 && color.blue == 0)
	{
		dark_color.red = dark_color.green = dark_color.blue = 0x4C;
		light_color.red = light_color.green = light_color.blue = 0xB2;
	}

	if(real_side == left_side || real_side == top_side)
	{
		line_color = is_inset ? dark_color : light_color;
	} else
	{
		line_color = is_inset ? light_color : dark_color;
	}
	draw_line(border_width / 2.0, top_border_width / 2.0, bottom_border_width / 2.0);
	cairo_set_line_cap(cr, CAIRO_LINE_CAP_BUTT);
	cairo_set_dash(cr, nullptr, 0, 0);
	set_color(cr, line_color);
	cairo_set_line_width(cr, border_width);
	cairo_stroke(cr);
}

void cairo::border::draw_double()
{
	if(border_width < 3)
	{
		draw_solid();
	} else
	{
		cairo_set_line_cap(cr, CAIRO_LINE_CAP_BUTT);
		cairo_set_dash(cr, nullptr, 0, 0);
		set_color(cr, color);

		double line_width = border_width / 3.0;
		cairo_set_line_width(cr, line_width);
		// draw external line
		draw_line(line_width / 2.0, top_border_width / 6.0, bottom_border_width / 6.0);
		cairo_stroke(cr);
		// draw internal line
		draw_line(border_width - line_width / 2.0, top_border_width - top_border_width / 6.0,
				  bottom_border_width - bottom_border_width / 6.0);
		cairo_stroke(cr);
	}
}

void cairo::border::draw_dashed()
{
	litehtml::pixel_t line_length = std::abs(bottom - top);
	if(line_length == 0)
		return;

	draw_line(border_width / 2.0, top_border_width / 2.0, bottom_border_width / 2.0);

	litehtml::pixel_t segment_length = border_width * 3;
	int				  seg_nums		 = (int) (line_length / segment_length);
	if(seg_nums < 2)
	{
		seg_nums = 2;
	}
	if(seg_nums % 2 != 0)
	{
		seg_nums = seg_nums + 1;
	}
	seg_nums++;
	double seg_len = (double) line_length / (double) seg_nums;

	double dashes[2];
	dashes[0] = seg_len;
	dashes[1] = seg_len;
	cairo_set_line_cap(cr, CAIRO_LINE_CAP_BUTT);
	cairo_set_dash(cr, dashes, 2, 0);
	set_color(cr, color);
	cairo_set_line_width(cr, border_width);
	cairo_stroke(cr);
}

void cairo::border::draw_solid()
{
	draw_line(border_width / 2.0, top_border_width / 2.0, bottom_border_width / 2.0);
	cairo_set_line_cap(cr, CAIRO_LINE_CAP_BUTT);
	cairo_set_dash(cr, nullptr, 0, 0);
	set_color(cr, color);
	cairo_set_line_width(cr, border_width);
	cairo_stroke(cr);
}

void cairo::border::draw_dotted()
{
	// Zero length line
	if(bottom == top)
		return;

	draw_line(border_width / 2.0, top_border_width / 2.0, bottom_border_width / 2.0);

	double line_length = std::abs(bottom - top);

	double dot_size	   = border_width;
	int	   num_dots	   = (int) std::nearbyint(line_length / (dot_size * 2.0));
	if(num_dots < 2)
	{
		num_dots = 2;
	}
	if(num_dots % 2 != 0)
	{
		num_dots = num_dots + 1;
	}
	num_dots++;
	double space_len = ((double) line_length - (double) border_width) / (num_dots - 1.0);

	double dashes[2];
	dashes[0] = 0;
	dashes[1] = space_len;
	cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
	cairo_set_dash(cr, dashes, 2, -dot_size / 2.0);

	set_color(cr, color);
	cairo_set_line_width(cr, border_width);
	cairo_stroke(cr);
}

void cairo::border::draw_groove_ridge(bool is_groove)
{
	if(border_width == 1)
	{
		draw_solid();
	} else
	{
		litehtml::web_color inner_line_color;
		litehtml::web_color outer_line_color;
		litehtml::web_color light_color = color;
		litehtml::web_color dark_color	= color.darken(0.33);
		if(color.red == 0 && color.green == 0 && color.blue == 0)
		{
			dark_color.red = dark_color.green = dark_color.blue = 0x4C;
			light_color.red = light_color.green = light_color.blue = 0xB2;
		}

		if(real_side == left_side || real_side == top_side)
		{
			outer_line_color = is_groove ? dark_color : light_color;
			inner_line_color = is_groove ? light_color : dark_color;
		} else
		{
			outer_line_color = is_groove ? light_color : dark_color;
			inner_line_color = is_groove ? dark_color : light_color;
		}

		cairo_set_line_cap(cr, CAIRO_LINE_CAP_BUTT);
		cairo_set_dash(cr, nullptr, 0, 0);

		double line_width = border_width / 2.0;
		cairo_set_line_width(cr, line_width);
		// draw external line
		draw_line(line_width / 2.0, top_border_width / 4.0, bottom_border_width / 4.0);
		set_color(cr, outer_line_color);
		cairo_stroke(cr);
		// draw internal line
		set_color(cr, inner_line_color);
		draw_line(border_width - line_width / 2.0, top_border_width - top_border_width / 4.0,
				  bottom_border_width - bottom_border_width / 4.0);
		cairo_stroke(cr);
	}
}
