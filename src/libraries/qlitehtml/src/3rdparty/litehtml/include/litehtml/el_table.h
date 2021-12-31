#ifndef LH_EL_TABLE_H
#define LH_EL_TABLE_H

#include "html_tag.h"

namespace litehtml
{
	struct col_info
	{
		int		width;
		bool	is_auto;
	};


	class el_table : public html_tag
	{
	public:
		explicit el_table(const std::shared_ptr<litehtml::document>& doc);

		bool appendChild(const litehtml::element::ptr& el) override;
		void parse_styles(bool is_reparse = false) override;
		void parse_attributes() override;
	};
}

#endif  // LH_EL_TABLE_H
