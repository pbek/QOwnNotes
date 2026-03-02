#pragma once
#include "..\win32\win32_container.h"

class gdiplus_container : public win32_container
{
  public:
	gdiplus_container();
	virtual ~gdiplus_container();

  private:
	ULONG_PTR m_gdiplusToken;

  protected:
	// win32_container members
	void draw_ellipse(HDC hdc, litehtml::pixel_t x, litehtml::pixel_t y, litehtml::pixel_t width,
					  litehtml::pixel_t height, litehtml::web_color color, int line_width) override;
	void fill_ellipse(HDC hdc, litehtml::pixel_t x, litehtml::pixel_t y, litehtml::pixel_t width,
					  litehtml::pixel_t height, litehtml::web_color color) override;
	void fill_rect(HDC hdc, litehtml::pixel_t x, litehtml::pixel_t y, litehtml::pixel_t width, litehtml::pixel_t height,
				   litehtml::web_color color) override;
	void get_img_size(uint_ptr img, litehtml::size& sz) override;
	void free_image(uint_ptr img) override;
	void draw_img_bg(HDC hdc, uint_ptr img, const litehtml::background_layer& layer) override;
	// litehtml::document_container members
	void draw_borders(uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos,
					  bool root) override;
};
