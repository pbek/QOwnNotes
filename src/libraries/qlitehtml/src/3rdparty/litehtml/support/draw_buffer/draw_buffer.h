#ifndef LITEBROWSER_DRAW_BUFFER_H
#define LITEBROWSER_DRAW_BUFFER_H

#include <cairo.h>
#include <functional>
#include <cmath>
#include <memory>
#include "litehtml/types.h"

namespace litebrowser
{
	/// @brief Draw Buffer Class
	///
	/// This class performs the draw operations into the cairo surface.
	/// The application draws everything to the buffer, then buffer is
	/// drawn on widget or window.
	class draw_buffer
	{
		cairo_surface_t* m_draw_buffer		= nullptr;
		int				 m_width			= 0;
		int				 m_height			= 0;
		int				 m_top				= 0;
		int				 m_left				= 0;
		double			 m_scale_factor		= 1;
		int				 m_min_int_position = 1;

	  public:
		using draw_page_function_t = std::function<void(cairo_t* cr, int x, int y, const litehtml::position* clip)>;

		~draw_buffer()
		{
			if(m_draw_buffer)
			{
				cairo_surface_destroy(m_draw_buffer);
			}
		}

		[[nodiscard]]
		int get_width() const
		{
			return m_width;
		}

		[[nodiscard]]
		int get_height() const
		{
			return m_height;
		}

		[[nodiscard]]
		int get_left() const
		{
			return m_left;
		}

		[[nodiscard]]
		int get_top() const
		{
			return m_top;
		}

		[[nodiscard]]
		cairo_surface_t* get_cairo_surface() const
		{
			return m_draw_buffer;
		}

		[[nodiscard]]
		double get_scale_factor() const
		{
			return m_scale_factor;
		}

		/// @brief Set scale factor for draw buffer
		/// @param cb_draw the callback for drawing the page
		/// @param scale the scale factor to be applied
		void set_scale_factor(const draw_page_function_t& cb_draw, double scale)
		{
			if(m_scale_factor != scale)
			{
				m_scale_factor	   = scale;
				m_min_int_position = get_denominator(m_scale_factor);

				m_top			   = fix_position(m_top);
				m_left			   = fix_position(m_left);

				if(m_draw_buffer)
				{
					cairo_surface_destroy(m_draw_buffer);
				}
				m_draw_buffer = nullptr;
				create_draw_buffer(m_width, m_height);
				redraw(cb_draw);
			}
		}

		/// @brief Create cairo surface for draw buffer
		/// @param width surface width (not scaled)
		/// @param height surface height (not scaled)
		/// @param scale_factor scale factor
		/// @return pointer to the cairo surface
		static cairo_surface_t* make_surface(int width, int height, double scale_factor)
		{
			return cairo_image_surface_create(CAIRO_FORMAT_RGB24, (int) std::ceil((double) width * scale_factor),
											  (int) std::ceil((double) height * scale_factor));
		}

		/// @brief Creates new buffer with specified size
		/// @param width draw buffer width (not scaled)
		/// @param height draw buffer height (not scaled)
		/// @return true if new draw buffer was created, false if the old buffer was used
		bool create_draw_buffer(int width, int height)
		{
			if(m_width != width || m_height != height || !m_draw_buffer)
			{
				m_width	 = width;
				m_height = height;
				if(m_draw_buffer)
				{
					cairo_surface_destroy(m_draw_buffer);
				}
				m_draw_buffer = nullptr;
				if(m_width > 0 && m_height > 0)
				{
					m_draw_buffer = make_surface(m_width, m_height, m_scale_factor);
				}
				return true;
			}
			return false;
		}

		/// @brief Call this function when widget size changed
		/// @param cb_draw the callback for drawing the page
		/// @param width new draw buffer width
		/// @param height new draw buffer height
		void on_size_allocate(const draw_page_function_t& cb_draw, int width, int height)
		{
			if(create_draw_buffer(width, height))
			{
				redraw(cb_draw);
			}
		}

		/// @brief Scrolls draw buffer to the position (left, top).
		///
		/// Note, the actual position of the draw buffer can be rounded according to the scale factor.
		/// Use get_left() and get_top() to know the actual position.
		///
		/// @param cb_draw the callback for drawing the page
		/// @param left new horizontal position
		/// @param top new vertical position
		/// @param fixed_boxes fixed boxes to be redrawn
		void on_scroll(const draw_page_function_t& cb_draw, int left, int top,
					   const litehtml::position::vector& fixed_boxes);

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
		void redraw_area(const draw_page_function_t& cb_draw, int x, int y, int width, int height);

		/// @brief Redraw entire buffer
		/// @param cb_draw the callback for drawing the page
		void redraw(const draw_page_function_t& cb_draw) { redraw_area(cb_draw, m_left, m_top, m_width, m_height); }

	  private:
		[[nodiscard]] int fix_position(int pos) const { return (pos / m_min_int_position) * m_min_int_position; }

		static int get_common_divisor(int a, int b)
		{
			while(b != 0)
			{
				int t = b;
				b	  = a % b;
				a	  = t;
			}
			return a;
		}

		static int get_denominator(double decimal)
		{
			int numerator	   = (int) (decimal * 100);
			int denominator	   = 100;

			int common_divisor = get_common_divisor(numerator, denominator);
			return denominator / common_divisor;
		}
	};

} // namespace litebrowser

#endif
