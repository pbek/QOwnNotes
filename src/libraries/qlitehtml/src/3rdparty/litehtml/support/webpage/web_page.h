#ifndef LITEBROWSER_WEB_PAGE_H
#define LITEBROWSER_WEB_PAGE_H

#include <memory>
#include <unistd.h>
#include <sstream>
#include <vector>
#include "container_cairo_pango.h"
#include "html_host.h"
#include "http_requests_pool.h"
#include "cairo_images_cache.h"
#include "litehtml/types.h"

namespace litebrowser
{
	class text_file
	{
		std::mutex		  wait_mutex;
		std::stringstream stream;
		bool			  data_ready = false;

	  public:
		text_file() { wait_mutex.lock(); }

		void set_ready() { data_ready = true; }

		std::string str() const { return data_ready ? stream.str() : ""; }
		void		wait() { wait_mutex.lock(); }
		void		on_data(void* data, size_t len, size_t downloaded, size_t total);
		void		on_page_downloaded(u_int32_t http_status, u_int32_t err_code, const std::string& err_text);
	};

	class image_file
	{
		int			m_fd = -1;
		std::string m_path;
		std::string m_url;
		bool		m_redraw_on_ready;

	  public:
		explicit image_file(std::string url, bool redraw_on_ready);
		void on_data(void* data, size_t len, size_t downloaded, size_t total);
		void close() const
		{
			if(m_fd > 0)
			{
				::close(m_fd);
			}
		}
		[[nodiscard]]
		const std::string& path() const
		{
			return m_path;
		}

		[[nodiscard]]
		const std::string& url() const
		{
			return m_url;
		}

		[[nodiscard]]
		bool redraw_only() const
		{
			return m_redraw_on_ready;
		}
	};

	class web_page : public container_cairo_pango, public std::enable_shared_from_this<web_page>
	{
		litehtml::string				m_url;
		litehtml::string				m_base_url;
		litehtml::document::ptr			m_html;
		mutable std::recursive_mutex	m_html_mutex;
		litehtml::string				m_cursor;
		litehtml::string				m_clicked_url;
		std::string						m_fragment;
		html_host_interface*			m_html_host;
		cairo_images_cache				m_images;
		litebrowser::http_requests_pool m_requests_pool;
		std::string						m_html_source;

		std::shared_ptr<browser_notify_interface> m_notify;

	  public:
		explicit web_page(html_host_interface* html_host, std::shared_ptr<browser_notify_interface> notify,
						  int pool_size) :
			m_html_host(html_host),
			m_requests_pool(pool_size, [this] { on_pool_update_state(); }),
			m_notify(std::move(notify))
		{
		}

		[[nodiscard]]
		uint64_t id() const
		{
			return (uint64_t) this;
		}

		[[nodiscard]]
		const std::string& get_html_source() const
		{
			return m_html_source;
		}

		void open(const litehtml::string& url, const litehtml::string& fragment);

		void get_viewport(litehtml::position& viewport) const override;
		void on_anchor_click(const char* url, const litehtml::element::ptr& el) override;
		void set_cursor(const char* cursor) override;
		void import_css(litehtml::string& text, const litehtml::string& url, litehtml::string& baseurl) override;
		void set_caption(const char* caption) override;
		void set_base_url(const char* base_url) override;
		cairo_surface_t* get_image(const std::string& url) override;
		void			 make_url(const char* url, const char* basepath, litehtml::string& out) override;
		void			 load_image(const char* src, const char* baseurl, bool redraw_on_ready) override;
		void			 on_mouse_event(const litehtml::element::ptr& el, litehtml::mouse_event event) override;
		double			 get_screen_dpi() const override;
		int				 get_screen_width() const override;
		int				 get_screen_height() const override;

		void show_fragment(const litehtml::string& fragment);
		void show_fragment_and_reset()
		{
			if(!m_fragment.empty() && m_html)
			{
				show_fragment(m_fragment);
				m_fragment = "";
			}
		}

		void on_mouse_over(int x, int y, int client_x, int client_y);
		void on_lbutton_down(int x, int y, int client_x, int client_y);
		void on_lbutton_up(int x, int y, int client_x, int client_y);

		[[nodiscard]]
		const std::string& get_cursor() const
		{
			return m_cursor;
		}

		void draw(litehtml::uint_ptr hdc, int x, int y, const litehtml::position* clip)
		{
			std::lock_guard<std::recursive_mutex> html_lock(m_html_mutex);
			if(m_html)
				m_html->draw(hdc, x, y, clip);
		}

		int render(int max_width)
		{
			std::lock_guard<std::recursive_mutex> html_lock(m_html_mutex);
			return m_html ? m_html->render(max_width) : 0;
		}

		[[nodiscard]]
		const std::string& url() const
		{
			return m_url;
		}

		[[nodiscard]]
		int width() const
		{
			std::lock_guard<std::recursive_mutex> html_lock(m_html_mutex);
			return m_html ? m_html->width() : 0;
		}

		[[nodiscard]]
		int height() const
		{
			std::lock_guard<std::recursive_mutex> html_lock(m_html_mutex);
			return m_html ? m_html->height() : 0;
		}

		bool media_changed()
		{
			std::lock_guard<std::recursive_mutex> html_lock(m_html_mutex);
			return m_html && m_html->media_changed();
		}

		litehtml::position::vector get_fixed_boxes()
		{
			litehtml::position::vector			  ret;
			std::lock_guard<std::recursive_mutex> html_lock(m_html_mutex);
			if(m_html)
			{
				m_html->get_fixed_boxes(ret);
			}
			return ret;
		}

		void stop_loading() { m_requests_pool.stop(); }

		[[nodiscard]]
		bool is_downloading()
		{
			return m_requests_pool.is_downloading();
		}

		std::vector<litehtml::scroll_values> on_scroll(litehtml::pixel_t dx, litehtml::pixel_t dy, int x, int y,
													   int client_x, int client_y)
		{
			std::lock_guard<std::recursive_mutex> html_lock(m_html_mutex);
			return m_html ? m_html->on_scroll(dx, dy, x, y, client_x, client_y)
						  : std::vector<litehtml::scroll_values>();
		}

		void run_with_document(const std::function<void(const std::shared_ptr<litehtml::document>)>& func)
		{
			std::lock_guard<std::recursive_mutex> html_lock(m_html_mutex);
			if(m_html)
				func(m_html);
		}

	  private:
		void http_request(
			const std::string&																	url,
			const std::function<void(void* data, size_t len, size_t downloaded, size_t total)>& cb_on_data,
			const std::function<void(u_int32_t http_status, u_int32_t err_code, const std::string& err_text,
									 const std::string& url)>&									cb_on_finish);
		void on_page_downloaded(std::shared_ptr<text_file> data, u_int32_t http_status, u_int32_t err_code,
								const std::string& err_text, const std::string& url);
		void on_image_downloaded(std::shared_ptr<image_file> data, u_int32_t http_status, u_int32_t err_code,
								 const std::string& err_text, const std::string& url);
		void on_pool_update_state();
	};
} // namespace litebrowser

#endif // LITEBROWSER_WEB_PAGE_H
