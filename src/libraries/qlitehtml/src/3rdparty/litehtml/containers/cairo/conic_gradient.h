#ifndef LITEHTML_CONIC_GRADIENT_H
#define LITEHTML_CONIC_GRADIENT_H

#include <litehtml.h>
#include <cairo.h>

using bg_color_point = litehtml::background_layer::color_point;

cairo_pattern_t* create_conic_gradient_pattern(double angle, double radius,
											   const std::vector<bg_color_point>& color_points);

#endif // LITEHTML_CONIC_GRADIENT_H
