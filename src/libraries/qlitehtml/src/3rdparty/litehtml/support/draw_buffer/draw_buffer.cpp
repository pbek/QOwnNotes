#include "draw_buffer.h"
#include "litehtml/types.h"

/// @brief Scrolls draw buffer to the position (left, top).
///
/// Note, the actual position of the draw buffer can be rounded according to the scale factor.
/// Use get_left() and get_top() to know the actual position.
///
/// @param cb_draw the callback for drawing the page
/// @param left new horizontal position
/// @param top new vertical position
/// @param fixed_boxes fixed boxes to be redrawn
void litebrowser::draw_buffer::on_scroll(const draw_page_function_t& cb_draw, int left, int top,
										 const litehtml::position::vector& fixed_boxes)
{
	if(m_width <= 0 || m_height <= 0 || !m_draw_buffer)
		return;

	top	 = fix_position(top);
	left = fix_position(left);

	if(m_left != left || m_top != top)
	{
		litehtml::position rec_current(m_left, m_top, m_width, m_height); // Current area
		litehtml::position rec_new(left, top, m_width, m_height);		  // New area
		litehtml::position rec_clean = rec_current.intersect(rec_new);	  // Clean area
		if(rec_clean.empty() || rec_new == rec_current)
		{
			m_left = left;
			m_top  = top;
			redraw(cb_draw);
		} else
		{
			int shift_x			 = m_left - left;
			int shift_y			 = m_top - top;

			int surface_shift_x	 = (int) std::floor((double) shift_x * m_scale_factor);
			int surface_shift_y	 = (int) std::floor((double) shift_y * m_scale_factor);

			auto	 new_surface = make_surface(m_width, m_height, m_scale_factor);
			cairo_t* cr			 = cairo_create(new_surface);
			cairo_rectangle(cr, (rec_clean.x - left) * m_scale_factor - m_scale_factor,
							(rec_clean.y - top) * m_scale_factor - m_scale_factor,
							std::ceil((double) rec_clean.width * m_scale_factor) + 2.0 * m_scale_factor,
							std::ceil((double) rec_clean.height * m_scale_factor) + 2.0 * m_scale_factor);
			cairo_clip(cr);
			cairo_set_source_surface(cr, m_draw_buffer, surface_shift_x, surface_shift_y);
			cairo_paint(cr);
			cairo_destroy(cr);
			cairo_surface_destroy(m_draw_buffer);
			m_draw_buffer	 = new_surface;

			m_left			 = left;
			m_top			 = top;

			int right		 = fix_position(m_left + m_width);
			int bottom		 = fix_position(m_top + m_height);
			int clean_right	 = fix_position(rec_clean.x + rec_clean.width);
			int clean_bottom = fix_position(rec_clean.y + rec_clean.height);

			if(rec_clean.x > m_left)
			{
				redraw_area(cb_draw, m_left, rec_clean.y, rec_clean.x - m_left, rec_clean.height);
			}
			if(clean_right < right)
			{
				redraw_area(cb_draw, clean_right, rec_clean.y, right - clean_right, rec_clean.height);
			}

			if(rec_clean.y > m_top)
			{
				redraw_area(cb_draw, m_left, m_top, m_width, rec_clean.y - m_top);
			}
			if(clean_bottom < bottom)
			{
				redraw_area(cb_draw, m_left, clean_bottom, m_width, bottom - clean_bottom);
			}

			for(const auto& box : fixed_boxes)
			{
				redraw_area(cb_draw, m_left + box.left(), m_top + box.top(), box.width, box.height);
				redraw_area(cb_draw, m_left + box.left() + shift_x, m_top + box.top() + shift_y, box.width, box.height);
			}
		}
	}
}

/// @brief Redraw the defined area of the buffer
///
/// All coordinated are not scaled. The actual rectangle could be different, according to the scale factor,
/// but it must always cover the requested.
///
/// @param cb_draw the callback for drawing the page
/// @param x left position of the area
/// @param y top position of the area
/// @param width width of the area
/// @param height height of the area
void litebrowser::draw_buffer::redraw_area(const draw_page_function_t& cb_draw, int x, int y, int width, int height)
{
	if(m_draw_buffer)
	{
		int fixed_left	 = fix_position(x - m_left);
		int fixed_right	 = fix_position(x - m_left + width);
		int fixed_top	 = fix_position(y - m_top);
		int fixed_bottom = fix_position(y - m_top + height);

		if(fixed_right < x + width)
			fixed_right += m_min_int_position;
		if(fixed_bottom < y + height)
			fixed_bottom += m_min_int_position;

		int fixed_x		 = fixed_left;
		int fixed_y		 = fixed_top;
		int fixed_width	 = fixed_right - fixed_left;
		int fixed_height = fixed_bottom - fixed_top;

		int s_x			 = (int) std::round((double) fixed_x * m_scale_factor);
		int s_y			 = (int) std::round((double) fixed_y * m_scale_factor);
		int s_width		 = (int) std::round((double) fixed_width * m_scale_factor);
		int s_height	 = (int) std::round((double) fixed_height * m_scale_factor);

		litehtml::position pos{(litehtml::pixel_t) fixed_x, (litehtml::pixel_t) fixed_y,
							   (litehtml::pixel_t) fixed_width, (litehtml::pixel_t) fixed_height};
		cairo_t*		   cr = cairo_create(m_draw_buffer);

		// Apply clip with scaled position to avoid artifacts
		cairo_rectangle(cr, s_x, s_y, s_width, s_height);
		cairo_clip(cr);

		// Clear rectangle with scaled position
		cairo_rectangle(cr, s_x, s_y, s_width, s_height);
		cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
		cairo_fill(cr);

		// Apply scale for drawing
		cairo_scale(cr, m_scale_factor, m_scale_factor);

		// Draw page
		cb_draw(cr, -m_left, -m_top, &pos);

		cairo_destroy(cr);
	}
}
