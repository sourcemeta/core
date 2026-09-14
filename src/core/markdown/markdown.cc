#include <sourcemeta/core/markdown.h>
#include <sourcemeta/core/unicode.h>

#include "blocks.h"
#include "characters.h"
#include "document.h"
#include "inlines.h"
#include "postprocess.h"
#include "render.h"

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint64_t
#include <cstring>     // std::memcpy
#include <string>      // std::string
#include <string_view> // std::string_view

namespace {

// The state of every thread is kept across calls, so that rendering many
// small inputs does not allocate the same buffers over and over
struct MarkdownConverter {
  sourcemeta::core::markdown::Document document;
  sourcemeta::core::markdown::BlockParser blocks{document};
  sourcemeta::core::markdown::InlineParser inlines{document};
  sourcemeta::core::markdown::PostProcessor postprocessor{document};
  sourcemeta::core::markdown::HTMLRenderer renderer{document};
  std::string repaired;
};

// Whether the input has NUL characters or byte sequences that are not UTF-8,
// which GFM section 2.3 requires replacing
auto needs_replacement(const std::string_view input) noexcept -> bool {
  constexpr std::uint64_t ONES{0x0101010101010101ULL};
  constexpr std::uint64_t HIGH_BITS{0x8080808080808080ULL};
  const auto size{input.size()};
  std::size_t index{0};
  while (index < size) {
    if (index + 8 <= size) {
      std::uint64_t word{0};
      std::memcpy(&word, input.data() + index, 8);
      if ((word & HIGH_BITS) == 0 && ((word - ONES) & ~word & HIGH_BITS) == 0) {
        index += 8;
        continue;
      }
    }

    const auto byte{static_cast<unsigned char>(input[index])};
    if (byte == 0) {
      return true;
    }

    if (byte < 0x80) {
      ++index;
      continue;
    }

    char32_t codepoint{0};
    const auto length{sourcemeta::core::markdown::decode_utf8(
        input.substr(index), codepoint)};
    if (length == 0) {
      return true;
    }

    index += length;
  }

  return false;
}

auto replace_invalid_characters(const std::string_view input,
                                std::string &output) -> void {
  output = sourcemeta::core::to_valid_utf8(input);
  if (output.find('\0') == std::string::npos) {
    return;
  }

  std::string replaced;
  replaced.reserve(output.size() + 16);
  for (const auto character : output) {
    if (character == '\0') {
      replaced.append("\xEF\xBF\xBD");
    } else {
      replaced.push_back(character);
    }
  }

  output.swap(replaced);
}

} // namespace

namespace sourcemeta::core {

auto markdown_to_html(const std::string_view input, const bool safe)
    -> std::string {
  thread_local MarkdownConverter converter;
  converter.document.clear();
  auto source{input};
  if (needs_replacement(input)) {
    replace_invalid_characters(input, converter.repaired);
    source = converter.repaired;
  }

  converter.blocks.parse(source, input.size());
  converter.postprocessor.parse_inlines(converter.inlines);
  converter.postprocessor.process_footnotes();
  std::string output;
  converter.renderer.render(output, !safe,
                            source.size() + (source.size() / 2) + 64);
  return output;
}

} // namespace sourcemeta::core
