#ifndef LITEHTML_RENDER_INLINE_H
#define LITEHTML_RENDER_INLINE_H

#include "render_item.h"
#include "types.h"

namespace litehtml
{
	class render_item_inline : public render_item
	{
	  protected:
		position::vector m_boxes;

	  public:
		explicit render_item_inline(std::shared_ptr<element> src_el) :
			render_item(std::move(src_el))
		{
		}

		void	get_inline_boxes(position::vector& boxes) const override { boxes = m_boxes; }
		void	set_inline_boxes(position::vector& boxes) override { m_boxes = boxes; }
		void	add_inline_box(const position& box) override { m_boxes.emplace_back(box); };
		void	clear_inline_boxes() override { m_boxes.clear(); }
		pixel_t get_first_baseline() override
		{
			return src_el()->css().get_font_metrics().height - src_el()->css().get_font_metrics().base_line();
		}
		pixel_t get_last_baseline() override
		{
			return src_el()->css().get_font_metrics().height - src_el()->css().get_font_metrics().base_line();
		}

		std::shared_ptr<render_item> clone() override { return std::make_shared<render_item_inline>(src_el()); }
		virtual void				 y_shift(pixel_t shift) override
		{
			if(css().get_display() == display_inline_text)
			{
				render_item::y_shift(shift);
			}
			for(auto& box : m_boxes)
			{
				box.y += shift;
			}
		}
	};
} // namespace litehtml

#endif // LITEHTML_RENDER_INLINE_H
