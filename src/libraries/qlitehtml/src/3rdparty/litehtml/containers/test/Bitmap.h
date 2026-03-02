#pragma once
#include <litehtml.h>
using namespace litehtml;
using rect = position;
namespace canvas_ity
{
	class canvas;
}
using namespace canvas_ity;

// color != web_color because sizeof(web_color) != 4
struct color
{
	byte r, g, b, a;

	color() :
		r(0),
		g(0),
		b(0),
		a(0)
	{
	}
	color(byte r, byte g, byte b, byte a) :
		r(r),
		g(g),
		b(b),
		a(a)
	{
	}
	color(web_color c) :
		r(c.red),
		g(c.green),
		b(c.blue),
		a(c.alpha)
	{
	}

	bool operator==(color c) const { return r == c.r && g == c.g && b == c.b && a == c.a; }
	bool operator!=(color c) const { return !(*this == c); }
};
const color white(255, 255, 255, 255);
const color black(0, 0, 0, 255);
const color transparent(0, 0, 0, 0);

class Bitmap
{
  public:
	int			  width	 = 0;
	int			  height = 0;
	vector<color> data;

	Bitmap() {}
	Bitmap(int width, int height, color color = white) :
		width(width),
		height(height)
	{
		data.resize(width * height, color);
	}
	Bitmap(string filename) { load(filename); }
	Bitmap(canvas& canvas);

	bool operator==(const Bitmap& bmp) const { return width == bmp.width && height == bmp.height && data == bmp.data; }
	bool operator!=(const Bitmap& bmp) const { return !(*this == bmp); }
		 operator bool() const { return width != 0 && height != 0; }

	color get_pixel(int x, int y) const;
	void  set_pixel(int x, int y, color color);
	void  draw_line(int x0, int y0, int x1, int y1, color color);
	void  draw_rect(int x, int y, int width, int height, color color);
	void  fill_rect(rect rect, color color);
	void  replace_color(color original, color replacement);

	rect find_picture(color bgcolor = white);
	void load(string filename);
	void save(string filename);
};

byte max_color_diff(const Bitmap& a, const Bitmap& b);
