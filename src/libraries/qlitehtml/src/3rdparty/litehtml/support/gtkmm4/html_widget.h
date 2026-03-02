#pragma once

#include <gtkmm.h>
#include "html_host.h"
#include "web_page.h"
#include "web_history.h"
#include "../draw_buffer/draw_buffer.h"
#include <queue>

enum page_state
{
	page_state_has_back	   = 0x01,
	page_state_has_forward = 0x02,
	page_state_downloading = 0x04,
};

/// @brief Allows perform actions in the GUI thread via notifications
class html_widget_notifier : public litebrowser::browser_notify_interface
{
  public:
	using redraw_func		  = std::function<void()>;
	using render_func		  = std::function<void()>;
	using update_state_func	  = std::function<void()>;
	using on_page_loaded_func = std::function<void(uint64_t)>;
	using on_set_caption_func = std::function<void(const std::string&)>;

  private:
	enum func_type
	{
		func_type_none,
		func_type_redraw,
		func_type_render,
		func_type_update_state,
		func_type_on_page_loaded,
		func_type_on_set_caption
	};
	struct queue_item
	{
		func_type	type;
		uint64_t	param;
		std::string str_param;
	};

	Glib::Dispatcher	m_dispatcher;
	redraw_func			m_redraw_func;
	render_func			m_render_func;
	update_state_func	m_update_state_func;
	on_page_loaded_func m_on_page_loaded_func;
	on_set_caption_func m_on_set_caption_func;

	std::mutex			   m_lock;
	bool				   m_disconnect = false;
	std::queue<queue_item> m_queue;

  public:
	html_widget_notifier() { m_dispatcher.connect(sigc::mem_fun(*this, &html_widget_notifier::on_notify)); }

	void disconnect()
	{
		std::lock_guard lock(m_lock);
		m_disconnect = true;
	}

	void connect_redraw(redraw_func _redraw_func) { m_redraw_func = _redraw_func; }

	void connect_render(render_func _render_func) { m_render_func = _render_func; }

	void connect_update_state(update_state_func _update_state_func) { m_update_state_func = _update_state_func; }

	void connect_on_page_loaded(on_page_loaded_func _on_page_loaded_func)
	{
		m_on_page_loaded_func = _on_page_loaded_func;
	}

	void connect_on_set_caption(on_set_caption_func _on_set_caption_func)
	{
		m_on_set_caption_func = _on_set_caption_func;
	}

  private:
	void redraw() override
	{
		{
			std::lock_guard lock(m_lock);
			m_queue.push(queue_item{func_type_redraw, 0, {}});
		}
		m_dispatcher.emit();
	}

	void render() override
	{
		{
			std::lock_guard lock(m_lock);
			m_queue.push(queue_item{func_type_render, 0, {}});
		}
		m_dispatcher.emit();
	}

	void update_state() override
	{
		{
			std::lock_guard lock(m_lock);
			m_queue.push(queue_item{func_type_update_state, 0, {}});
		}
		m_dispatcher.emit();
	}

	void on_page_loaded(uint64_t web_page_id) override
	{
		{
			std::lock_guard lock(m_lock);
			m_queue.push(queue_item{func_type_on_page_loaded, web_page_id, {}});
		}
		m_dispatcher.emit();
	}

	void on_set_caption(const std::string& caption) override
	{
		{
			std::lock_guard lock(m_lock);
			m_queue.push(queue_item{func_type_on_set_caption, 0, caption});
		}
		m_dispatcher.emit();
	}

	void on_notify()
	{
		std::queue<queue_item> local_queue;
		{
			std::lock_guard lock(m_lock);
			if(m_disconnect || m_queue.empty())
				return;

			while(!m_queue.empty())
			{
				local_queue.push(m_queue.front());
				m_queue.pop();
			}
		}
		func_type prev_type = func_type_none;
		while(!local_queue.empty())
		{
			const auto& item = local_queue.front();
			// We don't need do the same action some times
			if(item.type != prev_type)
			{
				prev_type = item.type;
				switch(item.type)
				{
				case func_type_redraw:
					if(m_redraw_func)
					{
						m_redraw_func();
					}
					break;
				case func_type_render:
					if(m_render_func)
					{
						m_render_func();
					}
					break;
				case func_type_update_state:
					if(m_update_state_func)
					{
						m_update_state_func();
					}
					break;
				case func_type_on_page_loaded:
					if(m_on_page_loaded_func)
					{
						m_on_page_loaded_func(item.param);
					}
					break;
				case func_type_on_set_caption:
					if(m_on_set_caption_func)
					{
						m_on_set_caption_func(item.str_param);
					}
					break;

				default:
					break;
				}
			}
			local_queue.pop();
		}
	}
};

class html_widget : public Gtk::Widget, public litebrowser::html_host_interface
{
	int									   m_rendered_width				   = 0;
	int									   m_rendered_height			   = 0;
	double								   m_mouse_x					   = 0;
	double								   m_mouse_y					   = 0;
	bool								   m_do_force_redraw_on_adjustment = true;
	std::mutex							   m_page_mutex;
	std::shared_ptr<litebrowser::web_page> m_current_page;
	std::shared_ptr<litebrowser::web_page> m_next_page;
	std::shared_ptr<html_widget_notifier>  m_notifier;
	web_history							   m_history;

	Gtk::Scrollbar*				  m_vscrollbar;
	Gtk::Scrollbar*				  m_hscrollbar;
	Glib::RefPtr<Gtk::Adjustment> m_vadjustment;
	Glib::RefPtr<Gtk::Adjustment> m_hadjustment;

	sigc::connection m_scrollbar_timer;

	litebrowser::draw_buffer m_draw_buffer;

  public:
	explicit html_widget();
	~html_widget() override;

	void	 on_page_loaded(uint64_t web_page_id);
	void	 render();
	void	 go_forward();
	void	 go_back();
	uint32_t get_state();
	void	 stop_download();
	void	 reload();

	std::string get_html_source();
	void		show_fragment(const std::string& fragment);
	bool		on_close();
	void		open_url(const std::string& url) override;

	void run_with_document(const std::function<void(const std::shared_ptr<litehtml::document>)>& func)
	{
		auto page = current_page();
		if(page)
		{
			page->run_with_document(func);
		}
	}

	// litebrowser::html_host_interface override
	double			 get_dpi() override;
	int				 get_screen_width() override;
	int				 get_screen_height() override;
	void			 open_page(const litehtml::string& url, const litehtml::string& fragment);
	void			 update_cursor() override;
	void			 redraw_boxes(const litehtml::position::vector& boxes) override;
	int				 get_render_width() override;
	void			 scroll_to(int x, int y) override;
	void			 get_viewport(litehtml::position& viewport) const override;
	cairo_surface_t* load_image(const std::string& path) override;

  protected:
	void snapshot_vfunc(const Glib::RefPtr<Gtk::Snapshot>& snapshot) override;
	void on_redraw();

	void on_button_press_event(int n_press, double x, double y);
	void on_button_release_event(int n_press, double x, double y);
	void on_mouse_move(double x, double y);
	bool on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);

	void size_allocate_vfunc(int width, int height, int baseline) override;
	void allocate_scrollbars(int width, int height);

	void set_caption(const std::string& caption);
	void on_vadjustment_changed();
	void on_hadjustment_changed();
	void on_adjustments_changed();
	bool on_scroll(double dx, double dy);
	bool on_scrollbar_timeout();
	void on_realize() override;

  private:
	std::shared_ptr<litebrowser::web_page> current_page()
	{
		std::lock_guard<std::mutex> lock(m_page_mutex);
		return m_current_page;
	}

	void update_view_port(std::shared_ptr<litebrowser::web_page> page);
	void restart_scrollbar_timer();
	void force_redraw()
	{
		queue_draw();
		while(g_main_context_iteration(nullptr, false)) {}
	}
	litebrowser::draw_buffer::draw_page_function_t get_draw_function(const std::shared_ptr<litebrowser::web_page>& page)
	{
		return [page](cairo_t* cr, int x, int y, const litehtml::position* clip) {
			if(page)
			{
				page->draw((litehtml::uint_ptr) cr, x, y, clip);
			}
		};
	}

  public:
	// Signals types
	using sig_set_address_t	 = sigc::signal<void(std::string)>;
	using sig_update_state_t = sigc::signal<void(uint32_t)>;

	sig_set_address_t  signal_set_address() { return m_sig_set_address; }
	sig_update_state_t signal_update_state() { return m_sig_update_state; }

  private:
	sig_update_state_t m_sig_update_state;
	sig_set_address_t  m_sig_set_address;
};
