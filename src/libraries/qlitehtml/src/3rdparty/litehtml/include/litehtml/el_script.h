#ifndef LH_EL_SCRIPT_H
#define LH_EL_SCRIPT_H

#include "html_tag.h"

namespace litehtml
{
	class el_script : public element
	{
		tstring m_text;
	public:
		explicit el_script(const std::shared_ptr<litehtml::document>& doc);

		void parse_attributes() override;
		bool appendChild(const ptr &el) override;
		const tchar_t*	get_tagName() const override;
	};
}

#endif  // LH_EL_SCRIPT_H
