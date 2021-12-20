#include "html.h"
#include "el_link.h"
#include "document.h"


litehtml::el_link::el_link(const std::shared_ptr<litehtml::document>& doc) : litehtml::html_tag(doc)
{

}

void litehtml::el_link::parse_attributes()
{
	bool processed = false;

	document::ptr doc = get_document();

	const tchar_t* rel = get_attr(_t("rel"));
	if(rel && !t_strcmp(rel, _t("stylesheet")))
	{
		const tchar_t* media	= get_attr(_t("media"));
		const tchar_t* href		= get_attr(_t("href"));
		if(href && href[0])
		{
			tstring css_text;
			tstring css_baseurl;
			doc->container()->import_css(css_text, href, css_baseurl);
			if(!css_text.empty())
			{
				doc->add_stylesheet(css_text.c_str(), css_baseurl.c_str(), media);
				processed = true;
			}
		}
	}

	if(!processed)
	{
		doc->container()->link(doc, shared_from_this());
	}
}
