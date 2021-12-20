#ifndef LH_ITERATORS_H
#define LH_ITERATORS_H

#include "types.h"

namespace litehtml
{
	class element;

	class iterator_selector
	{
	public:
		virtual bool select(const element::ptr& el) = 0;

        protected:
		~iterator_selector() = default;
	};

	class elements_iterator
	{
	private:
		struct stack_item
		{
			int				idx;
			element::ptr	el;
			stack_item() : idx(0)
			{
			}
			stack_item(const stack_item& val)
			{
				idx = val.idx;
				el = val.el;
			}
			stack_item(stack_item&& val)
			{
				idx = val.idx;
				el = std::move(val.el);
			}
		};

		std::vector<stack_item>		m_stack;
		element::ptr				m_el;
		int							m_idx;
		iterator_selector*			m_go_inside;
		iterator_selector*			m_select;
	public:

		elements_iterator(const element::ptr& el, iterator_selector* go_inside, iterator_selector* select)
		{ 
			m_el			= el;
			m_idx			= -1; 
			m_go_inside		= go_inside;
			m_select		= select;
		}

		~elements_iterator() = default;

		element::ptr next(bool ret_parent = true);
	
	private:
		void next_idx();
	};

	class go_inside_inline final : public iterator_selector
	{
	public:
		bool select(const element::ptr& el) override;
	};

	class go_inside_table final : public iterator_selector
	{
	public:
		bool select(const element::ptr& el) override;
	};

	class table_rows_selector final : public iterator_selector
	{
	public:
		bool select(const element::ptr& el) override;
	};

	class table_cells_selector final : public iterator_selector
	{
	public:
		bool select(const element::ptr& el) override;
	};
}

#endif  // LH_ITERATORS_H
