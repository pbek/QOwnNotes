#ifndef LH_MEDIA_QUERY_H
#define LH_MEDIA_QUERY_H

namespace litehtml
{
	struct media_query_expression
	{
		typedef std::vector<media_query_expression>	vector;
		media_feature	feature;
		int				val;
		int				val2;
		bool			check_as_bool;
		
		media_query_expression()
		{
			check_as_bool	= false;
			feature			= media_feature_none;
			val				= 0;
			val2			= 0;
		}

		bool check(const media_features& features) const;
	};

	class media_query
	{
	public:
		typedef std::shared_ptr<media_query>	ptr;
		typedef std::vector<media_query::ptr>	vector;
	private:
		media_query_expression::vector	m_expressions;
		bool							m_not;
		media_type						m_media_type;
	public:
		media_query();
		media_query(const media_query& val);

		static media_query::ptr create_from_string(const tstring& str, const std::shared_ptr<document>& doc);
		bool check(const media_features& features) const;
	};

	class media_query_list
	{
	public:
		typedef std::shared_ptr<media_query_list>	ptr;
		typedef std::vector<media_query_list::ptr>	vector;
	private:
		media_query::vector	m_queries;
		bool				m_is_used;
	public:
		media_query_list();
		media_query_list(const media_query_list& val);

		static media_query_list::ptr create_from_string(const tstring& str, const std::shared_ptr<document>& doc);
		bool is_used() const;
		bool apply_media_features(const media_features& features);	// returns true if the m_is_used changed
	};

	inline media_query_list::media_query_list(const media_query_list& val)
	{
		m_is_used	= val.m_is_used;
		m_queries	= val.m_queries;
	}

	inline media_query_list::media_query_list()
	{
		m_is_used = false;
	}

	inline bool media_query_list::is_used() const
	{
		return m_is_used;
	}

}

#endif  // LH_MEDIA_QUERY_H
