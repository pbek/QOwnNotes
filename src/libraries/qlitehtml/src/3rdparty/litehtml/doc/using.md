# How to use litehtml

To use litehtml, you need some easy steps:

1. Implement [litehtml::document_container](document_container.md) class.
2. Call some functions from **litehtml::document** to parse, render and draw HTML text.

## Implementing litehtml::document_container

The first step to integrate litehtml into your application is implementing abstract class [litehtml::document_container](document_container.md). This class draws the HTML elements and performs some actions depended on the platform.

## Loading HTML page

To load HTML page you have to call **litehtml::document::createFromString** function. This function creates the litehtml::document object from the HTML string. The HTML string must be in UTF-8 encoding. The function has the following signature:

```cpp
static document::ptr  document::createFromString(
	const estring&       str,
	document_container*  container,
	const string&        master_styles = litehtml::master_css,
	const string&        user_styles = "");
```

Please refer to the [document::createFromString](document_createFromString.md) document for more details.

**createFromString** returns the `litehtml::document` pointer. Call `litehtml::document::render(max_width)` to render HTML elements:

```cpp
m_doc->render(max_width);
```

The parameter `max_width` usually the browser window width. Also `render` returns the optimal width for HTML text. You can use the returned value to render elements into the optimal width. This can be useful for tooltips.

Use `height` and `width` `litehtml::document` functions to find the width and height of the rendered document:

```cpp
m_doc->height()
m_doc->width()
```

Now call **draw** function:

```cpp
m_doc->draw(hdc, m_left, m_top, &clip);
```

draw accepts 4 parameters:

- `uint_ptr hdc` - usually device context or something where to draw. For example HDC in windows. This parameter will be passed into all draw functions of [litehtml::document_container](document_container.md).
- `int x, int y` - position where to draw HTML.
- `const position* clip*` - clipping area. litehtml does not draw the elements that don't intersect the clipping rectangle. But note, you have to set your own clipping if you want to clip HTML content.

That's all! Your HTML page is painted!

## Fit to the document

If you don't have the fixed size window to draw HTML, you need to get the HTML _best_ width. It is straightforward: `document::render` returns the best width for you.

So the example looks like this:

```cpp
pixel_t best_width = m_doc->render(max_width);
if(best_width < max_width)
{
	m_doc->render(best_width);
}
```

First, you render document with the maximum possible width and `document::render` returns the best width for HTML. If the best width is less then maximum width, you have to render your document with the returned _best width_. Now you can use `document::width` to get the actual document width.

You can use this technique to show HTML tooltips in your application or to create the HTML widgets.

## Scrolling and Clipping

Scrolling of html document is released via x,y parameters of the document::draw function. For example, if you want to scroll document on 50px horizontally and 100px vertically:

```cpp
m_doc->draw(hdc, -50, -100, litehtml::position(50, 100, window_width, window_height));
```

Note, the `x` and `y` parameters are screen relative. The `clip` parameter is always document relative. Also note, defining the `clip` parameter in `draw` does not guarantee the valid clipping. Some elements can be drawn out off the clipping area. litehtml just checks if the element's bounding rectangle is intersected with clipping rectangle to draw elements. So you must implement your own clipping.

## Scrolling of the internal elements

The content of a block can be scrolled when element has CSS property `overflow: auto` or `overflow: scroll`. To make this possible call `document::on_scroll` function:

```cpp
std::vector<scroll_values> on_scroll(pixel_t dx, pixel_t dy, pixel_t x, pixel_t y, pixel_t client_x, pixel_t client_y) const;
```

Arguments:

- **dx, dy** - number of pixels to scroll horizontally and/or vertically
- **x, y** - cursor position in the document coordinates
- **client_x, client_y** - cursor position in the window/view port coordinates (for fixed elements)

If something has been scrolled the function returns non-empty array of `scroll_values`. The structure `scroll_values` defined as:

```cpp
struct scroll_values
{
    pixel_t	dx;
    pixel_t dy;
    position scroll_box;
};
```

Members:

- **dx, dy** - number of pixels scrolled horizontally and/or vertically. Note, these values can differ from arguments with the same names.
- **scroll_box** - position of the box that has been scrolled.

## Handling the mouse

If you want to handle the mouse you have call some functions from `litehtml::document`:

```cpp
bool litehtml::document::on_mouse_over( pixel_t x, pixel_t y, pixel_t client_x, pixel_t client_y, position::vector& redraw_boxes );
bool litehtml::document::on_mouse_leave( position::vector& redraw_boxes );
bool litehtml::document::on_lbutton_down( pixel_t x, pixel_t y, pixel_t client_x, pixel_t client_y, position::vector& redraw_boxes );
bool litehtml::document::on_lbutton_up( pixel_t x, pixel_t y, pixel_t client_x, pixel_t client_y, position::vector& redraw_boxes );
bool on_mouse_leave(position::vector& redraw_boxes);
```

All functions returns the `bool` to indicate that you have to redraw the rectangles from _redraw_boxes_ vector. Also note the `x` and `y` are relative to the HTML layout. So `0,0` is the top-left corner.
The parameters `client_x` and `client_y` are the mouse position in the client area (draw area). These parameters are used to handle the elements with **fixed** position.

## Processing anchor click

If you process the mouse, the clicking on anchor tag will call the function [document_container::on_anchor_click](document_container.md#on_anchor_click). This function gets the url of the anchor as parameter and pointer to litehtml::element. You can open the new document at this point.

**NOTE:** Don't delete the document inside [document_container::on_anchor_click](document_container.md#on_anchor_click), this will cause the crash. The [document_container::on_anchor_click](document_container.md#on_anchor_click) is generated by `document::on_lbutton_up`, so it is safe to delete old document and show new one after `document::on_lbutton_up` is finished.

## Processing named anchors

litehtml does not have the special handling of the named anchors. But it is easy to process the named anchors manually. You get the url with the anchor name into [document_container::on_anchor_click](document_container.md#on_anchor_click). Find the hash char (\#) and extract the name. In the example below, the variable `anchor_name` contains the anchor name:

```cpp
if(anchor_name.empty())
{
    std::string selector = "#" + anchor_name;
    litehtml::element::ptr el = m_html->root()->select_one(selector);
    if (!el)
    {
        selector = "[name=" + anchor_name + "]";
        el = m_html->root()->select_one(selector);
    }
    if (el)
    {
        litehtml::position pos = el->get_placement();
        m_html_host->scroll_to(0, pos.top());
    }
}
```

The function `element::select_one` returns the first element for the CSS selector. So `select_one("#name")` returns the element with attribute `id="name"`. `el->get_placement()` returns the absolute position of the element (document relative coordinates).

So, you have the position of the named anchor and now you can scroll your document into this position.

## Images

litehtml does not cache the images. So if you want to increase the performance, you have to create your own images cache. Also, you can implement the delayed images loading. Start image downloading in [document_container::load_image](document_container.md#load_image) function. If the image is not loaded return fake sizes and draw placeholder. When the image is loaded call `litehtml::document::render`, and redraw the page.

## Support for CSS media features

litehtml supports CSS `@media` at-rule as well are `media` attribute in the `<link>` and `<style>` html tags. To make CSS media support you need:

1. Implement [document_container::get_media_features](document_container.md#get_media_features) function and fill the `media` parameter with valid media features (like width, height etc.).
2. Call `document::media_changed` function when any media feature is changed (for example user is changed the window size).
