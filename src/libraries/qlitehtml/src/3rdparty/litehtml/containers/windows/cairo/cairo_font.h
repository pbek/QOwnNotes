#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <mlang.h>
#include <vector>
#include <cairo.h>
#include <cairo-win32.h>
#include <litehtml.h>

struct linked_font
{
	typedef std::vector<linked_font*> vector;

	DWORD			   code_pages;
	HFONT			   hFont;
	cairo_font_face_t* font_face;
};

struct text_chunk
{
	typedef std::vector<text_chunk*> vector;

	char*		 text;
	linked_font* font;

	~text_chunk()
	{
		if(text)
		{
			delete text;
		}
	}
};

struct cairo_font_metrics
{
	int height;
	int ascent;
	int descent;
	int x_height;
	int ch_width;
};

class cairo_font
{
	HFONT				m_hFont;
	cairo_font_face_t*	m_font_face;
	IMLangFontLink2*	m_font_link;
	DWORD				m_font_code_pages;
	linked_font::vector m_linked_fonts;
	int					m_size;
	BOOL				m_bUnderline;
	BOOL				m_bStrikeOut;
	cairo_font_metrics	m_metrics;

  public:
	// fonts are not thread safe :(
	// you have to declare and initialize cairo_font::m_sync before the first using.
	static CRITICAL_SECTION m_sync;

	cairo_font(IMLangFontLink2* fl, HFONT hFont, int size);
	cairo_font(IMLangFontLink2* fl, LPCWSTR facename, int size, int weight, BOOL italic, BOOL strikeout,
			   BOOL underline);

	void init();
	~cairo_font();

	void				show_text(cairo_t* cr, int x, int y, const char*);
	double				text_width(cairo_t* cr, const char* str);
	void				load_metrics(cairo_t* cr);
	cairo_font_metrics& metrics();
	static std::wstring utf8_to_wchar(const std::string& src);
	static std::string	wchar_to_utf8(const std::wstring& src);

  private:
	void			   split_text(const char* str, text_chunk::vector& chunks);
	void			   free_text_chunks(text_chunk::vector& chunks);
	cairo_font_face_t* create_font_face(HFONT fnt);
	void			   set_font(HFONT hFont);
	void			   clear();
	double			   text_width(cairo_t* cr, text_chunk::vector& chunks);
	void			   lock();
	void			   unlock();
	int				   round_d(double val);
	void			   get_metrics(cairo_t* cr, cairo_font_metrics* fm);
};

inline void cairo_font::lock()
{
	EnterCriticalSection(&m_sync);
}

inline void cairo_font::unlock()
{
	LeaveCriticalSection(&m_sync);
}

inline int cairo_font::round_d(double val)
{
	int int_val = (int) val;
	if(val - int_val >= 0.5)
	{
		int_val++;
	}
	return int_val;
}

inline cairo_font_metrics& cairo_font::metrics()
{
	return m_metrics;
}

inline void cairo_font::load_metrics(cairo_t* cr)
{
	get_metrics(cr, &m_metrics);
}
