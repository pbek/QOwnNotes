#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <gdiplus.h>
#include "gdiplus_container.h"
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;
using namespace litehtml;

gdiplus_container::gdiplus_container()
{
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}

gdiplus_container::~gdiplus_container()
{
	clear_images();
	GdiplusShutdown(m_gdiplusToken);
}

static Color gdiplus_color(web_color color)
{
	return Color(color.alpha, color.red, color.green, color.blue);
}

void gdiplus_container::draw_ellipse(HDC hdc, litehtml::pixel_t x, litehtml::pixel_t y, litehtml::pixel_t width,
									 litehtml::pixel_t height, web_color color, int line_width)
{
	Graphics graphics(hdc);

	graphics.SetCompositingQuality(CompositingQualityHighQuality);
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);

	Pen pen(gdiplus_color(color));
	graphics.DrawEllipse(&pen, x, y, width, height);
}

void gdiplus_container::fill_ellipse(HDC hdc, litehtml::pixel_t x, litehtml::pixel_t y, litehtml::pixel_t width,
									 litehtml::pixel_t height, web_color color)
{
	Graphics graphics(hdc);

	graphics.SetCompositingQuality(CompositingQualityHighQuality);
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);

	SolidBrush brush(gdiplus_color(color));
	graphics.FillEllipse(&brush, x, y, width, height);
}

void gdiplus_container::fill_rect(HDC hdc, litehtml::pixel_t x, litehtml::pixel_t y, litehtml::pixel_t width,
								  litehtml::pixel_t height, web_color color)
{
	Graphics graphics(hdc);

	SolidBrush brush(gdiplus_color(color));
	graphics.FillRectangle(&brush, x, y, width, height);
}

void gdiplus_container::get_img_size(uint_ptr img, size& sz)
{
	Bitmap* bmp = (Bitmap*) img;
	if(bmp)
	{
		sz.width  = bmp->GetWidth();
		sz.height = bmp->GetHeight();
	}
}

void gdiplus_container::free_image(uint_ptr img)
{
	Bitmap* bmp = (Bitmap*) img;
	delete bmp;
}

void gdiplus_container::draw_img_bg(HDC hdc, uint_ptr img, const litehtml::background_layer& bg)
{
	Bitmap* bgbmp = (Bitmap*) img;

	Graphics graphics(hdc);
	graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
	graphics.SetPixelOffsetMode(PixelOffsetModeHalf);

	Region reg(Rect(bg.border_box.left(), bg.border_box.top(), bg.border_box.width, bg.border_box.height));
	graphics.SetClip(&reg);

	Bitmap* scaled_img = nullptr;
	if(bg.origin_box.width != bgbmp->GetWidth() || bg.origin_box.height != bgbmp->GetHeight())
	{
		scaled_img = new Bitmap(bg.origin_box.width, bg.origin_box.height);
		Graphics gr(scaled_img);
		gr.SetPixelOffsetMode(PixelOffsetModeHighQuality);
		gr.DrawImage(bgbmp, 0.0, 0.0, bg.origin_box.width, bg.origin_box.height);
		bgbmp = scaled_img;
	}

	switch(bg.repeat)
	{
	case background_repeat_no_repeat:
		{
			graphics.DrawImage(bgbmp, bg.origin_box.x, bg.origin_box.y, bgbmp->GetWidth(), bgbmp->GetHeight());
		}
		break;
	case background_repeat_repeat_x:
		{
			CachedBitmap bmp(bgbmp, &graphics);
			int			 x = bg.origin_box.x;
			while(x > bg.clip_box.left())
				x -= bgbmp->GetWidth();
			for(; x < bg.clip_box.right(); x += bgbmp->GetWidth())
			{
				graphics.DrawCachedBitmap(&bmp, x, bg.origin_box.y);
			}
		}
		break;
	case background_repeat_repeat_y:
		{
			CachedBitmap bmp(bgbmp, &graphics);
			int			 y = bg.origin_box.y;
			while(y > bg.clip_box.top())
				y -= bgbmp->GetHeight();
			for(; y < bg.clip_box.bottom(); y += bgbmp->GetHeight())
			{
				graphics.DrawCachedBitmap(&bmp, bg.origin_box.x, y);
			}
		}
		break;
	case background_repeat_repeat:
		{
			CachedBitmap bmp(bgbmp, &graphics);
			int			 x = bg.origin_box.x;
			while(x > bg.clip_box.left())
				x -= bgbmp->GetWidth();
			int y0 = bg.origin_box.y;
			while(y0 > bg.clip_box.top())
				y0 -= bgbmp->GetHeight();

			for(; x < bg.clip_box.right(); x += bgbmp->GetWidth())
			{
				for(int y = y0; y < bg.clip_box.bottom(); y += bgbmp->GetHeight())
				{
					graphics.DrawCachedBitmap(&bmp, x, y);
				}
			}
		}
		break;
	}

	delete scaled_img;
}

// length of dash and space for "dashed" style, in multiples of pen width
const float dash  = 3;
const float space = 2;

static void draw_horz_border(Graphics& graphics, const border& border, litehtml::pixel_t y, litehtml::pixel_t left,
							 litehtml::pixel_t right)
{
	if(border.style != border_style_double || border.width < 3)
	{
		if(border.width == 1)
			right -= 1; // 1px-wide lines are longer by one pixel in GDI+ (the endpoint is also drawn)
		Pen pen(gdiplus_color(border.color), (float) border.width);
		if(border.style == border_style_dotted)
		{
			float dashValues[2] = {1, 1};
			pen.SetDashPattern(dashValues, 2);
		} else if(border.style == border_style_dashed)
		{
			float dashValues[2] = {dash, space};
			pen.SetDashPattern(dashValues, 2);
		}
		graphics.DrawLine(&pen, PointF(left, y + border.width / 2), PointF(right, y + border.width / 2));
	} else
	{
		litehtml::pixel_t single_line_width = border.width / 3.0;
		if(single_line_width == 1)
			right -= 1;
		Pen pen(gdiplus_color(border.color), (float) single_line_width);
		graphics.DrawLine(&pen, PointF(left, y + single_line_width / 2), PointF(right, y + single_line_width / 2));
		graphics.DrawLine(&pen, PointF(left, y + border.width - 1 - single_line_width / 2),
						  PointF(right, y + border.width - 1 - single_line_width / 2));
	}
}

static void draw_vert_border(Graphics& graphics, const border& border, litehtml::pixel_t x, litehtml::pixel_t top,
							 litehtml::pixel_t bottom)
{
	if(border.style != border_style_double || border.width < 3)
	{
		if(border.width == 1)
			bottom--;
		Pen pen(gdiplus_color(border.color), (float) border.width);
		if(border.style == border_style_dotted)
		{
			float dashValues[2] = {1, 1};
			pen.SetDashPattern(dashValues, 2);
		} else if(border.style == border_style_dashed)
		{
			float dashValues[2] = {dash, space};
			pen.SetDashPattern(dashValues, 2);
		}
		graphics.DrawLine(&pen, PointF(x + border.width / 2, top), PointF(x + border.width / 2, bottom));
	} else
	{
		litehtml::pixel_t single_line_width = border.width / 3.0;
		if(single_line_width == 1)
			bottom--;
		Pen pen(gdiplus_color(border.color), (float) single_line_width);
		graphics.DrawLine(&pen, PointF(x + single_line_width / 2, top), PointF(x + single_line_width / 2, bottom));
		graphics.DrawLine(&pen, PointF(x + border.width - 1 - single_line_width / 2, top),
						  PointF(x + border.width - 1 - single_line_width / 2, bottom));
	}
}

void gdiplus_container::draw_borders(uint_ptr hdc, const borders& borders, const position& draw_pos, bool root)
{
	apply_clip((HDC) hdc);
	Graphics graphics((HDC) hdc);

	if(borders.left.width != 0)
	{
		draw_vert_border(graphics, borders.left, draw_pos.left(), draw_pos.top(), draw_pos.bottom());
	}
	if(borders.right.width != 0)
	{
		draw_vert_border(graphics, borders.right, draw_pos.right() - borders.right.width, draw_pos.top(),
						 draw_pos.bottom());
	}
	if(borders.top.width != 0)
	{
		draw_horz_border(graphics, borders.top, draw_pos.top(), draw_pos.left(), draw_pos.right());
	}
	if(borders.bottom.width != 0)
	{
		draw_horz_border(graphics, borders.bottom, draw_pos.bottom() - borders.bottom.width, draw_pos.left(),
						 draw_pos.right());
	}

	release_clip((HDC) hdc);
}
