#include "render_inline_context.h"
#include "document.h"
#include "iterators.h"
#include "types.h"

litehtml::pixel_t litehtml::render_item_inline_context::_render_content(pixel_t /*x*/, pixel_t /*y*/,
																		bool /*second_pass*/,
																		const containing_block_context& self_size,
																		formatting_context*				fmt_ctx)
{
	m_line_boxes.clear();
	m_max_line_width		= 0;

	white_space ws			= src_el()->css().get_white_space();
	bool		skip_spaces = false;
	if(ws == white_space_normal || ws == white_space_nowrap || ws == white_space_pre_line)
	{
		skip_spaces = true;
	}

	bool was_space = false;

	go_inside_inline  go_inside_inlines_selector;
	inline_selector	  select_inlines;
	elements_iterator inlines_iter(true, &go_inside_inlines_selector, &select_inlines);

	inlines_iter.process(shared_from_this(), [&](const std::shared_ptr<render_item>& el, iterator_item_type item_type) {
		switch(item_type)
		{
		case iterator_item_type_child:
			{
				// skip spaces to make rendering a bit faster
				if(skip_spaces)
				{
					if(el->src_el()->is_white_space())
					{
						if(was_space)
						{
							el->skip(true);
							return;
						} else
						{
							was_space = true;
						}
					} else
					{
						// skip all spaces after line break
						was_space = el->src_el()->is_break();
					}
				}
				// place element into rendering flow
				place_inline(std::make_unique<line_box_item>(el), self_size, fmt_ctx);
			}
			break;

		case iterator_item_type_start_parent:
			{
				el->clear_inline_boxes();
				place_inline(std::make_unique<lbi_start>(el), self_size, fmt_ctx);
			}
			break;

		case iterator_item_type_end_parent:
			{
				place_inline(std::make_unique<lbi_end>(el), self_size, fmt_ctx);
			}
			break;
		}
	});

	finish_last_box(true, self_size);

	if(!m_line_boxes.empty())
	{
		if(collapse_top_margin())
		{
			pixel_t old_top = m_margins.top;
			m_margins.top	= std::max(m_line_boxes.front()->top_margin(), m_margins.top);
			if(m_margins.top != old_top)
			{
				fmt_ctx->update_floats(m_margins.top - old_top, shared_from_this());
			}
		}
		if(collapse_bottom_margin())
		{
			m_margins.bottom = std::max(m_line_boxes.back()->bottom_margin(), m_margins.bottom);
			m_pos.height	 = m_line_boxes.back()->bottom() - m_line_boxes.back()->bottom_margin();
		} else
		{
			m_pos.height = m_line_boxes.back()->bottom();
		}
	}

	return m_max_line_width;
}

void litehtml::render_item_inline_context::fix_line_width(element_float flt, const containing_block_context& self_size,
														  formatting_context* fmt_ctx)
{
	if(!m_line_boxes.empty())
	{
		auto el_front = m_line_boxes.back()->get_first_text_part();

		std::vector<std::shared_ptr<render_item>> els;
		bool									  was_cleared = false;
		if(el_front && el_front->src_el()->css().get_clear() != clear_none)
		{
			if(el_front->src_el()->css().get_clear() == clear_both)
			{
				was_cleared = true;
			} else
			{
				if((flt == float_left && el_front->src_el()->css().get_clear() == clear_left) ||
				   (flt == float_right && el_front->src_el()->css().get_clear() == clear_right))
				{
					was_cleared = true;
				}
			}
		}

		if(!was_cleared)
		{
			std::list<std::unique_ptr<line_box_item>> items = std::move(m_line_boxes.back()->items());
			m_line_boxes.pop_back();

			for(auto& item : items)
			{
				place_inline(std::move(item), self_size, fmt_ctx);
			}
		} else
		{
			pixel_t line_top   = 0;
			line_top		   = m_line_boxes.back()->top();

			pixel_t line_left  = 0;
			pixel_t line_right = self_size.render_width;
			fmt_ctx->get_line_left_right(line_top, self_size.render_width, line_left, line_right);

			if(m_line_boxes.size() == 1)
			{
				if(src_el()->css().get_list_style_type() != list_style_type_none &&
				   src_el()->css().get_list_style_position() == list_style_position_inside)
				{
					pixel_t sz_font	 = src_el()->css().get_font_size();
					line_left		+= sz_font;
				}

				if(src_el()->css().get_text_indent().val() != 0)
				{
					line_left += src_el()->css().get_text_indent().calc_percent(self_size.width);
				}
			}

			auto items = m_line_boxes.back()->new_width(line_left, line_right);
			for(auto& item : items)
			{
				place_inline(std::move(item), self_size, fmt_ctx);
			}
		}
	}
}

std::list<std::unique_ptr<litehtml::line_box_item>> litehtml::render_item_inline_context::finish_last_box(
	bool end_of_render, const containing_block_context& self_size)
{
	std::list<std::unique_ptr<line_box_item>> ret;

	if(!m_line_boxes.empty())
	{
		ret = m_line_boxes.back()->finish(end_of_render, self_size);

		if(m_line_boxes.back()->is_empty() && end_of_render)
		{
			// remove the last empty line
			m_line_boxes.pop_back();
		} else
		{
			m_max_line_width = std::max(m_max_line_width, m_line_boxes.back()->min_width());
		}
	}
	return ret;
}

litehtml::pixel_t litehtml::render_item_inline_context::new_box(const std::unique_ptr<line_box_item>& el,
																line_context&						  line_ctx,
																const containing_block_context&		  self_size,
																formatting_context*					  fmt_ctx)
{
	auto	items	 = finish_last_box(false, self_size);
	pixel_t line_top = 0;
	if(!m_line_boxes.empty())
	{
		line_top = m_line_boxes.back()->bottom();
	}
	line_ctx.top   = fmt_ctx->get_cleared_top(el->get_el(), line_top);

	line_ctx.left  = 0;
	line_ctx.right = self_size.render_width;
	line_ctx.fix_top();
	fmt_ctx->get_line_left_right(line_ctx.top, self_size.render_width, line_ctx.left, line_ctx.right);

	if(el->get_el()->src_el()->is_inline() || el->get_el()->src_el()->is_block_formatting_context())
	{
		if(el->get_el()->width() > line_ctx.right - line_ctx.left)
		{
			line_ctx.top   = fmt_ctx->find_next_line_top(line_ctx.top, el->get_el()->width(), self_size.render_width);
			line_ctx.left  = 0;
			line_ctx.right = self_size.render_width;
			line_ctx.fix_top();
			fmt_ctx->get_line_left_right(line_ctx.top, self_size.render_width, line_ctx.left, line_ctx.right);
		}
	}

	pixel_t first_line_margin = 0;
	pixel_t text_indent		  = 0;
	if(m_line_boxes.empty())
	{
		if(src_el()->css().get_list_style_type() != list_style_type_none &&
		   src_el()->css().get_list_style_position() == list_style_position_inside)
		{
			pixel_t sz_font	  = src_el()->css().get_font_size();
			first_line_margin = sz_font;
		}
		if(src_el()->css().get_text_indent().val() != 0)
		{
			text_indent = src_el()->css().get_text_indent().calc_percent(self_size.width);
		}
	}

	m_line_boxes.emplace_back(std::make_unique<line_box>(line_ctx.top, line_ctx.left + first_line_margin + text_indent,
														 line_ctx.right, css().line_height(), css().get_font_metrics(),
														 css().get_text_align()));

	// Add items returned by finish_last_box function into the new line
	for(auto& it : items)
	{
		m_line_boxes.back()->add_item(std::move(it));
	}

	return line_ctx.top;
}

void litehtml::render_item_inline_context::place_inline(std::unique_ptr<line_box_item>	item,
														const containing_block_context& self_size,
														formatting_context*				fmt_ctx)
{
	if(item->get_el()->src_el()->css().get_display() == display_none)
		return;

	if(item->get_el()->src_el()->is_float())
	{
		pixel_t line_top = 0;
		if(!m_line_boxes.empty())
		{
			line_top = m_line_boxes.back()->top();
		}
		pixel_t ret = place_float(item->get_el(), line_top, self_size, fmt_ctx);
		if(ret > m_max_line_width)
		{
			m_max_line_width = ret;
		}
		return;
	}

	line_context line_ctx;
	if(!m_line_boxes.empty())
	{
		line_ctx.top = m_line_boxes.back().get()->top();
	}
	line_ctx.right = self_size.render_width;
	line_ctx.fix_top();
	fmt_ctx->get_line_left_right(line_ctx.top, self_size.render_width, line_ctx.left, line_ctx.right);

	if(item->get_type() == line_box_item::type_text_part)
	{
		if(item->get_el()->src_el()->is_inline_box())
		{
			pixel_t min_rendered_width =
				item->get_el()->render(line_ctx.left, line_ctx.top, self_size.new_width(line_ctx.right), fmt_ctx);
			if(min_rendered_width < item->get_el()->width() &&
			   item->get_el()->src_el()->css().get_width().is_predefined())
			{
				item->get_el()->render(line_ctx.left, line_ctx.top, self_size.new_width(min_rendered_width), fmt_ctx);
			}
			item->set_rendered_min_width(min_rendered_width);
		} else if(item->get_el()->src_el()->css().get_display() == display_inline_text)
		{
			litehtml::size sz;
			item->get_el()->src_el()->get_content_size(sz, line_ctx.right);
			item->get_el()->pos() = sz;
			item->set_rendered_min_width(sz.width);
		}
	}

	bool add_box = true;
	if(!m_line_boxes.empty())
	{
		if(m_line_boxes.back()->can_hold(item, src_el()->css().get_white_space()))
		{
			add_box = false;
		}
	}
	if(add_box)
	{
		new_box(item, line_ctx, self_size, fmt_ctx);
	} else if(!m_line_boxes.empty())
	{
		line_ctx.top = m_line_boxes.back()->top();
	}

	if(line_ctx.top != line_ctx.calculatedTop)
	{
		line_ctx.left  = 0;
		line_ctx.right = self_size.render_width;
		line_ctx.fix_top();
		fmt_ctx->get_line_left_right(line_ctx.top, self_size.render_width, line_ctx.left, line_ctx.right);
	}

	if(!item->get_el()->src_el()->is_inline())
	{
		if(m_line_boxes.size() == 1)
		{
			if(collapse_top_margin())
			{
				pixel_t shift = item->get_el()->margin_top();
				if(shift >= 0)
				{
					line_ctx.top -= shift;
					m_line_boxes.back()->y_shift(-shift);
				}
			}
		} else
		{
			pixel_t shift		= 0;
			pixel_t prev_margin = m_line_boxes[m_line_boxes.size() - 2]->bottom_margin();

			if(prev_margin > item->get_el()->margin_top())
			{
				shift = item->get_el()->margin_top();
			} else
			{
				shift = prev_margin;
			}
			if(shift >= 0)
			{
				line_ctx.top -= shift;
				m_line_boxes.back()->y_shift(-shift);
			}
		}
	}

	m_line_boxes.back()->add_item(std::move(item));
}

void litehtml::render_item_inline_context::apply_vertical_align()
{
	if(!m_line_boxes.empty())
	{
		pixel_t add			   = 0;
		pixel_t content_height = m_line_boxes.back()->bottom();

		if(m_pos.height > content_height)
		{
			switch(src_el()->css().get_vertical_align())
			{
			case va_middle:
				add = (m_pos.height - content_height) / 2;
				break;
			case va_bottom:
				add = m_pos.height - content_height;
				break;
			default:
				add = 0;
				break;
			}
		}

		if(add != 0)
		{
			for(auto& box : m_line_boxes)
			{
				box->y_shift(add);
			}
		}
	}
}

litehtml::pixel_t litehtml::render_item_inline_context::get_first_baseline()
{
	pixel_t bl;
	if(!m_line_boxes.empty())
	{
		const auto& line = m_line_boxes.front();
		bl				 = line->bottom() - line->baseline() + content_offset_top();
	} else
	{
		bl = height() - margin_bottom();
	}
	return bl;
}

litehtml::pixel_t litehtml::render_item_inline_context::get_last_baseline()
{
	pixel_t bl;
	if(!m_line_boxes.empty())
	{
		const auto& line = m_line_boxes.back();
		bl				 = line->bottom() - line->baseline() + content_offset_top();
	} else
	{
		bl = height();
	}
	return bl;
}
