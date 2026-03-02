#ifndef LITEHTML_RENDER2PNG_H
#define LITEHTML_RENDER2PNG_H

#include <string>
#include <cairo.h>
#include <gdk/gdk.h>

namespace html2png
{
	class converter
	{
		int					  m_screen_width;
		int					  m_screen_height;
		double				  m_dpi;
		const char*			  m_default_font;
		cairo_font_options_t* m_font_options;

	  public:
		converter(int screen_width = 800, int screen_height = 600, double dpi = 96, const char* default_font = nullptr,
				  cairo_font_options_t* font_options = nullptr);
		bool	   to_png(const std::string& html_file, const std::string& png_file);
		GdkPixbuf* to_pixbuf(const std::string& html_file);

		int					  get_screen_width() const { return m_screen_width; }
		int					  get_screen_height() const { return m_screen_height; }
		const char*			  get_default_font() const { return m_default_font; }
		double				  get_dpi() const { return m_dpi; }
		cairo_font_options_t* get_font_options() { return m_font_options; }
	};

	enum png_diff_t
	{
		png_diff_same,
		png_diff_dimensions,
		png_diff_data,
		png_diff_error,
	};

	extern png_diff_t pngcmp(const std::string& png_file1, const std::string& png_file2);
	extern png_diff_t pngcmp(const GdkPixbuf* pixbuf1, const GdkPixbuf* pixbuf2);
	extern png_diff_t pngcmp(const GdkPixbuf* pixbuf, const std::string& png_file);
} // namespace html2png

#endif // LITEHTML_RENDER2PNG_H
