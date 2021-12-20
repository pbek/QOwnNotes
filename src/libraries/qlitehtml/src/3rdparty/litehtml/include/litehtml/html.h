#ifndef LH_HTML_H
#define LH_HTML_H

#include <stdlib.h>
#include <string>
#include <ctype.h>
#include <vector>
#include <map>
#include <cstring>
#include <algorithm>
#include <sstream>
#include <functional>
#include "os_types.h"
#include "types.h"
#include "background.h"
#include "borders.h"
#include "html_tag.h"
#include "web_color.h"
#include "media_query.h"

namespace litehtml
{
	struct list_marker
	{
		tstring			image;
		const tchar_t*	baseurl;
		list_style_type	marker_type;
		web_color		color;
		position		pos;
		int				index;
		uint_ptr		font;
	};

	// call back interface to draw text, images and other elements
	class document_container
	{
	public:
		virtual litehtml::uint_ptr	create_font(const litehtml::tchar_t* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics* fm) = 0;
		virtual void				delete_font(litehtml::uint_ptr hFont) = 0;
		virtual int					text_width(const litehtml::tchar_t* text, litehtml::uint_ptr hFont) = 0;
		virtual void				draw_text(litehtml::uint_ptr hdc, const litehtml::tchar_t* text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos) = 0;
		virtual int					pt_to_px(int pt) const = 0;
		virtual int					get_default_font_size() const = 0;
		virtual const litehtml::tchar_t*	get_default_font_name() const = 0;
		virtual void				draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker) = 0;
		virtual void				load_image(const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, bool redraw_on_ready) = 0;
		virtual void				get_image_size(const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, litehtml::size& sz) = 0;
		virtual void				draw_background(litehtml::uint_ptr hdc, const litehtml::background_paint& bg) = 0;
		virtual void				draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root) = 0;

		virtual	void				set_caption(const litehtml::tchar_t* caption) = 0;
		virtual	void				set_base_url(const litehtml::tchar_t* base_url) = 0;
		virtual void				link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el) = 0;
		virtual void				on_anchor_click(const litehtml::tchar_t* url, const litehtml::element::ptr& el) = 0;
		virtual	void				set_cursor(const litehtml::tchar_t* cursor) = 0;
		virtual	void				transform_text(litehtml::tstring& text, litehtml::text_transform tt) = 0;
		virtual void				import_css(litehtml::tstring& text, const litehtml::tstring& url, litehtml::tstring& baseurl) = 0;
		virtual void				set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius, bool valid_x, bool valid_y) = 0;
		virtual void				del_clip() = 0;
		virtual void				get_client_rect(litehtml::position& client) const = 0;
		virtual std::shared_ptr<litehtml::element>	create_element(const litehtml::tchar_t *tag_name,
																	 const litehtml::string_map &attributes,
																	 const std::shared_ptr<litehtml::document> &doc) = 0;

		virtual void				get_media_features(litehtml::media_features& media) const = 0;
		virtual void				get_language(litehtml::tstring& language, litehtml::tstring & culture) const = 0;
		virtual litehtml::tstring resolve_color(const litehtml::tstring& /*color*/) const { return litehtml::tstring(); }
		virtual void				split_text(const char* text, const std::function<void(const tchar_t*)>& on_word, const std::function<void(const tchar_t*)>& on_space);

	protected:
		~document_container() = default;
	};

	void trim(tstring &s);
	void lcase(tstring &s);
	int	 value_index(const tstring& val, const tstring& strings, int defValue = -1, tchar_t delim = _t(';'));
	bool value_in_list(const tstring& val, const tstring& strings, tchar_t delim = _t(';'));
	tstring::size_type find_close_bracket(const tstring &s, tstring::size_type off, tchar_t open_b = _t('('), tchar_t close_b = _t(')'));
	void split_string(const tstring& str, string_vector& tokens, const tstring& delims, const tstring& delims_preserve = _t(""), const tstring& quote = _t("\""));
	void join_string(tstring& str, const string_vector& tokens, const tstring& delims);
    double t_strtod(const tchar_t* string, tchar_t** endPtr);

	int t_strcasecmp(const tchar_t *s1, const tchar_t *s2);
	int t_strncasecmp(const tchar_t *s1, const tchar_t *s2, size_t n);
	
	inline int t_isdigit(int c)
	{
		return (c >= '0' && c <= '9');
	}
	
	inline int t_tolower(int c)
	{
		return (c >= 'A' && c <= 'Z' ? c + 'a' - 'A' : c);
	}
	
	inline int round_f(float val)
	{
		int int_val = (int) val;
		if(val - int_val >= 0.5)
		{
			int_val++;
		}
		return int_val;
	}

	inline int round_d(double val)
	{
		int int_val = (int) val;
		if(val - int_val >= 0.5)
		{
			int_val++;
		}
		return int_val;
	}
}

#endif  // LH_HTML_H
