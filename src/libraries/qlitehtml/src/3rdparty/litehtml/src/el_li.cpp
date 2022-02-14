#include "html.h"
#include "el_li.h"
#include "document.h"

litehtml::el_li::el_li(const std::shared_ptr<litehtml::document>& doc) : litehtml::html_tag(doc)
{
}

int litehtml::el_li::render(int x, int y, int max_width, bool second_pass)
{
	if (m_list_style_type >= list_style_type_armenian && !m_index_initialized)
	{
		if (auto p = parent())
		{
			const auto hasStart = p->get_attr(_t("start"));
			const int start = hasStart ? t_atoi(hasStart) : 1;
			tchar_t val[2] = { (tchar_t)start, 0 };
			for (int i = 0, n = (int)p->get_children_count(); i < n; ++i)
			{
				auto child = p->get_child(i);
				if (child.get() == this)
				{
					set_attr(_t("list_index"), val);
					break;
				}
				else if (!t_strcmp(child->get_tagName(), _t("li")))
					++val[0];
			}
		}

		m_index_initialized = true;
	}

	return html_tag::render(x, y, max_width, second_pass);
}
