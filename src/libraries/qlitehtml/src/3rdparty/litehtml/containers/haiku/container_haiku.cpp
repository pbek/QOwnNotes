/*
 * Copyright 2019-2020 Haiku Inc.
 * All rights reserved. Distributed under the terms of the BSD 3-clause license.
 * Constributors
 * 2019-2020	Adam Fowler <adamfowleruk@gmail.com>
 */
#include "container_haiku.h"

#include <string>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <map>

#include <Bitmap.h>
#include <String.h>
#include <Entry.h>
#include <Path.h>
#include <TranslationUtils.h>

LiteHtmlView::LiteHtmlView(BRect frame, const char *name)
	: BView(frame, name, B_FOLLOW_ALL, B_WILL_DRAW),
	fContext(NULL),
	m_html(NULL),
	m_images(),
	m_base_url(),
	m_url()
{
	
	BRect bounds(Bounds());
	BPoint topLeft = bounds.LeftTop();
	std::cout << "Initial bounds: topLeft x: " << +topLeft.x << ", y: " 
		<< +topLeft.y << ", width: " << +bounds.Width() << ", height: " 
		<< +bounds.Height() << std::endl;
	
	SetDrawingMode(B_OP_OVER);
	SetFont(be_plain_font);
	
	//FillRect(bounds,B_SOLID_LOW);
	
	//SetLowColor(B_DOCUMENT_PANEL_COLOR);
	//FillRect(rect);
}

LiteHtmlView::~LiteHtmlView()
{
}

void
LiteHtmlView::SetContext(litehtml::context* ctx)
{
	fContext = ctx;
}

void
LiteHtmlView::RenderFile(const char* localFilePath)
{
	std::cout << "RenderFile" << std::endl;
	//BUrlRequest req;
	// assume a local file for now, that is HTML
	std::ifstream t(localFilePath);
	std::string html((std::istreambuf_iterator<char>(t)),
					 std::istreambuf_iterator<char>());
	
	//std::cout << "HTML output:-" << std::endl << html << std::endl;
	
	// Get parent folder for the base url
	std::cout << "Loaded from file: " << localFilePath << std::endl;
	BPath htmlPath(localFilePath);
	BPath dirPath;
	htmlPath.GetParent(&dirPath);
	std::cout << "parent path: " << dirPath.Path() << std::endl;
	set_base_url(dirPath.Path());
	//std::cout << "    base url now:" << m_base_url << std::endl;
	
	RenderHTML(html);
}

void
LiteHtmlView::RenderHTML(const std::string& htmlText)
{
	std::cout << "RenderHTML" << std::endl;
	
	// now use this string
	m_html = litehtml::document::createFromString(
		htmlText.c_str(), this, fContext);
	if (m_html) 
	{
		std::cout << "Successfully read html" << std::endl;
		// success
		// post-parse render operations, if required.
		Invalidate();
	} else {
		std::cout << "Failed to read html" << std::endl;
	}
	// always fire the rendering complete message
	std::cout << "Sending html rendered message: " << M_HTML_RENDERED << std::endl;
}

void
LiteHtmlView::Draw(BRect b)
{
	std::cout << "DRAW CALLED" << std::endl;
	
	BRect bounds(Bounds());
	FillRect(bounds,B_SOLID_LOW);
	
	// b only part of the window, but we need to draw the whole lot
	
	if (NULL != m_html) {
		BPoint leftTop = bounds.LeftTop();
		litehtml::position clip(leftTop.x,leftTop.y,
			bounds.Width(),bounds.Height());
		m_html->render(bounds.Width());
		m_html->draw((litehtml::uint_ptr) this,0,0,&clip);
	}
	SendNotices(M_HTML_RENDERED,new BMessage(M_HTML_RENDERED));
}

void
LiteHtmlView::GetPreferredSize(float* width,float* height)
{
	if (NULL == m_html)
	{
		BRect bounds(Bounds());
		*width = bounds.Width(); 
		*height = bounds.Height();
	} else {
		*width = m_html->width();
		*height = m_html->height();
	}
}

litehtml::uint_ptr 
LiteHtmlView::create_font( const litehtml::tchar_t* faceName, int size, 
	int weight, litehtml::font_style italic, unsigned int decoration, 
	litehtml::font_metrics* fm )
{
	//std::cout << "create_font" << std::endl;
	litehtml::string_vector fonts;
	litehtml::split_string(faceName, fonts, ",");
	litehtml::trim(fonts[0]);

	uint16 face = B_REGULAR_FACE; // default
	if (italic == litehtml::fontStyleItalic)
	{
		face |= B_ITALIC_FACE;
	}
	if (decoration & litehtml::font_decoration_underline)
	{
		face |= B_UNDERSCORE_FACE;
	}
	if (decoration & litehtml::font_decoration_linethrough)
	{
		face |= B_STRIKEOUT_FACE;
	}
	// Note: LIGHT, HEAVY, CONDENSED not supported in BeOS R5
#ifdef __HAIKU__
	if(weight >= 0 && weight < 150)			face |= B_LIGHT_FACE;
	else if(weight >= 150 && weight < 250)	face |= B_LIGHT_FACE;
	else if(weight >= 250 && weight < 350)	face |= B_LIGHT_FACE;
	//else if(weight >= 350 && weight < 450)	face |= B_REGULAR_FACE;
	//else if(weight >= 450 && weight < 550)	face |= B_REGULAR_FACE;
	else if(weight >= 550 && weight < 650)	face |= B_CONDENSED_FACE;
#else
	else if(weight >= 550 && weight < 650)	face |= B_BOLD_FACE;
#endif
	else if(weight >= 650 && weight < 750)	face |= B_BOLD_FACE;
#ifndef __HAIKU__
	else if(weight >= 750 && weight < 850)	face |= B_BOLD_FACE;
	else if(weight >= 950)					face |= B_BOLD_FACE;
#else
	else if(weight >= 750 && weight < 850)	face |= B_HEAVY_FACE;
	else if(weight >= 950)					face |= B_HEAVY_FACE;
#endif

	BFont* tempFont = new BFont();
	bool found = false;
	for(litehtml::string_vector::iterator i = fonts.begin(); 
		i != fonts.end(); i++)
	{
		if (B_OK == tempFont->SetFamilyAndFace(i->c_str(),face))
		{
			found = true;
			break;
		}
	}
	
	if (!found)
	{
		// default to the Be plain font
		tempFont = new BFont(be_plain_font);
		if (weight >= 550) 
		{
			tempFont = new BFont(be_bold_font);
		}
		tempFont->SetFace(face); // chooses closest
	}
	
	tempFont->SetSize(size);
	
	font_height hgt;
	tempFont->GetHeight(&hgt);
	fm->ascent = hgt.ascent;
	fm->descent = hgt.descent;
	fm->height = (int) (hgt.ascent + hgt.descent);
	fm->x_height = (int) hgt.leading;
	
	return (litehtml::uint_ptr) tempFont;
}

void 
LiteHtmlView::delete_font( litehtml::uint_ptr hFont )
{
	std::cout << "delete_font" << std::endl;
}

int 
LiteHtmlView::text_width( const litehtml::tchar_t* text, 
	litehtml::uint_ptr hFont )
{
	//std::cout << "text_width" << std::endl;
	BFont* fnt = (BFont*)hFont;
	int width = fnt->StringWidth(text);
	//std::cout << "    Width: " << +width << std::endl;
	return width;
}

void 
LiteHtmlView::draw_text( litehtml::uint_ptr hdc, const litehtml::tchar_t* text,
	litehtml::uint_ptr hFont, litehtml::web_color color, 
	const litehtml::position& pos )
{
	//std::cout << "draw_text" << std::endl;
	if (!text) return;
	if (0 == strlen(text)) return;
	BFont* fnt = (BFont*)hFont;
	
	//std::cout << "    left: " << +pos.left() << ", top: " << +pos.top() << std::endl;
	//std::cout << "    RGBA: " << +color.red << "," << +color.green << "," << +color.blue << "," << +color.alpha << std::endl;
	//std::cout << "    Font size: " << +fnt->Size() << std::endl;
	//std::cout << "    Text: " << text << std::endl;
	BRect bounds(Bounds());

	//FillRect(bounds,B_SOLID_LOW);
	
	BPoint leftTop = bounds.LeftTop();
	//std::cout << "    Bounds left: " << +leftTop.x << ", top: " << +leftTop.y << ", Width: " << +bounds.Width() << ", Height: " << +bounds.Height() << std::endl;
	
	font_height fh;
	fnt->GetHeight(&fh);
	int baseline = fh.ascent + fh.descent;// + 10;
	int leftbase = 0; //10;
	MovePenTo(pos.left() + leftbase,pos.top() + baseline);//leftTop.x,leftTop.y);
	SetFont(fnt);
	//SetFont(be_plain_font);
	rgb_color clr = ui_color(B_DOCUMENT_TEXT_COLOR);
	/*
	rgb_color clr;
	clr.blue = 40;
	clr.red = 40;
	clr.green = 40;
	*/
		
	clr.red = color.red;
	clr.green = color.green;
	clr.blue = color.blue;
	clr.alpha = color.alpha;
	
	//std::cout << "    Final RGBA: " << +clr.red << "," << +clr.green << "," << +clr.blue << "," << +clr.alpha << std::endl;
	SetHighColor(clr);
	SetLowColor(ui_color(B_DOCUMENT_BACKGROUND_COLOR));
	BString mystr("");
	//mystr << "text: ";
	mystr << text;
	DrawString(mystr);
}

int 
LiteHtmlView::pt_to_px( int pt )
{
	std::cout << "pt_to_px" << std::endl;
	return (int) ((double) pt * 1.3333333333);
}

int 
LiteHtmlView::get_default_font_size() const
{
	//std::cout << "get_default_font_size" << std::endl;
	return 12;
}

const litehtml::tchar_t* 
LiteHtmlView::get_default_font_name() const
{
	//std::cout << "get_default_font_name" << std::endl;
	font_family fam;
	font_style style;
	be_plain_font->GetFamilyAndStyle(&fam,&style);
	char* cp = strdup(fam);
	return (litehtml::tchar_t*)cp;
}

void 
LiteHtmlView::draw_list_marker( litehtml::uint_ptr hdc, 
	const litehtml::list_marker& marker )
{
	std::cout << "draw_list_marker" << std::endl;
	if (!marker.image.empty())
	{
		std::cout << "    image marker" << std::endl;
	}
}

void 
LiteHtmlView::load_image( const litehtml::tchar_t* src, 
	const litehtml::tchar_t* baseurl, bool redraw_on_ready )
{
	std::cout << "load_image" << std::endl;
	
	std::string url;
	make_url(src, baseurl, url);
	if(m_images.find(url.c_str()) == m_images.end())
	{
		BEntry entry(url.c_str(), true);
		if (entry.Exists()) {
			std::cout << "    Loading bitmap from file" << std::endl;
			BBitmap* img = BTranslationUtils::GetBitmap(url.c_str());
			m_images[url] = img;	
		}
	}
}

void
LiteHtmlView::make_url(const litehtml::tchar_t* url, 
	const litehtml::tchar_t* basepath, litehtml::tstring& out)
{
	std::cout << "make_url" << std::endl;
	std::cout << "    url: " << url << std::endl;
	if(!basepath || (basepath && !basepath[0]))
	{
		if(!m_base_url.empty())
		{
			//out = urljoin(m_base_url, std::string(url));
			std::string ns(m_base_url);
			ns += "/";
			ns += url;
			out = ns;
		} else
		{
			out = url;
		}
	} else
	{
		std::cout << "    basepath: " << basepath << std::endl;
		//out = urljoin(std::string(basepath), std::string(url));
		std::string ns(basepath);
		ns += "/";
		ns += url;
		out = ns;
	}
	std::cout << "    Output url: " << out << std::endl;
}

void
LiteHtmlView::set_base_url(const litehtml::tchar_t* base_url)
{
	std::cout << "set_base_url" << std::endl;
/*
	if(base_url)
	{
		m_base_url = urljoin(m_url, std::string(base_url));
	} else
	{
	*/
		m_base_url = base_url;
	std::cout << "    base url set to: " << m_base_url << std::endl;
	//}
}


void 
LiteHtmlView::get_image_size( const litehtml::tchar_t* src, 
	const litehtml::tchar_t* baseurl, litehtml::size& sz )
{
	std::cout << "get_image_size" << std::endl;
	std::string url;
	make_url(src,NULL,url);
	const auto& miter(m_images.find(url.c_str()));
	if (m_images.end() != miter)
	{
		BBitmap* img = (BBitmap*)miter->second;
		BRect size = img->Bounds();
		sz.width = size.Width();
		sz.height = size.Height();
		std::cout << "    width: " << +sz.width << ", height: " << +sz.height << std::endl;
	}
}

void
LiteHtmlView::draw_image( litehtml::uint_ptr hdc, const litehtml::tchar_t* src,
	const litehtml::tchar_t* baseurl, const litehtml::position& pos )
{
	std::string url;
	make_url(src, baseurl, url);
	const auto& img = m_images.find(url.c_str());
	if(img != m_images.end())
	{
		if(img->second)
		{
			DrawBitmap(img->second,BPoint(pos.x,pos.y)); // TODO support scaling
			//draw_txdib(cr, img->second.get(), pos.x, pos.y, pos.width, pos.height);
		}
	}
}

void 
LiteHtmlView::draw_background( litehtml::uint_ptr hdc, 
	const litehtml::background_paint& bg )
{
	std::cout << "draw_background" << std::endl;
	if (0 < bg.image.length())
	{
		std::cout << "    background includes an image!" << std::endl;
		draw_image(hdc,bg.image.c_str(),m_base_url.c_str(),litehtml::position(bg.position_x,bg.position_y,bg.image_size.width,bg.image_size.height));
	}
}

void 
LiteHtmlView::draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root)
{
	std::cout << "draw_borders" << std::endl;
	int bdr_top		= 0;
	int bdr_bottom	= 0;
	int bdr_left	= 0;
	int bdr_right	= 0;
	
	//std::cout << "   uint ptr: " << +hdc << std::endl;
	//std::cout << "   this ptr: " << +this << std::endl;

	if(borders.top.width != 0 && borders.top.style > litehtml::border_style_hidden)
	{
		bdr_top = (int) borders.top.width;
		std::cout << "	Border top: " << bdr_right << std::endl;
	}
	if(borders.bottom.width != 0 && borders.bottom.style > litehtml::border_style_hidden)
	{
		bdr_bottom = (int) borders.bottom.width;
		std::cout << "	Border bottom: " << bdr_right << std::endl;
	}
	if(borders.left.width != 0 && borders.left.style > litehtml::border_style_hidden)
	{
		bdr_left = (int) borders.left.width;
		std::cout << "	Border left: " << bdr_right << std::endl;
	}
	if(borders.right.width != 0 && borders.right.style > litehtml::border_style_hidden)
	{
		bdr_right = (int) borders.right.width;
		std::cout << "	Border right: " << bdr_right << std::endl;
	}
	
	
	if (bdr_bottom)
	{
		// draw rectangle for now - no check for radius
		StrokeRect(
			BRect(
				BPoint(draw_pos.left(), draw_pos.bottom()),
				BPoint(draw_pos.left() + bdr_left, draw_pos.bottom() - bdr_bottom)
			)
		);
	}
}

void 
LiteHtmlView::transform_text(litehtml::tstring& text, litehtml::text_transform tt)
{
	std::cout << "transform_text" << std::endl;
}

void 
LiteHtmlView::set_clip( const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius, bool valid_x, bool valid_y )
{
	std::cout << "set_clip" << std::endl;
}

void 
LiteHtmlView::del_clip()
{
	std::cout << "del_clip" << std::endl;
}



std::shared_ptr<litehtml::element>	
LiteHtmlView::create_element(const litehtml::tchar_t *tag_name,
							 const litehtml::string_map &attributes,
							 const std::shared_ptr<litehtml::document> &doc)
{
	//std::cout << "create_element" << std::endl;
	return 0;
}

void 
LiteHtmlView::get_media_features(litehtml::media_features& media) const
{
	std::cout << "get_media_features" << std::endl;
	litehtml::position client;
    get_client_rect(client);
	media.type			= litehtml::media_type_screen;
	media.width			= client.width;
	media.height		= client.height;
	BRect bounds(Bounds());
	media.device_width	= bounds.Width();
	media.device_height	= bounds.Height();
	media.color			= 8;
	media.monochrome	= 0;
	media.color_index	= 256;
	media.resolution	= 96;
}

void 
LiteHtmlView::link(const std::shared_ptr<litehtml::document> &ptr, const litehtml::element::ptr& el)
{
	std::cout << "link" << std::endl;
}

void 
LiteHtmlView::set_caption(const char* caption)
{
	std::cout << "set_caption" << std::endl;
}

void
LiteHtmlView::get_client_rect(litehtml::position& client) const
{
	//std::cout << "get_client_rect" << std::endl;
	BRect bounds(Bounds());
	BPoint leftTop = bounds.LeftTop();
	client.width = bounds.Width();
	client.height = bounds.Height();
	client.x = leftTop.x;
	client.y = leftTop.y;
}

void 
LiteHtmlView::on_anchor_click(const char* base, const litehtml::element::ptr& anchor)
{
	std::cout << "on_anchor_click" << std::endl;
}

void 
LiteHtmlView::set_cursor(const char* cursor)
{
	std::cout << "set_cursor" << std::endl;
}

void 
LiteHtmlView::import_css(litehtml::tstring& s1, const litehtml::tstring& s2, litehtml::tstring& s3)
{
	std::cout << "import_css" << std::endl;
}

void 
LiteHtmlView::get_language(litehtml::tstring& s1, litehtml::tstring& s2) const
{
	std::cout << "get_language" << std::endl;
}
