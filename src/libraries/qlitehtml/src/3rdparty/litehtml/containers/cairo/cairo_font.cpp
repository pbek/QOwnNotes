#include "cairo_font.h"

cairo_font::cairo_font(IMLangFontLink2* fl, HFONT hFont, int size )
{
	init();
	m_font_link = fl;
	if(m_font_link)
	{
		m_font_link->AddRef();
	}
	m_size = size;
	set_font(hFont);
}

cairo_font::cairo_font(IMLangFontLink2* fl, LPCWSTR facename, int size, int weight, BOOL italic, BOOL strikeout, BOOL underline )
{
	init();
	m_size = size;
	m_font_link = fl;
	if(m_font_link)
	{
		m_font_link->AddRef();
	}

	LOGFONT lf;
	ZeroMemory(&lf, sizeof(lf));
	if(!lstrcmpi(facename, L"monospace"))
	{
		wcscpy_s(lf.lfFaceName, LF_FACESIZE, L"Courier New");
	} else if(!lstrcmpi(facename, L"serif"))
	{
		wcscpy_s(lf.lfFaceName, LF_FACESIZE, L"Times New Roman");
	} else if(!lstrcmpi(facename, L"sans-serif"))
	{
		wcscpy_s(lf.lfFaceName, LF_FACESIZE, L"Arial");
	} else if(!lstrcmpi(facename, L"fantasy"))
	{
		wcscpy_s(lf.lfFaceName, LF_FACESIZE, L"Impact");
	} else if(!lstrcmpi(facename, L"cursive"))
	{
		wcscpy_s(lf.lfFaceName, LF_FACESIZE, L"Comic Sans MS");
	} else
	{
		wcscpy_s(lf.lfFaceName, LF_FACESIZE, facename);
	}

	lf.lfHeight			= -size;
	lf.lfWeight			= weight;
	lf.lfItalic			= italic;
	lf.lfCharSet		= DEFAULT_CHARSET;
	lf.lfOutPrecision	= OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
	lf.lfQuality		= DEFAULT_QUALITY;
	lf.lfStrikeOut		= strikeout;
	lf.lfUnderline		= underline;

	HFONT fnt = CreateFontIndirect(&lf);
	set_font(fnt);
}

cairo_font::~cairo_font()
{
	if(m_font_face)
	{
		cairo_font_face_destroy(m_font_face);
	}
	for(size_t i = 0; i < m_linked_fonts.size(); i++)
	{
		if(m_linked_fonts[i]->hFont)
		{
			m_font_link->ReleaseFont(m_linked_fonts[i]->hFont);
		}
		if(m_linked_fonts[i]->font_face)
		{
			cairo_font_face_destroy(m_linked_fonts[i]->font_face);
		}
	}
	m_linked_fonts.clear();
	if(m_font_link)
	{
		m_font_link->AddRef();
	}
	if(m_hFont)
	{
		DeleteObject(m_hFont);
	}
}

void cairo_font::show_text( cairo_t* cr, int x, int y, const litehtml::tchar_t* str )
{
	lock();
	text_chunk::vector chunks;
	split_text(str, chunks);
	cairo_set_font_size(cr, m_size);
	cairo_move_to(cr, x, y);
	for(size_t i = 0; i < chunks.size(); i++)
	{
		if(chunks[i]->font)
		{
			cairo_set_font_face(cr, chunks[i]->font->font_face);
		} else
		{
			cairo_set_font_face(cr, m_font_face);
		}
		cairo_show_text(cr, chunks[i]->text);
	}
	unlock();

	if(m_bUnderline)
	{
		int tw = text_width(cr, chunks);

		lock();
		cairo_set_line_width(cr, 1);
		cairo_move_to(cr, x, y + 1.5);
		cairo_line_to(cr, x + tw, y + 1.5);
		cairo_stroke(cr);
		unlock();
	}
	if(m_bStrikeOut)
	{
		int tw = text_width(cr, chunks);

		cairo_font_metrics fm;
		get_metrics(cr, &fm);

		int ln_y = y - fm.x_height / 2;

		lock();
		cairo_set_line_width(cr, 1);
		cairo_move_to(cr, x, (double) ln_y - 0.5);
		cairo_line_to(cr, x + tw, (double) ln_y - 0.5);
		cairo_stroke(cr);
		unlock();
	}

	free_text_chunks(chunks);
}

void cairo_font::split_text( const litehtml::tchar_t* src, text_chunk::vector& chunks )
{
	wchar_t* str;
#ifdef LITEHTML_UTF8
	str = cairo_font::utf8_to_wchar(src);
	wchar_t* str_start = str;
#else
	str = (wchar_t*) src;
#endif

	int cch = lstrlen(str);

	HDC hdc = GetDC(NULL);
	SelectObject(hdc, m_hFont);
	HRESULT hr = S_OK;
	while(cch > 0)
	{
		DWORD dwActualCodePages;
		long cchActual;
		if(m_font_link)
		{
			hr = m_font_link->GetStrCodePages(str, cch, m_font_code_pages, &dwActualCodePages, &cchActual);
		} else
		{
			hr = S_FALSE;
		}
		
		if(hr != S_OK)
		{
			break;
		}
		
		text_chunk* chk = new text_chunk;

		int sz = WideCharToMultiByte(CP_UTF8, 0, str, cchActual, chk->text, 0, NULL, NULL) + 1;
		chk->text = new CHAR[sz];
		sz = WideCharToMultiByte(CP_UTF8, 0, str, cchActual, chk->text, sz, NULL, NULL);
		chk->text[sz] = 0;
		chk->font = NULL;

		if(!(dwActualCodePages & m_font_code_pages))
		{
			for(linked_font::vector::iterator i = m_linked_fonts.begin(); i != m_linked_fonts.end(); i++)
			{
				if((*i)->code_pages == dwActualCodePages)
				{
					chk->font = (*i);
					break;
				}
			}
			if(!chk->font)
			{
				linked_font* lkf = new linked_font;
				lkf->code_pages	= dwActualCodePages;
				lkf->hFont		= NULL;
				m_font_link->MapFont(hdc, dwActualCodePages, 0, &lkf->hFont);
				if (lkf->hFont)
				{
					lkf->font_face = create_font_face(lkf->hFont);
					m_linked_fonts.push_back(lkf);
				}
				else
				{
					delete lkf;
					lkf = NULL;
				}
				chk->font = lkf;
			}
		}

		chunks.push_back(chk);

		cch -= cchActual;
		str += cchActual;
	}

	if(hr != S_OK)
	{
		text_chunk* chk = new text_chunk;

		int sz = WideCharToMultiByte(CP_UTF8, 0, str, -1, chk->text, 0, NULL, NULL) + 1;
		chk->text = new CHAR[sz];
		sz = WideCharToMultiByte(CP_UTF8, 0, str, -1, chk->text, sz, NULL, NULL);
		chk->text[sz] = 0;
		chk->font = NULL;
		chunks.push_back(chk);
	}

	ReleaseDC(NULL, hdc);

#ifdef LITEHTML_UTF8
	delete str_start;
#endif
}

void cairo_font::free_text_chunks( text_chunk::vector& chunks )
{
	for(size_t i = 0; i < chunks.size(); i++)
	{
		delete chunks[i];
	}
	chunks.clear();
}

cairo_font_face_t* cairo_font::create_font_face( HFONT fnt )
{
	LOGFONT lf;
	GetObject(fnt, sizeof(LOGFONT), &lf);
	return cairo_win32_font_face_create_for_logfontw(&lf);
}

int cairo_font::text_width( cairo_t* cr, const litehtml::tchar_t* str )
{
	text_chunk::vector chunks;
	split_text(str, chunks);

	int ret = text_width(cr, chunks);

	free_text_chunks(chunks);

	return (int) ret;
}

int cairo_font::text_width( cairo_t* cr, text_chunk::vector& chunks )
{
	lock();
	cairo_set_font_size(cr, m_size);
	double ret = 0;
	for(size_t i = 0; i < chunks.size(); i++)
	{
		if(chunks[i]->font)
		{
			cairo_set_font_face(cr, chunks[i]->font->font_face);
		} else
		{
			cairo_set_font_face(cr, m_font_face);
		}
		cairo_text_extents_t ext;
		cairo_text_extents(cr, chunks[i]->text, &ext);
		ret += ext.x_advance;
	}
	unlock();

	return (int) ret;
}

void cairo_font::get_metrics(cairo_t* cr, cairo_font_metrics* fm )
{
	lock();
	cairo_set_font_face(cr, m_font_face);
	cairo_set_font_size(cr, m_size);
	cairo_font_extents_t ext;
	cairo_font_extents(cr, &ext);

	cairo_text_extents_t tex;
	cairo_text_extents(cr, "x", &tex);

	fm->ascent		= (int) ext.ascent;
	fm->descent		= (int) ext.descent;
	fm->height		= (int) (ext.ascent + ext.descent);
	fm->x_height	= (int) tex.height;
	unlock();
}

void cairo_font::set_font( HFONT hFont )
{
	clear();

	m_hFont				= hFont;
	m_font_face			= create_font_face(m_hFont);
	m_font_code_pages	= 0;
	if(m_font_link)
	{
		HDC hdc = GetDC(NULL);
		SelectObject(hdc, m_hFont);
		m_font_link->GetFontCodePages(hdc, m_hFont, &m_font_code_pages);
		ReleaseDC(NULL, hdc);
	}
	LOGFONT lf;
	GetObject(m_hFont, sizeof(LOGFONT), &lf);
	m_bUnderline	= lf.lfUnderline;
	m_bStrikeOut	= lf.lfStrikeOut;
}

void cairo_font::clear()
{
	if(m_font_face)
	{
		cairo_font_face_destroy(m_font_face);
		m_font_face = NULL;
	}
	for(size_t i = 0; i < m_linked_fonts.size(); i++)
	{
		if(m_linked_fonts[i]->hFont && m_font_link)
		{
			m_font_link->ReleaseFont(m_linked_fonts[i]->hFont);
		}
		if(m_linked_fonts[i]->font_face)
		{
			cairo_font_face_destroy(m_linked_fonts[i]->font_face);
		}
	}
	m_linked_fonts.clear();
	if(m_hFont)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}
}

void cairo_font::init()
{
	m_hFont				= NULL;
	m_font_face			= NULL;
	m_font_link			= NULL;
	m_font_code_pages	= 0;
	m_size				= 0;
	m_bUnderline		= FALSE;
	m_bStrikeOut		= FALSE;
}

wchar_t* cairo_font::utf8_to_wchar( const char* src )
{
	if(!src) return NULL;

	int len = (int) strlen(src);
	wchar_t* ret = new wchar_t[len + 1];
	MultiByteToWideChar(CP_UTF8, 0, src, -1, ret, len + 1);
	return ret;
}

char* cairo_font::wchar_to_utf8( const wchar_t* src )
{
	if(!src) return NULL;

	int len = WideCharToMultiByte(CP_UTF8, 0, src, -1, NULL, 0, NULL, NULL);
	char* ret = new char[len];
	WideCharToMultiByte(CP_UTF8, 0, src, -1, ret, len, NULL, NULL);
	return ret;
}