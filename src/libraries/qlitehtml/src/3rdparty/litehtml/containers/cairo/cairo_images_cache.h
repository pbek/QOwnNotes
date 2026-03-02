#ifndef LITEHTML_CAIRO_IMAGES_CACHE_H
#define LITEHTML_CAIRO_IMAGES_CACHE_H

#include <mutex>
#include <map>
#include <string>
#include <cairo.h>

class cairo_surface_wrapper
{
	cairo_surface_t* surface;

  public:
	cairo_surface_wrapper() :
		surface(nullptr)
	{
	}
	cairo_surface_wrapper(const cairo_surface_wrapper& v) :
		surface(v.surface)
	{
		if(v.surface)
		{
			surface = cairo_surface_reference(v.surface);
		}
	}
	explicit cairo_surface_wrapper(cairo_surface_t* v) :
		surface(v)
	{
	}
	cairo_surface_wrapper(cairo_surface_wrapper&& v) noexcept
	{
		surface	  = v.surface;
		v.surface = nullptr;
	}
	cairo_surface_wrapper& operator=(const cairo_surface_wrapper& v)
	{
		if(surface != v.surface)
		{
			if(surface)
			{
				cairo_surface_destroy(surface);
			}
			surface = cairo_surface_reference(v.surface);
		}
		return *this;
	}
	~cairo_surface_wrapper()
	{
		if(surface)
		{
			cairo_surface_destroy(surface);
		}
	}
	cairo_surface_t* get() { return cairo_surface_reference(surface); }
};

class cairo_images_cache
{
	std::mutex									 m_mutex;
	std::map<std::string, cairo_surface_wrapper> m_images;

  public:
	void add_image(const std::string& url, cairo_surface_t* image)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_images[url] = cairo_surface_wrapper(image);
	}

	cairo_surface_t* get_image(const std::string& url)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		auto						 iter = m_images.find(url);
		if(iter != m_images.end())
		{
			return iter->second.get();
		}
		return nullptr;
	}

	bool reserve(const std::string& url)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		auto						 iter = m_images.find(url);
		if(iter == m_images.end())
		{
			m_images[url] = cairo_surface_wrapper();
			return true;
		}
		return false;
	}

	bool exists(const std::string& url)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		auto						 iter = m_images.find(url);
		return iter != m_images.end();
	}
};

#endif // LITEHTML_CAIRO_IMAGES_CACHE_H
