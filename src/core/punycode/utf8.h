#ifndef SOURCEMETA_CORE_PUNYCODE_UTF8_H_
#define SOURCEMETA_CORE_PUNYCODE_UTF8_H_

#include <cstdint>  // std::uint8_t
#include <istream>  // std::istream
#include <optional> // std::optional, std::nullopt
#include <string>   // std::u32string

namespace sourcemeta::core {

inline auto utf8_to_utf32(std::istream &input)
    -> std::optional<std::u32string> {
  std::u32string result;
  std::uint8_t byte{0};

  while (input.read(reinterpret_cast<char *>(&byte), 1)) {
    char32_t code_point{0};
    std::uint8_t continuation_count{0};
    char32_t minimum{0};

    if (byte < 0x80) {
      result.push_back(byte);
      continue;
    } else if ((byte & 0xE0) == 0xC0) {
      code_point = byte & 0x1F;
      continuation_count = 1;
      minimum = 0x80;
    } else if ((byte & 0xF0) == 0xE0) {
      code_point = byte & 0x0F;
      continuation_count = 2;
      minimum = 0x800;
    } else if ((byte & 0xF8) == 0xF0) {
      code_point = byte & 0x07;
      continuation_count = 3;
      minimum = 0x10000;
    } else {
      return std::nullopt;
    }

    for (std::uint8_t index = 0; index < continuation_count; ++index) {
      std::uint8_t continuation{0};
      if (!input.read(reinterpret_cast<char *>(&continuation), 1) ||
          (continuation & 0xC0) != 0x80) {
        return std::nullopt;
      }

      code_point = (code_point << 6) | (continuation & 0x3F);
    }

    if (code_point < minimum || code_point > 0x10FFFF ||
        (code_point >= 0xD800 && code_point <= 0xDFFF)) {
      return std::nullopt;
    }

    result.push_back(code_point);
  }

  return result;
}

} // namespace sourcemeta::core

#endif
