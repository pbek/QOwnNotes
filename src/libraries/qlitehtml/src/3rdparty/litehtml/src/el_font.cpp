#include "html.h"
#include "el_font.h"


litehtml::el_font::el_font(const std::shared_ptr<litehtml::document>& doc) : html_tag(doc)
{

}

void litehtml::el_font::parse_attributes()
{
	const tchar_t* str = get_attr(_t("color"));
	if(str)
	{
		m_style.add_property(_t("color"), str, nullptr, false);
	}

	str = get_attr(_t("face"));
	if(str)
	{
		m_style.add_property(_t("font-face"), str, nullptr, false);
	}

	str = get_attr(_t("size"));
	if(str)
	{
		int sz = t_atoi(str);
		if(sz <= 1)
		{
			m_style.add_property(_t("font-size"), _t("x-small"), nullptr, false);
		} else if(sz >= 6)
		{
			m_style.add_property(_t("font-size"), _t("xx-large"), nullptr, false);
		} else
		{
			switch(sz)
			{
			case 2:
				m_style.add_property(_t("font-size"), _t("small"), nullptr, false);
				break;
			case 3:
				m_style.add_property(_t("font-size"), _t("medium"), nullptr, false);
				break;
			case 4:
				m_style.add_property(_t("font-size"), _t("large"), nullptr, false);
				break;
			case 5:
				m_style.add_property(_t("font-size"), _t("x-large"), nullptr, false);
				break;
			}
		}
	}

	html_tag::parse_attributes();
}
