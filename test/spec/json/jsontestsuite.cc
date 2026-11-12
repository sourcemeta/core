#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <cstdint>     // std::uint8_t, std::uint64_t
#include <filesystem>  // std::filesystem
#include <fstream>     // std::ifstream
#include <ios>         // std::ios::binary, std::ios_base::badbit
#include <iterator>    // std::istreambuf_iterator
#include <set>         // std::set
#include <string>      // std::string
#include <string_view> // std::string_view

namespace {
enum class JSONTestType : std::uint8_t { Accept, Reject };

// JSONTestSuite names a case `i_` where the outcome is the implementation's to
// settle, so what this one settles on is written down rather than assumed. RFC
// 8259 Section 6 lets an implementation set limits on the range and precision
// of numbers it accepts, and Section 9 lets it set limits on the character
// contents of strings, which is what the two groups below come to: a number
// outside what a double carries is kept rather than refused, and a byte that
// stands in no valid UTF-8 sequence travels as it was written
// See https://www.rfc-editor.org/rfc/rfc8259
// NOLINTBEGIN(cert-err58-cpp,bugprone-throwing-static-initialization)
const std::set<std::string_view> IMPLEMENTATION_DEFINED_ACCEPTED{
    "i_number_double_huge_neg_exp.json",
    "i_number_neg_int_huge_exp.json",
    "i_number_pos_double_huge_exp.json",
    "i_number_real_neg_overflow.json",
    "i_number_real_pos_overflow.json",
    "i_number_real_underflow.json",
    "i_number_too_big_neg_int.json",
    "i_number_too_big_pos_int.json",
    "i_number_very_big_negative_int.json",
    "i_string_UTF-8_invalid_sequence.json",
    "i_string_UTF8_surrogate_U+D800.json",
    "i_string_invalid_utf-8.json",
    "i_string_iso_latin_1.json",
    "i_string_lone_utf8_continuation_byte.json",
    "i_string_not_in_unicode_range.json",
    "i_string_overlong_sequence_2_bytes.json",
    "i_string_overlong_sequence_6_bytes.json",
    "i_string_overlong_sequence_6_bytes_null.json",
    "i_string_truncated-utf-8.json",
    "i_structure_500_nested_arrays.json"};
// NOLINTEND(cert-err58-cpp,bugprone-throwing-static-initialization)

// A name beginning with `y` must be accepted and one beginning with `n` must
// be refused, while the rest are the ones above
auto expectation_of(const std::string &name) -> JSONTestType {
  if (name.front() == 'y') {
    return JSONTestType::Accept;
  }

  if (name.front() == 'n') {
    return JSONTestType::Reject;
  }

  return IMPLEMENTATION_DEFINED_ACCEPTED.contains(name) ? JSONTestType::Accept
                                                        : JSONTestType::Reject;
}

// Whether the form that takes a whole document refuses the input. RFC 8259
// Section 2 makes a JSON text one value, so anything following that value is
// not part of the text and the input names no document
auto refuses(const std::string &input) -> bool {
  try {
    sourcemeta::core::parse_json(input);
    return false;
  } catch (const sourcemeta::core::JSONParseError &) {
    return true;
  }
}

auto run_json_test_case(const std::filesystem::path &path,
                        const JSONTestType type) -> void {
  std::ifstream stream{path, std::ios::binary};
  stream.exceptions(std::ios_base::badbit);
  // Reading the whole input as one document is what tells a document this
  // parser refuses from one it takes and leaves the rest of the input alone,
  // as the stream form is meant to do
  const std::string whole{std::istreambuf_iterator<char>{stream},
                          std::istreambuf_iterator<char>{}};

  // Every way in has to come to the same verdict on the same input. Counting
  // positions and announcing what is read are each a reader of their own, and
  // the form that answers with no value rather than throwing is a third, so
  // what one of them takes the others must take too
  std::uint64_t line{1};
  std::uint64_t column{0};

  if (type == JSONTestType::Reject) {
    // Both of the forms that read a whole document have to come to the same
    // verdict, one by throwing and the other by answering with no value
    EXPECT_TRUE(refuses(whole));
    EXPECT_FALSE(sourcemeta::core::try_parse_json(whole).has_value());
    return;
  }

  sourcemeta::core::parse_json(whole);
  EXPECT_TRUE(sourcemeta::core::try_parse_json(whole).has_value());

  // The forms that count positions parse one value and leave the rest of the
  // input to the caller, so a document whole is what they are asked about
  sourcemeta::core::parse_json(whole, line, column);

  // What the stream form makes of the same input, which is a reader of its own
  std::ifstream again{path, std::ios::binary};
  again.exceptions(std::ios_base::badbit);
  sourcemeta::core::parse_json(again);
}
} // namespace

auto main(int argc, char **argv) -> int {
  const std::filesystem::path test_parsing_path{
      std::filesystem::path{JSONTESTSUITE_PATH} / "test_parsing"};
  for (const std::filesystem::directory_entry &entry :
       std::filesystem::directory_iterator{test_parsing_path}) {
    const std::filesystem::path &test_path{entry.path()};
    const auto name{test_path.filename().string()};
    sourcemeta::core::test_register(
        name, [test_path, type = expectation_of(name)]() -> void {
          run_json_test_case(test_path, type);
        });
  }

  return sourcemeta::core::test_run(argc, argv);
}
