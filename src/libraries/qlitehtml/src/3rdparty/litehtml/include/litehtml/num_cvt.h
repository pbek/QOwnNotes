#ifndef NUM_CVT_H
#define NUM_CVT_H

#include <string>
#include "os_types.h"

namespace litehtml
{
	namespace num_cvt
	{
		litehtml::tstring to_latin_lower(int val);
		litehtml::tstring to_latin_upper(int val);
		litehtml::tstring to_greek_lower(int val);
		litehtml::tstring to_roman_lower(int value);
		litehtml::tstring to_roman_upper(int value);
	}
}

#endif	// NUM_CVT_H