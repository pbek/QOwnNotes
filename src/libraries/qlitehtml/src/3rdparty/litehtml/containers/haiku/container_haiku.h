/*
 * Copyright 2019-2020 Haiku Inc.
 * All rights reserved. Distributed under the terms of the BSD 3-clause license.
 * Contributors
 * 2019-2020	Adam Fowler <adamfowleruk@gmail.com>
 */
#ifndef LITEHTMLVIEW_H
#define LITEHTMLVIEW_H

#include "../../include/litehtml.h"

#include <map>
#include <string>

#include <View.h>

class BBitmap;

enum
{
	M_HTML_RENDERED = 'hrnd'
};

class LiteHtmlView : public BView, public litehtml::document_container
{
  public:
	// LiteHtmlView(BMessage *archive);
	LiteHtmlView(BRect frame, const char* name);
	// LiteHtmlView(const char *name, uint32 flags, BLayout *layout=NULL);

	virtual ~LiteHtmlView();

	void RenderFile(const char* localFilePath);
	void RenderHTML(const std::string& htmlText);

	virtual litehtml::uint_ptr create_font(const litehtml::font_description& descr, const litehtml::document* doc,
										   litehtml::font_metrics* fm) override;
	virtual void			   delete_font(litehtml::uint_ptr hFont) override;
	virtual litehtml::pixel_t  text_width(const char* text, litehtml::uint_ptr hFont) override;
	virtual void			   draw_text(litehtml::uint_ptr hdc, const char* text, litehtml::uint_ptr hFont,
										 litehtml::web_color color, const litehtml::position& pos) override;
	virtual litehtml::pixel_t  pt_to_px(float pt) const override;
	virtual litehtml::pixel_t  get_default_font_size() const override;
	virtual const char*		   get_default_font_name() const override;
	virtual void			   load_image(const char* src, const char* baseurl, bool redraw_on_ready) override;
	virtual void			   get_image_size(const char* src, const char* baseurl, litehtml::size& sz) override;
	virtual void			   draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders,
											const litehtml::position& draw_pos, bool root) override;
	virtual void			   draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker) override;
	virtual std::shared_ptr<litehtml::element> create_element(const char*								 tag_name,
															  const litehtml::string_map&				 attributes,
															  const std::shared_ptr<litehtml::document>& doc) override;
	virtual void							   get_media_features(litehtml::media_features& media) const override;
	// virtual void						get_language(litehtml::tstring& language, litehtml::tstring & culture) const
	// override;
	virtual void link(const std::shared_ptr<litehtml::document>& ptr, const litehtml::element::ptr& el) override;
	virtual void draw_image(litehtml::uint_ptr hdc, const litehtml::background_layer& layer, const std::string& url,
							const std::string& base_url) override;
	virtual void draw_solid_fill(litehtml::uint_ptr hdc, const litehtml::background_layer& layer,
								 const litehtml::web_color& color) override;

	virtual void transform_text(litehtml::string& text, litehtml::text_transform tt) override;
	virtual void set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius) override;
	virtual void del_clip() override;

	// unimplemented
	virtual void set_caption(const char*) override;
	virtual void get_viewport(litehtml::position& client) const override;
	virtual void set_base_url(const char*) override;
	virtual void on_anchor_click(const char*, const litehtml::element::ptr&) override;
	virtual void set_cursor(const char*) override;
	virtual void import_css(litehtml::string&, const litehtml::string&, litehtml::string&) override;
	virtual void get_language(litehtml::string&, litehtml::string&) const override;
	virtual void draw_linear_gradient(litehtml::uint_ptr hdc, const litehtml::background_layer& layer,
									  const litehtml::background_layer::linear_gradient& gradient) override
	{
	}
	virtual void draw_radial_gradient(litehtml::uint_ptr hdc, const litehtml::background_layer& layer,
									  const litehtml::background_layer::radial_gradient& gradient) override
	{
	}
	virtual void draw_conic_gradient(litehtml::uint_ptr hdc, const litehtml::background_layer& layer,
									 const litehtml::background_layer::conic_gradient& gradient) override
	{
	}
	virtual void on_mouse_event(const litehtml::element::ptr& el, litehtml::mouse_event event) override {};

	// BView
	virtual void Draw(BRect updateRect) override;
	virtual void GetPreferredSize(float* width, float* height) override;

  protected:
	void		 make_url(const char* url, const char* basepath, litehtml::string& out);
	virtual void draw_image(litehtml::uint_ptr hdc, const char* src, const char* baseurl,
							const litehtml::position& pos);

  private:
	litehtml::document::ptr				  m_html;
	std::map<const std::string, BBitmap*> m_images;
	std::string							  m_base_url;
	std::string							  m_url;
};

#endif
