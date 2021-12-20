#ifndef LH_CONTEXT_H
#define LH_CONTEXT_H

#include "stylesheet.h"

namespace litehtml
{
	class context
	{
		litehtml::css	m_master_css;
	public:
		void			load_master_stylesheet(const tchar_t* str);
		litehtml::css&	master_css()
		{
			return m_master_css;
		}
	};
}

#endif  // LH_CONTEXT_H
