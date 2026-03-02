#ifndef LITEBROWSER_CONTAINER_CAIRO_PANGO_H
#define LITEBROWSER_CONTAINER_CAIRO_PANGO_H

#include <litehtml.h>
#include "container_cairo.h"
#include <cairo.h>
#include <pango/pangocairo.h>
#include <pango/pango-font.h>
#include <set>

struct cairo_font
{
	PangoFontDescription* font;
	litehtml::pixel_t	  size;
	bool				  underline;
	bool				  strikeout;
	bool				  overline;
	litehtml::pixel_t	  ascent;
	litehtml::pixel_t	  descent;
	int					  underline_thickness;
	int					  underline_position;
	int					  strikethrough_thickness;
	int					  strikethrough_position;
	int					  overline_thickness;
	int					  overline_position;
	int					  decoration_style;
	litehtml::web_color	  decoration_color;
};

class container_cairo_pango : public container_cairo
{
	cairo_surface_t*	  m_temp_surface;
	cairo_t*			  m_temp_cr;
	std::set<std::string> m_all_fonts;

  public:
	container_cairo_pango();
	~container_cairo_pango() override;
	litehtml::uint_ptr create_font(const litehtml::font_description& descr, const litehtml::document* doc,
								   litehtml::font_metrics* fm) override;
	void			   delete_font(litehtml::uint_ptr hFont) override;
	litehtml::pixel_t  text_width(const char* text, litehtml::uint_ptr hFont) override;
	void draw_text(litehtml::uint_ptr hdc, const char* text, litehtml::uint_ptr hFont, litehtml::web_color color,
				   const litehtml::position& pos) override;

	virtual cairo_font_options_t* get_font_options() { return nullptr; }
};

#endif // LITEBROWSER_CONTAINER_CAIRO_PANGO_H
