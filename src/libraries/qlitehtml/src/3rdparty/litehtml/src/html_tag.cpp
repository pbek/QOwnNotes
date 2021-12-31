#include "html.h"
#include "html_tag.h"
#include "document.h"
#include "iterators.h"
#include "stylesheet.h"
#include "table.h"
#include <algorithm>
#include <locale>
#include "el_before_after.h"
#include "num_cvt.h"

litehtml::html_tag::html_tag(const std::shared_ptr<litehtml::document>& doc) : litehtml::element(doc)
{
	m_box_sizing			= box_sizing_content_box;
	m_z_index				= 0;
	m_overflow				= overflow_visible;
	m_box					= nullptr;
	m_text_align			= text_align_left;
	m_el_position			= element_position_static;
	m_display				= display_inline;
	m_vertical_align		= va_baseline;
	m_list_style_type		= list_style_type_none;
	m_list_style_position	= list_style_position_outside;
	m_float					= float_none;
	m_clear					= clear_none;
	m_font					= 0;
	m_font_size				= 0;
	m_white_space			= white_space_normal;
	m_lh_predefined			= false;
	m_line_height			= 0;
	m_visibility			= visibility_visible;
	m_border_spacing_x		= 0;
	m_border_spacing_y		= 0;
	m_border_collapse		= border_collapse_separate;
}

bool litehtml::html_tag::appendChild(const element::ptr &el)
{
	if(el)
	{
		el->parent(shared_from_this());
		m_children.push_back(el);
		return true;
	}
	return false;
}

bool litehtml::html_tag::removeChild(const element::ptr &el)
{
	if(el && el->parent() == shared_from_this())
	{
		el->parent(nullptr);
		m_children.erase(std::remove(m_children.begin(), m_children.end(), el), m_children.end());
		return true;
	}
	return false;
}

void litehtml::html_tag::clearRecursive()
{
	for(auto& el : m_children)
	{
		el->clearRecursive();
		el->parent(nullptr);
	}
	m_children.clear();
}


const litehtml::tchar_t* litehtml::html_tag::get_tagName() const
{
	return m_tag.c_str();
}

void litehtml::html_tag::set_attr( const tchar_t* name, const tchar_t* val )
{
	if(name && val)
	{
		tstring s_val = name;
		for(tchar_t& i : s_val)
		{
			i = std::tolower(i, std::locale::classic());
		}
		m_attrs[s_val] = val;

		if( t_strcasecmp( name, _t("class") ) == 0 )
		{
			m_class_values.resize( 0 );
			split_string( val, m_class_values, _t(" ") );
		}
	}
}

const litehtml::tchar_t* litehtml::html_tag::get_attr( const tchar_t* name, const tchar_t* def ) const
{
	auto attr = m_attrs.find(name);
	if(attr != m_attrs.end())
	{
		return attr->second.c_str();
	}
	return def;
}

litehtml::elements_vector litehtml::html_tag::select_all( const tstring& selector )
{
	css_selector sel(media_query_list::ptr(nullptr));
	sel.parse(selector);
	
	return select_all(sel);
}

litehtml::elements_vector litehtml::html_tag::select_all( const css_selector& selector )
{
	litehtml::elements_vector res;
	select_all(selector, res);
	return res;
}

void litehtml::html_tag::select_all(const css_selector& selector, elements_vector& res)
{
	if(select(selector))
	{
		res.push_back(shared_from_this());
	}
	
	for(auto& el : m_children)
	{
		el->select_all(selector, res);
	}
}


litehtml::element::ptr litehtml::html_tag::select_one( const tstring& selector )
{
	css_selector sel(media_query_list::ptr(nullptr));
	sel.parse(selector);

	return select_one(sel);
}

litehtml::element::ptr litehtml::html_tag::select_one( const css_selector& selector )
{
	if(select(selector))
	{
		return shared_from_this();
	}

	for(auto& el : m_children)
	{
		element::ptr res = el->select_one(selector);
		if(res)
		{
			return res;
		}
	}
	return nullptr;
}

void litehtml::html_tag::apply_stylesheet( const litehtml::css& stylesheet )
{
	remove_before_after();

	for(const auto& sel : stylesheet.selectors())
	{
		int apply = select(*sel, false);

		if(apply != select_no_match)
		{
			used_selector::ptr us = std::unique_ptr<used_selector>(new used_selector(sel, false));

			if(sel->is_media_valid())
			{
				if(apply & select_match_pseudo_class)
				{
					if(select(*sel, true))
					{
						if(apply & select_match_with_after)
						{
							element::ptr el = get_element_after();
							if(el)
							{
								el->add_style(*sel->m_style);
							}
						} else if(apply & select_match_with_before)
						{
							element::ptr el = get_element_before();
							if(el)
							{
								el->add_style(*sel->m_style);
							}
						}
						else
						{
							add_style(*sel->m_style);
							us->m_used = true;
						}
					}
				} else if(apply & select_match_with_after)
				{
					element::ptr el = get_element_after();
					if(el)
					{
						el->add_style(*sel->m_style);
					}
				} else if(apply & select_match_with_before)
				{
					element::ptr el = get_element_before();
					if(el)
					{
						el->add_style(*sel->m_style);
					}
				} else
				{
					add_style(*sel->m_style);
					us->m_used = true;
				}
			}
			m_used_styles.push_back(std::move(us));
		}
	}

	for(auto& el : m_children)
	{
		if(el->get_display() != display_inline_text)
		{
			el->apply_stylesheet(stylesheet);
		}
	}
}

void litehtml::html_tag::get_content_size( size& sz, int max_width )
{
	sz.height	= 0;
	if(m_display == display_block)
	{
		sz.width	= max_width;
	} else
	{
		sz.width	= 0;
	}
}

void litehtml::html_tag::draw( uint_ptr hdc, int x, int y, const position* clip )
{
	position pos = m_pos;
	pos.x	+= x;
	pos.y	+= y;

	draw_background(hdc, x, y, clip);

	if(m_display == display_list_item && m_list_style_type != list_style_type_none)
	{
		if(m_overflow > overflow_visible)
		{
			position border_box = pos;
			border_box += m_padding;
			border_box += m_borders;

			border_radiuses bdr_radius = m_css_borders.radius.calc_percents(border_box.width, border_box.height);

			bdr_radius -= m_borders;
			bdr_radius -= m_padding;

			get_document()->container()->set_clip(pos, bdr_radius, true, true);
		}

		draw_list_marker(hdc, pos);

		if(m_overflow > overflow_visible)
		{
			get_document()->container()->del_clip();
		}
	}
}

litehtml::uint_ptr litehtml::html_tag::get_font(font_metrics* fm)
{
	if(fm)
	{
		*fm = m_font_metrics;
	}
	return m_font;
}

const litehtml::tchar_t* litehtml::html_tag::get_style_property( const tchar_t* name, bool inherited, const tchar_t* def /*= 0*/ )
{
	const tchar_t* ret = m_style.get_property(name);
	element::ptr el_parent = parent();
	if (el_parent)
	{
		if ( ( ret && !t_strcasecmp(ret, _t("inherit")) ) || (!ret && inherited) )
		{
			ret = el_parent->get_style_property(name, inherited, def);
		}
	}

	if(!ret)
	{
		ret = def;
	}

	return ret;
}

void litehtml::html_tag::parse_styles(bool is_reparse)
{
	const tchar_t* style = get_attr(_t("style"));

	if(style)
	{
		m_style.add(style, nullptr);
	}

	init_font();
	document::ptr doc = get_document();

	m_el_position	= (element_position)	value_index(get_style_property(_t("position"),		false,	_t("static")),		element_position_strings,	element_position_fixed);
	m_text_align	= (text_align)			value_index(get_style_property(_t("text-align"),		true,	_t("left")),		text_align_strings,			text_align_left);
	m_overflow		= (overflow)			value_index(get_style_property(_t("overflow"),		false,	_t("visible")),		overflow_strings,			overflow_visible);
	m_white_space	= (white_space)			value_index(get_style_property(_t("white-space"),	true,	_t("normal")),		white_space_strings,		white_space_normal);
	m_display		= (style_display)		value_index(get_style_property(_t("display"),		false,	_t("inline")),		style_display_strings,		display_inline);
	m_visibility	= (visibility)			value_index(get_style_property(_t("visibility"),	true,	_t("visible")),		visibility_strings,			visibility_visible);
	m_box_sizing	= (box_sizing)			value_index(get_style_property(_t("box-sizing"),		false,	_t("content-box")),	box_sizing_strings,			box_sizing_content_box);

	if(m_el_position != element_position_static)
	{
		const tchar_t* val = get_style_property(_t("z-index"), false, nullptr);
		if(val)
		{
			m_z_index = t_atoi(val);
		}
	}

	const tchar_t* va	= get_style_property(_t("vertical-align"), true,	_t("baseline"));
	m_vertical_align = (vertical_align) value_index(va, vertical_align_strings, va_baseline);

	const tchar_t* fl	= get_style_property(_t("float"), false,	_t("none"));
	m_float = (element_float) value_index(fl, element_float_strings, float_none);

	m_clear = (element_clear) value_index(get_style_property(_t("clear"), false, _t("none")), element_clear_strings, clear_none);

	if (m_float != float_none)
	{
		// reset display in to block for floating elements
		if (m_display != display_none)
		{
			m_display = display_block;
		}
	}
	else if (m_display == display_table ||
		m_display == display_inline_table ||
		m_display == display_table_caption ||
		m_display == display_table_cell ||
		m_display == display_table_column ||
		m_display == display_table_column_group ||
		m_display == display_table_footer_group ||
		m_display == display_table_header_group ||
		m_display == display_table_row ||
		m_display == display_table_row_group)
	{
		doc->add_tabular(shared_from_this());
	}
	// fix inline boxes with absolute/fixed positions
	else if (m_display != display_none && is_inline_box())
	{
		if (m_el_position == element_position_absolute || m_el_position == element_position_fixed)
		{
			m_display = display_block;
		}
	}

	m_css_text_indent.fromString(	get_style_property(_t("text-indent"),	true,	_t("0")),	_t("0"));

	m_css_width.fromString(			get_style_property(_t("width"),			false,	_t("auto")), _t("auto"));
	m_css_height.fromString(		get_style_property(_t("height"),		false,	_t("auto")), _t("auto"));

	doc->cvt_units(m_css_width, m_font_size);
	doc->cvt_units(m_css_height, m_font_size);

	m_css_min_width.fromString(		get_style_property(_t("min-width"),		false,	_t("0")));
	m_css_min_height.fromString(	get_style_property(_t("min-height"),		false,	_t("0")));

	m_css_max_width.fromString(		get_style_property(_t("max-width"),		false,	_t("none")),	_t("none"));
	m_css_max_height.fromString(	get_style_property(_t("max-height"),		false,	_t("none")),	_t("none"));
	
	doc->cvt_units(m_css_min_width, m_font_size);
	doc->cvt_units(m_css_min_height, m_font_size);

	m_css_offsets.left.fromString(		get_style_property(_t("left"),				false,	_t("auto")), _t("auto"));
	m_css_offsets.right.fromString(		get_style_property(_t("right"),				false,	_t("auto")), _t("auto"));
	m_css_offsets.top.fromString(		get_style_property(_t("top"),				false,	_t("auto")), _t("auto"));
	m_css_offsets.bottom.fromString(	get_style_property(_t("bottom"),			false,	_t("auto")), _t("auto"));

	doc->cvt_units(m_css_offsets.left, m_font_size);
	doc->cvt_units(m_css_offsets.right, m_font_size);
	doc->cvt_units(m_css_offsets.top,		m_font_size);
	doc->cvt_units(m_css_offsets.bottom,	m_font_size);

	m_css_margins.left.fromString(		get_style_property(_t("margin-left"),		false,	_t("0")), _t("auto"));
	m_css_margins.right.fromString(		get_style_property(_t("margin-right"),		false,	_t("0")), _t("auto"));
	m_css_margins.top.fromString(		get_style_property(_t("margin-top"),			false,	_t("0")), _t("auto"));
	m_css_margins.bottom.fromString(	get_style_property(_t("margin-bottom"),		false,	_t("0")), _t("auto"));

	m_css_padding.left.fromString(		get_style_property(_t("padding-left"),		false,	_t("0")), _t(""));
	m_css_padding.right.fromString(		get_style_property(_t("padding-right"),		false,	_t("0")), _t(""));
	m_css_padding.top.fromString(		get_style_property(_t("padding-top"),		false,	_t("0")), _t(""));
	m_css_padding.bottom.fromString(	get_style_property(_t("padding-bottom"),		false,	_t("0")), _t(""));

	m_css_borders.left.width.fromString(	get_style_property(_t("border-left-width"),		false,	_t("medium")), border_width_strings);
	m_css_borders.right.width.fromString(	get_style_property(_t("border-right-width"),		false,	_t("medium")), border_width_strings);
	m_css_borders.top.width.fromString(		get_style_property(_t("border-top-width"),		false,	_t("medium")), border_width_strings);
	m_css_borders.bottom.width.fromString(	get_style_property(_t("border-bottom-width"),	false,	_t("medium")), border_width_strings);

	m_css_borders.left.color = web_color::from_string(get_style_property(_t("border-left-color"),	false,	_t("")), doc->container());
	m_css_borders.left.style = (border_style) value_index(get_style_property(_t("border-left-style"), false, _t("none")), border_style_strings, border_style_none);

    m_css_borders.right.color = web_color::from_string(get_style_property(_t("border-right-color"), false, _t("")), doc->container());
	m_css_borders.right.style = (border_style) value_index(get_style_property(_t("border-right-style"), false, _t("none")), border_style_strings, border_style_none);

    m_css_borders.top.color = web_color::from_string(get_style_property(_t("border-top-color"), false, _t("")), doc->container());
	m_css_borders.top.style = (border_style) value_index(get_style_property(_t("border-top-style"), false, _t("none")), border_style_strings, border_style_none);

    m_css_borders.bottom.color = web_color::from_string(get_style_property(_t("border-bottom-color"), false, _t("")), doc->container());
	m_css_borders.bottom.style = (border_style) value_index(get_style_property(_t("border-bottom-style"), false, _t("none")), border_style_strings, border_style_none);

	m_css_borders.radius.top_left_x.fromString(get_style_property(_t("border-top-left-radius-x"), false, _t("0")));
	m_css_borders.radius.top_left_y.fromString(get_style_property(_t("border-top-left-radius-y"), false, _t("0")));

	m_css_borders.radius.top_right_x.fromString(get_style_property(_t("border-top-right-radius-x"), false, _t("0")));
	m_css_borders.radius.top_right_y.fromString(get_style_property(_t("border-top-right-radius-y"), false, _t("0")));

	m_css_borders.radius.bottom_right_x.fromString(get_style_property(_t("border-bottom-right-radius-x"), false, _t("0")));
	m_css_borders.radius.bottom_right_y.fromString(get_style_property(_t("border-bottom-right-radius-y"), false, _t("0")));

	m_css_borders.radius.bottom_left_x.fromString(get_style_property(_t("border-bottom-left-radius-x"), false, _t("0")));
	m_css_borders.radius.bottom_left_y.fromString(get_style_property(_t("border-bottom-left-radius-y"), false, _t("0")));

	doc->cvt_units(m_css_borders.radius.bottom_left_x,			m_font_size);
	doc->cvt_units(m_css_borders.radius.bottom_left_y,			m_font_size);
	doc->cvt_units(m_css_borders.radius.bottom_right_x,			m_font_size);
	doc->cvt_units(m_css_borders.radius.bottom_right_y,			m_font_size);
	doc->cvt_units(m_css_borders.radius.top_left_x,				m_font_size);
	doc->cvt_units(m_css_borders.radius.top_left_y,				m_font_size);
	doc->cvt_units(m_css_borders.radius.top_right_x,				m_font_size);
	doc->cvt_units(m_css_borders.radius.top_right_y,				m_font_size);

	doc->cvt_units(m_css_text_indent,								m_font_size);

	m_margins.left		= doc->cvt_units(m_css_margins.left,		m_font_size);
	m_margins.right		= doc->cvt_units(m_css_margins.right,		m_font_size);
	m_margins.top		= doc->cvt_units(m_css_margins.top,		m_font_size);
	m_margins.bottom	= doc->cvt_units(m_css_margins.bottom,	m_font_size);

	m_padding.left		= doc->cvt_units(m_css_padding.left,		m_font_size);
	m_padding.right		= doc->cvt_units(m_css_padding.right,		m_font_size);
	m_padding.top		= doc->cvt_units(m_css_padding.top,		m_font_size);
	m_padding.bottom	= doc->cvt_units(m_css_padding.bottom,	m_font_size);

	m_borders.left		= doc->cvt_units(m_css_borders.left.width,	m_font_size);
	m_borders.right		= doc->cvt_units(m_css_borders.right.width,	m_font_size);
	m_borders.top		= doc->cvt_units(m_css_borders.top.width,		m_font_size);
	m_borders.bottom	= doc->cvt_units(m_css_borders.bottom.width,	m_font_size);

	css_length line_height;
	line_height.fromString(get_style_property(_t("line-height"),	true,	_t("normal")), _t("normal"));
	if(line_height.is_predefined())
	{
		m_line_height = m_font_metrics.height;
		m_lh_predefined = true;
	} else if(line_height.units() == css_units_none)
	{
		m_line_height = (int) (line_height.val() * (float) m_font_size);
		m_lh_predefined = false;
	} else
	{
		m_line_height =  doc->cvt_units(line_height,	m_font_size, m_font_size);
		m_lh_predefined = false;
	}


	if(m_display == display_list_item)
	{
		const tchar_t* list_type = get_style_property(_t("list-style-type"), true, _t("disc"));
		m_list_style_type = (list_style_type) value_index(list_type, list_style_type_strings, list_style_type_disc);

		const tchar_t* list_pos = get_style_property(_t("list-style-position"), true, _t("outside"));
		m_list_style_position = (list_style_position) value_index(list_pos, list_style_position_strings, list_style_position_outside);

		const tchar_t* list_image = get_style_property(_t("list-style-image"), true, nullptr);
		if(list_image && list_image[0])
		{
			tstring url;
			css::parse_css_url(list_image, url);

			const tchar_t* list_image_baseurl = get_style_property(_t("list-style-image-baseurl"), true, nullptr);
			doc->container()->load_image(url.c_str(), list_image_baseurl, true);
		}

	}

	parse_background();

	if(!is_reparse)
	{
		for(auto& el : m_children)
		{
			el->parse_styles();
		}
	}
}

int litehtml::html_tag::render( int x, int y, int max_width, bool second_pass )
{
	if (m_display == display_table || m_display == display_inline_table)
	{
		return render_table(x, y, max_width, second_pass);
	}

	return render_box(x, y, max_width, second_pass);
}

bool litehtml::html_tag::is_white_space() const
{
	return false;
}

int litehtml::html_tag::get_font_size() const
{
	return m_font_size;
}

int litehtml::html_tag::get_base_line()
{
	if(is_replaced())
	{
		return 0;
	}
	int bl = 0;
	if(!m_boxes.empty())
	{
		bl = m_boxes.back()->baseline() + content_margins_bottom();
	}
	return bl;
}

void litehtml::html_tag::init()
{
	if (m_display == display_table || m_display == display_inline_table)
	{
		if (m_grid)
		{
			m_grid->clear();
		}
		else
		{
			m_grid = std::unique_ptr<table_grid>(new table_grid());
		}

		go_inside_table 		table_selector;
		table_rows_selector		row_selector;
		table_cells_selector	cell_selector;

		elements_iterator row_iter(shared_from_this(), &table_selector, &row_selector);

		element::ptr row = row_iter.next(false);
		while (row)
		{
			m_grid->begin_row(row);

			elements_iterator cell_iter(row, &table_selector, &cell_selector);
			element::ptr cell = cell_iter.next();
			while (cell)
			{
				m_grid->add_cell(cell);

				cell = cell_iter.next(false);
			}
			row = row_iter.next(false);
		}

		for (auto& el : m_children)
		{
			if (el->get_display() == display_table_caption)
			{
				m_grid->captions().push_back(el);
			}
		}

		m_grid->finish();
	}

	for (auto& el : m_children)
	{
		el->init();
	}
}

int litehtml::html_tag::select(const css_selector& selector, bool apply_pseudo)
{
	int right_res = select(selector.m_right, apply_pseudo);
	if(right_res == select_no_match)
	{
		return select_no_match;
	}
	element::ptr el_parent = parent();
	if(selector.m_left)
	{
		if (!el_parent)
		{
			return select_no_match;
		}
		switch(selector.m_combinator)
		{
		case combinator_descendant:
			{
				bool is_pseudo = false;
				element::ptr res = find_ancestor(*selector.m_left, apply_pseudo, &is_pseudo);
				if(!res)
				{
					return select_no_match;
				} else
				{
					if(is_pseudo)
					{
						right_res |= select_match_pseudo_class;
					}
				}
			}
			break;
		case combinator_child:
			{
				int res = el_parent->select(*selector.m_left, apply_pseudo);
				if(res == select_no_match)
				{
					return select_no_match;
				} else
				{
					if(right_res != select_match_pseudo_class)
					{
						right_res |= res;
					}
				}
			}
			break;
		case combinator_adjacent_sibling:
			{
				bool is_pseudo = false;
				element::ptr res = el_parent->find_adjacent_sibling(shared_from_this(), *selector.m_left, apply_pseudo, &is_pseudo);
				if(!res)
				{
					return select_no_match;
				} else
				{
					if(is_pseudo)
					{
						right_res |= select_match_pseudo_class;
					}
				}
			}
			break;
		case combinator_general_sibling:
			{
				bool is_pseudo = false;
				element::ptr res =  el_parent->find_sibling(shared_from_this(), *selector.m_left, apply_pseudo, &is_pseudo);
				if(!res)
				{
					return select_no_match;
				} else
				{
					if(is_pseudo)
					{
						right_res |= select_match_pseudo_class;
					}
				}
			}
			break;
		default:
			right_res = select_no_match;
		}
	}
	return right_res;
}

int litehtml::html_tag::select(const css_element_selector& selector, bool apply_pseudo)
{
	if(!selector.m_tag.empty() && selector.m_tag != _t("*"))
	{
		if(selector.m_tag != m_tag)
		{
			return select_no_match;
		}
	}

	int res = select_match;
	element::ptr el_parent = parent();

	for(const auto& attr : selector.m_attrs)
	{
		const tchar_t* attr_value = get_attr(attr.attribute.c_str());
		switch(attr.condition)
		{
		case select_exists:
			if(!attr_value)
			{
				return select_no_match;
			}
			break;
		case select_equal:
			if(!attr_value)
			{
				return select_no_match;
			} else 
			{
				if(attr.attribute == _t("class"))
				{
					const string_vector & tokens1 = m_class_values;
					const string_vector & tokens2 = attr.class_val;
					bool found = true;
					for(const auto& str1 : tokens2)
					{
						bool f = false;
						for(const auto& str2 : tokens1)
						{
							if( !t_strcasecmp(str1.c_str(), str2.c_str()) )
							{
								f = true;
							}
						}
						if(!f)
						{
							found = false;
						}
					}
					if(!found)
					{
						return select_no_match;
					}
				} else
				{
					if( t_strcasecmp(attr.val.c_str(), attr_value) )
					{
						return select_no_match;
					}
				}
			}
			break;
		case select_contain_str:
			if(!attr_value || !t_strstr(attr_value, attr.val.c_str()))
			{
				return select_no_match;
			}
			break;
		case select_start_str:
			if(!attr_value || t_strncmp(attr_value, attr.val.c_str(), attr.val.length()))
			{
				return select_no_match;
			}
			break;
		case select_end_str:
			if(!attr_value)
			{
				return select_no_match;
			} else if(t_strncmp(attr_value, attr.val.c_str(), attr.val.length()))
			{
				const tchar_t* s = attr_value + t_strlen(attr_value) - attr.val.length() - 1;
				if(s < attr_value)
				{
					return select_no_match;
				}
				if(attr.val != s)
				{
					return select_no_match;
				}
			}
			break;
		case select_pseudo_element:
			if(attr.val == _t("after"))
			{
				res |= select_match_with_after;
			} else if(attr.val == _t("before"))
			{
				res |= select_match_with_before;
			} else
			{
				return select_no_match;
			}
			break;
		case select_pseudo_class:
			if(apply_pseudo)
			{
				if (!el_parent) return select_no_match;

				tstring selector_param;
				tstring	selector_name;

				tstring::size_type begin	= attr.val.find_first_of(_t('('));
				tstring::size_type end		= (begin == tstring::npos) ? tstring::npos : find_close_bracket(attr.val, begin);
				if(begin != tstring::npos && end != tstring::npos)
				{
					selector_param = attr.val.substr(begin + 1, end - begin - 1);
				}
				if(begin != tstring::npos)
				{
					selector_name = attr.val.substr(0, begin);
					litehtml::trim(selector_name);
				} else
				{
					selector_name = attr.val;
				}

				int pseudo_selector = value_index(selector_name, pseudo_class_strings);
				
				switch(pseudo_selector)
				{
				case pseudo_class_only_child:
					if (!el_parent->is_only_child(shared_from_this(), false))
					{
						return select_no_match;
					}
					break;
				case pseudo_class_only_of_type:
					if (!el_parent->is_only_child(shared_from_this(), true))
					{
						return select_no_match;
					}
					break;
				case pseudo_class_first_child:
					if (!el_parent->is_nth_child(shared_from_this(), 0, 1, false))
					{
						return select_no_match;
					}
					break;
				case pseudo_class_first_of_type:
					if (!el_parent->is_nth_child(shared_from_this(), 0, 1, true))
					{
						return select_no_match;
					}
					break;
				case pseudo_class_last_child:
					if (!el_parent->is_nth_last_child(shared_from_this(), 0, 1, false))
					{
						return select_no_match;
					}
					break;
				case pseudo_class_last_of_type:
					if (!el_parent->is_nth_last_child(shared_from_this(), 0, 1, true))
					{
						return select_no_match;
					}
					break;
				case pseudo_class_nth_child:
				case pseudo_class_nth_of_type:
				case pseudo_class_nth_last_child:
				case pseudo_class_nth_last_of_type:
					{
						if(selector_param.empty()) return select_no_match;

						int num = 0;
						int off = 0;

						parse_nth_child_params(selector_param, num, off);
						if(!num && !off) return select_no_match;
						switch(pseudo_selector)
						{
						case pseudo_class_nth_child:
							if (!el_parent->is_nth_child(shared_from_this(), num, off, false))
							{
								return select_no_match;
							}
							break;
						case pseudo_class_nth_of_type:
							if (!el_parent->is_nth_child(shared_from_this(), num, off, true))
							{
								return select_no_match;
							}
							break;
						case pseudo_class_nth_last_child:
							if (!el_parent->is_nth_last_child(shared_from_this(), num, off, false))
							{
								return select_no_match;
							}
							break;
						case pseudo_class_nth_last_of_type:
							if (!el_parent->is_nth_last_child(shared_from_this(), num, off, true))
							{
								return select_no_match;
							}
							break;
						}

					}
					break;
				case pseudo_class_not:
					{
						css_element_selector sel;
						sel.parse(selector_param);
						if(select(sel, apply_pseudo))
						{
							return select_no_match;
						}
					}
					break;
				case pseudo_class_lang:
					{
						trim( selector_param );

						if( !get_document()->match_lang( selector_param ) )
						{
							return select_no_match;
						}
					}
					break;
				default:
					if(std::find(m_pseudo_classes.begin(), m_pseudo_classes.end(), attr.val) == m_pseudo_classes.end())
					{
						return select_no_match;
					}
					break;
				}
			} else
			{
				res |= select_match_pseudo_class;
			}
			break;
		}
	}
	return res;
}

litehtml::element::ptr litehtml::html_tag::find_ancestor(const css_selector& selector, bool apply_pseudo, bool* is_pseudo)
{
	element::ptr el_parent = parent();
	if (!el_parent)
	{
		return nullptr;
	}
	int res = el_parent->select(selector, apply_pseudo);
	if(res != select_no_match)
	{
		if(is_pseudo)
		{
			if(res & select_match_pseudo_class)
			{
				*is_pseudo = true;
			} else
			{
				*is_pseudo = false;
			}
		}
		return el_parent;
	}
	return el_parent->find_ancestor(selector, apply_pseudo, is_pseudo);
}

int litehtml::html_tag::get_floats_height(element_float el_float) const
{
	if(is_floats_holder())
	{
		int h = 0;

		bool process = false;

		for(const auto& fb : m_floats_left)
		{
			process = false;
			switch(el_float)
			{
			case float_none:
				process = true;
				break;
			case float_left:
				if (fb.clear_floats == clear_left || fb.clear_floats == clear_both)
				{
					process = true;
				}
				break;
			case float_right:
				if (fb.clear_floats == clear_right || fb.clear_floats == clear_both)
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
			process = false;
			switch(el_float)
			{
			case float_none:
				process = true;
				break;
			case float_left:
				if (fb.clear_floats == clear_left || fb.clear_floats == clear_both)
				{
					process = true;
				}
				break;
			case float_right:
				if (fb.clear_floats == clear_right || fb.clear_floats == clear_both)
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

		return h;
	}
	element::ptr el_parent = parent();
	if (el_parent)
	{
		int h = el_parent->get_floats_height(el_float);
		return h - m_pos.y;
	}
	return 0;
}

int litehtml::html_tag::get_left_floats_height() const
{
	if(is_floats_holder())
	{
		int h = 0;
		if(!m_floats_left.empty())
		{
			for (const auto& fb : m_floats_left)
			{
				h = std::max(h, fb.pos.bottom());
			}
		}
		return h;
	}
	element::ptr el_parent = parent();
	if (el_parent)
	{
		int h = el_parent->get_left_floats_height();
		return h - m_pos.y;
	}
	return 0;
}

int litehtml::html_tag::get_right_floats_height() const
{
	if(is_floats_holder())
	{
		int h = 0;
		if(!m_floats_right.empty())
		{
			for(const auto& fb : m_floats_right)
			{
				h = std::max(h, fb.pos.bottom());
			}
		}
		return h;
	}
	element::ptr el_parent = parent();
	if (el_parent)
	{
		int h = el_parent->get_right_floats_height();
		return h - m_pos.y;
	}
	return 0;
}

int litehtml::html_tag::get_line_left( int y )
{
	if(is_floats_holder())
	{
		if(m_cahe_line_left.is_valid && m_cahe_line_left.hash == y)
		{
			return m_cahe_line_left.val;
		}

		int w = 0;
		for(const auto& fb : m_floats_left)
		{
			if (y >= fb.pos.top() && y < fb.pos.bottom())
			{
				w = std::max(w, fb.pos.right());
				if (w < fb.pos.right())
				{
					break;
				}
			}
		}
		m_cahe_line_left.set_value(y, w);
		return w;
	}
	element::ptr el_parent = parent();
	if (el_parent)
	{
		int w = el_parent->get_line_left(y + m_pos.y);
		if (w < 0)
		{
			w = 0;
		}
		return w - (w ? m_pos.x : 0);
	}
	return 0;
}

int litehtml::html_tag::get_line_right( int y, int def_right )
{
	if(is_floats_holder())
	{
		if(m_cahe_line_right.is_valid && m_cahe_line_right.hash == y)
		{
			if(m_cahe_line_right.is_default)
			{
				return def_right;
			} else
			{
				return std::min(m_cahe_line_right.val, def_right);
			}
		}

		int w = def_right;
		m_cahe_line_right.is_default = true;
		for(const auto& fb : m_floats_right)
		{
			if(y >= fb.pos.top() && y < fb.pos.bottom())
			{
				w = std::min(w, fb.pos.left());
				m_cahe_line_right.is_default = false;
				if(w > fb.pos.left())
				{
					break;
				}
			}
		}
		m_cahe_line_right.set_value(y, w);
		return w;
	}
	element::ptr el_parent = parent();
	if (el_parent)
	{
		int w = el_parent->get_line_right(y + m_pos.y, def_right + m_pos.x);
		return w - m_pos.x;
	}
	return 0;
}


void litehtml::html_tag::get_line_left_right( int y, int def_right, int& ln_left, int& ln_right )
{
	if(is_floats_holder())
	{
		ln_left		= get_line_left(y);
		ln_right	= get_line_right(y, def_right);
	} else
	{
		element::ptr el_parent = parent();
		if (el_parent)
		{
			el_parent->get_line_left_right(y + m_pos.y, def_right + m_pos.x, ln_left, ln_right);
		}
		ln_right -= m_pos.x;

		if(ln_left < 0)
		{
			ln_left = 0;
		} else if (ln_left > 0)
		{
			ln_left -= m_pos.x;
			if (ln_left < 0)
			{
				ln_left = 0;
			}
        }
	}
}

int litehtml::html_tag::fix_line_width( int max_width, element_float flt )
{
	int ret_width = 0;
	if(!m_boxes.empty())
	{
		elements_vector els;
		m_boxes.back()->get_elements(els);
		bool was_cleared = false;
		if(!els.empty() && els.front()->get_clear() != clear_none)
		{
			if(els.front()->get_clear() == clear_both)
			{
				was_cleared = true;
			} else
			{
				if(	(flt == float_left	&& els.front()->get_clear() == clear_left) ||
					(flt == float_right	&& els.front()->get_clear() == clear_right) )
				{
					was_cleared = true;
				}
			}
		}

		if(!was_cleared)
		{
			m_boxes.pop_back();

			for(const auto& el : els)
			{
				int rw = place_element(el, max_width);
				if(rw > ret_width)
				{
					ret_width = rw;
				}
			}
		} else
		{
			int line_top = 0;
			if(m_boxes.back()->get_type() == box_line)
			{
				line_top = m_boxes.back()->top();
			} else
			{
				line_top = m_boxes.back()->bottom();
			}

			int line_left	= 0;
			int line_right	= max_width;
			get_line_left_right(line_top, max_width, line_left, line_right);

			if(m_boxes.back()->get_type() == box_line)
			{
				if(m_boxes.size() == 1 && m_list_style_type != list_style_type_none && m_list_style_position == list_style_position_inside)
				{
					int sz_font = get_font_size();
					line_left += sz_font;
				}

				if(m_css_text_indent.val() != 0)
				{
					bool line_box_found = false;
					for(const auto& box : m_boxes)
					{
						if(box->get_type() == box_line)
						{
							line_box_found = true;
							break;
						}
					}
					if(!line_box_found)
					{
						line_left += m_css_text_indent.calc_percent(max_width);
					}
				}

			}

			els.clear();
			m_boxes.back()->new_width(line_left, line_right, els);
			for(auto& el : els)
			{
				int rw = place_element(el, max_width);
				if(rw > ret_width)
				{
					ret_width = rw;
				}
			}
		}
	}

	return ret_width;
}

void litehtml::html_tag::add_float(const element::ptr &el, int x, int y)
{
	if(is_floats_holder())
	{
		floated_box fb;
		fb.pos.x		= el->left() + x;
		fb.pos.y		= el->top()  + y;
		fb.pos.width	= el->width();
		fb.pos.height	= el->height();
		fb.float_side	= el->get_float();
		fb.clear_floats	= el->get_clear();
		fb.el			= el;

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
			m_cahe_line_left.invalidate();
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
			m_cahe_line_right.invalidate();
		}
	} else
	{
		element::ptr el_parent = parent();
		if (el_parent)
		{
			el_parent->add_float(el, x + m_pos.x, y + m_pos.y);
		}
	}
}

int litehtml::html_tag::find_next_line_top( int top, int width, int def_right )
{
	if(is_floats_holder())
	{
		int new_top = top;
		int_vector points;

		for(const auto& fb : m_floats_left)
		{
			if(fb.pos.top() >= top)
			{
				if(find(points.begin(), points.end(), fb.pos.top()) == points.end())
				{
					points.push_back(fb.pos.top());
				}
			}
			if (fb.pos.bottom() >= top)
			{
				if (find(points.begin(), points.end(), fb.pos.bottom()) == points.end())
				{
					points.push_back(fb.pos.bottom());
				}
			}
		}

		for (const auto& fb : m_floats_right)
		{
			if (fb.pos.top() >= top)
			{
				if (find(points.begin(), points.end(), fb.pos.top()) == points.end())
				{
					points.push_back(fb.pos.top());
				}
			}
			if (fb.pos.bottom() >= top)
			{
				if (find(points.begin(), points.end(), fb.pos.bottom()) == points.end())
				{
					points.push_back(fb.pos.bottom());
				}
			}
		}

		if(!points.empty())
		{
			sort(points.begin(), points.end(), std::less<int>( ));
			new_top = points.back();

			for(auto pt : points)
			{
				int pos_left	= 0;
				int pos_right	= def_right;
				get_line_left_right(pt, def_right, pos_left, pos_right);

				if(pos_right - pos_left >= width)
				{
					new_top = pt;
					break;
				}
			}
		}
		return new_top;
	}
	element::ptr el_parent = parent();
	if (el_parent)
	{
		int new_top = el_parent->find_next_line_top(top + m_pos.y, width, def_right + m_pos.x);
		return new_top - m_pos.y;
	}
	return 0;
}

void litehtml::html_tag::parse_background()
{
	// parse background-color
	m_bg.m_color		= get_color(_t("background-color"), false, web_color(0, 0, 0, 0));

	// parse background-position
	const tchar_t* str = get_style_property(_t("background-position"), false, _t("0% 0%"));
	if(str)
	{
		string_vector res;
		split_string(str, res, _t(" \t"));
		if(!res.empty())
		{
			if(res.size() == 1)
			{
				if( value_in_list(res[0], _t("left;right;center")) )
				{
					m_bg.m_position.x.fromString(res[0], _t("left;right;center"));
					m_bg.m_position.y.set_value(50, css_units_percentage);
				} else if( value_in_list(res[0], _t("top;bottom;center")) )
				{
					m_bg.m_position.y.fromString(res[0], _t("top;bottom;center"));
					m_bg.m_position.x.set_value(50, css_units_percentage);
				} else
				{
					m_bg.m_position.x.fromString(res[0], _t("left;right;center"));
					m_bg.m_position.y.set_value(50, css_units_percentage);
				}
			} else
			{
				if(value_in_list(res[0], _t("left;right")))
				{
					m_bg.m_position.x.fromString(res[0], _t("left;right;center"));
					m_bg.m_position.y.fromString(res[1], _t("top;bottom;center"));
				} else if(value_in_list(res[0], _t("top;bottom")))
				{
					m_bg.m_position.x.fromString(res[1], _t("left;right;center"));
					m_bg.m_position.y.fromString(res[0], _t("top;bottom;center"));
				} else if(value_in_list(res[1], _t("left;right")))
				{
					m_bg.m_position.x.fromString(res[1], _t("left;right;center"));
					m_bg.m_position.y.fromString(res[0], _t("top;bottom;center"));
				}else if(value_in_list(res[1], _t("top;bottom")))
				{
					m_bg.m_position.x.fromString(res[0], _t("left;right;center"));
					m_bg.m_position.y.fromString(res[1], _t("top;bottom;center"));
				} else
				{
					m_bg.m_position.x.fromString(res[0], _t("left;right;center"));
					m_bg.m_position.y.fromString(res[1], _t("top;bottom;center"));
				}
			}

			if(m_bg.m_position.x.is_predefined())
			{
				switch(m_bg.m_position.x.predef())
				{
				case 0:
					m_bg.m_position.x.set_value(0, css_units_percentage);
					break;
				case 1:
					m_bg.m_position.x.set_value(100, css_units_percentage);
					break;
				case 2:
					m_bg.m_position.x.set_value(50, css_units_percentage);
					break;
				}
			}
			if(m_bg.m_position.y.is_predefined())
			{
				switch(m_bg.m_position.y.predef())
				{
				case 0:
					m_bg.m_position.y.set_value(0, css_units_percentage);
					break;
				case 1:
					m_bg.m_position.y.set_value(100, css_units_percentage);
					break;
				case 2:
					m_bg.m_position.y.set_value(50, css_units_percentage);
					break;
				}
			}
		} else
		{
			m_bg.m_position.x.set_value(0, css_units_percentage);
			m_bg.m_position.y.set_value(0, css_units_percentage);
		}
	} else
	{
		m_bg.m_position.y.set_value(0, css_units_percentage);
		m_bg.m_position.x.set_value(0, css_units_percentage);
	}

	str = get_style_property(_t("background-size"), false, _t("auto"));
	if(str)
	{
		string_vector res;
		split_string(str, res, _t(" \t"));
		if(!res.empty())
		{
			m_bg.m_position.width.fromString(res[0], background_size_strings);
			if(res.size() > 1)
			{
				m_bg.m_position.height.fromString(res[1], background_size_strings);
			} else
			{
				m_bg.m_position.height.predef(background_size_auto);
			}
		} else
		{
			m_bg.m_position.width.predef(background_size_auto);
			m_bg.m_position.height.predef(background_size_auto);
		}
	}

	document::ptr doc = get_document();

	doc->cvt_units(m_bg.m_position.x,		m_font_size);
	doc->cvt_units(m_bg.m_position.y,		m_font_size);
	doc->cvt_units(m_bg.m_position.width,	m_font_size);
	doc->cvt_units(m_bg.m_position.height,	m_font_size);

	// parse background_attachment
	m_bg.m_attachment = (background_attachment) value_index(
		get_style_property(_t("background-attachment"), false, _t("scroll")), 
		background_attachment_strings, 
		background_attachment_scroll);

	// parse background_attachment
	m_bg.m_repeat = (background_repeat) value_index(
		get_style_property(_t("background-repeat"), false, _t("repeat")), 
		background_repeat_strings, 
		background_repeat_repeat);

	// parse background_clip
	m_bg.m_clip = (background_box) value_index(
		get_style_property(_t("background-clip"), false, _t("border-box")), 
		background_box_strings, 
		background_box_border);

	// parse background_origin
	m_bg.m_origin = (background_box) value_index(
		get_style_property(_t("background-origin"), false, _t("padding-box")), 
		background_box_strings, 
		background_box_content);

	// parse background-image
	css::parse_css_url(get_style_property(_t("background-image"), false, _t("")), m_bg.m_image);
	m_bg.m_baseurl = get_style_property(_t("background-image-baseurl"), false, _t(""));

	if(!m_bg.m_image.empty())
	{
		doc->container()->load_image(m_bg.m_image.c_str(), m_bg.m_baseurl.empty() ? nullptr : m_bg.m_baseurl.c_str(), true);
	}
}

void litehtml::html_tag::add_positioned(const element::ptr &el)
{
	if (m_el_position != element_position_static || (!have_parent()))
	{
		m_positioned.push_back(el);
	} else
	{
		element::ptr el_parent = parent();
		if (el_parent)
		{
			el_parent->add_positioned(el);
		}
	}
}

void litehtml::html_tag::calc_outlines( int parent_width )
{
	m_padding.left	= m_css_padding.left.calc_percent(parent_width);
	m_padding.right	= m_css_padding.right.calc_percent(parent_width);

	m_borders.left	= m_css_borders.left.width.calc_percent(parent_width);
	m_borders.right	= m_css_borders.right.width.calc_percent(parent_width);

	m_margins.left	= m_css_margins.left.calc_percent(parent_width);
	m_margins.right	= m_css_margins.right.calc_percent(parent_width);

	m_margins.top		= m_css_margins.top.calc_percent(parent_width);
	m_margins.bottom	= m_css_margins.bottom.calc_percent(parent_width);

	m_padding.top		= m_css_padding.top.calc_percent(parent_width);
	m_padding.bottom	= m_css_padding.bottom.calc_percent(parent_width);
}

void litehtml::html_tag::calc_auto_margins(int parent_width)
{
	if (get_element_position() != element_position_absolute && (m_display == display_block || m_display == display_table))
	{
		if (m_css_margins.left.is_predefined() && m_css_margins.right.is_predefined())
		{
			int el_width = m_pos.width + m_borders.left + m_borders.right + m_padding.left + m_padding.right;
			if (el_width <= parent_width)
			{
				m_margins.left = (parent_width - el_width) / 2;
				m_margins.right = (parent_width - el_width) - m_margins.left;
			}
			else
			{
				m_margins.left = 0;
				m_margins.right = 0;
			}
		}
		else if (m_css_margins.left.is_predefined() && !m_css_margins.right.is_predefined())
		{
			int el_width = m_pos.width + m_borders.left + m_borders.right + m_padding.left + m_padding.right + m_margins.right;
			m_margins.left = parent_width - el_width;
			if (m_margins.left < 0) m_margins.left = 0;
		}
		else if (!m_css_margins.left.is_predefined() && m_css_margins.right.is_predefined())
		{
			int el_width = m_pos.width + m_borders.left + m_borders.right + m_padding.left + m_padding.right + m_margins.left;
			m_margins.right = parent_width - el_width;
			if (m_margins.right < 0) m_margins.right = 0;
		}
	}
}

void litehtml::html_tag::parse_attributes()
{
	for(auto& el : m_children)
	{
		el->parse_attributes();
	}
}

void litehtml::html_tag::get_text( tstring& text )
{
	for (auto& el : m_children)
	{
		el->get_text(text);
	}
}

bool litehtml::html_tag::is_body()  const
{
	return false;
}

void litehtml::html_tag::set_data( const tchar_t* /*data*/ )
{

}

void litehtml::html_tag::get_inline_boxes( position::vector& boxes )
{
	litehtml::box* old_box = nullptr;
	position pos;
	for(auto& el : m_children)
	{
		if(!el->skip())
		{
			if(el->m_box)
			{
				if(el->m_box != old_box)
				{
					if(old_box)
					{
						if(boxes.empty())
						{
							pos.x		-= m_padding.left + m_borders.left;
							pos.width	+= m_padding.left + m_borders.left;
						}
						boxes.push_back(pos);
					}
					old_box		= el->m_box;
					pos.x		= el->left() + el->margin_left();
					pos.y		= el->top() - m_padding.top - m_borders.top;
					pos.width	= 0;
					pos.height	= 0;
				}
				pos.width	= el->right() - pos.x - el->margin_right() - el->margin_left();
				pos.height	= std::max(pos.height, el->height() + m_padding.top + m_padding.bottom + m_borders.top + m_borders.bottom);
			} else if(el->get_display() == display_inline)
			{
				position::vector sub_boxes;
				el->get_inline_boxes(sub_boxes);
				if(!sub_boxes.empty())
				{
					sub_boxes.rbegin()->width += el->margin_right();
					if(boxes.empty())
					{
						if(m_padding.left + m_borders.left > 0)
						{
							position padding_box = (*sub_boxes.begin());
							padding_box.x		-= m_padding.left + m_borders.left + el->margin_left();
							padding_box.width	= m_padding.left + m_borders.left + el->margin_left();
							boxes.push_back(padding_box);
						}
					}

					sub_boxes.rbegin()->width += el->margin_right();

					boxes.insert(boxes.end(), sub_boxes.begin(), sub_boxes.end());
				}
			}
		}
	}
	if(pos.width || pos.height)
	{
		if(boxes.empty())
		{
			pos.x		-= m_padding.left + m_borders.left;
			pos.width	+= m_padding.left + m_borders.left;
		}
		boxes.push_back(pos);
	}
	if(!boxes.empty())
	{
		if(m_padding.right + m_borders.right > 0)
		{
			boxes.back().width += m_padding.right + m_borders.right;
		}
	}
}

bool litehtml::html_tag::on_mouse_over()
{
	bool ret = false;

	element::ptr el = shared_from_this();
	while(el)
	{
		if(el->set_pseudo_class(_t("hover"), true))
		{
			ret = true;
		}
		el = el->parent();
	}

	return ret;
}

bool litehtml::html_tag::find_styles_changes( position::vector& redraw_boxes, int x, int y )
{
	if(m_display == display_inline_text)
	{
		return false;
	}

	bool ret = false;
	bool apply = false;
	for (const auto& used_style : m_used_styles)
	{
		if(used_style->m_selector->is_media_valid())
		{
			int res = select(*(used_style->m_selector), true);
			if( (res == select_no_match && used_style->m_used) || (res == select_match && !used_style->m_used) )
			{
				apply = true;
                break;
			}
		}
	}

	if(apply)
	{
		if(m_display == display_inline ||  m_display == display_table_row)
		{
			position::vector boxes;
			get_inline_boxes(boxes);
			for(auto & box : boxes)
			{
                box.x	+= x;
                box.y	+= y;
				redraw_boxes.push_back(box);
			}
		} else
		{
			position pos = m_pos;
			if(m_el_position != element_position_fixed)
			{
				pos.x += x;
				pos.y += y;
			}
			pos += m_padding;
			pos += m_borders;
			redraw_boxes.push_back(pos);
		}

		ret = true;
		refresh_styles();
		parse_styles();
	}
	for (auto& el : m_children)
	{
		if(!el->skip())
		{
			if(m_el_position != element_position_fixed)
			{
				if(el->find_styles_changes(redraw_boxes, x + m_pos.x, y + m_pos.y))
				{
					ret = true;
				}
			} else
			{
				if(el->find_styles_changes(redraw_boxes, m_pos.x, m_pos.y))
				{
					ret = true;
				}
			}
		}
	}
	return ret;
}

bool litehtml::html_tag::on_mouse_leave()
{
	bool ret = false;

	element::ptr el = shared_from_this();
	while(el)
	{
		if(el->set_pseudo_class(_t("hover"), false))
		{
			ret = true;
		}
		if(el->set_pseudo_class(_t("active"), false))
		{
			ret = true;
		}
		el = el->parent();
	}

	return ret;
}

bool litehtml::html_tag::on_lbutton_down()
{
    bool ret = false;

	element::ptr el = shared_from_this();
    while (el)
    {
        if (el->set_pseudo_class(_t("active"), true))
        {
            ret = true;
        }
        el = el->parent();
    }

    return ret;
}

bool litehtml::html_tag::on_lbutton_up()
{
	bool ret = false;

	element::ptr el = shared_from_this();
    while (el)
    {
        if (el->set_pseudo_class(_t("active"), false))
        {
            ret = true;
        }
        el = el->parent();
    }

    on_click();

	return ret;
}

void litehtml::html_tag::on_click()
{
	if (have_parent())
	{
		element::ptr el_parent = parent();
		if (el_parent)
		{
			el_parent->on_click();
		}
	}
}

const litehtml::tchar_t* litehtml::html_tag::get_cursor()
{
	return get_style_property(_t("cursor"), true, nullptr);
}

static const int font_size_table[8][7] =
{
	{ 9,    9,     9,     9,    11,    14,    18},
	{ 9,    9,     9,    10,    12,    15,    20},
	{ 9,    9,     9,    11,    13,    17,    22},
	{ 9,    9,    10,    12,    14,    18,    24},
	{ 9,    9,    10,    13,    16,    20,    26},
	{ 9,    9,    11,    14,    17,    21,    28},
	{ 9,   10,    12,    15,    17,    23,    30},
	{ 9,   10,    13,    16,    18,    24,    32}
};


void litehtml::html_tag::init_font()
{
	// initialize font size
	const tchar_t* str = get_style_property(_t("font-size"), false, nullptr);

	int parent_sz = 0;
	int doc_font_size = get_document()->container()->get_default_font_size();
	element::ptr el_parent = parent();
	if (el_parent)
	{
		parent_sz = el_parent->get_font_size();
	} else
	{
		parent_sz = doc_font_size;
	}


	if(!str)
	{
		m_font_size = parent_sz;
	} else
	{
		m_font_size = parent_sz;

		css_length sz;
		sz.fromString(str, font_size_strings);
		if(sz.is_predefined())
		{
			int idx_in_table = doc_font_size - 9;
			if(idx_in_table >= 0 && idx_in_table <= 7)
			{
				if(sz.predef() >= fontSize_xx_small && sz.predef() <= fontSize_xx_large)
				{
					m_font_size = font_size_table[idx_in_table][sz.predef()];
				} else
				{
					m_font_size = doc_font_size;
				}
			} else			
			{
				switch(sz.predef())
				{
				case fontSize_xx_small:
					m_font_size = doc_font_size * 3 / 5;
					break;
				case fontSize_x_small:
					m_font_size = doc_font_size * 3 / 4;
					break;
				case fontSize_small:
					m_font_size = doc_font_size * 8 / 9;
					break;
				case fontSize_large:
					m_font_size = doc_font_size * 6 / 5;
					break;
				case fontSize_x_large:
					m_font_size = doc_font_size * 3 / 2;
					break;
				case fontSize_xx_large:
					m_font_size = doc_font_size * 2;
					break;
				default:
					m_font_size = doc_font_size;
					break;
				}
			}
		} else
		{
			if(sz.units() == css_units_percentage)
			{
				m_font_size = sz.calc_percent(parent_sz);
			} else if(sz.units() == css_units_none)
			{
				m_font_size = parent_sz;
			} else
			{
				m_font_size = get_document()->cvt_units(sz, parent_sz);
			}
		}
	}

	// initialize font
	const tchar_t* name			= get_style_property(_t("font-family"),		true,	_t("inherit"));
	const tchar_t* weight		= get_style_property(_t("font-weight"),		true,	_t("normal"));
	const tchar_t* style		= get_style_property(_t("font-style"),		true,	_t("normal"));
	const tchar_t* decoration	= get_style_property(_t("text-decoration"),	true,	_t("none"));

	m_font = get_document()->get_font(name, m_font_size, weight, style, decoration, &m_font_metrics);
}

bool litehtml::html_tag::is_break() const
{
	return false;
}

void litehtml::html_tag::set_tagName( const tchar_t* tag )
{
	m_tag = tag;
	for (tchar_t& i : m_tag)
	{
		i = std::tolower(i, std::locale::classic());
	}
}

void litehtml::html_tag::draw_background( uint_ptr hdc, int x, int y, const position* clip )
{
	position pos = m_pos;
	pos.x	+= x;
	pos.y	+= y;

	position el_pos = pos;
	el_pos += m_padding;
	el_pos += m_borders;

	if(m_display != display_inline && m_display != display_table_row)
	{
		if(el_pos.does_intersect(clip))
		{
			if (m_grid)
			{
				int captions_height = m_grid->captions_height();
				pos.y += captions_height;
				pos.height -= captions_height;
			}

			const background* bg = get_background();
			if(bg)
			{
				background_paint bg_paint;
				init_background_paint(pos, bg_paint, bg);

				get_document()->container()->draw_background(hdc, bg_paint);
			}
			position border_box = pos;
			border_box += m_padding;
			border_box += m_borders;

			borders bdr = m_css_borders;
            if(bdr.is_visible())
            {
                bdr.radius = m_css_borders.radius.calc_percents(border_box.width, border_box.height);
                get_document()->container()->draw_borders(hdc, bdr, border_box, !have_parent());
            }
		}
	} else
	{
		const background* bg = get_background();

		position::vector boxes;
		get_inline_boxes(boxes);

		background_paint bg_paint;
		position content_box;

		for(auto box = boxes.begin(); box != boxes.end(); box++)
		{
			box->x	+= x;
			box->y	+= y;

			if(box->does_intersect(clip))
			{
				content_box = *box;
				content_box -= m_borders;
				content_box -= m_padding;

				if(bg)
				{
					init_background_paint(content_box, bg_paint, bg);
				}

				css_borders bdr;

				// set left borders radius for the first box
				if(box == boxes.begin())
				{
					bdr.radius.bottom_left_x	= m_css_borders.radius.bottom_left_x;
					bdr.radius.bottom_left_y	= m_css_borders.radius.bottom_left_y;
					bdr.radius.top_left_x		= m_css_borders.radius.top_left_x;
					bdr.radius.top_left_y		= m_css_borders.radius.top_left_y;
				}

				// set right borders radius for the last box
				if(box == boxes.end() - 1)
				{
					bdr.radius.bottom_right_x	= m_css_borders.radius.bottom_right_x;
					bdr.radius.bottom_right_y	= m_css_borders.radius.bottom_right_y;
					bdr.radius.top_right_x		= m_css_borders.radius.top_right_x;
					bdr.radius.top_right_y		= m_css_borders.radius.top_right_y;
				}

				
				bdr.top		= m_css_borders.top;
				bdr.bottom	= m_css_borders.bottom;
				if(box == boxes.begin())
				{
					bdr.left	= m_css_borders.left;
				}
				if(box == boxes.end() - 1)
				{
					bdr.right	= m_css_borders.right;
				}

				if(bg)
				{
					bg_paint.border_radius = bdr.radius.calc_percents(bg_paint.border_box.width, bg_paint.border_box.width);
					get_document()->container()->draw_background(hdc, bg_paint);
				}
                if(bdr.is_visible())
                {
                    borders b = bdr;
                    b.radius = bdr.radius.calc_percents(box->width, box->height);
                    get_document()->container()->draw_borders(hdc, b, *box, false);
                }
            }
		}
	}
}

int litehtml::html_tag::render_inline(const element::ptr &container, int max_width)
{
	int ret_width = 0;
	int rw = 0;

	white_space ws = get_white_space();
	bool skip_spaces = false;
	if (ws == white_space_normal ||
		ws == white_space_nowrap ||
		ws == white_space_pre_line)
	{
		skip_spaces = true;
	}
	bool was_space = false;

	for (auto& el : m_children)
	{
		// skip spaces to make rendering a bit faster
		if (skip_spaces)
		{
			if (el->is_white_space())
			{
				if (was_space)
				{
					el->skip(true);
					continue;
				}
				else
				{
					was_space = true;
				}
			}
			else
			{
				was_space = false;
			}
		}

		rw = container->place_element( el, max_width );
		if(rw > ret_width)
		{
			ret_width = rw;
		}
	}
	return ret_width;
}

int litehtml::html_tag::place_element(const element::ptr &el, int max_width)
{
	if(el->get_display() == display_none) return 0;

	if(el->get_display() == display_inline)
	{
		return el->render_inline(shared_from_this(), max_width);
	}

	element_position el_position = el->get_element_position();

	if(el_position == element_position_absolute || el_position == element_position_fixed)
	{
		int line_top = 0;
		if(!m_boxes.empty())
		{
			if(m_boxes.back()->get_type() == box_line)
			{
				line_top = m_boxes.back()->top();
				if(!m_boxes.back()->is_empty())
				{
					line_top += line_height();
				}
			} else
			{
				line_top = m_boxes.back()->bottom();
			}
		}

		el->render(0, line_top, max_width);
		el->m_pos.x	+= el->content_margins_left();
		el->m_pos.y	+= el->content_margins_top();

		return 0;
	}

	int ret_width = 0;

	switch(el->get_float())
	{
	case float_left:
		{
			int line_top = 0;
			if(!m_boxes.empty())
			{
				if(m_boxes.back()->get_type() == box_line)
				{
					line_top = m_boxes.back()->top();
				} else
				{
					line_top = m_boxes.back()->bottom();
				}
			}
			line_top		= get_cleared_top(el, line_top);
			int line_left	= 0;
			int line_right	= max_width;
			get_line_left_right(line_top, max_width, line_left, line_right);

			el->render(line_left, line_top, line_right);
			if(el->right() > line_right)
			{
				int new_top = find_next_line_top(el->top(), el->width(), max_width);
				el->m_pos.x = get_line_left(new_top) + el->content_margins_left();
				el->m_pos.y = new_top + el->content_margins_top();
			}
			add_float(el, 0, 0);
			ret_width = fix_line_width(max_width, float_left);
			if(!ret_width)
			{
				ret_width = el->right();
			}
		}
		break;
	case float_right:
		{
			int line_top = 0;
			if(!m_boxes.empty())
			{
				if(m_boxes.back()->get_type() == box_line)
				{
					line_top = m_boxes.back()->top();
				} else
				{
					line_top = m_boxes.back()->bottom();
				}
			}
			line_top		= get_cleared_top(el, line_top);
			int line_left	= 0;
			int line_right	= max_width;
			get_line_left_right(line_top, max_width, line_left, line_right);

			el->render(0, line_top, line_right);

			if(line_left + el->width() > line_right)
			{
				int new_top = find_next_line_top(el->top(), el->width(), max_width);
				el->m_pos.x = get_line_right(new_top, max_width) - el->width() + el->content_margins_left();
				el->m_pos.y = new_top + el->content_margins_top();
			} else
			{
				el->m_pos.x = line_right - el->width() + el->content_margins_left();
			}
			add_float(el, 0, 0);
			ret_width = fix_line_width(max_width, float_right);

			if(!ret_width)
			{
				line_left	= 0;
				line_right	= max_width;
				get_line_left_right(line_top, max_width, line_left, line_right);

				ret_width = ret_width + (max_width - line_right);
			}
		}
		break;
	default:
		{
			line_context line_ctx = {0};
			line_ctx.top = 0;
			if (!m_boxes.empty())
			{
				line_ctx.top = m_boxes.back()->top();
			}
			line_ctx.left = 0;
			line_ctx.right = max_width;
			line_ctx.fix_top();
			get_line_left_right(line_ctx.top, max_width, line_ctx.left, line_ctx.right);

			switch(el->get_display())
			{
			case display_inline_block:
			case display_inline_table:
				ret_width = el->render(line_ctx.left, line_ctx.top, line_ctx.right);
				break;
			case display_block:		
				if(el->is_replaced() || el->is_floats_holder())
				{
					element::ptr el_parent = el->parent();
					el->m_pos.width = el->get_css_width().calc_percent(line_ctx.right - line_ctx.left);
					el->m_pos.height = el->get_css_height().calc_percent(el_parent ? el_parent->m_pos.height : 0);
				}
				el->calc_outlines(line_ctx.right - line_ctx.left);
				break;
			case display_inline_text:
				{
					litehtml::size sz;
					el->get_content_size(sz, line_ctx.right);
					el->m_pos = sz;
				}
				break;
			default:
				ret_width = 0;
				break;
			}

			bool add_box = true;
			if(!m_boxes.empty())
			{
				if(m_boxes.back()->can_hold(el, m_white_space))
				{
					add_box = false;
				}
			}
			if(add_box)
			{
				new_box(el, max_width, line_ctx);
			} else if(!m_boxes.empty())
			{
				line_ctx.top = m_boxes.back()->top();
			}

			if (line_ctx.top != line_ctx.calculatedTop)
			{
				line_ctx.left = 0;
				line_ctx.right = max_width;
				line_ctx.fix_top();
				get_line_left_right(line_ctx.top, max_width, line_ctx.left, line_ctx.right);
			}

			if(!el->is_inline_box())
			{
				if(m_boxes.size() == 1)
				{
					if(collapse_top_margin())
					{
						int shift = el->margin_top();
						if(shift >= 0)
						{
							line_ctx.top -= shift;
							m_boxes.back()->y_shift(-shift);
						}
					}
				} else
				{
					int shift = 0;
					int prev_margin = m_boxes[m_boxes.size() - 2]->bottom_margin();

					if(prev_margin > el->margin_top())
					{
						shift = el->margin_top();
					} else
					{
						shift = prev_margin;
					}
					if(shift >= 0)
					{
						line_ctx.top -= shift;
						m_boxes.back()->y_shift(-shift);
					}
				}
			}

			switch(el->get_display())
			{
			case display_table:
			case display_list_item:
				ret_width = el->render(line_ctx.left, line_ctx.top, line_ctx.width());
				break;
			case display_block:
			case display_table_cell:
			case display_table_caption:
			case display_table_row:
				if(el->is_replaced() || el->is_floats_holder())
				{
					ret_width = el->render(line_ctx.left, line_ctx.top, line_ctx.width()) + line_ctx.left + (max_width - line_ctx.right);
				} else
				{
					ret_width = el->render(0, line_ctx.top, max_width);
				}
				break;
			default:
				ret_width = 0;
				break;
			}

			m_boxes.back()->add_element(el);

			if(el->is_inline_box() && !el->skip())
			{
				ret_width = el->right() + (max_width - line_ctx.right);
			}
		}
		break;
	}

	return ret_width;
}

bool litehtml::html_tag::set_pseudo_class( const tchar_t* pclass, bool add )
{
	bool ret = false;
	if(add)
	{
		if(std::find(m_pseudo_classes.begin(), m_pseudo_classes.end(), pclass) == m_pseudo_classes.end())
		{
			m_pseudo_classes.push_back(pclass);
			ret = true;
		}
	} else
	{
		auto pi = std::find(m_pseudo_classes.begin(), m_pseudo_classes.end(), pclass);
		if(pi != m_pseudo_classes.end())
		{
			m_pseudo_classes.erase(pi);
			ret = true;
		}
	}
	return ret;
}

bool litehtml::html_tag::set_class( const tchar_t* pclass, bool add )
{
	string_vector classes;
	bool changed = false;

	split_string( pclass, classes, _t(" ") );

	if(add)
	{
		for( auto & _class : classes  )
		{
			if(std::find(m_class_values.begin(), m_class_values.end(), _class) == m_class_values.end())
			{
				m_class_values.push_back( std::move( _class ) );
				changed = true;
			}
		}
	} else
	{
		for( const auto & _class : classes )
		{
			auto end = std::remove(m_class_values.begin(), m_class_values.end(), _class);

			if(end != m_class_values.end())
			{
				m_class_values.erase(end, m_class_values.end());
				changed = true;
			}
		}
	}

	if( changed )
	{
		tstring class_string;
		join_string(class_string, m_class_values, _t(" "));
		set_attr(_t("class"), class_string.c_str());

		return true;
	}
	else
	{
		return false;
	}

}

int litehtml::html_tag::line_height() const
{
	return m_line_height;
}

bool litehtml::html_tag::is_replaced() const
{
	return false;
}

int litehtml::html_tag::finish_last_box(bool end_of_render)
{
	int line_top = 0;

	if(!m_boxes.empty())
	{
		m_boxes.back()->finish(end_of_render);

		if(m_boxes.back()->is_empty())
		{
			line_top = m_boxes.back()->top();
			m_boxes.pop_back();
		}

		if(!m_boxes.empty())
		{
			line_top = m_boxes.back()->bottom();
		}
	}
	return line_top;
}

int litehtml::html_tag::new_box(const element::ptr &el, int max_width, line_context& line_ctx)
{
	line_ctx.top = get_cleared_top(el, finish_last_box());

	line_ctx.left = 0;
	line_ctx.right = max_width;
	line_ctx.fix_top();
	get_line_left_right(line_ctx.top, max_width, line_ctx.left, line_ctx.right);

	if(el->is_inline_box() || el->is_floats_holder())
	{
		if (el->width() > line_ctx.right - line_ctx.left)
		{
			line_ctx.top = find_next_line_top(line_ctx.top, el->width(), max_width);
			line_ctx.left = 0;
			line_ctx.right = max_width;
			line_ctx.fix_top();
			get_line_left_right(line_ctx.top, max_width, line_ctx.left, line_ctx.right);
		}
	}

	int first_line_margin = 0;
	if(m_boxes.empty() && m_list_style_type != list_style_type_none && m_list_style_position == list_style_position_inside)
	{
		int sz_font = get_font_size();
		first_line_margin = sz_font;
	}

	if(el->is_inline_box())
	{
		int text_indent = 0;
		if(m_css_text_indent.val() != 0)
		{
			bool line_box_found = false;
			for(auto & box : m_boxes)
			{
				if(box->get_type() == box_line)
				{
					line_box_found = true;
					break;
				}
			}
			if(!line_box_found)
			{
				text_indent = m_css_text_indent.calc_percent(max_width);
			}
		}

		font_metrics fm;
		get_font(&fm);
		m_boxes.emplace_back(std::unique_ptr<line_box>(new line_box(line_ctx.top, line_ctx.left + first_line_margin + text_indent, line_ctx.right, line_height(), fm, m_text_align)));
	} else
	{
		m_boxes.emplace_back(std::unique_ptr<block_box>(new block_box(line_ctx.top, line_ctx.left, line_ctx.right)));
	}

	return line_ctx.top;
}

int litehtml::html_tag::get_cleared_top(const element::ptr &el, int line_top) const
{
	switch(el->get_clear())
	{
	case clear_left:
		{
			int fh = get_left_floats_height();
			if(fh && fh > line_top)
			{
				line_top = fh;
			}
		}
		break;
	case clear_right:
		{
			int fh = get_right_floats_height();
			if(fh && fh > line_top)
			{
				line_top = fh;
			}
		}
		break;
	case clear_both:
		{
			int fh = get_floats_height();
			if(fh && fh > line_top)
			{
				line_top = fh;
			}
		}
		break;
	default:
		if(el->get_float() != float_none)
		{
			int fh = get_floats_height(el->get_float());
			if(fh && fh > line_top)
			{
				line_top = fh;
			}
		}
		break;
	}
	return line_top;
}

litehtml::style_display litehtml::html_tag::get_display() const
{
	return m_display;
}

litehtml::element_float litehtml::html_tag::get_float() const
{
	return m_float;
}

bool litehtml::html_tag::is_floats_holder() const
{
	if(	m_display == display_inline_block || 
		m_display == display_table_cell || 
		!have_parent() ||
		is_body() || 
		m_float != float_none ||
		m_el_position == element_position_absolute ||
		m_el_position == element_position_fixed ||
		m_overflow > overflow_visible)
	{
		return true;
	}
	return false;
}

bool litehtml::html_tag::is_first_child_inline(const element::ptr& el) const
{
	if(!m_children.empty())
	{
		for (const auto& this_el : m_children)
		{
			if (!this_el->is_white_space())
			{
				if (el == this_el)
				{
					return true;
				}
				if (this_el->get_display() == display_inline)
				{
					if (this_el->have_inline_child())
					{
						return false;
					}
				} else
				{
					return false;
				}
			}
		}
	}
	return false;
}

bool litehtml::html_tag::is_last_child_inline(const element::ptr& el)
{
	if(!m_children.empty())
	{
		for (auto this_el = m_children.rbegin(); this_el < m_children.rend(); ++this_el)
		{
			if (!(*this_el)->is_white_space())
			{
				if (el == (*this_el))
				{
					return true;
				}
				if ((*this_el)->get_display() == display_inline)
				{
					if ((*this_el)->have_inline_child())
					{
						return false;
					}
				} else
				{
					return false;
				}
			}
		}
	}
	return false;
}

litehtml::white_space litehtml::html_tag::get_white_space() const
{
	return m_white_space;
}

litehtml::vertical_align litehtml::html_tag::get_vertical_align() const
{
	return m_vertical_align;
}

litehtml::css_length litehtml::html_tag::get_css_left() const
{
	return m_css_offsets.left;
}

litehtml::css_length litehtml::html_tag::get_css_right() const
{
	return m_css_offsets.right;
}

litehtml::css_length litehtml::html_tag::get_css_top() const
{
	return m_css_offsets.top;
}

litehtml::css_length litehtml::html_tag::get_css_bottom() const
{
	return m_css_offsets.bottom;
}


litehtml::css_offsets litehtml::html_tag::get_css_offsets() const
{
	return m_css_offsets;
}

litehtml::element_clear litehtml::html_tag::get_clear() const
{
	return m_clear;
}

litehtml::css_length litehtml::html_tag::get_css_width() const
{
	return m_css_width;
}

litehtml::css_length litehtml::html_tag::get_css_height() const
{
	return m_css_height;
}

size_t litehtml::html_tag::get_children_count() const
{
	return m_children.size();
}

litehtml::element::ptr litehtml::html_tag::get_child( int idx ) const
{
	return m_children[idx];
}

void litehtml::html_tag::set_css_width( css_length& w )
{
	m_css_width = w;
}

void litehtml::html_tag::apply_vertical_align()
{
	if(!m_boxes.empty())
	{
		int add = 0;
		int content_height	= m_boxes.back()->bottom();

		if(m_pos.height > content_height)
		{
			switch(m_vertical_align)
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

		if(add)
		{
			for(auto & box : m_boxes)
			{
				box->y_shift(add);
			}
		}
	}
}

litehtml::element_position litehtml::html_tag::get_element_position(css_offsets* offsets) const
{
	if(offsets && m_el_position != element_position_static)
	{
		*offsets = m_css_offsets;
	}
	return m_el_position;
}

void litehtml::html_tag::init_background_paint(position pos, background_paint &bg_paint, const background* bg)
{
	if(!bg) return;

	bg_paint = *bg;
	position content_box	= pos;
	position padding_box	= pos;
	padding_box += m_padding;
	position border_box		= padding_box;
	border_box += m_borders;

	switch(bg->m_clip)
	{
	case litehtml::background_box_padding:
		bg_paint.clip_box = padding_box;
		break;
	case litehtml::background_box_content:
		bg_paint.clip_box = content_box;
		break;
	default:
		bg_paint.clip_box = border_box;
		break;
	}

	switch(bg->m_origin)
	{
	case litehtml::background_box_border:
		bg_paint.origin_box = border_box;
		break;
	case litehtml::background_box_content:
		bg_paint.origin_box = content_box;
		break;
	default:
		bg_paint.origin_box = padding_box;
		break;
	}

	if(!bg_paint.image.empty())
	{
		get_document()->container()->get_image_size(bg_paint.image.c_str(), bg_paint.baseurl.c_str(), bg_paint.image_size);
		if(bg_paint.image_size.width && bg_paint.image_size.height)
		{
			litehtml::size img_new_sz = bg_paint.image_size;
			double img_ar_width		= (double) bg_paint.image_size.width / (double) bg_paint.image_size.height;
			double img_ar_height	= (double) bg_paint.image_size.height / (double) bg_paint.image_size.width;


			if(bg->m_position.width.is_predefined())
			{
				switch(bg->m_position.width.predef())
				{
				case litehtml::background_size_contain:
					if( (int) ((double) bg_paint.origin_box.width * img_ar_height) <= bg_paint.origin_box.height )
					{
						img_new_sz.width = bg_paint.origin_box.width;
						img_new_sz.height	= (int) ((double) bg_paint.origin_box.width * img_ar_height);
					} else
					{
						img_new_sz.height = bg_paint.origin_box.height;
						img_new_sz.width	= (int) ((double) bg_paint.origin_box.height * img_ar_width);
					}
					break;
				case litehtml::background_size_cover:
					if( (int) ((double) bg_paint.origin_box.width * img_ar_height) >= bg_paint.origin_box.height )
					{
						img_new_sz.width = bg_paint.origin_box.width;
						img_new_sz.height	= (int) ((double) bg_paint.origin_box.width * img_ar_height);
					} else
					{
						img_new_sz.height = bg_paint.origin_box.height;
						img_new_sz.width	= (int) ((double) bg_paint.origin_box.height * img_ar_width);
					}
					break;
					break;
				case litehtml::background_size_auto:
					if(!bg->m_position.height.is_predefined())
					{
						img_new_sz.height	= bg->m_position.height.calc_percent(bg_paint.origin_box.height);
						img_new_sz.width	= (int) ((double) img_new_sz.height * img_ar_width);
					}
					break;
				}
			} else
			{
				img_new_sz.width = bg->m_position.width.calc_percent(bg_paint.origin_box.width);
				if(bg->m_position.height.is_predefined())
				{
					img_new_sz.height = (int) ((double) img_new_sz.width * img_ar_height);
				} else
				{
					img_new_sz.height = bg->m_position.height.calc_percent(bg_paint.origin_box.height);
				}
			}

			bg_paint.image_size = img_new_sz;
			bg_paint.position_x = bg_paint.origin_box.x + (int) bg->m_position.x.calc_percent(bg_paint.origin_box.width - bg_paint.image_size.width);
			bg_paint.position_y = bg_paint.origin_box.y + (int) bg->m_position.y.calc_percent(bg_paint.origin_box.height - bg_paint.image_size.height);
		}

	}
	bg_paint.border_radius	= m_css_borders.radius.calc_percents(border_box.width, border_box.height);
	bg_paint.border_box		= border_box;
	bg_paint.is_root		= !have_parent();
}

litehtml::visibility litehtml::html_tag::get_visibility() const
{
	return m_visibility;
}

void litehtml::html_tag::draw_list_marker( uint_ptr hdc, const position &pos )
{
	list_marker lm;

	const tchar_t* list_image = get_style_property(_t("list-style-image"), true, nullptr);
	size img_size;
	if(list_image)
	{
		css::parse_css_url(list_image, lm.image);
		lm.baseurl = get_style_property(_t("list-style-image-baseurl"), true, nullptr);
		get_document()->container()->get_image_size(lm.image.c_str(), lm.baseurl, img_size);
	} else
	{
		lm.baseurl = nullptr;
	}

	int ln_height	= line_height();
	int sz_font		= get_font_size();
	lm.pos.x		= pos.x;
	lm.pos.width = sz_font - sz_font * 2 / 3;
	lm.color = get_color(_t("color"), true, web_color(0, 0, 0));
	lm.marker_type = m_list_style_type;
	lm.font = get_font();

	if (m_list_style_type >= list_style_type_armenian)
	{
		lm.pos.y = pos.y;
		lm.pos.height = pos.height;
		lm.index = (unsigned char) get_attr(_t("list_index"), _t(""))[0];
	}
	else
	{
		lm.pos.height = sz_font - sz_font * 2 / 3;
		lm.pos.y = pos.y + ln_height / 2 - lm.pos.height / 2;
		lm.index = -1;
	}

	if(img_size.width && img_size.height)
	{
		if(lm.pos.y + img_size.height > pos.y + pos.height)
		{
			lm.pos.y = pos.y + pos.height - img_size.height;
		}
		if(img_size.width > lm.pos.width)
		{
			lm.pos.x -= img_size.width - lm.pos.width;
		}

		lm.pos.width	= img_size.width;
		lm.pos.height	= img_size.height;
	}

	if (m_list_style_position == list_style_position_outside)
	{
		if (m_list_style_type >= list_style_type_armenian)
		{
			auto tw_space = get_document()->container()->text_width(_t(" "), lm.font);
			lm.pos.x = pos.x - tw_space * 2;
			lm.pos.width = tw_space;
		}
		else
		{
			lm.pos.x -= sz_font;
		}
	}

	if (m_list_style_type >= list_style_type_armenian)
	{
		auto marker_text = get_list_marker_text(lm.index);
		lm.pos.height = ln_height;
		if (marker_text.empty())
		{
			get_document()->container()->draw_list_marker(hdc, lm);
		}
		else
		{
			marker_text += _t(".");
			auto tw = get_document()->container()->text_width(marker_text.c_str(), lm.font);
			auto text_pos = lm.pos;
			text_pos.move_to(text_pos.right() - tw, text_pos.y);
			text_pos.width = tw;
			get_document()->container()->draw_text(hdc, marker_text.c_str(), lm.font, lm.color, text_pos);
		}
	}
	else
	{
		get_document()->container()->draw_list_marker(hdc, lm);
	}
}

litehtml::tstring litehtml::html_tag::get_list_marker_text(int index)
{
	switch (m_list_style_type)
	{
	case litehtml::list_style_type_decimal:
		return t_to_string(index);
	case litehtml::list_style_type_decimal_leading_zero:
		{
			auto txt = t_to_string(index);
			if (txt.length() == 1)
			{
				txt = _t("0") + txt;
			}
			return txt;
		}
	case litehtml::list_style_type_lower_latin:
	case litehtml::list_style_type_lower_alpha:
		return num_cvt::to_latin_lower(index);
	case litehtml::list_style_type_lower_greek:
		return num_cvt::to_greek_lower(index);
	case litehtml::list_style_type_upper_alpha:
	case litehtml::list_style_type_upper_latin:
		return num_cvt::to_latin_upper(index);
	case litehtml::list_style_type_lower_roman:
		return num_cvt::to_roman_lower(index);
	case litehtml::list_style_type_upper_roman:
		return num_cvt::to_roman_upper(index);
    default:
        return _t("");
//	case litehtml::list_style_type_armenian:
//	case litehtml::list_style_type_georgian:
//	case litehtml::list_style_type_hebrew:
//	case litehtml::list_style_type_hiragana:
//	case litehtml::list_style_type_hiragana_iroha:
//	case litehtml::list_style_type_katakana:
//	case litehtml::list_style_type_katakana_iroha:
//  case litehtml::list_style_type_none:
//  case litehtml::list_style_type_circle:
//  case litehtml::list_style_type_disc:
//  case litehtml::list_style_type_square:
//  case litehtml::list_style_type_cjk_ideographic:
//      break;
    }
}

void litehtml::html_tag::draw_children( uint_ptr hdc, int x, int y, const position* clip, draw_flag flag, int zindex )
{
	if (m_display == display_table || m_display == display_inline_table)
	{
		draw_children_table(hdc, x, y, clip, flag, zindex);
	}
	else
	{
		draw_children_box(hdc, x, y, clip, flag, zindex);
	}
}

bool litehtml::html_tag::fetch_positioned()
{
	bool ret = false;

	m_positioned.clear();

	litehtml::element_position el_pos;

	for(auto& el : m_children)
	{
		el_pos = el->get_element_position();
		if (el_pos != element_position_static)
		{
			add_positioned(el);
		}
		if (!ret && (el_pos == element_position_absolute || el_pos == element_position_fixed))
		{
			ret = true;
		}
		if(el->fetch_positioned())
		{
			ret = true;
		}
	}
	return ret;
}

int litehtml::html_tag::get_zindex() const
{
	return m_z_index;
}

void litehtml::html_tag::render_positioned(render_type rt)
{
	position wnd_position;
	get_document()->container()->get_client_rect(wnd_position);

	element_position el_position;
	bool process;
	for (auto& el : m_positioned)
	{
		el_position = el->get_element_position();

		process = false;
		if(el->get_display() != display_none)
		{
			if(el_position == element_position_absolute)
			{
				if(rt != render_fixed_only)
				{
					process = true;
				}
			} else if(el_position == element_position_fixed)
			{
				if(rt != render_no_fixed)
				{
					process = true;
				}
			}
		}

		if(process)
		{
			int parent_height	= 0;
			int parent_width	= 0;
			int client_x		= 0;
			int client_y		= 0;
			if(el_position == element_position_fixed)
			{
				parent_height	= wnd_position.height;
				parent_width	= wnd_position.width;
				client_x		= wnd_position.left();
				client_y		= wnd_position.top();
			} else
			{
				element::ptr el_parent = el->parent();
				if(el_parent)
				{
					parent_height	= el_parent->height();
					parent_width	= el_parent->width();
				}
			}

			css_length	css_left	= el->get_css_left();
			css_length	css_right	= el->get_css_right();
			css_length	css_top		= el->get_css_top();
			css_length	css_bottom	= el->get_css_bottom();

			bool need_render = false;

			css_length el_w = el->get_css_width();
			css_length el_h = el->get_css_height();

            int new_width = -1;
            int new_height = -1;
			if(el_w.units() == css_units_percentage && parent_width)
			{
                new_width = el_w.calc_percent(parent_width);
                if(el->m_pos.width != new_width)
				{
					need_render = true;
                    el->m_pos.width = new_width;
				}
			}

			if(el_h.units() == css_units_percentage && parent_height)
			{
                new_height = el_h.calc_percent(parent_height);
                if(el->m_pos.height != new_height)
				{
					need_render = true;
                    el->m_pos.height = new_height;
				}
			}

			bool cvt_x = false;
			bool cvt_y = false;

			if(el_position == element_position_fixed)
			{
				if(!css_left.is_predefined() || !css_right.is_predefined())
				{
					if(!css_left.is_predefined() && css_right.is_predefined())
					{
						el->m_pos.x = css_left.calc_percent(parent_width) + el->content_margins_left();
					} else if(css_left.is_predefined() && !css_right.is_predefined())
					{
						el->m_pos.x = parent_width - css_right.calc_percent(parent_width) - el->m_pos.width - el->content_margins_right();
					} else
					{
						el->m_pos.x		= css_left.calc_percent(parent_width) + el->content_margins_left();
						el->m_pos.width	= parent_width - css_left.calc_percent(parent_width) - css_right.calc_percent(parent_width) - (el->content_margins_left() + el->content_margins_right());
						need_render = true;
					}
				}

				if(!css_top.is_predefined() || !css_bottom.is_predefined())
				{
					if(!css_top.is_predefined() && css_bottom.is_predefined())
					{
						el->m_pos.y = css_top.calc_percent(parent_height) + el->content_margins_top();
					} else if(css_top.is_predefined() && !css_bottom.is_predefined())
					{
						el->m_pos.y = parent_height - css_bottom.calc_percent(parent_height) - el->m_pos.height - el->content_margins_bottom();
					} else
					{
						el->m_pos.y			= css_top.calc_percent(parent_height) + el->content_margins_top();
						el->m_pos.height	= parent_height - css_top.calc_percent(parent_height) - css_bottom.calc_percent(parent_height) - (el->content_margins_top() + el->content_margins_bottom());
						need_render = true;
					}
				}
			} else 
			{
				if(!css_left.is_predefined() || !css_right.is_predefined())
				{
					if(!css_left.is_predefined() && css_right.is_predefined())
					{
						el->m_pos.x = css_left.calc_percent(parent_width) + el->content_margins_left() - m_padding.left;
					} else if(css_left.is_predefined() && !css_right.is_predefined())
					{
						el->m_pos.x = m_pos.width + m_padding.right - css_right.calc_percent(parent_width) - el->m_pos.width - el->content_margins_right();
					} else
					{
						el->m_pos.x		= css_left.calc_percent(parent_width) + el->content_margins_left() - m_padding.left;
						el->m_pos.width	= m_pos.width + m_padding.left + m_padding.right - css_left.calc_percent(parent_width) - css_right.calc_percent(parent_width) - (el->content_margins_left() + el->content_margins_right());
                        if (new_width != -1)
                        {
                            el->m_pos.x += (el->m_pos.width - new_width) / 2;
                            el->m_pos.width = new_width;
                        }
                        need_render = true;
					}
					cvt_x = true;
				}

				if(!css_top.is_predefined() || !css_bottom.is_predefined())
				{
					if(!css_top.is_predefined() && css_bottom.is_predefined())
					{
						el->m_pos.y = css_top.calc_percent(parent_height) + el->content_margins_top() - m_padding.top;
					} else if(css_top.is_predefined() && !css_bottom.is_predefined())
					{
						el->m_pos.y = m_pos.height + m_padding.bottom - css_bottom.calc_percent(parent_height) - el->m_pos.height - el->content_margins_bottom();
					} else
					{
						el->m_pos.y			= css_top.calc_percent(parent_height) + el->content_margins_top() - m_padding.top;
						el->m_pos.height	= m_pos.height + m_padding.top + m_padding.bottom - css_top.calc_percent(parent_height) - css_bottom.calc_percent(parent_height) - (el->content_margins_top() + el->content_margins_bottom());
                        if (new_height != -1)
                        {
                            el->m_pos.y += (el->m_pos.height - new_height) / 2;
                            el->m_pos.height = new_height;
                        }
                        need_render = true;
					}
					cvt_y = true;
				}
			}

			if(cvt_x || cvt_y)
			{
				int offset_x = 0;
				int offset_y = 0;
				element::ptr cur_el = el->parent();
				element::ptr this_el = shared_from_this();
				while(cur_el && cur_el != this_el)
				{
					offset_x += cur_el->m_pos.x;
					offset_y += cur_el->m_pos.y;
					cur_el = cur_el->parent();
				}
				if(cvt_x)	el->m_pos.x -= offset_x;
				if(cvt_y)	el->m_pos.y -= offset_y;
			}

			if(need_render)
			{
				position pos = el->m_pos;
				el->render(el->left(), el->top(), el->width(), true);
				el->m_pos = pos;
			}

			if(el_position == element_position_fixed)
			{
				position fixed_pos;
				el->get_redraw_box(fixed_pos);
				get_document()->add_fixed_box(fixed_pos);
			}
		}

		el->render_positioned();
	}

	if(!m_positioned.empty())
	{
		std::stable_sort(m_positioned.begin(), m_positioned.end(), [](const litehtml::element::ptr& Left, const litehtml::element::ptr& Right)
		{
			return (Left->get_zindex() < Right->get_zindex());
		});
	}
}

void litehtml::html_tag::draw_stacking_context( uint_ptr hdc, int x, int y, const position* clip, bool with_positioned )
{
	if(!is_visible()) return;

	std::map<int, bool> z_indexes;
	if(with_positioned)
	{
		for(const auto& idx : m_positioned)
		{
			z_indexes[idx->get_zindex()];
		}

        for(const auto& idx : z_indexes)
		{
			if(idx.first < 0)
			{
				draw_children(hdc, x, y, clip, draw_positioned, idx.first);
			}
		}
	}
	draw_children(hdc, x, y, clip, draw_block, 0);
	draw_children(hdc, x, y, clip, draw_floats, 0);
	draw_children(hdc, x, y, clip, draw_inlines, 0);
	if(with_positioned)
	{
		for(auto& z_index : z_indexes)
		{
			if(z_index.first == 0)
			{
				draw_children(hdc, x, y, clip, draw_positioned, z_index.first);
			}
		}

		for(auto& z_index : z_indexes)
		{
			if(z_index.first > 0)
			{
				draw_children(hdc, x, y, clip, draw_positioned, z_index.first);
			}
		}
	}
}

litehtml::overflow litehtml::html_tag::get_overflow() const
{
	return m_overflow;
}

bool litehtml::html_tag::is_nth_child(const element::ptr& el, int num, int off, bool of_type) const
{
	int idx = 1;
	for(const auto& child : m_children)
	{
		if(child->get_display() != display_inline_text)
		{
			if( (!of_type) || (of_type && !t_strcmp(el->get_tagName(), child->get_tagName())) )
			{
				if(el == child)
				{
					if(num != 0)
					{
						if((idx - off) >= 0 && (idx - off) % num == 0)
						{
							return true;
						}

					} else if(idx == off)
					{
						return true;
					}
					return false;
				}
				idx++;
			}
			if(el == child) break;
		}
	}
	return false;
}

bool litehtml::html_tag::is_nth_last_child(const element::ptr& el, int num, int off, bool of_type) const
{
	int idx = 1;
	for(auto child = m_children.rbegin(); child != m_children.rend(); child++)
	{
		if((*child)->get_display() != display_inline_text)
		{
			if( !of_type || (of_type && !t_strcmp(el->get_tagName(), (*child)->get_tagName())) )
			{
				if(el == (*child))
				{
					if(num != 0)
					{
						if((idx - off) >= 0 && (idx - off) % num == 0)
						{
							return true;
						}

					} else if(idx == off)
					{
						return true;
					}
					return false;
				}
				idx++;
			}
			if(el == (*child)) break;
		}
	}
	return false;
}

void litehtml::html_tag::parse_nth_child_params( const tstring& param, int &num, int &off )
{
	if(param == _t("odd"))
	{
		num = 2;
		off = 1;
	} else if(param == _t("even"))
	{
		num = 2;
		off = 0;
	} else
	{
		string_vector tokens;
		split_string(param, tokens, _t(" n"), _t("n"));

		tstring s_num;
		tstring s_off;

		tstring s_int;
		for(const auto& token : tokens)
		{
			if(token == _t("n"))
			{
				s_num = s_int;
				s_int.clear();
			} else
			{
				s_int += token;
			}
		}
		s_off = s_int;

		num = t_atoi(s_num.c_str());
		off = t_atoi(s_off.c_str());
	}
}

void litehtml::html_tag::calc_document_size( litehtml::size& sz, int x /*= 0*/, int y /*= 0*/ )
{
	if(is_visible() && m_el_position != element_position_fixed)
	{
		element::calc_document_size(sz, x, y);

		if(m_overflow == overflow_visible)
		{
			for(auto& el : m_children)
			{
				el->calc_document_size(sz, x + m_pos.x, y + m_pos.y);
			}
		}

		// root element (<html>) must to cover entire window
		if(!have_parent())
		{
			position client_pos;
			get_document()->container()->get_client_rect(client_pos);
			m_pos.height = std::max(sz.height, client_pos.height) - content_margins_top() - content_margins_bottom();
			m_pos.width	 = std::max(sz.width, client_pos.width) - content_margins_left() - content_margins_right();
		}
	}
}


void litehtml::html_tag::get_redraw_box(litehtml::position& pos, int x /*= 0*/, int y /*= 0*/)
{
	if(is_visible())
	{
		element::get_redraw_box(pos, x, y);

		if(m_overflow == overflow_visible)
		{
			for(auto& el : m_children)
			{
				if(el->get_element_position() != element_position_fixed)
				{
					el->get_redraw_box(pos, x + m_pos.x, y + m_pos.y);
				}
			}
		}
	}
}

litehtml::element::ptr litehtml::html_tag::find_adjacent_sibling( const element::ptr& el, const css_selector& selector, bool apply_pseudo /*= true*/, bool* is_pseudo /*= 0*/ )
{
	element::ptr ret;
	for(auto& e : m_children)
	{
		if(e->get_display() != display_inline_text)
		{
			if(e == el)
			{
				if(ret)
				{
					int res = ret->select(selector, apply_pseudo);
					if(res != select_no_match)
					{
						if(is_pseudo)
						{
							if(res & select_match_pseudo_class)
							{
								*is_pseudo = true;
							} else
							{
								*is_pseudo = false;
							}
						}
						return ret;
					}
				}
				return nullptr;
			} else
			{
				ret = e;
			}
		}
	}
	return nullptr;
}

litehtml::element::ptr litehtml::html_tag::find_sibling(const element::ptr& el, const css_selector& selector, bool apply_pseudo /*= true*/, bool* is_pseudo /*= 0*/)
{
	element::ptr ret = nullptr;
	for(auto& e : m_children)
	{
		if(e->get_display() != display_inline_text)
		{
			if(e == el)
			{
				return ret;
			} else if(!ret)
			{
				int res = e->select(selector, apply_pseudo);
				if(res != select_no_match)
				{
					if(is_pseudo)
					{
						if(res & select_match_pseudo_class)
						{
							*is_pseudo = true;
						} else
						{
							*is_pseudo = false;
						}
					}
					ret = e;
				}
			}
		}
	}
	return nullptr;
}

bool litehtml::html_tag::is_only_child(const element::ptr& el, bool of_type) const
{
	int child_count = 0;
	for(const auto& child : m_children)
	{
		if(child->get_display() != display_inline_text)
		{
			if( !of_type || (of_type && !t_strcmp(el->get_tagName(), child->get_tagName())) )
			{
				child_count++;
			}
			if(child_count > 1) break;
		}
	}
	if(child_count > 1)
	{
		return false;
	}
	return true;
}

void litehtml::html_tag::update_floats(int dy, const element::ptr &parent)
{
	if(is_floats_holder())
	{
		bool reset_cache = false;
		for(auto fb = m_floats_left.rbegin(); fb != m_floats_left.rend(); fb++)
		{
			if(fb->el->is_ancestor(parent))
			{
				reset_cache	= true;
				fb->pos.y	+= dy;
			}
		}
		if(reset_cache)
		{
			m_cahe_line_left.invalidate();
		}
		reset_cache = false;
		for(auto fb = m_floats_right.rbegin(); fb != m_floats_right.rend(); fb++)
		{
			if(fb->el->is_ancestor(parent))
			{
				reset_cache	= true;
				fb->pos.y	+= dy;
			}
		}
		if(reset_cache)
		{
			m_cahe_line_right.invalidate();
		}
	} else
	{
		element::ptr el_parent = this->parent();
		if (el_parent)
		{
			el_parent->update_floats(dy, parent);
		}
	}
}

void litehtml::html_tag::remove_before_after()
{
	if(!m_children.empty())
	{
		if( !t_strcmp(m_children.front()->get_tagName(), _t("::before")) )
		{
			m_children.erase(m_children.begin());
		}
	}
	if(!m_children.empty())
	{
		if( !t_strcmp(m_children.back()->get_tagName(), _t("::after")) )
		{
			m_children.erase(m_children.end() - 1);
		}
	}
}

litehtml::element::ptr litehtml::html_tag::get_element_before()
{
	if(!m_children.empty())
	{
		if( !t_strcmp(m_children.front()->get_tagName(), _t("::before")) )
		{
			return m_children.front();
		}
	}
	element::ptr el = std::make_shared<el_before>(get_document());
	el->parent(shared_from_this());
	m_children.insert(m_children.begin(), el);
	return el;
}

litehtml::element::ptr litehtml::html_tag::get_element_after()
{
	if(!m_children.empty())
	{
		if( !t_strcmp(m_children.back()->get_tagName(), _t("::after")) )
		{
			return m_children.back();
		}
	}
	element::ptr el = std::make_shared<el_after>(get_document());
	appendChild(el);
	return el;
}

void litehtml::html_tag::add_style( const litehtml::style& st )
{
	m_style.combine(st);
}

bool litehtml::html_tag::have_inline_child() const
{
	if(!m_children.empty())
	{
		for(const auto& el : m_children)
		{
			if(!el->is_white_space())
			{
				return true;
			}
		}
	}
	return false;
}

void litehtml::html_tag::refresh_styles()
{
	remove_before_after();

	for (auto& el : m_children)
	{
		if(el->get_display() != display_inline_text)
		{
			el->refresh_styles();
		}
	}

	m_style.clear();

	for (auto& usel : m_used_styles)
	{
		usel->m_used = false;

		if(usel->m_selector->is_media_valid())
		{
			int apply = select(*usel->m_selector, false);

			if(apply != select_no_match)
			{
				if(apply & select_match_pseudo_class)
				{
					if(select(*usel->m_selector, true))
					{
						if(apply & select_match_with_after)
						{
							element::ptr el = get_element_after();
							if(el)
							{
								el->add_style(*usel->m_selector->m_style);
							}
						} else if(apply & select_match_with_before)
						{
							element::ptr el = get_element_before();
							if(el)
							{
								el->add_style(*usel->m_selector->m_style);
							}
						}
						else
						{
							add_style(*usel->m_selector->m_style);
							usel->m_used = true;
						}
					}
				} else if(apply & select_match_with_after)
				{
					element::ptr el = get_element_after();
					if(el)
					{
						el->add_style(*usel->m_selector->m_style);
					}
				} else if(apply & select_match_with_before)
				{
					element::ptr el = get_element_before();
					if(el)
					{
						el->add_style(*usel->m_selector->m_style);
					}
				} else
				{
					add_style(*usel->m_selector->m_style);
					usel->m_used = true;
				}
			}
		}
	}
}

litehtml::element::ptr litehtml::html_tag::get_child_by_point(int x, int y, int client_x, int client_y, draw_flag flag, int zindex)
{
	element::ptr ret = nullptr;

	if(m_overflow > overflow_visible)
	{
		if(!m_pos.is_point_inside(x, y))
		{
			return ret;
		}
	}

	position pos = m_pos;
	pos.x	= x - pos.x;
	pos.y	= y - pos.y;

	for(auto i = m_children.rbegin(); i != m_children.rend() && !ret; i++)
	{
		element::ptr el = (*i);

		if(el->is_visible() && el->get_display() != display_inline_text)
		{
			switch(flag)
			{
			case draw_positioned:
				if(el->is_positioned() && el->get_zindex() == zindex)
				{
					if(el->get_element_position() == element_position_fixed)
					{
						ret = el->get_element_by_point(client_x, client_y, client_x, client_y);
						if(!ret && (*i)->is_point_inside(client_x, client_y))
						{
							ret = (*i);
						}
					} else
					{
						ret = el->get_element_by_point(pos.x, pos.y, client_x, client_y);
						if(!ret && (*i)->is_point_inside(pos.x, pos.y))
						{
							ret = (*i);
						}
					}
					el = nullptr;
				}
				break;
			case draw_block:
				if(!el->is_inline_box() && el->get_float() == float_none && !el->is_positioned())
				{
					if(el->is_point_inside(pos.x, pos.y))
					{
						ret = el;
					}
				}
				break;
			case draw_floats:
				if(el->get_float() != float_none && !el->is_positioned())
				{
					ret = el->get_element_by_point(pos.x, pos.y, client_x, client_y);

					if(!ret && (*i)->is_point_inside(pos.x, pos.y))
					{
						ret = (*i);
					}
					el = nullptr;
				}
				break;
			case draw_inlines:
				if(el->is_inline_box() && el->get_float() == float_none && !el->is_positioned())
				{
					if(el->get_display() == display_inline_block)
					{
						ret = el->get_element_by_point(pos.x, pos.y, client_x, client_y);
						el = nullptr;
					}
					if(!ret && (*i)->is_point_inside(pos.x, pos.y))
					{
						ret = (*i);
					}
				}
				break;
			default:
				break;
			}

			if(el && !el->is_positioned())
			{
				if(flag == draw_positioned)
				{
					element::ptr child = el->get_child_by_point(pos.x, pos.y, client_x, client_y, flag, zindex);
					if(child)
					{
						ret = child;
					}
				} else
				{
					if(	el->get_float() == float_none &&
						el->get_display() != display_inline_block)
					{
						element::ptr child = el->get_child_by_point(pos.x, pos.y, client_x, client_y, flag, zindex);
						if(child)
						{
							ret = child;
						}
					}
				}
			}
		}
	}

	return ret;
}

litehtml::element::ptr litehtml::html_tag::get_element_by_point(int x, int y, int client_x, int client_y)
{
	if(!is_visible()) return nullptr;

	element::ptr ret;

	std::map<int, bool> z_indexes;

	for(const auto& i : m_positioned)
	{
		z_indexes[i->get_zindex()];
	}

	for(const auto& zindex : z_indexes)
	{
		if(zindex.first > 0)
		{
			ret = get_child_by_point(x, y, client_x, client_y, draw_positioned, zindex.first);
            break;
		}
	}
	if(ret) return ret;

	for(const auto& z_index : z_indexes)
	{
		if(z_index.first == 0)
		{
			ret = get_child_by_point(x, y, client_x, client_y, draw_positioned, z_index.first);
            break;
		}
	}
	if(ret) return ret;

	ret = get_child_by_point(x, y, client_x, client_y, draw_inlines, 0);
	if(ret) return ret;

	ret = get_child_by_point(x, y, client_x, client_y, draw_floats, 0);
	if(ret) return ret;

	ret = get_child_by_point(x, y, client_x, client_y, draw_block, 0);
	if(ret) return ret;


	for(const auto& z_index : z_indexes)
	{
		if(z_index.first < 0)
		{
			ret = get_child_by_point(x, y, client_x, client_y, draw_positioned, z_index.first);
            break;
		}
	}
	if(ret) return ret;

	if(m_el_position == element_position_fixed)
	{
		if(is_point_inside(client_x, client_y))
		{
			ret = shared_from_this();
		}
	} else
	{
		if(is_point_inside(x, y))
		{
			ret = shared_from_this();
		}
	}

	return ret;
}

const litehtml::background* litehtml::html_tag::get_background(bool own_only)
{
	if(own_only)
	{
		// return own background with check for empty one
		if(m_bg.m_image.empty() && !m_bg.m_color.alpha)
		{
			return nullptr;
		}
		return &m_bg;
	}

	if(m_bg.m_image.empty() && !m_bg.m_color.alpha)
	{
		// if this is root element (<html>) try to get background from body
		if (!have_parent())
		{
			for (const auto& el : m_children)
			{
				if( el->is_body() )
				{
					// return own body background
					return el->get_background(true);
				}
			}
		}
		return nullptr;
	}
	
	if(is_body())
	{
		element::ptr el_parent = parent();
		if (el_parent)
		{
			if (!el_parent->get_background(true))
			{
				// parent of body will draw background for body
				return nullptr;
			}
		}
	}

	return &m_bg;
}

int litehtml::html_tag::render_box(int x, int y, int max_width, bool second_pass /*= false*/)
{
	int parent_width = max_width;

	calc_outlines(parent_width);

	m_pos.clear();
	m_pos.move_to(x, y);

	m_pos.x += content_margins_left();
	m_pos.y += content_margins_top();

	int ret_width = 0;

	def_value<int>	block_width(0);

	if (m_display != display_table_cell && !m_css_width.is_predefined())
	{
		int w = calc_width(parent_width);
		
		if (m_box_sizing == box_sizing_border_box)
		{
			w -= m_padding.width() + m_borders.width();
		}
		ret_width = max_width = block_width = w;
	}
	else
	{
		if (max_width)
		{
			max_width -= content_margins_left() + content_margins_right();
		}
	}

	// check for max-width (on the first pass only)
	if (!m_css_max_width.is_predefined() && !second_pass)
	{
		int mw = get_document()->cvt_units(m_css_max_width, m_font_size, parent_width);
		if (m_box_sizing == box_sizing_border_box)
		{
			mw -= m_padding.left + m_borders.left + m_padding.right + m_borders.right;
		}
		if (max_width > mw)
		{
			max_width = mw;
		}
	}

	m_floats_left.clear();
	m_floats_right.clear();
	m_boxes.clear();
	m_cahe_line_left.invalidate();
	m_cahe_line_right.invalidate();

	element_position el_position;

	int block_height = 0;

	m_pos.height = 0;

	if (get_predefined_height(block_height))
	{
		m_pos.height = block_height;
	}

	white_space ws = get_white_space();
	bool skip_spaces = false;
	if (ws == white_space_normal ||
		ws == white_space_nowrap ||
		ws == white_space_pre_line)
	{
		skip_spaces = true;
	}

	bool was_space = false;

	for (const auto& el : m_children)
	{
		// we don't need to process absolute and fixed positioned element on the second pass
		if (second_pass)
		{
			el_position = el->get_element_position();
			if ((el_position == element_position_absolute || el_position == element_position_fixed)) continue;
		}

		// skip spaces to make rendering a bit faster
		if (skip_spaces)
		{
			if (el->is_white_space())
			{
				if (was_space)
				{
					el->skip(true);
					continue;
				}
				else
				{
					was_space = true;
				}
			}
			else
			{
				was_space = false;
			}
		}

		// place element into rendering flow
		int rw = place_element(el, max_width);
		if (rw > ret_width)
		{
			ret_width = rw;
		}
	}

	finish_last_box(true);

	if (block_width.is_default() && is_inline_box())
	{
		m_pos.width = ret_width;
	}
	else
	{
		m_pos.width = max_width;
	}
	calc_auto_margins(parent_width);

	if (!m_boxes.empty())
	{
		if (collapse_top_margin())
		{
			int old_top = m_margins.top;
			m_margins.top = std::max(m_boxes.front()->top_margin(), m_margins.top);
			if (m_margins.top != old_top)
			{
				update_floats(m_margins.top - old_top, shared_from_this());
			}
		}
		if (collapse_bottom_margin())
		{
			m_margins.bottom = std::max(m_boxes.back()->bottom_margin(), m_margins.bottom);
			m_pos.height = m_boxes.back()->bottom() - m_boxes.back()->bottom_margin();
		}
		else
		{
			m_pos.height = m_boxes.back()->bottom();
		}
	}

	// add the floats height to the block height
	if (is_floats_holder())
	{
		int floats_height = get_floats_height();
		if (floats_height > m_pos.height)
		{
			m_pos.height = floats_height;
		}
	}

	// calculate the final position

	m_pos.move_to(x, y);
	m_pos.x += content_margins_left();
	m_pos.y += content_margins_top();

	if (get_predefined_height(block_height))
	{
		m_pos.height = block_height;
	}

	int min_height = 0;
	if (!m_css_min_height.is_predefined() && m_css_min_height.units() == css_units_percentage)
	{
		element::ptr el_parent = parent();
		if (el_parent)
		{
			if (el_parent->get_predefined_height(block_height))
			{
				min_height = m_css_min_height.calc_percent(block_height);
			}
		}
	}
	else
	{
		min_height = (int)m_css_min_height.val();
	}
	if (min_height != 0 && m_box_sizing == box_sizing_border_box)
	{
		min_height -= m_padding.top + m_borders.top + m_padding.bottom + m_borders.bottom;
		if (min_height < 0) min_height = 0;
	}

	if (m_display == display_list_item)
	{
		const tchar_t* list_image = get_style_property(_t("list-style-image"), true, nullptr);
		if (list_image)
		{
			tstring url;
			css::parse_css_url(list_image, url);

			size sz;
			const tchar_t* list_image_baseurl = get_style_property(_t("list-style-image-baseurl"), true, nullptr);
			get_document()->container()->get_image_size(url.c_str(), list_image_baseurl, sz);
			if (min_height < sz.height)
			{
				min_height = sz.height;
			}
		}

	}

	if (min_height > m_pos.height)
	{
		m_pos.height = min_height;
	}

	int min_width = m_css_min_width.calc_percent(parent_width);

	if (min_width != 0 && m_box_sizing == box_sizing_border_box)
	{
		min_width -= m_padding.left + m_borders.left + m_padding.right + m_borders.right;
		if (min_width < 0) min_width = 0;
	}

	if (min_width != 0)
	{
		if (min_width > m_pos.width)
		{
			m_pos.width = min_width;
		}
		if (min_width > ret_width)
		{
			ret_width = min_width;
		}
	}

	ret_width += content_margins_left() + content_margins_right();

	// re-render with new width
	if (ret_width < max_width && !second_pass && have_parent())
	{
		if (m_display == display_inline_block ||
			(m_css_width.is_predefined() &&
			(m_float != float_none ||
			m_display == display_table ||
			m_el_position == element_position_absolute ||
			m_el_position == element_position_fixed
			))
			)
		{
			render(x, y, ret_width, true);
			m_pos.width = ret_width - (content_margins_left() + content_margins_right());
		}
	}

	if (is_floats_holder() && !second_pass)
	{
		for (const auto& fb : m_floats_left)
		{
			fb.el->apply_relative_shift(fb.el->parent()->calc_width(m_pos.width));
		}
	}


	return ret_width;
}

int litehtml::html_tag::render_table(int x, int y, int max_width, bool /*second_pass = false*/)
{
	if (!m_grid) return 0;

	int parent_width = max_width;

	calc_outlines(parent_width);

	m_pos.clear();
	m_pos.move_to(x, y);

	m_pos.x += content_margins_left();
	m_pos.y += content_margins_top();

	def_value<int>	block_width(0);

	if (!m_css_width.is_predefined())
	{
		max_width = block_width = calc_width(parent_width) - m_padding.width() - m_borders.width();
	}
	else
	{
		if (max_width)
		{
			max_width -= content_margins_left() + content_margins_right();
		}
	}

	// Calculate table spacing
	int table_width_spacing = 0;
	if (m_border_collapse == border_collapse_separate)
	{
		table_width_spacing = m_border_spacing_x * (m_grid->cols_count() + 1);
	}
	else
	{
		table_width_spacing = 0;

		if (m_grid->cols_count())
		{
			table_width_spacing -= std::min(border_left(), m_grid->column(0).border_left);
			table_width_spacing -= std::min(border_right(), m_grid->column(m_grid->cols_count() - 1).border_right);
		}

		for (int col = 1; col < m_grid->cols_count(); col++)
		{
			table_width_spacing -= std::min(m_grid->column(col).border_left, m_grid->column(col - 1).border_right);
		}
	}


	// Calculate the minimum content width (MCW) of each cell: the formatted content may span any number of lines but may not overflow the cell box. 
	// If the specified 'width' (W) of the cell is greater than MCW, W is the minimum cell width. A value of 'auto' means that MCW is the minimum 
	// cell width.
	// 
	// Also, calculate the "maximum" cell width of each cell: formatting the content without breaking lines other than where explicit line breaks occur.

	if (m_grid->cols_count() == 1 && !block_width.is_default())
	{
		for (int row = 0; row < m_grid->rows_count(); row++)
		{
			table_cell* cell = m_grid->cell(0, row);
			if (cell && cell->el)
			{
				cell->min_width = cell->max_width = cell->el->render(0, 0, max_width - table_width_spacing);
				cell->el->m_pos.width = cell->min_width - cell->el->content_margins_left() - cell->el->content_margins_right();
			}
		}
	}
	else
	{
		for (int row = 0; row < m_grid->rows_count(); row++)
		{
			for (int col = 0; col < m_grid->cols_count(); col++)
			{
				table_cell* cell = m_grid->cell(col, row);
				if (cell && cell->el)
				{
					if (!m_grid->column(col).css_width.is_predefined() && m_grid->column(col).css_width.units() != css_units_percentage)
					{
						int css_w = m_grid->column(col).css_width.calc_percent(block_width);
						int el_w = cell->el->render(0, 0, css_w);
						cell->min_width = cell->max_width = std::max(css_w, el_w);
						cell->el->m_pos.width = cell->min_width - cell->el->content_margins_left() - cell->el->content_margins_right();
					}
					else
					{
						// calculate minimum content width
						cell->min_width = cell->el->render(0, 0, 1);
						// calculate maximum content width
						cell->max_width = cell->el->render(0, 0, max_width - table_width_spacing);
					}
				}
			}
		}
	}

	// For each column, determine a maximum and minimum column width from the cells that span only that column. 
	// The minimum is that required by the cell with the largest minimum cell width (or the column 'width', whichever is larger). 
	// The maximum is that required by the cell with the largest maximum cell width (or the column 'width', whichever is larger).

	for (int col = 0; col < m_grid->cols_count(); col++)
	{
		m_grid->column(col).max_width = 0;
		m_grid->column(col).min_width = 0;
		for (int row = 0; row < m_grid->rows_count(); row++)
		{
			if (m_grid->cell(col, row)->colspan <= 1)
			{
				m_grid->column(col).max_width = std::max(m_grid->column(col).max_width, m_grid->cell(col, row)->max_width);
				m_grid->column(col).min_width = std::max(m_grid->column(col).min_width, m_grid->cell(col, row)->min_width);
			}
		}
	}

	// For each cell that spans more than one column, increase the minimum widths of the columns it spans so that together, 
	// they are at least as wide as the cell. Do the same for the maximum widths. 
	// If possible, widen all spanned columns by approximately the same amount.

	for (int col = 0; col < m_grid->cols_count(); col++)
	{
		for (int row = 0; row < m_grid->rows_count(); row++)
		{
			if (m_grid->cell(col, row)->colspan > 1)
			{
				int max_total_width = m_grid->column(col).max_width;
				int min_total_width = m_grid->column(col).min_width;
				for (int col2 = col + 1; col2 < col + m_grid->cell(col, row)->colspan; col2++)
				{
					max_total_width += m_grid->column(col2).max_width;
					min_total_width += m_grid->column(col2).min_width;
				}
				if (min_total_width < m_grid->cell(col, row)->min_width)
				{
					m_grid->distribute_min_width(m_grid->cell(col, row)->min_width - min_total_width, col, col + m_grid->cell(col, row)->colspan - 1);
				}
				if (max_total_width < m_grid->cell(col, row)->max_width)
				{
					m_grid->distribute_max_width(m_grid->cell(col, row)->max_width - max_total_width, col, col + m_grid->cell(col, row)->colspan - 1);
				}
			}
		}
	}

	// If the 'table' or 'inline-table' element's 'width' property has a computed value (W) other than 'auto', the used width is the 
	// greater of W, CAPMIN, and the minimum width required by all the columns plus cell spacing or borders (MIN). 
	// If the used width is greater than MIN, the extra width should be distributed over the columns.
	//
	// If the 'table' or 'inline-table' element has 'width: auto', the used width is the greater of the table's containing block width, 
	// CAPMIN, and MIN. However, if either CAPMIN or the maximum width required by the columns plus cell spacing or borders (MAX) is 
	// less than that of the containing block, use max(MAX, CAPMIN).


	int table_width = 0;
	int min_table_width = 0;
	int max_table_width = 0;

	if (!block_width.is_default())
	{
		table_width = m_grid->calc_table_width(block_width - table_width_spacing, false, min_table_width, max_table_width);
	}
	else
	{
		table_width = m_grid->calc_table_width(max_width - table_width_spacing, true, min_table_width, max_table_width);
	}

	min_table_width += table_width_spacing;
	max_table_width += table_width_spacing;
	table_width += table_width_spacing;
	m_grid->calc_horizontal_positions(m_borders, m_border_collapse, m_border_spacing_x);

	bool row_span_found = false;

	// render cells with computed width
	for (int row = 0; row < m_grid->rows_count(); row++)
	{
		m_grid->row(row).height = 0;
		for (int col = 0; col < m_grid->cols_count(); col++)
		{
			table_cell* cell = m_grid->cell(col, row);
			if (cell->el)
			{
				int span_col = col + cell->colspan - 1;
				if (span_col >= m_grid->cols_count())
				{
					span_col = m_grid->cols_count() - 1;
				}
				int cell_width = m_grid->column(span_col).right - m_grid->column(col).left;

				if (cell->el->m_pos.width != cell_width - cell->el->content_margins_left() - cell->el->content_margins_right())
				{
					cell->el->render(m_grid->column(col).left, 0, cell_width);
					cell->el->m_pos.width = cell_width - cell->el->content_margins_left() - cell->el->content_margins_right();
				}
				else
				{
					cell->el->m_pos.x = m_grid->column(col).left + cell->el->content_margins_left();
				}

				if (cell->rowspan <= 1)
				{
					m_grid->row(row).height = std::max(m_grid->row(row).height, cell->el->height());
				}
				else
				{
					row_span_found = true;
				}

			}
		}
	}

	if (row_span_found)
	{
		for (int col = 0; col < m_grid->cols_count(); col++)
		{
			for (int row = 0; row < m_grid->rows_count(); row++)
			{
				table_cell* cell = m_grid->cell(col, row);
				if (cell->el)
				{
					int span_row = row + cell->rowspan - 1;
					if (span_row >= m_grid->rows_count())
					{
						span_row = m_grid->rows_count() - 1;
					}
					if (span_row != row)
					{
						int h = 0;
						for (int i = row; i <= span_row; i++)
						{
							h += m_grid->row(i).height;
						}
						if (h < cell->el->height())
						{
							m_grid->row(span_row).height += cell->el->height() - h;
						}
					}
				}
			}
		}
	}

	// Calculate vertical table spacing
	int table_height_spacing = 0;
	if (m_border_collapse == border_collapse_separate)
	{
		table_height_spacing = m_border_spacing_y * (m_grid->rows_count() + 1);
	}
	else
	{
		table_height_spacing = 0;

		if (m_grid->rows_count())
		{
			table_height_spacing -= std::min(border_top(), m_grid->row(0).border_top);
			table_height_spacing -= std::min(border_bottom(), m_grid->row(m_grid->rows_count() - 1).border_bottom);
		}

		for (int row = 1; row < m_grid->rows_count(); row++)
		{
			table_height_spacing -= std::min(m_grid->row(row).border_top, m_grid->row(row - 1).border_bottom);
		}
	}


	// calculate block height
	int block_height = 0;
	if (get_predefined_height(block_height))
	{
		block_height -= m_padding.height() + m_borders.height();
	}

	// calculate minimum height from m_css_min_height
	int min_height = 0;
	if (!m_css_min_height.is_predefined() && m_css_min_height.units() == css_units_percentage)
	{
		element::ptr el_parent = parent();
		if (el_parent)
		{
			int parent_height = 0;
			if (el_parent->get_predefined_height(parent_height))
			{
				min_height = m_css_min_height.calc_percent(parent_height);
			}
		}
	}
	else
	{
		min_height = (int)m_css_min_height.val();
	}

	int minimum_table_height = std::max(block_height, min_height);

	m_grid->calc_rows_height(minimum_table_height - table_height_spacing, m_border_spacing_y);
	m_grid->calc_vertical_positions(m_borders, m_border_collapse, m_border_spacing_y);

	int table_height = 0;

	// place cells vertically
	for (int col = 0; col < m_grid->cols_count(); col++)
	{
		for (int row = 0; row < m_grid->rows_count(); row++)
		{
			table_cell* cell = m_grid->cell(col, row);
			if (cell->el)
			{
				int span_row = row + cell->rowspan - 1;
				if (span_row >= m_grid->rows_count())
				{
					span_row = m_grid->rows_count() - 1;
				}
				cell->el->m_pos.y = m_grid->row(row).top + cell->el->content_margins_top();
				cell->el->m_pos.height = m_grid->row(span_row).bottom - m_grid->row(row).top - cell->el->content_margins_top() - cell->el->content_margins_bottom();
				table_height = std::max(table_height, m_grid->row(span_row).bottom);
				cell->el->apply_vertical_align();
			}
		}
	}

	if (m_border_collapse == border_collapse_collapse)
	{
		if (m_grid->rows_count())
		{
			table_height -= std::min(border_bottom(), m_grid->row(m_grid->rows_count() - 1).border_bottom);
		}
	}
	else
	{
		table_height += m_border_spacing_y;
	}

	m_pos.width = table_width;

	// Render table captions
	// Table border doesn't round the caption so we have to start caption in the border position
	int captions_height = -border_top();

	for (auto& caption : m_grid->captions())
	{
		caption->render(-border_left(), captions_height, table_width + border_left() + border_right());
		captions_height += caption->height();
	}

	if (captions_height)
	{
		// Add border height to get the top of cells
		captions_height += border_top();

		// Save caption height for draw_background
		m_grid->captions_height(captions_height);

		// Move table cells to the bottom side
		for (int row = 0; row < m_grid->rows_count(); row++)
		{
			m_grid->row(row).el_row->m_pos.y += captions_height;
			for (int col = 0; col < m_grid->cols_count(); col++)
			{
				table_cell* cell = m_grid->cell(col, row);
				if (cell->el)
				{
					cell->el->m_pos.y += captions_height;
				}
			}
		}
	}

	calc_auto_margins(parent_width);

	m_pos.move_to(x, y);
	m_pos.x += content_margins_left();
	m_pos.y += content_margins_top();
	m_pos.width = table_width;
	m_pos.height = table_height + captions_height;

	return max_table_width;
}

void litehtml::html_tag::draw_children_box(uint_ptr hdc, int x, int y, const position* clip, draw_flag flag, int zindex)
{
	position pos = m_pos;
	pos.x += x;
	pos.y += y;

	document::ptr doc = get_document();

	if (m_overflow > overflow_visible)
	{
		position border_box = pos;
		border_box += m_padding;
		border_box += m_borders;

		border_radiuses bdr_radius = m_css_borders.radius.calc_percents(border_box.width, border_box.height);

		bdr_radius -= m_borders;
		bdr_radius -= m_padding;

		doc->container()->set_clip(pos, bdr_radius, true, true);
	}

	element::ptr el;
	for (auto& item : m_children)
	{
		el = item;
		if (el->is_visible())
		{
			switch (flag)
			{
			case draw_positioned:
				if (el->is_positioned() && el->get_zindex() == zindex)
				{
					if (el->get_element_position() == element_position_fixed)
					{
                        position browser_wnd;
                        doc->container()->get_client_rect(browser_wnd);

						el->draw(hdc, browser_wnd.x, browser_wnd.y, clip);
						el->draw_stacking_context(hdc, browser_wnd.x, browser_wnd.y, clip, true);
					}
					else
					{
						el->draw(hdc, pos.x, pos.y, clip);
						el->draw_stacking_context(hdc, pos.x, pos.y, clip, true);
					}
					el = nullptr;
				}
				break;
			case draw_block:
				if (!el->is_inline_box() && el->get_float() == float_none && !el->is_positioned())
				{
					el->draw(hdc, pos.x, pos.y, clip);
				}
				break;
			case draw_floats:
				if (el->get_float() != float_none && !el->is_positioned())
				{
					el->draw(hdc, pos.x, pos.y, clip);
					el->draw_stacking_context(hdc, pos.x, pos.y, clip, false);
					el = nullptr;
				}
				break;
			case draw_inlines:
				if (el->is_inline_box() && el->get_float() == float_none && !el->is_positioned())
				{
					el->draw(hdc, pos.x, pos.y, clip);
					if (el->get_display() == display_inline_block)
					{
						el->draw_stacking_context(hdc, pos.x, pos.y, clip, false);
						el = nullptr;
					}
				}
				break;
			default:
				break;
			}

			if (el)
			{
				if (flag == draw_positioned)
				{
					if (!el->is_positioned())
					{
						el->draw_children(hdc, pos.x, pos.y, clip, flag, zindex);
					}
				}
				else
				{
					if (el->get_float() == float_none &&
						el->get_display() != display_inline_block &&
						!el->is_positioned())
					{
						el->draw_children(hdc, pos.x, pos.y, clip, flag, zindex);
					}
				}
			}
		}
	}

	if (m_overflow > overflow_visible)
	{
		doc->container()->del_clip();
	}
}

void litehtml::html_tag::draw_children_table(uint_ptr hdc, int x, int y, const position* clip, draw_flag flag, int zindex)
{
	if (!m_grid) return;

	position pos = m_pos;
	pos.x += x;
	pos.y += y;
	for (auto& caption : m_grid->captions())
	{
        if (flag == draw_block)
        {
		    caption->draw(hdc, pos.x, pos.y, clip);
        }
		caption->draw_children(hdc, pos.x, pos.y, clip, flag, zindex);
	}
	for (int row = 0; row < m_grid->rows_count(); row++)
	{
		if (flag == draw_block)
		{
			m_grid->row(row).el_row->draw_background(hdc, pos.x, pos.y, clip);
		}
		for (int col = 0; col < m_grid->cols_count(); col++)
		{
			table_cell* cell = m_grid->cell(col, row);
			if (cell->el)
			{
				if (flag == draw_block)
				{
					cell->el->draw(hdc, pos.x, pos.y, clip);
				}
				cell->el->draw_children(hdc, pos.x, pos.y, clip, flag, zindex);
			}
		}
	}
}
