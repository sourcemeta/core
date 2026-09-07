#include <sourcemeta/core/http.h>

#include "helpers.h"

#include <optional>    // std::optional, std::nullopt
#include <string_view> // std::string_view

namespace sourcemeta::core {

auto http_parse_media_type(const std::string_view media_type) noexcept
    -> std::optional<HTTPMediaType> {
  const auto [value, parameters] = http_split_entry(media_type);
  const auto bare{http_trim_leading_ows(value)};
  if (!http_is_media_type(bare)) {
    return std::nullopt;
  }

  const auto slash{bare.find('/')};
  const auto subtype{http_subview(bare, slash + 1, bare.size() - slash - 1)};

  // RFC 6838 §4.2: "Characters after last plus always specify a structured
  // syntax suffix", so a trailing plus with nothing after it names none
  const auto plus{subtype.rfind('+')};
  const auto suffix{
      (plus == std::string_view::npos || plus + 1 == subtype.size())
          ? std::string_view{}
          : http_subview(subtype, plus, subtype.size() - plus)};

  return HTTPMediaType{.type = http_subview(bare, 0, slash),
                       .subtype = subtype,
                       .suffix = suffix,
                       .parameters = parameters};
}

} // namespace sourcemeta::core
