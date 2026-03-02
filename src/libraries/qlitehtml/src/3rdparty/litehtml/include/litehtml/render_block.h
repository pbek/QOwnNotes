#ifndef LITEHTML_RENDER_BLOCK_H
#define LITEHTML_RENDER_BLOCK_H

#include "render_item.h"

namespace litehtml
{
	class render_item_block : public render_item
	{
	  protected:
		/**
		 * Render block content.
		 *
		 * @param x - horizontal position of the content
		 * @param y - vertical position of the content
		 * @param second_pass - true is this is the second pass.
		 * @param ret_width - input minimal width.
		 * @param self_size - defines calculated size of block
		 * @return return value is the minimal width of the content in block. Must be greater or equal to ret_width
		 * parameter
		 */
		virtual pixel_t _render_content(pixel_t /*x*/, pixel_t /*y*/, bool /*second_pass*/,
										const containing_block_context& /*self_size*/, formatting_context* /*fmt_ctx*/)
		{
			return 0;
		}
		pixel_t		 _render(pixel_t x, pixel_t y, const containing_block_context& containing_block_size,
							 formatting_context* fmt_ctx, bool second_pass) override;
		pixel_t		 place_float(const std::shared_ptr<render_item>& el, pixel_t top,
								 const containing_block_context& self_size, formatting_context* fmt_ctx);
		virtual void fix_line_width(element_float /*flt*/, const containing_block_context& /*containing_block_size*/,
									formatting_context* /*fmt_ctx*/)
		{
		}

	  public:
		explicit render_item_block(std::shared_ptr<element> src_el) :
			render_item(std::move(src_el))
		{
		}

		std::shared_ptr<render_item> clone() override { return std::make_shared<render_item_block>(src_el()); }
		std::shared_ptr<render_item> init() override;
	};
} // namespace litehtml

#endif // LITEHTML_RENDER_BLOCK_H
