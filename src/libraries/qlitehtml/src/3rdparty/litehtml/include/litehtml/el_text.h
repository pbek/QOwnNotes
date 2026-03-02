#ifndef LH_EL_TEXT_H
#define LH_EL_TEXT_H

#include "element.h"
#include "document.h"

namespace litehtml
{
	class el_text : public element
	{
	  protected:
		string m_text;
		string m_transformed_text;
		size   m_size;
		bool   m_use_transformed;
		bool   m_draw_spaces;

	  public:
		el_text(const char* text, const document::ptr& doc);

		void get_text(string& text) const override;
		void compute_styles(bool recursive) override;
		bool is_text() const override { return true; }

		void									draw(uint_ptr hdc, pixel_t x, pixel_t y, const position* clip,
													 const std::shared_ptr<render_item>& ri) override;
		string									dump_get_name() override;
		std::vector<std::tuple<string, string>> dump_get_attrs() override;

	  protected:
		void get_content_size(size& sz, pixel_t max_width) override;
	};
} // namespace litehtml

#endif // LH_EL_TEXT_H
