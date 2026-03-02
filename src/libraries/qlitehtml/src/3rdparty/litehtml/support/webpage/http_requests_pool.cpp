#include "http_requests_pool.h"

litebrowser::http_requests_pool::http_requests_pool(int pool_size, const std::function<void()>& cb_update_state) :
	m_cancel(false),
	m_cb_update_state(cb_update_state)
{
	for(int i = 0; i < pool_size; i++)
	{
		m_threads.emplace_back(std::make_shared<pool_thread>(this));
	}
}

void litebrowser::http_requests_pool::enqueue(
	const std::string&																	url,
	const std::function<void(void* data, size_t len, size_t downloaded, size_t total)>& cb_on_data,
	const std::function<void(u_int32_t http_status, u_int32_t err_code, const std::string& err_text,
							 const std::string& url)>&									cb_on_finish)
{
	{
		std::unique_lock<std::mutex> lock(m_queue_mutex);
		auto						 request = std::make_shared<http_request>(url, cb_on_data, cb_on_finish);
		m_queue.push_back(request);
	}
	m_mutex_condition.notify_one();
}

void litebrowser::http_requests_pool::stop()
{
	{
		std::unique_lock<std::mutex> lock(m_queue_mutex);
		m_cancel = true;
	}
	m_mutex_condition.notify_all();
	for(auto& active_thread : m_threads)
	{
		auto request = active_thread->get_request();
		if(request)
		{
			request->cancel();
		}
	}
	for(auto& active_thread : m_threads)
	{
		active_thread->thread().join();
	}
	m_threads.clear();
}

bool litebrowser::http_requests_pool::is_downloading()
{
	{
		std::unique_lock<std::mutex> lock(m_queue_mutex);
		if(m_cancel)
			return false;
		if(!m_queue.empty())
			return true;
	}
	for(auto& active_thread : m_threads)
	{
		auto request = active_thread->get_request();
		if(request)
		{
			return true;
		}
	}

	return false;
}

void litebrowser::pool_thread::thread_loop()
{
	while(true)
	{
		{
			std::unique_lock<std::mutex> lock(m_pool->m_queue_mutex);
			m_pool->m_mutex_condition.wait(lock, [this] { return !m_pool->m_queue.empty() || m_pool->m_cancel; });
			if(m_pool->m_cancel)
			{
				if(m_pool->m_cb_update_state)
				{
					m_pool->m_cb_update_state();
				}
				return;
			}
			{
				std::unique_lock<std::mutex> request_lock(m_request_mutex);
				m_request = m_pool->m_queue.front();
			}
			m_pool->m_queue.pop_front();
		}
		if(m_pool->m_cb_update_state)
		{
			m_pool->m_cb_update_state();
		}
		m_request->open();
		{
			std::unique_lock<std::mutex> request_lock(m_request_mutex);
			m_request = nullptr;
		}
		if(m_pool->m_cb_update_state)
		{
			m_pool->m_cb_update_state();
		}
	}
}
