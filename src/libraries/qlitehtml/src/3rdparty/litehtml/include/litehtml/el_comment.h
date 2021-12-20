#ifndef LH_EL_COMMENT_H
#define LH_EL_COMMENT_H

#include "html_tag.h"

namespace litehtml
{
	class el_comment : public element
	{
		tstring	m_text;
	public:
		explicit el_comment(const std::shared_ptr<litehtml::document>& doc);

		void get_text(tstring& text) override;
		void set_data(const tchar_t* data) override;
	};
}

#endif  // LH_EL_COMMENT_H
