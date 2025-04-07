#ifndef LH_OS_TYPES_H
#define LH_OS_TYPES_H

#include <cstdint>
#include <string>

namespace litehtml {
#if defined(WIN32) || defined(_WIN32) || defined(WINCE)

// noexcept appeared since Visual Studio 2013
#if _MSC_VER < 1900
#define noexcept
#endif

#ifndef LITEHTML_UTF8

typedef std::wstring tstring;
typedef wchar_t tchar_t;
typedef std::wstringstream tstringstream;

#define _t(quote) L##quote

#define t_strlen wcslen
#define t_strcmp wcscmp
#define t_strncmp wcsncmp
#define t_strtol wcstol
#define t_atoi _wtoi
#define t_itoa(value, buffer, size, radix) _itow_s(value, buffer, size, radix)
#define t_strstr wcsstr
#define t_isspace iswspace
#define t_to_string(val) std::to_wstring(val)

#else

typedef std::string tstring;
typedef char tchar_t;
typedef std::stringstream tstringstream;

#define _t(quote) quote

#define t_strlen strlen
#define t_strcmp strcmp
#define t_strncmp strncmp
#define t_strtol strtol
#define t_atoi atoi
#define t_itoa(value, buffer, size, radix) _itoa_s(value, buffer, size, radix)
#define t_strstr strstr
#define t_isspace isspace
#define t_to_string(val) std::to_string(val)

#endif

#ifdef _WIN64
typedef unsigned __int64 uint_ptr;
#else
typedef unsigned int uint_ptr;
#endif

#else
#define LITEHTML_UTF8

typedef std::string tstring;
typedef char tchar_t;
typedef std::uintptr_t uint_ptr;
typedef std::stringstream tstringstream;

#define _t(quote) quote

#define t_strlen strlen
#define t_strcmp strcmp
#define t_strncmp strncmp

#define t_itoa(value, buffer, size, radix) snprintf(buffer, size, "%d", value)

#define t_strtol strtol
#define t_atoi atoi
#define t_strstr strstr
#define t_isspace isspace
#define t_to_string(val) std::to_string(val)

#endif
}  // namespace litehtml

#endif  // LH_OS_TYPES_H
