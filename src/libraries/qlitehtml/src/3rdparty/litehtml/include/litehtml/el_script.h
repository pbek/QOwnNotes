#ifndef LH_EL_SCRIPT_H
#define LH_EL_SCRIPT_H

#include "element.h"

namespace litehtml
{
	class el_script : public element
	{
		string m_text;

	  public:
		explicit el_script(const std::shared_ptr<document>& doc);

		void		parse_attributes() override;
		bool		appendChild(const ptr& el) override;
		string_id	tag() const override;
		const char* get_tagName() const override;
	};
} // namespace litehtml

#endif // LH_EL_SCRIPT_H
