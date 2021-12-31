/*
 * Copyright 2019-2020 Haiku Inc.
 * All rights reserved. Distributed under the terms of the BSD 3-clause license.
 * Constributors
 * 2019-2020	Adam Fowler <adamfowleruk@gmail.com>
 */
#ifndef LITEHTMLVIEW_H
#define LITEHTMLVIEW_H

#include "../../include/litehtml.h"

#include <map>
#include <string>

#include <View.h>

class BBitmap;


enum {
	M_HTML_RENDERED = 'hrnd'
};

class LiteHtmlView : public BView, public litehtml::document_container
{
public:
										//LiteHtmlView(BMessage *archive);
										LiteHtmlView(BRect frame, const char *name);
										//LiteHtmlView(const char *name, uint32 flags, BLayout *layout=NULL);
								
	virtual								~LiteHtmlView();
	
			void						SetContext(litehtml::context* ctx);
			void						RenderFile(const char* localFilePath);
			void						RenderHTML(const std::string& htmlText);
			
			
	virtual litehtml::uint_ptr			create_font(const litehtml::tchar_t* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics* fm) override;
	virtual void						delete_font(litehtml::uint_ptr hFont) override;
	virtual int							text_width(const litehtml::tchar_t* text, litehtml::uint_ptr hFont) override;
	virtual void						draw_text(litehtml::uint_ptr hdc, const litehtml::tchar_t* text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos) override;
	virtual int							pt_to_px(int pt) override;
	virtual int							get_default_font_size() const override;
	virtual const litehtml::tchar_t*	get_default_font_name() const override;
	virtual void 						load_image(const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, bool redraw_on_ready) override;
	virtual void						get_image_size(const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, litehtml::size& sz) override;
	virtual void						draw_background(litehtml::uint_ptr hdc, const litehtml::background_paint& bg) override;
	virtual void						draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root) override;
	virtual void 						draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker) override;
	virtual std::shared_ptr<litehtml::element>	create_element(const litehtml::tchar_t *tag_name,
																 const litehtml::string_map &attributes,
																 const std::shared_ptr<litehtml::document> &doc) override;
	virtual void						get_media_features(litehtml::media_features& media) const override;
	//virtual void						get_language(litehtml::tstring& language, litehtml::tstring & culture) const override;
	virtual void 						link(const std::shared_ptr<litehtml::document> &ptr, const litehtml::element::ptr& el) override;


	virtual	void						transform_text(litehtml::tstring& text, litehtml::text_transform tt) override;
	virtual void						set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius, bool valid_x, bool valid_y) override;
	virtual void						del_clip() override;
	
	// unimplemented
	virtual void 						set_caption(const char*);
	virtual void						get_client_rect(litehtml::position& client) const;
	virtual void 						set_base_url(const char*);
	virtual void 						on_anchor_click(const char*, const litehtml::element::ptr&);
	virtual void 						set_cursor(const char*);
	virtual void 						import_css(litehtml::tstring&, const litehtml::tstring&, litehtml::tstring&);
	virtual void 						get_language(litehtml::tstring&, litehtml::tstring&) const;
	
	//BView
	virtual void 						Draw(BRect updateRect) override;
	virtual void						GetPreferredSize(float* width, float* height) override;
	
protected:
	void								make_url(const litehtml::tchar_t* url, const litehtml::tchar_t* basepath, litehtml::tstring& out);
	virtual void						draw_image( litehtml::uint_ptr hdc, const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, const litehtml::position& pos );
	
private:
	litehtml::context*					fContext;
	litehtml::document::ptr				m_html;
	std::map<const std::string,BBitmap*> m_images;
	litehtml::tstring					m_base_url;
	litehtml::tstring					m_url;
};

#endif
