#ifndef LH_HTML_MICROSYNTAXES_H
#define LH_HTML_MICROSYNTAXES_H

#include "types.h"

namespace litehtml
{

	bool html_parse_integer(const string& str, int& val);
	bool html_parse_non_negative_integer(const string& str, int& val);

	enum html_dimension_type
	{
		html_length,
		html_percentage
	};

	bool html_parse_dimension_value(const string& str, float& val, html_dimension_type& type);
	bool html_parse_nonzero_dimension_value(const string& str, float& val, html_dimension_type& type);

} // namespace litehtml

#endif // LH_HTML_MICROSYNTAXES_H
