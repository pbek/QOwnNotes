#include "html.h"
#include "el_anchor.h"
#include "document.h"

litehtml::el_anchor::el_anchor(const std::shared_ptr<litehtml::document>& doc) : html_tag(doc)
{
}

void litehtml::el_anchor::on_click()
{
	const tchar_t* href = get_attr(_t("href"));

	if(href)
	{
		get_document()->container()->on_anchor_click(href, shared_from_this());
	}
}

void litehtml::el_anchor::apply_stylesheet( const litehtml::css& stylesheet )
{
	if( get_attr(_t("href")) )
	{
		m_pseudo_classes.push_back(_t("link"));
	}
	html_tag::apply_stylesheet(stylesheet);
}
