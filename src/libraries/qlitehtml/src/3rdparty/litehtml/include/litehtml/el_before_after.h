#ifndef LH_EL_BEFORE_AFTER_H
#define LH_EL_BEFORE_AFTER_H

#include "html_tag.h"

namespace litehtml
{
	class el_before_after_base : public html_tag
	{
	public:
		el_before_after_base(const std::shared_ptr<litehtml::document>& doc, bool before);

		void add_style(const litehtml::style& st) override;
		void apply_stylesheet(const litehtml::css& stylesheet) override;
	private:
		void	add_text(const tstring& txt);
		void	add_function(const tstring& fnc, const tstring& params);
		static tstring convert_escape(const tchar_t* txt);
	};

	class el_before : public el_before_after_base
	{
	public:
		explicit el_before(const std::shared_ptr<litehtml::document>& doc) : el_before_after_base(doc, true)
		{

		}
	};

	class el_after : public el_before_after_base
	{
	public:
		explicit el_after(const std::shared_ptr<litehtml::document>& doc) : el_before_after_base(doc, false)
		{

		}
	};
}

#endif  // LH_EL_BEFORE_AFTER_H
