#include "test_container.h"
#include "Font.h"
#include "litehtml/types.h"
#include <fstream>
#include <sstream>
#define CANVAS_ITY_IMPLEMENTATION
#include "canvas_ity.hpp"

string readfile(string filename)
{
	std::stringstream ss;
	std::ifstream(filename) >> ss.rdbuf();
	return ss.str();
}

//
//  canvas_ity adapters
//

void set_color(canvas& cvs, brush_type type, color c)
{
	cvs.set_color(type, c.r / 255.f, c.g / 255.f, c.b / 255.f, c.a / 255.f);
}

void fill_rect(canvas& cvs, rect r)
{
	cvs.fill_rectangle((float) r.x, (float) r.y, (float) r.width, (float) r.height);
}

void fill_rect(canvas& cvs, rect r, color color)
{
	set_color(cvs, fill_style, color);
	fill_rect(cvs, r);
}

void fill_circle(canvas& cvs, rect rc, color color)
{
	float r = min(rc.width, rc.height) / 2.f;
	float x = rc.x + rc.width / 2.f;
	float y = rc.y + rc.height / 2.f;
	set_color(cvs, fill_style, color);
	cvs.begin_path();
	cvs.arc(x, y, r, 0, 2 * pi);
	cvs.fill();
}

void draw_circle(canvas& cvs, rect rc, color color)
{
	float r = min(rc.width, rc.height) / 2.f - .5f;
	float x = rc.x + rc.width / 2.f;
	float y = rc.y + rc.height / 2.f;
	set_color(cvs, stroke_style, color);
	cvs.begin_path();
	cvs.arc(x, y, r, 0, 2 * pi);
	cvs.stroke();
}

void clip_rect(canvas& cvs, rect r)
{
	cvs.begin_path();
	cvs.rectangle((float) r.x, (float) r.y, (float) r.width, (float) r.height);
	cvs.clip();
}

// without scaling
void draw_image(canvas& cvs, pixel_t x, pixel_t y, const Bitmap& bmp)
{
	cvs.draw_image((byte*) bmp.data.data(), bmp.width, bmp.height, bmp.width * 4, (float) x, (float) y,
				   (float) bmp.width, (float) bmp.height);
}

// with scaling
void draw_image(canvas& cvs, rect rc, const Bitmap& bmp)
{
	cvs.draw_image((byte*) bmp.data.data(), bmp.width, bmp.height, bmp.width * 4, (float) rc.x, (float) rc.y,
				   (float) rc.width, (float) rc.height);
}

void add_color_stop(canvas& cvs, brush_type type, float offset, color c, optional<float> hint)
{
	cvs.add_color_stop(type, offset, c.r / 255.f, c.g / 255.f, c.b / 255.f, c.a / 255.f, hint);
}

bool set_font(canvas& cvs, const string& raw_font_data, int pixel_size)
{
	return cvs.set_font((byte*) raw_font_data.data(), (int) raw_font_data.size(), (float) pixel_size);
}

void fill_polygon(canvas& cvs, vector<xy> points, color color)
{
	cvs.begin_path();
	cvs.polygon(points);
	set_color(cvs, fill_style, color);
	cvs.fill();
}

//
//  test_container implementation
//

uint_ptr test_container::create_font(const font_description& descr, const document*, litehtml::font_metrics* fm)
{
	Font*		  font	= 0;
	string_vector fonts = split_string(descr.family, ",", "", "");
	for(auto name : fonts)
	{
		font = Font::create(name, (int) descr.size, descr.weight);
		if(font)
			break;
	}
	if(!font)
		font = Font::create(get_default_font_name(), (int) descr.size, descr.weight);

	if(fm)
		*fm = *font;

	return (uint_ptr) font;
}

pixel_t test_container::text_width(const char* text, uint_ptr hFont)
{
	Font* font = (Font*) hFont;
	return font->text_width(text);
}

void test_container::draw_text(uint_ptr hdc, const char* text, uint_ptr hFont, web_color color, const position& pos)
{
	Font* font = (Font*) hFont;
	font->draw_text(*(canvas*) hdc, text, color, (int) pos.x, (int) pos.y);
}

pixel_t test_container::pt_to_px(float pt) const
{
	return pt * 96 / 72;
}
pixel_t test_container::get_default_font_size() const
{
	return 16;
}
const char* test_container::get_default_font_name() const
{
	return "Terminus";
}

void test_container::draw_solid_fill(uint_ptr hdc, const background_layer& layer, const web_color& color)
{
	auto cvs = (canvas*) hdc;
	fill_rect(*cvs, layer.border_box, color);
}

void test_container::draw_borders(uint_ptr hdc, const borders& borders, const position& pos, bool /*root*/)
{
	canvas img((int) pos.width, (int) pos.height);
	img.global_composite_operation = lighter;

	/*
		A_________________B
		 |\             /|
		 | \           / |
		 |  \_________/  |
		 |  |a       b|  |
		 |  |         |  |
		 |  |         |  |
		 |  |d_______c|  |
		 |  /         \  |
		 | /           \ |
		 |/_____________\|
		D                 C
	*/
	float width_				   = (float) pos.width;
	float height_				   = (float) pos.height;

	float left_width			   = (float) borders.left.width;
	float right_width			   = (float) borders.right.width;
	float top_width				   = (float) borders.top.width;
	float bottom_width			   = (float) borders.bottom.width;

	xy A						   = {0, 0};
	xy B						   = {width_, 0};
	xy C						   = {width_, height_};
	xy D						   = {0, height_};

	xy a						   = A + xy(left_width, top_width);
	xy b						   = B + xy(-right_width, top_width);
	xy c						   = C + xy(-right_width, -bottom_width);
	xy d						   = D + xy(left_width, -bottom_width);

	fill_polygon(img, {A, B, b, a}, borders.top.color);
	fill_polygon(img, {B, C, c, b}, borders.right.color);
	fill_polygon(img, {C, D, d, c}, borders.bottom.color);
	fill_polygon(img, {D, A, a, d}, borders.left.color);

	::draw_image(*(canvas*) hdc, pos.x, pos.y, img);
}

void test_container::draw_list_marker(uint_ptr hdc, const list_marker& marker)
{
	auto& cvs = *(canvas*) hdc;

	if(marker.image != "")
	{
		string url = make_url(marker.image.c_str(), marker.baseurl);
		auto&  img = images[url];
		if(img)
		{
			::draw_image(cvs, marker.pos, img);
			return;
		}
	}

	switch(marker.marker_type)
	{
	case list_style_type_circle:
		draw_circle(cvs, marker.pos, marker.color);
		break;

	case list_style_type_disc:
		fill_circle(cvs, marker.pos, marker.color);
		break;

	case list_style_type_square:
		fill_rect(cvs, marker.pos, marker.color);
		break;

	default:
		// do nothing
		break;
	}
}

string getdir(string filename)
{
	auto i = filename.find_last_of("\\/");
	return filename.substr(0, i);
}

string test_container::make_url(const char* src, const char* baseurl)
{
	return (baseurl && *baseurl ? getdir(baseurl) : basedir) + "/" + src;
}

void test_container::import_css(string& text, const string& url, string& baseurl)
{
	baseurl = make_url(url.c_str(), baseurl.c_str());
	text	= readfile(baseurl);
}

void test_container::get_viewport(position& client) const
{
	client = {0, 0, (pixel_t) width, (pixel_t) height};
}

void test_container::get_media_features(media_features& media) const
{
	position client;
	get_viewport(client);
	media.type		  = media_type_screen;
	media.width		  = client.width;
	media.height	  = client.height;
	media.color		  = 8;	// same as Chrome/Firefox
	media.monochrome  = 0;	// same as Chrome/Firefox
	media.color_index = 0;	// same as Chrome/Firefox
	media.resolution  = 96; // same as Chrome/Firefox
}

void test_container::load_image(const char* src, const char* baseurl, bool /*redraw_on_ready*/)
{
	string url	= make_url(src, baseurl);
	images[url] = Bitmap(url);
}

void test_container::get_image_size(const char* src, const char* baseurl, size& sz)
{
	string url = make_url(src, baseurl);
	auto&  img = images[url];
	sz		   = {(pixel_t) img.width, (pixel_t) img.height};
}

void draw_image_pattern(canvas& cvs, const background_layer& bg, const Bitmap& img)
{
	cvs.save();
	clip_rect(cvs, bg.clip_box);

	pixel_t x = bg.origin_box.x;
	pixel_t y = bg.origin_box.y;
	pixel_t w = bg.origin_box.width;
	pixel_t h = bg.origin_box.height;

	switch(bg.repeat)
	{
	case background_repeat_no_repeat:
		draw_image(cvs, {x, y, w, h}, img);
		break;

	case background_repeat_repeat_x:
		while(x > bg.clip_box.left())
			x -= w;
		for(; x < bg.clip_box.right(); x += w)
			draw_image(cvs, {x, y, w, h}, img);
		break;

	case background_repeat_repeat_y:
		while(y > bg.clip_box.top())
			y -= h;
		for(; y < bg.clip_box.bottom(); y += h)
			draw_image(cvs, {x, y, w, h}, img);
		break;

	case background_repeat_repeat:
		while(x > bg.clip_box.left())
			x -= w;
		while(y > bg.clip_box.top())
			y -= h;
		for(; x < bg.clip_box.right(); x += w)
			for(pixel_t _y = y; _y < bg.clip_box.bottom(); _y += h)
				draw_image(cvs, {x, _y, w, h}, img);
		break;
	}
	cvs.restore();
}

void test_container::draw_image(uint_ptr hdc, const background_layer& bg, const string& src, const string& base_url)
{
	auto&  cvs = *(canvas*) hdc;
	string url = make_url(src.c_str(), base_url.c_str());
	auto&  img = images[url];
	if(!img)
		return;

	draw_image_pattern(cvs, bg, img);
}

void set_gradient(canvas& cvs, const background_layer::linear_gradient& gradient, int origin_x, int origin_y)
{
	cvs.set_linear_gradient(fill_style, gradient.start.x - origin_x, gradient.start.y - origin_y,
							gradient.end.x - origin_x, gradient.end.y - origin_y);
}
void set_gradient(canvas& cvs, const background_layer::radial_gradient& gradient, int origin_x, int origin_y)
{
	cvs.set_css_radial_gradient(fill_style, gradient.position.x - origin_x, gradient.position.y - origin_y,
								gradient.radius.x, gradient.radius.y);
}
void set_gradient(canvas& cvs, const background_layer::conic_gradient& gradient, int origin_x, int origin_y)
{
	cvs.set_conic_gradient(fill_style, gradient.position.x - origin_x, gradient.position.y - origin_y, gradient.angle);
}

template <class Gradient> void draw_gradient(uint_ptr hdc, const background_layer& bg, const Gradient& gradient)
{
	pixel_t x = bg.origin_box.x;
	pixel_t y = bg.origin_box.y;
	pixel_t w = bg.origin_box.width;
	pixel_t h = bg.origin_box.height;

	canvas img((int) w, (int) h);

	set_gradient(img, gradient, (int) x, (int) y);

	for(auto cs : gradient.color_points)
		add_color_stop(img, fill_style, cs.offset, cs.color, cs.hint);

	fill_rect(img, {0, 0, w, h});

	draw_image_pattern(*(canvas*) hdc, bg, img);
}

void test_container::draw_linear_gradient(uint_ptr hdc, const background_layer& layer,
										  const background_layer::linear_gradient& gradient)
{
	draw_gradient(hdc, layer, gradient);
}

void test_container::draw_radial_gradient(uint_ptr hdc, const background_layer& layer,
										  const background_layer::radial_gradient& gradient)
{
	draw_gradient(hdc, layer, gradient);
}

void test_container::draw_conic_gradient(uint_ptr hdc, const background_layer& layer,
										 const background_layer::conic_gradient& gradient)
{
	draw_gradient(hdc, layer, gradient);
}
