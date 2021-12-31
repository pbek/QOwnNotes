#ifndef LH_EL_BASE_H
#define LH_EL_BASE_H

#include "html_tag.h"

namespace litehtml
{
	class el_base : public html_tag
	{
	public:
		explicit el_base(const std::shared_ptr<litehtml::document>& doc);

		void parse_attributes() override;
	};
}

#endif  // LH_EL_BASE_H
