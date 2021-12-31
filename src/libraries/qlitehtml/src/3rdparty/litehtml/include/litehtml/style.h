#ifndef LH_STYLE_H
#define LH_STYLE_H

#include "attributes.h"
#include <string>

namespace litehtml
{
	class property_value
	{
	public:
		tstring	m_value;
		bool			m_important;

		property_value()
		{
			m_important = false;
		}
		property_value(const tchar_t* val, bool imp)
		{
			m_important = imp;
			m_value		= val;
		}
		property_value(const property_value& val)
		{
			m_value		= val.m_value;
			m_important	= val.m_important;
		}

		property_value& operator=(const property_value& val)
		{
			m_value		= val.m_value;
			m_important	= val.m_important;
			return *this;
		}
	};

	typedef std::map<tstring, property_value>	props_map;

	class style
	{
	public:
		typedef std::shared_ptr<style>		ptr;
		typedef std::vector<style::ptr>		vector;
	private:
		props_map			m_properties;
		static string_map	m_valid_values;
	public:
		style() = default;
		style(const style& val);

		style& operator=(const style& val)
		{
			m_properties = val.m_properties;
            return *this;
		}

		void add(const tchar_t* txt, const tchar_t* baseurl)
		{
			parse(txt, baseurl);
		}

		void add_property(const tchar_t* name, const tchar_t* val, const tchar_t* baseurl, bool important);

		const tchar_t* get_property(const tchar_t* name) const
		{
			if(name)
			{
				auto f = m_properties.find(name);
				if(f != m_properties.end())
				{
					return f->second.m_value.c_str();
				}
			}
			return nullptr;
		}

		void combine(const litehtml::style& src);
		void clear()
		{
			m_properties.clear();
		}

	private:
		void parse_property(const tstring& txt, const tchar_t* baseurl);
		void parse(const tchar_t* txt, const tchar_t* baseurl);
		void parse_short_border(const tstring& prefix, const tstring& val, bool important);
		void parse_short_background(const tstring& val, const tchar_t* baseurl, bool important);
		void parse_short_font(const tstring& val, bool important);
		void add_parsed_property(const tstring& name, const tstring& val, bool important);
		void remove_property(const tstring& name, bool important);
	};
}

#endif  // LH_STYLE_H
