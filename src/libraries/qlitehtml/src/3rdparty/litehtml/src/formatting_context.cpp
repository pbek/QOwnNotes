#include "render_item.h"
#include "types.h"
#include "formatting_context.h"

void litehtml::formatting_context::add_float(const std::shared_ptr<render_item>& el, pixel_t min_width, int context)
{
	floated_box fb;
	fb.pos.x		= el->left() + m_current_left;
	fb.pos.y		= el->top() + m_current_top;
	fb.pos.width	= el->width();
	fb.pos.height	= el->height();
	fb.float_side	= el->src_el()->css().get_float();
	fb.clear_floats = el->src_el()->css().get_clear();
	fb.el			= el;
	fb.context		= context;
	fb.min_width	= min_width;

	if(fb.float_side == float_left)
	{
		if(m_floats_left.empty())
		{
			m_floats_left.push_back(fb);
		} else
		{
			bool inserted = false;
			for(auto i = m_floats_left.begin(); i != m_floats_left.end(); i++)
			{
				if(fb.pos.right() > i->pos.right())
				{
					m_floats_left.insert(i, std::move(fb));
					inserted = true;
					break;
				}
			}
			if(!inserted)
			{
				m_floats_left.push_back(std::move(fb));
			}
		}
		m_cache_line_left.invalidate();
	} else if(fb.float_side == float_right)
	{
		if(m_floats_right.empty())
		{
			m_floats_right.push_back(std::move(fb));
		} else
		{
			bool inserted = false;
			for(auto i = m_floats_right.begin(); i != m_floats_right.end(); i++)
			{
				if(fb.pos.left() < i->pos.left())
				{
					m_floats_right.insert(i, std::move(fb));
					inserted = true;
					break;
				}
			}
			if(!inserted)
			{
				m_floats_right.push_back(fb);
			}
		}
		m_cache_line_right.invalidate();
	}
}

litehtml::pixel_t litehtml::formatting_context::get_floats_height(element_float el_float) const
{
	pixel_t h = m_current_top;

	for(const auto& fb : m_floats_left)
	{
		bool process = false;
		switch(el_float)
		{
		case float_none:
			process = true;
			break;
		case float_left:
			if(fb.clear_floats == clear_left || fb.clear_floats == clear_both)
			{
				process = true;
			}
			break;
		case float_right:
			if(fb.clear_floats == clear_right || fb.clear_floats == clear_both)
			{
				process = true;
			}
			break;
		}
		if(process)
		{
			if(el_float == float_none)
			{
				h = std::max(h, fb.pos.bottom());
			} else
			{
				h = std::max(h, fb.pos.top());
			}
		}
	}

	for(const auto& fb : m_floats_right)
	{
		bool process = false;
		switch(el_float)
		{
		case float_none:
			process = true;
			break;
		case float_left:
			if(fb.clear_floats == clear_left || fb.clear_floats == clear_both)
			{
				process = true;
			}
			break;
		case float_right:
			if(fb.clear_floats == clear_right || fb.clear_floats == clear_both)
			{
				process = true;
			}
			break;
		}
		if(process)
		{
			if(el_float == float_none)
			{
				h = std::max(h, fb.pos.bottom());
			} else
			{
				h = std::max(h, fb.pos.top());
			}
		}
	}

	return h - m_current_top;
}

litehtml::pixel_t litehtml::formatting_context::get_left_floats_height() const
{
	pixel_t h = 0;
	if(!m_floats_left.empty())
	{
		for(const auto& fb : m_floats_left)
		{
			h = std::max(h, fb.pos.bottom());
		}
	}
	return h - m_current_top;
}

litehtml::pixel_t litehtml::formatting_context::get_right_floats_height() const
{
	pixel_t h = 0;
	if(!m_floats_right.empty())
	{
		for(const auto& fb : m_floats_right)
		{
			h = std::max(h, fb.pos.bottom());
		}
	}
	return h - m_current_top;
}

litehtml::pixel_t litehtml::formatting_context::get_line_left(pixel_t y)
{
	y += m_current_top;

	if(m_cache_line_left.is_valid && m_cache_line_left.hash == y)
	{
		if(m_cache_line_left.val - m_current_left < 0)
		{
			return 0;
		}
		return m_cache_line_left.val - m_current_left;
	}

	pixel_t w = 0;
	for(const auto& fb : m_floats_left)
	{
		if(y >= fb.pos.top() && y < fb.pos.bottom())
		{
			w = std::max(w, fb.pos.right());
			if(w < fb.pos.right())
			{
				break;
			}
		}
	}
	m_cache_line_left.set_value(y, w);
	w -= m_current_left;
	if(w < 0)
		return 0;
	return w;
}

litehtml::pixel_t litehtml::formatting_context::get_line_right(pixel_t y, pixel_t def_right)
{
	y		  += m_current_top;
	def_right += m_current_left;
	if(m_cache_line_right.is_valid && m_cache_line_right.hash == y)
	{
		if(m_cache_line_right.is_default)
		{
			return def_right - m_current_left;
		} else
		{
			pixel_t w = std::min(m_cache_line_right.val, def_right) - m_current_left;
			if(w < 0)
				return 0;
			return w;
		}
	}

	pixel_t w					  = def_right;
	m_cache_line_right.is_default = true;
	for(const auto& fb : m_floats_right)
	{
		if(y >= fb.pos.top() && y < fb.pos.bottom())
		{
			w							  = std::min(w, fb.pos.left());
			m_cache_line_right.is_default = false;
			if(w > fb.pos.left())
			{
				break;
			}
		}
	}
	m_cache_line_right.set_value(y, w);
	w -= m_current_left;
	if(w < 0)
		return 0;
	return w;
}

void litehtml::formatting_context::clear_floats(int context)
{
	auto iter = m_floats_left.begin();
	while(iter != m_floats_left.end())
	{
		if(iter->context >= context)
		{
			iter = m_floats_left.erase(iter);
			m_cache_line_left.invalidate();
		} else
		{
			iter++;
		}
	}

	iter = m_floats_right.begin();
	while(iter != m_floats_right.end())
	{
		if(iter->context >= context)
		{
			iter = m_floats_right.erase(iter);
			m_cache_line_right.invalidate();
		} else
		{
			iter++;
		}
	}
}

litehtml::pixel_t litehtml::formatting_context::get_cleared_top(const std::shared_ptr<render_item>& el,
																pixel_t								line_top) const
{
	switch(el->src_el()->css().get_clear())
	{
	case clear_left:
		{
			pixel_t fh = get_left_floats_height();
			if(fh != 0 && fh > line_top)
			{
				line_top = fh;
			}
		}
		break;
	case clear_right:
		{
			pixel_t fh = get_right_floats_height();
			if(fh != 0 && fh > line_top)
			{
				line_top = fh;
			}
		}
		break;
	case clear_both:
		{
			pixel_t fh = get_floats_height(float_none);
			if(fh != 0 && fh > line_top)
			{
				line_top = fh;
			}
		}
		break;
	default:
		if(el->src_el()->css().get_float() != float_none)
		{
			pixel_t fh = get_floats_height(el->src_el()->css().get_float());
			if(fh != 0 && fh > line_top)
			{
				line_top = fh;
			}
		}
		break;
	}
	return line_top;
}

litehtml::pixel_t litehtml::formatting_context::find_next_line_top(pixel_t top, pixel_t width, pixel_t def_right)
{
	top					 += m_current_top;
	def_right			 += m_current_left;

	pixel_t		 new_top  = top;
	pixel_vector points;

	for(const auto& fb : m_floats_left)
	{
		if(fb.pos.top() >= top)
		{
			if(find(points.begin(), points.end(), fb.pos.top()) == points.end())
			{
				points.push_back(fb.pos.top());
			}
		}
		if(fb.pos.bottom() >= top)
		{
			if(find(points.begin(), points.end(), fb.pos.bottom()) == points.end())
			{
				points.push_back(fb.pos.bottom());
			}
		}
	}

	for(const auto& fb : m_floats_right)
	{
		if(fb.pos.top() >= top)
		{
			if(find(points.begin(), points.end(), fb.pos.top()) == points.end())
			{
				points.push_back(fb.pos.top());
			}
		}
		if(fb.pos.bottom() >= top)
		{
			if(find(points.begin(), points.end(), fb.pos.bottom()) == points.end())
			{
				points.push_back(fb.pos.bottom());
			}
		}
	}

	if(!points.empty())
	{
		sort(points.begin(), points.end(), std::less<pixel_t>());
		new_top = points.back();

		for(auto pt : points)
		{
			pixel_t pos_left  = 0;
			pixel_t pos_right = def_right;
			get_line_left_right(pt - m_current_top, def_right - m_current_left, pos_left, pos_right);

			if(pos_right - pos_left >= width)
			{
				new_top = pt;
				break;
			}
		}
	}
	return new_top - m_current_top;
}

void litehtml::formatting_context::update_floats(pixel_t dy, const std::shared_ptr<render_item>& parent)
{
	bool reset_cache = false;
	for(auto fb = m_floats_left.rbegin(); fb != m_floats_left.rend(); fb++)
	{
		if(fb->el->src_el()->is_ancestor(parent->src_el()))
		{
			reset_cache	 = true;
			fb->pos.y	+= dy;
		}
	}
	if(reset_cache)
	{
		m_cache_line_left.invalidate();
	}
	reset_cache = false;
	for(auto fb = m_floats_right.rbegin(); fb != m_floats_right.rend(); fb++)
	{
		if(fb->el->src_el()->is_ancestor(parent->src_el()))
		{
			reset_cache	 = true;
			fb->pos.y	+= dy;
		}
	}
	if(reset_cache)
	{
		m_cache_line_right.invalidate();
	}
}

void litehtml::formatting_context::apply_relative_shift(const containing_block_context& containing_block_size)
{
	for(const auto& fb : m_floats_left)
	{
		fb.el->apply_relative_shift(containing_block_size);
	}
}

litehtml::pixel_t litehtml::formatting_context::find_min_left(pixel_t y, int context_idx)
{
	y				 += m_current_top;
	pixel_t min_left  = m_current_left;
	for(const auto& fb : m_floats_left)
	{
		if(y >= fb.pos.top() && y < fb.pos.bottom() && fb.context == context_idx)
		{
			min_left += fb.min_width;
		}
	}
	if(min_left < m_current_left)
		return 0;
	return min_left - m_current_left;
}

litehtml::pixel_t litehtml::formatting_context::find_min_right(pixel_t y, pixel_t right, int context_idx)
{
	y				  += m_current_top;
	pixel_t min_right  = right + m_current_left;
	for(const auto& fb : m_floats_right)
	{
		if(y >= fb.pos.top() && y < fb.pos.bottom() && fb.context == context_idx)
		{
			min_right -= fb.min_width;
		}
	}
	if(min_right < m_current_left)
		return 0;
	return min_right - m_current_left;
}
