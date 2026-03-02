/*
 * Copyright 2019-2020 Haiku Inc.
 * All rights reserved. Distributed under the terms of the BSD 3-clause license.
 * Contributors
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

LiteHtmlView::LiteHtmlView(BRect frame, const char* name) :
	BView(frame, name, B_FOLLOW_ALL, B_WILL_DRAW),
	m_html(NULL),
	m_images(),
	m_base_url(),
	m_url()
{

	BRect  bounds(Bounds());
	BPoint topLeft = bounds.LeftTop();
	std::cout << "Initial bounds: topLeft x: " << +topLeft.x << ", y: " << +topLeft.y << ", width: " << +bounds.Width()
			  << ", height: " << +bounds.Height() << std::endl;

	SetDrawingMode(B_OP_OVER);
	SetFont(be_plain_font);

	// FillRect(bounds,B_SOLID_LOW);

	// SetLowColor(B_DOCUMENT_PANEL_COLOR);
	// FillRect(rect);
}

LiteHtmlView::~LiteHtmlView() {}

void LiteHtmlView::RenderFile(const char* localFilePath)
{
	std::cout << "RenderFile" << std::endl;
	// BUrlRequest req;
	//  assume a local file for now, that is HTML
	std::ifstream t(localFilePath);
	std::string	  html((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

	// std::cout << "HTML output:-" << std::endl << html << std::endl;

	// Get parent folder for the base url
	std::cout << "Loaded from file: " << localFilePath << std::endl;
	BPath htmlPath(localFilePath);
	BPath dirPath;
	htmlPath.GetParent(&dirPath);
	std::cout << "parent path: " << dirPath.Path() << std::endl;
	set_base_url(dirPath.Path());
	// std::cout << "    base url now:" << m_base_url << std::endl;

	RenderHTML(html);
}

void LiteHtmlView::RenderHTML(const std::string& htmlText)
{
	std::cout << "RenderHTML" << std::endl;

	// now use this string
	m_html = litehtml::document::createFromString(htmlText.c_str(), this);
	if(m_html)
	{
		std::cout << "Successfully read html" << std::endl;
		// success
		// post-parse render operations, if required.
		Invalidate();
	} else
	{
		std::cout << "Failed to read html" << std::endl;
	}
	// always fire the rendering complete message
	std::cout << "Sending html rendered message: " << M_HTML_RENDERED << std::endl;
}

void LiteHtmlView::Draw(BRect b)
{
	std::cout << "DRAW CALLED" << std::endl;

	BRect bounds(Bounds());
	FillRect(bounds, B_SOLID_LOW);

	// b only part of the window, but we need to draw the whole lot

	if(NULL != m_html)
	{
		BPoint			   leftTop = bounds.LeftTop();
		litehtml::position clip(leftTop.x, leftTop.y, bounds.Width(), bounds.Height());
		m_html->render(bounds.Width());
		m_html->draw((litehtml::uint_ptr) this, 0, 0, &clip);
	}
	SendNotices(M_HTML_RENDERED, new BMessage(M_HTML_RENDERED));
}

void LiteHtmlView::GetPreferredSize(float* width, float* height)
{
	if(NULL == m_html)
	{
		BRect bounds(Bounds());
		*width	= bounds.Width();
		*height = bounds.Height();
	} else
	{
		*width	= m_html->width();
		*height = m_html->height();
	}
}

litehtml::uint_ptr LiteHtmlView::create_font(const litehtml::font_description& descr, const litehtml::document* doc,
											 litehtml::font_metrics* fm)
{
	// std::cout << "create_font" << std::endl;
	litehtml::string_vector fonts;
	litehtml::split_string(descr.family, fonts, ",");
	litehtml::trim(fonts[0]);

	uint16 face = B_REGULAR_FACE; // default
	if(descr.style & litehtml::font_style_italic)
	{
		face |= B_ITALIC_FACE;
	}
	if(descr.decoration_line & litehtml::text_decoration_line_underline)
	{
		face |= B_UNDERSCORE_FACE;
	}
	if(descr.decoration_line & litehtml::text_decoration_line_line_through)
	{
		face |= B_STRIKEOUT_FACE;
	}
	// Note: LIGHT, HEAVY, CONDENSED not supported in BeOS R5
#ifdef __HAIKU__
	if(descr.weight >= 0 && descr.weight < 150)
		face |= B_LIGHT_FACE;
	else if(descr.weight >= 150 && descr.weight < 250)
		face |= B_LIGHT_FACE;
	else if(descr.weight >= 250 && descr.weight < 350)
		face |= B_LIGHT_FACE;
	// else if(weight >= 350 && weight < 450)	face |= B_REGULAR_FACE;
	// else if(weight >= 450 && weight < 550)	face |= B_REGULAR_FACE;
	else if(descr.weight >= 550 && descr.weight < 650)
		face |= B_CONDENSED_FACE;
#else
	else if(descr.weight >= 550 && descr.weight < 650)
		face |= B_BOLD_FACE;
#endif
	else if(descr.weight >= 650 && descr.weight < 750)
		face |= B_BOLD_FACE;
#ifndef __HAIKU__
	else if(descr.weight >= 750 && descr.weight < 850)
		face |= B_BOLD_FACE;
	else if(descr.weight >= 950)
		face |= B_BOLD_FACE;
#else
	else if(descr.weight >= 750 && descr.weight < 850)
		face |= B_HEAVY_FACE;
	else if(descr.weight >= 950)
		face |= B_HEAVY_FACE;
#endif

	BFont* tempFont = new BFont();
	bool   found	= false;
	for(litehtml::string_vector::iterator i = fonts.begin(); i != fonts.end(); i++)
	{
		if(B_OK == tempFont->SetFamilyAndFace(i->c_str(), face))
		{
			found = true;
			break;
		}
	}

	if(!found)
	{
		// default to the Be plain font
		tempFont = new BFont(be_plain_font);
		if(descr.weight >= 550)
		{
			tempFont = new BFont(be_bold_font);
		}
		tempFont->SetFace(face); // chooses closest
	}

	tempFont->SetSize(descr.size);

	font_height hgt;
	tempFont->GetHeight(&hgt);
	fm->font_size = descr.size;
	fm->ascent	  = hgt.ascent;
	fm->descent	  = hgt.descent;
	fm->height	  = hgt.ascent + hgt.descent;
	fm->x_height  = hgt.leading;
	fm->ch_width  = tempFont->StringWidth("0");
	if(descr.style == litehtml::font_style_italic || descr.decoration_line != litehtml::text_decoration_line_none)
	{
		fm->draw_spaces = true;
	} else
	{
		fm->draw_spaces = false;
	}
	fm->sub_shift	= descr.size / 5;
	fm->super_shift = descr.size / 3;

	return (litehtml::uint_ptr) tempFont;
}

void LiteHtmlView::delete_font(litehtml::uint_ptr hFont)
{
	BFont* font = (BFont*) hFont;
	if(font)
	{
		delete font;
	}
}

litehtml::pixel_t LiteHtmlView::text_width(const char* text, litehtml::uint_ptr hFont)
{
	// std::cout << "text_width" << std::endl;
	BFont* fnt = (BFont*) hFont;
	return fnt->StringWidth(text);
}

void LiteHtmlView::draw_text(litehtml::uint_ptr hdc, const char* text, litehtml::uint_ptr hFont,
							 litehtml::web_color color, const litehtml::position& pos)
{
	// std::cout << "draw_text" << std::endl;
	if(!text)
		return;
	if(0 == strlen(text))
		return;
	BFont* fnt = (BFont*) hFont;

	// std::cout << "    left: " << +pos.left() << ", top: " << +pos.top() << std::endl;
	// std::cout << "    RGBA: " << +color.red << "," << +color.green << "," << +color.blue << "," << +color.alpha <<
	// std::endl; std::cout << "    Font size: " << +fnt->Size() << std::endl; std::cout << "    Text: " << text <<
	// std::endl;
	BRect bounds(Bounds());

	// FillRect(bounds,B_SOLID_LOW);

	BPoint leftTop = bounds.LeftTop();
	// std::cout << "    Bounds left: " << +leftTop.x << ", top: " << +leftTop.y << ", Width: " << +bounds.Width() << ",
	// Height: " << +bounds.Height() << std::endl;

	font_height fh;
	fnt->GetHeight(&fh);
	int baseline = fh.ascent + fh.descent;					// + 10;
	int leftbase = 0;										// 10;
	MovePenTo(pos.left() + leftbase, pos.top() + baseline); // leftTop.x,leftTop.y);
	SetFont(fnt);
	// SetFont(be_plain_font);
	rgb_color clr = ui_color(B_DOCUMENT_TEXT_COLOR);
	/*
	rgb_color clr;
	clr.blue = 40;
	clr.red = 40;
	clr.green = 40;
	*/

	clr.red		  = color.red;
	clr.green	  = color.green;
	clr.blue	  = color.blue;
	clr.alpha	  = color.alpha;

	// std::cout << "    Final RGBA: " << +clr.red << "," << +clr.green << "," << +clr.blue << "," << +clr.alpha <<
	// std::endl;
	SetHighColor(clr);
	SetLowColor(ui_color(B_DOCUMENT_BACKGROUND_COLOR));
	BString mystr("");
	// mystr << "text: ";
	mystr << text;
	DrawString(mystr);
}

litehtml::pixel_t LiteHtmlView::pt_to_px(float pt) const
{
	std::cout << "pt_to_px" << std::endl;
	return pt * 1.3333333333;
}

litehtml::pixel_t LiteHtmlView::get_default_font_size() const
{
	// std::cout << "get_default_font_size" << std::endl;
	return 12;
}

const char* LiteHtmlView::get_default_font_name() const
{
	// std::cout << "get_default_font_name" << std::endl;
	font_family fam;
	font_style	style;
	be_plain_font->GetFamilyAndStyle(&fam, &style);
	char* cp = strdup(fam);
	return cp;
}

void LiteHtmlView::draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker)
{
	std::cout << "draw_list_marker" << std::endl;
	if(!marker.image.empty())
	{
		std::cout << "    image marker" << std::endl;
	}
}

void LiteHtmlView::load_image(const char* src, const char* baseurl, bool redraw_on_ready)
{
	std::cout << "load_image" << std::endl;

	std::string url;
	make_url(src, baseurl, url);
	if(m_images.find(url.c_str()) == m_images.end())
	{
		BEntry entry(url.c_str(), true);
		if(entry.Exists())
		{
			std::cout << "    Loading bitmap from file" << std::endl;
			BBitmap* img  = BTranslationUtils::GetBitmap(url.c_str());
			m_images[url] = img;
		}
	}
}

void LiteHtmlView::make_url(const char* url, const char* basepath, litehtml::string& out)
{
	std::cout << "make_url" << std::endl;
	std::cout << "    url: " << url << std::endl;
	if(!basepath || (basepath && !basepath[0]))
	{
		if(!m_base_url.empty())
		{
			// out = urljoin(m_base_url, std::string(url));
			std::string ns(m_base_url);
			ns	+= "/";
			ns	+= url;
			out	 = ns;
		} else
		{
			out = url;
		}
	} else
	{
		std::cout << "    basepath: " << basepath << std::endl;
		// out = urljoin(std::string(basepath), std::string(url));
		std::string ns(basepath);
		ns	+= "/";
		ns	+= url;
		out	 = ns;
	}
	std::cout << "    Output url: " << out << std::endl;
}

void LiteHtmlView::set_base_url(const char* base_url)
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

void LiteHtmlView::get_image_size(const char* src, const char* baseurl, litehtml::size& sz)
{
	std::cout << "get_image_size" << std::endl;
	std::string url;
	make_url(src, NULL, url);
	const auto& miter(m_images.find(url.c_str()));
	if(m_images.end() != miter)
	{
		BBitmap* img  = (BBitmap*) miter->second;
		BRect	 size = img->Bounds();
		sz.width	  = size.Width();
		sz.height	  = size.Height();
		std::cout << "    width: " << +sz.width << ", height: " << +sz.height << std::endl;
	}
}

void LiteHtmlView::draw_image(litehtml::uint_ptr hdc, const litehtml::background_layer& layer, const std::string& url,
							  const std::string& base_url)
{
	draw_image(hdc, url.c_str(), base_url.c_str(), layer.border_box);
}

void LiteHtmlView::draw_image(litehtml::uint_ptr hdc, const char* src, const char* baseurl,
							  const litehtml::position& pos)
{
	std::string url;
	make_url(src, baseurl, url);
	const auto& img = m_images.find(url.c_str());
	if(img != m_images.end())
	{
		if(img->second)
		{
			DrawBitmap(img->second, BPoint(pos.x, pos.y)); // TODO support scaling
			// draw_txdib(cr, img->second.get(), pos.x, pos.y, pos.width, pos.height);
		}
	}
}

void LiteHtmlView::draw_solid_fill(litehtml::uint_ptr hdc, const litehtml::background_layer& layer,
								   const litehtml::web_color& color)
{
	SetHighColor(color.red, color.green, color.blue);
	FillRect(BRect(BPoint(layer.border_box.left(), layer.border_box.top()),
				   BPoint(layer.border_box.right(), layer.border_box.bottom())));
}

void LiteHtmlView::draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders,
								const litehtml::position& draw_pos, bool root)
{
	std::cout << "draw_borders" << std::endl;
	int bdr_top	   = 0;
	int bdr_bottom = 0;
	int bdr_left   = 0;
	int bdr_right  = 0;

	// std::cout << "   uint ptr: " << +hdc << std::endl;
	// std::cout << "   this ptr: " << +this << std::endl;

	if(borders.top.width != 0 && borders.top.style > litehtml::border_style_hidden)
	{
		SetHighColor(borders.top.color.red, borders.top.color.green, borders.top.color.blue);
		FillRect(BRect(BPoint(draw_pos.left(), draw_pos.top()),
					   BPoint(draw_pos.right(), draw_pos.top() + borders.top.width)));
	}
	if(borders.bottom.width != 0 && borders.bottom.style > litehtml::border_style_hidden)
	{
		SetHighColor(borders.bottom.color.red, borders.bottom.color.green, borders.bottom.color.blue);
		FillRect(BRect(BPoint(draw_pos.left(), draw_pos.bottom() - borders.bottom.width),
					   BPoint(draw_pos.right(), draw_pos.bottom())));
	}
	if(borders.left.width != 0 && borders.left.style > litehtml::border_style_hidden)
	{
		SetHighColor(borders.left.color.red, borders.left.color.green, borders.left.color.blue);
		FillRect(BRect(BPoint(draw_pos.left(), draw_pos.top()),
					   BPoint(draw_pos.left() + borders.left.width, draw_pos.bottom())));
	}
	if(borders.right.width != 0 && borders.right.style > litehtml::border_style_hidden)
	{
		SetHighColor(borders.right.color.red, borders.right.color.green, borders.right.color.blue);
		FillRect(BRect(BPoint(draw_pos.right() - borders.right.width, draw_pos.top()),
					   BPoint(draw_pos.right(), draw_pos.bottom())));
	}
}

void LiteHtmlView::transform_text(litehtml::string& text, litehtml::text_transform tt)
{
	std::cout << "transform_text" << std::endl;
}

void LiteHtmlView::set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius)
{
	std::cout << "set_clip" << std::endl;
}

void LiteHtmlView::del_clip()
{
	std::cout << "del_clip" << std::endl;
}

std::shared_ptr<litehtml::element> LiteHtmlView::create_element(const char*								   tag_name,
																const litehtml::string_map&				   attributes,
																const std::shared_ptr<litehtml::document>& doc)
{
	// std::cout << "create_element" << std::endl;
	return 0;
}

void LiteHtmlView::get_media_features(litehtml::media_features& media) const
{
	std::cout << "get_media_features" << std::endl;
	litehtml::position client;
	get_viewport(client);
	media.type	 = litehtml::media_type_screen;
	media.width	 = client.width;
	media.height = client.height;
	BRect bounds(Bounds());
	media.device_width	= bounds.Width();
	media.device_height = bounds.Height();
	media.color			= 8;
	media.monochrome	= 0;
	media.color_index	= 256;
	media.resolution	= 96;
}

void LiteHtmlView::link(const std::shared_ptr<litehtml::document>& ptr, const litehtml::element::ptr& el)
{
	std::cout << "link" << std::endl;
}

void LiteHtmlView::set_caption(const char* caption)
{
	std::cout << "set_caption" << std::endl;
}

void LiteHtmlView::get_viewport(litehtml::position& client) const
{
	// std::cout << "get_client_rect" << std::endl;
	BRect  bounds(Bounds());
	BPoint leftTop = bounds.LeftTop();
	client.width   = bounds.Width();
	client.height  = bounds.Height();
	client.x	   = leftTop.x;
	client.y	   = leftTop.y;
}

void LiteHtmlView::on_anchor_click(const char* base, const litehtml::element::ptr& anchor)
{
	std::cout << "on_anchor_click" << std::endl;
}

void LiteHtmlView::set_cursor(const char* cursor)
{
	std::cout << "set_cursor" << std::endl;
}

void LiteHtmlView::import_css(litehtml::string& s1, const litehtml::string& s2, litehtml::string& s3)
{
	std::cout << "import_css" << std::endl;
}

void LiteHtmlView::get_language(litehtml::string& s1, litehtml::string& s2) const
{
	std::cout << "get_language" << std::endl;
}
