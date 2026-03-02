# Litehtml support sources

This folder contains source code that can help to integrate litehtml to the application.

## [gtkmm4] GTKMM4 Widget

This is implementation widget for GTK4 applications. Widget is written with GTKMM4 library.

This is the example how to add widget to the window:

```c++
// Declaration of the widget
html_widget m_html;

// Append widget to the vertical box.
m_vbox.append(m_html);
m_html.set_expand(true);

// Connect to signals
m_html.signal_set_address().connect( sigc::mem_fun(*this, &browser_window::set_address) );
m_html.signal_update_state().connect( sigc::mem_fun(*this, &browser_window::update_buttons) );
signal_close_request().connect(sigc::mem_fun(m_html, &html_widget::on_close), false);

.....

// Set url to the text
void browser_window::set_address(const std::string& text)
{
    m_address_bar.set_text(text);
}

// Update the state of buttons depending of the current state of the widget
void browser_window::update_buttons(uint32_t)
{
	uint32_t state = m_html.get_state();

	if((m_prev_state & page_state_has_back) != (state & page_state_has_back))
	{
		m_back_button.set_sensitive(state & page_state_has_back);
	}
	if((m_prev_state & page_state_has_forward) != (state & page_state_has_forward))
	{
		m_forward_button.set_sensitive(state & page_state_has_forward);
	}
	if((m_prev_state & page_state_downloading) != (state & page_state_downloading))
	{
		if (state & page_state_downloading)
		{
			m_stop_reload_button.set_image_from_icon_name("process-stop-symbolic");
		} else
		{
			m_stop_reload_button.set_image_from_icon_name("view-refresh-symbolic");
		}
	}
	m_prev_state = state;
}

```

To open url just call `m_html.open_url("http://domain.tld/index")`.

Please note, widget requires all file from **webpage** folder.

## [webpage] Webpage implementation

A set of classes to implement webpage handling:

- Using `container_cairo_pango` as a container
- Download files with libcurl library
- Multi-threading downloading

Please refer the gtkmm4 widget to find detailed information about using webpage classes.

### litebrowser::html_host_interface

The interface to interact with widget or browser window. You have to implement all methods of this class and pass pointer to the `litebrowser::web_page` constructor.

#### Methods

```c++
virtual void open_url(const std::string& url);
```

Method is called to open urls, for example when user click the anchor.

```c++
virtual void update_cursor();
```

Method is called when the mounse cursor was changed. Call `web_page::get_cursor()` to get the cursor name.

```c++
virtual void scroll_to(int x, int y);
```

Scroll page to the position [x, y]. Applied to jump to the url fragment (#fragment).

```c++
virtual void get_client_rect(litehtml::position& client) const;
```

Return the client rectangle of the widget. Usually this is the rectangle with widget width and height.

```c++
virtual void redraw_boxes(const litehtml::position::vector& boxes);
```

Redraw the boxes from `boxes` vector. This method is called to support changes on mouse move/click like `:hover`.

```c++
virtual int get_render_width();
```

Get width to render to. Usually it is the widget width.

```c++
virtual double get_dpi()
```

Get screen DPI. This value is used to calculate font size and some units like `em`. Application should return at least default value 96.

```c++
virtual int get_screen_width();
virtual int get_screen_height();
```

Get screen width and height. Used for media queries.

```c++
virtual cairo_surface_t* load_image(const std::string& path);
```

Load image from specified `path`.

### litebrowser::browser_notify_interface

This class is similar to the `litebrowser::html_host_interface`, but all methods should be dispatched to the GUI thread.

#### Methods

```c++
virtual void redraw();
```

Query widget redraw

```c++
virtual void render();
```

Query widget re-render

```c++
virtual void update_state();
```

Notify about webpage state change

```c++
virtual void on_set_caption(const std::string& caption_text);
```

Query change the window title

```c++
virtual void on_page_loaded(uint64_t web_page_id);
```

Notify about page was loaded and rendered

### litebroeser::web_page

A helper class for the web page. This class implements all required methods of the `container_cairo_pango` class. Also it implements `std::enable_shared_from_this` class;

#### Methods

```c++
web_page(html_host_interface* html_host, std::shared_ptr<browser_notify_interface> notify, int pool_size);
```

Web page class constructor:

- **html_host** - implementation of `html_host_interface` class.
- **notify** - shared pointer to the `browser_notify_interface` implementation.
- **pool_size** - number of threads to use for page loading.

```c++
uint64_t id() const;
```

Returns page identifier.

```c++
const std::string& get_html_source() const;
```

Returns source code of the page.

```c++
void open(const litehtml::string& url, const litehtml::string& hash);
```

Open url.

- **url** - page url
- **hash** - page #fragment

```c++
void on_mouse_over(int x, int y, int client_x, int client_y);
```

Call this function on mouse move.

- **x** - mouse pointer horizontal position in document coordinates.
- **y** - mouse pointer vertical position in document coordinates.
- **client_x** - mouse pointer horizontal position in widget coordinates.
- **client_y** - mouse pointer vertical position in widget coordinates.

```c++
void on_lbutton_down(int x, int y, int client_x, int client_y);
```

Call this function on left mouse button down.

- **x** - mouse pointer horizontal position in document coordinates.
- **y** - mouse pointer vertical position in document coordinates.
- **client_x** - mouse pointer horizontal position in widget coordinates.
- **client_y** - mouse pointer vertical position in widget coordinates.

```c++
void on_lbutton_up(int x, int y, int client_x, int client_y);
```

Call this function on left mouse button up.

- **x** - mouse pointer horizontal position in document coordinates.
- **y** - mouse pointer vertical position in document coordinates.
- **client_x** - mouse pointer horizontal position in widget coordinates.
- **client_y** - mouse pointer vertical position in widget coordinates.

```c++
const std::string& get_cursor() const;
```

Get mouse cursor. Refer to the [CSS cursor property](https://developer.mozilla.org/en-US/docs/Web/CSS/cursor)

```c++
int render(int max_width);
```

Render page to the specified width.

```c++
const std::string& url() const;
```

Get page url.

```c++
int width() const;
int height() const;
```

Get page width and height.

```c++
bool media_changed();
```

Apply changed media to the webpage. Returns `true` is re-render is required.

```c++
void stop_loading()
```

Stop downloading the page.

```c++
bool is_downloading()
```

Returns `true` if something is downloading.

### litebrowser::draw_buffer

A helper class to perform the draw operations into the cairo surface. The application draws everything to the buffer, then buffer are drawn on widget or window.

Some notes about scaling support:

- All functions accept scale independent coordinates and sizes. So don't apply scale to the coordinates and sizes.
- The size of used `cairo_surface_t` is always scaled. So if scale factor is **2.0** and buffer width is **600**, then surface width will **1200**.
- If the scale factor is not **1** then buffer `left` and `top` will be rounded to the integer coordinates.
- Always use `draw_buffer::get_left()` and `draw_buffer::get_top()` for functions like `web_page::on_mouse_over`.

#### Methods

```c++
int get_width() const;
int get_height() const;
```

Get width and height of the buffer.

```c++
int get_left() const;
int get_top() const;
```

Get left and top position of the buffer's (0, 0) coordinates. Note: buffer works like viewport. left and top of the buffer defines a viewport shift.

```c++
cairo_surface_t* get_cairo_surface() const { return m_draw_buffer; }
```

Get buffer cairo surface.

```c++
double get_scale_factor() const { return m_scale_factor; }
```

Get buffer scale factor

```c++
void set_scale_factor(std::shared_ptr<litebrowser::web_page> page, double scale)
```

Set the scale factor for the buffer. Default scale factor is 1.

```c++
void on_size_allocate(std::shared_ptr<litebrowser::web_page> page, int width, int height);
```

Call this function when widget size is changed. This function re-allocate new buffer, re-render and redraw the page.

```c++
void on_scroll(std::shared_ptr<litebrowser::web_page> page, int left, int top);
```

Scroll page to the specified position.

```c++
void redraw_area(std::shared_ptr<litebrowser::web_page> page, int x, int y, int width, int height);
```

Redraw specified area

```c++
void redraw(std::shared_ptr<litebrowser::web_page> page)
```

Redraw everything.
