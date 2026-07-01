#include <sourcemeta/core/test.h>

#include <sourcemeta/core/jsonl.h>

#include <exception>
#include <sstream>
#include <vector>

#define EXPECT_PARSE_ERROR(stream, expected_line, expected_column)             \
  try {                                                                        \
    sourcemeta::core::JSONL parser{stream};                                    \
    sourcemeta::core::ConstJSONLIterator iterator{parser.cbegin()};            \
    while (iterator != parser.cend())                                          \
      ++iterator;                                                              \
    FAIL();                                                                    \
  } catch (const sourcemeta::core::JSONParseError &error) {                    \
    EXPECT_EQ(error.line(), expected_line);                                    \
    EXPECT_EQ(error.column(), expected_column);                                \
  } catch (const std::exception &) {                                           \
    FAIL();                                                                    \
  }

TEST(invalid_first_row) {
  std::istringstream input{"trrue"};
  EXPECT_PARSE_ERROR(input, 1, 3);
}

TEST(invalid_second_row) {
  std::istringstream input{"true\ntrrue"};
  EXPECT_PARSE_ERROR(input, 2, 3);
}

TEST(blank_with_whitespace_characters) {
  std::istringstream input{"\r\r\n\n\t\t"};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{input}) {
    result.push_back(document);
  }

  EXPECT_TRUE(result.empty());
}

TEST(invalid_delimiter_space) {
  std::istringstream input{"false true"};
  EXPECT_PARSE_ERROR(input, 1, 7);
}

TEST(invalid_delimiter_carriage_return) {
  std::istringstream input{"false\rtrue"};
  EXPECT_PARSE_ERROR(input, 1, 7);
}

TEST(invalid_delimiter_tab) {
  std::istringstream input{"false\ttrue"};
  EXPECT_PARSE_ERROR(input, 1, 7);
}

TEST(invalid_delimiter_x) {
  std::istringstream input{"false x true"};
  EXPECT_PARSE_ERROR(input, 1, 7);
}

TEST(invalid_multi_line_row) {
  // Multi-line JSON values are not valid JSONL.
  // Each line must be a complete JSON value.
  // See https://jsonlines.org
  std::istringstream input{
      "{\n \"foo\": 1\n }\n{\n \"bar\" 2\n }\n{\n \"baz\": 3\n }"};
  EXPECT_PARSE_ERROR(input, 1, 2);
}

TEST(backspace_prefix) {
  std::istringstream input{"false\n\btrue"};
  EXPECT_PARSE_ERROR(input, 2, 1);
}
