#ifndef LH_EL_BEFORE_AFTER_H
#define LH_EL_BEFORE_AFTER_H

#include "html_tag.h"

namespace litehtml
{
	class el_before_after_base : public html_tag
	{
	  public:
		el_before_after_base(const std::shared_ptr<document>& doc, bool before);

		void add_style(const style& style) override;

	  private:
		void		  add_text(const string& txt);
		void		  add_function(const string& fnc, const string& params);
		static string convert_escape(const char* txt);
	};

	class el_before : public el_before_after_base
	{
	  public:
		explicit el_before(const std::shared_ptr<document>& doc) :
			el_before_after_base(doc, true)
		{
		}
	};

	class el_after : public el_before_after_base
	{
	  public:
		explicit el_after(const std::shared_ptr<document>& doc) :
			el_before_after_base(doc, false)
		{
		}
	};
} // namespace litehtml

#endif // LH_EL_BEFORE_AFTER_H
