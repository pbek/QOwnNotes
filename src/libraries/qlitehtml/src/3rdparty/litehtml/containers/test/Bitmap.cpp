#include "Bitmap.h"
#include "lodepng.h"
#include "canvas_ity.hpp"
using namespace canvas_ity;

Bitmap::Bitmap(canvas& canvas) :
	Bitmap(canvas.width(), canvas.height())
{
	canvas.get_image_data((byte*) data.data(), width, height, width * 4, 0, 0);
}

color Bitmap::get_pixel(int x, int y) const
{
	if(x < 0 || x >= width || y < 0 || y >= height)
		return black;
	else
		return data[x + y * width];
}

void Bitmap::set_pixel(int x, int y, color color)
{
	if(x < 0 || x >= width || y < 0 || y >= height)
		return;
	if(color.a == 0)
		return;
	data[x + y * width] = color;
}

// endpoint is not drawn, like in GDI
void Bitmap::draw_line(int x0, int y0, int x1, int y1, color color)
{
	if(x0 != x1 && y0 != y1)
		return; // only horz and vert lines supported

	if(x0 == x1) // vert line
	{
		if(y0 > y1)
			swap(y0, y1);
		for(int y = y0; y < y1; y++)
			set_pixel(x0, y, color);
	} else if(y0 == y1) // horz line
	{
		if(x0 > x1)
			swap(x0, x1);
		for(int x = x0; x < x1; x++)
			set_pixel(x, y0, color);
	}
}

void Bitmap::draw_rect(int x, int y, int _width, int _height, color color)
{
	draw_line(x, y, x + _width, y, color);							   // top
	draw_line(x, y + _height - 1, x + _width, y + _height - 1, color); // bottom
	draw_line(x, y, x, y + _height, color);							   // left
	draw_line(x + _width - 1, y, x + _width - 1, y + _height, color);  // right
}

void Bitmap::fill_rect(rect rect, color color)
{
	for(int y = (int) rect.top(); y < rect.bottom(); y++)
		for(int x = (int) rect.left(); x < rect.right(); x++)
			set_pixel(x, y, color);
}

void Bitmap::replace_color(color original, color replacement)
{
	for(auto& pixel : data)
	{
		if(pixel == original)
			pixel = replacement;
	}
}

// find minimal rectangle containing pixels different from bgcolor
rect Bitmap::find_picture(color bgcolor)
{
	auto horz_line_empty = [&](int y) {
		for(int x = 0; x < width; x++)
			if(data[x + y * width] != bgcolor)
				return false;
		return true;
	};
	auto vert_line_empty = [&](int x) {
		for(int y = 0; y < height; y++)
			if(data[x + y * width] != bgcolor)
				return false;
		return true;
	};

	rect rect;
	int	 y;
	for(y = 0; y < height && horz_line_empty(y); y++)
		;
	if(y == height)
		return rect; // no picture
	rect.y = y;
	for(y = height - 1; y >= 0 && horz_line_empty(y); y--)
		;
	rect.height = y + 1 - rect.y;

	int x;
	for(x = 0; x < width && vert_line_empty(x); x++)
		;
	rect.x = x;
	for(x = width - 1; x >= 0 && vert_line_empty(x); x--)
		;
	rect.width = x + 1 - rect.x;

	return rect;
}

void Bitmap::load(string filename)
{
	vector<byte> image;
	unsigned	 w, h;
	lodepng::decode(image, w, h, filename);
	if(w * h == 0)
		return;

	width  = w;
	height = h;
	data.resize(w * h);
	memcpy(data.data(), image.data(), w * h * 4);
}

void Bitmap::save(string filename)
{
	lodepng::encode(filename, (byte*) data.data(), width, height);
}

// This function can be used to compare gradient rendering between different browsers.
byte max_color_diff(const Bitmap& a, const Bitmap& b)
{
	if(a.width != b.width || a.height != b.height)
		return 255;

	int diff = 0;
	for(int y = 0; y < a.height; y++)
		for(int x = 0; x < a.width; x++)
		{
			color A = a.get_pixel(x, y);
			color B = b.get_pixel(x, y);
			diff	= max({diff, abs(A.r - B.r), abs(A.g - B.g), abs(A.b - B.b), abs(A.a - B.a)});
		}
	return (byte) diff;
}
