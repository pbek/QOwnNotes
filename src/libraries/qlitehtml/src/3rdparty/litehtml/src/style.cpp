#include "html.h"
#include "style.h"
#ifndef WINCE
#include <locale>
#endif

litehtml::string_map litehtml::style::m_valid_values =
{
	{ _t("white-space"), white_space_strings }
};

litehtml::style::style( const style& val )
{
	m_properties = val.m_properties;
}

void litehtml::style::parse( const tchar_t* txt, const tchar_t* baseurl )
{
	std::vector<tstring> properties;
	split_string(txt, properties, _t(";"), _t(""), _t("\"'"));

	for(const auto & property : properties)
	{
		parse_property(property, baseurl);
	}
}

void litehtml::style::parse_property( const tstring& txt, const tchar_t* baseurl )
{
	tstring::size_type pos = txt.find_first_of(_t(':'));
	if(pos != tstring::npos)
	{
		tstring name = txt.substr(0, pos);
		tstring val	= txt.substr(pos + 1);

		trim(name); lcase(name);
		trim(val);

		if(!name.empty() && !val.empty())
		{
			string_vector vals;
			split_string(val, vals, _t("!"));
			if(vals.size() == 1)
			{
				add_property(name.c_str(), val.c_str(), baseurl, false);
			} else if(vals.size() > 1)
			{
				trim(vals[0]);
				lcase(vals[1]);
				add_property(name.c_str(), vals[0].c_str(), baseurl, vals[1] == _t("important"));
			}
		}
	}
}

void litehtml::style::combine( const litehtml::style& src )
{
	for(const auto& property : src.m_properties)
	{
		add_parsed_property(property.first, property.second.m_value, property.second.m_important);
	}
}

void litehtml::style::add_property( const tchar_t* name, const tchar_t* val, const tchar_t* baseurl, bool important )
{
	if(!name || !val)
	{
		return;
	}

	// Add baseurl for background image 
	if(	!t_strcmp(name, _t("background-image")))
	{
		add_parsed_property(name, val, important);
		if(baseurl)
		{
			add_parsed_property(_t("background-image-baseurl"), baseurl, important);
		}
	} else

	// Parse border spacing properties 
	if(	!t_strcmp(name, _t("border-spacing")))
	{
		string_vector tokens;
		split_string(val, tokens, _t(" "));
		if(tokens.size() == 1)
		{
			add_parsed_property(_t("-litehtml-border-spacing-x"), tokens[0], important);
			add_parsed_property(_t("-litehtml-border-spacing-y"), tokens[0], important);
		} else if(tokens.size() == 2)
		{
			add_parsed_property(_t("-litehtml-border-spacing-x"), tokens[0], important);
			add_parsed_property(_t("-litehtml-border-spacing-y"), tokens[1], important);
		}
	} else

	// Parse borders shorthand properties 

	if(	!t_strcmp(name, _t("border")))
	{
		string_vector tokens;
		split_string(val, tokens, _t(" "), _t(""), _t("("));
		int idx;
		tstring str;
		for(const auto& token : tokens)
		{
			idx = value_index(token, border_style_strings, -1);
			if(idx >= 0)
			{
				add_property(_t("border-left-style"), token.c_str(), baseurl, important);
				add_property(_t("border-right-style"), token.c_str(), baseurl, important);
				add_property(_t("border-top-style"), token.c_str(), baseurl, important);
				add_property(_t("border-bottom-style"), token.c_str(), baseurl, important);
			} else
			{
				if (t_isdigit(token[0]) || token[0] == _t('.') ||
					value_in_list(token, _t("thin;medium;thick")))
				{
					add_property(_t("border-left-width"), token.c_str(), baseurl, important);
					add_property(_t("border-right-width"), token.c_str(), baseurl, important);
					add_property(_t("border-top-width"), token.c_str(), baseurl, important);
					add_property(_t("border-bottom-width"), token.c_str(), baseurl, important);
				} 
				else
				{
					add_property(_t("border-left-color"), token.c_str(), baseurl, important);
					add_property(_t("border-right-color"), token.c_str(), baseurl, important);
					add_property(_t("border-top-color"), token.c_str(), baseurl, important);
					add_property(_t("border-bottom-color"), token.c_str(), baseurl, important);
				}
			}
		}
	} else if(	!t_strcmp(name, _t("border-left"))	||
		!t_strcmp(name, _t("border-right"))	||
		!t_strcmp(name, _t("border-top"))	||
		!t_strcmp(name, _t("border-bottom")) )
	{
		string_vector tokens;
		split_string(val, tokens, _t(" "), _t(""), _t("("));
		int idx;
		tstring str;
		for(const auto& token : tokens)
		{
			idx = value_index(token, border_style_strings, -1);
			if(idx >= 0)
			{
				str = name;
				str += _t("-style");
				add_property(str.c_str(), token.c_str(), baseurl, important);
			} else
			{
				if(web_color::is_color(token.c_str()))
				{
					str = name;
					str += _t("-color");
					add_property(str.c_str(), token.c_str(), baseurl, important);
				} else
				{
					str = name;
					str += _t("-width");
					add_property(str.c_str(), token.c_str(), baseurl, important);
				}
			}
		}
	} else 

	// Parse border radius shorthand properties 
	if(!t_strcmp(name, _t("border-bottom-left-radius")))
	{
		string_vector tokens;
		split_string(val, tokens, _t(" "));
		if(tokens.size() >= 2)
		{
			add_property(_t("border-bottom-left-radius-x"), tokens[0].c_str(), baseurl, important);
			add_property(_t("border-bottom-left-radius-y"), tokens[1].c_str(), baseurl, important);
		} else if(tokens.size() == 1)
		{
			add_property(_t("border-bottom-left-radius-x"), tokens[0].c_str(), baseurl, important);
			add_property(_t("border-bottom-left-radius-y"), tokens[0].c_str(), baseurl, important);
		}

	} else if(!t_strcmp(name, _t("border-bottom-right-radius")))
	{
		string_vector tokens;
		split_string(val, tokens, _t(" "));
		if(tokens.size() >= 2)
		{
			add_property(_t("border-bottom-right-radius-x"), tokens[0].c_str(), baseurl, important);
			add_property(_t("border-bottom-right-radius-y"), tokens[1].c_str(), baseurl, important);
		} else if(tokens.size() == 1)
		{
			add_property(_t("border-bottom-right-radius-x"), tokens[0].c_str(), baseurl, important);
			add_property(_t("border-bottom-right-radius-y"), tokens[0].c_str(), baseurl, important);
		}

	} else if(!t_strcmp(name, _t("border-top-right-radius")))
	{
		string_vector tokens;
		split_string(val, tokens, _t(" "));
		if(tokens.size() >= 2)
		{
			add_property(_t("border-top-right-radius-x"), tokens[0].c_str(), baseurl, important);
			add_property(_t("border-top-right-radius-y"), tokens[1].c_str(), baseurl, important);
		} else if(tokens.size() == 1)
		{
			add_property(_t("border-top-right-radius-x"), tokens[0].c_str(), baseurl, important);
			add_property(_t("border-top-right-radius-y"), tokens[0].c_str(), baseurl, important);
		}

	} else if(!t_strcmp(name, _t("border-top-left-radius")))
	{
		string_vector tokens;
		split_string(val, tokens, _t(" "));
		if(tokens.size() >= 2)
		{
			add_property(_t("border-top-left-radius-x"), tokens[0].c_str(), baseurl, important);
			add_property(_t("border-top-left-radius-y"), tokens[1].c_str(), baseurl, important);
		} else if(tokens.size() == 1)
		{
			add_property(_t("border-top-left-radius-x"), tokens[0].c_str(), baseurl, important);
			add_property(_t("border-top-left-radius-y"), tokens[0].c_str(), baseurl, important);
		}

	} else 

	// Parse border-radius shorthand properties 
	if(!t_strcmp(name, _t("border-radius")))
	{
		string_vector tokens;
		split_string(val, tokens, _t("/"));
		if(tokens.size() == 1)
		{
			add_property(_t("border-radius-x"), tokens[0].c_str(), baseurl, important);
			add_property(_t("border-radius-y"), tokens[0].c_str(), baseurl, important);
		} else if(tokens.size() >= 2)
		{
			add_property(_t("border-radius-x"), tokens[0].c_str(), baseurl, important);
			add_property(_t("border-radius-y"), tokens[1].c_str(), baseurl, important);
		}
	} else if(!t_strcmp(name, _t("border-radius-x")))
	{
		string_vector tokens;
		split_string(val, tokens, _t(" "));
		if(tokens.size() == 1)
		{
			add_property(_t("border-top-left-radius-x"),		tokens[0].c_str(), baseurl, important);
			add_property(_t("border-top-right-radius-x"),		tokens[0].c_str(), baseurl, important);
			add_property(_t("border-bottom-right-radius-x"),	tokens[0].c_str(), baseurl, important);
			add_property(_t("border-bottom-left-radius-x"),	tokens[0].c_str(), baseurl, important);
		} else if(tokens.size() == 2)
		{
			add_property(_t("border-top-left-radius-x"),		tokens[0].c_str(), baseurl, important);
			add_property(_t("border-top-right-radius-x"),		tokens[1].c_str(), baseurl, important);
			add_property(_t("border-bottom-right-radius-x"),	tokens[0].c_str(), baseurl, important);
			add_property(_t("border-bottom-left-radius-x"),	tokens[1].c_str(), baseurl, important);
		} else if(tokens.size() == 3)
		{
			add_property(_t("border-top-left-radius-x"),		tokens[0].c_str(), baseurl, important);
			add_property(_t("border-top-right-radius-x"),		tokens[1].c_str(), baseurl, important);
			add_property(_t("border-bottom-right-radius-x"),	tokens[2].c_str(), baseurl, important);
			add_property(_t("border-bottom-left-radius-x"),	tokens[1].c_str(), baseurl, important);
		} else if(tokens.size() == 4)
		{
			add_property(_t("border-top-left-radius-x"),		tokens[0].c_str(), baseurl, important);
			add_property(_t("border-top-right-radius-x"),		tokens[1].c_str(), baseurl, important);
			add_property(_t("border-bottom-right-radius-x"),	tokens[2].c_str(), baseurl, important);
			add_property(_t("border-bottom-left-radius-x"),	tokens[3].c_str(), baseurl, important);
		}
	} else if(!t_strcmp(name, _t("border-radius-y")))
	{
		string_vector tokens;
		split_string(val, tokens, _t(" "));
		if(tokens.size() == 1)
		{
			add_property(_t("border-top-left-radius-y"),		tokens[0].c_str(), baseurl, important);
			add_property(_t("border-top-right-radius-y"),		tokens[0].c_str(), baseurl, important);
			add_property(_t("border-bottom-right-radius-y"),	tokens[0].c_str(), baseurl, important);
			add_property(_t("border-bottom-left-radius-y"),	tokens[0].c_str(), baseurl, important);
		} else if(tokens.size() == 2)
		{
			add_property(_t("border-top-left-radius-y"),		tokens[0].c_str(), baseurl, important);
			add_property(_t("border-top-right-radius-y"),		tokens[1].c_str(), baseurl, important);
			add_property(_t("border-bottom-right-radius-y"),	tokens[0].c_str(), baseurl, important);
			add_property(_t("border-bottom-left-radius-y"),	tokens[1].c_str(), baseurl, important);
		} else if(tokens.size() == 3)
		{
			add_property(_t("border-top-left-radius-y"),		tokens[0].c_str(), baseurl, important);
			add_property(_t("border-top-right-radius-y"),		tokens[1].c_str(), baseurl, important);
			add_property(_t("border-bottom-right-radius-y"),	tokens[2].c_str(), baseurl, important);
			add_property(_t("border-bottom-left-radius-y"),	tokens[1].c_str(), baseurl, important);
		} else if(tokens.size() == 4)
		{
			add_property(_t("border-top-left-radius-y"),		tokens[0].c_str(), baseurl, important);
			add_property(_t("border-top-right-radius-y"),		tokens[1].c_str(), baseurl, important);
			add_property(_t("border-bottom-right-radius-y"),	tokens[2].c_str(), baseurl, important);
			add_property(_t("border-bottom-left-radius-y"),	tokens[3].c_str(), baseurl, important);
		}
	}
	else

	// Parse list-style shorthand properties 
	if(!t_strcmp(name, _t("list-style")))
	{
		add_parsed_property(_t("list-style-type"),			_t("disc"),		important);
		add_parsed_property(_t("list-style-position"),		_t("outside"),	important);
		add_parsed_property(_t("list-style-image"),			_t(""),			important);
		add_parsed_property(_t("list-style-image-baseurl"),	_t(""),			important);

		string_vector tokens;
		split_string(val, tokens, _t(" "), _t(""), _t("("));
		for(const auto& token : tokens)
		{
			int idx = value_index(token, list_style_type_strings, -1);
			if(idx >= 0)
			{
				add_parsed_property(_t("list-style-type"), token, important);
			} else
			{
				idx = value_index(token, list_style_position_strings, -1);
				if(idx >= 0)
				{
					add_parsed_property(_t("list-style-position"), token, important);
				} else if(!t_strncmp(val, _t("url"), 3))
				{
					add_parsed_property(_t("list-style-image"), token, important);
					if(baseurl)
					{
						add_parsed_property(_t("list-style-image-baseurl"), baseurl, important);
					}
				}
			}
		}
	} else 

	// Add baseurl for background image 
	if(	!t_strcmp(name, _t("list-style-image")))
	{
		add_parsed_property(name, val, important);
		if(baseurl)
		{
			add_parsed_property(_t("list-style-image-baseurl"), baseurl, important);
		}
	} else
		
	// Parse background shorthand properties 
	if(!t_strcmp(name, _t("background")))
	{
		parse_short_background(val, baseurl, important);

	} else 
		
	// Parse margin and padding shorthand properties 
	if(!t_strcmp(name, _t("margin")) || !t_strcmp(name, _t("padding")))
	{
		string_vector tokens;
		split_string(val, tokens, _t(" "));
		if(tokens.size() >= 4)
		{
			add_parsed_property(tstring(name) + _t("-top"),		tokens[0], important);
			add_parsed_property(tstring(name) + _t("-right"),		tokens[1], important);
			add_parsed_property(tstring(name) + _t("-bottom"),	tokens[2], important);
			add_parsed_property(tstring(name) + _t("-left"),		tokens[3], important);
		} else if(tokens.size() == 3)
		{
			add_parsed_property(tstring(name) + _t("-top"),		tokens[0], important);
			add_parsed_property(tstring(name) + _t("-right"),		tokens[1], important);
			add_parsed_property(tstring(name) + _t("-left"),		tokens[1], important);
			add_parsed_property(tstring(name) + _t("-bottom"),	tokens[2], important);
		} else if(tokens.size() == 2)
		{
			add_parsed_property(tstring(name) + _t("-top"),		tokens[0], important);
			add_parsed_property(tstring(name) + _t("-bottom"),	tokens[0], important);
			add_parsed_property(tstring(name) + _t("-right"),		tokens[1], important);
			add_parsed_property(tstring(name) + _t("-left"),		tokens[1], important);
		} else if(tokens.size() == 1)
		{
			add_parsed_property(tstring(name) + _t("-top"),		tokens[0], important);
			add_parsed_property(tstring(name) + _t("-bottom"),	tokens[0], important);
			add_parsed_property(tstring(name) + _t("-right"),		tokens[0], important);
			add_parsed_property(tstring(name) + _t("-left"),		tokens[0], important);
		}
	} else 
		
		
	// Parse border-* shorthand properties 
	if(	!t_strcmp(name, _t("border-left")) || 
		!t_strcmp(name, _t("border-right")) ||
		!t_strcmp(name, _t("border-top"))  || 
		!t_strcmp(name, _t("border-bottom")))
	{
		parse_short_border(name, val, important);
	} else 
		
	// Parse border-width/style/color shorthand properties 
	if(	!t_strcmp(name, _t("border-width")) ||
		!t_strcmp(name, _t("border-style"))  ||
		!t_strcmp(name, _t("border-color")) )
	{
		string_vector nametokens;
		split_string(name, nametokens, _t("-"));

		string_vector tokens;
		split_string(val, tokens, _t(" "));
		if(tokens.size() >= 4)
		{
			add_parsed_property(nametokens[0] + _t("-top-")		+ nametokens[1],	tokens[0], important);
			add_parsed_property(nametokens[0] + _t("-right-")	+ nametokens[1],	tokens[1], important);
			add_parsed_property(nametokens[0] + _t("-bottom-")	+ nametokens[1],	tokens[2], important);
			add_parsed_property(nametokens[0] + _t("-left-")	+ nametokens[1],	tokens[3], important);
		} else if(tokens.size() == 3)
		{
			add_parsed_property(nametokens[0] + _t("-top-")		+ nametokens[1],	tokens[0], important);
			add_parsed_property(nametokens[0] + _t("-right-")	+ nametokens[1],	tokens[1], important);
			add_parsed_property(nametokens[0] + _t("-left-")	+ nametokens[1],	tokens[1], important);
			add_parsed_property(nametokens[0] + _t("-bottom-")	+ nametokens[1],	tokens[2], important);
		} else if(tokens.size() == 2)
		{
			add_parsed_property(nametokens[0] + _t("-top-")		+ nametokens[1],	tokens[0], important);
			add_parsed_property(nametokens[0] + _t("-bottom-")	+ nametokens[1],	tokens[0], important);
			add_parsed_property(nametokens[0] + _t("-right-")	+ nametokens[1],	tokens[1], important);
			add_parsed_property(nametokens[0] + _t("-left-")	+ nametokens[1],	tokens[1], important);
		} else if(tokens.size() == 1)
		{
			add_parsed_property(nametokens[0] + _t("-top-")		+ nametokens[1],	tokens[0], important);
			add_parsed_property(nametokens[0] + _t("-bottom-")	+ nametokens[1],	tokens[0], important);
			add_parsed_property(nametokens[0] + _t("-right-")	+ nametokens[1],	tokens[0], important);
			add_parsed_property(nametokens[0] + _t("-left-")	+ nametokens[1],	tokens[0], important);
		}
	} else 
		
	// Parse font shorthand properties 
	if(!t_strcmp(name, _t("font")))
	{
		parse_short_font(val, important);
	} else 
	{
		add_parsed_property(name, val, important);
	}
}

void litehtml::style::parse_short_border( const tstring& prefix, const tstring& val, bool important )
{
	string_vector tokens;
	split_string(val, tokens, _t(" "), _t(""), _t("("));
	if(tokens.size() >= 3)
	{
		add_parsed_property(prefix + _t("-width"),	tokens[0], important);
		add_parsed_property(prefix + _t("-style"),	tokens[1], important);
		add_parsed_property(prefix + _t("-color"),	tokens[2], important);
	} else if(tokens.size() == 2)
	{
		if(iswdigit(tokens[0][0]) || value_index(val, border_width_strings) >= 0)
		{
			add_parsed_property(prefix + _t("-width"),	tokens[0], important);
			add_parsed_property(prefix + _t("-style"),	tokens[1], important);
		} else
		{
			add_parsed_property(prefix + _t("-style"),	tokens[0], important);
			add_parsed_property(prefix + _t("-color"),	tokens[1], important);
		}
	}
}

void litehtml::style::parse_short_background( const tstring& val, const tchar_t* baseurl, bool important )
{
	add_parsed_property(_t("background-color"),			_t("transparent"),	important);
	add_parsed_property(_t("background-image"),			_t(""),				important);
	add_parsed_property(_t("background-image-baseurl"), _t(""),				important);
	add_parsed_property(_t("background-repeat"),		_t("repeat"),		important);
	add_parsed_property(_t("background-origin"),		_t("padding-box"),	important);
	add_parsed_property(_t("background-clip"),			_t("border-box"),	important);
	add_parsed_property(_t("background-attachment"),	_t("scroll"),		important);

	if(val == _t("none"))
	{
		return;
	}

	string_vector tokens;
	split_string(val, tokens, _t(" "), _t(""), _t("("));
	bool origin_found = false;
	for(const auto& token : tokens)
	{
		if(token.substr(0, 3) == _t("url"))
		{
			add_parsed_property(_t("background-image"), token, important);
			if(baseurl)
			{
				add_parsed_property(_t("background-image-baseurl"), baseurl, important);
			}

		} else if( value_in_list(token, background_repeat_strings) )
		{
			add_parsed_property(_t("background-repeat"), token, important);
		} else if( value_in_list(token, background_attachment_strings) )
		{
			add_parsed_property(_t("background-attachment"), token, important);
		} else if( value_in_list(token, background_box_strings) )
		{
			if(!origin_found)
			{
				add_parsed_property(_t("background-origin"), token, important);
				origin_found = true;
			} else
			{
				add_parsed_property(_t("background-clip"), token, important);
			}
		} else if(	value_in_list(token, _t("left;right;top;bottom;center")) ||
					iswdigit(token[0]) ||
					token[0] == _t('-')	||
					token[0] == _t('.')	||
					token[0] == _t('+'))
		{
			if(m_properties.find(_t("background-position")) != m_properties.end())
			{
				m_properties[_t("background-position")].m_value = m_properties[_t("background-position")].m_value + _t(" ") + token;
			} else
			{
				add_parsed_property(_t("background-position"), token, important);
			}
		} else if (web_color::is_color(token.c_str()))
		{
			add_parsed_property(_t("background-color"), token, important);
		}
	}
}

void litehtml::style::parse_short_font( const tstring& val, bool important )
{
	add_parsed_property(_t("font-style"),	_t("normal"),	important);
	add_parsed_property(_t("font-variant"),	_t("normal"),	important);
	add_parsed_property(_t("font-weight"),	_t("normal"),	important);
	add_parsed_property(_t("font-size"),		_t("medium"),	important);
	add_parsed_property(_t("line-height"),	_t("normal"),	important);

	string_vector tokens;
	split_string(val, tokens, _t(" "), _t(""), _t("\""));

	int idx;
	bool is_family = false;
	tstring font_family;
	for(const auto& token : tokens)
	{
		idx = value_index(token, font_style_strings);
		if(!is_family)
		{
			if(idx >= 0)
			{
				if(idx == 0)
				{
					add_parsed_property(_t("font-weight"), token, important);
					add_parsed_property(_t("font-variant"), token, important);
					add_parsed_property(_t("font-style"), token, important);
				} else
				{
					add_parsed_property(_t("font-style"), token, important);
				}
			} else
			{
				if(value_in_list(token, font_weight_strings))
				{
					add_parsed_property(_t("font-weight"), token, important);
				} else
				{
					if(value_in_list(token, font_variant_strings))
					{
						add_parsed_property(_t("font-variant"), token, important);
					} else if( iswdigit(token[0]) )
					{
						string_vector szlh;
						split_string(token, szlh, _t("/"));

						if(szlh.size() == 1)
						{
							add_parsed_property(_t("font-size"),	szlh[0], important);
						} else	if(szlh.size() >= 2)
						{
							add_parsed_property(_t("font-size"),	szlh[0], important);
							add_parsed_property(_t("line-height"),	szlh[1], important);
						}
					} else
					{
						is_family = true;
						font_family += token;
					}
				}
			}
		} else
		{
			font_family += token;
		}
	}
	add_parsed_property(_t("font-family"), font_family, important);
}

void litehtml::style::add_parsed_property( const tstring& name, const tstring& val, bool important )
{
	bool is_valid = true;
	auto vals = m_valid_values.find(name);
	if (vals != m_valid_values.end())
	{
		if (!value_in_list(val, vals->second))
		{
			is_valid = false;
		}
	}

	if (is_valid)
	{
		auto prop = m_properties.find(name);
		if (prop != m_properties.end())
		{
			if (!prop->second.m_important || (important && prop->second.m_important))
			{
				prop->second.m_value = val;
				prop->second.m_important = important;
			}
		}
		else
		{
			m_properties[name] = property_value(val.c_str(), important);
		}
	}
}

void litehtml::style::remove_property( const tstring& name, bool important )
{
	auto prop = m_properties.find(name);
	if(prop != m_properties.end())
	{
		if( !prop->second.m_important || (important && prop->second.m_important) )
		{
			m_properties.erase(prop);
		}
	}
}
