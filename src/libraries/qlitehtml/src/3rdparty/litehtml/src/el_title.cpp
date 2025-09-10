#include "el_title.h"

#include "document.h"
#include "html.h"

litehtml::el_title::el_title(const std::shared_ptr<litehtml::document>& doc) : litehtml::html_tag(doc) {}

void litehtml::el_title::parse_attributes() {
  tstring text;
  get_text(text);
  get_document()->container()->set_caption(text.c_str());
}
