#include "html.h"
#include "el_base.h"
#include "document.h"

litehtml::el_base::el_base(const std::shared_ptr<litehtml::document>& doc) : html_tag(doc)
{
	
}

void litehtml::el_base::parse_attributes()
{
	get_document()->container()->set_base_url(get_attr(_t("href")));
}
