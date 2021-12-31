#include "html.h"
#include "document.h"
#include "stylesheet.h"
#include "html_tag.h"
#include "el_text.h"
#include "el_para.h"
#include "el_space.h"
#include "el_body.h"
#include "el_image.h"
#include "el_table.h"
#include "el_td.h"
#include "el_link.h"
#include "el_title.h"
#include "el_style.h"
#include "el_script.h"
#include "el_comment.h"
#include "el_cdata.h"
#include "el_base.h"
#include "el_anchor.h"
#include "el_break.h"
#include "el_div.h"
#include "el_font.h"
#include "el_tr.h"
#include "el_li.h"
#include <cmath>
#include <cstdio>
#include <algorithm>
#include <functional>
#include "gumbo.h"
#include "utf8_strings.h"

litehtml::document::document(litehtml::document_container* objContainer, litehtml::context* ctx)
{
	m_container	= objContainer;
	m_context	= ctx;
}

litehtml::document::~document()
{
	m_over_element = nullptr;
	if(m_container)
	{
		for(auto & m_font : m_fonts)
		{
			m_container->delete_font(m_font.second.font);
		}
	}
}

litehtml::document::ptr litehtml::document::createFromString( const tchar_t* str, litehtml::document_container* objPainter, litehtml::context* ctx, litehtml::css* user_styles)
{
	return createFromUTF8(litehtml_to_utf8(str), objPainter, ctx, user_styles);
}

litehtml::document::ptr litehtml::document::createFromUTF8(const char* str, litehtml::document_container* objPainter, litehtml::context* ctx, litehtml::css* user_styles)
{
	// parse document into GumboOutput
	GumboOutput* output = gumbo_parse((const char*) str);

	// Create litehtml::document
	litehtml::document::ptr doc = std::make_shared<litehtml::document>(objPainter, ctx);

	// Create litehtml::elements.
	elements_vector root_elements;
	doc->create_node(output->root, root_elements, true);
	if (!root_elements.empty())
	{
		doc->m_root = root_elements.back();
	}
	// Destroy GumboOutput
	gumbo_destroy_output(&kGumboDefaultOptions, output);

	// Let's process created elements tree
	if (doc->m_root)
	{
		doc->container()->get_media_features(doc->m_media);

		// apply master CSS
		doc->m_root->apply_stylesheet(ctx->master_css());

		// parse elements attributes
		doc->m_root->parse_attributes();

		// parse style sheets linked in document
		media_query_list::ptr media;
		for (const auto& css : doc->m_css)
		{
			if (!css.media.empty())
			{
				media = media_query_list::create_from_string(css.media, doc);
			}
			else
			{
				media = nullptr;
			}
			doc->m_styles.parse_stylesheet(css.text.c_str(), css.baseurl.c_str(), doc, media);
		}
		// Sort css selectors using CSS rules.
		doc->m_styles.sort_selectors();

		// get current media features
		if (!doc->m_media_lists.empty())
		{
			doc->update_media_lists(doc->m_media);
		}

		// Apply parsed styles.
		doc->m_root->apply_stylesheet(doc->m_styles);

		// Apply user styles if any
		if (user_styles)
		{
			doc->m_root->apply_stylesheet(*user_styles);
		}

		// Parse applied styles in the elements
		doc->m_root->parse_styles();

		// Now the m_tabular_elements is filled with tabular elements.
		// We have to check the tabular elements for missing table elements 
		// and create the anonymous boxes in visual table layout
		doc->fix_tables_layout();

		// Fanaly initialize elements
		doc->m_root->init();
	}

	return doc;
}

litehtml::uint_ptr litehtml::document::add_font( const tchar_t* name, int size, const tchar_t* weight, const tchar_t* style, const tchar_t* decoration, font_metrics* fm )
{
	uint_ptr ret = 0;

	if(!name || !t_strcasecmp(name, _t("inherit")))
	{
		name = m_container->get_default_font_name();
	}

	if(!size)
	{
		size = container()->get_default_font_size();
	}

	tchar_t strSize[20];
	t_itoa(size, strSize, 20, 10);

	tstring key = name;
	key += _t(":");
	key += strSize;
	key += _t(":");
	key += weight;
	key += _t(":");
	key += style;
	key += _t(":");
	key += decoration;

	if(m_fonts.find(key) == m_fonts.end())
	{
		font_style fs = (font_style) value_index(style, font_style_strings, fontStyleNormal);
		int	fw = value_index(weight, font_weight_strings, -1);
		if(fw >= 0)
		{
			switch(fw)
			{
			case litehtml::fontWeightBold:
				fw = 700;
				break;
			case litehtml::fontWeightBolder:
				fw = 600;
				break;
			case litehtml::fontWeightLighter:
				fw = 300;
				break;
			default:
				fw = 400;
				break;
			}
		} else
		{
			fw = t_atoi(weight);
			if(fw < 100)
			{
				fw = 400;
			}
		}

		unsigned int decor = 0;

		if(decoration)
		{
			std::vector<tstring> tokens;
			split_string(decoration, tokens, _t(" "));
			for(auto & token : tokens)
			{
				if(!t_strcasecmp(token.c_str(), _t("underline")))
				{
					decor |= font_decoration_underline;
				} else if(!t_strcasecmp(token.c_str(), _t("line-through")))
				{
					decor |= font_decoration_linethrough;
				} else if(!t_strcasecmp(token.c_str(), _t("overline")))
				{
					decor |= font_decoration_overline;
				}
			}
		}

		font_item fi= {0};

		fi.font = m_container->create_font(name, size, fw, fs, decor, &fi.metrics);
		m_fonts[key] = fi;
		ret = fi.font;
		if(fm)
		{
			*fm = fi.metrics;
		}
	}
	return ret;
}

litehtml::uint_ptr litehtml::document::get_font( const tchar_t* name, int size, const tchar_t* weight, const tchar_t* style, const tchar_t* decoration, font_metrics* fm )
{
	if(!name || !t_strcasecmp(name, _t("inherit")))
	{
		name = m_container->get_default_font_name();
	}

	if(!size)
	{
		size = m_container->get_default_font_size();
	}

	tchar_t strSize[20];
	t_itoa(size, strSize, 20, 10);

	tstring key = name;
	key += _t(":");
	key += strSize;
	key += _t(":");
	key += weight;
	key += _t(":");
	key += style;
	key += _t(":");
	key += decoration;

	auto el = m_fonts.find(key);

	if(el != m_fonts.end())
	{
		if(fm)
		{
			*fm = el->second.metrics;
		}
		return el->second.font;
	}
	return add_font(name, size, weight, style, decoration, fm);
}

int litehtml::document::render( int max_width, render_type rt )
{
	int ret = 0;
	if(m_root)
	{
		if(rt == render_fixed_only)
		{
			m_fixed_boxes.clear();
			m_root->render_positioned(rt);
		} else
		{
			ret = m_root->render(0, 0, max_width);
			if(m_root->fetch_positioned())
			{
				m_fixed_boxes.clear();
				m_root->render_positioned(rt);
			}
			m_size.width	= 0;
			m_size.height	= 0;
			m_root->calc_document_size(m_size);
		}
	}
	return ret;
}

void litehtml::document::draw( uint_ptr hdc, int x, int y, const position* clip )
{
	if(m_root)
	{
		m_root->draw(hdc, x, y, clip);
		m_root->draw_stacking_context(hdc, x, y, clip, true);
	}
}

int litehtml::document::cvt_units( const tchar_t* str, int fontSize, bool* is_percent/*= 0*/ ) const
{
	if(!str)	return 0;
	
	css_length val;
	val.fromString(str);
	if(is_percent && val.units() == css_units_percentage && !val.is_predefined())
	{
		*is_percent = true;
	}
	return cvt_units(val, fontSize);
}

int litehtml::document::cvt_units( css_length& val, int fontSize, int size ) const
{
	if(val.is_predefined())
	{
		return 0;
	}
	int ret;
	switch(val.units())
	{
	case css_units_percentage:
		ret = val.calc_percent(size);
		break;
	case css_units_em:
		ret = round_f(val.val() * (float) fontSize);
		val.set_value((float) ret, css_units_px);
		break;
	case css_units_pt:
		ret = m_container->pt_to_px((int) val.val());
		val.set_value((float) ret, css_units_px);
		break;
	case css_units_in:
		ret = m_container->pt_to_px((int) (val.val() * 72));
		val.set_value((float) ret, css_units_px);
		break;
	case css_units_cm:
		ret = m_container->pt_to_px((int) (val.val() * 0.3937 * 72));
		val.set_value((float) ret, css_units_px);
		break;
	case css_units_mm:
		ret = m_container->pt_to_px((int) (val.val() * 0.3937 * 72) / 10);
		val.set_value((float) ret, css_units_px);
		break;
	case css_units_vw:
		ret = (int)((double)m_media.width * (double)val.val() / 100.0);
		break;
	case css_units_vh:
		ret = (int)((double)m_media.height * (double)val.val() / 100.0);
		break;
	case css_units_vmin:
		ret = (int)((double)std::min(m_media.height, m_media.width) * (double)val.val() / 100.0);
		break;
	case css_units_vmax:
		ret = (int)((double)std::max(m_media.height, m_media.width) * (double)val.val() / 100.0);
		break;
	case css_units_rem:
		ret = (int) ((double) m_root->get_font_size() * (double) val.val());
		val.set_value((float) ret, css_units_px);
		break;
	default:
		ret = (int) val.val();
		break;
	}
	return ret;
}

int litehtml::document::width() const
{
	return m_size.width;
}

int litehtml::document::height() const
{
	return m_size.height;
}

void litehtml::document::add_stylesheet( const tchar_t* str, const tchar_t* baseurl, const tchar_t* media )
{
	if(str && str[0])
	{
		m_css.push_back(css_text(str, baseurl, media));
	}
}

bool litehtml::document::on_mouse_over( int x, int y, int client_x, int client_y, position::vector& redraw_boxes )
{
	if(!m_root)
	{
		return false;
	}

	element::ptr over_el = m_root->get_element_by_point(x, y, client_x, client_y);

	bool state_was_changed = false;

	if(over_el != m_over_element)
	{
		if(m_over_element)
		{
			if(m_over_element->on_mouse_leave())
			{
				state_was_changed = true;
			}
		}
		m_over_element = over_el;
	}

	const tchar_t* cursor = nullptr;

	if(m_over_element)
	{
		if(m_over_element->on_mouse_over())
		{
			state_was_changed = true;
		}
		cursor = m_over_element->get_cursor();
	}
	
	m_container->set_cursor(cursor ? cursor : _t("auto"));
	
	if(state_was_changed)
	{
		return m_root->find_styles_changes(redraw_boxes, 0, 0);
	}
	return false;
}

bool litehtml::document::on_mouse_leave( position::vector& redraw_boxes )
{
	if(!m_root)
	{
		return false;
	}
	if(m_over_element)
	{
		if(m_over_element->on_mouse_leave())
		{
			return m_root->find_styles_changes(redraw_boxes, 0, 0);
		}
	}
	return false;
}

bool litehtml::document::on_lbutton_down( int x, int y, int client_x, int client_y, position::vector& redraw_boxes )
{
	if(!m_root)
	{
		return false;
	}

	element::ptr over_el = m_root->get_element_by_point(x, y, client_x, client_y);

	bool state_was_changed = false;

	if(over_el != m_over_element)
	{
		if(m_over_element)
		{
			if(m_over_element->on_mouse_leave())
			{
				state_was_changed = true;
			}
		}
		m_over_element = over_el;
		if(m_over_element)
		{
			if(m_over_element->on_mouse_over())
			{
				state_was_changed = true;
			}
		}
	}

	const tchar_t* cursor = nullptr;

	if(m_over_element)
	{
		if(m_over_element->on_lbutton_down())
		{
			state_was_changed = true;
		}
		cursor = m_over_element->get_cursor();
	}

	m_container->set_cursor(cursor ? cursor : _t("auto"));

	if(state_was_changed)
	{
		return m_root->find_styles_changes(redraw_boxes, 0, 0);
	}

	return false;
}

bool litehtml::document::on_lbutton_up( int x, int y, int client_x, int client_y, position::vector& redraw_boxes )
{
	if(!m_root)
	{
		return false;
	}
	if(m_over_element)
	{
		if(m_over_element->on_lbutton_up())
		{
			return m_root->find_styles_changes(redraw_boxes, 0, 0);
		}
	}
	return false;
}

litehtml::element::ptr litehtml::document::create_element(const tchar_t* tag_name, const string_map& attributes)
{
	element::ptr newTag;
	document::ptr this_doc = shared_from_this();
	if(m_container)
	{
		newTag = m_container->create_element(tag_name, attributes, this_doc);
	}
	if(!newTag)
	{
		if(!t_strcmp(tag_name, _t("br")))
		{
			newTag = std::make_shared<litehtml::el_break>(this_doc);
		} else if(!t_strcmp(tag_name, _t("p")))
		{
			newTag = std::make_shared<litehtml::el_para>(this_doc);
		} else if(!t_strcmp(tag_name, _t("img")))
		{
			newTag = std::make_shared<litehtml::el_image>(this_doc);
		} else if(!t_strcmp(tag_name, _t("table")))
		{
			newTag = std::make_shared<litehtml::el_table>(this_doc);
		} else if(!t_strcmp(tag_name, _t("td")) || !t_strcmp(tag_name, _t("th")))
		{
			newTag = std::make_shared<litehtml::el_td>(this_doc);
		} else if(!t_strcmp(tag_name, _t("link")))
		{
			newTag = std::make_shared<litehtml::el_link>(this_doc);
		} else if(!t_strcmp(tag_name, _t("title")))
		{
			newTag = std::make_shared<litehtml::el_title>(this_doc);
		} else if(!t_strcmp(tag_name, _t("a")))
		{
			newTag = std::make_shared<litehtml::el_anchor>(this_doc);
		} else if(!t_strcmp(tag_name, _t("tr")))
		{
			newTag = std::make_shared<litehtml::el_tr>(this_doc);
		} else if(!t_strcmp(tag_name, _t("style")))
		{
			newTag = std::make_shared<litehtml::el_style>(this_doc);
		} else if(!t_strcmp(tag_name, _t("base")))
		{
			newTag = std::make_shared<litehtml::el_base>(this_doc);
		} else if(!t_strcmp(tag_name, _t("body")))
		{
			newTag = std::make_shared<litehtml::el_body>(this_doc);
		} else if(!t_strcmp(tag_name, _t("div")))
		{
			newTag = std::make_shared<litehtml::el_div>(this_doc);
		} else if(!t_strcmp(tag_name, _t("script")))
		{
			newTag = std::make_shared<litehtml::el_script>(this_doc);
		} else if(!t_strcmp(tag_name, _t("font")))
		{
			newTag = std::make_shared<litehtml::el_font>(this_doc);
		} else if(!t_strcmp(tag_name, _t("li")))
		{
			newTag = std::make_shared<litehtml::el_li>(this_doc);
		} else
		{
			newTag = std::make_shared<litehtml::html_tag>(this_doc);
		}
	}

	if(newTag)
	{
		newTag->set_tagName(tag_name);
		for (const auto & attribute : attributes)
		{
			newTag->set_attr(attribute.first.c_str(), attribute.second.c_str());
		}
	}

	return newTag;
}

void litehtml::document::get_fixed_boxes( position::vector& fixed_boxes )
{
	fixed_boxes = m_fixed_boxes;
}

void litehtml::document::add_fixed_box( const position& pos )
{
	m_fixed_boxes.push_back(pos);
}

bool litehtml::document::media_changed()
{
	if(!m_media_lists.empty())
	{
		container()->get_media_features(m_media);
		if (update_media_lists(m_media))
		{
			m_root->refresh_styles();
			m_root->parse_styles();
			return true;
		}
	}
	return false;
}

bool litehtml::document::lang_changed()
{
	if(!m_media_lists.empty())
	{
		tstring culture;
		container()->get_language(m_lang, culture);
		if(!culture.empty())
		{
			m_culture = m_lang + _t('-') + culture;
		}
		else
		{
			m_culture.clear();
		}
		m_root->refresh_styles();
		m_root->parse_styles();
		return true;
	}
	return false;
}

bool litehtml::document::update_media_lists(const media_features& features)
{
	bool update_styles = false;
	for(auto & m_media_list : m_media_lists)
	{
		if(m_media_list->apply_media_features(features))
		{
			update_styles = true;
		}
	}
	return update_styles;
}

void litehtml::document::add_media_list( const media_query_list::ptr& list )
{
	if(list)
	{
		if(std::find(m_media_lists.begin(), m_media_lists.end(), list) == m_media_lists.end())
		{
			m_media_lists.push_back(list);
		}
	}
}

void litehtml::document::create_node(void* gnode, elements_vector& elements, bool parseTextNode)
{
	auto* node = (GumboNode*)gnode;
	switch (node->type)
	{
	case GUMBO_NODE_ELEMENT:
		{
			string_map attrs;
			GumboAttribute* attr;
			for (unsigned int i = 0; i < node->v.element.attributes.length; i++)
			{
				attr = (GumboAttribute*)node->v.element.attributes.data[i];
				attrs[tstring(litehtml_from_utf8(attr->name))] = litehtml_from_utf8(attr->value);
			}


			element::ptr ret;
			const char* tag = gumbo_normalized_tagname(node->v.element.tag);
			if (tag[0])
			{
				ret = create_element(litehtml_from_utf8(tag), attrs);
			}
			else
			{
				if (node->v.element.original_tag.data && node->v.element.original_tag.length)
				{
					std::string strA;
					gumbo_tag_from_original_text(&node->v.element.original_tag);
					strA.append(node->v.element.original_tag.data, node->v.element.original_tag.length);
					ret = create_element(litehtml_from_utf8(strA.c_str()), attrs);
				}
			}
			if (!strcmp(tag, "script"))
			{
				parseTextNode = false;
			}
			if (ret)
			{
				elements_vector child;
				for (unsigned int i = 0; i < node->v.element.children.length; i++)
				{
					child.clear();
					create_node(static_cast<GumboNode*> (node->v.element.children.data[i]), child, parseTextNode);
					std::for_each(child.begin(), child.end(), 
						[&ret](element::ptr& el)
						{
							ret->appendChild(el);
						}
					);
				}
				elements.push_back(ret);
			}
		}
		break;
	case GUMBO_NODE_TEXT:
		{
			std::wstring str;
			std::wstring str_in = (const wchar_t*) (utf8_to_wchar(node->v.text.text));
			if (!parseTextNode)
			{
				elements.push_back(std::make_shared<el_text>(litehtml_from_wchar(str_in).c_str(), shared_from_this()));
			}
			else
			{
				m_container->split_text(node->v.text.text,
					[this, &elements](const tchar_t* text) { elements.push_back(std::make_shared<el_text>(text, shared_from_this())); },
					[this, &elements](const tchar_t* text) { elements.push_back(std::make_shared<el_space>(text, shared_from_this())); });
			}
		}
		break;
	case GUMBO_NODE_CDATA:
		{
			element::ptr ret = std::make_shared<el_cdata>(shared_from_this());
			ret->set_data(litehtml_from_utf8(node->v.text.text));
			elements.push_back(ret);
		}
		break;
	case GUMBO_NODE_COMMENT:
		{
			element::ptr ret = std::make_shared<el_comment>(shared_from_this());
			ret->set_data(litehtml_from_utf8(node->v.text.text));
			elements.push_back(ret);
		}
		break;
	case GUMBO_NODE_WHITESPACE:
		{
			tstring str = litehtml_from_utf8(node->v.text.text);
			for (size_t i = 0; i < str.length(); i++)
			{
				elements.push_back(std::make_shared<el_space>(str.substr(i, 1).c_str(), shared_from_this()));
			}
		}
		break;
	default:
		break;
	}
}

void litehtml::document::fix_tables_layout()
{
	size_t i = 0;
	while (i < m_tabular_elements.size())
	{
		element::ptr el_ptr = m_tabular_elements[i];

		switch (el_ptr->get_display())
		{
		case display_inline_table:
		case display_table:
			fix_table_children(el_ptr, display_table_row_group, _t("table-row-group"));
			break;
		case display_table_footer_group:
		case display_table_row_group:
		case display_table_header_group:
			{
				element::ptr parent = el_ptr->parent();
				if (parent)
				{
					if (parent->get_display() != display_inline_table)
						fix_table_parent(el_ptr, display_table, _t("table"));
				}
				fix_table_children(el_ptr, display_table_row, _t("table-row"));
			}
			break;
		case display_table_row:
			fix_table_parent(el_ptr, display_table_row_group, _t("table-row-group"));
			fix_table_children(el_ptr, display_table_cell, _t("table-cell"));
			break;
		case display_table_cell:
			fix_table_parent(el_ptr, display_table_row, _t("table-row"));
			break;
		// TODO: make table layout fix for table-caption, table-column etc. elements
		case display_table_caption:
		case display_table_column:
		case display_table_column_group:
		default:
			break;
		}
		i++;
	}
}

void litehtml::document::fix_table_children(element::ptr& el_ptr, style_display disp, const tchar_t* disp_str)
{
	elements_vector tmp;
	auto first_iter = el_ptr->m_children.begin();
	auto cur_iter = el_ptr->m_children.begin();

	auto flush_elements = [&]()
	{
		element::ptr annon_tag = std::make_shared<html_tag>(shared_from_this());
		style st;
		st.add_property(_t("display"), disp_str, nullptr, false);
		annon_tag->add_style(st);
		annon_tag->parent(el_ptr);
		annon_tag->parse_styles();
		std::for_each(tmp.begin(), tmp.end(),
			[&annon_tag](element::ptr& el)
			{
				annon_tag->appendChild(el);
			}
		);
		first_iter = el_ptr->m_children.insert(first_iter, annon_tag);
		cur_iter = first_iter + 1;
		while (cur_iter != el_ptr->m_children.end() && (*cur_iter)->parent() != el_ptr)
		{
			cur_iter = el_ptr->m_children.erase(cur_iter);
		}
		first_iter = cur_iter;
		tmp.clear();
	};

	while (cur_iter != el_ptr->m_children.end())
	{
		if ((*cur_iter)->get_display() != disp)
		{
			if (!(*cur_iter)->is_white_space() || ((*cur_iter)->is_white_space() && !tmp.empty()))
			{
				if (disp != display_table_row_group || (*cur_iter)->get_display() != display_table_caption)
				{
					if (tmp.empty())
					{
						first_iter = cur_iter;
					}
					tmp.push_back((*cur_iter));
				}
			}
			cur_iter++;
		}
		else if (!tmp.empty())
		{
			flush_elements();
		}
		else
		{
			cur_iter++;
		}
	}
	if (!tmp.empty())
	{
		flush_elements();
	}
}

void litehtml::document::fix_table_parent(element::ptr& el_ptr, style_display disp, const tchar_t* disp_str)
{
	element::ptr parent = el_ptr->parent();

	if (parent->get_display() != disp)
	{
		auto this_element = std::find_if(parent->m_children.begin(), parent->m_children.end(),
			[&](element::ptr& el)
			{
				if (el == el_ptr)
				{
					return true;
				}
				return false;
			}
		);
		if (this_element != parent->m_children.end())
		{
			style_display el_disp = el_ptr->get_display();
			auto first = this_element;
			auto last = this_element;
			auto cur = this_element;

			// find first element with same display
			while (true)
			{
				if (cur == parent->m_children.begin()) break;
				cur--;
				if ((*cur)->is_white_space() || (*cur)->get_display() == el_disp)
				{
					first = cur;
				}
				else
				{
					break;
				}
			}

			// find last element with same display
			cur = this_element;
			while (true)
			{
				cur++;
				if (cur == parent->m_children.end()) break;

				if ((*cur)->is_white_space() || (*cur)->get_display() == el_disp)
				{
					last = cur;
				}
				else
				{
					break;
				}
			}

			// extract elements with the same display and wrap them with anonymous object
			element::ptr annon_tag = std::make_shared<html_tag>(shared_from_this());
			style st;
			st.add_property(_t("display"), disp_str, nullptr, false);
			annon_tag->add_style(st);
			annon_tag->parent(parent);
			annon_tag->parse_styles();
			std::for_each(first, last + 1,
				[&annon_tag](element::ptr& el)
				{
					annon_tag->appendChild(el);
				}
			);
			first = parent->m_children.erase(first, last + 1);
			parent->m_children.insert(first, annon_tag);
		}
	}
}

void litehtml::document::append_children_from_string(element& parent, const tchar_t* str)
{
	append_children_from_utf8(parent, litehtml_to_utf8(str));
}

void litehtml::document::append_children_from_utf8(element& parent, const char* str)
{
	// parent must belong to this document
	if (parent.get_document().get() != this)
	{
		return;
	}

	// parse document into GumboOutput
	GumboOutput* output = gumbo_parse((const char*) str);

	// Create litehtml::elements.
	elements_vector child_elements;
	create_node(output->root, child_elements, true);

	// Destroy GumboOutput
	gumbo_destroy_output(&kGumboDefaultOptions, output);

	// Let's process created elements tree
	for (const auto& child : child_elements)
	{
		// Add the child element to parent
		parent.appendChild(child);

		// apply master CSS
		child->apply_stylesheet(m_context->master_css());

		// parse elements attributes
		child->parse_attributes();

		// Apply parsed styles.
		child->apply_stylesheet(m_styles);

		// Parse applied styles in the elements
		child->parse_styles();

		// Now the m_tabular_elements is filled with tabular elements.
		// We have to check the tabular elements for missing table elements 
		// and create the anonymous boxes in visual table layout
		fix_tables_layout();

		// Fanaly initialize elements
		child->init();
	}
}