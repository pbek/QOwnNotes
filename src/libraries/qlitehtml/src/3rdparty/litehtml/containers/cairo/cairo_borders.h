#ifndef LITEHTML_CAIRO_BORDERS_H
#define LITEHTML_CAIRO_BORDERS_H

#include <litehtml.h>
#include <cairo.h>

namespace cairo
{
	extern void add_path_arc(cairo_t* cr, double x, double y, double rx, double ry, double a1, double a2, bool neg);
	inline void set_color(cairo_t* cr, const litehtml::web_color& color)
	{
		cairo_set_source_rgba(cr, color.red / 255.0, color.green / 255.0, color.blue / 255.0, color.alpha / 255.0);
	}

	class border
	{
	  public:
		enum real_side_t
		{
			left_side,
			top_side,
			right_side,
			bottom_side
		};

		real_side_t			   real_side; /// real side of the border
		litehtml::web_color	   color;
		litehtml::border_style style;

		litehtml::pixel_t border_width;
		litehtml::pixel_t top_border_width;
		litehtml::pixel_t bottom_border_width;

		litehtml::pixel_t radius_top_x;
		litehtml::pixel_t radius_top_y;
		litehtml::pixel_t radius_bottom_x;
		litehtml::pixel_t radius_bottom_y;

		border(cairo_t* _cr, litehtml::pixel_t _left, litehtml::pixel_t _top, litehtml::pixel_t _bottom) :
			real_side(left_side),
			color(),
			style(litehtml::border_style_none),
			border_width(0),
			top_border_width(0),
			bottom_border_width(0),
			radius_top_x(0),
			radius_top_y(0),
			radius_bottom_x(0),
			radius_bottom_y(0),
			cr(_cr),
			left(_left),
			top(_top),
			bottom(_bottom)
		{
		}

		void draw_border();

	  private:
		cairo_t*		  cr;
		litehtml::pixel_t left;
		litehtml::pixel_t top;
		litehtml::pixel_t bottom;
		void			  draw_line(double line_offset, double top_line_offset, double bottom_line_offset);
		void			  draw_solid();
		void			  draw_dotted();
		void			  draw_dashed();
		void			  draw_double();
		void			  draw_inset_outset(bool is_inset);
		void			  draw_groove_ridge(bool is_groove);
	};
} // namespace cairo

#endif // LITEHTML_CAIRO_BORDERS_H
