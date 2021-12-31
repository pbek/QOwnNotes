#include "html.h"
#include "el_tr.h"


litehtml::el_tr::el_tr(const std::shared_ptr<litehtml::document>& doc) : html_tag(doc)
{

}

void litehtml::el_tr::parse_attributes()
{
	const tchar_t* str = get_attr(_t("align"));
	if(str)
	{
		m_style.add_property(_t("text-align"), str, nullptr, false);
	}
	str = get_attr(_t("valign"));
	if(str)
	{
		m_style.add_property(_t("vertical-align"), str, nullptr, false);
	}
	str = get_attr(_t("bgcolor"));
	if (str)
	{
		m_style.add_property(_t("background-color"), str, nullptr, false);
	}
	html_tag::parse_attributes();
}

void litehtml::el_tr::get_inline_boxes( position::vector& boxes )
{
	position pos;
	for(auto& el : m_children)
	{
		if(el->get_display() == display_table_cell)
		{
			pos.x		= el->left() + el->margin_left();
			pos.y		= el->top() - m_padding.top - m_borders.top;

			pos.width	= el->right() - pos.x - el->margin_right() - el->margin_left();
			pos.height	= el->height() + m_padding.top + m_padding.bottom + m_borders.top + m_borders.bottom;

			boxes.push_back(pos);
		}
	}
}
