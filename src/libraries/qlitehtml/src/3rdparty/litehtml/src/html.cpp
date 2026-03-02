#include "html.h"
#include "types.h"

namespace litehtml
{

	string& trim(string& s, const string& chars_to_trim)
	{
		string::size_type pos = s.find_first_not_of(chars_to_trim);
		if(pos != string::npos)
		{
			s.erase(s.begin(), s.begin() + pos);
		} else
		{
			s = "";
			return s;
		}
		pos = s.find_last_not_of(chars_to_trim);
		if(pos != string::npos)
		{
			s.erase(s.begin() + pos + 1, s.end());
		}
		return s;
	}

	string trim(const string& s, const string& chars_to_trim)
	{
		string str = s;
		trim(str, chars_to_trim);
		return str;
	}

	string& lcase(string& s)
	{
		for(char& i : s)
		{
			i = (char) t_tolower(i);
		}
		return s;
	}

	string::size_type find_close_bracket(const string& s, string::size_type off, char open_b, char close_b)
	{
		int cnt = 0;
		for(string::size_type i = off; i < s.length(); i++)
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
		return string::npos;
	}

	string index_value(int index, const string& strings, char delim)
	{
		std::vector<string> vals;
		string				delims;
		delims.push_back(delim);
		split_string(strings, vals, delims);
		if(index >= 0 && index < (int) vals.size())
		{
			return vals[index];
		}
		return std::to_string(index);
	}

	int value_index(const string& val, const string& strings, int defValue, char delim)
	{
		if(val.empty() || strings.empty() || !delim)
		{
			return defValue;
		}

		int				  idx		  = 0;
		string::size_type delim_start = 0;
		string::size_type delim_end	  = strings.find(delim, delim_start);
		string::size_type item_len;
		while(true)
		{
			if(delim_end == string::npos)
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
			if(delim_start == string::npos)
				break;
			delim_start++;
			if(delim_start == strings.length())
				break;
			delim_end = strings.find(delim, delim_start);
		}
		return defValue;
	}

	bool value_in_list(const string& val, const string& strings, char delim)
	{
		int idx = value_index(val, strings, -1, delim);
		if(idx >= 0)
		{
			return true;
		}
		return false;
	}

	string_vector split_string(const string& str, const string& delims, const string& delims_preserve,
							   const string& quote)
	{
		string_vector result;
		split_string(str, result, delims, delims_preserve, quote);
		return result;
	}

	void split_string(const string& str, string_vector& tokens, const string& delims, const string& delims_preserve,
					  const string& quote)
	{
		if(str.empty() || (delims.empty() && delims_preserve.empty()))
		{
			return;
		}

		string all_delims			  = delims + delims_preserve + quote;

		string::size_type token_start = 0;
		string::size_type token_end	  = str.find_first_of(all_delims, token_start);
		string::size_type token_len;
		string			  token;
		while(true)
		{
			while(token_end != string::npos && quote.find_first_of(str[token_end]) != string::npos)
			{
				if(str[token_end] == '(')
				{
					token_end = find_close_bracket(str, token_end, '(', ')');
				} else if(str[token_end] == '[')
				{
					token_end = find_close_bracket(str, token_end, '[', ']');
				} else if(str[token_end] == '{')
				{
					token_end = find_close_bracket(str, token_end, '{', '}');
				} else
				{
					token_end = str.find_first_of(str[token_end], token_end + 1);
				}
				if(token_end != string::npos)
				{
					token_end = str.find_first_of(all_delims, token_end + 1);
				}
			}

			if(token_end == string::npos)
			{
				token_len = string::npos;
			} else
			{
				token_len = token_end - token_start;
			}

			token = str.substr(token_start, token_len);
			if(!token.empty())
			{
				tokens.push_back(token);
			}
			if(token_end != string::npos && !delims_preserve.empty() &&
			   delims_preserve.find_first_of(str[token_end]) != string::npos)
			{
				tokens.push_back(str.substr(token_end, 1));
			}

			token_start = token_end;
			if(token_start == string::npos)
				break;
			token_start++;
			if(token_start == str.length())
				break;
			token_end = str.find_first_of(all_delims, token_start);
		}
	}

	void join_string(string& str, const string_vector& tokens, const string& delims)
	{
		str = "";
		for(size_t i = 0; i < tokens.size(); i++)
		{
			if(i != 0)
			{
				str += delims;
			}
			str += tokens[i];
		}
	}

	int t_strcasecmp(const char* s1, const char* s2)
	{
		int i, d, c;

		for(i = 0;; i++)
		{
			c = t_tolower((unsigned char) s1[i]);
			d = c - t_tolower((unsigned char) s2[i]);
			if(d < 0)
				return -1;
			else if(d > 0)
				return 1;
			else if(c == 0)
				return 0;
		}
	}

	int t_strncasecmp(const char* s1, const char* s2, size_t n)
	{
		int i, d, c;

		for(i = 0; i < (int) n; i++)
		{
			c = t_tolower((unsigned char) s1[i]);
			d = c - t_tolower((unsigned char) s2[i]);
			if(d < 0)
				return -1;
			else if(d > 0)
				return 1;
			else if(c == 0)
				return 0;
		}

		return 0;
	}

	string get_escaped_string(const string& in_str)
	{
		string ret;
		for(auto ch : in_str)
		{
			switch(ch)
			{
			case '\'':
				ret += "\\'";
				break;

			case '\"':
				ret += "\\\"";
				break;

			case '\?':
				ret += "\\?";
				break;

			case '\\':
				ret += "\\\\";
				break;

			case '\a':
				ret += "\\a";
				break;

			case '\b':
				ret += "\\b";
				break;

			case '\f':
				ret += "\\f";
				break;

			case '\n':
				ret += "\\n";
				break;

			case '\r':
				ret += "\\r";
				break;

			case '\t':
				ret += "\\t";
				break;

			case '\v':
				ret += "\\v";
				break;

			default:
				ret += ch;
			}
		}
		return ret;
	}

	bool is_number(const string& string, const bool allow_dot)
	{
		for(auto ch : string)
		{
			if(!(t_isdigit(ch) || (allow_dot && ch == '.')))
			{
				return false;
			}
		}
		return true;
	}

} // namespace litehtml
