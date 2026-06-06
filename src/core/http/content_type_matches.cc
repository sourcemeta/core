#include <sourcemeta/core/http.h>

#include "helpers.h"

#include <cstddef>     // std::size_t
#include <string_view> // std::string_view

namespace sourcemeta::core {

auto http_content_type_matches(const std::string_view content_type_header,
                               const std::string_view media_type) noexcept
    -> bool {
  std::string_view bare{content_type_header};
  std::size_t semicolon{0};
  while (semicolon < bare.size() && bare[semicolon] != ';') {
    ++semicolon;
  }
  bare = http_subview(bare, 0, semicolon);
  bare = http_trim_leading_ows(http_trim_trailing_ows(bare));
  return http_iequals_ascii(bare, media_type);
}

} // namespace sourcemeta::core
