#include <litehtml.h>
#include "win32_container.h"


litehtml::win32_container::win32_container()
{
	m_hClipRgn = NULL;
}

litehtml::win32_container::~win32_container()
{
	if(m_hClipRgn)
	{
		DeleteObject(m_hClipRgn);
	}
}

litehtml::uint_ptr litehtml::win32_container::create_font( const wchar_t* faceName, int size, int weight, font_style italic, unsigned int decoration )
{
	litehtml::string_vector fonts;
	tokenize(faceName, fonts, L",");
	litehtml::trim(fonts[0]);

	LOGFONT lf;
	ZeroMemory(&lf, sizeof(lf));
	wcscpy_s(lf.lfFaceName, LF_FACESIZE, fonts[0].c_str());

	lf.lfHeight			= -size;
	lf.lfWeight			= weight;
	lf.lfItalic			= (italic == litehtml::fontStyleItalic) ? TRUE : FALSE;
	lf.lfCharSet		= DEFAULT_CHARSET;
	lf.lfOutPrecision	= OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
	lf.lfQuality		= DEFAULT_QUALITY;
	lf.lfStrikeOut		= (decoration & litehtml::font_decoration_linethrough) ? TRUE : FALSE;
	lf.lfUnderline		= (decoration & litehtml::font_decoration_underline) ? TRUE : FALSE;
	HFONT hFont = CreateFontIndirect(&lf);

	return (uint_ptr) hFont;
}

void litehtml::win32_container::delete_font( uint_ptr hFont )
{
	DeleteObject((HFONT) hFont);
}

int litehtml::win32_container::line_height( uint_ptr hdc, uint_ptr hFont )
{
	HFONT oldFont = (HFONT) SelectObject((HDC) hdc, (HFONT) hFont);
	TEXTMETRIC tm;
	GetTextMetrics((HDC) hdc, &tm);
	SelectObject((HDC) hdc, oldFont);
	return (int) tm.tmHeight;
}

int litehtml::win32_container::text_width( uint_ptr hdc, const wchar_t* text, uint_ptr hFont )
{
	HFONT oldFont = (HFONT) SelectObject((HDC) hdc, (HFONT) hFont);

	SIZE sz = {0, 0};

	GetTextExtentPoint32((HDC) hdc, text, lstrlen(text), &sz);

	SelectObject((HDC) hdc, oldFont);

	return (int) sz.cx;
}

void litehtml::win32_container::draw_text( uint_ptr hdc, const wchar_t* text, uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos )
{
	apply_clip((HDC) hdc);

	HFONT oldFont = (HFONT) SelectObject((HDC) hdc, (HFONT) hFont);

	SetBkMode((HDC) hdc, TRANSPARENT);

	SetTextColor((HDC) hdc, RGB(color.red, color.green, color.blue));

	RECT rcText = { pos.left(), pos.top(), pos.right(), pos.bottom() };
	DrawText((HDC) hdc, text, -1, &rcText, DT_SINGLELINE | DT_NOPREFIX | DT_BOTTOM | DT_NOCLIP);

	SelectObject((HDC) hdc, oldFont);

	release_clip((HDC) hdc);
}

void litehtml::win32_container::fill_rect( uint_ptr hdc, const litehtml::position& pos, const litehtml::web_color color, const litehtml::css_border_radius& radius )
{
	apply_clip((HDC) hdc);
	fill_rect((HDC) hdc, pos.x, pos.y, pos.width, pos.height, color, radius);
	release_clip((HDC) hdc);
}

litehtml::uint_ptr litehtml::win32_container::get_temp_dc()
{
	return (litehtml::uint_ptr) GetDC(NULL);
}

void litehtml::win32_container::release_temp_dc( uint_ptr hdc )
{
	ReleaseDC(NULL, (HDC) hdc);
}

int litehtml::win32_container::pt_to_px( int pt )
{
	HDC dc = GetDC(NULL);
	int ret = MulDiv(pt, GetDeviceCaps(dc, LOGPIXELSY), 72);
	ReleaseDC(NULL, dc);
	return ret;
}

int litehtml::win32_container::get_text_base_line( uint_ptr hdc, uint_ptr hFont )
{
	HDC dc = (HDC) hdc;
	if(!dc)
	{
		dc = GetDC(NULL);
	}
	HFONT oldFont = (HFONT) SelectObject(dc, (HFONT) hFont);
	TEXTMETRIC tm;
	GetTextMetrics(dc, &tm);
	SelectObject(dc, oldFont);
	if(!hdc)
	{
		ReleaseDC(NULL, dc);
	}
	return (int) tm.tmDescent;
}

void litehtml::win32_container::draw_list_marker(uint_ptr hdc, const litehtml::list_marker& marker)
{
	apply_clip((HDC)hdc);

	int top_margin = marker.pos.height / 3;
	if (top_margin < 4)
		top_margin = 0;

	int draw_x = marker.pos.x;
	int draw_y = marker.pos.y + top_margin;
	int draw_width = marker.pos.height - top_margin * 2;
	int draw_height = marker.pos.height - top_margin * 2;

	switch (marker.marker_type)
	{
	case list_style_type_circle:
		{
			draw_ellipse((HDC)hdc, draw_x, draw_y, draw_width, draw_height, marker.color, 1);
		}
		break;
	case list_style_type_disc:
		{
			fill_ellipse((HDC)hdc, draw_x, draw_y, draw_width, draw_height, marker.color);
		}
		break;
	case list_style_type_square:
		{
			fill_rect((HDC)hdc, draw_x, draw_y, draw_width, draw_height, marker.color, css_border_radius());
		}
		break;
	}
	release_clip((HDC)hdc);
}

void litehtml::win32_container::load_image( const wchar_t* src, const wchar_t* baseurl, bool redraw_on_ready )
{
	std::wstring url;
	make_url(src, baseurl, url);
	if(m_images.find(url.c_str()) == m_images.end())
	{
		uint_ptr img = get_image(url.c_str());
		if(img)
		{ 
			m_images[url.c_str()] = img;
		}
	}
}

void litehtml::win32_container::get_image_size( const wchar_t* src, const wchar_t* baseurl, litehtml::size& sz )
{
	std::wstring url;
	make_url(src, baseurl, url);

	images_map::iterator img = m_images.find(url.c_str());
	if(img != m_images.end())
	{
		get_img_size(img->second, sz);
	}
}

void litehtml::win32_container::draw_image( uint_ptr hdc, const wchar_t* src, const wchar_t* baseurl, const litehtml::position& pos )
{
	apply_clip((HDC) hdc);

	std::wstring url;
	make_url(src, baseurl, url);
	images_map::iterator img = m_images.find(url.c_str());
	if(img != m_images.end())
	{
		draw_img((HDC) hdc, img->second, pos);
	}

	release_clip((HDC) hdc);
}

void litehtml::win32_container::clear_images()
{
	for(images_map::iterator i = m_images.begin(); i != m_images.end(); i++)
	{
		if(i->second)
		{
			free_image(i->second);
		}
	}
	m_images.clear();
}

int litehtml::win32_container::get_default_font_size() const
{
	return 16;
}

void litehtml::win32_container::draw_background( uint_ptr hdc, const wchar_t* image, const wchar_t* baseurl, const litehtml::position& draw_pos, const litehtml::css_position& bg_pos, litehtml::background_repeat repeat, litehtml::background_attachment attachment )
{
	apply_clip((HDC) hdc);

	std::wstring url;
	make_url(image, baseurl, url);

	images_map::iterator img = m_images.find(url.c_str());
	if(img != m_images.end())
	{
		litehtml::size img_sz;
		get_img_size(img->second, img_sz);

		litehtml::position pos = draw_pos;

		if(bg_pos.x.units() != css_units_percentage)
		{
			pos.x += (int) bg_pos.x.val();
		} else
		{
			pos.x += (int) ((float) (draw_pos.width - img_sz.width) * bg_pos.x.val() / 100.0);
		}

		if(bg_pos.y.units() != css_units_percentage)
		{
			pos.y += (int) bg_pos.y.val();
		} else
		{
			pos.y += (int) ( (float) (draw_pos.height - img_sz.height) * bg_pos.y.val() / 100.0);
		}

		draw_img_bg((HDC) hdc, img->second, draw_pos, pos, repeat, attachment);
	}

	release_clip((HDC) hdc);
}

wchar_t litehtml::win32_container::toupper( const wchar_t c )
{
	return (wchar_t) CharUpper((LPWSTR) c);
}

wchar_t litehtml::win32_container::tolower( const wchar_t c )
{
	return (wchar_t) CharLower((LPWSTR) c);
}

void litehtml::win32_container::set_clip( const litehtml::position& pos, bool valid_x, bool valid_y )
{
	litehtml::position clip_pos = pos;
	litehtml::position client_pos;
	get_client_rect(client_pos);
	if(!valid_x)
	{
		clip_pos.x		= client_pos.x;
		clip_pos.width	= client_pos.width;
	}
	if(!valid_y)
	{
		clip_pos.y		= client_pos.y;
		clip_pos.height	= client_pos.height;
	}
	m_clips.push_back(clip_pos);
}

void litehtml::win32_container::del_clip()
{
	if(!m_clips.empty())
	{
		m_clips.pop_back();
		if(!m_clips.empty())
		{
			litehtml::position clip_pos = m_clips.back();
		}
	}
}

void litehtml::win32_container::apply_clip(HDC hdc)
{
	if(m_hClipRgn)
	{
		DeleteObject(m_hClipRgn);
		m_hClipRgn = NULL;
	}

	if(!m_clips.empty())
	{
		POINT ptView = {0, 0};
		GetWindowOrgEx(hdc, &ptView);

		litehtml::position clip_pos = m_clips.back();
		m_hClipRgn = CreateRectRgn(clip_pos.left() - ptView.x, clip_pos.top() - ptView.y, clip_pos.right() - ptView.x, clip_pos.bottom() - ptView.y);
		SelectClipRgn(hdc, m_hClipRgn);
	}
}

void litehtml::win32_container::release_clip(HDC hdc)
{
	SelectClipRgn(hdc, NULL);

	if(m_hClipRgn)
	{
		DeleteObject(m_hClipRgn);
		m_hClipRgn = NULL;
	}
}
