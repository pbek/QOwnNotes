#ifndef LH_CONTAINER_LINUX_H
#define LH_CONTAINER_LINUX_H

#include "litehtml/types.h"
#include <litehtml.h>
#include <cairo.h>
#include <vector>

struct cairo_clip_box
{
	typedef std::vector<cairo_clip_box> vector;
	litehtml::position					box;
	litehtml::border_radiuses			radius;

	cairo_clip_box(const litehtml::position& vBox, const litehtml::border_radiuses& vRad)
	{
		box	   = vBox;
		radius = vRad;
	}

	cairo_clip_box(const cairo_clip_box& val)
	{
		box	   = val.box;
		radius = val.radius;
	}
	cairo_clip_box& operator=(const cairo_clip_box& val)
	{
		box	   = val.box;
		radius = val.radius;
		return *this;
	}
};

class container_cairo : public litehtml::document_container
{
  protected:
	cairo_clip_box::vector m_clips;

  public:
	container_cairo()		   = default;
	virtual ~container_cairo() = default;

	litehtml::pixel_t pt_to_px(float pt) const override;
	litehtml::pixel_t get_default_font_size() const override;
	const char*		  get_default_font_name() const override;
	void			  get_image_size(const char* src, const char* baseurl, litehtml::size& sz) override;
	void draw_image(litehtml::uint_ptr hdc, const litehtml::background_layer& layer, const std::string& url,
					const std::string& base_url) override;
	void draw_solid_fill(litehtml::uint_ptr hdc, const litehtml::background_layer& layer,
						 const litehtml::web_color& color) override;
	void draw_linear_gradient(litehtml::uint_ptr hdc, const litehtml::background_layer& layer,
							  const litehtml::background_layer::linear_gradient& gradient) override;
	void draw_radial_gradient(litehtml::uint_ptr hdc, const litehtml::background_layer& layer,
							  const litehtml::background_layer::radial_gradient& gradient) override;
	void draw_conic_gradient(litehtml::uint_ptr hdc, const litehtml::background_layer& layer,
							 const litehtml::background_layer::conic_gradient& gradient) override;
	void draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos,
					  bool root) override;
	void draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker) override;
	std::shared_ptr<litehtml::element> create_element(const char* tag_name, const litehtml::string_map& attributes,
													  const std::shared_ptr<litehtml::document>& doc) override;
	void							   get_media_features(litehtml::media_features& media) const override;
	void get_language(litehtml::string& language, litehtml::string& culture) const override;
	void link(const std::shared_ptr<litehtml::document>& ptr, const litehtml::element::ptr& el) override;

	void transform_text(litehtml::string& text, litehtml::text_transform tt) override;
	void set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius) override;
	void del_clip() override;

	virtual void			 make_url(const char* url, const char* basepath, litehtml::string& out);
	virtual cairo_surface_t* get_image(const std::string& url) = 0;
	virtual double			 get_screen_dpi() const			   = 0;
	virtual int				 get_screen_width() const		   = 0;
	virtual int				 get_screen_height() const		   = 0;

	void clear_images();

  protected:
	virtual void draw_ellipse(cairo_t* cr, litehtml::pixel_t x, litehtml::pixel_t y, litehtml::pixel_t width,
							  litehtml::pixel_t height, const litehtml::web_color& color, litehtml::pixel_t line_width);
	virtual void fill_ellipse(cairo_t* cr, litehtml::pixel_t x, litehtml::pixel_t y, litehtml::pixel_t width,
							  litehtml::pixel_t height, const litehtml::web_color& color);
	virtual void rounded_rectangle(cairo_t* cr, const litehtml::position& pos, const litehtml::border_radiuses& radius);

	void		clip_background_layer(cairo_t* cr, const litehtml::background_layer& layer);
	void		apply_clip(cairo_t* cr);
	static void set_color(cairo_t* cr, const litehtml::web_color& color)
	{
		cairo_set_source_rgba(cr, color.red / 255.0, color.green / 255.0, color.blue / 255.0, color.alpha / 255.0);
	}

  private:
	static void add_path_arc(cairo_t* cr, double x, double y, double rx, double ry, double a1, double a2, bool neg);
	static void draw_pixbuf(cairo_t* cr, cairo_surface_t* bmp, litehtml::pixel_t x, litehtml::pixel_t y, int cx,
							int cy);
	static cairo_surface_t* scale_surface(cairo_surface_t* surface, int width, int height);
};

#endif
