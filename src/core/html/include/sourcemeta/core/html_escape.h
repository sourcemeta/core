#ifndef SOURCEMETA_CORE_HTML_ESCAPE_H_
#define SOURCEMETA_CORE_HTML_ESCAPE_H_

#ifndef SOURCEMETA_CORE_HTML_EXPORT
#include <sourcemeta/core/html_export.h>
#endif

#include <sourcemeta/core/html_buffer.h>
#include <sourcemeta/core/preprocessor.h>

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t, std::uint64_t
#include <cstring>     // std::memcpy
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::core {

/// @ingroup html
/// HTML character escaping implementation per HTML Living Standard.
/// See: https://html.spec.whatwg.org/multipage/parsing.html#escapingString
///
/// This function escapes the five HTML special characters in-place, the
/// ampersand, less-than sign, greater-than sign, double quote, and apostrophe,
/// along with the no-break space, each becoming its corresponding HTML entity.
///
/// For example:
///
/// ```cpp
/// #include <sourcemeta/core/html.h>
/// #include <cassert>
///
/// std::string text{"1 < 2 & 3 > 0 'x' \"y\""};
/// sourcemeta::core::html_escape(text);
/// assert(text == "1 &lt; 2 &amp; 3 &gt; 0 &#39;x&#39; &quot;y&quot;");
/// ```
SOURCEMETA_CORE_HTML_EXPORT
auto html_escape(std::string &text) -> void;

#ifndef DOXYGEN
// Whether escaping may replace a byte, which covers the quotation mark, the
// ampersand, the apostrophe, the angle brackets, and the first byte of the
// UTF-8 encoding of the no-break space
inline constexpr std::array<std::uint8_t, 256> HTML_ESCAPE_SPECIAL_BYTES{{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x00
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x10
    0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, // 0x20
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, // 0x30
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x40
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x50
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x60
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x70
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x80
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x90
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xA0
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xB0
    0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xC0
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xD0
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xE0
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // 0xF0
}};

struct HTMLEscapeReplacement {
  std::string_view text;
  std::size_t consumed;
};

// Flag the bytes of a word that equal the given byte. A subtraction borrow can
// also flag the byte right after a genuine match, which is harmless when only
// asking whether there is a match at all
constexpr auto html_escape_match_byte(const std::uint64_t word,
                                      const unsigned char byte) noexcept
    -> std::uint64_t {
  constexpr std::uint64_t LOW_BITS{0x0101010101010101ULL};
  constexpr std::uint64_t HIGH_BITS{0x8080808080808080ULL};
  const auto difference{word ^ (LOW_BITS * byte)};
  return (difference - LOW_BITS) & ~difference & HIGH_BITS;
}

// Whether a word may have a byte to replace. Setting the lowest bit of every
// byte makes the ampersand match the apostrophe, and setting the second lowest
// bit makes the less-than sign match the greater-than sign, so four
// comparisons cover the six bytes
constexpr auto html_escape_may_replace(const std::uint64_t word) noexcept
    -> bool {
  constexpr std::uint64_t LOW_BITS{0x0101010101010101ULL};
  return (html_escape_match_byte(word | LOW_BITS, '\'') |
          html_escape_match_byte(word | (LOW_BITS * 2U), '>') |
          html_escape_match_byte(word, '"') |
          html_escape_match_byte(word, 0xC2)) != 0;
}

constexpr auto html_escape_replacement(const std::string_view input,
                                       const std::size_t position) noexcept
    -> HTMLEscapeReplacement {
  switch (input[position]) {
    case '&':
      return {.text = "&amp;", .consumed = 1};
    case '<':
      return {.text = "&lt;", .consumed = 1};
    case '>':
      return {.text = "&gt;", .consumed = 1};
    case '"':
      return {.text = "&quot;", .consumed = 1};
    case '\'':
      return {.text = "&#39;", .consumed = 1};
    default:
      // The no-break space is replaced by its named entity (HTML Living
      // Standard "escaping a string" step 2)
      if (static_cast<unsigned char>(input[position]) == 0xC2 &&
          position + 1 < input.size() &&
          static_cast<unsigned char>(input[position + 1]) == 0xA0) {
        return {.text = "&nbsp;", .consumed = 2};
      }

      return {.text = {}, .consumed = 0};
  }
}

// The number of leading bytes of the input that escaping leaves as they are,
// looking at eight bytes at a time, where the first byte of a two-byte UTF-8
// sequence that is not the no-break space also ends the prefix
SOURCEMETA_FORCEINLINE inline auto
html_escape_clean_prefix(const std::string_view input) noexcept -> std::size_t {
  const auto size{input.size()};
  std::size_t position{0};
  while (position + 8 <= size) {
    std::uint64_t word{0};
    std::memcpy(&word, input.data() + position, 8);
    if (html_escape_may_replace(word)) {
      break;
    }

    position += 8;
  }

  while (
      position < size &&
      HTML_ESCAPE_SPECIAL_BYTES[static_cast<unsigned char>(input[position])] ==
          0) {
    position += 1;
  }

  return position;
}

// Append the input in runs, looking at eight bytes at a time for anything to
// replace, as most text has nothing to escape, and going byte by byte only
// through the words that may have something
template <typename Output>
inline auto html_escape_append_to(Output &output, const std::string_view input)
    -> void {
  const auto size{input.size()};
  std::size_t run_start{0};
  std::size_t position{0};
  while (position < size) {
    auto end{size};
    if (position + 8 <= size) {
      std::uint64_t word{0};
      std::memcpy(&word, input.data() + position, 8);
      if (!html_escape_may_replace(word)) {
        position += 8;
        continue;
      }

      end = position + 8;
    }

    while (position < end) {
      if (HTML_ESCAPE_SPECIAL_BYTES[static_cast<unsigned char>(
              input[position])] == 0) {
        position += 1;
        continue;
      }

      const auto replacement{html_escape_replacement(input, position)};
      if (replacement.consumed == 0) {
        position += 1;
        continue;
      }

      output.append(input.substr(run_start, position - run_start));
      output.append(replacement.text);
      position += replacement.consumed;
      run_start = position;
    }
  }

  output.append(input.substr(run_start));
}
#endif

/// @ingroup html
/// Append the HTML-escaped form of `input` directly to `output`,
/// without allocating a temporary string. The input must not reference the
/// output, since appending to the output may relocate its storage.
SOURCEMETA_FORCEINLINE inline auto html_escape_append(std::string &output,
                                                      std::string_view input)
    -> void {
  const auto prefix{html_escape_clean_prefix(input)};
  output.append(input.substr(0, prefix));
  if (prefix < input.size()) {
    html_escape_append_to(output, input.substr(prefix));
  }
}

/// @ingroup html
/// Append the HTML-escaped form of `input` directly to a buffer. The input must
/// not reference the buffer, since appending to it may relocate its storage.
SOURCEMETA_FORCEINLINE inline auto html_escape_append(HTMLBuffer &output,
                                                      std::string_view input)
    -> void {
  const auto prefix{html_escape_clean_prefix(input)};
  output.append(input.substr(0, prefix));
  if (prefix < input.size()) {
    html_escape_append_to(output, input.substr(prefix));
  }
}

} // namespace sourcemeta::core

#endif
