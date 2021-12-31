#include "html.h"
#include "el_table.h"
#include "document.h"
#include "iterators.h"


litehtml::el_table::el_table(const std::shared_ptr<litehtml::document>& doc) : html_tag(doc)
{
	m_border_spacing_x	= 0;
	m_border_spacing_y	= 0;
	m_border_collapse	= border_collapse_separate;
}


bool litehtml::el_table::appendChild(const litehtml::element::ptr& el)
{
	if(!el)	return false;
	if( !t_strcmp(el->get_tagName(), _t("tbody")) || 
		!t_strcmp(el->get_tagName(), _t("thead")) || 
		!t_strcmp(el->get_tagName(), _t("tfoot")) ||
		!t_strcmp(el->get_tagName(), _t("caption")))
	{
		return html_tag::appendChild(el);
	}
	return false;
}

void litehtml::el_table::parse_styles(bool is_reparse)
{
	html_tag::parse_styles(is_reparse);

	m_border_collapse = (border_collapse) value_index(get_style_property(_t("border-collapse"), true, _t("separate")), border_collapse_strings, border_collapse_separate);

	if(m_border_collapse == border_collapse_separate)
	{
		m_css_border_spacing_x.fromString(get_style_property(_t("-litehtml-border-spacing-x"), true, _t("0px")));
		m_css_border_spacing_y.fromString(get_style_property(_t("-litehtml-border-spacing-y"), true, _t("0px")));

		int fntsz = get_font_size();
		document::ptr doc = get_document();
		m_border_spacing_x = doc->cvt_units(m_css_border_spacing_x, fntsz);
		m_border_spacing_y = doc->cvt_units(m_css_border_spacing_y, fntsz);
	} else
	{
		m_border_spacing_x	= 0;
		m_border_spacing_y	= 0;
		m_padding.bottom	= 0;
		m_padding.top		= 0;
		m_padding.left		= 0;
		m_padding.right		= 0;
		m_css_padding.bottom.set_value(0, css_units_px);
		m_css_padding.top.set_value(0, css_units_px);
		m_css_padding.left.set_value(0, css_units_px);
		m_css_padding.right.set_value(0, css_units_px);
	}
}

void litehtml::el_table::parse_attributes()
{
	const tchar_t* str = get_attr(_t("width"));
	if(str)
	{
		m_style.add_property(_t("width"), str, nullptr, false);
	}

	str = get_attr(_t("align"));
	if(str)
	{
		int align = value_index(str, _t("left;center;right"));
		switch(align)
		{
		case 1:
			m_style.add_property(_t("margin-left"), _t("auto"), nullptr, false);
			m_style.add_property(_t("margin-right"), _t("auto"), nullptr, false);
			break;
		case 2:
			m_style.add_property(_t("margin-left"), _t("auto"), nullptr, false);
			m_style.add_property(_t("margin-right"), _t("0"), nullptr, false);
			break;
		}
	}

	str = get_attr(_t("cellspacing"));
	if(str)
	{
		tstring val = str;
		val += _t(" ");
		val += str;
		m_style.add_property(_t("border-spacing"), val.c_str(), nullptr, false);
	}
	
	str = get_attr(_t("border"));
	if(str)
	{
		m_style.add_property(_t("border-width"), str, nullptr, false);
	}

	str = get_attr(_t("bgcolor"));
	if (str)
	{
		m_style.add_property(_t("background-color"), str, nullptr, false);
	}

	html_tag::parse_attributes();
}
