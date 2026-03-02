#include "web_history.h"

void web_history::url_opened(const std::string& url)
{
	if(!m_items.empty())
	{
		if(m_current_item != m_items.size() - 1)
		{
			if(m_current_item > 0 && m_items[m_current_item - 1] == url)
			{
				m_current_item--;
			} else if(m_current_item < m_items.size() - 1 && m_items[m_current_item + 1] == url)
			{
				m_current_item++;
			} else
			{
				m_items.erase(m_items.begin() + m_current_item + 1, m_items.end());
				m_items.push_back(url);
				m_current_item = m_items.size() - 1;
			}
		} else
		{
			if(m_current_item > 0 && m_items[m_current_item - 1] == url)
			{
				m_current_item--;
			} else
			{
				m_items.push_back(url);
				m_current_item = m_items.size() - 1;
			}
		}
	} else
	{
		m_items.push_back(url);
		m_current_item = m_items.size() - 1;
	}
}

bool web_history::back(std::string& url)
{
	if(m_items.empty())
		return false;

	if(m_current_item > 0)
	{
		url = m_items[m_current_item - 1];
		return true;
	}
	return false;
}

bool web_history::forward(std::string& url)
{
	if(m_items.empty())
		return false;

	if(m_current_item < m_items.size() - 1)
	{
		url = m_items[m_current_item + 1];
		return true;
	}
	return false;
}
