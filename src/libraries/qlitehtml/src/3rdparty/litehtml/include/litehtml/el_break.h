#ifndef LH_EL_BREAK_H
#define LH_EL_BREAK_H

#include "html_tag.h"

namespace litehtml
{
	class el_break : public html_tag
	{
	public:
		explicit el_break(const std::shared_ptr<litehtml::document>& doc);

		bool is_break() const override;
	};
}

#endif  // LH_EL_BREAK_H
