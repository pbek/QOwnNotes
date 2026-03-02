#include "win32_container.h"
#include "litehtml/os_types.h"

using namespace std;
using namespace litehtml;

static wstring utf8_to_utf16(string str)
{
	utf8_to_utf32 utf32 = str;
	wstring		  wstr;
	for(const char32_t* ptr = utf32; *ptr; ptr++)
	{
		char32_t ch = *ptr;
		if(ch <= 0xFFFF)
			wstr += (wchar_t) ch;
		// high surrogate goes first in UTF-16LE
		else
			wstr += {(wchar_t) (((ch - 0x10000) >> 10) + 0xD800), (wchar_t) ((ch & 0x3FF) + 0xDC00)};
	}
	return wstr;
}

static string utf16_to_utf8(wstring str)
{
	u32string ustr;
	for(size_t i = 0; i < str.size(); i++)
	{
		char16_t ch = str[i];
		if(ch >= 0xD800 && ch < 0xDC00)
		{
			char16_t ch2 = str[i + 1];
			if(ch2 >= 0xDC00 && ch2 < 0xE000)
			{
				ustr += 0x10000 + (ch2 - 0xDC00) + (ch - 0xD800) * 0x400;
				i++;
				continue;
			}
		}
		ustr += ch;
	}
	return utf32_to_utf8(ustr).c_str();
}

win32_container::win32_container()
{
	m_hClipRgn = NULL;
	m_tmp_hdc  = GetDC(NULL);
	InitializeCriticalSection(&m_img_sync);

	EnumFonts(m_tmp_hdc, NULL, EnumFontsProc, (LPARAM) this);
	m_installed_fonts.insert(L"monospace");
	m_installed_fonts.insert(L"serif");
	m_installed_fonts.insert(L"sans-serif");
	m_installed_fonts.insert(L"fantasy");
	m_installed_fonts.insert(L"cursive");
}

win32_container::~win32_container()
{
	DeleteCriticalSection(&m_img_sync);
	if(m_hClipRgn)
	{
		DeleteObject(m_hClipRgn);
	}
	ReleaseDC(NULL, m_tmp_hdc);
}

int CALLBACK win32_container::EnumFontsProc(const LOGFONT* lplf, const TEXTMETRIC* lptm, DWORD dwType, LPARAM lpData)
{
	win32_container* container = (win32_container*) lpData;
	container->m_installed_fonts.insert(lplf->lfFaceName);
	return 1;
}

static LPCWSTR get_exact_font_name(LPCWSTR facename)
{
	if(!lstrcmpi(facename, L"monospace"))
		return L"Courier New";
	else if(!lstrcmpi(facename, L"serif"))
		return L"Times New Roman";
	else if(!lstrcmpi(facename, L"sans-serif"))
		return L"Arial";
	else if(!lstrcmpi(facename, L"fantasy"))
		return L"Impact";
	else if(!lstrcmpi(facename, L"cursive"))
		return L"Comic Sans MS";
	else
		return facename;
}

static void trim_quotes(string& str)
{
	if(str.front() == '"' || str.front() == '\'')
		str.erase(0, 1);

	if(str.back() == '"' || str.back() == '\'')
		str.erase(str.length() - 1, 1);
}

uint_ptr win32_container::create_font(const litehtml::font_description& descr, const litehtml::document* doc,
									  litehtml::font_metrics* fm)
{
	wstring		  font_name;
	string_vector fonts;
	split_string(descr.family, fonts, ",");
	bool found = false;
	for(auto& name : fonts)
	{
		trim(name);
		trim_quotes(name);
		wstring wname = utf8_to_utf16(name);
		if(m_installed_fonts.count(wname))
		{
			font_name = wname;
			found	  = true;
			break;
		}
	}
	if(!found)
		font_name = utf8_to_utf16(get_default_font_name());
	font_name  = get_exact_font_name(font_name.c_str());

	LOGFONT lf = {};
	wcscpy_s(lf.lfFaceName, LF_FACESIZE, font_name.c_str());

	lf.lfHeight		   = -descr.size;
	lf.lfWeight		   = descr.weight;
	lf.lfItalic		   = descr.style == font_style_italic;
	lf.lfCharSet	   = DEFAULT_CHARSET;
	lf.lfOutPrecision  = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality	   = DEFAULT_QUALITY;
	lf.lfStrikeOut	   = (descr.decoration_line & text_decoration_line_line_through) != 0;
	lf.lfUnderline	   = (descr.decoration_line & text_decoration_line_underline) != 0;
	HFONT hFont		   = CreateFontIndirect(&lf);

	if(fm)
	{
		SelectObject(m_tmp_hdc, hFont);
		TEXTMETRIC tm = {};
		GetTextMetrics(m_tmp_hdc, &tm);
		fm->font_size	= descr.size;
		fm->ascent		= tm.tmAscent;
		fm->descent		= tm.tmDescent;
		fm->height		= tm.tmHeight;
		fm->x_height	= tm.tmHeight / 2; // this is an estimate; call GetGlyphOutline to get the real value
		fm->ch_width	= tm.tmAveCharWidth;
		fm->draw_spaces = descr.style == font_style_italic || descr.decoration_line != text_decoration_line_none;
		fm->sub_shift	= descr.size / 5;
		fm->super_shift = descr.size / 3;
	}

	return (uint_ptr) hFont;
}

void win32_container::delete_font(uint_ptr hFont)
{
	DeleteObject((HFONT) hFont);
}

const char* win32_container::get_default_font_name() const
{
	return "Times New Roman";
}

litehtml::pixel_t win32_container::get_default_font_size() const
{
	return 16;
}

litehtml::pixel_t win32_container::text_width(const char* text, uint_ptr hFont)
{
	SIZE size = {};
	SelectObject(m_tmp_hdc, (HFONT) hFont);
	wstring wtext = utf8_to_utf16(text);
	GetTextExtentPoint32(m_tmp_hdc, wtext.c_str(), (int) wtext.size(), &size);
	return size.cx;
}

void win32_container::draw_text(uint_ptr hdc, const char* text, uint_ptr hFont, web_color color, const position& pos)
{
	apply_clip((HDC) hdc);

	HFONT oldFont = (HFONT) SelectObject((HDC) hdc, (HFONT) hFont);

	SetBkMode((HDC) hdc, TRANSPARENT);

	SetTextColor((HDC) hdc, RGB(color.red, color.green, color.blue));

	RECT rcText = {pos.left(), pos.top(), pos.right(), pos.bottom()};
	DrawText((HDC) hdc, utf8_to_utf16(text).c_str(), -1, &rcText, DT_SINGLELINE | DT_NOPREFIX | DT_BOTTOM | DT_NOCLIP);

	SelectObject((HDC) hdc, oldFont);

	release_clip((HDC) hdc);
}

litehtml::pixel_t win32_container::pt_to_px(float pt) const
{
	return MulDiv(pt, GetDeviceCaps(m_tmp_hdc, LOGPIXELSY), 72);
}

void win32_container::draw_list_marker(uint_ptr hdc, const list_marker& marker)
{
	apply_clip((HDC) hdc);

	litehtml::pixel_t top_margin = marker.pos.height / 3;
	if(top_margin < 4)
		top_margin = 0;

	litehtml::pixel_t draw_x	  = marker.pos.x;
	litehtml::pixel_t draw_y	  = marker.pos.y + top_margin;
	litehtml::pixel_t draw_width  = marker.pos.height - top_margin * 2;
	litehtml::pixel_t draw_height = marker.pos.height - top_margin * 2;

	switch(marker.marker_type)
	{
	case list_style_type_circle:
		{
			draw_ellipse((HDC) hdc, draw_x, draw_y, draw_width, draw_height, marker.color, 1);
		}
		break;
	case list_style_type_disc:
		{
			fill_ellipse((HDC) hdc, draw_x, draw_y, draw_width, draw_height, marker.color);
		}
		break;
	case list_style_type_square:
		{
			fill_rect((HDC) hdc, draw_x, draw_y, draw_width, draw_height, marker.color);
		}
		break;
	}
	release_clip((HDC) hdc);
}

void win32_container::make_url_utf8(const char* url, const char* basepath, wstring& out)
{
	if(!basepath)
		basepath = "";
	make_url(utf8_to_utf16(url).c_str(), utf8_to_utf16(basepath).c_str(), out);
}

void win32_container::load_image(const char* src, const char* baseurl, bool redraw_on_ready)
{
	wstring url;
	make_url_utf8(src, baseurl, url);

	lock_images_cache();
	if(m_images.count(url) == 0)
	{
		unlock_images_cache();
		uint_ptr img = get_image(url.c_str(), redraw_on_ready);
		add_image(url.c_str(), img);
	} else
	{
		unlock_images_cache();
	}
}

void win32_container::add_image(LPCWSTR url, uint_ptr img)
{
	lock_images_cache();
	m_images[url] = img;
	unlock_images_cache();
}

void win32_container::get_image_size(const char* src, const char* baseurl, litehtml::size& sz)
{
	wstring url;
	make_url_utf8(src, baseurl, url);

	sz.width  = 0;
	sz.height = 0;

	lock_images_cache();
	images_map::iterator img = m_images.find(url);
	if(img != m_images.end() && img->second)
	{
		get_img_size(img->second, sz);
	}
	unlock_images_cache();
}

void win32_container::clear_images()
{
	lock_images_cache();
	for(auto& img : m_images)
	{
		if(img.second)
		{
			free_image(img.second);
		}
	}
	m_images.clear();
	unlock_images_cache();
}

void win32_container::lock_images_cache()
{
	EnterCriticalSection(&m_img_sync);
}

void win32_container::unlock_images_cache()
{
	LeaveCriticalSection(&m_img_sync);
}

void win32_container::draw_solid_fill(uint_ptr _hdc, const background_layer& layer, const web_color& color)
{
	HDC hdc = (HDC) _hdc;
	apply_clip(hdc);

	auto border_box = layer.border_box;
	fill_rect(hdc, border_box.x, border_box.y, border_box.width, border_box.height, color);

	release_clip(hdc);
}

void win32_container::draw_image(uint_ptr _hdc, const background_layer& layer, const string& url,
								 const string& base_url)
{
	HDC hdc = (HDC) _hdc;
	apply_clip(hdc);

	wstring wurl;
	make_url_utf8(url.c_str(), base_url.c_str(), wurl);

	lock_images_cache();
	images_map::iterator img = m_images.find(wurl);
	if(img != m_images.end() && img->second)
	{
		draw_img_bg(hdc, img->second, layer);
	}
	unlock_images_cache();

	release_clip(hdc);
}

void win32_container::set_clip(const position& pos, const border_radiuses& bdr_radius)
{
	m_clips.push_back(pos);
}

void win32_container::del_clip()
{
	if(!m_clips.empty())
	{
		m_clips.pop_back();
	}
}

void win32_container::apply_clip(HDC hdc)
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

		position clip_pos = m_clips.back();
		m_hClipRgn = CreateRectRgn(clip_pos.left() - ptView.x, clip_pos.top() - ptView.y, clip_pos.right() - ptView.x,
								   clip_pos.bottom() - ptView.y);
		SelectClipRgn(hdc, m_hClipRgn);
	}
}

void win32_container::release_clip(HDC hdc)
{
	SelectClipRgn(hdc, NULL);

	if(m_hClipRgn)
	{
		DeleteObject(m_hClipRgn);
		m_hClipRgn = NULL;
	}
}

element::ptr win32_container::create_element(const char* tag_name, const string_map& attributes,
											 const document::ptr& doc)
{
	return 0;
}

void win32_container::get_media_features(media_features& media) const
{
	position client;
	get_viewport(client);

	media.type			= media_type_screen;
	media.width			= client.width;
	media.height		= client.height;
	media.color			= 8;
	media.monochrome	= 0;
	media.color_index	= 256;
	media.resolution	= GetDeviceCaps(m_tmp_hdc, LOGPIXELSX);
	media.device_width	= GetDeviceCaps(m_tmp_hdc, HORZRES);
	media.device_height = GetDeviceCaps(m_tmp_hdc, VERTRES);
}

void win32_container::get_language(string& language, string& culture) const
{
	language = "en";
	culture	 = "";
}

void win32_container::transform_text(string& text, text_transform tt)
{
	if(text.empty())
		return;

	LPWSTR txt = _wcsdup(utf8_to_utf16(text).c_str());
	switch(tt)
	{
	case text_transform_capitalize:
		CharUpperBuff(txt, 1);
		break;
	case text_transform_uppercase:
		CharUpperBuff(txt, lstrlen(txt));
		break;
	case text_transform_lowercase:
		CharLowerBuff(txt, lstrlen(txt));
		break;
	}
	text = utf16_to_utf8(txt);
	free(txt);
}

void win32_container::link(const document::ptr& doc, const element::ptr& el) {}

string win32_container::resolve_color(const string& color) const
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
		if(!t_strcasecmp(color.c_str(), clr.name))
		{
			char  str_clr[20];
			DWORD rgb_color = GetSysColor(clr.color_index);
			t_snprintf(str_clr, 20, "#%02X%02X%02X", GetRValue(rgb_color), GetGValue(rgb_color), GetBValue(rgb_color));
			return std::move(string(str_clr));
		}
	}
	return std::move(string());
}
