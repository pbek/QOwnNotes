#ifndef LH_HTML_TAG_H
#define LH_HTML_TAG_H

#include "element.h"
#include "style.h"
#include "background.h"
#include "css_margins.h"
#include "borders.h"
#include "css_selector.h"
#include "stylesheet.h"
#include "box.h"
#include "table.h"

namespace litehtml
{
	struct line_context
	{
		int calculatedTop;
		int top;
		int left;
		int right;

		int width() const
		{
			return right - left;
		}
		void fix_top()
		{
			calculatedTop = top;
		}
	};

	class html_tag : public element
	{
		friend class elements_iterator;
		friend class el_table;
		friend class table_grid;
		friend class block_box;
		friend class line_box;
	public:
		typedef std::shared_ptr<litehtml::html_tag>	ptr;
	protected:
		box::vector				m_boxes;
		string_vector			m_class_values;
		tstring					m_tag;
		litehtml::style			m_style;
		string_map				m_attrs;
		vertical_align			m_vertical_align;
		text_align				m_text_align;
		style_display			m_display;
		list_style_type			m_list_style_type;
		list_style_position		m_list_style_position;
		white_space				m_white_space;
		element_float			m_float;
		element_clear			m_clear;
		floated_box::vector		m_floats_left;
		floated_box::vector		m_floats_right;
		elements_vector			m_positioned;
		background				m_bg;
		element_position		m_el_position;
		int						m_line_height;
		bool					m_lh_predefined;
		string_vector			m_pseudo_classes;
		used_selector::vector	m_used_styles;		
		
		uint_ptr				m_font;
		int						m_font_size;
		font_metrics			m_font_metrics;

		css_margins				m_css_margins;
		css_margins				m_css_padding;
		css_borders				m_css_borders;
		css_length				m_css_width;
		css_length				m_css_height;
		css_length				m_css_min_width;
		css_length				m_css_min_height;
		css_length				m_css_max_width;
		css_length				m_css_max_height;
		css_offsets				m_css_offsets;
		css_length				m_css_text_indent;

		overflow				m_overflow;
		visibility				m_visibility;
		int						m_z_index;
		box_sizing				m_box_sizing;

		int_int_cache			m_cahe_line_left;
		int_int_cache			m_cahe_line_right;

		// data for table rendering
		std::unique_ptr<table_grid>	m_grid;
		css_length				m_css_border_spacing_x;
		css_length				m_css_border_spacing_y;
		int						m_border_spacing_x;
		int						m_border_spacing_y;
		border_collapse			m_border_collapse;

		void			select_all(const css_selector& selector, elements_vector& res) override;

	public:
		explicit html_tag(const std::shared_ptr<litehtml::document>& doc);

		/* render functions */

		int					render(int x, int y, int max_width, bool second_pass = false) override;

		int					render_inline(const element::ptr &container, int max_width) override;
		int					place_element(const element::ptr &el, int max_width) override;
		bool				fetch_positioned() override;
		void				render_positioned(render_type rt = render_all) override;

		int					new_box(const element::ptr &el, int max_width, line_context& line_ctx);

		int					get_cleared_top(const element::ptr &el, int line_top) const;
		int					finish_last_box(bool end_of_render = false);

		bool				appendChild(const element::ptr &el) override;
		bool				removeChild(const element::ptr &el) override;
		void				clearRecursive() override;
		const tchar_t*		get_tagName() const override;
		void				set_tagName(const tchar_t* tag) override;
		void				set_data(const tchar_t* data) override;
		element_float		get_float() const override;
		vertical_align		get_vertical_align() const override;
		css_length			get_css_left() const override;
		css_length			get_css_right() const override;
		css_length			get_css_top() const override;
		css_length			get_css_bottom() const override;
		css_length			get_css_width() const override;
		css_offsets			get_css_offsets() const override;
		void				set_css_width(css_length& w) override;
		css_length			get_css_height() const override;
		element_clear		get_clear() const override;
		size_t				get_children_count() const override;
		element::ptr		get_child(int idx) const override;
		element_position	get_element_position(css_offsets* offsets = nullptr) const override;
		overflow			get_overflow() const override;

		void				set_attr(const tchar_t* name, const tchar_t* val) override;
		const tchar_t*		get_attr(const tchar_t* name, const tchar_t* def = nullptr) const override;
		void				apply_stylesheet(const litehtml::css& stylesheet) override;
		void				refresh_styles() override;

		bool				is_white_space() const override;
		bool				is_body() const override;
		bool				is_break() const override;
		int					get_base_line() override;
		bool				on_mouse_over() override;
		bool				on_mouse_leave() override;
		bool				on_lbutton_down() override;
		bool				on_lbutton_up() override;
		void				on_click() override;
		bool				find_styles_changes(position::vector& redraw_boxes, int x, int y) override;
		const tchar_t*		get_cursor() override;
		void				init_font() override;
		bool				set_pseudo_class(const tchar_t* pclass, bool add) override;
		bool				set_class(const tchar_t* pclass, bool add) override;
		bool				is_replaced() const override;
		int					line_height() const override;
		white_space			get_white_space() const override;
		style_display		get_display() const override;
		visibility			get_visibility() const override;
		void				parse_styles(bool is_reparse = false) override;
		void				draw(uint_ptr hdc, int x, int y, const position* clip) override;
		void				draw_background(uint_ptr hdc, int x, int y, const position* clip) override;

		const tchar_t*		get_style_property(const tchar_t* name, bool inherited, const tchar_t* def = nullptr) override;
		uint_ptr			get_font(font_metrics* fm = nullptr) override;
		int					get_font_size() const override;

		elements_vector&			children();
		void				calc_outlines(int parent_width) override;
		void				calc_auto_margins(int parent_width) override;

		int					select(const css_selector& selector, bool apply_pseudo = true) override;
		int					select(const css_element_selector& selector, bool apply_pseudo = true) override;

		elements_vector		select_all(const tstring& selector) override;
		elements_vector		select_all(const css_selector& selector) override;

		element::ptr		select_one(const tstring& selector) override;
		element::ptr		select_one(const css_selector& selector) override;

		element::ptr		find_ancestor(const css_selector& selector, bool apply_pseudo = true, bool* is_pseudo = nullptr) override;
		element::ptr		find_adjacent_sibling(const element::ptr& el, const css_selector& selector, bool apply_pseudo = true, bool* is_pseudo = nullptr) override;
		element::ptr		find_sibling(const element::ptr& el, const css_selector& selector, bool apply_pseudo = true, bool* is_pseudo = nullptr) override;
		void				get_text(tstring& text) override;
		void				parse_attributes() override;

		bool				is_first_child_inline(const element::ptr& el) const override;
		bool				is_last_child_inline(const element::ptr& el) override;
		bool				have_inline_child() const override;
		void				get_content_size(size& sz, int max_width) override;
		void				init() override;
		void				get_inline_boxes(position::vector& boxes) override;
		bool				is_floats_holder() const override;
		int					get_floats_height(element_float el_float = float_none) const override;
		int					get_left_floats_height() const override;
		int					get_right_floats_height() const override;
		int					get_line_left(int y) override;
		int					get_line_right(int y, int def_right) override;
		void				get_line_left_right(int y, int def_right, int& ln_left, int& ln_right) override;
		void				add_float(const element::ptr &el, int x, int y) override;
		void				update_floats(int dy, const element::ptr &parent) override;
		void				add_positioned(const element::ptr &el) override;
		int					find_next_line_top(int top, int width, int def_right) override;
		void				apply_vertical_align() override;
		void				draw_children(uint_ptr hdc, int x, int y, const position* clip, draw_flag flag, int zindex) override;
		int					get_zindex() const override;
		void				draw_stacking_context(uint_ptr hdc, int x, int y, const position* clip, bool with_positioned) override;
		void				calc_document_size(litehtml::size& sz, int x = 0, int y = 0) override;
		void				get_redraw_box(litehtml::position& pos, int x = 0, int y = 0) override;
		void				add_style(const litehtml::style& st) override;
		element::ptr		get_element_by_point(int x, int y, int client_x, int client_y) override;
		element::ptr		get_child_by_point(int x, int y, int client_x, int client_y, draw_flag flag, int zindex) override;

		bool				is_nth_child(const element::ptr& el, int num, int off, bool of_type) const override;
		bool				is_nth_last_child(const element::ptr& el, int num, int off, bool of_type) const override;
		bool				is_only_child(const element::ptr& el, bool of_type) const override;
		const background*	get_background(bool own_only = false) override;

	protected:
		void				draw_children_box(uint_ptr hdc, int x, int y, const position* clip, draw_flag flag, int zindex);
		void				draw_children_table(uint_ptr hdc, int x, int y, const position* clip, draw_flag flag, int zindex);
		int					render_box(int x, int y, int max_width, bool second_pass = false);
		int					render_table(int x, int y, int max_width, bool second_pass = false);
		int					fix_line_width(int max_width, element_float flt);
		void				parse_background();
		void				init_background_paint( position pos, background_paint &bg_paint, const background* bg );
		void				draw_list_marker( uint_ptr hdc, const position &pos );
		tstring				get_list_marker_text(int index);
		static void			parse_nth_child_params( const tstring& param, int &num, int &off );
		void				remove_before_after();
		litehtml::element::ptr  get_element_before();
		litehtml::element::ptr  get_element_after();
	};

	/************************************************************************/
	/*                        Inline Functions                              */
	/************************************************************************/

	inline elements_vector& litehtml::html_tag::children()
	{
		return m_children;
	}
}

#endif  // LH_HTML_TAG_H
