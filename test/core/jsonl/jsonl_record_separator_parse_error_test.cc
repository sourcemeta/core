#include <sourcemeta/core/jsonl.h>
#include <sourcemeta/core/test.h>

#include <exception> // std::exception
#include <sstream>   // std::istringstream
#include <string>    // std::string

// A hexadecimal escape consumes every hexadecimal digit that follows it, so
// the record separator always closes the string literal it appears in
static constexpr auto FRAMING{sourcemeta::core::JSONLFraming::RecordSeparator};
static constexpr auto MODE{sourcemeta::core::JSONL::Mode::Raw};

#define EXPECT_PARSE_ERROR(stream, expected_line, expected_column)             \
  try {                                                                        \
    sourcemeta::core::JSONL parser{stream, MODE, FRAMING};                     \
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

TEST(content_before_first_separator) {
  const std::string data{"garbage"
                         "\x1E"
                         "1\n"};
  std::istringstream input{data};
  EXPECT_PARSE_ERROR(input, 1, 1);
}

TEST(value_before_first_separator) {
  const std::string data{"1\n"
                         "\x1E"
                         "2\n"};
  std::istringstream input{data};
  EXPECT_PARSE_ERROR(input, 1, 1);
}

TEST(invalid_first_record) {
  const std::string data{"\x1E"
                         "trrue\n"};
  std::istringstream input{data};
  EXPECT_PARSE_ERROR(input, 1, 4);
}

TEST(invalid_second_record) {
  const std::string data{"\x1E"
                         "1\n"
                         "\x1E"
                         "trrue\n"};
  std::istringstream input{data};
  EXPECT_PARSE_ERROR(input, 2, 4);
}

TEST(trailing_content_in_record) {
  const std::string data{"\x1E"
                         "1 2\n"};
  std::istringstream input{data};
  EXPECT_PARSE_ERROR(input, 1, 4);
}

TEST(trailing_content_in_multi_line_record) {
  const std::string data{"\x1E"
                         "{\n  \"foo\": 1\n} x\n"};
  std::istringstream input{data};
  EXPECT_PARSE_ERROR(input, 3, 3);
}

TEST(invalid_multi_line_record) {
  const std::string data{"\x1E"
                         "{\n  \"foo\": }\n"};
  std::istringstream input{data};
  EXPECT_PARSE_ERROR(input, 2, 10);
}
