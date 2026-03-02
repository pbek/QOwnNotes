#pragma once
#include <string>
#include <vector>

using string_vector = std::vector<std::string>;

class web_history
{
	string_vector			 m_items;
	string_vector::size_type m_current_item = 0;

  public:
	web_history()		   = default;
	virtual ~web_history() = default;

	void url_opened(const std::string& url);
	bool back(std::string& url);
	bool forward(std::string& url);

	[[nodiscard]]
	std::string current() const
	{
		if(!m_items.empty() && m_current_item < m_items.size())
		{
			return m_items[m_current_item];
		}
		return "";
	}
};
