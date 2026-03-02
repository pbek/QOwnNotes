#include "html.h"
#include "stylesheet.h"
#include "css_parser.h"
#include "document.h"
#include "document_container.h"

namespace litehtml
{

	// https://www.w3.org/TR/css-syntax-3/#parse-a-css-stylesheet
	template <class Input> // Input == string or css_token_vector
	void css::parse_css_stylesheet(const Input& input, string baseurl, document::ptr doc,
								   media_query_list_list::ptr media, bool top_level)
	{
		if(doc && media)
			doc->add_media_list(media);

		// To parse a CSS stylesheet, first parse a stylesheet.
		auto rules			= css_parser::parse_stylesheet(input, top_level);
		bool import_allowed = top_level;

		// Interpret all of the resulting top-level qualified rules as style rules, defined below.
		// If any style rule is invalid, or any at-rule is not recognized or is invalid according
		// to its grammar or context, it's a parse error. Discard that rule.
		for(auto rule : rules)
		{
			if(rule->type == raw_rule::qualified)
			{
				if(parse_style_rule(rule, baseurl, doc, media))
					import_allowed = false;
				continue;
			}

			// Otherwise: at-rule
			switch(_id(lowcase(rule->name)))
			{
			case _charset_: // ignored  https://www.w3.org/TR/css-syntax-3/#charset-rule
				break;

			case _import_:
				if(import_allowed)
					parse_import_rule(rule, baseurl, doc, media);
				else
					css_parse_error("incorrect placement of @import rule");
				break;

			// https://www.w3.org/TR/css-conditional-3/#at-media
			// @media <media-query-list> { <stylesheet> }
			case _media_:
				{
					if(rule->block.type != CURLY_BLOCK)
						break;
					auto new_media = media;
					auto mq_list   = parse_media_query_list(rule->prelude, doc);
					// An empty media query list evaluates to true.
					// https://drafts.csswg.org/mediaqueries-5/#example-6f06ee45
					if(!mq_list.empty())
					{
						new_media = make_shared<media_query_list_list>(media ? *media : media_query_list_list());
						new_media->add(mq_list);
					}
					parse_css_stylesheet(rule->block.value, baseurl, doc, new_media, false);
					import_allowed = false;
					break;
				}

			default:
				css_parse_error("unrecognized rule @" + rule->name);
			}
		}
	}

	// https://drafts.csswg.org/css-cascade-5/#at-import
	// `layer` and `supports` are not supported
	// @import [ <url> | <string> ] <media-query-list>?
	void css::parse_import_rule(raw_rule::ptr rule, string baseurl, document::ptr doc, media_query_list_list::ptr media)
	{
		auto tokens = rule->prelude;
		int	 index	= 0;
		skip_whitespace(tokens, index);
		auto   tok = at(tokens, index);
		string url;
		auto   parse_string = [](const css_token& tok, string& str) {
			  if(tok.type != STRING)
				  return false;
			  str = tok.str;
			  return true;
		};
		bool ok = parse_url(tok, url) || parse_string(tok, url);
		if(!ok)
		{
			css_parse_error("invalid @import rule");
			return;
		}
		document_container* container = doc->container();
		string				css_text;
		string				css_baseurl = baseurl;
		container->import_css(css_text, url, css_baseurl);

		auto new_media = media;
		tokens		   = slice(tokens, index + 1);
		auto mq_list   = parse_media_query_list(tokens, doc);
		if(!mq_list.empty())
		{
			new_media = make_shared<media_query_list_list>(media ? *media : media_query_list_list());
			new_media->add(mq_list);
		}

		parse_css_stylesheet(css_text, css_baseurl, doc, new_media, true);
	}

	// https://www.w3.org/TR/css-syntax-3/#style-rules
	bool css::parse_style_rule(raw_rule::ptr rule, string baseurl, document::ptr doc, media_query_list_list::ptr media)
	{
		// The prelude of the qualified rule is parsed as a <selector-list>. If this returns failure, the entire style
		// rule is invalid.
		auto list = parse_selector_list(rule->prelude, strict_mode, doc->mode());
		if(list.empty())
		{
			css_parse_error("invalid selector");
			return false;
		}

		style::ptr style = make_shared<litehtml::style>(); // style block
		// The content of the qualified rule's block is parsed as a style block's contents.
		style->add(rule->block.value, baseurl, doc->container());

		for(auto sel : list)
		{
			sel->m_style	   = style;
			sel->m_media_query = media;
			sel->calc_specificity();
			add_selector(sel);
		}
		return true;
	}

	void css::sort_selectors()
	{
		std::sort(m_selectors.begin(), m_selectors.end(),
				  [](const css_selector::ptr& v1, const css_selector::ptr& v2) { return (*v1) < (*v2); });
	}

} // namespace litehtml
