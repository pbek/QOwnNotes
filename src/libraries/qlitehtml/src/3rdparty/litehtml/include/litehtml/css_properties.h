#ifndef LITEHTML_CSS_PROPERTIES_H
#define LITEHTML_CSS_PROPERTIES_H

#include "string_id.h"
#include "types.h"
#include "css_margins.h"
#include "borders.h"
#include "css_offsets.h"
#include "background.h"

namespace litehtml
{
	class html_tag;
	class document;

	template <class CssT, class CompT> class css_property
	{
	  public:
		CssT  css_value;
		CompT computed_value;

		css_property(const CssT& css_val, const CompT& computed_val) :
			css_value(css_val),
			computed_value(computed_val)
		{
		}
	};

	// CSS Properties types
	using css_line_height_t = css_property<css_length, pixel_t>;

	class css_properties
	{
	  private:
		element_position	  m_el_position;
		text_align			  m_text_align;
		overflow			  m_overflow;
		white_space			  m_white_space;
		style_display		  m_display;
		visibility			  m_visibility;
		appearance			  m_appearance;
		box_sizing			  m_box_sizing;
		css_length			  m_z_index;
		vertical_align		  m_vertical_align;
		element_float		  m_float;
		element_clear		  m_clear;
		css_margins			  m_css_margins;
		css_margins			  m_css_padding;
		css_borders			  m_css_borders;
		css_length			  m_css_width;
		css_length			  m_css_height;
		css_length			  m_css_min_width;
		css_length			  m_css_min_height;
		css_length			  m_css_max_width;
		css_length			  m_css_max_height;
		css_offsets			  m_css_offsets;
		css_length			  m_css_text_indent;
		css_length			  m_css_line_height;
		css_line_height_t	  m_line_height{{}, 0};
		list_style_type		  m_list_style_type;
		list_style_position	  m_list_style_position;
		string				  m_list_style_image;
		string				  m_list_style_image_baseurl;
		background			  m_bg;
		uint_ptr			  m_font;
		css_length			  m_font_size;
		string				  m_font_family;
		css_length			  m_font_weight;
		font_style			  m_font_style;
		int					  m_text_decoration_line  = text_decoration_line_none;
		text_decoration_style m_text_decoration_style = text_decoration_style_solid;
		css_length			  m_text_decoration_thickness;
		web_color			  m_text_decoration_color;
		string				  m_text_emphasis_style;
		web_color			  m_text_emphasis_color;
		int					  m_text_emphasis_position;
		font_metrics		  m_font_metrics;
		text_transform		  m_text_transform;
		web_color			  m_color;
		string				  m_cursor;
		string				  m_content;
		border_collapse		  m_border_collapse;
		css_length			  m_css_border_spacing_x;
		css_length			  m_css_border_spacing_y;

		float				 m_flex_grow;
		float				 m_flex_shrink;
		css_length			 m_flex_basis;
		flex_direction		 m_flex_direction;
		flex_wrap			 m_flex_wrap;
		flex_justify_content m_flex_justify_content;
		flex_align_items	 m_flex_align_items;
		flex_align_items	 m_flex_align_self;
		flex_align_content	 m_flex_align_content;

		caption_side m_caption_side;

		int m_order;

	  private:
		void	  compute_font(const html_tag* el, const std::shared_ptr<document>& doc);
		void	  compute_background(const html_tag* el, const std::shared_ptr<document>& doc);
		void	  compute_flex(const html_tag* el, const std::shared_ptr<document>& doc);
		web_color get_color_property(const html_tag* el, string_id name, bool inherited, web_color default_value,
									 uint_ptr member_offset) const;
		void	  snap_border_width(css_length& width, const std::shared_ptr<document>& doc);

	  public:
		css_properties() :
			m_el_position(element_position_static),
			m_text_align(text_align_left),
			m_overflow(overflow_visible),
			m_white_space(white_space_normal),
			m_display(display_inline),
			m_visibility(visibility_visible),
			m_appearance(appearance_none),
			m_box_sizing(box_sizing_content_box),
			m_z_index(0),
			m_vertical_align(va_baseline),
			m_float(float_none),
			m_clear(clear_none),
			m_css_margins(),
			m_css_padding(),
			m_css_borders(),
			m_css_width(),
			m_css_height(),
			m_css_min_width(),
			m_css_min_height(),
			m_css_max_width(),
			m_css_max_height(),
			m_css_offsets(),
			m_css_text_indent(),
			m_css_line_height(0),
			m_list_style_type(list_style_type_none),
			m_list_style_position(list_style_position_outside),
			m_bg(),
			m_font(0),
			m_font_size(0),
			m_font_metrics(),
			m_text_transform(text_transform_none),
			m_border_collapse(border_collapse_separate),
			m_css_border_spacing_x(),
			m_css_border_spacing_y(),
			m_flex_grow(0),
			m_flex_shrink(1),
			m_flex_direction(flex_direction_row),
			m_flex_wrap(flex_wrap_nowrap),
			m_flex_justify_content(flex_justify_content_flex_start),
			m_flex_align_items(flex_align_items_stretch),
			m_flex_align_self(flex_align_items_auto),
			m_flex_align_content(flex_align_content_stretch),
			m_order(0)
		{
		}

		void									compute(const html_tag* el, const std::shared_ptr<document>& doc);
		std::vector<std::tuple<string, string>> dump_get_attrs();

		element_position get_position() const;
		void			 set_position(element_position mElPosition);

		text_align get_text_align() const;
		void	   set_text_align(text_align mTextAlign);

		overflow get_overflow() const;
		void	 set_overflow(overflow mOverflow);

		white_space get_white_space() const;
		void		set_white_space(white_space mWhiteSpace);

		style_display get_display() const;
		void		  set_display(style_display mDisplay);

		visibility get_visibility() const;
		void	   set_visibility(visibility mVisibility);

		appearance get_appearance() const;
		void	   set_appearance(appearance mAppearance);

		box_sizing get_box_sizing() const;
		void	   set_box_sizing(box_sizing mBoxSizing);

		int	 get_z_index() const;
		void set_z_index(int mZIndex);

		vertical_align get_vertical_align() const;
		void		   set_vertical_align(vertical_align mVerticalAlign);

		element_float get_float() const;
		void		  set_float(element_float mFloat);

		element_clear get_clear() const;
		void		  set_clear(element_clear mClear);

		const css_margins& get_margins() const;
		void			   set_margins(const css_margins& mCssMargins);

		const css_margins& get_padding() const;
		void			   set_padding(const css_margins& mCssPadding);

		const css_borders& get_borders() const;
		void			   set_borders(const css_borders& mCssBorders);

		const css_length& get_width() const;
		void			  set_width(const css_length& mCssWidth);

		const css_length& get_height() const;
		void			  set_height(const css_length& mCssHeight);

		const css_length& get_min_width() const;
		void			  set_min_width(const css_length& mCssMinWidth);

		const css_length& get_min_height() const;
		void			  set_min_height(const css_length& mCssMinHeight);

		const css_length& get_max_width() const;
		void			  set_max_width(const css_length& mCssMaxWidth);

		const css_length& get_max_height() const;
		void			  set_max_height(const css_length& mCssMaxHeight);

		const css_offsets& get_offsets() const;
		void			   set_offsets(const css_offsets& mCssOffsets);

		const css_length& get_text_indent() const;
		void			  set_text_indent(const css_length& mCssTextIndent);

		const css_line_height_t& line_height() const;
		css_line_height_t&		 line_height_w();

		list_style_type get_list_style_type() const;
		void			set_list_style_type(list_style_type mListStyleType);

		list_style_position get_list_style_position() const;
		void				set_list_style_position(list_style_position mListStylePosition);

		const string& get_list_style_image() const;
		void		  set_list_style_image(const string& url);

		const string& get_list_style_image_baseurl() const;
		void		  set_list_style_image_baseurl(const string& url);

		const background& get_bg() const;
		void			  set_bg(const background& mBg);

		pixel_t get_font_size() const;
		void	set_font_size(pixel_t mFontSize);

		uint_ptr get_font() const;
		void	 set_font(uint_ptr mFont);

		const font_metrics& get_font_metrics() const;
		void				set_font_metrics(const font_metrics& mFontMetrics);

		text_transform get_text_transform() const;
		void		   set_text_transform(text_transform mTextTransform);

		web_color get_color() const;
		void	  set_color(web_color color);

		const string& get_cursor() const;
		void		  set_cursor(const string& cursor);

		const string& get_content() const;
		void		  set_content(const string& content);

		border_collapse get_border_collapse() const;
		void			set_border_collapse(border_collapse mBorderCollapse);

		const css_length& get_border_spacing_x() const;
		void			  set_border_spacing_x(const css_length& mBorderSpacingX);

		const css_length& get_border_spacing_y() const;
		void			  set_border_spacing_y(const css_length& mBorderSpacingY);

		caption_side get_caption_side() const;
		void		 set_caption_side(caption_side side);

		float				 get_flex_grow() const;
		float				 get_flex_shrink() const;
		const css_length&	 get_flex_basis() const;
		flex_direction		 get_flex_direction() const;
		flex_wrap			 get_flex_wrap() const;
		flex_justify_content get_flex_justify_content() const;
		flex_align_items	 get_flex_align_items() const;
		flex_align_items	 get_flex_align_self() const;
		flex_align_content	 get_flex_align_content() const;

		int	 get_order() const;
		void set_order(int order);

		int					  get_text_decoration_line() const;
		text_decoration_style get_text_decoration_style() const;
		const css_length&	  get_text_decoration_thickness() const;
		const web_color&	  get_text_decoration_color() const;

		string	  get_text_emphasis_style() const;
		web_color get_text_emphasis_color() const;
		int		  get_text_emphasis_position() const;
	};

	inline element_position css_properties::get_position() const
	{
		return m_el_position;
	}

	inline void css_properties::set_position(element_position mElPosition)
	{
		m_el_position = mElPosition;
	}

	inline text_align css_properties::get_text_align() const
	{
		return m_text_align;
	}

	inline void css_properties::set_text_align(text_align mTextAlign)
	{
		m_text_align = mTextAlign;
	}

	inline overflow css_properties::get_overflow() const
	{
		return m_overflow;
	}

	inline void css_properties::set_overflow(overflow mOverflow)
	{
		m_overflow = mOverflow;
	}

	inline white_space css_properties::get_white_space() const
	{
		return m_white_space;
	}

	inline void css_properties::set_white_space(white_space mWhiteSpace)
	{
		m_white_space = mWhiteSpace;
	}

	inline style_display css_properties::get_display() const
	{
		return m_display;
	}

	inline void css_properties::set_display(style_display mDisplay)
	{
		m_display = mDisplay;
	}

	inline visibility css_properties::get_visibility() const
	{
		return m_visibility;
	}

	inline void css_properties::set_visibility(visibility mVisibility)
	{
		m_visibility = mVisibility;
	}

	inline appearance css_properties::get_appearance() const
	{
		return m_appearance;
	}

	inline void css_properties::set_appearance(appearance mAppearance)
	{
		m_appearance = mAppearance;
	}

	inline box_sizing css_properties::get_box_sizing() const
	{
		return m_box_sizing;
	}

	inline void css_properties::set_box_sizing(box_sizing mBoxSizing)
	{
		m_box_sizing = mBoxSizing;
	}

	inline int css_properties::get_z_index() const
	{
		return (int) m_z_index.val();
	}

	inline void css_properties::set_z_index(int mZIndex)
	{
		m_z_index.set_value((float) mZIndex, css_units_none);
	}

	inline vertical_align css_properties::get_vertical_align() const
	{
		return m_vertical_align;
	}

	inline void css_properties::set_vertical_align(vertical_align mVerticalAlign)
	{
		m_vertical_align = mVerticalAlign;
	}

	inline element_float css_properties::get_float() const
	{
		return m_float;
	}

	inline void css_properties::set_float(element_float mFloat)
	{
		m_float = mFloat;
	}

	inline element_clear css_properties::get_clear() const
	{
		return m_clear;
	}

	inline void css_properties::set_clear(element_clear mClear)
	{
		m_clear = mClear;
	}

	inline const css_margins& css_properties::get_margins() const
	{
		return m_css_margins;
	}

	inline void css_properties::set_margins(const css_margins& mCssMargins)
	{
		m_css_margins = mCssMargins;
	}

	inline const css_margins& css_properties::get_padding() const
	{
		return m_css_padding;
	}

	inline void css_properties::set_padding(const css_margins& mCssPadding)
	{
		m_css_padding = mCssPadding;
	}

	inline const css_borders& css_properties::get_borders() const
	{
		return m_css_borders;
	}

	inline void css_properties::set_borders(const css_borders& mCssBorders)
	{
		m_css_borders = mCssBorders;
	}

	inline const css_length& css_properties::get_width() const
	{
		return m_css_width;
	}

	inline void css_properties::set_width(const css_length& mCssWidth)
	{
		m_css_width = mCssWidth;
	}

	inline const css_length& css_properties::get_height() const
	{
		return m_css_height;
	}

	inline void css_properties::set_height(const css_length& mCssHeight)
	{
		m_css_height = mCssHeight;
	}

	inline const css_length& css_properties::get_min_width() const
	{
		return m_css_min_width;
	}

	inline void css_properties::set_min_width(const css_length& mCssMinWidth)
	{
		m_css_min_width = mCssMinWidth;
	}

	inline const css_length& css_properties::get_min_height() const
	{
		return m_css_min_height;
	}

	inline void css_properties::set_min_height(const css_length& mCssMinHeight)
	{
		m_css_min_height = mCssMinHeight;
	}

	inline const css_length& css_properties::get_max_width() const
	{
		return m_css_max_width;
	}

	inline void css_properties::set_max_width(const css_length& mCssMaxWidth)
	{
		m_css_max_width = mCssMaxWidth;
	}

	inline const css_length& css_properties::get_max_height() const
	{
		return m_css_max_height;
	}

	inline void css_properties::set_max_height(const css_length& mCssMaxHeight)
	{
		m_css_max_height = mCssMaxHeight;
	}

	inline const css_offsets& css_properties::get_offsets() const
	{
		return m_css_offsets;
	}

	inline void css_properties::set_offsets(const css_offsets& mCssOffsets)
	{
		m_css_offsets = mCssOffsets;
	}

	inline const css_length& css_properties::get_text_indent() const
	{
		return m_css_text_indent;
	}

	inline void css_properties::set_text_indent(const css_length& mCssTextIndent)
	{
		m_css_text_indent = mCssTextIndent;
	}

	inline const css_line_height_t& css_properties::line_height() const
	{
		return m_line_height;
	}

	inline css_line_height_t& css_properties::line_height_w()
	{
		return m_line_height;
	}

	inline list_style_type css_properties::get_list_style_type() const
	{
		return m_list_style_type;
	}

	inline void css_properties::set_list_style_type(list_style_type mListStyleType)
	{
		m_list_style_type = mListStyleType;
	}

	inline list_style_position css_properties::get_list_style_position() const
	{
		return m_list_style_position;
	}

	inline void css_properties::set_list_style_position(list_style_position mListStylePosition)
	{
		m_list_style_position = mListStylePosition;
	}

	inline const string& css_properties::get_list_style_image() const
	{
		return m_list_style_image;
	}
	inline void css_properties::set_list_style_image(const string& url)
	{
		m_list_style_image = url;
	}

	inline const string& css_properties::get_list_style_image_baseurl() const
	{
		return m_list_style_image_baseurl;
	}
	inline void css_properties::set_list_style_image_baseurl(const string& url)
	{
		m_list_style_image_baseurl = url;
	}

	inline const background& css_properties::get_bg() const
	{
		return m_bg;
	}

	inline void css_properties::set_bg(const background& mBg)
	{
		m_bg = mBg;
	}

	inline pixel_t css_properties::get_font_size() const
	{
		return (pixel_t) m_font_size.val();
	}

	inline void css_properties::set_font_size(pixel_t mFontSize)
	{
		m_font_size = (float) mFontSize;
	}

	inline uint_ptr css_properties::get_font() const
	{
		return m_font;
	}

	inline void css_properties::set_font(uint_ptr mFont)
	{
		m_font = mFont;
	}

	inline const font_metrics& css_properties::get_font_metrics() const
	{
		return m_font_metrics;
	}

	inline void css_properties::set_font_metrics(const font_metrics& mFontMetrics)
	{
		m_font_metrics = mFontMetrics;
	}

	inline text_transform css_properties::get_text_transform() const
	{
		return m_text_transform;
	}

	inline void css_properties::set_text_transform(text_transform mTextTransform)
	{
		m_text_transform = mTextTransform;
	}

	inline web_color css_properties::get_color() const
	{
		return m_color;
	}
	inline void css_properties::set_color(web_color color)
	{
		m_color = color;
	}

	inline const string& css_properties::get_cursor() const
	{
		return m_cursor;
	}
	inline void css_properties::set_cursor(const string& cursor)
	{
		m_cursor = cursor;
	}

	inline const string& css_properties::get_content() const
	{
		return m_content;
	}
	inline void css_properties::set_content(const string& content)
	{
		m_content = content;
	}

	inline border_collapse css_properties::get_border_collapse() const
	{
		return m_border_collapse;
	}

	inline void css_properties::set_border_collapse(border_collapse mBorderCollapse)
	{
		m_border_collapse = mBorderCollapse;
	}

	inline const css_length& css_properties::get_border_spacing_x() const
	{
		return m_css_border_spacing_x;
	}

	inline void css_properties::set_border_spacing_x(const css_length& mBorderSpacingX)
	{
		m_css_border_spacing_x = mBorderSpacingX;
	}

	inline const css_length& css_properties::get_border_spacing_y() const
	{
		return m_css_border_spacing_y;
	}

	inline void css_properties::set_border_spacing_y(const css_length& mBorderSpacingY)
	{
		m_css_border_spacing_y = mBorderSpacingY;
	}

	inline float css_properties::get_flex_grow() const
	{
		return m_flex_grow;
	}

	inline float css_properties::get_flex_shrink() const
	{
		return m_flex_shrink;
	}

	inline const css_length& css_properties::get_flex_basis() const
	{
		return m_flex_basis;
	}

	inline flex_direction css_properties::get_flex_direction() const
	{
		return m_flex_direction;
	}

	inline flex_wrap css_properties::get_flex_wrap() const
	{
		return m_flex_wrap;
	}

	inline flex_justify_content css_properties::get_flex_justify_content() const
	{
		return m_flex_justify_content;
	}

	inline flex_align_items css_properties::get_flex_align_items() const
	{
		return m_flex_align_items;
	}

	inline flex_align_items css_properties::get_flex_align_self() const
	{
		return m_flex_align_self;
	}

	inline flex_align_content css_properties::get_flex_align_content() const
	{
		return m_flex_align_content;
	}

	inline caption_side css_properties::get_caption_side() const
	{
		return m_caption_side;
	}
	inline void css_properties::set_caption_side(caption_side side)
	{
		m_caption_side = side;
	}

	inline int css_properties::get_order() const
	{
		return m_order;
	}

	inline void css_properties::set_order(int order)
	{
		m_order = order;
	}

	inline int css_properties::get_text_decoration_line() const
	{
		return m_text_decoration_line;
	}

	inline text_decoration_style css_properties::get_text_decoration_style() const
	{
		return m_text_decoration_style;
	}

	inline const css_length& css_properties::get_text_decoration_thickness() const
	{
		return m_text_decoration_thickness;
	}

	inline const web_color& css_properties::get_text_decoration_color() const
	{
		return m_text_decoration_color;
	}

	inline string css_properties::get_text_emphasis_style() const
	{
		return m_text_emphasis_style;
	}

	inline web_color css_properties::get_text_emphasis_color() const
	{
		return m_text_emphasis_color;
	}

	inline int css_properties::get_text_emphasis_position() const
	{
		return m_text_emphasis_position;
	}
} // namespace litehtml

#endif // LITEHTML_CSS_PROPERTIES_H
