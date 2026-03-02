#ifndef LITEBROWSER_HTTP_REQUEST_H
#define LITEBROWSER_HTTP_REQUEST_H

#include <curl/curl.h>
#include <functional>
#include <cstdlib>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include <memory>

namespace litebrowser
{
	class http_requests_pool;

	class http_request : public std::enable_shared_from_this<http_request>
	{
	  private:
		std::function<void(void* data, size_t len, size_t downloaded, size_t total)> m_on_data;
		std::function<void(u_int32_t http_status, u_int32_t err_code, const std::string& err_text,
						   const std::string& url)>
														 m_on_finish;
		volatile bool									 m_canceled;
		size_t											 m_downloaded;
		size_t											 m_total;
		volatile void*									 m_session;
		std::vector<std::string>						 m_headers;
		std::vector<std::pair<std::string, std::string>> m_form_data;
		std::string										 m_user_agent;
		std::string										 m_url;

	  public:
		http_request(std::string																  url,
					 std::function<void(void* data, size_t len, size_t downloaded, size_t total)> cb_on_data,
					 std::function<void(u_int32_t http_status, u_int32_t err_code, const std::string& err_text,
										const std::string& url)>
						 cb_on_finish) :
			m_on_data(std::move(cb_on_data)),
			m_on_finish(std::move(cb_on_finish)),
			m_canceled(false),
			m_downloaded(0),
			m_total(0),
			m_session(nullptr),
			m_url(std::move(url))
		{
		}
		~http_request() {}

		bool open();
		void cancel() { m_canceled = true; }
		void add_header(const std::string& header) { m_headers.push_back(header); }
		void clear_headers() { m_headers.clear(); }
		void add_form_data(const std::string& field, const std::string& value)
		{
			m_form_data.emplace_back(field, value);
		}
		void			   clear_form_data() { m_form_data.clear(); }
		const std::string& get_url() { return m_url; }

	  private:
		void		  thread_proc();
		static size_t write_function(void* ptr, size_t size, size_t nmemb, void* stream);
		static int	  progress_function(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal,
										curl_off_t ulnow);
	};
} // namespace litebrowser

#endif // LITEBROWSER_HTTP_REQUEST_H
