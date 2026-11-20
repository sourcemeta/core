#include <sourcemeta/core/http_request.h>
#include <sourcemeta/core/http_syntax.h>

#include <algorithm>   // std::ranges::all_of
#include <cstddef>     // std::size_t
#include <string_view> // std::string_view

namespace sourcemeta::core {

auto http_parse_request_head(const std::string_view input) -> HTTPRequestHead {
  constexpr HTTPRequestHeadStatus MALFORMED{HTTPRequestHeadStatus::Malformed};
  HTTPRequestHead result;
  std::size_t offset{0};
  while (true) {
    const auto end{input.find_first_of("\r\n", offset)};
    if (end == std::string_view::npos) {
      return {};
    }

    if (input[end] != '\r') {
      return {.status = MALFORMED};
    }

    if (end + 1 == input.size()) {
      return {};
    }

    if (input[end + 1] != '\n') {
      return {.status = MALFORMED};
    }

    const auto line{input.substr(offset, end - offset)};
    if (offset == 0) {
      const auto method_end{line.find(' ')};
      if (method_end == std::string_view::npos) {
        return {.status = MALFORMED};
      }

      const auto target_start{method_end + 1};
      const auto target_end{line.find(' ', target_start)};
      if (target_end == std::string_view::npos ||
          line.substr(target_end + 1) != "HTTP/1.1") {
        return {.status = MALFORMED};
      }

      result.method = line.substr(0, method_end);
      result.target = line.substr(target_start, target_end - target_start);
      if (!http_is_token(result.method) || result.target.empty() ||
          !std::ranges::all_of(result.target, [](const char byte) {
            return byte >= '!' && byte <= '~';
          })) {
        return {.status = MALFORMED};
      }
    } else if (line.empty()) {
      result.status = HTTPRequestHeadStatus::Complete;
      result.consumed = end + 2;
      return result;
    } else {
      const auto colon{line.find(':')};
      if (colon == std::string_view::npos ||
          !http_is_token(line.substr(0, colon))) {
        return {.status = MALFORMED};
      }

      const auto value{line.substr(colon + 1)};
      if (!std::ranges::all_of(value, http_is_field_octet)) {
        return {.status = MALFORMED};
      }

      result.fields.emplace_back(
          line.substr(0, colon),
          http_trim_trailing_ows(http_trim_leading_ows(value)));
    }

    offset = end + 2;
  }
}

} // namespace sourcemeta::core
