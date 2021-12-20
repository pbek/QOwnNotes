#include "html.h"
#include "el_div.h"


litehtml::el_div::el_div(const std::shared_ptr<litehtml::document>& doc) : html_tag(doc)
{

}

void litehtml::el_div::parse_attributes()
{
	const tchar_t* str = get_attr(_t("align"));
	if(str)
	{
		m_style.add_property(_t("text-align"), str, 0, false);
	}
	html_tag::parse_attributes();
}
