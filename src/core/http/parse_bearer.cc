#include <sourcemeta/core/http.h>

#include "helpers.h"

#include <string_view> // std::string_view

namespace sourcemeta::core {

auto http_parse_bearer(const std::string_view authorization) noexcept
    -> std::string_view {
  constexpr std::string_view scheme{"bearer"};
  if (authorization.size() <= scheme.size() ||
      authorization[scheme.size()] != ' ') {
    return {};
  }

  if (!http_iequals_ascii(http_subview(authorization, 0, scheme.size()),
                          scheme)) {
    return {};
  }

  const auto token{http_subview(authorization, scheme.size() + 1,
                                authorization.size() - scheme.size() - 1)};
  return http_trim_trailing_ows(http_trim_leading_ows(token));
}

} // namespace sourcemeta::core
