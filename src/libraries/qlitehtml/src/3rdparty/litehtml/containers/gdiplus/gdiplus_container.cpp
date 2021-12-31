#include "gdiplus_container.h"

gdiplus_container::gdiplus_container(void)
{
}

gdiplus_container::~gdiplus_container(void)
{
}

void gdiplus_container::draw_ellipse( HDC hdc, int x, int y, int width, int height, const litehtml::web_color& color, int line_width )
{
	Gdiplus::Graphics graphics(hdc);
	Gdiplus::LinearGradientBrush* brush = NULL;

	graphics.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	Gdiplus::Pen pen( Gdiplus::Color(color.alpha, color.red, color.green, color.blue) );
	graphics.DrawEllipse(&pen, x, y, width, height);
}

void gdiplus_container::fill_ellipse( HDC hdc, int x, int y, int width, int height, const litehtml::web_color& color )
{
	Gdiplus::Graphics graphics(hdc);

	graphics.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	Gdiplus::SolidBrush brush( Gdiplus::Color(color.alpha, color.red, color.green, color.blue) );
	graphics.FillEllipse(&brush, x, y, width, height);
}

void gdiplus_container::fill_rect( HDC hdc, int x, int y, int width, int height, const litehtml::web_color& color, const litehtml::css_border_radius& radius )
{
	Gdiplus::Graphics graphics(hdc);

	Gdiplus::SolidBrush brush( Gdiplus::Color(color.alpha, color.red, color.green, color.blue) );
	graphics.FillRectangle(&brush, x, y, width, height);
}

void gdiplus_container::get_img_size( litehtml::uint_ptr img, litehtml::size& sz )
{
	Gdiplus::Bitmap* bmp = (Gdiplus::Bitmap*) img;
	if(bmp)
	{
		sz.width	= bmp->GetWidth();
		sz.height	= bmp->GetHeight();
	}
}

void gdiplus_container::draw_img(HDC hdc, litehtml::uint_ptr img, const litehtml::position& pos )
{
	Gdiplus::Bitmap* bmp = (Gdiplus::Bitmap*) img;
	if(bmp)
	{
		Gdiplus::Graphics graphics(hdc);
		graphics.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
		graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
		graphics.DrawImage(bmp, pos.x, pos.y, pos.width, pos.height);
	}
}

void gdiplus_container::free_image( litehtml::uint_ptr img )
{
	Gdiplus::Bitmap* bmp = (Gdiplus::Bitmap*) img;
	if(bmp)
	{
		delete bmp;
	}
}

void gdiplus_container::draw_img_bg( HDC hdc, litehtml::uint_ptr img, const litehtml::position& draw_pos, const litehtml::position& pos, litehtml::background_repeat repeat, litehtml::background_attachment attachment )
{
	Gdiplus::Bitmap* bgbmp = (Gdiplus::Bitmap*) img;

	int img_width	= bgbmp->GetWidth();
	int img_height	= bgbmp->GetHeight();

	Gdiplus::Graphics graphics(hdc);
	graphics.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
	graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);

	Gdiplus::Region reg(Gdiplus::Rect(draw_pos.left(), draw_pos.top(), draw_pos.width, draw_pos.height));
	graphics.SetClip(&reg);

	switch(repeat)
	{
	case litehtml::background_repeat_no_repeat:
		{
			graphics.DrawImage(bgbmp, pos.x, pos.y, bgbmp->GetWidth(), bgbmp->GetHeight());
		}
		break;
	case litehtml::background_repeat_repeat_x:
		{
			Gdiplus::CachedBitmap bmp(bgbmp, &graphics);
			for(int x = pos.left(); x < pos.right(); x += bgbmp->GetWidth())
			{
				graphics.DrawCachedBitmap(&bmp, x, pos.top());
			}

			for(int x = pos.left() - bgbmp->GetWidth(); x + (int) bgbmp->GetWidth() > draw_pos.left(); x -= bgbmp->GetWidth())
			{
				graphics.DrawCachedBitmap(&bmp, x, pos.top());
			}
		}
		break;
	case litehtml::background_repeat_repeat_y:
		{
			Gdiplus::CachedBitmap bmp(bgbmp, &graphics);
			for(int y = pos.top(); y < pos.bottom(); y += bgbmp->GetHeight())
			{
				graphics.DrawCachedBitmap(&bmp, pos.left(), y);
			}

			for(int y = pos.top() - bgbmp->GetHeight(); y + (int) bgbmp->GetHeight() > draw_pos.top(); y -= bgbmp->GetHeight())
			{
				graphics.DrawCachedBitmap(&bmp, pos.left(), y);
			}
		}
		break;
	case litehtml::background_repeat_repeat:
		{
			Gdiplus::CachedBitmap bmp(bgbmp, &graphics);
			if(bgbmp->GetHeight() >= 0)
			{
				for(int x = pos.left(); x < pos.right(); x += bgbmp->GetWidth())
				{
					for(int y = pos.top(); y < pos.bottom(); y += bgbmp->GetHeight())
					{
						graphics.DrawCachedBitmap(&bmp, x, y);
					}
				}
			}
		}
		break;
	}
}

void gdiplus_container::draw_borders( litehtml::uint_ptr hdc, const litehtml::css_borders& borders, const litehtml::position& draw_pos )
{
	apply_clip((HDC) hdc);

	// draw left border
	if(borders.left.width.val() != 0 && borders.left.style > litehtml::border_style_hidden)
	{
		HPEN pen = CreatePen(PS_SOLID, 1, RGB(borders.left.color.red, borders.left.color.green, borders.left.color.blue));
		HPEN oldPen = (HPEN) SelectObject((HDC) hdc, pen);
		for(int x = 0; x < borders.left.width.val(); x++)
		{
			MoveToEx((HDC) hdc, draw_pos.left() + x, draw_pos.top(), NULL);
			LineTo((HDC) hdc, draw_pos.left() + x, draw_pos.bottom());
		}
		SelectObject((HDC) hdc, oldPen);
		DeleteObject(pen);
	}
	// draw right border
	if(borders.right.width.val() != 0 && borders.right.style > litehtml::border_style_hidden)
	{
		HPEN pen = CreatePen(PS_SOLID, 1, RGB(borders.right.color.red, borders.right.color.green, borders.right.color.blue));
		HPEN oldPen = (HPEN) SelectObject((HDC) hdc, pen);
		for(int x = 0; x < borders.right.width.val(); x++)
		{
			MoveToEx((HDC) hdc, draw_pos.right() - x - 1, draw_pos.top(), NULL);
			LineTo((HDC) hdc, draw_pos.right() - x - 1, draw_pos.bottom());
		}
		SelectObject((HDC) hdc, oldPen);
		DeleteObject(pen);
	}
	// draw top border
	if(borders.top.width.val() != 0 && borders.top.style > litehtml::border_style_hidden)
	{
		HPEN pen = CreatePen(PS_SOLID, 1, RGB(borders.top.color.red, borders.top.color.green, borders.top.color.blue));
		HPEN oldPen = (HPEN) SelectObject((HDC) hdc, pen);
		for(int y = 0; y < borders.top.width.val(); y++)
		{
			MoveToEx((HDC) hdc, draw_pos.left(), draw_pos.top() + y, NULL);
			LineTo((HDC) hdc, draw_pos.right(), draw_pos.top() + y);
		}
		SelectObject((HDC) hdc, oldPen);
		DeleteObject(pen);
	}
	// draw bottom border
	if(borders.bottom.width.val() != 0 && borders.bottom.style > litehtml::border_style_hidden)
	{
		HPEN pen = CreatePen(PS_SOLID, 1, RGB(borders.bottom.color.red, borders.bottom.color.green, borders.bottom.color.blue));
		HPEN oldPen = (HPEN) SelectObject((HDC) hdc, pen);
		for(int y = 0; y < borders.bottom.width.val(); y++)
		{
			MoveToEx((HDC) hdc, draw_pos.left(), draw_pos.bottom() - y - 1, NULL);
			LineTo((HDC) hdc, draw_pos.right(), draw_pos.bottom() - y - 1);
		}
		SelectObject((HDC) hdc, oldPen);
		DeleteObject(pen);
	}

	release_clip((HDC) hdc);
}
