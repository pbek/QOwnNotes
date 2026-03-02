#include <set>
#include <sstream>
#include "http_request.h"

bool litebrowser::http_request::open()
{
	m_canceled = false;
	CURL* curl = curl_easy_init();

	if(m_url[0] == '/')
	{
		std::set<char>	  chars_to_escape = {' ', ':'};
		std::stringstream new_url;
		new_url << "file://";
		for(auto ch : m_url)
		{
			if(chars_to_escape.find(ch) != chars_to_escape.end())
			{
				new_url << '%' << std::hex << (uint32_t) ch;
			} else
			{
				new_url << ch;
			}
		}
		m_url = new_url.str();
	}

	curl_easy_setopt(curl, CURLOPT_URL, m_url.c_str());
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, (long) 1);
	curl_easy_setopt(curl, CURLOPT_MAXREDIRS, (long) 10);

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_request::write_function);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, (long) 0);

	curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, (long) 512);
	curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, (long) 30);

	curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, http_request::progress_function);
	curl_easy_setopt(curl, CURLOPT_XFERINFODATA, this);

	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, (long) 0);

	char errMessage[1024];
	errMessage[0] = 0;

	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errMessage);
	curl_easy_setopt(curl, CURLOPT_FAILONERROR, (long) 1);
	if(!m_user_agent.empty())
	{
		curl_easy_setopt(curl, CURLOPT_USERAGENT, m_user_agent.c_str());
	} else
	{
		curl_easy_setopt(curl, CURLOPT_USERAGENT,
						 "Mozilla/5.0 (X11; Linux x86_64; rv:122.0) Gecko/20100101 Firefox/122.0");
	}

	curl_slist* slist = nullptr;
	if(!m_headers.empty())
	{
		for(auto& item : m_headers)
		{
			slist = curl_slist_append(slist, item.c_str());
		}
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
	}

	std::string post_str;
	if(!m_form_data.empty())
	{
		for(const auto& field : m_form_data)
		{
			if(!post_str.empty())
				post_str += "&";
			post_str += field.first;
			post_str += "=";
			post_str += field.second;
		}
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_str.c_str());
	}

	m_session	  = curl;
	CURLcode res  = curl_easy_perform(curl);

	long httpCode = 0;

	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

	m_session = nullptr;
	curl_easy_cleanup(curl);

	if(slist)
	{
		curl_slist_free_all(slist);
	}
	m_form_data.clear();
	m_headers.clear();

	if(m_on_finish)
	{
		m_on_finish(httpCode, res, errMessage, m_url);
	}
	return true;
}

size_t litebrowser::http_request::write_function(void* ptr, size_t size, size_t nmemb, void* stream)
{
	auto pThis			 = (http_request*) stream;

	pThis->m_downloaded += size * nmemb;
	if(!pThis->m_total)
	{
		curl_off_t sz = 0;
		curl_easy_getinfo((CURL*) pThis->m_session, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &sz);
		pThis->m_total = (size_t) sz;
	}
	if(pThis->m_on_data)
	{
		pThis->m_on_data(ptr, size * nmemb, pThis->m_downloaded, pThis->m_total);
	}

	return size * nmemb;
}

int litebrowser::http_request::progress_function(void* clientp, curl_off_t /*dltotal*/, curl_off_t /*dlnow*/,
												 curl_off_t /*ultotal*/, curl_off_t /*ulnow*/)
{
	auto pThis = (http_request*) clientp;
	return pThis->m_canceled ? 1 : 0;
}
