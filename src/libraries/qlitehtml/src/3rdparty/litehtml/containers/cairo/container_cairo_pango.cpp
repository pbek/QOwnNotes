#include <array>
#include "container_cairo_pango.h"

container_cairo_pango::container_cairo_pango()
{
	m_temp_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 2, 2);
	m_temp_cr	   = cairo_create(m_temp_surface);
	cairo_save(m_temp_cr);
	PangoLayout*	  layout  = pango_cairo_create_layout(m_temp_cr);
	PangoContext*	  context = pango_layout_get_context(layout);
	PangoFontFamily** families;
	int				  n;
	pango_context_list_families(context, &families, &n);
	for(int i = 0; i < n; i++)
	{
		PangoFontFamily* family = families[i];
		if(!PANGO_IS_FONT_FAMILY(family))
			continue;
		std::string font_name = pango_font_family_get_name(family);
		litehtml::lcase(font_name);
		m_all_fonts.insert(font_name);
	}
	g_free(families);
	cairo_restore(m_temp_cr);
	g_object_unref(layout);
}

container_cairo_pango::~container_cairo_pango()
{
	clear_images();
	cairo_surface_destroy(m_temp_surface);
	cairo_destroy(m_temp_cr);
}

litehtml::uint_ptr container_cairo_pango::create_font(const litehtml::font_description& descr,
													  const litehtml::document* doc, litehtml::font_metrics* fm)
{
	litehtml::string_vector tokens;
	litehtml::split_string(descr.family, tokens, ",");
	std::string fonts;

	for(auto& font : tokens)
	{
		litehtml::trim(font, " \t\r\n\f\v\"\'");
		if(litehtml::value_in_list(font, "serif;sans-serif;monospace;cursive;fantasy;"))
		{
			fonts = font + ",";
			break;
		}
		litehtml::lcase(font);
		if(m_all_fonts.find(font) != m_all_fonts.end())
		{
			fonts  = font;
			fonts += ",";
			break;
		}
	}

	if(fonts.empty())
	{
		fonts = "serif,";
	}

	PangoFontDescription* desc = pango_font_description_from_string(fonts.c_str());
	pango_font_description_set_absolute_size(desc, descr.size * PANGO_SCALE);
	if(descr.style == litehtml::font_style_italic)
	{
		pango_font_description_set_style(desc, PANGO_STYLE_ITALIC);
	} else
	{
		pango_font_description_set_style(desc, PANGO_STYLE_NORMAL);
	}

	pango_font_description_set_weight(desc, (PangoWeight) descr.weight);

	cairo_font* ret = nullptr;

	if(fm)
	{
		fm->font_size = descr.size;

		cairo_save(m_temp_cr);
		PangoLayout*   layout	= pango_cairo_create_layout(m_temp_cr);
		PangoContext*  context	= pango_layout_get_context(layout);
		PangoLanguage* language = pango_language_get_default();
		pango_layout_set_font_description(layout, desc);
		PangoFontMetrics* metrics = pango_context_get_metrics(context, desc, language);

		fm->ascent				  = PANGO_PIXELS((double) pango_font_metrics_get_ascent(metrics));
		fm->height				  = PANGO_PIXELS((double) pango_font_metrics_get_height(metrics));
		fm->descent				  = fm->height - fm->ascent;
		fm->x_height			  = fm->height;
		fm->draw_spaces			  = (descr.decoration_line != litehtml::text_decoration_line_none);
		fm->sub_shift			  = descr.size / 5;
		fm->super_shift			  = descr.size / 3;

		pango_layout_set_text(layout, "x", 1);

		PangoRectangle ink_rect;
		PangoRectangle logical_rect;
		pango_layout_get_pixel_extents(layout, &ink_rect, &logical_rect);
		fm->x_height = ink_rect.height;
		if(fm->x_height == fm->height)
			fm->x_height = fm->x_height * 4 / 5;

		pango_layout_set_text(layout, "0", 1);

		pango_layout_get_pixel_extents(layout, &ink_rect, &logical_rect);
		fm->ch_width = logical_rect.width;

		cairo_restore(m_temp_cr);

		ret					  = new cairo_font;
		ret->font			  = desc;
		ret->size			  = descr.size;
		ret->strikeout		  = (descr.decoration_line & litehtml::text_decoration_line_line_through) != 0;
		ret->underline		  = (descr.decoration_line & litehtml::text_decoration_line_underline) != 0;
		ret->overline		  = (descr.decoration_line & litehtml::text_decoration_line_overline) != 0;
		ret->ascent			  = fm->ascent;
		ret->descent		  = fm->descent;
		ret->decoration_color = descr.decoration_color;
		ret->decoration_style = descr.decoration_style;

		auto thinkness		  = descr.decoration_thickness;
		if(!thinkness.is_predefined())
		{
			litehtml::css_length one_em(1.0, litehtml::css_units_em);
			doc->cvt_units(one_em, *fm, 0);
			doc->cvt_units(thinkness, *fm, (int) one_em.val());
		}

		ret->underline_position = -pango_font_metrics_get_underline_position(metrics);
		if(thinkness.is_predefined())
		{
			ret->underline_thickness = pango_font_metrics_get_underline_thickness(metrics);
		} else
		{
			ret->underline_thickness = (int) (thinkness.val() * PANGO_SCALE);
		}
		pango_quantize_line_geometry(&ret->underline_thickness, &ret->underline_position);
		ret->underline_thickness	= PANGO_PIXELS(ret->underline_thickness);
		ret->underline_position		= PANGO_PIXELS(ret->underline_position);

		ret->strikethrough_position = pango_font_metrics_get_strikethrough_position(metrics);
		if(thinkness.is_predefined())
		{
			ret->strikethrough_thickness = pango_font_metrics_get_strikethrough_thickness(metrics);
		} else
		{
			ret->strikethrough_thickness = (int) (thinkness.val() * PANGO_SCALE);
		}
		pango_quantize_line_geometry(&ret->strikethrough_thickness, &ret->strikethrough_position);
		ret->strikethrough_thickness = PANGO_PIXELS(ret->strikethrough_thickness);
		ret->strikethrough_position	 = PANGO_PIXELS(ret->strikethrough_position);

		ret->overline_position		 = pango_units_from_double(fm->ascent);
		if(thinkness.is_predefined())
		{
			ret->overline_thickness = pango_font_metrics_get_underline_thickness(metrics);
		} else
		{
			ret->overline_thickness = (int) (thinkness.val() * PANGO_SCALE);
		}
		pango_quantize_line_geometry(&ret->overline_thickness, &ret->overline_position);
		ret->overline_thickness = PANGO_PIXELS(ret->overline_thickness);
		ret->overline_position	= PANGO_PIXELS(ret->overline_position);

		g_object_unref(layout);
		pango_font_metrics_unref(metrics);
	}

	return (litehtml::uint_ptr) ret;
}

void container_cairo_pango::delete_font(litehtml::uint_ptr hFont)
{
	auto* fnt = (cairo_font*) hFont;
	if(fnt)
	{
		pango_font_description_free(fnt->font);
		delete fnt;
	}
}

litehtml::pixel_t container_cairo_pango::text_width(const char* text, litehtml::uint_ptr hFont)
{
	auto* fnt = (cairo_font*) hFont;

	cairo_save(m_temp_cr);

	PangoLayout* layout = pango_cairo_create_layout(m_temp_cr);
	pango_layout_set_font_description(layout, fnt->font);

	pango_layout_set_text(layout, text, -1);
	pango_cairo_update_layout(m_temp_cr, layout);

	int x_width, x_height;
	pango_layout_get_pixel_size(layout, &x_width, &x_height);

	cairo_restore(m_temp_cr);

	g_object_unref(layout);

	return (int) x_width;
}

enum class draw_type
{
	DRAW_OVERLINE,
	DRAW_STRIKETHROUGH,
	DRAW_UNDERLINE
};

static inline void draw_single_line(cairo_t* cr, litehtml::pixel_t x, litehtml::pixel_t y, litehtml::pixel_t width,
									int thickness, draw_type type)
{
	double top;
	switch(type)
	{
	case draw_type::DRAW_UNDERLINE:
		top = y + (double) thickness / 2.0;
		break;
	case draw_type::DRAW_OVERLINE:
		top = y - (double) thickness / 2.0;
		break;
	case draw_type::DRAW_STRIKETHROUGH:
		top = y + 0.5;
		break;
	default:
		top = y;
		break;
	}
	cairo_move_to(cr, x, top);
	cairo_line_to(cr, x + width, top);
}

static void draw_solid_line(cairo_t* cr, litehtml::pixel_t x, litehtml::pixel_t y, litehtml::pixel_t width,
							int thickness, draw_type type, litehtml::web_color& color)
{
	draw_single_line(cr, x, y, width, thickness, type);

	cairo_set_source_rgba(cr, (double) color.red / 255.0, (double) color.green / 255.0, (double) color.blue / 255.0,
						  (double) color.alpha / 255.0);
	cairo_set_line_width(cr, thickness);
	cairo_stroke(cr);
}

static void draw_dotted_line(cairo_t* cr, litehtml::pixel_t x, litehtml::pixel_t y, litehtml::pixel_t width,
							 int thickness, draw_type type, litehtml::web_color& color)
{
	draw_single_line(cr, x, y, width, thickness, type);

	std::array<double, 2> dashes{0, thickness * 2.0};
	if(thickness == 1)
		dashes[1] += thickness / 2.0;
	cairo_set_line_width(cr, thickness);
	cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
	cairo_set_dash(cr, dashes.data(), 2, x);

	cairo_set_source_rgba(cr, (double) color.red / 255.0, (double) color.green / 255.0, (double) color.blue / 255.0,
						  (double) color.alpha / 255.0);
	cairo_stroke(cr);
}

static void draw_dashed_line(cairo_t* cr, litehtml::pixel_t x, litehtml::pixel_t y, litehtml::pixel_t width,
							 int thickness, draw_type type, litehtml::web_color& color)
{
	draw_single_line(cr, x, y, width, thickness, type);

	std::array<double, 2> dashes{thickness * 2.0, thickness * 3.0};
	cairo_set_line_width(cr, thickness);
	cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
	cairo_set_dash(cr, dashes.data(), 2, x);

	cairo_set_source_rgba(cr, (double) color.red / 255.0, (double) color.green / 255.0, (double) color.blue / 255.0,
						  (double) color.alpha / 255.0);
	cairo_stroke(cr);
}

static void draw_wavy_line(cairo_t* cr, litehtml::pixel_t x, litehtml::pixel_t y, litehtml::pixel_t width,
						   int thickness, draw_type type, litehtml::web_color& color)
{
	int h_pad		 = 1;
	int brush_height = (int) thickness * 3 + h_pad * 2;
	int brush_width	 = brush_height * 2 - 2 * thickness;

	double top;
	switch(type)
	{
	case draw_type::DRAW_UNDERLINE:
		top = y + (double) brush_height / 2.0;
		break;
	case draw_type::DRAW_OVERLINE:
		top = y - (double) brush_height / 2.0;
		break;
	default:
		top = y;
		break;
	}

	cairo_surface_t* brush_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, brush_width, brush_height);
	cairo_t*		 brush_cr	   = cairo_create(brush_surface);

	cairo_set_source_rgba(brush_cr, (double) color.red / 255.0, (double) color.green / 255.0,
						  (double) color.blue / 255.0, (double) color.alpha / 255.0);
	cairo_set_line_width(brush_cr, thickness);
	double w = thickness / 2.0;
	cairo_move_to(brush_cr, 0, brush_height - (double) thickness / 2.0 - h_pad);
	cairo_line_to(brush_cr, w, brush_height - (double) thickness / 2.0 - h_pad);
	cairo_line_to(brush_cr, brush_width / 2.0 - w, (double) thickness / 2.0 + h_pad);
	cairo_line_to(brush_cr, brush_width / 2.0 + w, (double) thickness / 2.0 + h_pad);
	cairo_line_to(brush_cr, brush_width - w, brush_height - (double) thickness / 2.0 - h_pad);
	cairo_line_to(brush_cr, brush_width, brush_height - (double) thickness / 2.0 - h_pad);
	cairo_stroke(brush_cr);
	cairo_destroy(brush_cr);

	cairo_pattern_t* pattern = cairo_pattern_create_for_surface(brush_surface);
	cairo_pattern_set_extend(pattern, CAIRO_EXTEND_REPEAT);
	cairo_matrix_t patterm_matrix;
	cairo_matrix_init_translate(&patterm_matrix, 0, -top + brush_height / 2.0);
	cairo_pattern_set_matrix(pattern, &patterm_matrix);
	cairo_set_source(cr, pattern);

	cairo_set_line_width(cr, brush_height);
	cairo_move_to(cr, x, top);
	cairo_line_to(cr, x + width, top);
	cairo_stroke(cr);

	cairo_pattern_destroy(pattern);
	cairo_surface_destroy(brush_surface);
}

static void draw_double_line(cairo_t* cr, litehtml::pixel_t x, litehtml::pixel_t y, litehtml::pixel_t width,
							 int thickness, draw_type type, litehtml::web_color& color)
{
	cairo_set_line_width(cr, thickness);
	double top1;
	double top2;
	switch(type)
	{
	case draw_type::DRAW_UNDERLINE:
		top1 = y + (double) thickness / 2.0;
		top2 = top1 + (double) thickness + (double) thickness / 2.0 + 0.5;
		break;
	case draw_type::DRAW_OVERLINE:
		top1 = y - (double) thickness / 2.0;
		top2 = top1 - (double) thickness - (double) thickness / 2.0 - 0.5;
		break;
	case draw_type::DRAW_STRIKETHROUGH:
		top1 = y - (double) thickness + 0.5;
		top2 = y + (double) thickness + 0.5;
		break;
	default:
		top1 = y;
		top2 = y;
		break;
	}
	cairo_move_to(cr, x, top1);
	cairo_line_to(cr, x + width, top1);
	cairo_stroke(cr);
	cairo_move_to(cr, x, top2);
	cairo_line_to(cr, x + width, top2);
	cairo_set_source_rgba(cr, (double) color.red / 255.0, (double) color.green / 255.0, (double) color.blue / 255.0,
						  (double) color.alpha / 255.0);
	cairo_stroke(cr);
}

void container_cairo_pango::draw_text(litehtml::uint_ptr hdc, const char* text, litehtml::uint_ptr hFont,
									  litehtml::web_color color, const litehtml::position& pos)
{
	auto* fnt = (cairo_font*) hFont;
	auto* cr  = (cairo_t*) hdc;
	cairo_save(cr);

	apply_clip(cr);

	set_color(cr, color);

	litehtml::web_color decoration_color = color;

	PangoLayout* layout					 = pango_cairo_create_layout(cr);
	pango_layout_set_font_description(layout, fnt->font);
	pango_layout_set_text(layout, text, -1);

	auto font_options = get_font_options();
	if(font_options)
	{
		auto ctx = pango_layout_get_context(layout);
		pango_cairo_context_set_font_options(ctx, font_options);
	}

	PangoRectangle ink_rect, logical_rect;
	pango_layout_get_pixel_extents(layout, &ink_rect, &logical_rect);

	litehtml::pixel_t text_baseline = pos.height - fnt->descent;

	litehtml::pixel_t x				= pos.left() + logical_rect.x;
	litehtml::pixel_t y				= pos.top();

	cairo_move_to(cr, x, y);
	pango_cairo_update_layout(cr, layout);
	pango_cairo_show_layout(cr, layout);

	litehtml::pixel_t tw = 0;

	if(fnt->underline || fnt->strikeout || fnt->overline)
	{
		tw = text_width(text, hFont);
	}

	if(!fnt->decoration_color.is_current_color)
	{
		decoration_color = fnt->decoration_color;
	}

	std::array<decltype(&draw_solid_line), litehtml::text_decoration_style_max> draw_funcs{
		draw_solid_line,  // text_decoration_style_solid
		draw_double_line, // text_decoration_style_double
		draw_dotted_line, // text_decoration_style_dotted
		draw_dashed_line, // text_decoration_style_dashed
		draw_wavy_line,	  // text_decoration_style_wavy
	};

	if(fnt->underline)
	{
		draw_funcs[fnt->decoration_style](cr, x, pos.top() + text_baseline + fnt->underline_position, tw,
										  fnt->underline_thickness, draw_type::DRAW_UNDERLINE, decoration_color);
	}

	if(fnt->strikeout)
	{
		draw_funcs[fnt->decoration_style](cr, x, pos.top() + text_baseline - fnt->strikethrough_position, tw,
										  fnt->strikethrough_thickness, draw_type::DRAW_STRIKETHROUGH,
										  decoration_color);
	}

	if(fnt->overline)
	{
		draw_funcs[fnt->decoration_style](cr, x, pos.top() + text_baseline - fnt->overline_position, tw,
										  fnt->overline_thickness, draw_type::DRAW_OVERLINE, decoration_color);
	}

	cairo_restore(cr);

	g_object_unref(layout);
}
