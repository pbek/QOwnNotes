#ifndef LH_HTML_H
#define LH_HTML_H

#include <cstdlib>
#include <cmath>
#include <cctype>
#include <cstring>
#include <algorithm>
#include "types.h"
#include "media_query.h"

namespace litehtml
{
	const string	  whitespace = " \n\r\t\f";
	string&			  trim(string& s, const string& chars_to_trim = whitespace);
	string			  trim(const string& s, const string& chars_to_trim = whitespace);
	string&			  lcase(string& s);
	int				  value_index(const string& val, const string& strings, int defValue = -1, char delim = ';');
	string			  index_value(int index, const string& strings, char delim = ';');
	bool			  value_in_list(const string& val, const string& strings, char delim = ';');
	string::size_type find_close_bracket(const string& s, string::size_type off, char open_b = '(', char close_b = ')');
	void			  split_string(const string& str, string_vector& tokens, const string& delims = whitespace,
								   const string& delims_preserve = "", const string& quote = "\"");
	string_vector split_string(const string& str, const string& delims = whitespace, const string& delims_preserve = "",
							   const string& quote = "\"");
	void		  join_string(string& str, const string_vector& tokens, const string& delims);
	double		  t_strtod(const char* string, char** endPtr = nullptr);
	string		  get_escaped_string(const string& in_str);

	template <typename T, typename... Opts> bool is_one_of(T val, Opts... opts)
	{
		return (... || (val == opts));
	}
	template <class T> const T& at(const vector<T>& vec, int index /*may be negative*/)
	{
		static T invalid_item; // T's default constructor must create invalid item
		if(index < 0)
			index += (int) vec.size();
		return index >= 0 && index < (int) vec.size() ? vec[index] : invalid_item;
	}
	template <class Map, class Key> auto at(const Map& map, Key key)
	{
		static typename Map::mapped_type invalid_value; // mapped_type's default constructor must create invalid item
		auto							 it = map.find(key);
		return it != map.end() ? it->second : invalid_value;
	}
	template <typename T> vector<T> slice(const vector<T>& vec, int index, int count = -1)
	{
		if(count == -1)
			count = (int) vec.size() - index;
		return {vec.begin() + index, vec.begin() + index + count};
	}
	template <class C> // C == vector or string
	void remove(C& vec, int index /*may be negative*/, int count = 1)
	{
		if(index < 0)
			index += (int) vec.size();

		if(!(index >= 0 && index < (int) vec.size()))
			return;

		count = min(count, (int) vec.size() - index);
		if(count <= 0)
			return;

		vec.erase(vec.begin() + index, vec.begin() + index + count);
	}
	template <class T> void insert(vector<T>& vec, int index, const vector<T>& x)
	{
		vec.insert(vec.begin() + index, x.begin(), x.end());
	}
	template <class T> vector<T>& operator+=(vector<T>& vec, const vector<T>& x)
	{
		vec.insert(vec.end(), x.begin(), x.end());
		return vec;
	}
	template <class C, class T> bool contains(const C& coll, const T& item)
	{
		return std::find(coll.begin(), coll.end(), item) != coll.end();
	}
	inline bool contains(const string& str, const string& substr)
	{
		return str.find(substr) != string::npos;
	}
	template <class C> void sort(C& coll)
	{
		std::sort(coll.begin(), coll.end());
	}

	int			t_strcasecmp(const char* s1, const char* s2);
	int			t_strncasecmp(const char* s1, const char* s2, size_t n);
	inline bool equal_i(const string& s1, const string& s2)
	{
		if(s1.size() != s2.size())
			return false;
		return t_strncasecmp(s1.c_str(), s2.c_str(), s1.size()) == 0;
	}
	inline bool match(const string& str, int index /*may be negative*/, const string& substr)
	{
		if(index < 0)
			index += (int) str.size();
		if(index < 0)
			return false;
		return str.substr(index, substr.size()) == substr;
	}
	inline bool match_i(const string& str, int index /*may be negative*/, const string& substr)
	{
		if(index < 0)
			index += (int) str.size();
		if(index < 0)
			return false;
		return equal_i(str.substr(index, substr.size()), substr);
	}

	bool is_number(const string& string, bool allow_dot = true);

	// https://infra.spec.whatwg.org/#ascii-whitespace
	inline bool is_whitespace(int c)
	{
		return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f';
	}

	inline int t_isalpha(int c)
	{
		return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
	}
	const auto is_letter = t_isalpha;

	inline int t_tolower(int c)
	{
		return (c >= 'A' && c <= 'Z' ? c + 'a' - 'A' : c);
	}
	// https://infra.spec.whatwg.org/#ascii-lowercase
	inline int lowcase(int c)
	{
		return t_tolower(c);
	}
	inline string lowcase(string str)
	{
		for(char& c : str)
			c = (char) t_tolower(c);
		return str;
	}

	inline int t_isdigit(int c)
	{
		return (c >= '0' && c <= '9');
	}
	const auto is_digit = t_isdigit;

	inline bool is_hex_digit(int ch)
	{
		return is_digit(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
	}

	inline int digit_value(int ch)
	{
		return is_digit(ch) ? ch - '0' : lowcase(ch) - 'a' + 10;
	}

	inline bool is_surrogate(int ch)
	{
		return ch >= 0xD800 && ch < 0xE000;
	}

	inline int round_f(float val)
	{
		int int_val = (int) val;
		if(val - int_val >= 0.5)
		{
			int_val++;
		}
		return int_val;
	}

	inline int round_d(double val)
	{
		int int_val = (int) val;
		if(val - int_val >= 0.5)
		{
			int_val++;
		}
		return int_val;
	}

	inline float t_strtof(const string& str, char** endPtr = nullptr)
	{
		return (float) t_strtod(str.c_str(), endPtr);
	}

	inline pixel_t baseline_align(pixel_t line_height, pixel_t line_base_line, pixel_t height, pixel_t baseline)
	{
		return (line_height - line_base_line) - (height - baseline);
	}
} // namespace litehtml

#endif // LH_HTML_H
