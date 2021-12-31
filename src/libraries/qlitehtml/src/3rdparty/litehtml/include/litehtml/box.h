#ifndef LH_BOX_H
#define LH_BOX_H

namespace litehtml
{
	class html_tag;

	enum box_type
	{
		box_block,
		box_line
	};

	class box
	{
	public:
		typedef std::unique_ptr<litehtml::box>	ptr;
		typedef std::vector< box::ptr >			vector;
	protected:
		int		m_box_top;
		int		m_box_left;
		int		m_box_right;
	public:
		box(int top, int left, int right)
		{
			m_box_top	= top;
			m_box_left	= left;
			m_box_right	= right;
		}
		virtual ~box() = default;

		int		bottom() const	{ return m_box_top + height();	}
		int		top() const		{ return m_box_top;				}
		int		right() const	{ return m_box_left + width();	}
		int		left() const	{ return m_box_left;			}

		virtual litehtml::box_type	get_type() const = 0;
		virtual int					height() const = 0;
		virtual int					width() const = 0;
		virtual void				add_element(const element::ptr &el) = 0;
		virtual bool				can_hold(const element::ptr &el, white_space ws) const = 0;
		virtual void				finish(bool last_box = false) = 0;
		virtual bool				is_empty() const = 0;
		virtual int					baseline() const = 0;
		virtual void				get_elements(elements_vector& els) = 0;
		virtual int					top_margin() const = 0;
		virtual int					bottom_margin() const = 0;
		virtual void				y_shift(int shift) = 0;
		virtual void				new_width(int left, int right, elements_vector& els) = 0;
	};

	//////////////////////////////////////////////////////////////////////////

	class block_box : public box
	{
		element::ptr m_element;
	public:
		block_box(int top, int left, int right) : box(top, left, right)
		{
			m_element = nullptr;
		}

		litehtml::box_type	get_type() const override;
		int					height() const override;
		int					width() const override;
		void				add_element(const element::ptr &el) override;
		bool				can_hold(const element::ptr &el, white_space ws) const override;
		void				finish(bool last_box = false) override;
		bool				is_empty() const override;
		int					baseline() const override;
		void				get_elements(elements_vector& els) override;
		int					top_margin() const override;
		int					bottom_margin() const override;
		void				y_shift(int shift) override;
		void				new_width(int left, int right, elements_vector& els) override;
	};

	//////////////////////////////////////////////////////////////////////////

	class line_box : public box
	{
		elements_vector			m_items;
		int						m_height;
		int						m_width;
		int						m_line_height;
		font_metrics			m_font_metrics;
		int						m_baseline;
		text_align				m_text_align;
	public:
		line_box(int top, int left, int right, int line_height, font_metrics& fm, text_align align) : box(top, left, right)
		{
			m_height		= 0;
			m_width			= 0;
			m_font_metrics	= fm;
			m_line_height	= line_height;
			m_baseline		= 0;
			m_text_align	= align;
		}

		litehtml::box_type	get_type() const override;
		int					height() const override;
		int					width() const override;
		void				add_element(const element::ptr &el) override;
		bool				can_hold(const element::ptr &el, white_space ws) const override;
		void				finish(bool last_box = false) override;
		bool				is_empty() const override;
		int					baseline() const override;
		void				get_elements(elements_vector& els) override;
		int					top_margin() const override;
		int					bottom_margin() const override;
		void				y_shift(int shift) override;
		void				new_width(int left, int right, elements_vector& els) override;

	private:
		bool						have_last_space() const;
		bool						is_break_only() const;
	};
}

#endif  // LH_BOX_H
