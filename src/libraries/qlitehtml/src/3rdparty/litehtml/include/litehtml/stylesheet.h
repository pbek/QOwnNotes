#ifndef LH_STYLESHEET_H
#define LH_STYLESHEET_H

#include "css_selector.h"
#include "css_tokenizer.h"

namespace litehtml
{

	// https://www.w3.org/TR/cssom-1/#css-declarations
	struct raw_declaration
	{
		using vector = std::vector<raw_declaration>;

		string			 name; // property name
		css_token_vector value =
			{}; // default value is specified here to get rid of gcc warning "missing initializer for member"
		bool important = false;

		operator bool() const { return name != ""; }
	};

	// intermediate half-parsed rule that is used internally by the parser
	class raw_rule
	{
	  public:
		using ptr	 = shared_ptr<raw_rule>;
		using vector = std::vector<ptr>;

		enum rule_type
		{
			qualified,
			at
		};

		raw_rule(rule_type type, string name = "") :
			type(type),
			name(name)
		{
		}

		rule_type type;
		// An at-rule has a name, a prelude consisting of a list of component values, and an optional block consisting
		// of a simple {} block.
		string name;
		// https://www.w3.org/TR/css-syntax-3/#qualified-rule
		// A qualified rule has a prelude consisting of a list of component values, and a block consisting of a simple
		// {} block. Note: Most qualified rules will be style rules, where the prelude is a selector and the block a
		// list of declarations.
		css_token_vector prelude;
		css_token		 block;
	};

	class css
	{
		css_selector::vector m_selectors;

	  public:
		const css_selector::vector& selectors() const { return m_selectors; }

		template <class Input>
		void parse_css_stylesheet(const Input& input, string baseurl, shared_ptr<document> doc,
								  media_query_list_list::ptr media = nullptr, bool top_level = true);

		void sort_selectors();

	  private:
		bool parse_style_rule(raw_rule::ptr rule, string baseurl, shared_ptr<document> doc,
							  media_query_list_list::ptr media);
		void parse_import_rule(raw_rule::ptr rule, string baseurl, shared_ptr<document> doc,
							   media_query_list_list::ptr media);
		void add_selector(const css_selector::ptr& selector);
	};

	inline void css::add_selector(const css_selector::ptr& selector)
	{
		selector->m_order = (int) m_selectors.size();
		m_selectors.push_back(selector);
	}

} // namespace litehtml

#endif // LH_STYLESHEET_H
