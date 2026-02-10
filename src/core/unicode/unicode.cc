#include <sourcemeta/core/unicode.h>

#include <sstream> // std::ostringstream

namespace sourcemeta::core {

auto codepoint_to_utf8(const char32_t codepoint, std::ostream &output) -> void {
  if (codepoint < 0x80) {
    output.put(static_cast<char>(codepoint));
  } else if (codepoint < 0x800) {
    output.put(static_cast<char>(0xC0 | (codepoint >> 6)));
    output.put(static_cast<char>(0x80 | (codepoint & 0x3F)));
  } else if (codepoint < 0x10000) {
    output.put(static_cast<char>(0xE0 | (codepoint >> 12)));
    output.put(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
    output.put(static_cast<char>(0x80 | (codepoint & 0x3F)));
  } else {
    output.put(static_cast<char>(0xF0 | (codepoint >> 18)));
    output.put(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
    output.put(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
    output.put(static_cast<char>(0x80 | (codepoint & 0x3F)));
  }
}

auto codepoint_to_utf8(const char32_t codepoint) -> std::string {
  std::ostringstream output;
  codepoint_to_utf8(codepoint, output);
  return output.str();
}

} // namespace sourcemeta::core
