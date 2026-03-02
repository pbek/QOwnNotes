#ifndef LH_CSS_SELECTOR_H
#define LH_CSS_SELECTOR_H

#include "string_id.h"
#include "style.h"
#include "media_query.h"
#include "css_tokenizer.h"

namespace litehtml
{
	//////////////////////////////////////////////////////////////////////////

	struct selector_specificity
	{
		int a;
		int b;
		int c;
		int d;

		explicit selector_specificity(int va = 0, int vb = 0, int vc = 0, int vd = 0)
		{
			a = va;
			b = vb;
			c = vc;
			d = vd;
		}

		void operator+=(const selector_specificity& val)
		{
			a += val.a;
			b += val.b;
			c += val.c;
			d += val.d;
		}

		bool operator==(const selector_specificity& val) const
		{
			if(a == val.a && b == val.b && c == val.c && d == val.d)
			{
				return true;
			}
			return false;
		}

		bool operator!=(const selector_specificity& val) const
		{
			if(a != val.a || b != val.b || c != val.c || d != val.d)
			{
				return true;
			}
			return false;
		}

		bool operator>(const selector_specificity& val) const
		{
			if(a > val.a)
			{
				return true;
			} else if(a < val.a)
			{
				return false;
			} else
			{
				if(b > val.b)
				{
					return true;
				} else if(b < val.b)
				{
					return false;
				} else
				{
					if(c > val.c)
					{
						return true;
					} else if(c < val.c)
					{
						return false;
					} else
					{
						if(d > val.d)
						{
							return true;
						} else if(d < val.d)
						{
							return false;
						}
					}
				}
			}
			return false;
		}

		bool operator>=(const selector_specificity& val) const
		{
			if((*this) == val)
				return true;
			if((*this) > val)
				return true;
			return false;
		}

		bool operator<=(const selector_specificity& val) const
		{
			if((*this) > val)
			{
				return false;
			}
			return true;
		}

		bool operator<(const selector_specificity& val) const
		{
			if((*this) <= val && (*this) != val)
			{
				return true;
			}
			return false;
		}
	};

	//////////////////////////////////////////////////////////////////////////

	enum attr_select_type
	{
		select_class,
		select_id,
		select_attr,
		select_pseudo_class,
		select_pseudo_element,
	};

	// https://www.w3.org/TR/selectors-4/#attribute-selectors
	enum attr_matcher : char
	{
		attribute_exists					= 0,
		attribute_equals					= '=',
		attribute_contains_string			= '*', // *=
		attribute_contains_word				= '~', // ~=
		attribute_starts_with_string		= '^', // ^=
		attribute_starts_with_string_hyphen = '|', // |=
		attribute_ends_with_string			= '$', // $=
	};

	//////////////////////////////////////////////////////////////////////////

	class css_selector;
	class html_tag;

	// <subclass-selector> | <pseudo-element-selector>
	// = <id-selector> | <class-selector> | <attribute-selector> | <pseudo-class-selector> | <pseudo-element-selector>
	struct css_attribute_selector
	{
		using vector = std::vector<css_attribute_selector>;

		attr_select_type type;
		string_id		 prefix; // [prefix|name]
		string_id		 name;	 // .name, #name, [name], :name
		string			 value;	 // [name=value], :lang(value)

		attr_matcher matcher;		 // <attr-matcher>   = ~= |= ^= $= *=
		bool		 caseless_match; // value is matched ASCII case-insensitively

		std::vector<shared_ptr<css_selector>> selector_list; // :not(selector_list)
		int									  a, b;			 // :nth-child(an+b of selector_list)

		css_attribute_selector(attr_select_type type = select_class, string name = "") :
			type(type),
			prefix(empty_id),
			name(_id(name)),
			matcher(),
			caseless_match(0),
			a(0),
			b(0)
		{
		}

		operator bool() const { return name != empty_id; }
	};

	//////////////////////////////////////////////////////////////////////////

	class css_element_selector // compound selector: div.class:hover
	{
	  public:
		using ptr = shared_ptr<css_element_selector>;

	  public:
		string_id					   m_prefix;
		string_id					   m_tag;
		css_attribute_selector::vector m_attrs; // subclass and pseudo-element selectors
	};

	//////////////////////////////////////////////////////////////////////////

	enum css_combinator
	{
		combinator_descendant		= ' ',
		combinator_child			= '>',
		combinator_adjacent_sibling = '+',
		combinator_general_sibling	= '~'
	};

	//////////////////////////////////////////////////////////////////////////

	class css_selector // complex selector: div + p
	{
	  public:
		using ptr	 = shared_ptr<css_selector>;
		using vector = std::vector<css_selector::ptr>;

	  public:
		selector_specificity	   m_specificity;
		int						   m_order = 0;
		css_selector::ptr		   m_left;
		css_element_selector	   m_right;
		css_combinator			   m_combinator = combinator_descendant;
		media_query_list_list::ptr m_media_query;
		style::ptr				   m_style;

	  public:
		bool parse(const string& text, document_mode mode);
		void calc_specificity();
		bool is_media_valid() const;
		void add_media_to_doc(document* doc) const;
	};

	inline bool css_selector::is_media_valid() const
	{
		if(!m_media_query)
		{
			return true;
		}
		return m_media_query->is_used();
	}

	//////////////////////////////////////////////////////////////////////////

	inline bool operator>(const css_selector& v1, const css_selector& v2)
	{
		if(v1.m_specificity == v2.m_specificity)
		{
			return (v1.m_order > v2.m_order);
		}
		return (v1.m_specificity > v2.m_specificity);
	}

	inline bool operator<(const css_selector& v1, const css_selector& v2)
	{
		if(v1.m_specificity == v2.m_specificity)
		{
			return (v1.m_order < v2.m_order);
		}
		return (v1.m_specificity < v2.m_specificity);
	}

	inline bool operator>(const css_selector::ptr& v1, const css_selector::ptr& v2)
	{
		return (*v1 > *v2);
	}

	inline bool operator<(const css_selector::ptr& v1, const css_selector::ptr& v2)
	{
		return (*v1 < *v2);
	}

	//////////////////////////////////////////////////////////////////////////

	class used_selector
	{
	  public:
		typedef std::unique_ptr<used_selector>	ptr;
		typedef std::vector<used_selector::ptr> vector;

		css_selector::ptr m_selector;
		bool			  m_used;

		used_selector(const css_selector::ptr& selector, bool used)
		{
			m_used	   = used;
			m_selector = selector;
		}
	};

	enum
	{
		strict_mode			   = 0,
		forgiving_mode		   = 1,
		forbid_pseudo_elements = 1 << 1,
	};

	css_selector::vector parse_selector_list(const css_token_vector& tokens, int options, document_mode mode);
} // namespace litehtml

#endif // LH_CSS_SELECTOR_H
