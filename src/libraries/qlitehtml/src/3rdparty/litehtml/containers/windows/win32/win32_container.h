#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <set>
#include <litehtml.h>

class win32_container : public litehtml::document_container
{
  public:
	typedef litehtml::uint_ptr				 uint_ptr;
	typedef std::map<std::wstring, uint_ptr> images_map;

  protected:
	images_map				   m_images;
	litehtml::position::vector m_clips;
	HRGN					   m_hClipRgn;
	std::set<std::wstring>	   m_installed_fonts;
	HDC						   m_tmp_hdc;
	CRITICAL_SECTION		   m_img_sync;

  public:
	win32_container();
	virtual ~win32_container();

	// litehtml::document_container members
	uint_ptr		  create_font(const litehtml::font_description& descr, const litehtml::document* doc,
								  litehtml::font_metrics* fm) override;
	void			  delete_font(uint_ptr hFont) override;
	const char*		  get_default_font_name() const override;
	litehtml::pixel_t get_default_font_size() const override;
	litehtml::pixel_t text_width(const char* text, uint_ptr hFont) override;
	void			  draw_text(uint_ptr hdc, const char* text, uint_ptr hFont, litehtml::web_color color,
								const litehtml::position& pos) override;
	void			  transform_text(litehtml::string& text, litehtml::text_transform tt) override;

	litehtml::pixel_t pt_to_px(float pt) const override;
	void			  draw_list_marker(uint_ptr hdc, const litehtml::list_marker& marker) override;
	void			  load_image(const char* src, const char* baseurl, bool redraw_on_ready) override;
	void			  get_image_size(const char* src, const char* baseurl, litehtml::size& sz) override;
	void draw_image(litehtml::uint_ptr hdc, const litehtml::background_layer& layer, const std::string& url,
					const std::string& base_url) override;
	void draw_solid_fill(litehtml::uint_ptr hdc, const litehtml::background_layer& layer,
						 const litehtml::web_color& color) override;
	void draw_linear_gradient(litehtml::uint_ptr hdc, const litehtml::background_layer& layer,
							  const litehtml::background_layer::linear_gradient& gradient) override
	{
	}
	void draw_radial_gradient(litehtml::uint_ptr hdc, const litehtml::background_layer& layer,
							  const litehtml::background_layer::radial_gradient& gradient) override
	{
	}
	void draw_conic_gradient(litehtml::uint_ptr hdc, const litehtml::background_layer& layer,
							 const litehtml::background_layer::conic_gradient& gradient) override
	{
	}

	void set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius) override;
	void del_clip() override;
	litehtml::element::ptr create_element(const char* tag_name, const litehtml::string_map& attributes,
										  const litehtml::document::ptr& doc) override;
	void				   get_media_features(litehtml::media_features& media) const override;
	void				   get_language(litehtml::string& language, litehtml::string& culture) const override;
	void				   link(const litehtml::document::ptr& doc, const litehtml::element::ptr& el) override;
	litehtml::string	   resolve_color(const litehtml::string& color) const override;

  protected:
	void apply_clip(HDC hdc);
	void release_clip(HDC hdc);

	virtual void make_url(LPCWSTR url, LPCWSTR basepath, std::wstring& out) = 0;
	void		 make_url_utf8(const char* url, const char* basepath, std::wstring& out);

	// get_image is called by load_image.
	// if url_or_path is URL then get_image may return 0, the image should be added later by add_image when it becomes
	// available
	virtual uint_ptr get_image(LPCWSTR url_or_path, bool redraw_on_ready) = 0;
	void			 add_image(LPCWSTR url, uint_ptr img);
	void			 clear_images();
	virtual void	 free_image(uint_ptr img)													   = 0;
	virtual void	 get_img_size(uint_ptr img, litehtml::size& sz)								   = 0;
	virtual void	 draw_img_bg(HDC hdc, uint_ptr img, const litehtml::background_layer& layer)   = 0;

	virtual void draw_ellipse(HDC hdc, litehtml::pixel_t x, litehtml::pixel_t y, litehtml::pixel_t width,
							  litehtml::pixel_t height, litehtml::web_color color, int line_width) = 0;
	virtual void fill_ellipse(HDC hdc, litehtml::pixel_t x, litehtml::pixel_t y, litehtml::pixel_t width,
							  litehtml::pixel_t height, litehtml::web_color color)				   = 0;
	virtual void fill_rect(HDC hdc, litehtml::pixel_t x, litehtml::pixel_t y, litehtml::pixel_t width,
						   litehtml::pixel_t height, litehtml::web_color color)					   = 0;

  private:
	static int CALLBACK EnumFontsProc(const LOGFONT* lplf, const TEXTMETRIC* lptm, DWORD dwType, LPARAM lpData);
	void				lock_images_cache();
	void				unlock_images_cache();
};
