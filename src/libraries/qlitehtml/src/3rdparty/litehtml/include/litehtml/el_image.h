#ifndef LH_EL_IMAGE_H
#define LH_EL_IMAGE_H

#include "html_tag.h"

namespace litehtml
{

	class el_image : public html_tag
	{
		tstring	m_src;
	public:
		el_image(const std::shared_ptr<litehtml::document>& doc);
		virtual ~el_image(void);

		virtual int		line_height() const override;
		virtual bool	is_replaced() const override;
		virtual int		render(int x, int y, int max_width, bool second_pass = false) override;
		virtual void	parse_attributes() override;
		virtual void	parse_styles(bool is_reparse = false) override;
		virtual void	draw(uint_ptr hdc, int x, int y, const position* clip) override;
		virtual void	get_content_size(size& sz, int max_width) override;
	private:
		int calc_max_height(int image_height);
	};
}

#endif  // LH_EL_IMAGE_H
