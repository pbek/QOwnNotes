#ifndef LITEBROWSER_HTTP_REQUESTS_POOL_H
#define LITEBROWSER_HTTP_REQUESTS_POOL_H

#include <memory>
#include <mutex>
#include <list>
#include <condition_variable>
#include "http_request.h"

namespace litebrowser
{
	class http_requests_pool;

	class pool_thread
	{
		http_requests_pool*			  m_pool;
		std::shared_ptr<http_request> m_request;
		std::mutex					  m_request_mutex;
		std::thread					  m_thread;

	  public:
		explicit pool_thread(http_requests_pool* pool) :
			m_pool(pool)
		{
			m_thread = std::thread(&pool_thread::thread_loop, this);
		}
		std::shared_ptr<http_request> get_request()
		{
			std::unique_lock<std::mutex> request_lock(m_request_mutex);
			return m_request;
		}
		std::thread& thread() { return m_thread; }

	  private:
		void thread_loop();
	};

	class http_requests_pool : public std::enable_shared_from_this<http_requests_pool>
	{
		friend class pool_thread;

	  protected:
		bool									  m_cancel;
		std::mutex								  m_queue_mutex;
		std::condition_variable					  m_mutex_condition;
		std::list<std::shared_ptr<http_request>>  m_queue;
		std::vector<std::shared_ptr<pool_thread>> m_threads;
		std::function<void()>					  m_queue_empty_cb;
		std::function<void()>					  m_cb_update_state;

	  public:
		explicit http_requests_pool(int pool_size, const std::function<void()>& cb_update_state);
		~http_requests_pool() { stop(); }

		void enqueue(const std::string&																	 url,
					 const std::function<void(void* data, size_t len, size_t downloaded, size_t total)>& cb_on_data,
					 const std::function<void(u_int32_t http_status, u_int32_t err_code, const std::string& err_text,
											  const std::string& url)>&									 cb_on_finish);
		void stop();
		bool is_downloading();

	  private:
	};
} // namespace litebrowser

#endif // LITEBROWSER_HTTP_REQUESTS_POOL_H
