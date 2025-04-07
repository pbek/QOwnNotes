#ifndef LH_EL_SPACE_H
#define LH_EL_SPACE_H

#include "el_text.h"
#include "html_tag.h"

namespace litehtml {
class el_space : public el_text {
 public:
  el_space(const tchar_t* text, const std::shared_ptr<litehtml::document>& doc);

  bool is_white_space() const override;
  bool is_break() const override;
};
}  // namespace litehtml

#endif  // LH_EL_SPACE_H
