#include "windows_container.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "cairo_font.h"
#include <strsafe.h>

windows_container::windows_container(void)
{
	m_temp_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 2, 2);
	m_temp_cr	   = cairo_create(m_temp_surface);
	m_font_link	   = NULL;
	CoCreateInstance(CLSID_CMultiLanguage, NULL, CLSCTX_ALL, IID_IMLangFontLink2, (void**) &m_font_link);
}

windows_container::~windows_container(void)
{
	clear_images();
	if(m_font_link)
	{
		m_font_link->Release();
	}
	cairo_surface_destroy(m_temp_surface);
	cairo_destroy(m_temp_cr);
}

litehtml::uint_ptr windows_container::create_font(const litehtml::font_description& descr,
												  const litehtml::document* doc, litehtml::font_metrics* fm)
{
	std::wstring fnt_name = L"sans-serif";

	litehtml::string_vector fonts;
	litehtml::split_string(descr.family, fonts, ",");
	if(!fonts.empty())
	{
		litehtml::trim(fonts[0]);
		fnt_name = cairo_font::utf8_to_wchar(fonts[0]);
		if(fnt_name.front() == L'"' || fnt_name.front() == L'\'')
		{
			fnt_name.erase(0, 1);
		}
		if(fnt_name.back() == L'"' || fnt_name.back() == L'\'')
		{
			fnt_name.erase(fnt_name.length() - 1, 1);
		}
	}

	cairo_font* fnt =
		new cairo_font(m_font_link, fnt_name.c_str(), descr.size, descr.weight,
					   (descr.style == litehtml::font_style_italic) ? TRUE : FALSE,
					   (descr.decoration_line & litehtml::text_decoration_line_line_through) ? TRUE : FALSE,
					   (descr.decoration_line & litehtml::text_decoration_line_underline) ? TRUE : FALSE);

	cairo_save(m_temp_cr);
	fnt->load_metrics(m_temp_cr);

	if(fm)
	{
		fm->font_size = descr.size;
		fm->ascent	  = fnt->metrics().ascent;
		fm->descent	  = fnt->metrics().descent;
		fm->height	  = fnt->metrics().height;
		fm->x_height  = fnt->metrics().x_height;
		fm->ch_width  = fnt->metrics().ch_width;
		if(descr.style == litehtml::font_style_italic || descr.decoration_line != litehtml::text_decoration_line_none)
		{
			fm->draw_spaces = true;
		} else
		{
			fm->draw_spaces = false;
		}
		fm->sub_shift	= descr.size / 5;
		fm->super_shift = descr.size / 3;
	}

	cairo_restore(m_temp_cr);

	return (litehtml::uint_ptr) fnt;
}

void windows_container::delete_font(litehtml::uint_ptr hFont)
{
	cairo_font* fnt = (cairo_font*) hFont;
	if(fnt)
	{
		delete fnt;
	}
}

litehtml::pixel_t windows_container::text_width(const char* text, litehtml::uint_ptr hFont)
{
	cairo_font* fnt = (cairo_font*) hFont;

	cairo_save(m_temp_cr);
	double ret = fnt->text_width(m_temp_cr, text);
	cairo_restore(m_temp_cr);
	return (litehtml::pixel_t) ret;
}

void windows_container::draw_text(litehtml::uint_ptr hdc, const char* text, litehtml::uint_ptr hFont,
								  litehtml::web_color color, const litehtml::position& pos)
{
	if(hFont)
	{
		cairo_font* fnt = (cairo_font*) hFont;
		cairo_t*	cr	= (cairo_t*) hdc;
		cairo_save(cr);

		apply_clip(cr);

		int x = pos.left();
		int y = pos.bottom() - fnt->metrics().descent;

		set_color(cr, color);
		fnt->show_text(cr, x, y, text);

		cairo_restore(cr);
	}
}

litehtml::string windows_container::resolve_color(const litehtml::string& color) const
{
	struct custom_color
	{
		const char* name;
		int			color_index;
	};

	static custom_color colors[] = {
		{"ActiveBorder",		 COLOR_ACTIVEBORDER	   },
		{"ActiveCaption",		  COLOR_ACTIVECAPTION		 },
		{"AppWorkspace",		 COLOR_APPWORKSPACE	   },
		{"Background",		   COLOR_BACKGROUND		   },
		{"ButtonFace",		   COLOR_BTNFACE			},
		{"ButtonHighlight",		COLOR_BTNHIGHLIGHT		  },
		{"ButtonShadow",		 COLOR_BTNSHADOW			},
		{"ButtonText",		   COLOR_BTNTEXT			},
		{"CaptionText",			COLOR_CAPTIONTEXT		 },
		{"GrayText",			 COLOR_GRAYTEXT		   },
		{"Highlight",			  COLOR_HIGHLIGHT			 },
		{"HighlightText",		  COLOR_HIGHLIGHTTEXT		 },
		{"InactiveBorder",	   COLOR_INACTIVEBORDER	   },
		{"InactiveCaption",		COLOR_INACTIVECAPTION	 },
		{"InactiveCaptionText", COLOR_INACTIVECAPTIONTEXT},
		{"InfoBackground",	   COLOR_INFOBK			   },
		{"InfoText",			 COLOR_INFOTEXT		   },
		{"Menu",				 COLOR_MENU			   },
		{"MenuText",			 COLOR_MENUTEXT		   },
		{"Scrollbar",			  COLOR_SCROLLBAR			 },
		{"ThreeDDarkShadow",	 COLOR_3DDKSHADOW		 },
		{"ThreeDFace",		   COLOR_3DFACE			   },
		{"ThreeDHighlight",		COLOR_3DHILIGHT		   },
		{"ThreeDLightShadow",	  COLOR_3DLIGHT			   },
		{"ThreeDShadow",		 COLOR_3DSHADOW		   },
		{"Window",			   COLOR_WINDOW			   },
		{"WindowFrame",			COLOR_WINDOWFRAME		 },
		{"WindowText",		   COLOR_WINDOWTEXT		   }
	};

	for(auto& clr : colors)
	{
		if(!litehtml::t_strcasecmp(clr.name, color.c_str()))
		{
			char  str_clr[20];
			DWORD rgb_color = GetSysColor(clr.color_index);
			StringCchPrintfA(str_clr, 20, "#%02X%02X%02X", GetRValue(rgb_color), GetGValue(rgb_color),
							 GetBValue(rgb_color));
			return std::move(litehtml::string(str_clr));
		}
	}
	return std::move(litehtml::string());
}

cairo_surface_t* windows_container::get_image(const std::string& url)
{
	return nullptr;
}

double windows_container::get_screen_dpi() const
{
	HDC hdc = GetDC(NULL);
	int ret = GetDeviceCaps(hdc, LOGPIXELSX);
	ReleaseDC(NULL, hdc);
	return ret;
}

int windows_container::get_screen_width() const
{
	HDC hdc = GetDC(NULL);
	int ret = GetDeviceCaps(hdc, HORZRES);
	ReleaseDC(NULL, hdc);
	return ret;
}
int windows_container::get_screen_height() const
{
	HDC hdc = GetDC(NULL);
	int ret = GetDeviceCaps(hdc, VERTRES);
	ReleaseDC(NULL, hdc);
	return ret;
}
