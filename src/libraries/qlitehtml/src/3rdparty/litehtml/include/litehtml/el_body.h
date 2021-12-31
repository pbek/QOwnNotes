#ifndef LH_EL_BODY_H
#define LH_EL_BODY_H

#include "html_tag.h"

namespace litehtml
{
	class el_body : public html_tag
	{
	public:
		explicit el_body(const std::shared_ptr<litehtml::document>& doc);

		bool is_body() const override;
	};
}

#endif  // LH_EL_BODY_H
