#include <exception>
#include <gtest/gtest.h>
#include <sstream>
#include <string>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/json_error.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonpointer_error.h>

#define EXPECT_JSON_PARSE_ERROR(input)                                         \
  try {                                                                        \
    std::ostringstream stream;                                                 \
    stream << "\"" << (input) << "\"";                                         \
    sourcemeta::core::parse_json(stream.str());                                \
    FAIL() << "The parse function was expected to throw";                      \
  } catch (const sourcemeta::core::JSONParseError &) {                         \
    SUCCEED();                                                                 \
  } catch (const std::exception &) {                                           \
    FAIL() << "The parse operation threw an unexpected error";                 \
  }

#define EXPECT_POINTER_PARSE_ERROR(input, expected_column)                     \
  try {                                                                        \
    sourcemeta::core::to_pointer(input);                                       \
    FAIL() << "The to_pointer function was expected to throw";                 \
  } catch (const sourcemeta::core::PointerParseError &error) {                 \
    EXPECT_EQ(error.column(), expected_column);                                \
    SUCCEED();                                                                 \
  } catch (const std::exception &) {                                           \
    FAIL() << "The to_pointer function threw an unexpected error";             \
  }

TEST(JSONPointer_parse_error, missing_initial_slash) {
  const std::string input{"1"};
  EXPECT_POINTER_PARSE_ERROR(input, 1);
}

TEST(JSONPointer_parse_error, tilde) {
  const std::string input{"/~"};
  EXPECT_POINTER_PARSE_ERROR(input, 3);
}

TEST(JSONPointer_parse_error, tilde_2) {
  const std::string input{"/~2"};
  EXPECT_POINTER_PARSE_ERROR(input, 3);
}

TEST(JSONPointer_parse_error, tilde_tilde) {
  const std::string input{"/~~"};
  EXPECT_POINTER_PARSE_ERROR(input, 3);
}

TEST(JSONPointer_parse_error, foo_tilde) {
  const std::string input{"/foo~"};
  EXPECT_POINTER_PARSE_ERROR(input, 6);
}

TEST(JSONPointer_parse_error, index_foo_tilde) {
  const std::string input{"/123/foo~"};
  EXPECT_POINTER_PARSE_ERROR(input, 10);
}

TEST(JSONPointer_parse_error, zero_index_foo_tilde) {
  const std::string input{"/0/foo~"};
  EXPECT_POINTER_PARSE_ERROR(input, 8);
}

TEST(JSONPointer_parse_error, slash_slash_foo_tilde) {
  const std::string input{"//foo~"};
  EXPECT_POINTER_PARSE_ERROR(input, 7);
}

TEST(JSONPointer_parse_error, foo_tilde_2) {
  const std::string input{"/foo~2"};
  EXPECT_POINTER_PARSE_ERROR(input, 6);
}

TEST(JSONPointer_parse_error, foo_tilde_tilde) {
  const std::string input{"/foo~~"};
  EXPECT_POINTER_PARSE_ERROR(input, 6);
}

TEST(JSONPointer_parse_error, backspace) {
  const std::string input{"/\b"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, formfeed) {
  const std::string input{"/\f"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, newline) {
  const std::string input{"/\n"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, carriage_return) {
  const std::string input{"/\r"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, tab) {
  const std::string input{"/\t"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, null) {
  // See https://stackoverflow.com/a/164274
  using namespace std::string_literals;
  const std::string input{"/\0"s};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, backspace_within_word) {
  const std::string input{"/foo\bbar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, formfeed_within_word) {
  const std::string input{"/foo\fbar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, newline_within_word) {
  const std::string input{"/foo\nbar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, carriage_return_within_word) {
  const std::string input{"/foo\rbar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, tab_within_word) {
  const std::string input{"/foo\tbar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, null_within_word) {
  // See https://stackoverflow.com/a/164274
  using namespace std::string_literals;
  const std::string input{"/foo\0bar"s};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0001) {
  const std::string input{"/\u0001"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0002) {
  const std::string input{"/\u0002"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0003) {
  const std::string input{"/\u0003"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0004) {
  const std::string input{"/\u0004"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0005) {
  const std::string input{"/\u0005"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0006) {
  const std::string input{"/\u0006"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0007) {
  const std::string input{"/\u0007"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0008) {
  const std::string input{"/\u0008"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0009) {
  const std::string input{"/\u0009"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_000A) {
  const std::string input{"/\u000A"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_000B) {
  const std::string input{"/\u000B"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_000C) {
  const std::string input{"/\u000C"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_000D) {
  const std::string input{"/\u000D"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_000E) {
  const std::string input{"/\u000E"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_000F) {
  const std::string input{"/\u000F"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0010) {
  const std::string input{"/\u0010"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0011) {
  const std::string input{"/\u0011"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0012) {
  const std::string input{"/\u0012"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0013) {
  const std::string input{"/\u0013"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0014) {
  const std::string input{"/\u0014"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0015) {
  const std::string input{"/\u0015"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0016) {
  const std::string input{"/\u0016"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0017) {
  const std::string input{"/\u0017"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0018) {
  const std::string input{"/\u0018"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0019) {
  const std::string input{"/\u0019"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_001A) {
  const std::string input{"/\u001A"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_001B) {
  const std::string input{"/\u001B"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_001C) {
  const std::string input{"/\u001C"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_001D) {
  const std::string input{"/\u001D"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_001E) {
  const std::string input{"/\u001E"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_001F) {
  const std::string input{"/\u001F"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0001_within_word) {
  const std::string input{"/foo\u0001bar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0002_within_word) {
  const std::string input{"/foo\u0002bar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0003_within_word) {
  const std::string input{"/foo\u0003bar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0004_within_word) {
  const std::string input{"/foo\u0004bar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0005_within_word) {
  const std::string input{"/foo\u0005bar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0006_within_word) {
  const std::string input{"/foo\u0006bar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0007_within_word) {
  const std::string input{"/foo\u0007bar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0008_within_word) {
  const std::string input{"/foo\u0008bar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0009_within_word) {
  const std::string input{"/foo\u0009bar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_000A_within_word) {
  const std::string input{"/foo\u000Abar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_000B_within_word) {
  const std::string input{"/foo\u000Bbar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_000C_within_word) {
  const std::string input{"/foo\u000Cbar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_000D_within_word) {
  const std::string input{"/foo\u000Dbar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_000E_within_word) {
  const std::string input{"/foo\u000Ebar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_000F_within_word) {
  const std::string input{"/foo\u000Fbar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0010_within_word) {
  const std::string input{"/foo\u0010bar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0011_within_word) {
  const std::string input{"/foo\u0011bar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0012_within_word) {
  const std::string input{"/foo\u0012bar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0013_within_word) {
  const std::string input{"/foo\u0013bar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0014_within_word) {
  const std::string input{"/foo\u0014bar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0015_within_word) {
  const std::string input{"/foo\u0015bar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0016_within_word) {
  const std::string input{"/foo\u0016bar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0017_within_word) {
  const std::string input{"/foo\u0017bar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0018_within_word) {
  const std::string input{"/foo\u0018bar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_0019_within_word) {
  const std::string input{"/foo\u0019bar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_001A_within_word) {
  const std::string input{"/foo\u001Abar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_001B_within_word) {
  const std::string input{"/foo\u001Bbar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_001C_within_word) {
  const std::string input{"/foo\u001Cbar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_001D_within_word) {
  const std::string input{"/foo\u001Dbar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_001E_within_word) {
  const std::string input{"/foo\u001Ebar"};
  EXPECT_JSON_PARSE_ERROR(input);
}

TEST(JSONPointer_parse_error, unicode_001F_within_word) {
  const std::string input{"/foo\u001Fbar"};
  EXPECT_JSON_PARSE_ERROR(input);
}
