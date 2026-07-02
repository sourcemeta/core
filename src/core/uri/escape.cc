#include <sourcemeta/core/uri.h>

#include "escaping.h"
#include "grammar.h"

#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::core {

auto URI::escape(const std::string_view input, std::string &output) -> void {
  output.reserve(output.size() + input.size());
  for (const auto character : input) {
    if (uri_is_unreserved(character)) {
      output += character;
      continue;
    }

    const auto byte{static_cast<unsigned char>(character)};
    const auto high{(byte >> 4) & 0x0F};
    const auto low{byte & 0x0F};
    output += URI_PERCENT;
    output += static_cast<char>(high < 10 ? '0' + high : 'A' + high - 10);
    output += static_cast<char>(low < 10 ? '0' + low : 'A' + low - 10);
  }
}

auto URI::escape(const std::string_view input) -> std::string {
  std::string result;
  URI::escape(input, result);
  return result;
}

auto URI::unescape(const std::string_view input) -> std::string {
  std::string result{input};
  uri_unescape_all_inplace(result);
  return result;
}

} // namespace sourcemeta::core
