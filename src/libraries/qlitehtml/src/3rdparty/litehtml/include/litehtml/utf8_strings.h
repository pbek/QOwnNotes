#ifndef LH_UTF8_STRINGS_H
#define LH_UTF8_STRINGS_H

#include "os_types.h"
#include "types.h"

namespace litehtml
{
	class utf8_to_wchar
	{
		const byte* m_utf8;
		std::wstring m_str;
	public:
		utf8_to_wchar(const char* val);
		operator const wchar_t*() const
		{
			return m_str.c_str();
		}
	private:
		ucode_t getb()
		{
			if (!(*m_utf8)) return 0;
			return *m_utf8++;
		}
		ucode_t get_next_utf8(ucode_t val)
		{
			return (val & 0x3f);
		}
		ucode_t get_char();
	};

	class wchar_to_utf8
	{
		std::string m_str;
	public:
		wchar_to_utf8(const std::wstring& val);
		operator const char*() const
		{
			return m_str.c_str();
		}

		const char* c_str() const
		{
			return m_str.c_str();
		}
	};

#ifdef LITEHTML_UTF8
#define litehtml_from_utf8(str)		str
#define litehtml_to_utf8(str)		str
#define litehtml_from_wchar(str)	litehtml::wchar_to_utf8(str)
#else
#define litehtml_from_utf8(str)		litehtml::utf8_to_wchar(str)
#define litehtml_from_wchar(str)	str
#define litehtml_to_utf8(str)		litehtml::wchar_to_utf8(str)
#endif
}

#endif  // LH_UTF8_STRINGS_H
