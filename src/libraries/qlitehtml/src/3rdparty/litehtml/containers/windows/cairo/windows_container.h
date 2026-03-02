#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <mlang.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <vector>
#include "cairo.h"
#include "cairo-win32.h"
#include <litehtml.h>
#include <dib.h>
#include <txdib.h>
#include "../../cairo/container_cairo.h"

class windows_container : public container_cairo
{
  protected:
	cairo_surface_t* m_temp_surface;
	cairo_t*		 m_temp_cr;
	IMLangFontLink2* m_font_link;

  public:
	windows_container(void);
	virtual ~windows_container(void);

	litehtml::uint_ptr create_font(const litehtml::font_description& descr, const litehtml::document* doc,
								   litehtml::font_metrics* fm) override;
	void			   delete_font(litehtml::uint_ptr hFont) override;
	litehtml::pixel_t  text_width(const char* text, litehtml::uint_ptr hFont) override;
	void draw_text(litehtml::uint_ptr hdc, const char* text, litehtml::uint_ptr hFont, litehtml::web_color color,
				   const litehtml::position& pos) override;
	litehtml::string resolve_color(const litehtml::string& color) const override;

	cairo_surface_t* get_image(const std::string& url) override;
	double			 get_screen_dpi() const override;
	int				 get_screen_width() const override;
	int				 get_screen_height() const override;
};
