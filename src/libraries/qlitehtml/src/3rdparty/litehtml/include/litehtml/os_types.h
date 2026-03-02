#ifndef LH_OS_TYPES_H
#define LH_OS_TYPES_H

#if defined(WIN32) || defined(_WIN32) || defined(WINCE)

#define t_itoa(value, buffer, size, radix) _itoa_s(value, buffer, size, radix)
#define t_snprintf(s, n, format, ...)	   _snprintf_s(s, _TRUNCATE, n, format, __VA_ARGS__)

#else

#define t_itoa(value, buffer, size, radix) snprintf(buffer, size, "%d", value)
#define t_snprintf(s, n, format, ...)	   snprintf(s, n, format, __VA_ARGS__)

#endif

#endif // LH_OS_TYPES_H
