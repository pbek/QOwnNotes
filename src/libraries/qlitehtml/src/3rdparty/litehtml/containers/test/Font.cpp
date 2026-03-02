#include "Bitmap.h"
#include "litehtml/types.h"
#define _CRT_SECURE_NO_WARNINGS
#include "Font.h"
#include "canvas_ity.hpp"
using namespace canvas_ity;
string readfile(string filename);
void   draw_image(canvas& cvs, pixel_t x, pixel_t y, const Bitmap& bmp);
bool   set_font(canvas& cvs, const string& raw_font_data, int pixel_size);
void   set_color(canvas& cvs, brush_type type, color c);

RasterFont::size_name RasterFont::installed_fonts[] = {
	{12, "terminus-12px.yaff"},
	{14, "terminus-14px.yaff"},
	{16, "terminus-16px.yaff"},
	{18, "terminus-18px.yaff"},
	{20, "terminus-20px.yaff"},
	{22, "terminus-22px.yaff"},
	{24, "terminus-24px.yaff"},
	{28, "terminus-28px.yaff"},
	{32, "terminus-32px.yaff"},
	{0,	""				  }
};

string get_font_dir()
{
	string font_cpp = __FILE__;
	auto   i		= font_cpp.find_last_of("\\/");
	return font_cpp.substr(0, i) + "/fonts/";
}

RasterFont::RasterFont(int size, int weight)
{
	// find most suitable font
	string name;
	if(size == 16 && weight == 700)
		name = "terminus-16px-bold.yaff";
	else
	{
		int min_diff = 1000;
		int n		 = 0;
		for(int i = 0; installed_fonts[i].size; i++)
		{
			int diff = abs(installed_fonts[i].size - size);
			if(diff < min_diff)
			{
				min_diff = diff;
				n		 = i;
			}
		}
		name = installed_fonts[n].name;
	}

	load(get_font_dir() + name);

	font_size	= size;
	x_height	= get_glyph('x', color(0, 0, 0, 255)).height;
	ch_width	= get_glyph('0', color(0, 0, 0, 255)).width;
	sub_shift	= (pixel_t) size / 5;
	super_shift = (pixel_t) size / 3;
}

RasterFont* RasterFont::create(string name, int size, int weight)
{
	if(lowcase(name) == "terminus")
		return new RasterFont(size, weight);

	return nullptr;
}

Bitmap RasterFont::get_glyph(int ch, color color)
{
	if(glyphs[ch].width == 0)
	{
		Bitmap bmp(width, (int) height, transparent);
		bmp.draw_rect(1, 1, width - 2, (int) height - 2, color);
		return bmp;
	} else if(color != black)
	{
		Bitmap bmp = glyphs[ch];
		bmp.replace_color(black, color);
		return bmp;
	} else
	{
		return glyphs[ch];
	}
}

// load .yaff font file in an ad hoc manner (can't parse arbitrary yaff files)
void RasterFont::load(string filename)
{
	string		  text	= readfile(filename);
	string_vector lines = split_string(text, "\n", "", "");

	int i;
	// parse header
	for(i = 0; i < (int) lines.size(); i++)
	{
		string line = lines[i];
		trim(line);
		if(line.empty() || line[0] == '#')
			continue; // skip empty lines and comments

		auto sep = line.find(':');
		if(sep == string::npos)
			return; // line without ':' - error

		auto key = line.substr(0, sep);
		trim(key);
		auto val = line.substr(sep + 1);
		trim(val);
		if(val.empty())
			break; // end of header

		if(key == "cell-size")
		{
			int parsed_height;
			sscanf(val.c_str(), "%d %d", &width, &parsed_height);
			height = (pixel_t) parsed_height;
		} else if(key == "ascent")
		{
			ascent = atoi(val.c_str());
		} else if(key == "descent")
		{
			descent = atoi(val.c_str());
		}
	}

	// parse glyphs

	// only u+NNNN: label is recognized, all others are skipped
	auto parse_key = [&]() {
		int ch = -1;
		for(; i < (int) lines.size(); i++)
		{
			string line = lines[i];
			trim(line);
			if(line.empty())
				continue;
			if(line.find(':') == string::npos)
				break; // start of glyph data
			if(line.substr(0, 2) == "u+")
				sscanf(line.c_str(), "u+%X:", (unsigned int*) &ch);
		}
		return ch;
	};

	auto parse_glyph = [&](int ch) {
		int		glyph_width = (int) trim(lines[i]).size();
		Bitmap& glyph = glyphs[ch] = Bitmap(glyph_width, (int) height, transparent);
		for(int y = 0; i < (int) lines.size() && y < height; i++, y++)
		{
			string line = trim(lines[i]);
			for(int x = 0; x < min((int) line.size(), glyph_width); x++)
			{
				if(line[x] == '@')
					glyph.set_pixel(x, y, black);
			}
		}
	};

	while(i < (int) lines.size())
	{
		int ch = parse_key();
		if(ch < 0)
			break;
		parse_glyph(ch);
	}

	x_height = glyphs[(int) 'x'].find_picture(transparent).height;
}

pixel_t RasterFont::text_width(string text)
{
	utf8_to_utf32 utf32(text);
	int			  width_ = 0;
	for(const char32_t* p = utf32; *p; p++)
		width_ += get_glyph(*p, black).width;
	return (pixel_t) width_;
}

void RasterFont::draw_text(canvas& cvs, string text, color color, int x, int y)
{
	utf8_to_utf32 utf32(text);
	for(const char32_t* p = utf32; *p; p++)
	{
		Bitmap glyph = get_glyph(*p, color);
		draw_image(cvs, x, y, glyph);
		x += glyph.width;
	}
}

// keys must be in lowcase
string_map OutlineFont::installed_fonts = {
	{"ahem", "ahem.ttf"}
};

OutlineFont* OutlineFont::create(string name, int size)
{
	lcase(name);

	if(installed_fonts.count(name))
		return new OutlineFont(name, size);

	return nullptr;
}

OutlineFont::OutlineFont(string name, int size) :
	name(name),
	size(size)
{
	string filename = at(installed_fonts, name);
	data			= readfile(get_font_dir() + filename);

	canvas cvs;
	set_font(cvs, data, size);

	int metric_ascent;
	int metric_descent;
	int metric_height;
	int metric_x_height;
	cvs.get_font_metrics(metric_ascent, metric_descent, metric_height, metric_x_height);

	ascent		= (pixel_t) metric_ascent;
	descent		= (pixel_t) metric_descent;
	height		= (pixel_t) metric_height;
	x_height	= (pixel_t) metric_x_height;
	sub_shift	= (pixel_t) size / 5;
	super_shift = (pixel_t) size / 3;
}

pixel_t OutlineFont::text_width(string text)
{
	canvas cvs;
	set_font(cvs, data, size);
	return cvs.measure_text(text.c_str());
}

void OutlineFont::draw_text(canvas& cvs, string text, color color, int x, int y)
{
	set_font(cvs, data, size);
	cvs.text_baseline = top;
	set_color(cvs, fill_style, color);
	cvs.fill_text(text.c_str(), (float) x, (float) y);
}

Font* Font::create(string name, int size, int weight)
{
	if(RasterFont* font = RasterFont::create(name, size, weight))
		return font;

	return OutlineFont::create(name, size);
}
