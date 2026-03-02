#ifndef LH_MEDIA_QUERY_H
#define LH_MEDIA_QUERY_H

#include "css_tokenizer.h"
#include "string_id.h"
#include "types.h"

namespace litehtml
{
	// https://drafts.csswg.org/mediaqueries/#evaluating
	enum trilean
	{
		False,
		True,
		Unknown
	};
	static_assert(False == false && True == true); // for casting bool to trilean

	inline trilean operator!(trilean x)
	{
		if(x == False)
			return True;
		if(x == True)
			return False;
		return Unknown;
	}

	inline trilean operator&&(trilean a, trilean b)
	{
		if(a == False || b == False)
			return False;
		if(a == True && b == True)
			return True;
		return Unknown;
	}

	inline trilean operator||(trilean a, trilean b)
	{
		if(a == True || b == True)
			return True;
		if(a == False && b == False)
			return False;
		return Unknown;
	}

	struct media_condition;

	// <media-query> = <media-condition> | [ not | only ]? <media-type> [ and <media-condition-without-or> ]?
	struct media_query
	{
		bool					m_not		 = true;
		media_type				m_media_type = media_type_all;
		vector<media_condition> m_conditions;

		trilean check(const media_features& features) const;
	};

	struct media_in_parens;

	// <media-condition> = not <media-in-parens> | <media-in-parens> [ [and <media-in-parens>]* | [or
	// <media-in-parens>]* ]
	struct media_condition
	{
		string_id				op = _and_; // _not_ _and_ _or_
		vector<media_in_parens> m_conditions;

		trilean check(const media_features& features) const;
	};

	// <media-feature> = ( [ <mf-plain> | <mf-boolean> | <mf-range> ] )
	// <mf-plain> = <mf-name> : <mf-value>
	struct media_feature
	{
		string name;
		float  value  = 0;
		float  value2 = 0;
		short  op	  = 0;
		short  op2	  = 0;

		bool verify_and_convert_units(string_id syntax, css_token val[2] = 0, css_token val2[2] = 0,
									  shared_ptr<document> doc = 0);

		bool compare(int x) const { return compare((float) x); }
		bool compare(float x) const;
		bool check(const media_features& features) const;
	};

	// <media-in-parens> = ( <media-condition> ) | <media-feature> | <general-enclosed>
	struct unknown
	{
	}; // <general-enclosed>
	struct media_in_parens : variant<media_condition, media_feature, unknown>
	{
		using base::base; // inherit ctors

		trilean check(const media_features& features) const;
	};

	struct media_query_list
	{
		vector<media_query> m_queries;
		bool				empty() const { return m_queries.empty(); }
		bool				check(const media_features& features) const;
	};
	media_query_list parse_media_query_list(const css_token_vector& tokens, shared_ptr<document> doc);
	media_query_list parse_media_query_list(const string& str, shared_ptr<document> doc);

	class media_query_list_list
	{
	  public:
		using ptr	 = shared_ptr<media_query_list_list>;
		using vector = std::vector<ptr>;

	  private:
		std::vector<media_query_list> m_media_query_lists;
		bool						  m_is_used = false;

	  public:
		void add(const media_query_list& mq_list) { m_media_query_lists.push_back(mq_list); }
		bool is_used() const { return m_is_used; }

		bool apply_media_features(const media_features& features); // returns true if the m_is_used changed
	};

} // namespace litehtml

#endif // LH_MEDIA_QUERY_H
