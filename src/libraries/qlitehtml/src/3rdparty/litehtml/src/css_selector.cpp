#include "html.h"
#include "css_selector.h"
#include "document.h"

void litehtml::css_element_selector::parse( const tstring& txt )
{
	tstring::size_type el_end = txt.find_first_of(_t(".#[:"));
	m_tag = txt.substr(0, el_end);
	litehtml::lcase(m_tag);
	m_attrs.clear();
	while(el_end != tstring::npos)
	{
		if(txt[el_end] == _t('.'))
		{
			css_attribute_selector attribute;

			tstring::size_type pos = txt.find_first_of(_t(".#[:"), el_end + 1);
			attribute.val		= txt.substr(el_end + 1, pos - el_end - 1);
			split_string( attribute.val, attribute.class_val, _t(" ") );
			attribute.condition	= select_equal;
			attribute.attribute	= _t("class");
			m_attrs.push_back(attribute);
			el_end = pos;
		} else if(txt[el_end] == _t(':'))
		{
			css_attribute_selector attribute;

			if(txt[el_end + 1] == _t(':'))
			{
				tstring::size_type pos = txt.find_first_of(_t(".#[:"), el_end + 2);
				attribute.val		= txt.substr(el_end + 2, pos - el_end - 2);
				attribute.condition	= select_pseudo_element;
				litehtml::lcase(attribute.val);
				attribute.attribute	= _t("pseudo-el");
				m_attrs.push_back(attribute);
				el_end = pos;
			} else
			{
				tstring::size_type pos = txt.find_first_of(_t(".#[:("), el_end + 1);
				if(pos != tstring::npos && txt.at(pos) == _t('('))
				{
					pos = find_close_bracket(txt, pos);
					if(pos != tstring::npos)
					{
						pos++;
					}
				}
				if(pos != tstring::npos)
				{
					attribute.val		= txt.substr(el_end + 1, pos - el_end - 1);
				} else
				{
					attribute.val		= txt.substr(el_end + 1);
				}
				litehtml::lcase(attribute.val);
				if(attribute.val == _t("after") || attribute.val == _t("before"))
				{
					attribute.condition	= select_pseudo_element;
				} else
				{
					attribute.condition	= select_pseudo_class;
				}
				attribute.attribute	= _t("pseudo");
				m_attrs.push_back(attribute);
				el_end = pos;
			}
		} else if(txt[el_end] == _t('#'))
		{
			css_attribute_selector attribute;

			tstring::size_type pos = txt.find_first_of(_t(".#[:"), el_end + 1);
			attribute.val		= txt.substr(el_end + 1, pos - el_end - 1);
			attribute.condition	= select_equal;
			attribute.attribute	= _t("id");
			m_attrs.push_back(attribute);
			el_end = pos;
		} else if(txt[el_end] == _t('['))
		{
			css_attribute_selector attribute;

			tstring::size_type pos = txt.find_first_of(_t("]~=|$*^"), el_end + 1);
			tstring attr = txt.substr(el_end + 1, pos - el_end - 1);
			trim(attr);
			litehtml::lcase(attr);
			if(pos != tstring::npos)
			{
				if(txt[pos] == _t(']'))
				{
					attribute.condition = select_exists;
				} else if(txt[pos] == _t('='))
				{
					attribute.condition = select_equal;
					pos++;
				} else if(txt.substr(pos, 2) == _t("~="))
				{
					attribute.condition = select_contain_str;
					pos += 2;
				} else if(txt.substr(pos, 2) == _t("|="))
				{
					attribute.condition = select_start_str;
					pos += 2;
				} else if(txt.substr(pos, 2) == _t("^="))
				{
					attribute.condition = select_start_str;
					pos += 2;
				} else if(txt.substr(pos, 2) == _t("$="))
				{
					attribute.condition = select_end_str;
					pos += 2;
				} else if(txt.substr(pos, 2) == _t("*="))
				{
					attribute.condition = select_contain_str;
					pos += 2;
				} else
				{
					attribute.condition = select_exists;
					pos += 1;
				}
				pos = txt.find_first_not_of(_t(" \t"), pos);
				if(pos != tstring::npos)
				{
					if(txt[pos] == _t('"'))
					{
						tstring::size_type pos2 = txt.find_first_of(_t('\"'), pos + 1);
						attribute.val = txt.substr(pos + 1, pos2 == tstring::npos ? pos2 : (pos2 - pos - 1));
						pos = pos2 == tstring::npos ? pos2 : (pos2 + 1);
					} else if(txt[pos] == _t(']'))
					{
						pos ++;
					} else
					{
						tstring::size_type pos2 = txt.find_first_of(_t(']'), pos + 1);
						attribute.val = txt.substr(pos, pos2 == tstring::npos ? pos2 : (pos2 - pos));
						trim(attribute.val);
						pos = pos2 == tstring::npos ? pos2 : (pos2 + 1);
					}
				}
			} else
			{
				attribute.condition = select_exists;
			}
			attribute.attribute	= attr;
			m_attrs.push_back(attribute);
			el_end = pos;
		} else
		{
			el_end++;
		}
		el_end = txt.find_first_of(_t(".#[:"), el_end);
	}
}


bool litehtml::css_selector::parse( const tstring& text )
{
	if(text.empty())
	{
		return false;
	}
	string_vector tokens;
	split_string(text, tokens, _t(""), _t(" \t>+~"), _t("(["));

	if(tokens.empty())
	{
		return false;
	}

	tstring left;
	tstring right = tokens.back();
	tchar_t combinator = 0;

	tokens.pop_back();
	while(!tokens.empty() && (tokens.back() == _t(" ") || tokens.back() == _t("\t") || tokens.back() == _t("+") || tokens.back() == _t("~") || tokens.back() == _t(">")))
	{
		if(combinator == _t(' ') || combinator == 0)
		{
			combinator = tokens.back()[0];
		}
		tokens.pop_back();
	}

	for(const auto & token : tokens)
	{
		left += token;
	}

	trim(left);
	trim(right);

	if(right.empty())
	{
		return false;
	}

	m_right.parse(right);

	switch(combinator)
	{
	case _t('>'):
		m_combinator	= combinator_child;
		break;
	case _t('+'):
		m_combinator	= combinator_adjacent_sibling;
		break;
	case _t('~'):
		m_combinator	= combinator_general_sibling;
		break;
	default:
		m_combinator	= combinator_descendant;
		break;
	}

	m_left = nullptr;

	if(!left.empty())
	{
		m_left = std::make_shared<css_selector>(media_query_list::ptr(nullptr));
		if(!m_left->parse(left))
		{
			return false;
		}
	}

	return true;
}

void litehtml::css_selector::calc_specificity()
{
	if(!m_right.m_tag.empty() && m_right.m_tag != _t("*"))
	{
		m_specificity.d = 1;
	}
	for(const auto& attr : m_right.m_attrs)
	{
		if(attr.attribute == _t("id"))
		{
			m_specificity.b++;
		} else
		{
			if(attr.attribute == _t("class"))
			{
				m_specificity.c += (int) attr.class_val.size();
			} else
			{
				m_specificity.c++;
			}
		}	
	}
	if(m_left)
	{
		m_left->calc_specificity();
		m_specificity += m_left->m_specificity;
	}
}

void litehtml::css_selector::add_media_to_doc( document* doc ) const
{
	if(m_media_query && doc)
	{
		doc->add_media_list(m_media_query);
	}
}

