#pragma once
#include <windows.h>
#include <mlang.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <mlang.h>
#include <vector>
#include <litehtml.h>
#include <GdiPlus.h>
#include "..\win32\win32_container.h"

class gdiplus_container : public litehtml::win32_container
{
public:
	gdiplus_container(void);
	virtual ~gdiplus_container(void);

protected:
	virtual void		draw_ellipse(HDC hdc, int x, int y, int width, int height, const litehtml::web_color& color, int line_width);
	virtual void		fill_ellipse(HDC hdc, int x, int y, int width, int height, const litehtml::web_color& color);
	virtual void		fill_rect(HDC hdc, int x, int y, int width, int height, const litehtml::web_color& color, const litehtml::css_border_radius& radius);
	virtual void		get_img_size(litehtml::uint_ptr img, litehtml::size& sz);
	virtual void		draw_img(HDC hdc, litehtml::uint_ptr img, const litehtml::position& pos);
	virtual void		free_image(litehtml::uint_ptr img);
	virtual void		draw_img_bg(HDC hdc, litehtml::uint_ptr img, const litehtml::position& draw_pos, const litehtml::position& pos, litehtml::background_repeat repeat, litehtml::background_attachment attachment);
	virtual void		draw_borders(litehtml::uint_ptr hdc, const litehtml::css_borders& borders, const litehtml::position& draw_pos);
};
