#ifndef LH_EL_SPACE_H
#define LH_EL_SPACE_H

#include "html_tag.h"
#include "el_text.h"

namespace litehtml
{
	class el_space : public el_text
	{
	public:
		el_space(const tchar_t* text, const std::shared_ptr<litehtml::document>& doc);

		bool is_white_space() const override;
		bool is_break() const override;
	};
}

#endif  // LH_EL_SPACE_H
