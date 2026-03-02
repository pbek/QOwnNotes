#ifndef LITEBROWSER_HTML_HOST_H
#define LITEBROWSER_HTML_HOST_H

#include <cairo.h>
#include <litehtml.h>

namespace litebrowser
{
	class html_host_interface
	{
	  public:
		virtual ~html_host_interface()												   = default;

		virtual void			 open_url(const std::string& url)					   = 0;
		virtual void			 update_cursor()									   = 0;
		virtual void			 scroll_to(int x, int y)							   = 0;
		virtual void			 get_viewport(litehtml::position& viewport) const	   = 0;
		virtual void			 redraw_boxes(const litehtml::position::vector& boxes) = 0;
		virtual int				 get_render_width()									   = 0;
		virtual double			 get_dpi()											   = 0;
		virtual int				 get_screen_width()									   = 0;
		virtual int				 get_screen_height()								   = 0;
		virtual cairo_surface_t* load_image(const std::string& path)				   = 0;
	};

	class browser_notify_interface
	{
	  public:
		virtual ~browser_notify_interface()							 = default;

		virtual void redraw()										 = 0;
		virtual void render()										 = 0;
		virtual void update_state()									 = 0;
		virtual void on_set_caption(const std::string& caption_text) = 0;
		virtual void on_page_loaded(uint64_t web_page_id)			 = 0;
	};

} // namespace litebrowser

#endif // LITEBROWSER_HTML_HOST_H
