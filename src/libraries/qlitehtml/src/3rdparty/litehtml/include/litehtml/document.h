#ifndef LH_DOCUMENT_H
#define LH_DOCUMENT_H

#include "style.h"
#include "types.h"
#include "context.h"

namespace litehtml
{
	struct css_text
	{
		typedef std::vector<css_text>	vector;

		tstring	text;
		tstring	baseurl;
		tstring	media;
		
		css_text() = default;

		css_text(const tchar_t* txt, const tchar_t* url, const tchar_t* media_str)
		{
			text	= txt ? txt : _t("");
			baseurl	= url ? url : _t("");
			media	= media_str ? media_str : _t("");
		}

		css_text(const css_text& val)
		{
			text	= val.text;
			baseurl	= val.baseurl;
			media	= val.media;
		}
	};

	class html_tag;

	class document : public std::enable_shared_from_this<document>
	{
	public:
		typedef std::shared_ptr<document>	ptr;
		typedef std::weak_ptr<document>		weak_ptr;
	private:
		std::shared_ptr<element>			m_root;
		document_container*					m_container;
		fonts_map							m_fonts;
		css_text::vector					m_css;
		litehtml::css						m_styles;
		litehtml::web_color					m_def_color;
		litehtml::context*					m_context;
		litehtml::size						m_size;
		position::vector					m_fixed_boxes;
		media_query_list::vector			m_media_lists;
		element::ptr						m_over_element;
		elements_vector						m_tabular_elements;
		media_features						m_media;
		tstring                             m_lang;
		tstring                             m_culture;
	public:
		document(litehtml::document_container* objContainer, litehtml::context* ctx);
		virtual ~document();

		litehtml::document_container*	container()	{ return m_container; }
		uint_ptr						get_font(const tchar_t* name, int size, const tchar_t* weight, const tchar_t* style, const tchar_t* decoration, font_metrics* fm);
		int								render(int max_width, render_type rt = render_all);
		void							draw(uint_ptr hdc, int x, int y, const position* clip);
		web_color						get_def_color()	{ return m_def_color; }
		int								cvt_units(const tchar_t* str, int fontSize, bool* is_percent = nullptr) const;
		int								cvt_units(css_length& val, int fontSize, int size = 0) const;
		int								width() const;
		int								height() const;
		void							add_stylesheet(const tchar_t* str, const tchar_t* baseurl, const tchar_t* media);
		bool							on_mouse_over(int x, int y, int client_x, int client_y, position::vector& redraw_boxes);
		bool							on_lbutton_down(int x, int y, int client_x, int client_y, position::vector& redraw_boxes);
		bool							on_lbutton_up(int x, int y, int client_x, int client_y, position::vector& redraw_boxes);
		bool							on_mouse_leave(position::vector& redraw_boxes);
		litehtml::element::ptr			create_element(const tchar_t* tag_name, const string_map& attributes);
		element::ptr					root();
		void							get_fixed_boxes(position::vector& fixed_boxes);
		void							add_fixed_box(const position& pos);
		void							add_media_list(const media_query_list::ptr& list);
		bool							media_changed();
		bool							lang_changed();
		bool                            match_lang(const tstring & lang);
		void							add_tabular(const element::ptr& el);
		element::const_ptr		        get_over_element() const { return m_over_element; }

		void                            append_children_from_string(element& parent, const tchar_t* str);
		void                            append_children_from_utf8(element& parent, const char* str);

		static litehtml::document::ptr createFromString(const tchar_t* str, litehtml::document_container* objPainter, litehtml::context* ctx, litehtml::css* user_styles = nullptr);
		static litehtml::document::ptr createFromUTF8(const char* str, litehtml::document_container* objPainter, litehtml::context* ctx, litehtml::css* user_styles = nullptr);
	
	private:
		litehtml::uint_ptr	add_font(const tchar_t* name, int size, const tchar_t* weight, const tchar_t* style, const tchar_t* decoration, font_metrics* fm);

		void create_node(void* gnode, elements_vector& elements, bool parseTextNode);
		bool update_media_lists(const media_features& features);
		void fix_tables_layout();
		void fix_table_children(element::ptr& el_ptr, style_display disp, const tchar_t* disp_str);
		void fix_table_parent(element::ptr& el_ptr, style_display disp, const tchar_t* disp_str);
	};

	inline element::ptr document::root()
	{
		return m_root;
	}
	inline void document::add_tabular(const element::ptr& el)
	{
		m_tabular_elements.push_back(el);
	}
	inline bool document::match_lang(const tstring & lang)
	{
		return lang == m_lang || lang == m_culture;
	}
}

#endif  // LH_DOCUMENT_H
