#ifndef LH_UTF8_STRINGS_H
#define LH_UTF8_STRINGS_H

#include "types.h"

namespace litehtml
{
	// converts UTF-32 ch to UTF-8 and appends it to str
	void	 append_char(string& str, char32_t ch);
	char32_t read_utf8_char(const string& str, int& index);
	void	 prev_utf8_char(const string& str, int& index);

	class utf8_to_utf32
	{
		std::u32string m_str;

	  public:
		utf8_to_utf32(const string& val);
		operator const char32_t*() const { return m_str.c_str(); }
	};

	class utf32_to_utf8
	{
		string m_str;

	  public:
		utf32_to_utf8(const std::u32string& val);
					operator const char*() const { return m_str.c_str(); }
		const char* c_str() const { return m_str.c_str(); }
	};

#define litehtml_from_utf32(str) litehtml::utf32_to_utf8(str)
#define litehtml_to_utf32(str)	 litehtml::utf8_to_utf32(str)
} // namespace litehtml

#endif // LH_UTF8_STRINGS_H
