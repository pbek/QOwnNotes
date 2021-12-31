#include "html.h"
#include "utf8_strings.h"


litehtml::utf8_to_wchar::utf8_to_wchar(const char* val)
{
	m_utf8 = (const byte*) val;
	while (true)
	{
		ucode_t wch = get_char();
		if (!wch) break;
		m_str += wch;
	}
}

litehtml::ucode_t litehtml::utf8_to_wchar::get_char()
{
	ucode_t b1 = getb();

	if (!b1)
	{
		return 0;
	}

	// Determine whether we are dealing
	// with a one-, two-, three-, or four-
	// byte sequence.
	if ((b1 & 0x80) == 0)
	{
		// 1-byte sequence: 000000000xxxxxxx = 0xxxxxxx
		return b1;
	}
	else if ((b1 & 0xe0) == 0xc0)
	{
		// 2-byte sequence: 00000yyyyyxxxxxx = 110yyyyy 10xxxxxx
		ucode_t r = (b1 & 0x1f) << 6;
		r |= get_next_utf8(getb());
		return r;
	}
	else if ((b1 & 0xf0) == 0xe0)
	{
		// 3-byte sequence: zzzzyyyyyyxxxxxx = 1110zzzz 10yyyyyy 10xxxxxx
		ucode_t r = (b1 & 0x0f) << 12;
		r |= get_next_utf8(getb()) << 6;
		r |= get_next_utf8(getb());
		return r;
	}
	else if ((b1 & 0xf8) == 0xf0)
	{
		// 4-byte sequence: 11101110wwwwzzzzyy + 110111yyyyxxxxxx
		//     = 11110uuu 10uuzzzz 10yyyyyy 10xxxxxx
		// (uuuuu = wwww + 1)
		int b2 = get_next_utf8(getb());
		int b3 = get_next_utf8(getb());
		int b4 = get_next_utf8(getb());
		return ((b1 & 7) << 18) | ((b2 & 0x3f) << 12) |
			((b3 & 0x3f) << 6) | (b4 & 0x3f);
	}

	//bad start for UTF-8 multi-byte sequence
	return '?';
}

litehtml::wchar_to_utf8::wchar_to_utf8(const std::wstring& val)
{
	unsigned int code;
	for (int i = 0; val[i]; i++)
	{
		code = val[i];
		if (code <= 0x7F)
		{
			m_str += (char)code;
		}
		else if (code <= 0x7FF)
		{
			m_str += (code >> 6) + 192;
			m_str += (code & 63) + 128;
		}
		else if (0xd800 <= code && code <= 0xdfff)
		{
			//invalid block of utf8
		}
		else if (code <= 0xFFFF)
		{
			m_str += (code >> 12) + 224;
			m_str += ((code >> 6) & 63) + 128;
			m_str += (code & 63) + 128;
		}
		else if (code <= 0x10FFFF)
		{
			m_str += (code >> 18) + 240;
			m_str += ((code >> 12) & 63) + 128;
			m_str += ((code >> 6) & 63) + 128;
			m_str += (code & 63) + 128;
		}
	}
}
