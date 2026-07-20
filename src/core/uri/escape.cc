#include <sourcemeta/core/text.h>
#include <sourcemeta/core/uri.h>

#include "escaping.h"
#include "grammar.h"

#include <cstddef>     // std::size_t
#include <cstdint>     // std::int8_t
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::core {

auto URI::escape(const std::string_view input, const bool maybe_encoded)
    -> std::string {
  std::string result;
  URI::escape(input, result, maybe_encoded);
  return result;
}

auto URI::unescape(const std::string_view input) -> std::string {
  std::string result{input};
  uri_unescape_all_inplace(result);
  return result;
}

auto URI::unescape_form(const std::string_view input, std::string &output)
    -> bool {
  const auto base{output.size()};
  output.reserve(base + input.size());
  for (std::size_t position = 0; position < input.size();) {
    const auto character{input[position]};
    if (character == '+') {
      output += ' ';
      position += 1;
    } else if (character == URI_PERCENT) {
      const auto high{position + 2 < input.size()
                          ? hex_digit_value(input[position + 1])
                          : static_cast<std::int8_t>(-1)};
      const auto low{high < 0 ? static_cast<std::int8_t>(-1)
                              : hex_digit_value(input[position + 2])};
      if (low < 0) {
        output.resize(base);
        return false;
      }

      output += static_cast<char>((high << 4) | low);
      position += 3;
    } else {
      output += character;
      position += 1;
    }
  }

  return true;
}

} // namespace sourcemeta::core
