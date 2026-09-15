#include <sourcemeta/core/markdown.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/unicode.h>

#include "blocks.h"
#include "characters.h"
#include "document.h"
#include "inlines.h"
#include "postprocess.h"
#include "render.h"

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

auto replace_invalid_characters(const std::string_view input,
                                std::string &output) -> void {
  output = sourcemeta::core::to_valid_utf8(input);
  if (output.find('\0') != std::string::npos) {
    output = sourcemeta::core::replace(output, std::string_view{"\0", 1},
                                       "\xEF\xBF\xBD");
  }
}

} // namespace

namespace sourcemeta::core {

auto markdown_to_html(const std::string_view input, const bool safe)
    -> std::string {
  thread_local MarkdownConverter converter;
  converter.document.clear();
  auto source{input};
  // GFM section 2.3 requires replacing the NUL character, and byte sequences
  // that are not well-formed UTF-8 are replaced too
  if (input.find('\0') != std::string_view::npos ||
      !sourcemeta::core::is_valid_utf8(input)) {
    replace_invalid_characters(input, converter.repaired);
    source = converter.repaired;
  }

  converter.blocks.parse(source, input.size());
  converter.postprocessor.parse_inlines(converter.inlines);
  converter.postprocessor.process_footnotes();
  return converter.renderer.render(!safe,
                                   source.size() + (source.size() / 2) + 64);
}

} // namespace sourcemeta::core
