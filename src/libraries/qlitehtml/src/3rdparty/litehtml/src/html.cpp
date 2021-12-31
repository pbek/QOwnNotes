#include "html.h"
#include "types.h"
#include "utf8_strings.h"

void litehtml::trim(tstring &s) 
{
	tstring::size_type pos = s.find_first_not_of(_t(" \n\r\t"));
	if(pos != tstring::npos)
	{
		s.erase(s.begin(), s.begin() + pos);
	}
	pos = s.find_last_not_of(_t(" \n\r\t"));
	if(pos != tstring::npos)
	{
		s.erase(s.begin() + pos + 1, s.end());
	}
}

void litehtml::lcase(tstring &s) 
{
	for(tchar_t & i : s)
	{
		i = t_tolower(i);
	}
}

litehtml::tstring::size_type litehtml::find_close_bracket(const tstring &s, tstring::size_type off, tchar_t open_b, tchar_t close_b)
{
	int cnt = 0;
	for(tstring::size_type i = off; i < s.length(); i++)
	{
		if(s[i] == open_b)
		{
			cnt++;
		} else if(s[i] == close_b)
		{
			cnt--;
			if(!cnt)
			{
				return i;
			}
		}
	}
	return tstring::npos;
}

int litehtml::value_index( const tstring& val, const tstring& strings, int defValue, tchar_t delim )
{
	if(val.empty() || strings.empty() || !delim)
	{
		return defValue;
	}

	int idx = 0;
	tstring::size_type delim_start	= 0;
	tstring::size_type delim_end	= strings.find(delim, delim_start);
	tstring::size_type item_len;
	while(true)
	{
		if(delim_end == tstring::npos)
		{
			item_len = strings.length() - delim_start;
		} else
		{
			item_len = delim_end - delim_start;
		}
		if(item_len == val.length())
		{
			if(val == strings.substr(delim_start, item_len))
			{
				return idx;
			}
		}
		idx++;
		delim_start = delim_end;
		if(delim_start == tstring::npos) break;
		delim_start++;
		if(delim_start == strings.length()) break;
		delim_end = strings.find(delim, delim_start);
	}
	return defValue;
}

bool litehtml::value_in_list( const tstring& val, const tstring& strings, tchar_t delim )
{
	int idx = value_index(val, strings, -1, delim);
	if(idx >= 0)
	{
		return true;
	}
	return false;
}

void litehtml::split_string(const tstring& str, string_vector& tokens, const tstring& delims, const tstring& delims_preserve, const tstring& quote)
{
	if(str.empty() || (delims.empty() && delims_preserve.empty()))
	{
		return;
	}

	tstring all_delims = delims + delims_preserve + quote;

	tstring::size_type token_start	= 0;
	tstring::size_type token_end	= str.find_first_of(all_delims, token_start);
	tstring::size_type token_len;
	tstring token;
	while(true)
	{
		while( token_end != tstring::npos && quote.find_first_of(str[token_end]) != tstring::npos )
		{
			if(str[token_end] == _t('('))
			{
				token_end = find_close_bracket(str, token_end, _t('('), _t(')'));
			} else if(str[token_end] == _t('['))
			{
				token_end = find_close_bracket(str, token_end, _t('['), _t(']'));
			} else if(str[token_end] == _t('{'))
			{
				token_end = find_close_bracket(str, token_end, _t('{'), _t('}'));
			} else
			{
				token_end = str.find_first_of(str[token_end], token_end + 1);
			}
			if(token_end != tstring::npos)
			{
				token_end = str.find_first_of(all_delims, token_end + 1);
			}
		}

		if(token_end == tstring::npos)
		{
			token_len = tstring::npos;
		} else
		{
			token_len = token_end - token_start;
		}

		token = str.substr(token_start, token_len);
		if(!token.empty())
		{
			tokens.push_back( token );
		}
		if(token_end != tstring::npos && !delims_preserve.empty() && delims_preserve.find_first_of(str[token_end]) != tstring::npos)
		{
			tokens.push_back( str.substr(token_end, 1) );
		}

		token_start = token_end;
		if(token_start == tstring::npos) break;
		token_start++;
		if(token_start == str.length()) break;
		token_end = str.find_first_of(all_delims, token_start);
	}
}

void litehtml::join_string(tstring& str, const string_vector& tokens, const tstring& delims)
{
	tstringstream ss;
	for(size_t i=0; i<tokens.size(); ++i)
	{
		if(i != 0)
		{
			ss << delims;
		}
		ss << tokens[i];
	}

	str = ss.str();
}

int litehtml::t_strcasecmp(const litehtml::tchar_t *s1, const litehtml::tchar_t *s2)
{
	int i, d, c;

	for (i = 0;; i++)
	{
		c = t_tolower((unsigned char)s1[i]);
		d = c - t_tolower((unsigned char)s2[i]);
		if (d < 0)
			return -1;
		else if (d > 0)
			return 1;
		else if (c == 0)
			return 0;
	}
}

int litehtml::t_strncasecmp(const litehtml::tchar_t *s1, const litehtml::tchar_t *s2, size_t n)
{
	int i, d, c;

	for (i = 0; i < n; i++)
	{
		c = t_tolower((unsigned char)s1[i]);
		d = c - t_tolower((unsigned char)s2[i]);
		if (d < 0)
			return -1;
		else if (d > 0)
			return 1;
	}

	return 0;
}

void litehtml::document_container::split_text(const char* text, const std::function<void(const tchar_t*)>& on_word, const std::function<void(const tchar_t*)>& on_space)
{
	std::wstring str;
	std::wstring str_in = (const wchar_t*)(utf8_to_wchar(text));
	ucode_t c;
	for (size_t i = 0; i < str_in.length(); i++)
	{
		c = (ucode_t)str_in[i];
		if (c <= ' ' && (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f'))
		{
			if (!str.empty())
			{
				on_word(litehtml_from_wchar(str.c_str()));
				str.clear();
			}
			str += c;
			on_space(litehtml_from_wchar(str.c_str()));
			str.clear();
		}
		// CJK character range
		else if (c >= 0x4E00 && c <= 0x9FCC)
		{
			if (!str.empty())
			{
				on_word(litehtml_from_wchar(str.c_str()));
				str.clear();
			}
			str += c;
			on_word(litehtml_from_wchar(str.c_str()));
			str.clear();
		}
		else
		{
			str += c;
		}
	}
	if (!str.empty())
	{
		on_word(litehtml_from_wchar(str.c_str()));
	}
}
