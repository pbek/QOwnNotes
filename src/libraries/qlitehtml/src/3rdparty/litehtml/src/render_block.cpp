#include "render_block.h"
#include "render_inline_context.h"
#include "render_block_context.h"
#include "document.h"
#include "document_container.h"
#include "html_tag.h"
#include "types.h"

litehtml::pixel_t litehtml::render_item_block::place_float(const std::shared_ptr<render_item>& el, pixel_t top,
														   const containing_block_context& self_size,
														   formatting_context*			   fmt_ctx)
{
	pixel_t line_top   = fmt_ctx->get_cleared_top(el, top);
	pixel_t line_left  = 0;
	pixel_t line_right = self_size.render_width;
	fmt_ctx->get_line_left_right(line_top, self_size.render_width, line_left, line_right);

	pixel_t ret_width		   = 0;

	pixel_t min_rendered_width = el->render(line_left, line_top, self_size.new_width(line_right), fmt_ctx);
	if(min_rendered_width < el->width() && el->src_el()->css().get_width().is_predefined())
	{
		el->render(line_left, line_top, self_size.new_width(min_rendered_width), fmt_ctx);
	}

	if(el->src_el()->css().get_float() == float_left)
	{
		if(el->right() > line_right)
		{
			line_top	= fmt_ctx->find_next_line_top(el->top(), el->width(), self_size.render_width);
			el->pos().x = fmt_ctx->get_line_left(line_top) + el->content_offset_left();
			el->pos().y = line_top + el->content_offset_top();
		}
		fmt_ctx->add_float(el, min_rendered_width, self_size.context_idx);
		fix_line_width(float_left, self_size, fmt_ctx);

		ret_width = fmt_ctx->find_min_left(line_top, self_size.context_idx);
	} else if(el->src_el()->css().get_float() == float_right)
	{
		if(line_left + el->width() > line_right)
		{
			pixel_t new_top = fmt_ctx->find_next_line_top(el->top(), el->width(), self_size.render_width);
			el->pos().x =
				fmt_ctx->get_line_right(new_top, self_size.render_width) - el->width() + el->content_offset_left();
			el->pos().y = new_top + el->content_offset_top();
		} else
		{
			el->pos().x = line_right - el->width() + el->content_offset_left();
		}
		fmt_ctx->add_float(el, min_rendered_width, self_size.context_idx);
		fix_line_width(float_right, self_size, fmt_ctx);
		line_right = fmt_ctx->find_min_right(line_top, self_size.render_width, self_size.context_idx);
		ret_width  = self_size.render_width - line_right;
	}
	return ret_width;
}

std::shared_ptr<litehtml::render_item> litehtml::render_item_block::init()
{
	std::shared_ptr<render_item> ret;

	// Initialize indexes for list items
	if(src_el()->css().get_display() == display_list_item &&
	   src_el()->css().get_list_style_type() >= list_style_type_armenian)
	{
		if(auto p = src_el()->parent())
		{
			int val = atoi(p->get_attr("start", "1"));
			for(const auto& child : p->children())
			{
				if(child == src_el())
				{
					src_el()->set_attr("list_index", std::to_string(val).c_str());
					break;
				} else if(child->css().get_display() == display_list_item)
					val++;
			}
		}
	}
	// Split inline blocks with box blocks inside
	auto iter = m_children.begin();
	while(iter != m_children.end())
	{
		const auto& el = *iter;
		if(el->src_el()->css().get_display() == display_inline && !el->children().empty())
		{
			auto split_el = el->split_inlines();
			if(std::get<0>(split_el))
			{
				iter = m_children.erase(iter);
				iter = m_children.insert(iter, std::get<2>(split_el));
				iter = m_children.insert(iter, std::get<1>(split_el));
				iter = m_children.insert(iter, std::get<0>(split_el));

				std::get<0>(split_el)->parent(shared_from_this());
				std::get<1>(split_el)->parent(shared_from_this());
				std::get<2>(split_el)->parent(shared_from_this());
				continue;
			}
		}
		++iter;
	}

	bool has_block_level = false;
	bool has_inlines	 = false;
	for(const auto& el : m_children)
	{
		if(!el->src_el()->is_float())
		{
			if(el->src_el()->is_block_box())
			{
				has_block_level = true;
			} else if(el->src_el()->is_inline())
			{
				has_inlines = true;
			}
		}
		if(has_block_level && has_inlines)
			break;
	}
	if(has_block_level)
	{
		ret = std::make_shared<render_item_block_context>(src_el());
		ret->parent(parent());

		auto				 doc = src_el()->get_document();
		decltype(m_children) new_children;
		decltype(m_children) inlines;
		bool				 not_ws_added = false;
		for(const auto& el : m_children)
		{
			if(el->src_el()->is_inline())
			{
				inlines.push_back(el);
				if(!el->src_el()->is_white_space())
					not_ws_added = true;
			} else
			{
				if(not_ws_added)
				{
					auto anon_el = std::make_shared<html_tag>(src_el());
					auto anon_ri = std::make_shared<render_item_block>(anon_el);
					for(const auto& inl : inlines)
					{
						anon_ri->add_child(inl);
					}

					not_ws_added = false;
					new_children.push_back(anon_ri);
					anon_ri->parent(ret);
				}
				new_children.push_back(el);
				el->parent(ret);
				inlines.clear();
			}
		}
		if(!inlines.empty() && not_ws_added)
		{
			auto anon_el = std::make_shared<html_tag>(src_el());
			auto anon_ri = std::make_shared<render_item_block>(anon_el);
			for(const auto& inl : inlines)
			{
				anon_ri->add_child(inl);
			}

			new_children.push_back(anon_ri);
			anon_ri->parent(ret);
		}
		ret->children() = new_children;
	}

	if(!ret)
	{
		ret = std::make_shared<render_item_inline_context>(src_el());
		ret->parent(parent());
		ret->children() = children();
		for(const auto& el : ret->children())
		{
			el->parent(ret);
		}
	}

	ret->src_el()->add_render(ret);

	for(auto& el : ret->children())
	{
		el = el->init();
	}

	return ret;
}

litehtml::pixel_t litehtml::render_item_block::_render(pixel_t x, pixel_t y,
													   const containing_block_context& containing_block_size,
													   formatting_context* fmt_ctx, bool second_pass)
{
	containing_block_context self_size = calculate_containing_block_context(containing_block_size);

	//*****************************************
	// Render content
	//*****************************************
	pixel_t ret_width				   = _render_content(x, y, second_pass, self_size, fmt_ctx);
	//*****************************************

	if(src_el()->css().get_display() == display_list_item)
	{
		if(m_pos.height == 0)
		{
			m_pos.height = css().line_height().computed_value;
		}
	}

	bool requires_rerender = false; // when true, the second pass for content rendering is required

	// Set block width
	if(!(containing_block_size.size_mode & containing_block_context::size_mode_content))
	{
		if(self_size.width.type == containing_block_context::cbc_value_type_absolute)
		{
			ret_width = m_pos.width = self_size.render_width;
		} else
		{
			m_pos.width = self_size.render_width;
		}
	} else
	{
		m_pos.width = ret_width;
		if(self_size.width.type == containing_block_context::cbc_value_type_absolute && ret_width > self_size.width)
		{
			ret_width = self_size.width;
		}
	}

	// Fix width with max-width attribute
	if(self_size.max_width.type != containing_block_context::cbc_value_type_none)
	{
		if(m_pos.width > self_size.max_width)
		{
			m_pos.width		  = self_size.max_width;
			requires_rerender = true;
		}
	}

	// Fix width with min-width attribute
	if(self_size.min_width.type != containing_block_context::cbc_value_type_none)
	{
		if(m_pos.width < self_size.min_width)
		{
			m_pos.width		  = self_size.min_width;
			requires_rerender = true;
		}
	} else if(m_pos.width < 0)
	{
		m_pos.width = 0;
	}

	// re-render content with new width if required
	if(requires_rerender && !second_pass && !is_root())
	{
		if(src_el()->is_block_formatting_context())
		{
			fmt_ctx->clear_floats(-1);
		} else
		{
			fmt_ctx->clear_floats(self_size.context_idx);
		}

		_render_content(x, y, true, self_size.new_width(m_pos.width), fmt_ctx);
	}

	// Set block height
	if(self_size.height.type != containing_block_context::cbc_value_type_auto &&
	   !(containing_block_size.size_mode & containing_block_context::size_mode_content))
	{
		// TODO: Something wrong here
		// Percentage height from undefined containing block height is usually <= 0
		if(self_size.height.type == containing_block_context::cbc_value_type_percentage)
		{
			if(self_size.height > 0)
			{
				m_pos.height = self_size.height;
			}
		} else
		{
			m_pos.height = self_size.height;
		}
		m_pos.height -= box_sizing_height();
	} else if(src_el()->is_block_formatting_context())
	{
		// add the floats' height to the block height
		pixel_t floats_height = fmt_ctx->get_floats_height();
		if(floats_height > m_pos.height)
		{
			m_pos.height = floats_height;
		}
	}
	if(containing_block_size.size_mode & containing_block_context::size_mode_content)
	{
		if(self_size.height.type == containing_block_context::cbc_value_type_absolute)
		{
			if(m_pos.height > self_size.height)
			{
				m_pos.height = self_size.height;
			}
		}
	}

	// Fix height with min-height attribute
	if(self_size.min_height.type != containing_block_context::cbc_value_type_none)
	{
		if(m_pos.height < self_size.min_height)
		{
			m_pos.height = self_size.min_height;
		}
	} else if(m_pos.height < 0)
	{
		m_pos.height = 0;
	}

	// Fix width with max-width attribute
	if(self_size.max_height.type != containing_block_context::cbc_value_type_none)
	{
		if(m_pos.height > self_size.max_height)
		{
			m_pos.height = self_size.max_height;
		}
	}

	// calculate the final position
	m_pos.move_to(x, y);
	m_pos.x += content_offset_left();
	m_pos.y += content_offset_top();

	if(src_el()->css().get_display() == display_list_item)
	{
		string list_image = src_el()->css().get_list_style_image();
		if(!list_image.empty())
		{
			size   sz;
			string list_image_baseurl = src_el()->css().get_list_style_image_baseurl();
			src_el()->get_document()->container()->get_image_size(list_image.c_str(), list_image_baseurl.c_str(), sz);
			if(m_pos.height < sz.height)
			{
				m_pos.height = sz.height;
			}
		}
	}

	return ret_width + content_offset_width();
}
