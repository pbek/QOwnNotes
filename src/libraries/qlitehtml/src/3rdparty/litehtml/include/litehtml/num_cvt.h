#ifndef NUM_CVT_H
#define NUM_CVT_H

#include "types.h"

namespace litehtml
{
	namespace num_cvt
	{
		string to_latin_lower(int val);
		string to_latin_upper(int val);
		string to_greek_lower(int val);
		string to_roman_lower(int value);
		string to_roman_upper(int value);
	} // namespace num_cvt
} // namespace litehtml

#endif // NUM_CVT_H
