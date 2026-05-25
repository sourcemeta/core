#ifndef SOURCEMETA_CORE_UNICODE_H_
#define SOURCEMETA_CORE_UNICODE_H_

#ifndef SOURCEMETA_CORE_UNICODE_EXPORT
#include <sourcemeta/core/unicode_export.h>
#endif

#include <cstddef>     // std::size_t
#include <istream>     // std::istream
#include <optional>    // std::optional
#include <ostream>     // std::ostream
#include <string>      // std::string, std::u32string
#include <string_view> // std::string_view

/// @defgroup unicode Unicode
/// @brief Unicode encoding utilities.
///
/// This functionality is included as follows:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// ```

namespace sourcemeta::core {

/// @ingroup unicode
/// Encode a single Unicode codepoint as a UTF-8 string. For example:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::codepoint_to_utf8(0x41) == "A");
/// ```
SOURCEMETA_CORE_UNICODE_EXPORT
auto codepoint_to_utf8(const char32_t codepoint) -> std::string;

/// @ingroup unicode
/// Encode a single Unicode codepoint as UTF-8 into an output stream.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// #include <sstream>
/// #include <cassert>
///
/// std::ostringstream output;
/// sourcemeta::core::codepoint_to_utf8(0x41, output);
/// assert(output.str() == "A");
/// ```
SOURCEMETA_CORE_UNICODE_EXPORT
auto codepoint_to_utf8(const char32_t codepoint, std::ostream &output) -> void;

/// @ingroup unicode
/// Encode a single Unicode codepoint as UTF-8, appending to an existing string.
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// #include <cassert>
///
/// std::string output;
/// sourcemeta::core::codepoint_to_utf8(0x41, output);
/// assert(output == "A");
/// ```
SOURCEMETA_CORE_UNICODE_EXPORT
auto codepoint_to_utf8(const char32_t codepoint, std::string &output) -> void;

/// @ingroup unicode
/// Decode a UTF-8 byte stream into a sequence of Unicode codepoints (UTF-32).
/// Returns std::nullopt if the input contains invalid UTF-8. For example:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// #include <sstream>
/// #include <cassert>
///
/// std::istringstream input{"A"};
/// const auto result{sourcemeta::core::utf8_to_utf32(input)};
/// assert(result.has_value());
/// assert(result.value() == std::u32string{0x41});
/// ```
SOURCEMETA_CORE_UNICODE_EXPORT
auto utf8_to_utf32(std::istream &input) -> std::optional<std::u32string>;

/// @ingroup unicode
/// Decode a UTF-8 string into a sequence of Unicode codepoints (UTF-32).
/// Returns std::nullopt if the input contains invalid UTF-8. For example:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// #include <cassert>
///
/// const auto result{sourcemeta::core::utf8_to_utf32("A")};
/// assert(result.has_value());
/// assert(result.value() == std::u32string{0x41});
/// ```
SOURCEMETA_CORE_UNICODE_EXPORT
auto utf8_to_utf32(const std::string_view input)
    -> std::optional<std::u32string>;

/// @ingroup unicode
/// Determine the byte length of the valid UTF-8 codepoint starting at the
/// given position within the input. Returns 1 for an ASCII byte, 2/3/4 for a
/// valid multi-byte UTF-8 sequence (RFC 6532 Section 3.1, excluding overlong
/// encodings, surrogates, and code points above U+10FFFF), or 0 if the bytes
/// at that position do not start a valid UTF-8 codepoint. For example:
///
/// ```cpp
/// #include <sourcemeta/core/unicode.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::utf8_codepoint_length("A", 0) == 1);
/// assert(sourcemeta::core::utf8_codepoint_length("\xce\xb1", 0) == 2);
/// assert(sourcemeta::core::utf8_codepoint_length("\xe4\xb8\xad", 0) == 3);
/// assert(sourcemeta::core::utf8_codepoint_length("\xf0\x9f\x98\x80", 0) == 4);
/// assert(sourcemeta::core::utf8_codepoint_length("\xed\xa0\x80", 0) == 0);
/// ```
inline constexpr auto
utf8_codepoint_length(const std::string_view input,
                      const std::string_view::size_type position)
    -> std::size_t {
  if (position >= input.size()) {
    return 0;
  }
  const auto byte_0{static_cast<unsigned char>(input[position])};

  // ASCII: 1-byte UTF-8 sequence
  if (byte_0 < 0x80) {
    return 1;
  }

  // UTF8-2 = %xC2-DF UTF8-tail
  if (byte_0 >= 0xC2 && byte_0 <= 0xDF) {
    if (position + 1 >= input.size()) {
      return 0;
    }
    const auto byte_1{static_cast<unsigned char>(input[position + 1])};
    return (byte_1 >= 0x80 && byte_1 <= 0xBF) ? 2 : 0;
  }

  // UTF8-3
  if (byte_0 >= 0xE0 && byte_0 <= 0xEF) {
    if (position + 2 >= input.size()) {
      return 0;
    }
    const auto byte_1{static_cast<unsigned char>(input[position + 1])};
    const auto byte_2{static_cast<unsigned char>(input[position + 2])};
    bool byte_1_ok{false};
    if (byte_0 == 0xE0) {
      byte_1_ok = byte_1 >= 0xA0 && byte_1 <= 0xBF;
    } else if (byte_0 == 0xED) {
      byte_1_ok = byte_1 >= 0x80 && byte_1 <= 0x9F;
    } else {
      // 0xE1-0xEC and 0xEE-0xEF share the same continuation range
      byte_1_ok = byte_1 >= 0x80 && byte_1 <= 0xBF;
    }
    return (byte_1_ok && byte_2 >= 0x80 && byte_2 <= 0xBF) ? 3 : 0;
  }

  // UTF8-4
  if (byte_0 >= 0xF0 && byte_0 <= 0xF4) {
    if (position + 3 >= input.size()) {
      return 0;
    }
    const auto byte_1{static_cast<unsigned char>(input[position + 1])};
    const auto byte_2{static_cast<unsigned char>(input[position + 2])};
    const auto byte_3{static_cast<unsigned char>(input[position + 3])};
    bool byte_1_ok{false};
    if (byte_0 == 0xF0) {
      byte_1_ok = byte_1 >= 0x90 && byte_1 <= 0xBF;
    } else if (byte_0 >= 0xF1 && byte_0 <= 0xF3) {
      byte_1_ok = byte_1 >= 0x80 && byte_1 <= 0xBF;
    } else {
      byte_1_ok = byte_1 >= 0x80 && byte_1 <= 0x8F;
    }
    return (byte_1_ok && byte_2 >= 0x80 && byte_2 <= 0xBF && byte_3 >= 0x80 &&
            byte_3 <= 0xBF)
               ? 4
               : 0;
  }

  return 0;
}

} // namespace sourcemeta::core

#endif
