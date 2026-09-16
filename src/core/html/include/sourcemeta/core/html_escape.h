#ifndef SOURCEMETA_CORE_HTML_ESCAPE_H_
#define SOURCEMETA_CORE_HTML_ESCAPE_H_

#ifndef SOURCEMETA_CORE_HTML_EXPORT
#include <sourcemeta/core/html_export.h>
#endif

#include <sourcemeta/core/html_buffer.h>
#include <sourcemeta/core/preprocessor.h>

#include <array>       // std::array
#include <concepts>    // std::same_as
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

/// @ingroup html
/// Append the HTML-escaped form of `input` to a string or to a buffer, without
/// allocating a temporary string. The input must not reference the output,
/// since appending to the output may relocate its storage. For example:
///
/// ```cpp
/// #include <sourcemeta/core/html.h>
/// #include <cassert>
///
/// std::string output{"<p>"};
/// sourcemeta::core::html_escape_append(output, "1 < 2");
/// assert(output == "<p>1 &lt; 2");
/// ```
template <typename Output>
  requires std::same_as<Output, std::string> || std::same_as<Output, HTMLBuffer>
SOURCEMETA_FORCEINLINE inline auto
html_escape_append(Output &output, const std::string_view input) -> void {
  // The bytes that escaping may replace, which are the quotation mark, the
  // ampersand, the apostrophe, the angle brackets, and the first byte of the
  // UTF-8 encoding of the no-break space
  static constexpr std::array<std::uint8_t, 256> SPECIAL_BYTES{{
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
  constexpr std::uint64_t LOW_BITS{0x0101010101010101ULL};
  constexpr std::uint64_t HIGH_BITS{0x8080808080808080ULL};

  // Flag the bytes of a word that equal the given byte. A subtraction borrow
  // can also flag the byte right after a genuine match, which is harmless when
  // only asking whether there is a match at all
  const auto match_byte{
      [](const std::uint64_t word, const unsigned char byte) noexcept {
        const auto difference{word ^ (LOW_BITS * byte)};
        return (difference - LOW_BITS) & ~difference & HIGH_BITS;
      }};

  // Whether a word may have a byte to replace. Setting the lowest bit of every
  // byte makes the ampersand match the apostrophe, and setting the second
  // lowest bit makes the less-than sign match the greater-than sign, so four
  // comparisons cover the six bytes
  const auto may_replace{[&match_byte](const std::uint64_t word) noexcept {
    return (match_byte(word | LOW_BITS, '\'') |
            match_byte(word | (LOW_BITS * 2U), '>') | match_byte(word, '"') |
            match_byte(word, 0xC2)) != 0;
  }};

  const auto size{input.size()};
  std::size_t position{0};
  // Most text has nothing to escape, so the leading bytes that stay as they
  // are get copied in one go, looking at eight bytes at a time
  while (size - position >= 8) {
    std::uint64_t word{0};
    std::memcpy(&word, input.data() + position, 8);
    if (may_replace(word)) {
      break;
    }

    position += 8;
  }

  while (position < size &&
         SPECIAL_BYTES[static_cast<unsigned char>(input[position])] == 0) {
    position += 1;
  }

  output.append(input.substr(0, position));
  if (position == size) {
    return;
  }

  // What is left is appended in runs, going byte by byte only through the
  // words that may have something to replace
  std::size_t run_start{position};
  while (position < size) {
    auto end{size};
    if (size - position >= 8) {
      std::uint64_t word{0};
      std::memcpy(&word, input.data() + position, 8);
      if (!may_replace(word)) {
        position += 8;
        continue;
      }

      end = position + 8;
    }

    while (position < end) {
      if (SPECIAL_BYTES[static_cast<unsigned char>(input[position])] == 0) {
        position += 1;
        continue;
      }

      std::string_view replacement;
      std::size_t consumed{1};
      switch (input[position]) {
        case '&':
          replacement = "&amp;";
          break;
        case '<':
          replacement = "&lt;";
          break;
        case '>':
          replacement = "&gt;";
          break;
        case '"':
          replacement = "&quot;";
          break;
        case '\'':
          replacement = "&#39;";
          break;
        default:
          // The no-break space is replaced by its named entity (HTML Living
          // Standard "escaping a string" step 2)
          if (static_cast<unsigned char>(input[position]) == 0xC2 &&
              position + 1 < size &&
              static_cast<unsigned char>(input[position + 1]) == 0xA0) {
            replacement = "&nbsp;";
            consumed = 2;
          }

          break;
      }

      if (replacement.empty()) {
        position += 1;
        continue;
      }

      output.append(input.substr(run_start, position - run_start));
      output.append(replacement);
      position += consumed;
      run_start = position;
    }
  }

  output.append(input.substr(run_start));
}

} // namespace sourcemeta::core

#endif
