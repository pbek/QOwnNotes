#include "render_block_context.h"
#include "document.h"
#include "types.h"

litehtml::pixel_t litehtml::render_item_block_context::_render_content(pixel_t /*x*/, pixel_t /*y*/, bool second_pass,
																	   const containing_block_context& self_size,
																	   formatting_context*			   fmt_ctx)
{
	element_position el_position;

	pixel_t						 ret_width	 = 0;
	pixel_t						 child_top	 = 0;
	pixel_t						 last_margin = 0;
	std::shared_ptr<render_item> last_margin_el;
	bool						 is_first = true;
	for(const auto& el : m_children)
	{
		// we don't need to process absolute and fixed positioned element on the second pass
		if(second_pass)
		{
			el_position = el->src_el()->css().get_position();
			if((el_position == element_position_absolute || el_position == element_position_fixed))
				continue;
		}

		if(el->src_el()->css().get_float() != float_none)
		{
			pixel_t rw = place_float(el, child_top, self_size, fmt_ctx);
			if(rw > ret_width)
			{
				ret_width = rw;
			}
		} else if(el->src_el()->css().get_display() != display_none)
		{
			if(el->src_el()->css().get_position() == element_position_absolute ||
			   el->src_el()->css().get_position() == element_position_fixed)
			{
				pixel_t min_rendered_width = el->render(0, child_top, self_size, fmt_ctx);
				if(min_rendered_width < el->width() && el->src_el()->css().get_width().is_predefined())
				{
					el->render(0, child_top, self_size.new_width(min_rendered_width), fmt_ctx);
				}
			} else
			{
				child_top			= fmt_ctx->get_cleared_top(el, child_top);
				pixel_t child_x		= 0;
				pixel_t child_width = self_size.render_width;
				pixel_t line_right	= self_size.render_width;
				pixel_t top_margin	= m_margins.top;

				el->calc_outlines(self_size.width);

				// Adjust child width for tables and replaced elements with floting blocks width
				if(el->src_el()->is_replaced() || el->src_el()->is_block_formatting_context() ||
				   el->src_el()->css().get_display() == display_table)
				{
					pixel_t line_left = 0;
					fmt_ctx->get_line_left_right(child_top, child_width, line_left, line_right);
					if(line_left != child_x)
					{
						child_x = line_left - el->margin_left();
					}
					if(line_right != self_size.render_width)
					{
						line_right += el->margin_right();
					}
					if(el->css().get_width().is_predefined())
					{
						child_width = line_right - line_left;
					}
				}

				// Collapse top margin
				if(is_first && collapse_top_margin())
				{
					if(el->get_margins().top > 0)
					{
						child_top -= el->get_margins().top;
						if(el->get_margins().top > get_margins().top)
						{
							top_margin = el->get_margins().top;
						}
					}
				} else
				{
					if(el->get_margins().top > 0)
					{
						if(last_margin > el->get_margins().top)
						{
							child_top -= el->get_margins().top;
						} else
						{
							child_top -= last_margin;
						}
					}
				}

				pixel_t rw = el->render(child_x, child_top, self_size.new_width(child_width), fmt_ctx);
				// Render table with "width: auto" into returned width
				if(el->src_el()->css().get_display() == display_table && rw < child_width &&
				   el->src_el()->css().get_width().is_predefined())
				{
					el->render(child_x, child_top, self_size.new_width(rw), fmt_ctx);
				}

				// Check if we need to move block to the next line
				if(el->src_el()->is_replaced() || el->src_el()->is_block_formatting_context() ||
				   el->src_el()->css().get_display() == display_table)
				{
					if(el->right() > line_right)
					{
						pixel_t ln_left	 = 0;
						pixel_t ln_right = el->width();
						pixel_t new_top	 = fmt_ctx->find_next_line_top(child_top, el->width(), ln_right);
						// If block was moved to the next line, recalculate its position
						if(new_top != child_top)
						{
							child_top = new_top;
							fmt_ctx->get_line_left_right(child_top, el->width(), ln_left, ln_right);
							el->pos().x	 = ln_left + el->content_offset_left();
							el->pos().y	 = child_top + el->border_top() + el->padding_top();
							child_top	-= el->get_margins().top;
							// Rollback top margin collapse
							if(is_first && collapse_top_margin())
							{
								top_margin = m_margins.top;
							}
						}
					}
				}

				pixel_t auto_margin = el->calc_auto_margins(child_width);
				if(auto_margin != 0)
				{
					el->pos().x += auto_margin;
				}
				if(rw > ret_width)
				{
					ret_width = rw;
				}
				m_margins.top	= top_margin;
				child_top	   += el->height();
				last_margin		= el->get_margins().bottom;
				last_margin_el	= el;
				is_first		= false;

				if(el->src_el()->css().get_position() == element_position_relative)
				{
					el->apply_relative_shift(self_size);
				}
			}
		}
	}

	if(self_size.height.type != containing_block_context::cbc_value_type_auto && self_size.height > 0)
	{
		m_pos.height = self_size.height;
	} else
	{
		m_pos.height = child_top;
		if(collapse_bottom_margin())
		{
			m_pos.height -= last_margin;
			if(m_margins.bottom < last_margin)
			{
				m_margins.bottom = last_margin;
			}
			if(last_margin_el)
			{
				last_margin_el->get_margins().bottom = 0;
			}
		}
	}

	return ret_width;
}

litehtml::pixel_t litehtml::render_item_block_context::get_first_baseline()
{
	if(m_children.empty())
	{
		return height() - margin_bottom();
	}
	const auto& item = m_children.front();
	return content_offset_top() + item->top() + item->get_first_baseline();
}

litehtml::pixel_t litehtml::render_item_block_context::get_last_baseline()
{
	if(m_children.empty())
	{
		return height() - margin_bottom();
	}
	const auto& item = m_children.back();
	return content_offset_top() + item->top() + item->get_last_baseline();
}
