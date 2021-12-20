#ifndef LH_EL_TEXT_H
#define LH_EL_TEXT_H

#include "html_tag.h"

namespace litehtml
{
	class el_text : public element
	{
	protected:
		tstring			m_text;
		tstring			m_transformed_text;
		size			m_size;
		text_transform	m_text_transform;
		bool			m_use_transformed;
		bool			m_draw_spaces;
	public:
		el_text(const tchar_t* text, const std::shared_ptr<litehtml::document>& doc);

		void				get_text(tstring& text) override;
		const tchar_t*		get_style_property(const tchar_t* name, bool inherited, const tchar_t* def = nullptr) override;
		void				parse_styles(bool is_reparse) override;
		int					get_base_line() override;
		void				draw(uint_ptr hdc, int x, int y, const position* clip) override;
		int					line_height() const override;
		uint_ptr			get_font(font_metrics* fm = nullptr) override;
		style_display		get_display() const override;
		white_space			get_white_space() const override;
		element_position	get_element_position(css_offsets* offsets = nullptr) const override;
		css_offsets			get_css_offsets() const override;

	protected:
		void				get_content_size(size& sz, int max_width) override;
	};
}

#endif  // LH_EL_TEXT_H
