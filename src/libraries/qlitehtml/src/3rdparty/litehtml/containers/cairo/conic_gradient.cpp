#include "conic_gradient.h"

#define INTERPOLATE_COLOR(C1, C2, t) (((C2) - (C1)) * (t) + (C1))

static void sector_patch(cairo_pattern_t* pattern, double radius, double angle_A, double A_r, double A_g, double A_b,
						 double A_a, double angle_B, double B_r, double B_g, double B_b, double B_a)
{
	double r_sin_A, r_cos_A;
	double r_sin_B, r_cos_B;
	double h;

	r_sin_A	  = radius * sin(angle_A);
	r_cos_A	  = radius * cos(angle_A);
	r_sin_B	  = radius * sin(angle_B);
	r_cos_B	  = radius * cos(angle_B);

	h		  = 4.0 / 3.0 * tan((angle_B - angle_A) / 4.0);

	double x0 = r_cos_A;
	double y0 = r_sin_A;
	double x1 = r_cos_A - h * r_sin_A;
	double y1 = r_sin_A + h * r_cos_A;
	double x2 = r_cos_B + h * r_sin_B;
	double y2 = r_sin_B - h * r_cos_B;
	double x3 = r_cos_B;
	double y3 = r_sin_B;

	cairo_mesh_pattern_begin_patch(pattern);

	cairo_mesh_pattern_move_to(pattern, 0, 0);
	cairo_mesh_pattern_line_to(pattern, x0, y0);

	cairo_mesh_pattern_curve_to(pattern, x1, y1, x2, y2, x3, y3);
	cairo_mesh_pattern_line_to(pattern, 0, 0);

	cairo_mesh_pattern_set_corner_color_rgba(pattern, 0, A_r, A_g, A_b, A_a);
	cairo_mesh_pattern_set_corner_color_rgba(pattern, 1, A_r, A_g, A_b, A_a);
	cairo_mesh_pattern_set_corner_color_rgba(pattern, 2, B_r, B_g, B_b, B_a);
	cairo_mesh_pattern_set_corner_color_rgba(pattern, 3, B_r, B_g, B_b, B_a);

	cairo_mesh_pattern_end_patch(pattern);
}

cairo_pattern_t* create_conic_gradient_pattern(double angle, double radius,
											   const std::vector<bg_color_point>& color_points)
{
	if(color_points.empty())
	{
		return nullptr;
	}

	if(color_points.size() == 2)
	{
		std::vector<bg_color_point> points;
		points.push_back(color_points[0]);
		bg_color_point cp;
		cp.offset	 = 0.5f;
		cp.color.red = (litehtml::byte) INTERPOLATE_COLOR(color_points[0].color.red, color_points[1].color.red, 0.5f);
		cp.color.green =
			(litehtml::byte) INTERPOLATE_COLOR(color_points[0].color.green, color_points[1].color.green, 0.5f);
		cp.color.blue =
			(litehtml::byte) INTERPOLATE_COLOR(color_points[0].color.blue, color_points[1].color.blue, 0.5f);
		cp.color.alpha =
			(litehtml::byte) INTERPOLATE_COLOR(color_points[0].color.alpha, color_points[1].color.alpha, 0.5f);
		points.push_back(cp);
		points.push_back(color_points[1]);
		return create_conic_gradient_pattern(angle, radius, points);
	}

	const double two_pi		 = 2.0 * M_PI;

	cairo_pattern_t* pattern = cairo_pattern_create_mesh();

	for(size_t i = 0; i < color_points.size() - 1; ++i)
	{
		const bg_color_point& cp_A	  = color_points[i];
		const bg_color_point& cp_B	  = color_points[i + 1];
		double				  angle_A = cp_A.offset * two_pi + angle;
		double				  angle_B = color_points[i + 1].offset * two_pi + angle;

		double A_r					  = cp_A.color.red / 255.0;
		double A_g					  = cp_A.color.green / 255.0;
		double A_b					  = cp_A.color.blue / 255.0;
		double A_a					  = cp_A.color.alpha / 255.0;

		double B_r					  = cp_B.color.red / 255.0;
		double B_g					  = cp_B.color.green / 255.0;
		double B_b					  = cp_B.color.blue / 255.0;
		double B_a					  = cp_B.color.alpha / 255.0;

		sector_patch(pattern, radius, angle_A, A_r, A_g, A_b, A_a, angle_B, B_r, B_g, B_b, B_a);
	}

	return pattern;
}
