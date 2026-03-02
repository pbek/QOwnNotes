#include <litehtml.h>
#include "Bitmap.h"
#include "canvas_ity.hpp"
#include "litehtml/types.h"
using namespace litehtml;
using namespace canvas_ity;

class test_container : public document_container
{
  public:
	int						 width;
	int						 height;
	string					 basedir;
	std::map<string, Bitmap> images;

	test_container(int width, int height, string basedir) :
		width(width),
		height(height),
		basedir(basedir)
	{
	}

	string make_url(const char* src, const char* baseurl);

	uint_ptr create_font(const font_description& descr, const document* doc, litehtml::font_metrics* fm) override;
	void	 delete_font(uint_ptr /*hFont*/) override {}
	pixel_t	 text_width(const char* text, uint_ptr hFont) override;
	void	 draw_text(uint_ptr hdc, const char* text, uint_ptr hFont, web_color color, const position& pos) override;
	pixel_t	 pt_to_px(float pt) const override;
	pixel_t	 get_default_font_size() const override;
	const char* get_default_font_name() const override;
	void		load_image(const char* src, const char* baseurl, bool redraw_on_ready) override;
	void		get_image_size(const char* src, const char* baseurl, size& sz) override;
	void draw_image(uint_ptr hdc, const background_layer& layer, const string& url, const string& base_url) override;
	void draw_solid_fill(uint_ptr hdc, const background_layer& layer, const web_color& color) override;
	void draw_linear_gradient(uint_ptr hdc, const background_layer& layer,
							  const background_layer::linear_gradient& gradient) override;
	void draw_radial_gradient(uint_ptr hdc, const background_layer& layer,
							  const background_layer::radial_gradient& gradient) override;
	void draw_conic_gradient(uint_ptr hdc, const background_layer& layer,
							 const background_layer::conic_gradient& gradient) override;
	void draw_borders(uint_ptr hdc, const borders& borders, const position& draw_pos, bool root) override;
	void draw_list_marker(uint_ptr hdc, const list_marker& marker) override;
	element::ptr create_element(const char* /*tag_name*/, const string_map& /*attributes*/,
								const document::ptr& /*doc*/) override
	{
		return nullptr;
	}
	void get_media_features(media_features& media) const override;
	void get_language(string& /*language*/, string& /*culture*/) const override {}
	void link(const document::ptr& /*doc*/, const element::ptr& /*el*/) override {}

	void transform_text(string& /*text*/, text_transform /*tt*/) override {}
	void set_clip(const position& /*pos*/, const border_radiuses& /*bdr_radius*/) override {}
	void del_clip() override {}

	void set_caption(const char* /*caption*/) override {}
	void set_base_url(const char* /*base_url*/) override {}
	void on_anchor_click(const char* /*url*/, const element::ptr& /*el*/) override {}
	void on_mouse_event(const element::ptr& /*el*/, mouse_event /*event*/) override {};
	void set_cursor(const char* /*cursor*/) override {}
	void import_css(string& text, const string& url, string& baseurl) override;
	void get_viewport(position& client) const override;
};
