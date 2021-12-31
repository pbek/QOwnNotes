#pragma once
#include <Windows.h>

namespace litehtml
{
	class win32_container : public document_container
	{
	public:
		typedef std::map<std::wstring, litehtml::uint_ptr>	images_map;
	
		protected:
		
		images_map					m_images;
		litehtml::position::vector	m_clips;
		HRGN						m_hClipRgn;

	public:
		win32_container();
		virtual ~win32_container();

		// litehtml::document_container members
		virtual uint_ptr	create_font(const wchar_t* faceName, int size, int weight, font_style italic, unsigned int decoration);
		virtual void		delete_font(uint_ptr hFont);
		virtual int			line_height(uint_ptr hdc, uint_ptr hFont);
		virtual int			get_text_base_line(uint_ptr hdc, uint_ptr hFont);
		virtual int			text_width(uint_ptr hdc, const wchar_t* text, uint_ptr hFont);
		virtual void		draw_text(uint_ptr hdc, const wchar_t* text, uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos);
		virtual void		fill_rect(uint_ptr hdc, const litehtml::position& pos, const litehtml::web_color color, const litehtml::css_border_radius& radius);
		virtual uint_ptr	get_temp_dc();
		virtual void		release_temp_dc(uint_ptr hdc);
		virtual int			pt_to_px(int pt);
		virtual void		draw_list_marker(uint_ptr hdc, const litehtml::list_marker& marker);
		virtual void		load_image(const wchar_t* src, const wchar_t* baseurl, bool redraw_on_ready);
		virtual void		get_image_size(const wchar_t* src, const wchar_t* baseurl, litehtml::size& sz);
		virtual void		draw_image(uint_ptr hdc, const wchar_t* src, const wchar_t* baseurl, const litehtml::position& pos);
		virtual void		draw_background(uint_ptr hdc, 
											const wchar_t* image, 
											const wchar_t* baseurl, 
											const litehtml::position& draw_pos,
											const litehtml::css_position& bg_pos,
											litehtml::background_repeat repeat, 
											litehtml::background_attachment attachment);

		virtual int			get_default_font_size() const;
		virtual	wchar_t		toupper(const wchar_t c);
		virtual	wchar_t		tolower(const wchar_t c);
		virtual void		set_clip(const litehtml::position& pos, bool valid_x, bool valid_y);
		virtual void		del_clip();

	protected:
		void				apply_clip(HDC hdc);
		void				release_clip(HDC hdc);
		void				clear_images();
		virtual void		make_url( LPCWSTR url, LPCWSTR basepath, std::wstring& out ) = 0;
		virtual uint_ptr	get_image(LPCWSTR url) = 0;
		virtual void		free_image(uint_ptr img) = 0;
		virtual void		get_client_rect(litehtml::position& client) = 0;
		virtual void		draw_ellipse(HDC hdc, int x, int y, int width, int height, const web_color& color, int line_width) = 0;
		virtual void		fill_ellipse(HDC hdc, int x, int y, int width, int height, const web_color& color) = 0;
		virtual void		fill_rect(HDC hdc, int x, int y, int width, int height, const web_color& color, const litehtml::css_border_radius& radius) = 0;
		virtual void		get_img_size(litehtml::uint_ptr img, litehtml::size& sz) = 0;
		virtual void		draw_img(HDC hdc, litehtml::uint_ptr img, const litehtml::position& pos) = 0;
		virtual void		draw_img_bg(HDC hdc, litehtml::uint_ptr img, const litehtml::position& draw_pos, const litehtml::position& pos, litehtml::background_repeat repeat, litehtml::background_attachment attachment) = 0;
	};
}