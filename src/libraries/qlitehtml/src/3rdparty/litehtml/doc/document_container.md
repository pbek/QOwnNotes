# litehtml::document_container

`litehtml::document_container` class draws the HTML elements and perform some actions depended of the platform. This is the only class you should implement in your application.

```cpp
class document_container
{
public:
    virtual litehtml::uint_ptr  create_font(const font_description& descr, const document* doc, litehtml::font_metrics* fm) = 0;
    virtual void                delete_font(litehtml::uint_ptr hFont) = 0;
    virtual pixel_t             text_width(const char* text, litehtml::uint_ptr hFont) = 0;
    virtual void                draw_text(litehtml::uint_ptr hdc, const char* text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos) = 0;
    virtual pixel_t             pt_to_px(float pt) const = 0;
    virtual pixel_t             get_default_font_size() const = 0;
    virtual const char*         get_default_font_name() const = 0;
    virtual void                draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker) = 0;
    virtual void                load_image(const char* src, const char* baseurl, bool redraw_on_ready) = 0;
    virtual void                get_image_size(const char* src, const char* baseurl, litehtml::size& sz) = 0;
    virtual void                draw_image(litehtml::uint_ptr hdc, const background_layer& layer, const std::string& url, const std::string& base_url) = 0;
    virtual void                draw_solid_fill(litehtml::uint_ptr hdc, const background_layer& layer, const web_color& color) = 0;
    virtual void                draw_linear_gradient(litehtml::uint_ptr hdc, const background_layer& layer, const background_layer::linear_gradient& gradient) = 0;
    virtual void                draw_radial_gradient(litehtml::uint_ptr hdc, const background_layer& layer, const background_layer::radial_gradient& gradient) = 0;
    virtual void                draw_conic_gradient(litehtml::uint_ptr hdc, const background_layer& layer, const background_layer::conic_gradient& gradient) = 0;
    virtual void                draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root) = 0;

    virtual void                set_caption(const char* caption) = 0;
    virtual void                set_base_url(const char* base_url) = 0;
    virtual void                link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el) = 0;
    virtual void                on_anchor_click(const char* url, const litehtml::element::ptr& el) = 0;
    virtual bool                on_element_click(const litehtml::element::ptr& /*el*/) { return false; };
    virtual void                on_mouse_event(const litehtml::element::ptr& el, litehtml::mouse_event event) = 0;
    virtual void                set_cursor(const char* cursor) = 0;
    virtual void                transform_text(litehtml::string& text, litehtml::text_transform tt) = 0;
    virtual void                import_css(litehtml::string& text, const litehtml::string& url, litehtml::string& baseurl) = 0;
    virtual void                set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius) = 0;
    virtual void                del_clip() = 0;
    virtual void                get_viewport(litehtml::position& viewport) const = 0;
    virtual litehtml::element::ptr create_element( const char* tag_name,
                                                   const litehtml::string_map& attributes,
                                                   const std::shared_ptr<litehtml::document>& doc) = 0;

    virtual void                get_media_features(litehtml::media_features& media) const = 0;
    virtual void                get_language(litehtml::string& language, litehtml::string& culture) const = 0;
    virtual litehtml::string    resolve_color(const litehtml::string& /*color*/) const { return litehtml::string(); }
    virtual void                split_text(const char* text, const std::function<void(const char*)>& on_word, const std::function<void(const char*)>& on_space);

protected:
    virtual ~document_container() = default;
};

```

Below is the list of member functions you have to implement:

1. **Font Management**

- [create_font](#create_font)
- [delete_font](#delete_font)
- [text_width](#text_width)
- [draw_text](#draw_text)
- [pt_to_px](#pt_to_px)
- [get_default_font_size](#get_default_font_size)
- [get_default_font_name](#get_default_font_name)

2. **Drawing**

- [draw_list_marker](#draw_list_marker)
- [draw_image](#draw_image)
- [draw_solid_fill](#draw_solid_fill)
- [draw_linear_gradient](#draw_linear_gradient)
- [draw_radial_gradient](#draw_radial_gradient)
- [draw_conic_gradient](#draw_conic_gradient)
- [draw_borders](#draw_borders)

3. **Image Handling**

- [load_image](#load_image)
- [get_image_size](#get_image_size)

4. **HTML Element Interaction**

- [link](#link)
- [on_anchor_click](#on_anchor_click)
- [on_element_click](#on_element_click)
- [on_mouse_event](#on_mouse_event)

5. **CSS and Styling**

- [import_css](#import_css)
- [transform_text](#transform_text)
- [set_cursor](#set_cursor)

6. **Clipping**

- [set_clip](#set_clip)
- [del_clip](#del_clip)

7. **Viewport and Media**

- [get_viewport](#get_viewport)
- [get_media_features](#get_media_features)

8. **Custom Elements**

- [create_element](#create_element)

9. **Localization**

- [get_language](#get_language)

10. **Miscellaneous**

- [set_caption](#set_caption)
- [set_base_url](#set_base_url)
- [resolve_color](#resolve_color)
- [split_text](#split_text)

### create_font

```cpp
virtual litehtml::uint_ptr create_font(const font_description& descr, const document* doc, litehtml::font_metrics* fm);
```

This function called by litehtml to create the font. `create_font` returns `uint_ptr` that must identify the created font.
Parameters:

- `const font_description& descr` - the description of the font. This structure contains the font face name, size, weight and decoration.

`font_description` definition:

```cpp
struct font_description
{
    std::string             family;
    int                     size;
    font_style              style;
    int                     weight;
    int                     decoration_line;
    css_length              decoration_thickness;
    text_decoration_style   decoration_style;
    web_color               decoration_color;
    std::string             emphasis_style;
    web_color               emphasis_color;
    int                     emphasis_position;
};
```

- `family` - Font family name as described in [CSS specifications](https://developer.mozilla.org/en-US/docs/Web/CSS/font-family). You have to select the proper font.
- `size` - Font size in pixels.
- `style` - Font style:
  - `font_style_normal` - normal font style.
  - `font_style_italic` - italic font style.
  - `font_style_oblique` - oblique font style.
- `weight` - Font weight. Numerical value from 100 to 900. 400 is the same as normal, and 700 is the same as bold. See [CSS specifications](https://developer.mozilla.org/en-US/docs/Web/CSS/font-weight).
- `decoration_line` - one or more flags:
  - `const unsigned int font_decoration_none=0x00;`
  - `const unsigned int font_decoration_underline=0x01;`
  - `const unsigned int font_decoration_linethrough=0x02;`
  - `const unsigned int font_decoration_overline=0x04;`
- `decoration_thickness` - the thickness of the decoration line in pixels. See predefined values in enum text_decoration_thickness.
- `decoration_style` - the style of the decoration line. See enum text_decoration_style.
- `decoration_color` - the color of the decoration line.
- `emphasis_style` - the emphasis style.
- `emphasis_color` - the emphasis color.
- `emphasis_position` - the emphasis position.

---

- `litehtml::font_metrics* fm` Defines the font metrics. This is the output format so you have to fill this structure with the font metrics:

```cpp
struct font_metrics
{
    int     font_size;
    int     height;
    int     ascent;
    int     descent;
    int     x_height;
    int     ch_width;
    bool    draw_spaces;
    int     sub_shift;
    int     super_shift;
};
```

- `font_size` - the font size in pixels. The same as size argument of the create_font function.
- `height` - the recommended vertical distance between baselines when setting consecutive lines of text with the font. This is greater than `ascent+descent` by a quantity known as the line spacing or external leading. When space is at a premium, most fonts can be set with only a distance of `ascent+descent` between lines.
- `ascent` - the distance from the baseline to the top of a line of text.
- `descent` - the distance from the baseline to the bottom of a line of text.
- `x_height` - height of the symbol x.
- `ch_width` - width of the symbol 0.
- `draw_spaces` - true to call draw text function for spaces. If false, just use space width without draw.
- `sub_shift` - the baseline shift for subscripts.
- `super_shift` - the baseline shift for superscripts.

### delete_font

```cpp
virtual void delete_font(uint_ptr hFont);
```

delete the font created in [create_font](#create_font) function

### text_width

```cpp
virtual pixel_t text_width(const char* text, uint_ptr hFont);
```

Returns the text width.

### draw_text

```cpp
virtual void draw_text(uint_ptr hdc, const char* text, uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos);
```

This function draw the text string.

### pt_to_px

```cpp
virtual pixel_t pt_to_px(float pt);
```

Convert **points** into **pixels**.

### get_default_font_size

```cpp
virtual pixel_t get_default_font_size();
```

Returns the default font size in pixels.

### get_default_font_name

```cpp
virtual const char* get_default_font_name();
```

Returns the default font face name.

### draw_list_marker

```cpp
virtual void draw_list_marker(uint_ptr hdc, const litehtml::list_marker& marker);
```

Draws the list marker. Use the parameter **marker** to find the marker type and position.

### load_image

```cpp
virtual void load_image(const char* src, const char* baseurl, bool redraw_on_ready);
```

You can preload image in this function. litehtml does not cache the images, so you have to create own images cache if you need it.

### get_image_size

```cpp
virtual void get_image_size(const char* src, const char* baseurl, litehtml::size& sz);
```

Fill the **sz** parameter with image width and height.

## Draw Background members

There are five functions to draw the background:

- [draw_image](#draw_image) draw the image background. CSS example: `background: no-repeat url("image.png");`. Also this function is used to draw the `<img>` tags.
- [draw_solid_fill](#draw_solid_fill) draw the solid color background. CSS example: `background-color: #FF0000;`.
- [draw_linear_gradient](#draw_linear_gradient) draw the linear gradient background. CSS example: `background: linear-gradient(to right, #FF0000, #00FF00);`.
- [draw_radial_gradient](#draw_radial_gradient) draw the radial gradient background. CSS example: `background: radial-gradient(circle, #FF0000, #00FF00);`.
- [draw_conic_gradient](#draw_conic_gradient) draw the conic gradient background. CSS example: `background: conic-gradient(#FF0000, #00FF00);`.

All background functions accept the `background_layer` parameter. This structure defines the position and size of the background.
The definition of the `background_layer` structure:

```cpp
class background_layer
{
public:
    position border_box;
    border_radiuses border_radius;
    position clip_box;
    position origin_box;
    background_attachment attachment;
    background_repeat repeat;
    bool is_root;
}
```

- `border_box` - defines the draw boundary. Everything must be drawn inside this rectangle only.
- `border_radius` - defines the radius of the border_box.
- `clip_box` - defines the clipping rectangle. Works like border_box. Container must set additional clipping.
- `origin_box` - defines the origin rectangle.
- `attachment` - defines the background attachment:
  - `background_attachment_fixed` - fixed background.
  - `background_attachment_scroll` - scroll background.
- `repeat` - defines the background repeat:
  - `background_repeat_no_repeat` - no repeat.
  - `background_repeat_repeat` - repeat.
  - `background_repeat_repeat_x` - repeat horizontally.
  - `background_repeat_repeat_y` - repeat vertically.
- `is_root` - is true for a root element. Container can use this flag to apply background to the top window.

The gradient related functions accept the parameter based on the `background_layer::gradient_base` structure:

```cpp
class gradient_base
{
public:
    vector<color_point>  color_points;
    color_space_t        color_space       = color_space_none;
    hue_interpolation_t  hue_interpolation = hue_interpolation_none;
};
```

- `color_points` - the color points of the gradient. Each point is defined by the color and position.
- `color_space` - the color space of the gradient. See enum `color_space_t`
- `hue_interpolation` - the hue interpolation of the gradient. See enum `hue_interpolation_t`.

### draw_image

```cpp
virtual void draw_image(litehtml::uint_ptr hdc, const background_layer& layer, const std::string& url, const std::string& base_url);
```

Draw the image in this function. The parameter `layer` describes the image position and size. The `url` and `base_url` parameters are the image source.

### draw_solid_fill

```cpp
virtual void draw_solid_fill(uint_ptr hdc, const background_layer& layer, const web_color& color);
```

Draw the solid color in the background. The `layer` parameter defines the position and size of the rectangle. The `color` parameter is the color to fill.

### draw_linear_gradient

```cpp
virtual void draw_linear_gradient(uint_ptr hdc, const background_layer& layer, const background_layer::linear_gradient& gradient);
```

Draw the linear gradient in the background. The `layer` parameter defines the position and size of the rectangle. The `gradient` parameter is the gradient to fill.

The definition of the `background_layer::linear_gradient` structure:

```cpp
class linear_gradient : public gradient_base
{
public:
    pointF start;
    pointF end;
};
```

- `start` - the start point of the gradient.
- `end` - the end point of the gradient.

### draw_radial_gradient

```cpp
virtual void draw_radial_gradient(uint_ptr hdc, const background_layer& layer, const background_layer::radial_gradient& gradient);
```

Draw the radial gradient in the background. The `layer` parameter defines the position and size of the rectangle. The `gradient` parameter is the gradient to fill.

The definition of the `background_layer::radial_gradient` structure:

```cpp
class radial_gradient : public gradient_base
{
public:
    pointF position;
    pointF radius;
};
```

- `position` - the center of the gradient.
- `radius` - the radius of the gradient.

### draw_conic_gradient

```cpp
virtual void draw_conic_gradient(uint_ptr hdc, const background_layer& layer, const background_layer::conic_gradient& gradient);
```

Draw the conic gradient in the background. The `layer` parameter defines the position and size of the rectangle. The `gradient` parameter is the gradient to fill.

The definition of the `background_layer::conic_gradient` structure:

```cpp
class conic_gradient : public gradient_base
{
public:
    pointF position;
    float  angle;
    float  radius;
};
```

- `position` - the center of the conic gradient
- `angle` - the angle of the gradient in degrees, starting from 0 at the top and going clockwise
- `radius` - the distance from the center to the farthest corner of the background box

### draw_borders

```cpp
virtual void draw_borders(uint_ptr hdc, const css_borders& borders, const litehtml::position& draw_pos, bool root);
```

Draw the element borders here. The parameter `root` is `true` if you have to draw the borders for the root element (usually this is `<html>`).

### set_caption

```cpp
virtual void set_caption(const char* caption);
```

litehtml calls this function with `<title>` html tag text. You can use the `caption` parameter to set the window caption text into the html page title.

### set_base_url

```cpp
virtual void set_base_url(const char* base_url);
```

litehtml calls this function for the `<base>` html tag to set the base url. Save this string for future use in the functions that get the `baseurl` parameter.

### link

```cpp
virtual void link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el);
```

This function is used to process the `<link>` html tags. Note, litehtml processes the stylesheets references automatically and calls import_css function to load CSS file.

### on_anchor_click

```cpp
virtual void on_anchor_click(const char* url, litehtml::element::ptr el);
```

litehtml calls this function on anchor element click. You can open new website or do something other here.

### on_element_click

```cpp
virtual bool on_element_click(const litehtml::element::ptr& el)
```

litehtml calls this function on element click.

### on_mouse_event

```cpp
virtual void on_mouse_event(const litehtml::element::ptr& el, litehtml::mouse_event event);
```

litehtml calls this function on mouse event. The parameter **el** is the element that received the event. The **event** parameter is the type of the event:

- `mouse_event_enter` - mouse over the element.
- `mouse_event_leave` - mouse leave the element.

### set_cursor

```cpp
virtual void set_cursor(const char* cursor);
```

Define this function to handle the CSS cursor property.

### transform_text

```cpp
virtual void transform_text(std::string& text, litehtml::text_transform tt);
```

Transform the `text` parameter according the `tt` value:

- `text_transform_capitalize` - make the first char upper case.
- `text_transform_uppercase` - make all chars upper case.
- `text_transform_lowercase` - make all chars lower case.

### import_css

```cpp
virtual void import_css(std::string& text, const std::string& url, std::string& baseurl);
```

litehtml calls this function to load stylesheet. You have to download CSS file referred by **url** and **baseurl** parameters and copy content into **text** parameter.

### set_clip

```cpp
virtual void set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius);
```

Set the painting clip rectangle here. `bdr_radius` defines borders radius for rounded rectangles. Please note, litehtml can set some clip rects. You have to save the clip positions and apply clipping on draw something.

### del_clip

```cpp
virtual void del_clip();
```

Deletes the last clipping.

### get_viewport

```cpp
virtual void get_viewport(litehtml::position& viewport) const;
```

Fill the parameter **viewport** with the viewport position and size. Usually this is the size of the client rectangle of the window where you want to draw html.

### create_element

```cpp
virtual litehtml::element::ptr create_element( const char* tag_name, const litehtml::string_map& attributes, const std::shared_ptr<litehtml::document>& doc);
```

Using this function, you can process custom tags. Just make your own `litehtml::element` and return is from this function. Parameter `tag_name` is the HTML tag (a, p, table etc.).

### get_media_features

```cpp
virtual void get_media_features(litehtml::media_features& media)
```

Fill the `media` with the parameters of media where you render HTML. litehtml process stylesheets using the media features. Call `document::media_changed` function when any media feature is changed (for example user is changed the window size).

The definition of the `media_features` structure:

```cpp
struct media_features
{
    media_type    type;
    int            width;
    int            height;
    int            device_width;
    int            device_height;
    int            color;
    int            color_index;
    int            monochrome;
    int            resolution;
};
```

- `type` - the type of the media. See enum `media_type`.
- `width` - (pixels) For continuous media, this is the width of the viewport including the size of a rendered scroll bar (if any). For paged media, this is the width of the page box.
- `height` - (pixels) The height of the targeted display area of the output device. For continuous media, this is the height of the viewport including the size of a rendered scroll bar (if any). For paged media, this is the height of the page box.
- `device_width` - (pixels) The width of the rendering surface of the output device. For continuous media, this is the width of the screen. For paged media, this is the width of the page sheet size.
- `device_height` - (pixels) The height of the rendering surface of the output device. For continuous media, this is the height of the screen. For paged media, this is the height of the page sheet size.
- `color` - The number of bits per color component of the output device. If the device is not a color device, the value is zero.
- `color_index` - The number of entries in the color lookup table of the output device. If the device does not use a color lookup table, the value is zero.
- `monochrome` - The number of bits per pixel in a monochrome frame buffer. If the device is not a monochrome device, the output device value will be 0.
- `resolution` - The resolution of the output device (in DPI).

### get_language

```cpp
virtual void get_language(litehtml::string& language, litehtml::string& culture) const;
```

Fill the **language** and **culture** parameters with the language and culture of the user. This is used to process the `lang` attribute of the HTML tags.

### resolve_color

```cpp
virtual litehtml::string resolve_color(const litehtml::string& color) const;
```

Resolve the color name to the color value. For example, `red` will be resolved to `#FF0000`. If you don't need this, just return the empty string.

**Note**: The standard colors are resolved in the litehtml library. You can use the `resolve_color` function to resolve the custom colors.

### split_text

```cpp
virtual void split_text(const char* text, const std::function<void(const char*)>& on_word, const std::function<void(const char*)>& on_space);
```

This function is used to split the text into words and spaces. The `text` parameter is the text to split. The `on_word` and `on_space` parameters are the functions to call for each word and space respectively. The functions are called with the pointer to the word or space.

**Note**: The default implementation splits the text with this condition:

```cpp
if (c <= ' ' && (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f'))
```
