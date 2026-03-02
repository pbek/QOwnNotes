// canvas_ity v1.00 -- ISC license
// Copyright (c) 2022 Andrew Kensler
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

// ======== ABOUT ========
//
// This is a tiny, single-header C++ library for rasterizing immediate-mode
// 2D vector graphics, closely modeled on the basic W3C (not WHATWG) HTML5 2D
// canvas specification (https://www.w3.org/TR/2015/REC-2dcontext-20151119/).
//
// The priorities for this library are high-quality rendering, ease of use,
// and compact size.  Speed is important too, but secondary to the other
// priorities.  Notably, this library takes an opinionated approach and
// does not provide options for trading off quality for speed.
//
// Despite its small size, it supports nearly everything listed in the W3C
// HTML5 2D canvas specification, except for hit regions and getting certain
// properties.  The main differences lie in the surface-level API to make this
// easier for C++ use, while the underlying implementation is carefully based
// on the specification.  In particular, stroke, fill, gradient, pattern,
// image, and font styles are specified slightly differently (avoiding strings
// and auxiliary classes).  Nonetheless, the goal is that this library could
// produce a conforming HTML5 2D canvas implementation if wrapped in a thin
// layer of JavaScript bindings.  See the accompanying C++ automated test
// suite and its HTML5 port for a mapping between the APIs and a comparison
// of this library's rendering output against browser canvas implementations.

// ======== FEATURES ========
//
// High-quality rendering:
//
// - Trapezoidal area antialiasing provides very smooth antialiasing, even
//     when lines are nearly horizontal or vertical.
// - Gamma-correct blending, interpolation, and resampling are used
//     throughout.  It linearizes all colors and premultiplies alpha on
//     input and converts back to unpremultiplied sRGB on output.  This
//     reduces muddiness on many gradients (e.g., red to green), makes line
//     thicknesses more perceptually uniform, and avoids dark fringes when
//     interpolating opacity.
// - Bicubic convolution resampling is used whenever it needs to resample a
//     pattern or image.  This smoothly interpolates with less blockiness when
//     magnifying, and antialiases well when minifying.  It can simultaneously
//     magnify and minify along different axes.
// - Ordered dithering is used on output.  This reduces banding on subtle
//     gradients while still being compression-friendly.
// - High curvature is handled carefully in line joins.  Thick lines are
//     drawn correctly as though tracing with a wide pen nib, even where
//     the lines curve sharply.  (Simpler curve offsetting approaches tend
//     to show bite-like artifacts in these cases.)
//
// Ease of use:
//
// - Provided as a single-header library with no dependencies beside the
//     standard C++ library.  There is nothing to link, and it even includes
//     built-in binary parsing for TrueType font (TTF) files.  It is pure CPU
//     code and does not require a GPU or GPU context.
// - Has extensive Doxygen-style documentation comments for the public API.
// - Compiles cleanly at moderately high warning levels on most compilers.
// - Shares no internal pointers, nor holds any external pointers.  Newcomers
//     to C++ can have fun drawing with this library without worrying so much
//     about resource lifetimes or mutability.
// - Uses no static or global variables.  Threads may safely work with
//     different canvas instances concurrently without locking.
// - Allocates no dynamic memory after reaching the high-water mark.  Except
//     for the pixel buffer, flat std::vector instances embedded in the canvas
//     instance handle all dynamic memory.  This reduces fragmentation and
//     makes it easy to change the code to reserve memory up front or even
//     to use statically allocated vectors.
// - Works with exceptions and RTTI disabled.
// - Intentionally uses a plain C++03 style to make it as widely portable
//     as possible, easier to understand, and (with indexing preferred over
//     pointer arithmetic) easier to port natively to other languages.  The
//     accompanying test suite may also help with porting.
//
// Compact size:
//
// - The source code for the entire library consists of a bit over 2300 lines
//     (not counting comments or blanks), each no longer than 78 columns.
//     Alternately measured, it has fewer than 1300 semicolons.
// - The object code for the library can be less than 36 KiB on x86-64 with
//     appropriate compiler settings for size.
// - Due to the library's small size, the accompanying automated test suite
//     achieves 100% line coverage of the library in gcov and llvm-cov.

// ======== LIMITATIONS ========
//
// - Trapezoidal antialiasing overestimates coverage where paths self-
//     intersect within a single pixel.  Where inner joins are visible, this
//     can lead to a "grittier" appearance due to the extra windings used.
// - Clipping uses an antialiased sparse pixel mask rather than geometrically
//     intersecting paths.  Therefore, it is not subpixel-accurate.
// - Text rendering is extremely basic and mainly for convenience.  It only
//     supports left-to-right text, and does not do any hinting, kerning,
//     ligatures, text shaping, or text layout.  If you require any of those,
//     consider using another library to provide those and feed the results
//     to this library as either placed glyphs or raw paths.
// - TRUETYPE FONT PARSING IS NOT SECURE!  It does some basic validity
//     checking, but should only be used with known-good or sanitized fonts.
// - Parameter checking does not test for non-finite floating-point values.
// - Rendering is single-threaded, not explicitly vectorized, and not GPU-
//     accelerated.  It also copies data to avoid ownership issues.  If you
//     need the speed, you are better off using a more fully-featured library.
// - The library does no input or output on its own.  Instead, you must
//     provide it with buffers to copy into or out of.

// ======== USAGE ========
//
// This is a single-header library.  You may freely include it in any of
// your source files to declare the canvas_ity namespace and its members.
// However, to get the implementation, you must
//     #define CANVAS_ITY_IMPLEMENTATION
// in exactly one C++ file before including this header.
//
// Then, construct an instance of the canvas_ity::canvas class with the pixel
// dimensions that you want and draw into it using any of the various drawing
// functions.  You can then use the get_image_data() function to retrieve the
// currently drawn image at any time.
//
// See each of the public member function and data member (i.e., method
// and field) declarations for the full API documentation.  Also see the
// accompanying C++ automated test suite for examples of the usage of each
// public member, and the test suite's HTML5 port for how these map to the
// HTML5 canvas API.

#ifndef CANVAS_ITY_HPP
#define CANVAS_ITY_HPP

#include <cstddef>
#include <vector>
#include <optional>

namespace canvas_ity
{

// Public API enums
enum composite_operation {
    source_in = 1, source_copy, source_out, destination_in,
    destination_atop = 7,
    // confusing name, 'lighter' should be called 'plus'
    // https://www.w3.org/TR/compositing-1/#porterduffcompositingoperators_plus
    // RED_out = ALPHA_src * RED_src + ALPHA_dst * RED_dst
    // ALPHA_out = ALPHA_src + ALPHA_dst
    // https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/globalCompositeOperation
    // "Where both shapes overlap, the color is determined by adding color values."
    lighter = 10,
    destination_over, destination_out,
    source_atop, source_over, exclusive_or };
enum cap_style { butt, square, circle };
enum join_style { miter, bevel, rounded };
enum brush_type { fill_style, stroke_style };
enum repetition_style { repeat, repeat_x, repeat_y, no_repeat };
enum align_style { leftward, rightward, center, start = 0, ending };
enum baseline_style {
    alphabetic, top, middle, bottom, hanging, ideographic = 3 };

// Implementation details
struct xy { float x, y; xy(); xy( float, float ); };
struct rgba { float r, g, b, a; rgba(); rgba( float, float, float, float ); };
struct affine_matrix { float a, b, c, d, e, f; };
struct paint_brush { enum types { color, pattern, linear, radial, css_radial, conic } type;
                     std::vector< rgba > colors; std::vector< float > stops; std::vector< std::optional<float> > hints;
                     xy start, end; float start_radius, end_radius; xy css_radius; float angle;
                     int width, height; repetition_style repetition; };
struct font_face { std::vector< unsigned char > data;
                   int cmap, glyf, head, hhea, hmtx, loca, maxp, os_2;
                   float scale; };
struct subpath_data { size_t count; bool closed; };
struct bezier_path { std::vector< xy > points;
                     std::vector< subpath_data > subpaths; };
struct line_path { std::vector< xy > points;
                   std::vector< subpath_data > subpaths; };
struct pixel_run { unsigned short x, y; float delta; };
typedef std::vector< pixel_run > pixel_runs;

class canvas
{
public:

    // ======== LIFECYCLE ========

    /// @brief  Construct a new canvas.
    ///
    /// It will begin with all pixels set to transparent black.  Initially,
    /// the visible coordinates will run from (0, 0) in the upper-left to
    /// (width, height) in the lower-right and with pixel centers offset
    /// (0.5, 0.5) from the integer grid, though all this may be changed
    /// by transforms.  The sizes must be between 1 and 32768, inclusive.
    ///
    /// @param width  horizontal size of the new canvas in pixels
    /// @param height  vertical size of the new canvas in pixels
    ///
    canvas(
        int width,
        int height );

    canvas(
        int width,
        int height,
        rgba color);

    canvas() : canvas(0, 0) {}

    /// @brief  Destroy the canvas and release all associated memory.
    ///
    ~canvas();

    // ======== TRANSFORMS ========

    /// @brief  Scale the current transform.
    ///
    /// Scaling may be non-uniform if the x and y scaling factors are
    /// different.  When a scale factor is less than one, things will be
    /// shrunk in that direction.  When a scale factor is greater than
    /// one, they will grow bigger.  Negative scaling factors will flip or
    /// mirror it in that direction.  The scaling factors must be non-zero.
    /// If either is zero, most drawing operations will do nothing.
    ///
    /// @param x  horizontal scaling factor
    /// @param y  vertical scaling factor
    ///
    void scale(
        float x,
        float y );

    /// @brief  Rotate the current transform.
    ///
    /// The rotation is applied clockwise in a direction around the origin.
    ///
    /// Tip: to rotate around another point, first translate that point to
    ///      the origin, then do the rotation, and then translate back.
    ///
    /// @param angle  clockwise angle in radians
    ///
    void rotate(
        float angle );

    /// @brief  Translate the current transform.
    ///
    /// By default, positive x values shift that many pixels to the right,
    /// while negative y values shift left, positive y values shift up, and
    /// negative y values shift down.
    ///
    /// @param x  amount to shift horizontally
    /// @param y  amount to shift vertically
    ///
    void translate(
        float x,
        float y );

    /// @brief  Add an arbitrary transform to the current transform.
    ///
    /// This takes six values for the upper two rows of a homogeneous 3x3
    /// matrix (i.e., {{a, c, e}, {b, d, f}, {0.0, 0.0, 1.0}}) describing an
    /// arbitrary affine transform and appends it to the current transform.
    /// The values can represent any affine transform such as scaling,
    /// rotation, translation, or skew, or any composition of affine
    /// transforms.  The matrix must be invertible.  If it is not, most
    /// drawing operations will do nothing.
    ///
    /// @param a  horizontal scaling factor (m11)
    /// @param b  vertical skewing (m12)
    /// @param c  horizontal skewing (m21)
    /// @param d  vertical scaling factor (m22)
    /// @param e  horizontal translation (m31)
    /// @param f  vertical translation (m32)
    ///
    void transform(
        float a,
        float b,
        float c,
        float d,
        float e,
        float f );

    /// @brief  Replace the current transform.
    ///
    /// This takes six values for the upper two rows of a homogeneous 3x3
    /// matrix (i.e., {{a, c, e}, {b, d, f}, {0.0, 0.0, 1.0}}) describing
    /// an arbitrary affine transform and replaces the current transform
    /// with it.  The values can represent any affine transform such as
    /// scaling, rotation, translation, or skew, or any composition of
    /// affine transforms.  The matrix must be invertible.  If it is not,
    /// most drawing operations will do nothing.
    ///
    /// Tip: to reset the current transform back to the default, use
    ///      1.0, 0.0, 0.0, 1.0, 0.0, 0.0.
    ///
    /// @param a  horizontal scaling factor (m11)
    /// @param b  vertical skewing (m12)
    /// @param c  horizontal skewing (m21)
    /// @param d  vertical scaling factor (m22)
    /// @param e  horizontal translation (m31)
    /// @param f  vertical translation (m32)
    ///
    void set_transform(
        float a,
        float b,
        float c,
        float d,
        float e,
        float f );

    // ======== COMPOSITING ========

    /// @brief  Set the degree of opacity applied to all drawing operations.
    ///
    /// If an operation already uses a transparent color, this can make it
    /// yet more transparent.  It must be in the range from 0.0 for fully
    /// transparent to 1.0 for fully opaque, inclusive.  If it is not, this
    /// does nothing.  Defaults to 1.0 (opaque).
    ///
    /// @param alpha  degree of opacity applied to all drawing operations
    ///
    void set_global_alpha(
        float alpha );

    /// @brief  Compositing operation for blending new drawing and old pixels.
    ///
    /// The source_copy, source_in, source_out, destination_atop, and
    /// destination_in operations may clear parts of the canvas outside the
    /// new drawing but within the clip region.  Defaults to source_over.
    ///
    /// source_atop:       Show new over old where old is opaque.
    /// source_copy:       Replace old with new.
    /// source_in:         Replace old with new where old was opaque.
    /// source_out:        Replace old with new where old was transparent.
    /// source_over:       Show new over old.
    /// destination_atop:  Show old over new where new is opaque.
    /// destination_in:    Clear old where new is transparent.
    /// destination_out:   Clear old where new is opaque.
    /// destination_over:  Show new under old.
    /// exclusive_or:      Show new and old but clear where both are opaque.
    /// lighter:           Sum the new with the old.
    ///
    composite_operation global_composite_operation;

    // ======== SHADOWS ========

    /// @brief  Set the color and opacity of the shadow.
    ///
    /// Most drawing operations can optionally draw a blurred drop shadow
    /// before doing the main drawing.  The shadow is modulated by the opacity
    /// of the drawing and will be blended into the existing pixels subject to
    /// the compositing settings and clipping region.  Shadows will only be
    /// drawn if the shadow color has any opacity and the shadow is either
    /// offset or blurred.  The color and opacity values will be clamped to
    /// the 0.0 to 1.0 range, inclusive.  Defaults to 0.0, 0.0, 0.0, 0.0
    /// (transparent black).
    ///
    /// @param red    sRGB red component of the shadow color
    /// @param green  sRGB green component of the shadow color
    /// @param blue   sRGB blue component of the shadow color
    /// @param alpha  opacity of the shadow (not premultiplied)
    ///
    void set_shadow_color(
        float red,
        float green,
        float blue,
        float alpha );

    /// @brief  Horizontal offset of the shadow in pixels.
    ///
    /// Negative shifts left, positive shifts right.  This is not affected
    /// by the current transform.  Defaults to 0.0 (no offset).
    ///
    float shadow_offset_x;

    /// @brief  Vertical offset of the shadow in pixels.
    ///
    /// Negative shifts up, positive shifts down.  This is not affected by
    /// the current transform.  Defaults to 0.0 (no offset).
    ///
    float shadow_offset_y;

    /// @brief  Set the level of Gaussian blurring on the shadow.
    ///
    /// Zero produces no blur, while larger values will blur the shadow
    /// more strongly.  This is not affected by the current transform.
    /// Must be non-negative.  If it is not, this does nothing.  Defaults to
    /// 0.0 (no blur).
    ///
    /// @param level  the level of Gaussian blurring on the shadow
    ///
    void set_shadow_blur(
        float level );

    // ======== LINE STYLES ========

    /// @brief  Set the width of the lines when stroking.
    ///
    /// Initially this is measured in pixels, though the current transform
    /// at the time of drawing can affect this.  Must be positive.  If it
    /// is not, this does nothing.  Defaults to 1.0.
    ///
    /// @param width  width of the lines when stroking
    ///
    void set_line_width(
        float width );

    /// @brief  Cap style for the ends of open subpaths and dash segments.
    ///
    /// The actual shape may be affected by the current transform at the time
    /// of drawing.  Only affects stroking.  Defaults to butt.
    ///
    /// butt:    Use a flat cap flush to the end of the line.
    /// square:  Use a half-square cap that extends past the end of the line.
    /// circle:  Use a semicircular cap.
    ///
    cap_style line_cap;

    /// @brief  Join style for connecting lines within the paths.
    ///
    /// The actual shape may be affected by the current transform at the time
    /// of drawing.  Only affects stroking.  Defaults to miter.
    ///
    /// miter:  Continue the ends until they intersect, if within miter limit.
    /// bevel:  Connect the ends with a flat triangle.
    /// round:  Join the ends with a circular arc.
    ///
    join_style line_join;

    /// @brief  Set the limit on maximum pointiness allowed for miter joins.
    ///
    /// If the distance from the point where the lines intersect to the
    /// point where the outside edges of the join intersect exceeds this
    /// ratio relative to the line width, then a bevel join will be used
    /// instead, and the miter will be lopped off.  Larger values allow
    /// pointier miters.  Only affects stroking and only when the line join
    /// style is miter.  Must be positive.  If it is not, this does nothing.
    /// Defaults to 10.0.
    ///
    /// @param limit  the limit on maximum pointiness allowed for miter joins
    ///
    void set_miter_limit(
        float limit );

    /// @brief  Offset where each subpath starts the dash pattern.
    ///
    /// Changing this shifts the location of the dashes along the path and
    /// animating it will produce a marching ants effect.  Only affects
    /// stroking and only when a dash pattern is set.  May be negative or
    /// exceed the length of the dash pattern, in which case it will wrap.
    /// Defaults to 0.0.
    ///
    float line_dash_offset;

    /// @brief  Set or clear the line dash pattern.
    ///
    /// Takes an array with entries alternately giving the lengths of dash
    /// and gap segments.  All must be non-negative; if any are not, this
    /// does nothing.  These will be used to draw with dashed lines when
    /// stroking, with each subpath starting at the length along the dash
    /// pattern indicated by the line dash offset.  Initially these lengths
    /// are measured in pixels, though the current transform at the time of
    /// drawing can affect this.  The count must be non-negative.  If it is
    /// odd, the array will be appended to itself to make an even count.  If
    /// it is zero, or the pointer is null, the dash pattern will be cleared
    /// and strokes will be drawn as solid lines.  The array is copied and
    /// it is safe to change or destroy it after this call.  Defaults to
    /// solid lines.
    ///
    /// @param segments  pointer to array for dash pattern
    /// @param count     number of entries in the array
    ///
    void set_line_dash(
        float const *segments,
        int count );

    // ======== FILL AND STROKE STYLES ========

    /// @brief  Set filling or stroking to use a constant color and opacity.
    ///
    /// The color and opacity values will be clamped to the 0.0 to 1.0 range,
    /// inclusive.  Filling and stroking defaults a constant color with 0.0,
    /// 0.0, 0.0, 1.0 (opaque black).
    ///
    /// @param type   whether to set the fill_style or stroke_style
    /// @param red    sRGB red component of the painting color
    /// @param green  sRGB green component of the painting color
    /// @param blue   sRGB blue component of the painting color
    /// @param alpha  opacity to paint with (not premultiplied)
    ///
    void set_color(
        brush_type type,
        float red,
        float green,
        float blue,
        float alpha );

    void set_color(
        brush_type type,
        rgba c)
    {
        set_color(type, c.r, c.g, c.b, c.a);
    }

    /// @brief  Set filling or stroking to use a linear gradient.
    ///
    /// Positions the start and end points of the gradient and clears all
    /// color stops to reset the gradient to transparent black.  Color stops
    /// can then be added again.  When drawing, pixels will be painted with
    /// the color of the gradient at the nearest point on the line segment
    /// between the start and end points.  This is affected by the current
    /// transform at the time of drawing.
    ///
    /// @param type     whether to set the fill_style or stroke_style
    /// @param start_x  horizontal coordinate of the start of the gradient
    /// @param start_y  vertical coordinate of the start of the gradient
    /// @param end_x    horizontal coordinate of the end of the gradient
    /// @param end_y    vertical coordinate of the end of the gradient
    ///
    void set_linear_gradient(
        brush_type type,
        float start_x,
        float start_y,
        float end_x,
        float end_y );

    /// @brief  Set filling or stroking to use a radial gradient.
    ///
    /// Positions the start and end circles of the gradient and clears all
    /// color stops to reset the gradient to transparent black.  Color stops
    /// can then be added again.  When drawing, pixels will be painted as
    /// though the starting circle moved and changed size linearly to match
    /// the ending circle, while sweeping through the colors of the gradient.
    /// Pixels not touched by the moving circle will be left transparent
    /// black.  The radial gradient is affected by the current transform
    /// at the time of drawing.  The radii must be non-negative.
    ///
    /// @param type          whether to set the fill_style or stroke_style
    /// @param start_x       horizontal starting coordinate of the circle
    /// @param start_y       vertical starting coordinate of the circle
    /// @param start_radius  starting radius of the circle
    /// @param end_x         horizontal ending coordinate of the circle
    /// @param end_y         vertical ending coordinate of the circle
    /// @param end_radius    ending radius of the circle
    ///
    void set_radial_gradient(
        brush_type type,
        float start_x,
        float start_y,
        float start_radius,
        float end_x,
        float end_y,
        float end_radius );

    void set_css_radial_gradient(
        brush_type type,
        float x,
        float y,
        float radius_x,
        float radius_y);

    void set_conic_gradient(
        brush_type type,
        float x,
        float y,
        float angle);

    /// @brief  Add a color stop to a linear or radial gradient.
    ///
    /// Each color stop has an offset which defines its position from 0.0 at
    /// the start of the gradient to 1.0 at the end.  Colors and opacity are
    /// linearly interpolated along the gradient between adjacent pairs of
    /// stops without premultiplying the alpha.  If more than one stop is
    /// added for a given offset, the first one added is considered closest
    /// to 0.0 and the last is closest to 1.0.  If no stop is at offset 0.0
    /// or 1.0, the stops with the closest offsets will be extended.  If no
    /// stops are added, the gradient will be fully transparent black.  Set a
    /// new linear or radial gradient to clear all the stops and redefine the
    /// gradient colors.  Color stops may be added to a gradient at any time.
    /// The color and opacity values will be clamped to the 0.0 to 1.0 range,
    /// inclusive.  The offset must be in the 0.0 to 1.0 range, inclusive.
    /// If it is not, or if chosen style type is not currently set to a
    /// gradient, this does nothing.
    ///
    /// @param type    whether to add to the fill_style or stroke_style
    /// @param offset  position of the color stop along the gradient
    /// @param red     sRGB red component of the color stop
    /// @param green   sRGB green component of the color stop
    /// @param blue    sRGB blue component of the color stop
    /// @param alpha   opacity of the color stop (not premultiplied)
    ///
    void add_color_stop(
        brush_type type,
        float offset,
        float red,
        float green,
        float blue,
        float alpha,
        std::optional<float> hint = {} );

    /// @brief  Set filling or stroking to draw with an image pattern.
    ///
    /// Initially, pixels in the pattern correspond exactly to pixels on the
    /// canvas, with the pattern starting in the upper left.  The pattern
    /// is affected by the current transform at the time of drawing, and
    /// the pattern will be resampled as needed (with the filtering always
    /// wrapping regardless of the repetition setting).  The pattern can be
    /// repeated either horizontally, vertically, both, or neither, relative
    /// to the source image.  If the pattern is not repeated, then beyond it
    /// will be considered transparent black.  The pattern image, which should
    /// be in top to bottom rows of contiguous pixels from left to right,
    /// is copied and it is safe to change or destroy it after this call.
    /// The width and height must both be positive.  If either are not, or
    /// the image pointer is null, this does nothing.
    ///
    /// Tip: to use a small piece of a larger image, reduce the width and
    ///      height, and offset the image pointer while keeping the stride.
    ///
    /// @param type        whether to set the fill_style or stroke_style
    /// @param image       pointer to unpremultiplied sRGB RGBA8 image data
    /// @param width       width of the pattern image in pixels
    /// @param height      height of the pattern image in pixels
    /// @param stride      number of bytes between the start of each image row
    /// @param repetition  repeat, repeat_x, repeat_y, or no_repeat
    ///
    void set_pattern(
        brush_type type,
        unsigned char const *image,
        int width,
        int height,
        int stride,
        repetition_style repetition );

    // ======== BUILDING PATHS ========

    /// @brief  Reset the current path.
    ///
    /// The current path and all subpaths will be cleared after this, and a
    /// new path can be built.
    ///
    void begin_path();

    /// @brief  Create a new subpath.
    ///
    /// The given point will become the first point of the new subpath and
    /// is subject to the current transform at the time this is called.
    ///
    /// @param x  horizontal coordinate of the new first point
    /// @param y  vertical coordinate of the new first point
    ///
    void move_to(
        float x,
        float y );

    /// @brief  Close the current subpath.
    ///
    /// Adds a straight line from the end of the current subpath back to its
    /// first point and marks the subpath as closed so that this line will
    /// join with the beginning of the path at this point.  A new, empty
    /// subpath will be started beginning with the same first point.  If the
    /// current path is empty, this does nothing.
    ///
    void close_path();

    /// @brief  Extend the current subpath with a straight line.
    ///
    /// The line will go from the current end point (if the current path is
    /// not empty) to the given point, which will become the new end point.
    /// Its position is affected by the current transform at the time that
    /// this is called.  If the current path was empty, this is equivalent
    /// to just a move.
    ///
    /// @param x  horizontal coordinate of the new end point
    /// @param y  vertical coordinate of the new end point
    ///
    void line_to(
        float x,
        float y );

    /// @brief  Extend the current subpath with a quadratic Bezier curve.
    ///
    /// The curve will go from the current end point (or the control point
    /// if the current path is empty) to the given point, which will become
    /// the new end point.  The curve will be tangent toward the control
    /// point at both ends.  The current transform at the time that this
    /// is called will affect both points passed in.
    ///
    /// Tip: to make multiple curves join smoothly, ensure that each new end
    ///      point is on a line between the adjacent control points.
    ///
    /// @param control_x  horizontal coordinate of the control point
    /// @param control_y  vertical coordinate of the control point
    /// @param x          horizontal coordinate of the new end point
    /// @param y          vertical coordinate of the new end point
    ///
    void quadratic_curve_to(
        float control_x,
        float control_y,
        float x,
        float y );

    /// @brief  Extend the current subpath with a cubic Bezier curve.
    ///
    /// The curve will go from the current end point (or the first control
    /// point if the current path is empty) to the given point, which will
    /// become the new end point.  The curve will be tangent toward the first
    /// control point at the beginning and tangent toward the second control
    /// point at the end.  The current transform at the time that this is
    /// called will affect all points passed in.
    ///
    /// Tip: to make multiple curves join smoothly, ensure that each new end
    ///      point is on a line between the adjacent control points.
    ///
    /// @param control_1_x  horizontal coordinate of the first control point
    /// @param control_1_y  vertical coordinate of the first control point
    /// @param control_2_x  horizontal coordinate of the second control point
    /// @param control_2_y  vertical coordinate of the second control point
    /// @param x            horizontal coordinate of the new end point
    /// @param y            vertical coordinate of the new end point
    ///
    void bezier_curve_to(
        float control_1_x,
        float control_1_y,
        float control_2_x,
        float control_2_y,
        float x,
        float y );

    /// @brief  Extend the current subpath with an arc tangent to two lines.
    ///
    /// The arc is from the circle with the given radius tangent to both
    /// the line from the current end point to the vertex, and to the line
    /// from the vertex to the given point.  A straight line will be added
    /// from the current end point to the first tangent point (unless the
    /// current path is empty), then the shortest arc from the first to the
    /// second tangent points will be added.  The second tangent point will
    /// become the new end point.  If the radius is large, these tangent
    /// points may fall outside the line segments.  The current transform
    /// at the time that this is called will affect the passed in points
    /// and the arc.  If the current path was empty, this is equivalent to
    /// a move.  If the arc would be degenerate, it is equivalent to a line
    /// to the vertex point.  The radius must be non-negative.  If it is not,
    /// or if the current transform is not invertible, this does nothing.
    ///
    /// Tip: to draw a polygon with rounded corners, call this once for each
    ///      vertex and pass the midpoint of the adjacent edge as the second
    ///      point; this works especially well for rounded boxes.
    ///
    /// @param vertex_x  horizontal coordinate where the tangent lines meet
    /// @param vertex_y  vertical coordinate where the tangent lines meet
    /// @param x         a horizontal coordinate on the second tangent line
    /// @param y         a vertical coordinate on the second tangent line
    /// @param radius    radius of the circle containing the arc
    ///
    void arc_to(
        float vertex_x,
        float vertex_y,
        float x,
        float y,
        float radius );

    /// @brief  Extend the current subpath with an arc between two angles.
    ///
    /// The arc is from the circle centered at the given point and with the
    /// given radius.  A straight line will be added from the current end
    /// point to the starting point of the arc (unless the current path is
    /// empty), then the arc along the circle from the starting angle to the
    /// ending angle in the given direction will be added.  If they are more
    /// than two pi radians apart in the given direction, the arc will stop
    /// after one full circle.  The point at the ending angle will become
    /// the new end point of the path.  Initially, the angles are clockwise
    /// relative to the x-axis.  The current transform at the time that
    /// this is called will affect the passed in point, angles, and arc.
    /// The radius must be non-negative.
    ///
    /// @param x                  horizontal coordinate of the circle center
    /// @param y                  vertical coordinate of the circle center
    /// @param radius             radius of the circle containing the arc
    /// @param start_angle        radians clockwise from x-axis to begin
    /// @param end_angle          radians clockwise from x-axis to end
    /// @param counter_clockwise  true if the arc turns counter-clockwise
    ///
    void arc(
        float x,
        float y,
        float radius,
        float start_angle,
        float end_angle,
        bool counter_clockwise = false );

    /// @brief  Add a closed subpath in the shape of a rectangle.
    ///
    /// The rectangle has one corner at the given point and then goes in the
    /// direction along the width before going in the direction of the height
    /// towards the opposite corner.  The current transform at the time that
    /// this is called will affect the given point and rectangle.  The width
    /// and/or the height may be negative or zero, and this can affect the
    /// winding direction.
    ///
    /// @param x       horizontal coordinate of a rectangle corner
    /// @param y       vertical coordinate of a rectangle corner
    /// @param width   width of the rectangle
    /// @param height  height of the rectangle
    ///
    void rectangle(
        float x,
        float y,
        float width,
        float height );

    void polygon(std::vector<xy> points);

    // ======== DRAWING PATHS ========

    /// @brief  Draw the interior of the current path using the fill style.
    ///
    /// Interior pixels are determined by the non-zero winding rule, with
    /// all open subpaths implicitly closed by a straight line beforehand.
    /// If shadows have been enabled by setting the shadow color with any
    /// opacity and either offsetting or blurring the shadows, then the
    /// shadows of the filled areas will be drawn first, followed by the
    /// filled areas themselves.  Both will be blended into the canvas
    /// according to the global alpha, the global compositing operation,
    /// and the clip region.  If the fill style is a gradient or a pattern,
    /// it will be affected by the current transform.  The current path is
    /// left unchanged by filling; begin a new path to clear it.  If the
    /// current transform is not invertible, this does nothing.
    ///
    void fill();

    /// @brief  Draw the edges of the current path using the stroke style.
    ///
    /// Edges of the path will be expanded into strokes according to the
    /// current dash pattern, dash offset, line width, line join style
    /// (and possibly miter limit), line cap, and transform.  If shadows
    /// have been enabled by setting the shadow color with any opacity and
    /// either offsetting or blurring the shadows, then the shadow will be
    /// drawn for the stroked lines first, then the stroked lines themselves.
    /// Both will be blended into the canvas according to the global alpha,
    /// the global compositing operation, and the clip region.  If the stroke
    /// style is a gradient or a pattern, it will be affected by the current
    /// transform.  The current path is left unchanged by stroking; begin a
    /// new path to clear it.  If the current transform is not invertible,
    /// this does nothing.
    ///
    /// Tip: to draw with a calligraphy-like angled brush effect, add a
    ///      non-uniform scale transform just before stroking.
    ///
    void stroke();

    /// @brief  Restrict the clip region by the current path.
    ///
    /// Intersects the current clip region with the interior of the current
    /// path (the region that would be filled), and replaces the current
    /// clip region with this intersection.  Subsequent calls to clip can
    /// only reduce this further.  When filling or stroking, only pixels
    /// within the current clip region will change.  The current path is
    /// left unchanged by updating the clip region; begin a new path to
    /// clear it.  Defaults to the entire canvas.
    ///
    /// Tip: to be able to reset the current clip region, save the canvas
    ///      state first before clipping then restore the state to reset it.
    ///
    void clip();

    /// @brief  Tests whether a point is in or on the current path.
    ///
    /// Interior areas are determined by the non-zero winding rule, with
    /// all open subpaths treated as implicitly closed by a straight line
    /// beforehand.  Points exactly on the boundary are also considered
    /// inside.  The point to test is interpreted without being affected by
    /// the current transform, nor is the clip region considered.  The current
    /// path is left unchanged by this test.
    ///
    /// @param x  horizontal coordinate of the point to test
    /// @param y  vertical coordinate of the point to test
    /// @return   true if the point is in or on the current path
    ///
    bool is_point_in_path(
        float x,
        float y );

    // ======== DRAWING RECTANGLES ========

    /// @brief  Clear a rectangular area back to transparent black.
    ///
    /// The clip region may limit the area cleared.  The current path is not
    /// affected by this clearing.  The current transform at the time that
    /// this is called will affect the given point and rectangle.  The width
    /// and/or the height may be negative or zero.  If either is zero, or the
    /// current transform is not invertible, this does nothing.
    ///
    /// @param x       horizontal coordinate of a rectangle corner
    /// @param y       vertical coordinate of a rectangle corner
    /// @param width   width of the rectangle
    /// @param height  height of the rectangle
    ///
    void clear_rectangle(
        float x,
        float y,
        float width,
        float height );

    /// @brief  Fill a rectangular area.
    ///
    /// This behaves as though the current path were reset to a single
    /// rectangle and then filled as usual.  However, the current path is
    /// not actually changed.  The current transform at the time that this is
    /// called will affect the given point and rectangle.  The width and/or
    /// the height may be negative but not zero.  If either is zero, or the
    /// current transform is not invertible, this does nothing.
    ///
    /// @param x       horizontal coordinate of a rectangle corner
    /// @param y       vertical coordinate of a rectangle corner
    /// @param width   width of the rectangle
    /// @param height  height of the rectangle
    ///
    void fill_rectangle(
        float x,
        float y,
        float width,
        float height );

    /// @brief  Stroke a rectangular area.
    ///
    /// This behaves as though the current path were reset to a single
    /// rectangle and then stroked as usual.  However, the current path is
    /// not actually changed.  The current transform at the time that this
    /// is called will affect the given point and rectangle.  The width
    /// and/or the height may be negative or zero.  If both are zero, or
    /// the current transform is not invertible, this does nothing.  If only
    /// one is zero, this behaves as though it strokes a single horizontal or
    /// vertical line.
    ///
    /// @param x       horizontal coordinate of a rectangle corner
    /// @param y       vertical coordinate of a rectangle corner
    /// @param width   width of the rectangle
    /// @param height  height of the rectangle
    ///
    void stroke_rectangle(
        float x,
        float y,
        float width,
        float height );

    // ======== DRAWING TEXT ========

    /// @brief  Horizontal position of the text relative to the anchor point.
    ///
    /// When drawing text, the positioning of the text relative to the anchor
    /// point includes the side bearings of the first and last glyphs.
    /// Defaults to leftward.
    ///
    /// leftward:   Draw the text's left edge at the anchor point.
    /// rightward:  Draw the text's right edge at the anchor point.
    /// center:     Draw the text's horizontal center at the anchor point.
    /// start:      This is a synonym for leftward.
    /// ending:     This is a synonym for rightward.
    ///
    align_style text_align;

    /// @brief  Vertical position of the text relative to the anchor point.
    ///
    /// Defaults to alphabetic.
    ///
    /// alphabetic:   Draw with the alphabetic baseline at the anchor point.
    /// top:          Draw the top of the em box at the anchor point.
    /// middle:       Draw the exact middle of the em box at the anchor point.
    /// bottom:       Draw the bottom of the em box at the anchor point.
    /// hanging:      Draw 60% of an em over the baseline at the anchor point.
    /// ideographic:  This is a synonym for bottom.
    ///
    baseline_style text_baseline;

    /// @brief  Set the font to use for text drawing.
    ///
    /// The font must be a TrueType font (TTF) file which has been loaded or
    /// mapped into memory.  Following some basic validation, the relevant
    /// sections of the font file contents are copied, and it is safe to
    /// change or destroy after this call.  As an optimization, calling this
    /// with either a null pointer or zero for the number of bytes will allow
    /// for changing the size of the previous font without recopying from
    /// the file.  Note that the font parsing is not meant to be secure;
    /// only use this with trusted TTF files!
    ///
    /// @param font   pointer to the contents of a TrueType font (TTF) file
    /// @param bytes  number of bytes in the font file
    /// @param size   size in pixels per em to draw at
    /// @return       true if the font was set successfully
    ///
    bool set_font(
        unsigned char const *font,
        int bytes,
        float size );

    void get_font_metrics(int& ascent, int& descent, int& height, int& x_height);

    /// @brief  Draw a line of text by filling its outline.
    ///
    /// This behaves as though the current path were reset to the outline
    /// of the given text in the current font and size, positioned relative
    /// to the given anchor point according to the current alignment and
    /// baseline, and then filled as usual.  However, the current path is
    /// not actually changed.  The current transform at the time that this
    /// is called will affect the given anchor point and the text outline.
    /// However, the comparison to the maximum width in pixels and the
    /// condensing if needed is done before applying the current transform.
    /// The maximum width (if given) must be positive.  If it is not, or
    /// the text pointer is null, or the font has not been set yet, or the
    /// last setting of it was unsuccessful, or the current transform is not
    /// invertible, this does nothing.
    ///
    /// @param text           null-terminated UTF-8 string of text to fill
    /// @param x              horizontal coordinate of the anchor point
    /// @param y              vertical coordinate of the anchor point
    /// @param maximum_width  horizontal width to condense wider text to
    ///
    void fill_text(
        char const *text,
        float x,
        float y,
        float maximum_width = 1.0e30f );

    /// @brief  Draw a line of text by stroking its outline.
    ///
    /// This behaves as though the current path were reset to the outline
    /// of the given text in the current font and size, positioned relative
    /// to the given anchor point according to the current alignment and
    /// baseline, and then stroked as usual.  However, the current path is
    /// not actually changed.  The current transform at the time that this
    /// is called will affect the given anchor point and the text outline.
    /// However, the comparison to the maximum width in pixels and the
    /// condensing if needed is done before applying the current transform.
    /// The maximum width (if given) must be positive.  If it is not, or
    /// the text pointer is null, or the font has not been set yet, or the
    /// last setting of it was unsuccessful, or the current transform is not
    /// invertible, this does nothing.
    ///
    /// @param text           null-terminated UTF-8 string of text to stroke
    /// @param x              horizontal coordinate of the anchor point
    /// @param y              vertical coordinate of the anchor point
    /// @param maximum_width  horizontal width to condense wider text to
    ///
    void stroke_text(
        char const *text,
        float x,
        float y,
        float maximum_width = 1.0e30f );

    /// @brief  Measure the width in pixels of a line of text.
    ///
    /// The measured width is the advance width, which includes the side
    /// bearings of the first and last glyphs.  However, text as drawn may
    /// go outside this (e.g., due to glyphs that spill beyond their nominal
    /// widths or stroked text with wide lines).  Measurements ignore the
    /// current transform.  If the text pointer is null, or the font has
    /// not been set yet, or the last setting of it was unsuccessful, this
    /// returns zero.
    ///
    /// @param text  null-terminated UTF-8 string of text to measure
    /// @return      width of the text in pixels
    ///
    float measure_text(
        char const *text );

    // ======== DRAWING IMAGES ========

    /// @brief  Draw an image onto the canvas.
    ///
    /// The position of the rectangle that the image is drawn to is affected
    /// by the current transform at the time of drawing, and the image will
    /// be resampled as needed (with the filtering always clamping to the
    /// edges of the image).  The drawing is also affected by the shadow,
    /// global alpha, global compositing operation settings, and by the
    /// clip region.  The current path is not affected by drawing an image.
    /// The image data, which should be in top to bottom rows of contiguous
    /// pixels from left to right, is not retained and it is safe to change
    /// or destroy it after this call.  The width and height must both be
    /// positive and the width and/or the height to scale to may be negative
    /// but not zero.  Otherwise, or if the image pointer is null, or the
    /// current transform is not invertible, this does nothing.
    ///
    /// Tip: to use a small piece of a larger image, reduce the width and
    ///      height, and offset the image pointer while keeping the stride.
    ///
    /// @param image      pointer to unpremultiplied sRGB RGBA8 image data
    /// @param width      width of the image in pixels
    /// @param height     height of the image in pixels
    /// @param stride     number of bytes between the start of each image row
    /// @param x          horizontal coordinate to draw the corner at
    /// @param y          vertical coordinate to draw the corner at
    /// @param to_width   width to scale the image to
    /// @param to_height  height to scale the image to
    ///
    void draw_image(
        unsigned char const *image,
        int width,
        int height,
        int stride,
        float x,
        float y,
        float to_width,
        float to_height );

    // ======== PIXEL MANIPULATION ========

    /// @brief  Fetch a rectangle of pixels from the canvas to an image.
    ///
    /// This call is akin to a direct pixel-for-pixel copy from the canvas
    /// buffer without resampling.  The position and size of the canvas
    /// rectangle is not affected by the current transform.  The image data
    /// is copied into, from top to bottom in rows of contiguous pixels from
    /// left to right, and it is safe to change or destroy it after this call.
    /// The requested rectangle may safely extend outside the bounds of the
    /// canvas; these pixels will be set to transparent black.  The width
    /// and height must be positive.  If not, or if the image pointer is
    /// null, this does nothing.
    ///
    /// Tip: to copy into a section of a larger image, reduce the width and
    ///      height, and offset the image pointer while keeping the stride.
    /// Tip: use this to get the rendered image from the canvas after drawing.
    ///
    /// @param image   pointer to unpremultiplied sRGB RGBA8 image data
    /// @param width   width of the image in pixels
    /// @param height  height of the image in pixels
    /// @param stride  number of bytes between the start of each image row
    /// @param x       horizontal coordinate of upper-left pixel to fetch
    /// @param y       vertical coordinate of upper-left pixel to fetch
    ///
    void get_image_data(
        unsigned char *image,
        int width,
        int height,
        int stride,
        int x,
        int y );

    /// @brief  Replace a rectangle of pixels on the canvas with an image.
    ///
    /// This call is akin to a direct pixel-for-pixel copy into the canvas
    /// buffer without resampling.  The position and size of the canvas
    /// rectangle is not affected by the current transform.  Nor is the
    /// result affected by the current settings for the global alpha, global
    /// compositing operation, shadows, or the clip region.  The image data,
    /// which should be in top to bottom rows of contiguous pixels from left
    /// to right, is copied from and it is safe to change or destroy it after
    /// this call.  The width and height must be positive.  If not, or if the
    /// image pointer is null, this does nothing.
    ///
    /// Tip: to copy from a section of a larger image, reduce the width and
    ///      height, and offset the image pointer while keeping the stride.
    /// Tip: use this to prepopulate the canvas before drawing.
    ///
    /// @param image   pointer to unpremultiplied sRGB RGBA8 image data
    /// @param width   width of the image in pixels
    /// @param height  height of the image in pixels
    /// @param stride  number of bytes between the start of each image row
    /// @param x       horizontal coordinate of upper-left pixel to replace
    /// @param y       vertical coordinate of upper-left pixel to replace
    ///
    void put_image_data(
        unsigned char const *image,
        int width,
        int height,
        int stride,
        int x,
        int y );

    int width() { return size_x; }
    int height() { return size_y; }

    // ======== CANVAS STATE ========

    /// @brief  Save the current state as though to a stack.
    ///
    /// The full state of the canvas is saved, except for the pixels in the
    /// canvas buffer, and the current path.
    ///
    /// Tip: to be able to reset the current clip region, save the canvas
    ///      state first before clipping then restore the state to reset it.
    ///
    void save();

    /// @brief  Restore a previously saved state as though from a stack.
    ///
    /// This does not affect the pixels in the canvas buffer or the current
    /// path.  If the stack of canvas states is empty, this does nothing.
    ///
    void restore();

private:
    int size_x;
    int size_y;
    affine_matrix forward;
    affine_matrix inverse;
    float global_alpha;
    rgba shadow_color;
    float shadow_blur;
    std::vector< float > shadow;
    float line_width;
    float miter_limit;
    std::vector< float > line_dash;
    paint_brush fill_brush;
    paint_brush stroke_brush;
    paint_brush image_brush;
    bezier_path path;
    line_path lines;
    line_path scratch;
    pixel_runs runs;
    pixel_runs mask;
    font_face face;
    rgba *bitmap;
    canvas *saves;
    canvas( canvas const & );
    canvas &operator=( canvas const & );
    void add_tessellation( xy, xy, xy, xy, float, int );
    void add_bezier( xy, xy, xy, xy, float );
    void path_to_lines( bool );
    void add_glyph( int, float );
    int character_to_glyph( char const *, int & );
    void text_to_lines( char const *, xy, float, bool );
    void dash_lines();
    void add_half_stroke( size_t, size_t, bool );
    void stroke_lines();
    void add_runs( xy, xy );
    void lines_to_runs( xy, int );
    rgba paint_pixel( xy, paint_brush const & );
    void render_shadow( paint_brush const & );
    void render_main( paint_brush const & );
};

}

#endif // CANVAS_ITY_HPP

// ======== IMPLEMENTATION ========
//
// The general internal data flow (albeit not control flow!) for rendering
// a shape onto the canvas is as follows:
//
// 1. Construct a set of polybeziers representing the current path via the
//      public path building API (move_to, line_to, bezier_curve_to, etc.).
// 2. Adaptively tessellate the polybeziers into polylines (path_to_lines).
// 3. Maybe break the polylines into shorter polylines according to the dash
//      pattern (dash_lines).
// 4. Maybe stroke expand the polylines into new polylines that can be filled
//      to show the lines with width, joins, and caps (stroke_lines).
// 5. Scan-convert the polylines into a sparse representation of fractional
//      pixel coverage (lines_to_runs).
// 6. Maybe paint the covered pixel span alphas "offscreen", blur, color,
//      and blend them onto the canvas where not clipped (render_shadow).
// 7. Paint the covered pixel spans and blend them onto the canvas where not
//      clipped (render_main).

#ifdef CANVAS_ITY_IMPLEMENTATION
#define LINEARIZE_RGB 2

#include <algorithm>
#include <cmath>
#include <numeric>

namespace canvas_ity
{

// 2D vector math operations
const float pi = 3.14159265f;
xy::xy() : x( 0.0f ), y( 0.0f ) {}
xy::xy( float new_x, float new_y ) : x( new_x ), y( new_y ) {}
static xy &operator+=( xy &left, xy right ) {
    left.x += right.x; left.y += right.y; return left; }
static xy &operator-=( xy &left, xy right ) {
    left.x -= right.x; left.y -= right.y; return left; }
static xy &operator*=( xy &left, float right ) {
    left.x *= right; left.y *= right; return left; }
static xy const operator+( xy left, xy right ) {
    return left += right; }
static xy const operator-( xy left, xy right ) {
    return left -= right; }
static xy const operator*( float left, xy right ) {
    return right *= left; }
static xy const operator*( affine_matrix const &left, xy right ) {
    return xy( left.a * right.x + left.c * right.y + left.e,
               left.b * right.x + left.d * right.y + left.f ); }
static float dot( xy left, xy right ) {
    return left.x * right.x + left.y * right.y; }
static float length( xy that ) {
    return sqrtf( dot( that, that ) ); }
static float direction( xy that ) {
    return atan2f( that.y, that.x ); }
static xy const normalized( xy that ) {
    return 1.0f / std::max( 1.0e-6f, length( that ) ) * that; }
static xy const perpendicular( xy that ) {
    return xy( -that.y, that.x ); }
static xy const lerp( xy from, xy to, float ratio ) {
    return from + ratio * ( to - from ); }
// ensure 0 <= angle < 360
float normalize_angle(float angle) {
    return fmodf(angle, 360) + (angle < 0 ? 360 : 0);
}

// RGBA color operations
rgba::rgba() : r( 0.0f ), g( 0.0f ), b( 0.0f ), a( 0.0f ) {}
rgba::rgba( float new_r, float new_g, float new_b, float new_a )
    : r( new_r ), g( new_g ), b( new_b ), a( new_a ) {}
static rgba &operator+=( rgba &left, rgba right ) {
    left.r += right.r; left.g += right.g; left.b += right.b;
    left.a += right.a; return left; }
//static rgba &operator-=( rgba &left, rgba right ) {
//    left.r -= right.r; left.g -= right.g; left.b -= right.b;
//    left.a -= right.a; return left; }
static rgba &operator*=( rgba &left, float right ) {
    left.r *= right; left.g *= right; left.b *= right;
    left.a *= right; return left; }
static rgba const operator+( rgba left, rgba right ) {
    return left += right; }
//static rgba const operator-( rgba left, rgba right ) {
//    return left -= right; }
static rgba const operator*( float left, rgba right ) {
    return right *= left; }
#if (CANVAS_ITY_IMPLEMENTATION+0) & LINEARIZE_RGB
static float linearized( float value ) {
    return value < 0.04045f ? value / 12.92f :
        powf( ( value + 0.055f ) / 1.055f, 2.4f ); }
static float delinearized( float value ) {
    return value < 0.0031308f ? 12.92f * value :
        1.055f * powf( value, 1.0f / 2.4f ) - 0.055f; }
#else
static float linearized(float value) { return value; }
static float delinearized(float value) { return value; }
#endif
static rgba const linearized( rgba that ) {
    return rgba( linearized( that.r ), linearized( that.g ),
                 linearized( that.b ), that.a ); }
static rgba const premultiplied( rgba that ) {
    return rgba( that.r * that.a, that.g * that.a,
                 that.b * that.a, that.a ); }
static rgba const delinearized( rgba that ) {
    return rgba( delinearized( that.r ), delinearized( that.g ),
                 delinearized( that.b ), that.a ); }
static rgba const unpremultiplied( rgba that ) {
    static float const threshold = 1.0f / 8160.0f;
    return that.a < threshold ? rgba( 0.0f, 0.0f, 0.0f, 0.0f ) :
        rgba( 1.0f / that.a * that.r, 1.0f / that.a * that.g,
              1.0f / that.a * that.b, that.a ); }
static rgba const clamped( rgba that ) {
    return rgba( std::min( std::max( that.r, 0.0f ), 1.0f ),
                 std::min( std::max( that.g, 0.0f ), 1.0f ),
                 std::min( std::max( that.b, 0.0f ), 1.0f ),
                 std::min( std::max( that.a, 0.0f ), 1.0f ) ); }

// Helpers for TTF file parsing
static int unsigned_8( std::vector< unsigned char > &data, int index ) {
    return data[ static_cast< size_t >( index ) ]; }
static int signed_8( std::vector< unsigned char > &data, int index ) {
    size_t place = static_cast< size_t >( index );
    return static_cast< signed char >( data[ place ] ); }
static int unsigned_16( std::vector< unsigned char > &data, int index ) {
    size_t place = static_cast< size_t >( index );
    return data[ place ] << 8 | data[ place + 1 ]; }
static int signed_16( std::vector< unsigned char > &data, int index ) {
    size_t place = static_cast< size_t >( index );
    return static_cast< short >( data[ place ] << 8 | data[ place + 1 ] ); }
static int signed_32( std::vector< unsigned char > &data, int index ) {
    size_t place = static_cast< size_t >( index );
    return ( data[ place + 0 ] << 24 | data[ place + 1 ] << 16 |
             data[ place + 2 ] <<  8 | data[ place + 3 ] <<  0 ); }

// Tessellate (at low-level) a cubic Bezier curve and add it to the polyline
// data.  This recursively splits the curve until two criteria are met
// (subject to a hard recursion depth limit).  First, the control points
// must not be farther from the line between the endpoints than the tolerance.
// By the Bezier convex hull property, this ensures that the distance between
// the true curve and the polyline approximation will be no more than the
// tolerance.  Secondly, it takes the cosine of an angular turn limit; the
// curve will be split until it turns less than this amount.  This is used
// for stroking, with the angular limit chosen such that the sagita of an arc
// with that angle and a half-stroke radius will be equal to the tolerance.
// This keeps expanded strokes approximately within tolerance.  Note that
// in the base case, it adds the control points as well as the end points.
// This way, stroke expansion infers the correct tangents from the ends of
// the polylines.
//
void canvas::add_tessellation(
    xy point_1,
    xy control_1,
    xy control_2,
    xy point_2,
    float angular,
    int limit )
{
    static float const tolerance = 0.125f;
    float flatness = tolerance * tolerance;
    xy edge_1 = control_1 - point_1;
    xy edge_2 = control_2 - control_1;
    xy edge_3 = point_2 - control_2;
    xy segment = point_2 - point_1;
    float squared_1 = dot( edge_1, edge_1 );
    float squared_2 = dot( edge_2, edge_2 );
    float squared_3 = dot( edge_3, edge_3 );
    static float const epsilon = 1.0e-4f;
    float length_squared = std::max( epsilon, dot( segment, segment ) );
    float projection_1 = dot( edge_1, segment ) / length_squared;
    float projection_2 = dot( edge_3, segment ) / length_squared;
    float clamped_1 = std::min( std::max( projection_1, 0.0f ), 1.0f );
    float clamped_2 = std::min( std::max( projection_2, 0.0f ), 1.0f );
    xy to_line_1 = point_1 + clamped_1 * segment - control_1;
    xy to_line_2 = point_2 - clamped_2 * segment - control_2;
    float cosine = 1.0f;
    if ( angular > -1.0f )
    {
        if ( squared_1 * squared_3 != 0.0f )
            cosine = dot( edge_1, edge_3 ) / sqrtf( squared_1 * squared_3 );
        else if ( squared_1 * squared_2 != 0.0f )
            cosine = dot( edge_1, edge_2 ) / sqrtf( squared_1 * squared_2 );
        else if ( squared_2 * squared_3 != 0.0f )
            cosine = dot( edge_2, edge_3 ) / sqrtf( squared_2 * squared_3 );
    }
    if ( ( dot( to_line_1, to_line_1 ) <= flatness &&
           dot( to_line_2, to_line_2 ) <= flatness &&
           cosine >= angular ) ||
         !limit )
    {
        if ( angular > -1.0f && squared_1 != 0.0f )
            lines.points.push_back( control_1 );
        if ( angular > -1.0f && squared_2 != 0.0f )
            lines.points.push_back( control_2 );
        if ( angular == -1.0f || squared_3 != 0.0f )
            lines.points.push_back( point_2 );
        return;
    }
    xy left_1 = lerp( point_1, control_1, 0.5f );
    xy middle = lerp( control_1, control_2, 0.5f );
    xy right_2 = lerp( control_2, point_2, 0.5f );
    xy left_2 = lerp( left_1, middle, 0.5f );
    xy right_1 = lerp( middle, right_2, 0.5f );
    xy split = lerp( left_2, right_1, 0.5f );
    add_tessellation( point_1, left_1, left_2, split, angular, limit - 1 );
    add_tessellation( split, right_1, right_2, point_2, angular, limit - 1 );
}

// Tessellate (at high-level) a cubic Bezier curve and add it to the polyline
// data.  This solves both for the extreme in curvature and for the horizontal
// and vertical extrema.  It then splits the curve into segments at these
// points and passes them off to the lower-level recursive tessellation.
// This preconditioning means the polyline exactly includes any cusps or
// ends of tight loops without the flatness test needing to locate it via
// bisection, and the angular limit test can use simple dot products without
// fear of curves turning more than 90 degrees.
//
void canvas::add_bezier(
    xy point_1,
    xy control_1,
    xy control_2,
    xy point_2,
    float angular )
{
    xy edge_1 = control_1 - point_1;
    xy edge_2 = control_2 - control_1;
    xy edge_3 = point_2 - control_2;
    if ( dot( edge_1, edge_1 ) == 0.0f &&
         dot( edge_3, edge_3 ) == 0.0f )
    {
        lines.points.push_back( point_2 );
        return;
    }
    float at[ 7 ] = { 0.0f, 1.0f };
    int cuts = 2;
    xy extrema_a = -9.0f * edge_2 + 3.0f * ( point_2 - point_1 );
    xy extrema_b = 6.0f * ( point_1 + control_2 ) - 12.0f * control_1;
    xy extrema_c = 3.0f * edge_1;
    static float const epsilon = 1.0e-4f;
    if ( fabsf( extrema_a.x ) > epsilon )
    {
        float discriminant =
            extrema_b.x * extrema_b.x - 4.0f * extrema_a.x * extrema_c.x;
        if ( discriminant >= 0.0f )
        {
            float sign = extrema_b.x > 0.0f ? 1.0f : -1.0f;
            float term = -extrema_b.x - sign * sqrtf( discriminant );
            float extremum_1 = term / ( 2.0f * extrema_a.x );
            at[ cuts++ ] = extremum_1;
            at[ cuts++ ] = extrema_c.x / ( extrema_a.x * extremum_1 );
        }
    }
    else if ( fabsf( extrema_b.x ) > epsilon )
        at[ cuts++ ] = -extrema_c.x / extrema_b.x;
    if ( fabsf( extrema_a.y ) > epsilon )
    {
        float discriminant =
            extrema_b.y * extrema_b.y - 4.0f * extrema_a.y * extrema_c.y;
        if ( discriminant >= 0.0f )
        {
            float sign = extrema_b.y > 0.0f ? 1.0f : -1.0f;
            float term = -extrema_b.y - sign * sqrtf( discriminant );
            float extremum_1 = term / ( 2.0f * extrema_a.y );
            at[ cuts++ ] = extremum_1;
            at[ cuts++ ] = extrema_c.y / ( extrema_a.y * extremum_1 );
        }
    }
    else if ( fabsf( extrema_b.y ) > epsilon )
        at[ cuts++ ] = -extrema_c.y / extrema_b.y;
    float determinant_1 = dot( perpendicular( edge_1 ), edge_2 );
    float determinant_2 = dot( perpendicular( edge_1 ), edge_3 );
    float determinant_3 = dot( perpendicular( edge_2 ), edge_3 );
    float curve_a = determinant_1 - determinant_2 + determinant_3;
    float curve_b = -2.0f * determinant_1 + determinant_2;
    if ( fabsf( curve_a ) > epsilon &&
         fabsf( curve_b ) > epsilon )
        at[ cuts++ ] = -0.5f * curve_b / curve_a;
    for ( int index = 1; index < cuts; ++index )
    {
        float value = at[ index ];
        int sorted = index - 1;
        for ( ; 0 <= sorted && value < at[ sorted ]; --sorted )
            at[ sorted + 1 ] = at[ sorted ];
        at[ sorted + 1 ] = value;
    }
    xy split_point_1 = point_1;
    for ( int index = 0; index < cuts - 1; ++index )
    {
        if ( !( 0.0f <= at[ index ] && at[ index + 1 ] <= 1.0f &&
                at[ index ] != at[ index + 1 ] ) )
            continue;
        float ratio = at[ index ] / at[ index + 1 ];
        xy partial_1 = lerp( point_1, control_1, at[ index + 1 ] );
        xy partial_2 = lerp( control_1, control_2, at[ index + 1 ] );
        xy partial_3 = lerp( control_2, point_2, at[ index + 1 ] );
        xy partial_4 = lerp( partial_1, partial_2, at[ index + 1 ] );
        xy partial_5 = lerp( partial_2, partial_3, at[ index + 1 ] );
        xy partial_6 = lerp( partial_1, partial_4, ratio );
        xy split_point_2 = lerp( partial_4, partial_5, at[ index + 1 ] );
        xy split_control_2 = lerp( partial_4, split_point_2, ratio );
        xy split_control_1 = lerp( partial_6, split_control_2, ratio );
        add_tessellation( split_point_1, split_control_1,
                          split_control_2, split_point_2,
                          angular, 20 );
        split_point_1 = split_point_2;
    }
}

// Convert the current path to a set of polylines.  This walks over the
// complete set of subpaths in the current path (stored as sets of cubic
// Beziers) and converts each Bezier curve segment to a polyline while
// preserving information about where subpaths begin and end and whether
// they are closed or open.  This replaces the previous polyline data.
//
void canvas::path_to_lines(
    bool stroking )
{
    static float const tolerance = 0.125f;
    float ratio = tolerance / std::max( 0.5f * line_width, tolerance );
    float angular = stroking ? ( ratio - 2.0f ) * ratio * 2.0f + 1.0f : -1.0f;
    lines.points.clear();
    lines.subpaths.clear();
    size_t index = 0;
    size_t ending = 0;
    for ( size_t subpath = 0; subpath < path.subpaths.size(); ++subpath )
    {
        ending += path.subpaths[ subpath ].count;
        size_t first = lines.points.size();
        xy point_1 = path.points[ index++ ];
        lines.points.push_back( point_1 );
        for ( ; index < ending; index += 3 )
        {
            xy control_1 = path.points[ index + 0 ];
            xy control_2 = path.points[ index + 1 ];
            xy point_2 = path.points[ index + 2 ];
            add_bezier( point_1, control_1, control_2, point_2, angular );
            point_1 = point_2;
        }
        subpath_data entry = {
            lines.points.size() - first,
            path.subpaths[ subpath ].closed };
        lines.subpaths.push_back( entry );
    }
}

// Add a text glyph directly to the polylines.  Given a glyph index, this
// parses the data for that glyph directly from the TTF glyph data table and
// immediately tessellates it to a set of a polyline subpaths which it adds
// to any subpaths already present.  It uses the current transform matrix to
// transform from the glyph's vertices in font units to the proper size and
// position on the canvas.
//
void canvas::add_glyph(
    int glyph,
    float angular )
{
    int loc_format = unsigned_16( face.data, face.head + 50 );
    int offset = face.glyf + ( loc_format ?
        signed_32( face.data, face.loca + glyph * 4 ) :
        unsigned_16( face.data, face.loca + glyph * 2 ) * 2 );
    int next = face.glyf + ( loc_format ?
        signed_32( face.data, face.loca + glyph * 4 + 4 ) :
        unsigned_16( face.data, face.loca + glyph * 2 + 2 ) * 2 );
    if ( offset == next )
        return;
    int contours = signed_16( face.data, offset );
    if ( contours < 0 )
    {
        offset += 10;
        for ( ; ; )
        {
            int flags = unsigned_16( face.data, offset );
            int component = unsigned_16( face.data, offset + 2 );
            if ( !( flags & 2 ) )
                return; // Matching points are not supported
            float e = static_cast< float >( flags & 1 ?
                signed_16( face.data, offset + 4 ) :
                signed_8( face.data, offset + 4 ) );
            float f = static_cast< float >( flags & 1 ?
                signed_16( face.data, offset + 6 ) :
                signed_8( face.data, offset + 5 ) );
            offset += flags & 1 ? 8 : 6;
            float a = flags & 200 ? static_cast< float >(
                signed_16( face.data, offset ) ) / 16384.0f : 1.0f;
            float b = flags & 128 ? static_cast< float >(
                signed_16( face.data, offset + 2 ) ) / 16384.0f : 0.0f;
            float c = flags & 128 ? static_cast< float >(
                signed_16( face.data, offset + 4 ) ) / 16384.0f : 0.0f;
            float d = flags & 8 ? a :
                flags & 64 ? static_cast< float >(
                    signed_16( face.data, offset + 2 ) ) / 16384.0f :
                flags & 128 ? static_cast< float >(
                    signed_16( face.data, offset + 6 ) ) / 16384.0f :
                1.0f;
            offset += flags & 8 ? 2 : flags & 64 ? 4 : flags & 128 ? 8 : 0;
            affine_matrix saved_forward = forward;
            affine_matrix saved_inverse = inverse;
            transform( a, b, c, d, e, f );
            add_glyph( component, angular );
            forward = saved_forward;
            inverse = saved_inverse;
            if ( !( flags & 32 ) )
                return;
        }
    }
    int hmetrics = unsigned_16( face.data, face.hhea + 34 );
    int left_side_bearing = glyph < hmetrics ?
        signed_16( face.data, face.hmtx + glyph * 4 + 2 ) :
        signed_16( face.data, face.hmtx + hmetrics * 2 + glyph * 2 );
    int x_min = signed_16( face.data, offset + 2 );
    int points = unsigned_16( face.data, offset + 8 + contours * 2 ) + 1;
    int instructions = unsigned_16( face.data, offset + 10 + contours * 2 );
    int flags_array = offset + 12 + contours * 2 + instructions;
    int flags_size = 0;
    int x_size = 0;
    for ( int index = 0; index < points; )
    {
        int flags = unsigned_8( face.data, flags_array + flags_size++ );
        int repeated = flags & 8 ?
            unsigned_8( face.data, flags_array + flags_size++ ) + 1 : 1;
        x_size += repeated * ( flags & 2 ? 1 : flags & 16 ? 0 : 2 );
        index += repeated;
    }
    int x_array = flags_array + flags_size;
    int y_array = x_array + x_size;
    int x = left_side_bearing - x_min;
    int y = 0;
    int flags = 0;
    int repeated = 0;
    int index = 0;
    for ( int contour = 0; contour < contours; ++contour )
    {
        int beginning = index;
        int ending = unsigned_16( face.data, offset + 10 + contour * 2 );
        xy begin_point = xy( 0.0f, 0.0f );
        bool begin_on = false;
        xy end_point = xy( 0.0f, 0.0f );
        bool end_on = false;
        size_t first = lines.points.size();
        for ( ; index <= ending; ++index )
        {
            if ( repeated )
                --repeated;
            else
            {
                flags = unsigned_8( face.data, flags_array++ );
                if ( flags & 8 )
                    repeated = unsigned_8( face.data, flags_array++ );
            }
            if ( flags & 2 )
                x += ( unsigned_8( face.data, x_array ) *
                       ( flags & 16 ? 1 : -1 ) );
            else if ( !( flags & 16 ) )
                x += signed_16( face.data, x_array );
            if ( flags & 4 )
                y += ( unsigned_8( face.data, y_array ) *
                       ( flags & 32 ? 1 : -1 ) );
            else if ( !( flags & 32 ) )
                y += signed_16( face.data, y_array );
            x_array += flags & 2 ? 1 : flags & 16 ? 0 : 2;
            y_array += flags & 4 ? 1 : flags & 32 ? 0 : 2;
            xy point = forward * xy( static_cast< float >( x ),
                                     static_cast< float >( y ) );
            int on_curve = flags & 1;
            if ( index == beginning )
            {
                begin_point = point;
                begin_on = on_curve;
                if ( on_curve )
                    lines.points.push_back( point );
            }
            else
            {
                xy point_2 = on_curve ? point :
                    lerp( end_point, point, 0.5f );
                if ( lines.points.size() == first ||
                     ( end_on && on_curve ) )
                    lines.points.push_back( point_2 );
                else if ( !end_on || on_curve )
                {
                    xy point_1 = lines.points.back();
                    xy control_1 = lerp( point_1, end_point, 2.0f / 3.0f );
                    xy control_2 = lerp( point_2, end_point, 2.0f / 3.0f );
                    add_bezier( point_1, control_1, control_2, point_2,
                                angular );
                }
            }
            end_point = point;
            end_on = on_curve;
        }
        if ( begin_on ^ end_on )
        {
            xy point_1 = lines.points.back();
            xy point_2 = lines.points[ first ];
            xy control = end_on ? begin_point : end_point;
            xy control_1 = lerp( point_1, control, 2.0f / 3.0f );
            xy control_2 = lerp( point_2, control, 2.0f / 3.0f );
            add_bezier( point_1, control_1, control_2, point_2, angular );
        }
        else if ( !begin_on && !end_on )
        {
            xy point_1 = lines.points.back();
            xy split = lerp( begin_point, end_point, 0.5f );
            xy point_2 = lines.points[ first ];
            xy left_1 = lerp( point_1, end_point, 2.0f / 3.0f );
            xy left_2 = lerp( split, end_point, 2.0f / 3.0f );
            xy right_1 = lerp( split, begin_point, 2.0f / 3.0f );
            xy right_2 = lerp( point_2, begin_point, 2.0f / 3.0f );
            add_bezier( point_1, left_1, left_2, split, angular );
            add_bezier( split, right_1, right_2, point_2, angular );
        }
        lines.points.push_back( lines.points[ first ] );
        subpath_data entry = { lines.points.size() - first, true };
        lines.subpaths.push_back( entry );
    }
}

// Decode the next codepoint from a null-terminated UTF-8 string to its glyph
// index within the font.  The index to the next codepoint in the string
// is advanced accordingly.  It checks for valid UTF-8 encoding, but not
// for valid unicode codepoints.  Where it finds an invalid encoding, it
// decodes it as the Unicode replacement character (U+FFFD) and advances to
// the invalid byte, per Unicode recommendation.  It also replaces low-ASCII
// whitespace characters with regular spaces.  After decoding the codepoint,
// it looks up the corresponding glyph index from the current font's character
// map table, returning a glyph index of 0 for the .notdef character (i.e.,
// "tofu") if the font lacks a glyph for that codepoint.
//
int canvas::character_to_glyph(
    char const *text,
    int &index )
{
    int bytes = ( ( text[ index ] & 0x80 ) == 0x00 ? 1 :
                  ( text[ index ] & 0xe0 ) == 0xc0 ? 2 :
                  ( text[ index ] & 0xf0 ) == 0xe0 ? 3 :
                  ( text[ index ] & 0xf8 ) == 0xf0 ? 4 :
                  0 );
    int const masks[] = { 0x0, 0x7f, 0x1f, 0x0f, 0x07 };
    int codepoint = bytes ? text[ index ] & masks[ bytes ] : 0xfffd;
    ++index;
    while ( --bytes > 0 )
        if ( ( text[ index ] & 0xc0 ) == 0x80 )
            codepoint = codepoint << 6 | ( text[ index++ ] & 0x3f );
        else
        {
            codepoint = 0xfffd;
            break;
        }
    if ( codepoint == '\t' || codepoint == '\v' || codepoint == '\f' ||
         codepoint == '\r' || codepoint == '\n' )
        codepoint = ' ';
    int tables = unsigned_16( face.data, face.cmap + 2 );
    int format_12 = 0;
    int format_4 = 0;
    int format_0 = 0;
    for ( int table = 0; table < tables; ++table )
    {
        int platform = unsigned_16( face.data, face.cmap + table * 8 + 4 );
        int encoding = unsigned_16( face.data, face.cmap + table * 8 + 6 );
        int offset = signed_32( face.data, face.cmap + table * 8 + 8 );
        int format = unsigned_16( face.data, face.cmap + offset );
        if ( platform == 3 && encoding == 10 && format == 12 )
            format_12 = face.cmap + offset;
        else if ( platform == 3 && encoding == 1 && format == 4 )
            format_4 = face.cmap + offset;
        else if ( format == 0 )
            format_0 = face.cmap + offset;
    }
    if ( format_12 )
    {
        int groups = signed_32( face.data, format_12 + 12 );
        for ( int group = 0; group < groups; ++group )
        {
            int start = signed_32( face.data, format_12 + 16 + group * 12 );
            int end = signed_32( face.data, format_12 + 20 + group * 12 );
            int glyph = signed_32( face.data, format_12 + 24 + group * 12 );
            if ( start <= codepoint && codepoint <= end )
                return codepoint - start + glyph;
        }
    }
    else if ( format_4 )
    {
        int segments = unsigned_16( face.data, format_4 + 6 );
        int end_array = format_4 + 14;
        int start_array = end_array + 2 + segments;
        int delta_array = start_array + segments;
        int range_array = delta_array + segments;
        for ( int segment = 0; segment < segments; segment += 2 )
        {
            int start = unsigned_16( face.data, start_array + segment );
            int end = unsigned_16( face.data, end_array + segment );
            int delta = signed_16( face.data, delta_array + segment );
            int range = unsigned_16( face.data, range_array + segment );
            if ( start <= codepoint && codepoint <= end )
                return range ?
                    unsigned_16( face.data, range_array + segment +
                                 ( codepoint - start ) * 2 + range ) :
                    ( codepoint + delta ) & 0xffff;
        }
    }
    else if ( format_0 && 0 <= codepoint && codepoint < 256 )
        return unsigned_8( face.data, format_0 + 6 + codepoint );
    return 0;
}

void canvas::get_font_metrics(int& ascent, int& descent, int& height, int& x_height)
{
    if (face.data.empty()) return;

    // https://www.w3.org/TR/css-inline/#ascent-descent
    // "It is recommended that implementations that use OpenType or TrueType fonts use the metrics sTypoAscender and sTypoDescender from the font’s OS/2 table"
    float sTypoAscender = (float)signed_16(face.data, face.os_2 + 68);
    float sTypoDescender = (float)signed_16(face.data, face.os_2 + 70);
    // Some fonts, eg. Inconsolata, don't have the sxHeight field (it is defined in the second version of OS/2 table).
    // If it is absent (yMax - yMin) for 'x' from glyf table can be used.
    int os2ver = unsigned_16(face.data, face.os_2);
    float sxHeight = os2ver >= 2 ? (float)signed_16(face.data, face.os_2 + 86) : 0;

    ascent = (int)ceil(sTypoAscender * face.scale);
    descent = (int)ceil(-sTypoDescender * face.scale);

    // https://www.w3.org/TR/css-inline/#font-line-gap
    // https://www.w3.org/TR/css-inline/#inline-height
    // In several fonts I examined, including Inconsolata and csstest-ascii.ttf from w3.org,
    // both OS/2 sTypoLineGap and hhea lineGap are either 0 or very small (I used https://fontdrop.info/ viewer).
    // cairo container sets height to ascent + descent, litehtml uses this value as a normal line height and for baseline calculations.
    height = (int)ceil((sTypoAscender - sTypoDescender) * face.scale);

    x_height = (int)ceil(sxHeight * face.scale);
}

// Convert a text string to a set of polylines.  This works out the placement
// of the text string relative to the anchor position.  Then it walks through
// the string, sizing and placing each character by temporarily changing the
// current transform matrix to map from font units to canvas pixel coordinates
// before adding the glyph to the polylines.  This replaces the previous
// polyline data.
//
void canvas::text_to_lines(
    char const *text,
    xy position,
    float maximum_width,
    bool stroking )
{
    static float const tolerance = 0.125f;
    float ratio = tolerance / std::max( 0.5f * line_width, tolerance );
    float angular = stroking ? ( ratio - 2.0f ) * ratio * 2.0f + 1.0f : -1.0f;
    lines.points.clear();
    lines.subpaths.clear();
    if ( face.data.empty() || !text || maximum_width <= 0.0f )
        return;
    float width = maximum_width == 1.0e30f && text_align == leftward ? 0.0f :
        measure_text( text );
    float reduction = maximum_width / std::max( maximum_width, width );
    if ( text_align == rightward )
        position.x -= width * reduction;
    else if ( text_align == center )
        position.x -= 0.5f * width * reduction;
    xy scaling = face.scale * xy( reduction, 1.0f );
    float units_per_em = static_cast< float >(
        unsigned_16( face.data, face.head + 18 ) );
    float ascender = static_cast< float >(
        signed_16( face.data, face.os_2 + 68 ) );
    float descender = static_cast< float >(
        signed_16( face.data, face.os_2 + 70 ) );
    if ( text_baseline == top )
        position.y += ascender * face.scale;
    else if ( text_baseline == middle )
        position.y += ( ascender - descender ) * 0.5f * face.scale;
    else if ( text_baseline == bottom )
        position.y += descender * face.scale;
    else if ( text_baseline == hanging )
        position.y += 0.6f * face.scale * units_per_em;
    affine_matrix saved_forward = forward;
    affine_matrix saved_inverse = inverse;
    int hmetrics = unsigned_16( face.data, face.hhea + 34 );
    int place = 0;
    for ( int index = 0; text[ index ]; )
    {
        int glyph = character_to_glyph( text, index );
        forward = saved_forward;
        transform( scaling.x, 0.0f, 0.0f, -scaling.y,
                   position.x + static_cast< float >( place ) * scaling.x,
                   position.y );
        add_glyph( glyph, angular );
        int entry = std::min( glyph, hmetrics - 1 );
        place += unsigned_16( face.data, face.hmtx + entry * 4 );
    }
    forward = saved_forward;
    inverse = saved_inverse;
}

// Break the polylines into smaller pieces according to the dash settings.
// This walks along the polyline subpaths and dash pattern together, emitting
// new points via lerping where dash segments begin and end.  Each dash
// segment becomes a new open subpath in the polyline.  Some care is to
// taken to handle two special cases of closed subpaths.  First, those that
// are completely within the first dash segment should be emitted as-is and
// remain closed.  Secondly, those that start and end within a dash should
// have the two dashes merged together so that the lines join.  This replaces
// the previous polyline data.
//
void canvas::dash_lines()
{
    if ( line_dash.empty() )
        return;
    lines.points.swap( scratch.points );
    lines.points.clear();
    lines.subpaths.swap( scratch.subpaths );
    lines.subpaths.clear();
    float total = std::accumulate( line_dash.begin(), line_dash.end(), 0.0f );
    float offset = fmodf( line_dash_offset, total );
    if ( offset < 0.0f )
        offset += total;
    size_t start = 0;
    while ( offset >= line_dash[ start ] )
    {
        offset -= line_dash[ start ];
        start = start + 1 < line_dash.size() ? start + 1 : 0;
    }
    size_t ending = 0;
    for ( size_t subpath = 0; subpath < scratch.subpaths.size(); ++subpath )
    {
        size_t index = ending;
        ending += scratch.subpaths[ subpath ].count;
        size_t first = lines.points.size();
        size_t segment = start;
        bool emit = ~start & 1;
        size_t merge_point = lines.points.size();
        size_t merge_subpath = lines.subpaths.size();
        bool merge_emit = emit;
        float next = line_dash[ start ] - offset;
        for ( ; index + 1 < ending; ++index )
        {
            xy from = scratch.points[ index ];
            xy to = scratch.points[ index + 1 ];
            if ( emit )
                lines.points.push_back( from );
            float line = length( inverse * to - inverse * from );
            while ( next < line )
            {
                lines.points.push_back( lerp( from, to, next / line ) );
                if ( emit )
                {
                    subpath_data entry = {
                        lines.points.size() - first, false };
                    lines.subpaths.push_back( entry );
                    first = lines.points.size();
                }
                segment = segment + 1 < line_dash.size() ? segment + 1 : 0;
                emit = !emit;
                next += line_dash[ segment ];
            }
            next -= line;
        }
        if ( emit )
        {
            lines.points.push_back( scratch.points[ index ] );
            subpath_data entry = { lines.points.size() - first, false };
            lines.subpaths.push_back( entry );
            if ( scratch.subpaths[ subpath ].closed && merge_emit )
            {
                if ( lines.subpaths.size() == merge_subpath + 1 )
                    lines.subpaths.back().closed = true;
                else
                {
                    size_t count = lines.subpaths.back().count;
                    std::rotate( ( lines.points.begin() +
                                   static_cast< ptrdiff_t >( merge_point ) ),
                                 ( lines.points.end() -
                                   static_cast< ptrdiff_t >( count ) ),
                                 lines.points.end() );
                    lines.subpaths[ merge_subpath ].count += count;
                    lines.subpaths.pop_back();
                }
            }
        }
    }
}

// Trace along a series of points from a subpath in the scratch polylines
// and add new points to the main polylines with the stroke expansion on
// one side.  Calling this again with the ends reversed adds the other
// half of the stroke.  If the original subpath was closed, each pass
// adds a complete closed loop, with one adding the inside and one adding
// the outside.  The two will wind in opposite directions.  If the original
// subpath was open, each pass ends with one of the line caps and the two
// passes together form a single closed loop.  In either case, this handles
// adding line joins, including inner joins.  Care is taken to fill tight
// inside turns correctly by adding additional windings.  See Figure 10 of
// "Converting Stroked Primitives to Filled Primitives" by Diego Nehab, for
// the inspiration for these extra windings.
//
void canvas::add_half_stroke(
    size_t beginning,
    size_t ending,
    bool closed )
{
    float half = line_width * 0.5f;
    float ratio = miter_limit * miter_limit * half * half;
    xy in_direction = xy( 0.0f, 0.0f );
    float in_length = 0.0f;
    xy point = inverse * scratch.points[ beginning ];
    size_t finish = beginning;
    size_t index = beginning;
    do
    {
        xy next = inverse * scratch.points[ index ];
        xy out_direction = normalized( next - point );
        float out_length = length( next - point );
        static float const epsilon = 1.0e-4f;
        if ( in_length != 0.0f && out_length >= epsilon )
        {
            if ( closed && finish == beginning )
                finish = index;
            xy side_in = point + half * perpendicular( in_direction );
            xy side_out = point + half * perpendicular( out_direction );
            float turn = dot( perpendicular( in_direction ), out_direction );
            if ( fabsf( turn ) < epsilon )
                turn = 0.0f;
            xy offset = turn == 0.0f ? xy( 0.0f, 0.0f ) :
                half / turn * ( out_direction - in_direction );
            bool tight = ( dot( offset, in_direction ) < -in_length &&
                           dot( offset, out_direction ) > out_length );
            if ( turn > 0.0f && tight )
            {
                std::swap( side_in, side_out );
                std::swap( in_direction, out_direction );
                lines.points.push_back( forward * side_out );
                lines.points.push_back( forward * point );
                lines.points.push_back( forward * side_in );
            }
            if ( ( turn > 0.0f && !tight ) ||
                 ( turn != 0.0f && line_join == miter &&
                   dot( offset, offset ) <= ratio ) )
                lines.points.push_back( forward * ( point + offset ) );
            else if ( line_join == rounded )
            {
                float cosine = dot( in_direction, out_direction );
                float angle = acosf(
                    std::min( std::max( cosine, -1.0f ), 1.0f ) );
                float alpha = 4.0f / 3.0f * tanf( 0.25f * angle );
                lines.points.push_back( forward * side_in );
                add_bezier(
                    forward * side_in,
                    forward * ( side_in + alpha * half * in_direction ),
                    forward * ( side_out - alpha * half * out_direction ),
                    forward * side_out,
                    -1.0f );
            }
            else
            {
                lines.points.push_back( forward * side_in );
                lines.points.push_back( forward * side_out );
            }
            if ( turn > 0.0f && tight )
            {
                lines.points.push_back( forward * side_out );
                lines.points.push_back( forward * point );
                lines.points.push_back( forward * side_in );
                std::swap( in_direction, out_direction );
            }
        }
        if ( out_length >= epsilon )
        {
            in_direction = out_direction;
            in_length = out_length;
            point = next;
        }
        index = ( index == ending ? beginning :
                  ending > beginning ? index + 1 :
                  index - 1 );
    } while ( index != finish );
    if ( closed || in_length == 0.0f )
        return;
    xy ahead = half * in_direction;
    xy side = perpendicular( ahead );
    if ( line_cap == butt )
    {
        lines.points.push_back( forward * ( point + side ) );
        lines.points.push_back( forward * ( point - side ) );
    }
    else if ( line_cap == square )
    {
        lines.points.push_back( forward * ( point + ahead + side ) );
        lines.points.push_back( forward * ( point + ahead - side ) );
    }
    else if ( line_cap == circle )
    {
        static float const alpha = 0.55228475f; // 4/3*tan(pi/8)
        lines.points.push_back( forward * ( point + side ) );
        add_bezier( forward * ( point + side ),
                    forward * ( point + side + alpha * ahead ),
                    forward * ( point + ahead + alpha * side ),
                    forward * ( point + ahead ),
                    -1.0f );
        add_bezier( forward * ( point + ahead ),
                    forward * ( point + ahead - alpha * side ),
                    forward * ( point - side + alpha * ahead ),
                    forward * ( point - side ),
                    -1.0f );
    }
}

// Perform stroke expansion on the polylines.  After first breaking them up
// according to the dash pattern (if any), it then moves the polyline data to
// the scratch space.  Each subpath is then traced both forwards and backwards
// to add the points for a half stroke, which together create the points for
// one (if the original subpath was open) or two (if it was closed) new closed
// subpaths which, when filled, will draw the stroked lines.  While the lower
// level code this calls only adds the points of the half strokes, this
// adds subpath information for the strokes.  This replaces the previous
// polyline data.
//
void canvas::stroke_lines()
{
    if ( forward.a * forward.d - forward.b * forward.c == 0.0f )
        return;
    dash_lines();
    lines.points.swap( scratch.points );
    lines.points.clear();
    lines.subpaths.swap( scratch.subpaths );
    lines.subpaths.clear();
    size_t ending = 0;
    for ( size_t subpath = 0; subpath < scratch.subpaths.size(); ++subpath )
    {
        size_t beginning = ending;
        ending += scratch.subpaths[ subpath ].count;
        if ( ending - beginning < 2 )
            continue;
        size_t first = lines.points.size();
        add_half_stroke( beginning, ending - 1,
                         scratch.subpaths[ subpath ].closed );
        if ( scratch.subpaths[ subpath ].closed )
        {
            subpath_data entry = { lines.points.size() - first, true };
            lines.subpaths.push_back( entry );
            first = lines.points.size();
        }
        add_half_stroke( ending - 1, beginning,
                         scratch.subpaths[ subpath ].closed );
        subpath_data entry = { lines.points.size() - first, true };
        lines.subpaths.push_back( entry );
    }
}

// Scan-convert a single polyline segment.  This walks along the pixels that
// the segment touches in left-to-right order, using signed trapezoidal area
// to accumulate a list of changes in signed coverage at each visible pixel
// when processing them from left to right.  Each run of horizontal pixels
// ends with one final update to the right of the last pixel to bring the
// total up to date.  Note that this does not clip to the screen boundary.
//
void canvas::add_runs(
    xy from,
    xy to )
{
    static float const epsilon = 2.0e-5f;
    if ( fabsf( to.y - from.y ) < epsilon)
        return;
    float sign = to.y > from.y ? 1.0f : -1.0f;
    if ( from.x > to.x )
        std::swap( from, to );
    xy now = from;
    xy pixel = xy( floorf( now.x ), floorf( now.y ) );
    xy corner = pixel + xy( 1.0f, to.y > from.y ? 1.0f : 0.0f );
    xy slope = xy( ( to.x - from.x ) / ( to.y - from.y ),
                   ( to.y - from.y ) / ( to.x - from.x ) );
    xy next_x = ( to.x - from.x < epsilon ) ? to :
        xy( corner.x, now.y + ( corner.x - now.x ) * slope.y );
    xy next_y = xy( now.x + ( corner.y - now.y ) * slope.x, corner.y );
    if ( ( from.y < to.y && to.y < next_y.y ) ||
         ( from.y > to.y && to.y > next_y.y ) )
        next_y = to;
    float y_step = to.y > from.y ? 1.0f : -1.0f;
    do
    {
        float carry = 0.0f;
        while ( next_x.x < next_y.x )
        {
            float strip = std::min( std::max( ( next_x.y - now.y ) * y_step,
                                              0.0f ), 1.0f );
            float mid = ( next_x.x + now.x ) * 0.5f;
            float area = ( mid - pixel.x ) * strip;
            pixel_run piece = { static_cast< unsigned short >( pixel.x ),
                                static_cast< unsigned short >( pixel.y ),
                                ( carry + strip - area ) * sign };
            runs.push_back( piece );
            carry = area;
            now = next_x;
            next_x.x += 1.0f;
            next_x.y = ( next_x.x - from.x ) * slope.y + from.y;
            pixel.x += 1.0f;
        }
        float strip = std::min( std::max( ( next_y.y - now.y ) * y_step,
                                          0.0f ), 1.0f );
        float mid = ( next_y.x + now.x ) * 0.5f;
        float area = ( mid - pixel.x ) * strip;
        pixel_run piece_1 = { static_cast< unsigned short >( pixel.x ),
                              static_cast< unsigned short >( pixel.y ),
                              ( carry + strip - area ) * sign };
        pixel_run piece_2 = { static_cast< unsigned short >( pixel.x + 1.0f ),
                              static_cast< unsigned short >( pixel.y ),
                              area * sign };
        runs.push_back( piece_1 );
        runs.push_back( piece_2 );
        now = next_y;
        next_y.y += y_step;
        next_y.x = ( next_y.y - from.y ) * slope.x + from.x;
        pixel.y += y_step;
        if ( ( from.y < to.y && to.y < next_y.y ) ||
             ( from.y > to.y && to.y > next_y.y ) )
            next_y = to;
    } while ( now.y != to.y );
}

static bool operator<(
    pixel_run left,
    pixel_run right )
{
    return ( left.y < right.y ? true :
             left.y > right.y ? false :
             left.x < right.x ? true :
             left.x > right.x ? false :
             fabsf( left.delta ) < fabsf( right.delta ) );
}

// Scan-convert the polylines to prepare them for antialiased rendering.
// For each of the polyline loops, it first clips them to the screen.
// See "Reentrant Polygon Clipping" by Sutherland and Hodgman for details.
// Then it walks the polyline loop and scan-converts each line segment to
// produce a list of changes in signed pixel coverage when processed in
// left-to-right, top-to-bottom order.  The list of changes is then sorted
// into that order, and multiple changes to the same pixel are coalesced
// by summation.  The result is a sparse, run-length encoded description
// of the coverage of each pixel to be drawn.
//
void canvas::lines_to_runs(
    xy offset,
    int padding )
{
    runs.clear();
    float width = static_cast< float >( size_x + padding );
    float height = static_cast< float >( size_y + padding );
    size_t ending = 0;
    for ( size_t subpath = 0; subpath < lines.subpaths.size(); ++subpath )
    {
        size_t beginning = ending;
        ending += lines.subpaths[ subpath ].count;
        scratch.points.clear();
        for ( size_t index = beginning; index < ending; ++index )
            scratch.points.push_back( offset + lines.points[ index ] );
        for ( int edge = 0; edge < 4; ++edge )
        {
            xy normal = xy( edge == 0 ? 1.0f : edge == 2 ? -1.0f : 0.0f,
                            edge == 1 ? 1.0f : edge == 3 ? -1.0f : 0.0f );
            float place = edge == 2 ? width : edge == 3 ? height : 0.0f;
            size_t first = scratch.points.size();
            for ( size_t index = 0; index < first; ++index )
            {
                xy from = scratch.points[ ( index ? index : first ) - 1 ];
                xy to = scratch.points[ index ];
                float from_side = dot( from, normal ) + place;
                float to_side = dot( to, normal ) + place;
                if ( from_side * to_side < 0.0f )
                    scratch.points.push_back( lerp( from, to,
                        from_side / ( from_side - to_side ) ) );
                if ( to_side >= 0.0f )
                    scratch.points.push_back( to );
            }
            scratch.points.erase(
                scratch.points.begin(),
                scratch.points.begin() + static_cast< ptrdiff_t >( first ) );
        }
        size_t last = scratch.points.size();
        for ( size_t index = 0; index < last; ++index )
        {
            xy from = scratch.points[ ( index ? index : last ) - 1 ];
            xy to = scratch.points[ index ];
            add_runs( xy( std::min( std::max( from.x, 0.0f ), width ),
                          std::min( std::max( from.y, 0.0f ), height ) ),
                      xy( std::min( std::max( to.x, 0.0f ), width ),
                          std::min( std::max( to.y, 0.0f ), height ) ) );
        }
    }
    if ( runs.empty() )
        return;
    std::sort( runs.begin(), runs.end() );
    size_t to = 0;
    for ( size_t from = 1; from < runs.size(); ++from )
        if ( runs[ from ].x == runs[ to ].x &&
             runs[ from ].y == runs[ to ].y )
            runs[ to ].delta += runs[ from ].delta;
        else if ( runs[ from ].delta != 0.0f )
            runs[ ++to ] = runs[ from ];
    runs.erase( runs.begin() + static_cast< ptrdiff_t >( to ) + 1,
                runs.end() );
}

// Paint a pixel according to its point location and a paint style to produce
// a premultiplied, linearized RGBA color.  This handles all supported paint
// styles: solid colors, linear gradients, radial gradients, and patterns.
// For gradients and patterns, it takes into account the current transform.
// Patterns are resampled using a separable bicubic convolution filter,
// with edges handled according to the wrap mode.  See "Cubic Convolution
// Interpolation for Digital Image Processing" by Keys.  This filter is best
// known for magnification, but also works well for antialiased minification,
// since it's actually a Catmull-Rom spline approximation of Lanczos-2.
//
rgba canvas::paint_pixel(
    xy point,
    paint_brush const &brush )
{
    if ( brush.colors.empty() )
        return rgba( 0.0f, 0.0f, 0.0f, 0.0f );
    if ( brush.type == paint_brush::color )
        return brush.colors.front();
    point = inverse * point;
    if ( brush.type == paint_brush::pattern )
    {
        float width = static_cast< float >( brush.width );
        float height = static_cast< float >( brush.height );
        if ( ( ( brush.repetition & 2 ) &&
               ( point.x < 0.0f || width <= point.x ) ) ||
             ( ( brush.repetition & 1 ) &&
               ( point.y < 0.0f || height <= point.y ) ) )
            return rgba( 0.0f, 0.0f, 0.0f, 0.0f );
        float scale_x = fabsf( inverse.a ) + fabsf( inverse.c );
        float scale_y = fabsf( inverse.b ) + fabsf( inverse.d );
        scale_x = std::max( 1.0f, std::min( scale_x, width * 0.25f ) );
        scale_y = std::max( 1.0f, std::min( scale_y, height * 0.25f ) );
        float reciprocal_x = 1.0f / scale_x;
        float reciprocal_y = 1.0f / scale_y;
        point -= xy( 0.5f, 0.5f );
        int left = static_cast< int >( ceilf( point.x - scale_x * 2.0f ) );
        int top = static_cast< int >( ceilf( point.y - scale_y * 2.0f ) );
        int right = static_cast< int >( ceilf( point.x + scale_x * 2.0f ) );
        int bottom = static_cast< int >( ceilf( point.y + scale_y * 2.0f ) );
        rgba total_color = rgba( 0.0f, 0.0f, 0.0f, 0.0f );
        float total_weight = 0.0f;
        for ( int pattern_y = top; pattern_y < bottom; ++pattern_y )
        {
            float y = fabsf( reciprocal_y *
                ( static_cast< float >( pattern_y ) - point.y ) );
            float weight_y = ( y < 1.0f ?
                (    1.5f * y - 2.5f ) * y          * y + 1.0f :
                ( ( -0.5f * y + 2.5f ) * y - 4.0f ) * y + 2.0f );
            int wrapped_y = pattern_y % brush.height;
            if ( wrapped_y < 0 )
                wrapped_y += brush.height;
            if ( &brush == &image_brush )
                wrapped_y = std::min( std::max( pattern_y, 0 ),
                                      brush.height - 1 );
            for ( int pattern_x = left; pattern_x < right; ++pattern_x )
            {
                float x = fabsf( reciprocal_x *
                    ( static_cast< float >( pattern_x ) - point.x ) );
                float weight_x = ( x < 1.0f ?
                    (    1.5f * x - 2.5f ) * x          * x + 1.0f :
                    ( ( -0.5f * x + 2.5f ) * x - 4.0f ) * x + 2.0f );
                int wrapped_x = pattern_x % brush.width;
                if ( wrapped_x < 0 )
                    wrapped_x += brush.width;
                if ( &brush == &image_brush )
                    wrapped_x = std::min( std::max( pattern_x, 0 ),
                                          brush.width - 1 );
                float weight = weight_x * weight_y;
                size_t index = static_cast< size_t >(
                    wrapped_y * brush.width + wrapped_x );
                total_color += weight * brush.colors[ index ];
                total_weight += weight;
            }
        }
        return ( 1.0f / total_weight ) * total_color;
    }
    float offset = 0;
    xy relative = point - brush.start;
    xy line = brush.end - brush.start;
    float gradient = dot( relative, line );
    float span = dot( line, line );
    if ( brush.type == paint_brush::linear )
    {
        if ( span == 0.0f )
            return rgba( 0.0f, 0.0f, 0.0f, 0.0f );
        offset = gradient / span;
    }
    else if (brush.type == paint_brush::radial)
    {
        float initial = brush.start_radius;
        float change = brush.end_radius - initial;
        float a = span - change * change;
        float b = -2.0f * ( gradient + initial * change );
        float c = dot( relative, relative ) - initial * initial;
        float discriminant = b * b - 4.0f * a * c;
        if ( discriminant < 0.0f ||
             ( span == 0.0f && change == 0.0f ) )
            return rgba( 0.0f, 0.0f, 0.0f, 0.0f );
        float root = sqrtf( discriminant );
        float reciprocal = 1.0f / ( 2.0f * a );
        float offset_1 = ( -b - root ) * reciprocal;
        float offset_2 = ( -b + root ) * reciprocal;
        float radius_1 = initial + change * offset_1;
        float radius_2 = initial + change * offset_2;
        if ( radius_2 >= 0.0f )
            offset = offset_2;
        else if ( radius_1 >= 0.0f )
            offset = offset_1;
        else
            return rgba( 0.0f, 0.0f, 0.0f, 0.0f );
    }
    else if (brush.type == paint_brush::css_radial)
    {
        if (brush.css_radius.x == 0 || brush.css_radius.y == 0)
            offset = 1;
        else
        {
            xy rel = {relative.x / brush.css_radius.x, relative.y / brush.css_radius.y};
            offset = length(rel);
        }
    }
    else if (brush.type == paint_brush::conic)
    {
        float angle = 90 + direction(relative) * 180 / pi;
        offset = normalize_angle(angle - brush.angle) / 360;
    }
    size_t index = static_cast< size_t >(
        std::upper_bound( brush.stops.begin(), brush.stops.end(), offset ) -
        brush.stops.begin() );
    if ( index == 0 )
        return premultiplied( brush.colors.front() );
    if ( index == brush.stops.size() )
        return premultiplied( brush.colors.back() );
    struct { rgba color; float stop; std::optional<float> hint; }
        A = { brush.colors[index-1], brush.stops[index-1], brush.hints[index-1] },
        B = { brush.colors[index],   brush.stops[index],   {}                   };
    // https://drafts.csswg.org/css-images-4/#coloring-gradient-line
    // 1. Determine the location of the transition hint as a percentage of the distance between the two color stops, denoted as a number between 0 and 1
    float H = !A.hint ? .5f : (*A.hint - A.stop) / (B.stop - A.stop);
    // 2. For any given point between the two color stops, determine the point’s location as a percentage of the distance between the two color stops, in the same way as the previous step.
    float P = (offset - A.stop) / (B.stop - A.stop);
    // 3. Let C, the color weighting at that point, be equal to P^logH(.5).
    float C = pow(P, log(.5f) / log(H));
    // 4. The color at that point is then a linear blend between the colors of the two color stops, blending (1 - C) of the first stop and C of the second stop.
    return premultiplied((1 - C) * A.color + C * B.color);
}

// Render the shadow of the polylines into the pixel buffer if needed.  After
// computing the border as the maximum distance that one pixel can affect
// another via the blur, it scan-converts the lines to runs with the shadow
// offset and that extra amount of border padding.  Then it bounds the scan
// converted shape, pads this with border, clips that to the extended canvas
// size, and rasterizes to fill a working area with the alpha.  Next, a fast
// approximation of a Gaussian blur is done using three passes of box blurs
// each in the rows and columns.  Note that these box blurs have a small extra
// weight on the tails to allow for fractional widths.  See "Theoretical
// Foundations of Gaussian Convolution by Extended Box Filtering" by Gwosdek
// et al. for details.  Finally, it colors the blurred alpha image with
// the shadow color and blends this into the pixel buffer according to the
// compositing settings and clip mask.  Note that it does not bother clearing
// outside the area of the alpha image when the compositing settings require
// clearing; that will be done on the subsequent main rendering pass.
//
void canvas::render_shadow(
    paint_brush const &brush )
{
    if ( shadow_color.a == 0.0f || ( shadow_blur == 0.0f &&
                                     shadow_offset_x == 0.0f &&
                                     shadow_offset_y == 0.0f ) )
        return;
    float sigma_squared = 0.25f * shadow_blur * shadow_blur;
    size_t radius = static_cast< size_t >(
        0.5f * sqrtf( 4.0f * sigma_squared + 1.0f ) - 0.5f );
    int border = 3 * ( static_cast< int >( radius ) + 1 );
    xy offset = xy( static_cast< float >( border ) + shadow_offset_x,
                    static_cast< float >( border ) + shadow_offset_y );
    lines_to_runs( offset, 2 * border );
    int left = size_x + 2 * border;
    int right = 0;
    int top = size_y + 2 * border;
    int bottom = 0;
    for ( size_t index = 0; index < runs.size(); ++index )
    {
        left = std::min( left, static_cast< int >( runs[ index ].x ) );
        right = std::max( right, static_cast< int >( runs[ index ].x ) );
        top = std::min( top, static_cast< int >( runs[ index ].y ) );
        bottom = std::max( bottom, static_cast< int >( runs[ index ].y ) );
    }
    left = std::max( left - border, 0 );
    right = std::min( right + border, size_x + 2 * border ) + 1;
    top = std::max( top - border, 0 );
    bottom = std::min( bottom + border, size_y + 2 * border );
    size_t width = static_cast< size_t >( std::max( right - left, 0 ) );
    size_t height = static_cast< size_t >( std::max( bottom - top, 0 ) );
    size_t working = width * height;
    shadow.clear();
    shadow.resize( working + std::max( width, height ) );
    static float const threshold = 1.0f / 8160.0f;
    {
        int x = -1;
        int y = -1;
        float sum = 0.0f;
        for ( size_t index = 0; index < runs.size(); ++index )
        {
            pixel_run next = runs[ index ];
            float coverage = std::min( fabsf( sum ), 1.0f );
            int to = next.y == y ? next.x : x + 1;
            if ( coverage >= threshold )
                for ( ; x < to; ++x )
                    shadow[ static_cast< size_t >( y - top ) * width +
                            static_cast< size_t >( x - left ) ] = coverage *
                        paint_pixel( xy( static_cast< float >( x ) + 0.5f,
                                         static_cast< float >( y ) + 0.5f ) -
                                     offset, brush ).a;
            if ( next.y != y )
                sum = 0.0f;
            x = next.x;
            y = next.y;
            sum += next.delta;
        }
    }
    float alpha = static_cast< float >( 2 * radius + 1 ) *
        ( static_cast< float >( radius * ( radius + 1 ) ) - sigma_squared ) /
        ( 2.0f * sigma_squared -
          static_cast< float >( 6 * ( radius + 1 ) * ( radius + 1 ) ) );
    float divisor = 2.0f * ( alpha + static_cast< float >( radius ) ) + 1.0f;
    float weight_1 = alpha / divisor;
    float weight_2 = ( 1.0f - alpha ) / divisor;
    for ( size_t y = 0; y < height; ++y )
        for ( int pass = 0; pass < 3; ++pass )
        {
            for ( size_t x = 0; x < width; ++x )
                shadow[ working + x ] = shadow[ y * width + x ];
            float running = weight_1 * shadow[ working + radius + 1 ];
            for ( size_t x = 0; x <= radius; ++x )
                running += ( weight_1 + weight_2 ) * shadow[ working + x ];
            shadow[ y * width ] = running;
            for ( size_t x = 1; x < width; ++x )
            {
                if ( x >= radius + 1 )
                    running -= weight_2 * shadow[ working + x - radius - 1 ];
                if ( x >= radius + 2 )
                    running -= weight_1 * shadow[ working + x - radius - 2 ];
                if ( x + radius < width )
                    running += weight_2 * shadow[ working + x + radius ];
                if ( x + radius + 1 < width )
                    running += weight_1 * shadow[ working + x + radius + 1 ];
                shadow[ y * width + x ] = running;
            }
        }
    for ( size_t x = 0; x < width; ++x )
        for ( int pass = 0; pass < 3; ++pass )
        {
            for ( size_t y = 0; y < height; ++y )
                shadow[ working + y ] = shadow[ y * width + x ];
            float running = weight_1 * shadow[ working + radius + 1 ];
            for ( size_t y = 0; y <= radius; ++y )
                running += ( weight_1 + weight_2 ) * shadow[ working + y ];
            shadow[ x ] = running;
            for ( size_t y = 1; y < height; ++y )
            {
                if ( y >= radius + 1 )
                    running -= weight_2 * shadow[ working + y - radius - 1 ];
                if ( y >= radius + 2 )
                    running -= weight_1 * shadow[ working + y - radius - 2 ];
                if ( y + radius < height )
                    running += weight_2 * shadow[ working + y + radius ];
                if ( y + radius + 1 < height )
                    running += weight_1 * shadow[ working + y + radius + 1 ];
                shadow[ y * width + x ] = running;
            }
        }
    int operation = global_composite_operation;
    int x = -1;
    int y = -1;
    float sum = 0.0f;
    for ( size_t index = 0; index < mask.size(); ++index )
    {
        pixel_run next = mask[ index ];
        float visibility = std::min( fabsf( sum ), 1.0f );
        int to = std::min( next.y == y ? next.x : x + 1, right - border );
        if ( visibility >= threshold &&
             top <= y + border && y + border < bottom )
            for ( ; x < to; ++x )
            {
                rgba &back = bitmap[ y * size_x + x ];
                rgba fore = global_alpha *
                    shadow[
                        static_cast< size_t >( y + border - top ) * width +
                        static_cast< size_t >( x + border - left ) ] *
                    shadow_color;
                float mix_fore = operation & 1 ? back.a : 0.0f;
                if ( operation & 2 )
                    mix_fore = 1.0f - mix_fore;
                float mix_back = operation & 4 ? fore.a : 0.0f;
                if ( operation & 8 )
                    mix_back = 1.0f - mix_back;
                rgba blend = mix_fore * fore + mix_back * back;
                blend.a = std::min( blend.a, 1.0f );
                back = visibility * blend + ( 1.0f - visibility ) * back;
            }
        if ( next.y != y )
            sum = 0.0f;
        x = std::max( static_cast< int >( next.x ), left - border );
        y = next.y;
        sum += next.delta;
    }
}

// Render the polylines into the pixel buffer.  It scan-converts the lines
// to runs which represent changes to the signed fractional coverage when
// read from left-to-right, top-to-bottom.  It scans through these to
// determine spans of pixels that need to be drawn, paints those pixels
// according to the brush, and then blends them into the buffer according
// to the current compositing settings.  This is slightly more complicated
// because it interleaves this with a simultaneous scan through a similar
// set of runs representing the current clip mask to determine which pixels
// it can composite into.  Note that shadows are always drawn first.
//
void canvas::render_main(
    paint_brush const &brush )
{
    if ( forward.a * forward.d - forward.b * forward.c == 0.0f )
        return;
    render_shadow( brush );
    lines_to_runs( xy( 0.0f, 0.0f ), 0 );
    int operation = global_composite_operation;
    int x = -1;
    int y = -1;
    float path_sum = 0.0f;
    float clip_sum = 0.0f;
    size_t path_index = 0;
    size_t clip_index = 0;
    while ( clip_index < mask.size() )
    {
        bool which = ( path_index < runs.size() &&
                       runs[ path_index ] < mask[ clip_index ] );
        pixel_run next = which ? runs[ path_index ] : mask[ clip_index ];
        float coverage = std::min( fabsf( path_sum ), 1.0f );
        float visibility = std::min( fabsf( clip_sum ), 1.0f );
        int to = next.y == y ? next.x : x + 1;
        static float const threshold = 1.0f / 8160.0f;
        if ( ( coverage >= threshold || ~operation & 8 ) &&
             visibility >= threshold )
            for ( ; x < to; ++x )
            {
                rgba &back = bitmap[ y * size_x + x ];
                rgba fore = coverage * global_alpha *
                    paint_pixel( xy( static_cast< float >( x ) + 0.5f,
                                     static_cast< float >( y ) + 0.5f ),
                                 brush );
                float mix_fore = operation & 1 ? back.a : 0.0f;
                if ( operation & 2 )
                    mix_fore = 1.0f - mix_fore;
                float mix_back = operation & 4 ? fore.a : 0.0f;
                if ( operation & 8 )
                    mix_back = 1.0f - mix_back;
                rgba blend = mix_fore * fore + mix_back * back;
                blend.a = std::min( blend.a, 1.0f );
                back = visibility * blend + ( 1.0f - visibility ) * back;
            }
        x = next.x;
        if ( next.y != y )
        {
            y = next.y;
            path_sum = 0.0f;
            clip_sum = 0.0f;
        }
        if ( which )
            path_sum += runs[ path_index++ ].delta;
        else
            clip_sum += mask[ clip_index++ ].delta;
    }
}

canvas::canvas(
    int width,
    int height )
    : global_composite_operation( source_over ),
      shadow_offset_x( 0.0f ),
      shadow_offset_y( 0.0f ),
      line_cap( butt ),
      line_join( miter ),
      line_dash_offset( 0.0f ),
      text_align( start ),
      text_baseline( alphabetic ),
      size_x( width ),
      size_y( height ),
      global_alpha( 1.0f ),
      shadow_blur( 0.0f ),
      line_width( 1.0f ),
      miter_limit( 10.0f ),
      fill_brush(),
      stroke_brush(),
      image_brush(),
      face(),
      bitmap( new rgba[ width * height ] ),
      saves( 0 )
{
    affine_matrix identity = { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };
    forward = identity;
    inverse = identity;
    set_color( fill_style, 0.0f, 0.0f, 0.0f, 1.0f );
    set_color( stroke_style, 0.0f, 0.0f, 0.0f, 1.0f );
    for ( unsigned short y = 0; y < size_y; ++y )
    {
        pixel_run piece_1 = { 0, y, 1.0f };
        pixel_run piece_2 = { static_cast< unsigned short >( size_x ), y,
                              -1.0f };
        mask.push_back( piece_1 );
        mask.push_back( piece_2 );
    }
}

canvas::canvas(int width, int height, rgba c) : canvas(width, height)
{
    save();
    set_color(fill_style, c.r, c.g, c.b, c.a);
    fill_rectangle(0, 0, (float)width, (float)height);
    restore();
}

canvas::~canvas()
{
    delete[] bitmap;
    while ( canvas *head = saves )
    {
        saves = head->saves;
        head->saves = 0;
        delete head;
    }
}

void canvas::scale(
    float x,
    float y )
{
    transform( x, 0.0f, 0.0f, y, 0.0f, 0.0f );
}

void canvas::rotate(
    float angle )
{
    float cosine = cosf( angle );
    float sine = sinf( angle );
    transform( cosine, sine, -sine, cosine, 0.0f, 0.0f );
}

void canvas::translate(
    float x,
    float y )
{
    transform( 1.0f, 0.0f, 0.0f, 1.0f, x, y );
}

void canvas::transform(
    float a,
    float b,
    float c,
    float d,
    float e,
    float f )
{
    set_transform( forward.a * a + forward.c * b,
                   forward.b * a + forward.d * b,
                   forward.a * c + forward.c * d,
                   forward.b * c + forward.d * d,
                   forward.a * e + forward.c * f + forward.e,
                   forward.b * e + forward.d * f + forward.f );
}

void canvas::set_transform(
    float a,
    float b,
    float c,
    float d,
    float e,
    float f )
{
    float determinant = a * d - b * c;
    float scaling = determinant != 0.0f ? 1.0f / determinant : 0.0f;
    affine_matrix new_forward = { a, b, c, d, e, f };
    affine_matrix new_inverse = {
        scaling * d, scaling * -b, scaling * -c, scaling * a,
        scaling * ( c * f - d * e ), scaling * ( b * e - a * f ) };
    forward = new_forward;
    inverse = new_inverse;
}

void canvas::set_global_alpha(
    float alpha )
{
    if ( 0.0f <= alpha && alpha <= 1.0f )
        global_alpha = alpha;
}

void canvas::set_shadow_color(
    float red,
    float green,
    float blue,
    float alpha )
{
    shadow_color = premultiplied( linearized( clamped(
        rgba( red, green, blue, alpha ) ) ) );
}

void canvas::set_shadow_blur(
    float level )
{
    if ( 0.0f <= level )
        shadow_blur = level;
}

void canvas::set_line_width(
    float width )
{
    if ( 0.0f < width )
        line_width = width;
}

void canvas::set_miter_limit(
    float limit )
{
    if ( 0.0f < limit )
        miter_limit = limit;
}

void canvas::set_line_dash(
    float const *segments,
    int count )
{
    for ( int index = 0; index < count; ++index )
        if ( segments && segments[ index ] < 0.0f )
            return;
    line_dash.clear();
    if ( !segments )
        return;
    for ( int index = 0; index < count; ++index )
        line_dash.push_back( segments[ index ] );
    if ( count & 1 )
        for ( int index = 0; index < count; ++index )
            line_dash.push_back( segments[ index ] );
}

void canvas::set_color(
    brush_type type,
    float red,
    float green,
    float blue,
    float alpha )
{
    paint_brush &brush = type == fill_style ? fill_brush : stroke_brush;
    brush.type = paint_brush::color;
    brush.colors.clear();
    brush.colors.push_back( premultiplied( linearized( clamped(
        rgba( red, green, blue, alpha ) ) ) ) );
}

void canvas::set_linear_gradient(
    brush_type type,
    float start_x,
    float start_y,
    float end_x,
    float end_y )
{
    paint_brush &brush = type == fill_style ? fill_brush : stroke_brush;
    brush.type = paint_brush::linear;
    brush.colors.clear();
    brush.stops.clear();
    brush.hints.clear();
    brush.start = xy( start_x, start_y );
    brush.end = xy( end_x, end_y );
}

void canvas::set_radial_gradient(
    brush_type type,
    float start_x,
    float start_y,
    float start_radius,
    float end_x,
    float end_y,
    float end_radius )
{
    if ( start_radius < 0.0f || end_radius < 0.0f )
        return;
    paint_brush &brush = type == fill_style ? fill_brush : stroke_brush;
    brush.type = paint_brush::radial;
    brush.colors.clear();
    brush.stops.clear();
    brush.hints.clear();
    brush.start = xy( start_x, start_y );
    brush.end = xy( end_x, end_y );
    brush.start_radius = start_radius;
    brush.end_radius = end_radius;
}

void canvas::set_css_radial_gradient(
    brush_type type,
    float x,
    float y,
    float radius_x,
    float radius_y)
{
    if (radius_x < 0.0f || radius_y < 0.0f)
        return;
    paint_brush& brush = type == fill_style ? fill_brush : stroke_brush;
    brush.type = paint_brush::css_radial;
    brush.colors.clear();
    brush.stops.clear();
    brush.hints.clear();
    brush.start = {x, y};
    brush.css_radius = {radius_x, radius_y};
}

void canvas::set_conic_gradient(
    brush_type type,
    float x,
    float y,
    float angle)
{
    paint_brush& brush = type == fill_style ? fill_brush : stroke_brush;
    brush.type = paint_brush::conic;
    brush.colors = {};
    brush.stops = {};
    brush.hints = {};
    brush.start = {x, y};
    brush.angle = angle;
}

void canvas::add_color_stop(
    brush_type type,
    float offset,
    float red,
    float green,
    float blue,
    float alpha,
    std::optional<float> hint )
{
    paint_brush &brush = type == fill_style ? fill_brush : stroke_brush;
    if ( ( brush.type != paint_brush::linear &&
           brush.type != paint_brush::radial &&
           brush.type != paint_brush::css_radial &&
           brush.type != paint_brush::conic ) ||
         offset < 0.0f || 1.0f < offset )
        return;
    ptrdiff_t index = std::upper_bound(
        brush.stops.begin(), brush.stops.end(), offset ) -
        brush.stops.begin();
    rgba color = linearized( clamped( rgba( red, green, blue, alpha ) ) );
    brush.colors.insert( brush.colors.begin() + index, color );
    brush.stops.insert( brush.stops.begin() + index, offset );
    brush.hints.insert( brush.hints.begin() + index, hint );
}

void canvas::set_pattern(
    brush_type type,
    unsigned char const *image,
    int width,
    int height,
    int stride,
    repetition_style repetition )
{
    if ( !image || width <= 0 || height <= 0 )
        return;
    paint_brush &brush = type == fill_style ? fill_brush : stroke_brush;
    brush.type = paint_brush::pattern;
    brush.colors.clear();
    for ( int y = 0; y < height; ++y )
        for ( int x = 0; x < width; ++x )
        {
            int index = y * stride + x * 4;
            rgba color = rgba(
                image[ index + 0 ] / 255.0f, image[ index + 1 ] / 255.0f,
                image[ index + 2 ] / 255.0f, image[ index + 3 ] / 255.0f );
            brush.colors.push_back( premultiplied( linearized( color ) ) );
        }
    brush.width = width;
    brush.height = height;
    brush.repetition = repetition;
}

void canvas::begin_path()
{
    path.points.clear();
    path.subpaths.clear();
}

void canvas::move_to(
    float x,
    float y )
{
    if ( !path.subpaths.empty() && path.subpaths.back().count == 1 )
    {
        path.points.back() = forward * xy( x, y );
        return;
    }
    subpath_data subpath = { 1, false };
    path.points.push_back( forward * xy( x, y ) );
    path.subpaths.push_back( subpath );
}

void canvas::close_path()
{
    if ( path.subpaths.empty() )
        return;
    xy first = path.points[ path.points.size() - path.subpaths.back().count ];
    affine_matrix saved_forward = forward;
    affine_matrix identity = { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };
    forward = identity;
    line_to( first.x, first.y );
    path.subpaths.back().closed = true;
    move_to( first.x, first.y );
    forward = saved_forward;
}

void canvas::line_to(
    float x,
    float y )
{
    if ( path.subpaths.empty() )
    {
        move_to( x, y );
        return;
    }
    xy point_1 = path.points.back();
    xy point_2 = forward * xy( x, y );
    if ( dot( point_2 - point_1, point_2 - point_1 ) == 0.0f )
        return;
    path.points.push_back( point_1 );
    path.points.push_back( point_2 );
    path.points.push_back( point_2 );
    path.subpaths.back().count += 3;
}

void canvas::quadratic_curve_to(
    float control_x,
    float control_y,
    float x,
    float y )
{
    if ( path.subpaths.empty() )
        move_to( control_x, control_y );
    xy point_1 = path.points.back();
    xy control = forward * xy( control_x, control_y );
    xy point_2 = forward * xy( x, y );
    xy control_1 = lerp( point_1, control, 2.0f / 3.0f );
    xy control_2 = lerp( point_2, control, 2.0f / 3.0f );
    path.points.push_back( control_1 );
    path.points.push_back( control_2 );
    path.points.push_back( point_2 );
    path.subpaths.back().count += 3;
}

void canvas::bezier_curve_to(
    float control_1_x,
    float control_1_y,
    float control_2_x,
    float control_2_y,
    float x,
    float y )
{
    if ( path.subpaths.empty() )
        move_to( control_1_x, control_1_y );
    xy control_1 = forward * xy( control_1_x, control_1_y );
    xy control_2 = forward * xy( control_2_x, control_2_y );
    xy point_2 = forward * xy( x, y );
    path.points.push_back( control_1 );
    path.points.push_back( control_2 );
    path.points.push_back( point_2 );
    path.subpaths.back().count += 3;
}

void canvas::arc_to(
    float vertex_x,
    float vertex_y,
    float x,
    float y,
    float radius )
{
    if ( radius < 0.0f ||
         forward.a * forward.d - forward.b * forward.c == 0.0f )
        return;
    if ( path.subpaths.empty() )
        move_to( vertex_x, vertex_y );
    xy point_1 = inverse * path.points.back();
    xy vertex = xy( vertex_x, vertex_y );
    xy point_2 = xy( x, y );
    xy edge_1 = normalized( point_1 - vertex );
    xy edge_2 = normalized( point_2 - vertex );
    float sine = fabsf( dot( perpendicular( edge_1 ), edge_2 ) );
    static float const epsilon = 1.0e-4f;
    if ( sine < epsilon )
    {
        line_to( vertex_x, vertex_y );
        return;
    }
    xy offset = radius / sine * ( edge_1 + edge_2 );
    xy center = vertex + offset;
    float angle_1 = direction( dot( offset, edge_1 ) * edge_1 - offset );
    float angle_2 = direction( dot( offset, edge_2 ) * edge_2 - offset );
    bool reverse = static_cast< int >(
        floorf( ( angle_2 - angle_1 ) / pi ) ) & 1;
    arc( center.x, center.y, radius, angle_1, angle_2, reverse );
}

void canvas::arc(
    float x,
    float y,
    float radius,
    float start_angle,
    float end_angle,
    bool counter_clockwise )
{
    if ( radius < 0.0f )
        return;
    static float const tau = 2 * pi;
    float winding = counter_clockwise ? -1.0f : 1.0f;
    float from = fmodf( start_angle, tau );
    float span = fmodf( end_angle, tau ) - from;
    if ( ( end_angle - start_angle ) * winding >= tau )
        span = tau * winding;
    else if ( span * winding < 0.0f )
        span += tau * winding;
    xy centered_1 = radius * xy( cosf( from ), sinf( from ) );
    line_to( x + centered_1.x, y + centered_1.y );
    if ( span == 0.0f )
        return;
    int steps = static_cast< int >(
        std::max( 1.0f, roundf( 16.0f / tau * span * winding ) ) );
    float segment = span / static_cast< float >( steps );
    float alpha = 4.0f / 3.0f * tanf( 0.25f * segment );
    for ( int step = 0; step < steps; ++step )
    {
        float angle = from + static_cast< float >( step + 1 ) * segment;
        xy centered_2 = radius * xy( cosf( angle ), sinf( angle ) );
        xy point_1 = xy( x, y ) + centered_1;
        xy point_2 = xy( x, y ) + centered_2;
        xy control_1 = point_1 + alpha * perpendicular( centered_1 );
        xy control_2 = point_2 - alpha * perpendicular( centered_2 );
        bezier_curve_to( control_1.x, control_1.y,
                         control_2.x, control_2.y,
                         point_2.x, point_2.y );
        centered_1 = centered_2;
    }
}

void canvas::rectangle(
    float x,
    float y,
    float width,
    float height )
{
    move_to( x, y );
    line_to( x + width, y );
    line_to( x + width, y + height );
    line_to( x, y + height );
    close_path();
}

void canvas::polygon(std::vector<xy> points)
{
    move_to(points[0].x, points[0].y);
    for (auto pt : points)
        line_to(pt.x, pt.y);
    close_path();
}

void canvas::fill()
{
    path_to_lines( false );
    render_main( fill_brush );
}

void canvas::stroke()
{
    path_to_lines( true );
    stroke_lines();
    render_main( stroke_brush );
}

void canvas::clip()
{
    path_to_lines( false );
    lines_to_runs( xy( 0.0f, 0.0f ), 0 );
    size_t part = runs.size();
    runs.insert( runs.end(), mask.begin(), mask.end() );
    mask.clear();
    int y = -1;
    float last = 0.0f;
    float sum_1 = 0.0f;
    float sum_2 = 0.0f;
    size_t index_1 = 0;
    size_t index_2 = part;
    while ( index_1 < part && index_2 < runs.size() )
    {
        bool which = runs[ index_1 ] < runs[ index_2 ];
        pixel_run next = which ? runs[ index_1 ] : runs[ index_2 ];
        if ( next.y != y )
        {
            y = next.y;
            last = 0.0f;
            sum_1 = 0.0f;
            sum_2 = 0.0f;
        }
        if ( which )
            sum_1 += runs[ index_1++ ].delta;
        else
            sum_2 += runs[ index_2++ ].delta;
        float visibility = ( std::min( fabsf( sum_1 ), 1.0f ) *
                             std::min( fabsf( sum_2 ), 1.0f ) );
        if ( visibility == last )
            continue;
        if ( !mask.empty() &&
             mask.back().x == next.x && mask.back().y == next.y )
            mask.back().delta += visibility - last;
        else
        {
            pixel_run piece = { next.x, next.y, visibility - last };
            mask.push_back( piece );
        }
        last = visibility;
    }
}

bool canvas::is_point_in_path(
    float x,
    float y )
{
    path_to_lines( false );
    int winding = 0;
    size_t subpath = 0;
    size_t beginning = 0;
    size_t ending = 0;
    for ( size_t index = 0; index < lines.points.size(); ++index )
    {
        while ( index >= ending )
        {
            beginning = ending;
            ending += lines.subpaths[ subpath++ ].count;
        }
        xy from = lines.points[ index ];
        xy to = lines.points[ index + 1 < ending ? index + 1 : beginning ];
        if ( ( from.y < y && y <= to.y ) || ( to.y < y && y <= from.y ) )
        {
            float side = dot( perpendicular( to - from ), xy( x, y ) - from );
            if ( side == 0.0f )
                return true;
            winding += side > 0.0f ? 1 : -1;
        }
        else if ( from.y == y && y == to.y &&
                  ( ( from.x <= x && x <= to.x ) ||
                    ( to.x <= x && x <= from.x ) ) )
            return true;
    }
    return winding;
}

void canvas::clear_rectangle(
    float x,
    float y,
    float width,
    float height )
{
    composite_operation saved_operation = global_composite_operation;
    float saved_global_alpha = global_alpha;
    float saved_alpha = shadow_color.a;
    paint_brush::types saved_type = fill_brush.type;
    global_composite_operation = destination_out;
    global_alpha = 1.0f;
    shadow_color.a = 0.0f;
    fill_brush.type = paint_brush::color;
    fill_rectangle( x, y, width, height );
    fill_brush.type = saved_type;
    shadow_color.a = saved_alpha;
    global_alpha = saved_global_alpha;
    global_composite_operation = saved_operation;
}

void canvas::fill_rectangle(
    float x,
    float y,
    float width,
    float height )
{
    if ( width == 0.0f || height == 0.0f )
        return;
    lines.points.clear();
    lines.subpaths.clear();
    lines.points.push_back( forward * xy( x, y ) );
    lines.points.push_back( forward * xy( x + width, y ) );
    lines.points.push_back( forward * xy( x + width, y + height ) );
    lines.points.push_back( forward * xy( x, y + height ) );
    subpath_data entry = { 4, true };
    lines.subpaths.push_back( entry );
    render_main( fill_brush );
}

void canvas::stroke_rectangle(
    float x,
    float y,
    float width,
    float height )
{
    if ( width == 0.0f && height == 0.0f )
        return;
    lines.points.clear();
    lines.subpaths.clear();
    if ( width == 0.0f || height == 0.0f )
    {
        lines.points.push_back( forward * xy( x, y ) );
        lines.points.push_back( forward * xy( x + width, y + height ) );
        subpath_data entry = { 2, false };
        lines.subpaths.push_back( entry );
    }
    else
    {
        lines.points.push_back( forward * xy( x, y ) );
        lines.points.push_back( forward * xy( x + width, y ) );
        lines.points.push_back( forward * xy( x + width, y + height ) );
        lines.points.push_back( forward * xy( x, y + height ) );
        lines.points.push_back( forward * xy( x, y ) );
        subpath_data entry = { 5, true };
        lines.subpaths.push_back( entry );
    }
    stroke_lines();
    render_main( stroke_brush );
}

bool canvas::set_font(
    unsigned char const *font,
    int bytes,
    float size )
{
    if ( font && bytes )
    {
        face.data.clear();
        face.cmap = 0;
        face.glyf = 0;
        face.head = 0;
        face.hhea = 0;
        face.hmtx = 0;
        face.loca = 0;
        face.maxp = 0;
        face.os_2 = 0;
        if ( bytes < 6 )
            return false;
        int version = ( font[ 0 ] << 24 | font[ 1 ] << 16 |
                        font[ 2 ] <<  8 | font[ 3 ] <<  0 );
        int tables = font[ 4 ] << 8 | font[ 5 ];
        if ( ( version != 0x00010000 && version != 0x74727565 ) ||
             bytes < tables * 16 + 12 )
            return false;
        face.data.insert( face.data.end(), font, font + tables * 16 + 12 );
        for ( int index = 0; index < tables; ++index )
        {
            int tag = signed_32( face.data, index * 16 + 12 );
            int offset = signed_32( face.data, index * 16 + 20 );
            int span = signed_32( face.data, index * 16 + 24 );
            if ( bytes < offset + span )
            {
                face.data.clear();
                return false;
            }
            int place = static_cast< int >( face.data.size() );
            if ( tag == 0x636d6170 )
                face.cmap = place;
            else if ( tag == 0x676c7966 )
                face.glyf = place;
            else if ( tag == 0x68656164 )
                face.head = place;
            else if ( tag == 0x68686561 )
                face.hhea = place;
            else if ( tag == 0x686d7478 )
                face.hmtx = place;
            else if ( tag == 0x6c6f6361 )
                face.loca = place;
            else if ( tag == 0x6d617870 )
                face.maxp = place;
            else if ( tag == 0x4f532f32 )
                face.os_2 = place;
            else
                continue;
            face.data.insert(
                face.data.end(), font + offset, font + offset + span );
        }
        if ( !face.cmap || !face.glyf || !face.head || !face.hhea ||
             !face.hmtx || !face.loca || !face.maxp || !face.os_2 )
        {
            face.data.clear();
            return false;
        }
    }
    if ( face.data.empty() )
        return false;
    int units_per_em = unsigned_16( face.data, face.head + 18 );
    face.scale = size / static_cast< float >( units_per_em );
    return true;
}

void canvas::fill_text(
    char const *text,
    float x,
    float y,
    float maximum_width )
{
    text_to_lines( text, xy( x, y ), maximum_width, false );
    render_main( fill_brush );
}

void canvas::stroke_text(
    char const *text,
    float x,
    float y,
    float maximum_width )
{
    text_to_lines( text, xy( x, y ), maximum_width, true );
    stroke_lines();
    render_main( stroke_brush );
}

float canvas::measure_text(
    char const *text )
{
    if ( face.data.empty() || !text )
        return 0.0f;
    int hmetrics = unsigned_16( face.data, face.hhea + 34 );
    int width = 0;
    for ( int index = 0; text[ index ]; )
    {
        int glyph = character_to_glyph( text, index );
        int entry = std::min( glyph, hmetrics - 1 );
        width += unsigned_16( face.data, face.hmtx + entry * 4 );
    }
    return static_cast< float >( width ) * face.scale;
}

void canvas::draw_image(
    unsigned char const *image,
    int width,
    int height,
    int stride,
    float x,
    float y,
    float to_width,
    float to_height )
{
    if ( !image || width <= 0 || height <= 0 ||
         to_width == 0.0f || to_height == 0.0f )
        return;
    std::swap( fill_brush, image_brush );
    set_pattern( fill_style, image, width, height, stride, repeat );
    std::swap( fill_brush, image_brush );
    lines.points.clear();
    lines.subpaths.clear();
    lines.points.push_back( forward * xy( x, y ) );
    lines.points.push_back( forward * xy( x + to_width, y ) );
    lines.points.push_back( forward * xy( x + to_width, y + to_height ) );
    lines.points.push_back( forward * xy( x, y + to_height ) );
    subpath_data entry = { 4, true };
    lines.subpaths.push_back( entry );
    affine_matrix saved_forward = forward;
    affine_matrix saved_inverse = inverse;
    translate( x + std::min( 0.0f, to_width ),
               y + std::min( 0.0f, to_height ) );
    scale( fabsf( to_width ) / static_cast< float >( width ),
           fabsf( to_height ) / static_cast< float >( height ) );
    render_main( image_brush );
    forward = saved_forward;
    inverse = saved_inverse;
}

void canvas::get_image_data(
    unsigned char *image,
    int width,
    int height,
    int stride,
    int x,
    int y )
{
    if ( !image )
        return;
    static float const bayer[][ 4 ] = {
        { 0.03125f, 0.53125f, 0.15625f, 0.65625f },
        { 0.78125f, 0.28125f, 0.90625f, 0.40625f },
        { 0.21875f, 0.71875f, 0.09375f, 0.59375f },
        { 0.96875f, 0.46875f, 0.84375f, 0.34375f } };
    for ( int image_y = 0; image_y < height; ++image_y )
        for ( int image_x = 0; image_x < width; ++image_x )
        {
            int index = image_y * stride + image_x * 4;
            int canvas_x = x + image_x;
            int canvas_y = y + image_y;
            rgba color = rgba( 0.0f, 0.0f, 0.0f, 0.0f );
            if ( 0 <= canvas_x && canvas_x < size_x &&
                 0 <= canvas_y && canvas_y < size_y )
                color = bitmap[ canvas_y * size_x + canvas_x ];
            float threshold = bayer[ canvas_y & 3 ][ canvas_x & 3 ];
            color = rgba( threshold, threshold, threshold, threshold ) +
                255.0f * delinearized( clamped( unpremultiplied( color ) ) );
            image[ index + 0 ] = static_cast< unsigned char >( color.r );
            image[ index + 1 ] = static_cast< unsigned char >( color.g );
            image[ index + 2 ] = static_cast< unsigned char >( color.b );
            image[ index + 3 ] = static_cast< unsigned char >( color.a );
        }
}

void canvas::put_image_data(
    unsigned char const *image,
    int width,
    int height,
    int stride,
    int x,
    int y )
{
    if ( !image )
        return;
    for ( int image_y = 0; image_y < height; ++image_y )
        for ( int image_x = 0; image_x < width; ++image_x )
        {
            int index = image_y * stride + image_x * 4;
            int canvas_x = x + image_x;
            int canvas_y = y + image_y;
            if ( canvas_x < 0 || size_x <= canvas_x ||
                 canvas_y < 0 || size_y <= canvas_y )
                continue;
            rgba color = rgba(
                image[ index + 0 ] / 255.0f, image[ index + 1 ] / 255.0f,
                image[ index + 2 ] / 255.0f, image[ index + 3 ] / 255.0f );
            bitmap[ canvas_y * size_x + canvas_x ] =
                premultiplied( linearized( color ) );
        }
}

void canvas::save()
{
    canvas *state = new canvas( 0, 0 );
    state->global_composite_operation = global_composite_operation;
    state->shadow_offset_x = shadow_offset_x;
    state->shadow_offset_y = shadow_offset_y;
    state->line_cap = line_cap;
    state->line_join = line_join;
    state->line_dash_offset = line_dash_offset;
    state->text_align = text_align;
    state->text_baseline = text_baseline;
    state->forward = forward;
    state->inverse = inverse;
    state->global_alpha = global_alpha;
    state->shadow_color = shadow_color;
    state->shadow_blur = shadow_blur;
    state->line_width = line_width;
    state->miter_limit = miter_limit;
    state->line_dash = line_dash;
    state->fill_brush = fill_brush;
    state->stroke_brush = stroke_brush;
    state->mask = mask;
    state->face = face;
    state->saves = saves;
    saves = state;
}

void canvas::restore()
{
    if ( !saves )
        return;
    canvas *state = saves;
    global_composite_operation = state->global_composite_operation;
    shadow_offset_x = state->shadow_offset_x;
    shadow_offset_y = state->shadow_offset_y;
    line_cap = state->line_cap;
    line_join = state->line_join;
    line_dash_offset = state->line_dash_offset;
    text_align = state->text_align;
    text_baseline = state->text_baseline;
    forward = state->forward;
    inverse = state->inverse;
    global_alpha = state->global_alpha;
    shadow_color = state->shadow_color;
    shadow_blur = state->shadow_blur;
    line_width = state->line_width;
    miter_limit = state->miter_limit;
    line_dash = state->line_dash;
    fill_brush = state->fill_brush;
    stroke_brush = state->stroke_brush;
    mask = state->mask;
    face = state->face;
    saves = state->saves;
    state->saves = 0;
    delete state;
}

}

#endif // CANVAS_ITY_IMPLEMENTATION
