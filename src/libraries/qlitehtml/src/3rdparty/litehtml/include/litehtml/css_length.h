#ifndef LH_CSS_LENGTH_H
#define LH_CSS_LENGTH_H

#include "types.h"
#include "css_tokenizer.h"

namespace litehtml
{
	// from_token options (flags)
	enum
	{
		f_length			= 1,					   // <length> (includes unitless zero)
		f_percentage		= 2,					   // <percentage>
		f_length_percentage = f_length | f_percentage, // <length-percentage>
		f_number			= 4,					   // <number>
		f_integer			= 8,					   // <integer>
		f_positive			= 16					   // ⩾0 (non-negative)
	};

	// <length> | <percentage> | <number> | auto | none | normal
	// https://developer.mozilla.org/en-US/docs/Web/CSS/length
	class css_length
	{
		union {
			float m_value;
			int	  m_predef;
		};
		css_units m_units;
		bool	  m_is_predefined;

	  public:
		css_length();
		css_length(float val, css_units units = css_units_px);
		css_length& operator=(float val);

		bool			  is_predefined() const;
		void			  predef(int val);
		int				  predef() const;
		static css_length predef_value(int val = 0);
		void			  set_value(float val, css_units units);
		float			  val() const;
		css_units		  units() const;
		pixel_t			  calc_percent(pixel_t width) const;
		bool			  from_token(const css_token& token, int options, const string& predefined_keywords = "");
		string			  to_string() const;
	};

	using length_vector = vector<css_length>;

	// css_length inlines

	inline css_length::css_length()
	{
		m_value			= 0;
		m_predef		= 0;
		m_units			= css_units_none;
		m_is_predefined = false;
	}

	inline css_length::css_length(float val, css_units units)
	{
		m_value			= val;
		m_units			= units;
		m_is_predefined = false;
	}

	inline css_length& css_length::operator=(float val)
	{
		m_value			= val;
		m_units			= css_units_px;
		m_is_predefined = false;
		return *this;
	}

	inline bool css_length::is_predefined() const
	{
		return m_is_predefined;
	}

	inline void css_length::predef(int val)
	{
		m_predef		= val;
		m_is_predefined = true;
	}

	inline int css_length::predef() const
	{
		if(m_is_predefined)
		{
			return m_predef;
		}
		return 0;
	}

	inline void css_length::set_value(float val, css_units units)
	{
		m_value			= val;
		m_is_predefined = false;
		m_units			= units;
	}

	inline float css_length::val() const
	{
		if(!m_is_predefined)
		{
			return m_value;
		}
		return 0;
	}

	inline css_units css_length::units() const
	{
		return m_units;
	}

	inline pixel_t css_length::calc_percent(pixel_t width) const
	{
		if(!is_predefined())
		{
			if(units() == css_units_percentage)
			{
				return (pixel_t) (width * m_value / 100.0);
			} else
			{
				return (pixel_t) val();
			}
		}
		return 0;
	}
} // namespace litehtml

#endif // LH_CSS_LENGTH_H
