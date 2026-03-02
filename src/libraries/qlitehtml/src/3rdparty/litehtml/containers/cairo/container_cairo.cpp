#include "container_cairo.h"
#include "cairo_borders.h"
#include "conic_gradient.h"
#include "litehtml/html.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

litehtml::pixel_t container_cairo::pt_to_px(float pt) const
{
	double dpi = get_screen_dpi();

	return pt * dpi / 72.0;
}

litehtml::pixel_t container_cairo::get_default_font_size() const
{
	return pt_to_px(12);
}

void container_cairo::draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker)
{
	if(!marker.image.empty())
	{
		litehtml::string url;
		make_url(marker.image.c_str(), marker.baseurl, url);

		auto img = get_image(url);
		if(img)
		{
			draw_pixbuf((cairo_t*) hdc, img, marker.pos.x, marker.pos.y, cairo_image_surface_get_width(img),
						cairo_image_surface_get_height(img));
			cairo_surface_destroy(img);
		}
	} else
	{
		switch(marker.marker_type)
		{
		case litehtml::list_style_type_circle:
			{
				draw_ellipse((cairo_t*) hdc, marker.pos.x, marker.pos.y, marker.pos.width, marker.pos.height,
							 marker.color, 1);
			}
			break;
		case litehtml::list_style_type_disc:
			{
				fill_ellipse((cairo_t*) hdc, marker.pos.x, marker.pos.y, marker.pos.width, marker.pos.height,
							 marker.color);
			}
			break;
		case litehtml::list_style_type_square:
			if(hdc)
			{
				auto* cr = (cairo_t*) hdc;
				cairo_save(cr);

				cairo_new_path(cr);
				cairo_rectangle(cr, marker.pos.x, marker.pos.y, marker.pos.width, marker.pos.height);

				set_color(cr, marker.color);
				cairo_fill(cr);
				cairo_restore(cr);
			}
			break;
		default:
			/*do nothing*/
			break;
		}
	}
}

void container_cairo::get_image_size(const char* src, const char* baseurl, litehtml::size& sz)
{
	litehtml::string url;
	make_url(src, baseurl, url);

	auto img = get_image(url);
	if(img)
	{
		sz.width  = cairo_image_surface_get_width(img);
		sz.height = cairo_image_surface_get_height(img);
		cairo_surface_destroy(img);
	} else
	{
		sz.width  = 0;
		sz.height = 0;
	}
}

void container_cairo::clip_background_layer(cairo_t* cr, const litehtml::background_layer& layer)
{
	rounded_rectangle(cr, layer.border_box, layer.border_radius);
	cairo_clip(cr);

	cairo_rectangle(cr, layer.clip_box.x, layer.clip_box.y, layer.clip_box.width, layer.clip_box.height);
	cairo_clip(cr);
}

void container_cairo::draw_image(litehtml::uint_ptr hdc, const litehtml::background_layer& layer,
								 const std::string& url, const std::string& base_url)
{
	if(url.empty() || (layer.clip_box.width == 0 && layer.clip_box.height == 0))
	{
		return;
	}

	auto* cr = (cairo_t*) hdc;
	cairo_save(cr);
	apply_clip(cr);

	clip_background_layer(cr, layer);

	std::string img_url;
	make_url(url.c_str(), base_url.c_str(), img_url);

	auto bgbmp = get_image(img_url);
	if(bgbmp)
	{
		int image_width	 = litehtml::round_f(layer.origin_box.width);
		int image_height = litehtml::round_f(layer.origin_box.height);

		if(image_width != cairo_image_surface_get_width(bgbmp) || image_height != cairo_image_surface_get_height(bgbmp))
		{
			auto new_img = scale_surface(bgbmp, image_width, image_height);
			cairo_surface_destroy(bgbmp);
			bgbmp = new_img;
		}

		cairo_pattern_t* pattern = cairo_pattern_create_for_surface(bgbmp);
		cairo_matrix_t	 flib_m;
		cairo_matrix_init_identity(&flib_m);
		cairo_matrix_translate(&flib_m, -layer.origin_box.x, -layer.origin_box.y);
		cairo_pattern_set_extend(pattern, CAIRO_EXTEND_REPEAT);
		cairo_pattern_set_matrix(pattern, &flib_m);

		switch(layer.repeat)
		{
		case litehtml::background_repeat_no_repeat:
			draw_pixbuf(cr, bgbmp, layer.origin_box.x, layer.origin_box.y, cairo_image_surface_get_width(bgbmp),
						cairo_image_surface_get_height(bgbmp));
			break;

		case litehtml::background_repeat_repeat_x:
			cairo_set_source(cr, pattern);
			cairo_rectangle(cr, layer.clip_box.left(), layer.origin_box.y, layer.clip_box.width,
							cairo_image_surface_get_height(bgbmp));
			cairo_fill(cr);
			break;

		case litehtml::background_repeat_repeat_y:
			cairo_set_source(cr, pattern);
			cairo_rectangle(cr, layer.origin_box.x, layer.clip_box.top(), cairo_image_surface_get_width(bgbmp),
							layer.clip_box.height);
			cairo_fill(cr);
			break;

		case litehtml::background_repeat_repeat:
			cairo_set_source(cr, pattern);
			cairo_rectangle(cr, layer.clip_box.left(), layer.clip_box.top(), layer.clip_box.width,
							layer.clip_box.height);
			cairo_fill(cr);
			break;
		}

		cairo_pattern_destroy(pattern);
		cairo_surface_destroy(bgbmp);
	}
	cairo_restore(cr);
}

void container_cairo::draw_solid_fill(litehtml::uint_ptr hdc, const litehtml::background_layer& layer,
									  const litehtml::web_color& color)
{
	if(color == litehtml::web_color::transparent)
	{
		return;
	}

	auto* cr = (cairo_t*) hdc;
	cairo_save(cr);
	apply_clip(cr);

	clip_background_layer(cr, layer);

	set_color(cr, color);
	cairo_paint(cr);

	cairo_restore(cr);
}

/**
 * @brief Draw pattern using layer.repeat property.
 *
 * Pattern must be defined relatively to the (layer.origin_box.x, layer.origin_box.y) point.
 * Function calculates rectangles for repeat-x/repeat-y properties and transform pattern to the correct position.
 * Then call draw callback to draw single pattern.
 *
 * @param cr - cairo context
 * @param pattern - cairo pattern
 * @param layer - background layer
 * @param draw - pattern draw function
 */
static void draw_pattern(
	cairo_t* cr, cairo_pattern_t* pattern, const litehtml::background_layer& layer,
	const std::function<void(cairo_t* cr, cairo_pattern_t* pattern, litehtml::pixel_t x, litehtml::pixel_t y,
							 litehtml::pixel_t width, litehtml::pixel_t height)>& draw)
{
	litehtml::pixel_t start_x = layer.origin_box.x;
	int				  num_x	  = 1;
	litehtml::pixel_t start_y = layer.origin_box.y;
	int				  num_y	  = 1;
	if(layer.repeat == litehtml::background_repeat_repeat_x || layer.repeat == litehtml::background_repeat_repeat)
	{
		if(layer.origin_box.left() > layer.clip_box.left())
		{
			int num_left = (int) ((layer.origin_box.left() - layer.clip_box.left()) / layer.origin_box.width);
			if(layer.origin_box.left() - num_left * layer.origin_box.width > layer.clip_box.left())
			{
				num_left++;
			}
			start_x	 = layer.origin_box.left() - num_left * layer.origin_box.width;
			num_x	+= num_left;
		}
		if(layer.origin_box.right() < layer.clip_box.right())
		{
			int num_right = (int) ((layer.clip_box.right() - layer.origin_box.right()) / layer.origin_box.width);
			if(layer.origin_box.left() + num_right * layer.origin_box.width < layer.clip_box.right())
			{
				num_right++;
			}
			num_x += num_right;
		}
	}
	if(layer.repeat == litehtml::background_repeat_repeat_y || layer.repeat == litehtml::background_repeat_repeat)
	{
		if(layer.origin_box.top() > layer.clip_box.top())
		{
			int num_top = (int) ((layer.origin_box.top() - layer.clip_box.top()) / layer.origin_box.height);
			if(layer.origin_box.top() - num_top * layer.origin_box.height > layer.clip_box.top())
			{
				num_top++;
			}
			start_y	 = layer.origin_box.top() - num_top * layer.origin_box.height;
			num_y	+= num_top;
		}
		if(layer.origin_box.bottom() < layer.clip_box.bottom())
		{
			int num_bottom = (int) ((layer.clip_box.bottom() - layer.origin_box.bottom()) / layer.origin_box.height);
			if(layer.origin_box.bottom() + num_bottom * layer.origin_box.height < layer.clip_box.bottom())
			{
				num_bottom++;
			}
			num_y += num_bottom;
		}
	}

	for(int i_x = 0; i_x < num_x; i_x++)
	{
		for(int i_y = 0; i_y < num_y; i_y++)
		{
			cairo_matrix_t flib_m;
			cairo_matrix_init_translate(&flib_m, -(start_x + i_x * layer.origin_box.width),
										-(start_y + i_y * layer.origin_box.height));
			cairo_pattern_set_matrix(pattern, &flib_m);
			draw(cr, pattern, start_x + i_x * layer.origin_box.width, start_y + i_y * layer.origin_box.height,
				 layer.origin_box.width, layer.origin_box.height);
		}
	}
}

void container_cairo::draw_linear_gradient(litehtml::uint_ptr hdc, const litehtml::background_layer& layer,
										   const litehtml::background_layer::linear_gradient& gradient)
{
	auto* cr = (cairo_t*) hdc;
	cairo_save(cr);
	apply_clip(cr);

	clip_background_layer(cr, layer);

	// Translate pattern to the (layer.origin_box.x, layer.origin_box.y) point
	cairo_pattern_t* pattern = cairo_pattern_create_linear(
		gradient.start.x - (float) layer.origin_box.x, gradient.start.y - (float) layer.origin_box.y,
		gradient.end.x - (float) layer.origin_box.x, gradient.end.y - (float) layer.origin_box.y);

	for(const auto& color_stop : gradient.color_points)
	{
		cairo_pattern_add_color_stop_rgba(pattern, color_stop.offset, color_stop.color.red / 255.0,
										  color_stop.color.green / 255.0, color_stop.color.blue / 255.0,
										  color_stop.color.alpha / 255.0);
	}

	draw_pattern(cr, pattern, layer,
				 [](cairo_t* cr, cairo_pattern_t* pattern, litehtml::pixel_t x, litehtml::pixel_t y,
					litehtml::pixel_t width, litehtml::pixel_t height) {
					 cairo_set_source(cr, pattern);
					 cairo_rectangle(cr, x, y, width, height);
					 cairo_fill(cr);
				 });

	cairo_pattern_destroy(pattern);
	cairo_restore(cr);
}

void container_cairo::make_url(const char* url, const char* /*basepath*/, litehtml::string& out)
{
	out = url;
}

void container_cairo::add_path_arc(cairo_t* cr, double x, double y, double rx, double ry, double a1, double a2,
								   bool neg)
{
	if(rx > 0 && ry > 0)
	{

		cairo_save(cr);

		cairo_translate(cr, x, y);
		cairo_scale(cr, 1, ry / rx);
		cairo_translate(cr, -x, -y);

		if(neg)
		{
			cairo_arc_negative(cr, x, y, rx, a1, a2);
		} else
		{
			cairo_arc(cr, x, y, rx, a1, a2);
		}

		cairo_restore(cr);
	} else
	{
		cairo_move_to(cr, x, y);
	}
}

void container_cairo::draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders,
								   const litehtml::position& draw_pos, bool /*root*/)
{
	auto* cr = (cairo_t*) hdc;
	cairo_save(cr);
	apply_clip(cr);

	cairo_new_path(cr);

	litehtml::pixel_t bdr_top	 = 0;
	litehtml::pixel_t bdr_bottom = 0;
	litehtml::pixel_t bdr_left	 = 0;
	litehtml::pixel_t bdr_right	 = 0;

	if(borders.top.width != 0 && borders.top.style > litehtml::border_style_hidden)
	{
		bdr_top = borders.top.width;
	}
	if(borders.bottom.width != 0 && borders.bottom.style > litehtml::border_style_hidden)
	{
		bdr_bottom = borders.bottom.width;
	}
	if(borders.left.width != 0 && borders.left.style > litehtml::border_style_hidden)
	{
		bdr_left = borders.left.width;
	}
	if(borders.right.width != 0 && borders.right.style > litehtml::border_style_hidden)
	{
		bdr_right = borders.right.width;
	}

	// draw right border
	if(bdr_right != 0)
	{
		cairo_matrix_t save_matrix;
		cairo_get_matrix(cr, &save_matrix);
		cairo_translate(cr, draw_pos.left(), draw_pos.top());
		cairo_rotate(cr, M_PI);
		cairo_translate(cr, -draw_pos.left(), -draw_pos.top());

		cairo::border border(cr, draw_pos.left() - draw_pos.width, draw_pos.top() - draw_pos.height, draw_pos.top());
		border.real_side		   = cairo::border::right_side;
		border.color			   = borders.right.color;
		border.style			   = borders.right.style;
		border.border_width		   = bdr_right;
		border.top_border_width	   = bdr_bottom;
		border.bottom_border_width = bdr_top;
		border.radius_top_x		   = borders.radius.bottom_right_x;
		border.radius_top_y		   = borders.radius.bottom_right_y;
		border.radius_bottom_x	   = borders.radius.top_right_x;
		border.radius_bottom_y	   = borders.radius.top_right_y;
		border.draw_border();

		cairo_set_matrix(cr, &save_matrix);
	}

	// draw bottom border
	if(bdr_bottom != 0)
	{
		cairo_matrix_t save_matrix;
		cairo_get_matrix(cr, &save_matrix);
		cairo_translate(cr, draw_pos.left(), draw_pos.top());
		cairo_rotate(cr, -M_PI / 2.0);
		cairo_translate(cr, -draw_pos.left(), -draw_pos.top());

		cairo::border border(cr, draw_pos.left() - draw_pos.height, draw_pos.top(), draw_pos.top() + draw_pos.width);
		border.real_side		   = cairo::border::bottom_side;
		border.color			   = borders.bottom.color;
		border.style			   = borders.bottom.style;
		border.border_width		   = bdr_bottom;
		border.top_border_width	   = bdr_left;
		border.bottom_border_width = bdr_right;
		border.radius_top_x		   = borders.radius.bottom_left_x;
		border.radius_top_y		   = borders.radius.bottom_left_y;
		border.radius_bottom_x	   = borders.radius.bottom_right_x;
		border.radius_bottom_y	   = borders.radius.bottom_right_y;
		border.draw_border();

		cairo_set_matrix(cr, &save_matrix);
	}

	// draw top border
	if(bdr_top != 0)
	{
		cairo_matrix_t save_matrix;
		cairo_get_matrix(cr, &save_matrix);
		cairo_translate(cr, draw_pos.left(), draw_pos.top());
		cairo_rotate(cr, M_PI / 2.0);
		cairo_translate(cr, -draw_pos.left(), -draw_pos.top());

		cairo::border border(cr, draw_pos.left(), draw_pos.top() - draw_pos.width, draw_pos.top());
		border.real_side		   = cairo::border::top_side;
		border.color			   = borders.top.color;
		border.style			   = borders.top.style;
		border.border_width		   = bdr_top;
		border.top_border_width	   = bdr_right;
		border.bottom_border_width = bdr_left;
		border.radius_top_x		   = borders.radius.top_right_x;
		border.radius_top_y		   = borders.radius.top_right_y;
		border.radius_bottom_x	   = borders.radius.top_left_x;
		border.radius_bottom_y	   = borders.radius.top_left_y;
		border.draw_border();

		cairo_set_matrix(cr, &save_matrix);
	}

	// draw left border
	if(bdr_left != 0)
	{
		cairo::border border(cr, draw_pos.left(), draw_pos.top(), draw_pos.bottom());
		border.real_side		   = cairo::border::left_side;
		border.color			   = borders.left.color;
		border.style			   = borders.left.style;
		border.border_width		   = bdr_left;
		border.top_border_width	   = bdr_top;
		border.bottom_border_width = bdr_bottom;
		border.radius_top_x		   = borders.radius.top_left_x;
		border.radius_top_y		   = borders.radius.top_left_y;
		border.radius_bottom_x	   = borders.radius.bottom_left_x;
		border.radius_bottom_y	   = borders.radius.bottom_left_y;
		border.draw_border();
	}
	cairo_restore(cr);
}

void container_cairo::transform_text(litehtml::string& /*text*/, litehtml::text_transform /*tt*/) {}

void container_cairo::set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius)
{
	m_clips.emplace_back(pos, bdr_radius);
}

void container_cairo::del_clip()
{
	if(!m_clips.empty())
	{
		m_clips.pop_back();
	}
}

void container_cairo::apply_clip(cairo_t* cr)
{
	for(const auto& clip_box : m_clips)
	{
		rounded_rectangle(cr, clip_box.box, clip_box.radius);
		cairo_clip(cr);
	}
}

void container_cairo::draw_ellipse(cairo_t* cr, litehtml::pixel_t x, litehtml::pixel_t y, litehtml::pixel_t width,
								   litehtml::pixel_t height, const litehtml::web_color& color,
								   litehtml::pixel_t line_width)
{
	if(!cr || width == 0 || height == 0)
		return;
	cairo_save(cr);

	apply_clip(cr);

	cairo_new_path(cr);

	cairo_translate(cr, x + width / 2.0, y + height / 2.0);
	cairo_scale(cr, width / 2.0, height / 2.0);
	cairo_arc(cr, 0, 0, 1, 0, 2 * M_PI);

	set_color(cr, color);
	cairo_set_line_width(cr, line_width);
	cairo_stroke(cr);

	cairo_restore(cr);
}

void container_cairo::fill_ellipse(cairo_t* cr, litehtml::pixel_t x, litehtml::pixel_t y, litehtml::pixel_t width,
								   litehtml::pixel_t height, const litehtml::web_color& color)
{
	if(!cr || width == 0 || height == 0)
		return;
	cairo_save(cr);

	apply_clip(cr);

	cairo_new_path(cr);

	cairo_translate(cr, x + width / 2.0, y + height / 2.0);
	cairo_scale(cr, width / 2.0, height / 2.0);
	cairo_arc(cr, 0, 0, 1, 0, 2 * M_PI);

	set_color(cr, color);
	cairo_fill(cr);

	cairo_restore(cr);
}

void container_cairo::clear_images()
{
	/*	for(images_map::iterator i = m_images.begin(); i != m_images.end(); i++)
		{
			if(i->second)
			{
				delete i->second;
			}
		}
		m_images.clear();
	*/
}

const char* container_cairo::get_default_font_name() const
{
	return "Times New Roman";
}

std::shared_ptr<litehtml::element> container_cairo::create_element(const char* /*tag_name*/,
																   const litehtml::string_map& /*attributes*/,
																   const std::shared_ptr<litehtml::document>& /*doc*/)
{
	return nullptr;
}

void container_cairo::rounded_rectangle(cairo_t* cr, const litehtml::position& pos,
										const litehtml::border_radiuses& radius)
{
	cairo_new_path(cr);
	if(radius.top_left_x != 0 && radius.top_left_y != 0)
	{
		add_path_arc(cr, pos.left() + radius.top_left_x, pos.top() + radius.top_left_y, radius.top_left_x,
					 radius.top_left_y, M_PI, M_PI * 3.0 / 2.0, false);
	} else
	{
		cairo_move_to(cr, pos.left(), pos.top());
	}

	cairo_line_to(cr, pos.right() - radius.top_right_x, pos.top());

	if(radius.top_right_x != 0 && radius.top_right_y != 0)
	{
		add_path_arc(cr, pos.right() - radius.top_right_x, pos.top() + radius.top_right_y, radius.top_right_x,
					 radius.top_right_y, M_PI * 3.0 / 2.0, 2.0 * M_PI, false);
	}

	cairo_line_to(cr, pos.right(), pos.bottom() - radius.bottom_right_x);

	if(radius.bottom_right_x != 0 && radius.bottom_right_y != 0)
	{
		add_path_arc(cr, pos.right() - radius.bottom_right_x, pos.bottom() - radius.bottom_right_y,
					 radius.bottom_right_x, radius.bottom_right_y, 0, M_PI / 2.0, false);
	}

	cairo_line_to(cr, pos.left() - radius.bottom_left_x, pos.bottom());

	if(radius.bottom_left_x != 0 && radius.bottom_left_y != 0)
	{
		add_path_arc(cr, pos.left() + radius.bottom_left_x, pos.bottom() - radius.bottom_left_y, radius.bottom_left_x,
					 radius.bottom_left_y, M_PI / 2.0, M_PI, false);
	}
}

cairo_surface_t* container_cairo::scale_surface(cairo_surface_t* surface, int width, int height)
{
	int				 s_width  = cairo_image_surface_get_width(surface);
	int				 s_height = cairo_image_surface_get_height(surface);
	cairo_surface_t* result	 = cairo_surface_create_similar(surface, cairo_surface_get_content(surface), width, height);
	cairo_pattern_t* pattern = cairo_pattern_create_for_surface(surface);
	cairo_t*		 cr		 = cairo_create(result);
	cairo_pattern_set_filter(pattern, CAIRO_FILTER_BILINEAR);
	cairo_pattern_set_extend(pattern, CAIRO_EXTEND_PAD);
	cairo_scale(cr, (double) width / (double) s_width, (double) height / (double) s_height);
	cairo_set_source(cr, pattern);
	cairo_rectangle(cr, 0, 0, s_width, s_height);
	cairo_fill(cr);
	cairo_destroy(cr);
	cairo_pattern_destroy(pattern);
	return result;
}

void container_cairo::draw_pixbuf(cairo_t* cr, cairo_surface_t* bmp, litehtml::pixel_t x, litehtml::pixel_t y, int cx,
								  int cy)
{
	cairo_save(cr);

	{
		cairo_matrix_t flip_m;
		cairo_matrix_init(&flip_m, 1, 0, 0, -1, 0, 0);

		if(cx != cairo_image_surface_get_width(bmp) || cy != cairo_image_surface_get_height(bmp))
		{
			auto bmp_scaled = scale_surface(bmp, cx, cy);
			cairo_set_source_surface(cr, bmp_scaled, x, y);
			cairo_paint(cr);
			cairo_surface_destroy(bmp_scaled);
		} else
		{
			cairo_set_source_surface(cr, bmp, x, y);
			cairo_paint(cr);
		}
	}

	cairo_restore(cr);
}

void container_cairo::get_media_features(litehtml::media_features& media) const
{
	litehtml::position viewport;
	get_viewport(viewport);
	media.type			= litehtml::media_type_screen;
	media.width			= viewport.width;
	media.height		= viewport.height;
	media.device_width	= get_screen_width();
	media.device_height = get_screen_height();
	media.color			= 8;
	media.monochrome	= 0;
	media.color_index	= 256;
	media.resolution	= 96;
}

void container_cairo::get_language(litehtml::string& language, litehtml::string& culture) const
{
	language = "en";
	culture	 = "";
}

void container_cairo::link(const std::shared_ptr<litehtml::document>& /*ptr*/, const litehtml::element::ptr& /*el*/) {}

void container_cairo::draw_radial_gradient(litehtml::uint_ptr hdc, const litehtml::background_layer& layer,
										   const litehtml::background_layer::radial_gradient& gradient)
{
	auto* cr = (cairo_t*) hdc;
	cairo_save(cr);
	apply_clip(cr);

	clip_background_layer(cr, layer);

	// Translate pattern to the (layer.origin_box.x, layer.origin_box.y) point
	litehtml::pointF position  = gradient.position;
	position.x				  -= (float) layer.origin_box.x;
	position.y				  -= (float) layer.origin_box.y;

	cairo_pattern_t* pattern =
		cairo_pattern_create_radial(position.x, position.y, 0, position.x, position.y, gradient.radius.x);

	for(const auto& color_stop : gradient.color_points)
	{
		cairo_pattern_add_color_stop_rgba(pattern, color_stop.offset, color_stop.color.red / 255.0,
										  color_stop.color.green / 255.0, color_stop.color.blue / 255.0,
										  color_stop.color.alpha / 255.0);
	}

	draw_pattern(cr, pattern, layer,
				 [&gradient, &position](cairo_t* cr, cairo_pattern_t* pattern, litehtml::pixel_t x, litehtml::pixel_t y,
										litehtml::pixel_t w, litehtml::pixel_t h) {
					 cairo_matrix_t save_matrix;
					 cairo_get_matrix(cr, &save_matrix);

					 auto top	 = (float) y;
					 auto height = (float) h;
					 if(gradient.radius.x != gradient.radius.y)
					 {
						 litehtml::pointF pos  = position;
						 pos.x				  += (float) x;
						 pos.y				  += (float) y;
						 // Scale height and top of the origin box
						 float aspect_ratio	   = gradient.radius.x / gradient.radius.y;
						 height				  *= aspect_ratio;
						 auto center_y		   = (pos.y - (float) y) * aspect_ratio;
						 top				   = pos.y - center_y;

						 cairo_translate(cr, pos.x, pos.y);
						 cairo_scale(cr, 1, gradient.radius.y / gradient.radius.x);
						 cairo_translate(cr, -pos.x, -pos.y);
					 }

					 cairo_set_source(cr, pattern);
					 cairo_rectangle(cr, x, top, w, height);
					 cairo_fill(cr);

					 cairo_set_matrix(cr, &save_matrix);
				 });

	cairo_pattern_destroy(pattern);
	cairo_restore(cr);
}

void container_cairo::draw_conic_gradient(litehtml::uint_ptr hdc, const litehtml::background_layer& layer,
										  const litehtml::background_layer::conic_gradient& gradient)
{
	auto* cr = (cairo_t*) hdc;
	cairo_save(cr);
	apply_clip(cr);

	clip_background_layer(cr, layer);

	cairo_pattern_t* pattern = create_conic_gradient_pattern(gradient.angle * M_PI / 180.0 - M_PI / 2.0,
															 gradient.radius, gradient.color_points);
	if(!pattern)
		return;

	// Translate a pattern to the (layer.origin_box.x, layer.origin_box.y) point
	litehtml::pointF position  = gradient.position;
	position.x				  -= (float) layer.origin_box.x;
	position.y				  -= (float) layer.origin_box.y;

	draw_pattern(cr, pattern, layer,
				 [&position](cairo_t* cr, cairo_pattern_t* pattern, litehtml::pixel_t x, litehtml::pixel_t y,
							 litehtml::pixel_t w, litehtml::pixel_t h) {
					 cairo_matrix_t flib_m;
					 cairo_matrix_init_translate(&flib_m, -(position.x + (float) x), -(position.y + (float) y));
					 cairo_pattern_set_matrix(pattern, &flib_m);

					 cairo_set_source(cr, pattern);
					 cairo_rectangle(cr, x, y, w, h);
					 cairo_fill(cr);
				 });

	cairo_pattern_destroy(pattern);
	cairo_restore(cr);
}
