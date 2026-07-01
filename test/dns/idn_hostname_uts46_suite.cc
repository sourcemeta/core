#include <sourcemeta/core/dns.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/numeric.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/unicode.h>

#include <cstddef>     // std::size_t
#include <filesystem>  // std::filesystem
#include <sstream>     // std::ostringstream
#include <string>      // std::string, std::u32string
#include <string_view> // std::string_view
#include <vector>      // std::vector

namespace {

constexpr std::size_t TOTAL_CODEPOINTS{0x110000};

auto hex_to_codepoint(const std::string_view token) -> char32_t {
  return static_cast<char32_t>(
      sourcemeta::core::to_uint32_t(token, 16).value_or(0));
}

// Decode an IdnaTestV2.txt string field: literal UTF-8 with \uXXXX and
// \x{XXXX} escapes for characters that would otherwise display confusingly.
// The literal "" denotes the empty string. Surrogate escapes are preserved as
// codepoints so the caller can re-encode them as (ill-formed) input.
auto decode_field(const std::string_view raw) -> std::u32string {
  std::u32string result;
  if (raw == "\"\"") {
    return result;
  }
  std::size_t position{0};
  while (position < raw.size()) {
    if (raw[position] == '\\' && position + 5 < raw.size() &&
        raw[position + 1] == 'u') {
      result.push_back(hex_to_codepoint(raw.substr(position + 2, 4)));
      position += 6;
    } else if (raw[position] == '\\' && position + 2 < raw.size() &&
               raw[position + 1] == 'x' && raw[position + 2] == '{') {
      const auto end{raw.find('}', position + 3)};
      result.push_back(
          hex_to_codepoint(raw.substr(position + 3, end - (position + 3))));
      position = end + 1;
    } else {
      const auto decoded{sourcemeta::core::utf8_decode(raw, position)};
      if (!decoded.has_value()) {
        position += 1;
        continue;
      }
      result.push_back(decoded->first);
      position += decoded->second;
    }
  }
  return result;
}

// The set of codepoints tagged NV8 or XV8 in IdnaMappingTable.txt. Per the
// IdnaTestV2.txt conformance notes, an IDNA 2008 implementation treats any line
// whose toUnicode field contains such a codepoint as invalid, because those are
// valid for UTS #46 mapping but excluded from IDNA 2008.
auto build_never_valid_set(const std::filesystem::path &path)
    -> std::vector<bool> {
  std::vector<bool> result(TOTAL_CODEPOINTS, false);
  const auto contents{sourcemeta::core::read_file_to_string(path)};
  sourcemeta::core::split(
      contents, '\n', [&result](const std::string_view raw_line) {
        const auto line{sourcemeta::core::take_until(raw_line, '#')};
        const auto columns{sourcemeta::core::split(line, ';')};
        if (columns.size() < 4) {
          return;
        }
        const auto tag{sourcemeta::core::trim(columns[3])};
        if (tag != "NV8" && tag != "XV8") {
          return;
        }
        const auto range{sourcemeta::core::trim(columns[0])};
        const auto bounds{
            sourcemeta::core::split_once(range, std::string_view{".."})};
        const auto first{
            hex_to_codepoint(bounds.has_value() ? bounds->first : range)};
        const auto last{bounds.has_value() ? hex_to_codepoint(bounds->second)
                                           : first};
        for (char32_t codepoint{first}; codepoint <= last; codepoint += 1) {
          result[codepoint] = true;
        }
      });
  return result;
}

auto contains_never_valid(const std::u32string_view codepoints,
                          const std::vector<bool> &never_valid) -> bool {
  for (const auto codepoint : codepoints) {
    if (codepoint < TOTAL_CODEPOINTS && never_valid[codepoint]) {
      return true;
    }
  }
  return false;
}

// Whether a toAsciiN status field denotes "no errors": blank inherits the
// toUnicode status, and an explicit or inherited empty set means valid. No
// status codes are ignored: the predicate implements every check the corpus
// exercises for the IDNA 2008 profile.
auto is_success_status(const std::string_view ascii_status,
                       const std::string_view unicode_status) -> bool {
  const auto effective{ascii_status.empty() ? unicode_status : ascii_status};
  return effective.empty() || effective == "[]";
}

} // namespace

auto main(int argc, char **argv) -> int {
  const auto never_valid{
      build_never_valid_set(std::filesystem::path{IDNAMAPPINGTABLE_PATH})};
  const auto contents{sourcemeta::core::read_file_to_string(
      std::filesystem::path{IDNATESTV2_PATH})};

  std::size_t line_number{0};
  sourcemeta::core::split(contents, '\n', [&](const std::string_view raw_line) {
    line_number += 1;
    const auto line{
        sourcemeta::core::trim(sourcemeta::core::take_until(raw_line, '#'))};
    if (line.empty()) {
      return;
    }

    const auto columns{sourcemeta::core::split(line, ';')};
    if (columns.size() < 5) {
      return;
    }

    const auto source{decode_field(sourcemeta::core::trim(columns[0]))};
    const auto unicode_column{sourcemeta::core::trim(columns[1])};
    const auto unicode{unicode_column.empty() ? source
                                              : decode_field(unicode_column)};

    // A line whose toUnicode field contains an NV8/XV8 codepoint is valid
    // only under the more permissive pure-UTS #46 profile. Every such
    // codepoint is DISALLOWED under IDNA 2008, so this IDNA 2008 predicate
    // must reject the line. Assert that directly, per the IdnaTestV2.txt
    // conformance note for IDNA 2008 implementations.
    const bool expected{
        contains_never_valid(unicode, never_valid)
            ? false
            : is_success_status(sourcemeta::core::trim(columns[4]),
                                sourcemeta::core::trim(columns[2]))};
    // Lenient encoding so a source with a lone surrogate becomes ill-formed
    // UTF-8, which the predicate rejects (matching the corpus)
    const auto source_utf8{sourcemeta::core::utf32_to_utf8_lenient(source)};

    std::ostringstream name;
    name << "line_" << line_number;
    sourcemeta::core::test_register(
        name.str(), [source_utf8, expected]() -> void {
          EXPECT_EQ(sourcemeta::core::is_idn_hostname_uts46(source_utf8),
                    expected);
        });
  });

  return sourcemeta::core::test_run(argc, argv);
}
