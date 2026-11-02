#include <sourcemeta/core/jsonl.h>
#include <sourcemeta/core/test.h>

#include <sstream> // std::istringstream
#include <string>  // std::string
#include <vector>  // std::vector

// A hexadecimal escape consumes every hexadecimal digit that follows it, so
// the record separator always closes the string literal it appears in
static constexpr auto FRAMING{sourcemeta::core::JSONLFraming::RecordSeparator};
static constexpr auto MODE{sourcemeta::core::JSONL::Mode::Raw};

TEST(empty) {
  const std::string input;
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream, MODE, FRAMING}) {
    result.push_back(document);
  }

  EXPECT_TRUE(result.empty());
}

TEST(blank) {
  const std::string input{"    "};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream, MODE, FRAMING}) {
    result.push_back(document);
  }

  EXPECT_TRUE(result.empty());
}

TEST(integers) {
  const std::string input{"\x1E"
                          "1\n"
                          "\x1E"
                          "2\n"
                          "\x1E"
                          "3\n"
                          "\x1E"
                          "4\n"};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream, MODE, FRAMING}) {
    result.push_back(document);
  }

  EXPECT_EQ(result.size(), 4);
  EXPECT_TRUE(result.at(0).is_integer());
  EXPECT_TRUE(result.at(1).is_integer());
  EXPECT_TRUE(result.at(2).is_integer());
  EXPECT_TRUE(result.at(3).is_integer());
  EXPECT_EQ(result.at(0).to_integer(), 1);
  EXPECT_EQ(result.at(1).to_integer(), 2);
  EXPECT_EQ(result.at(2).to_integer(), 3);
  EXPECT_EQ(result.at(3).to_integer(), 4);
}

TEST(objects) {
  const std::string input{"\x1E"
                          "{ \"foo\": 1 }\n"
                          "\x1E"
                          "{ \"bar\": 2 }\n"};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream, MODE, FRAMING}) {
    result.push_back(document);
  }

  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.at(0).is_object());
  EXPECT_TRUE(result.at(1).is_object());
  EXPECT_EQ(result.at(0).size(), 1);
  EXPECT_EQ(result.at(1).size(), 1);
  EXPECT_TRUE(result.at(0).defines("foo"));
  EXPECT_TRUE(result.at(1).defines("bar"));
  EXPECT_EQ(result.at(0).at("foo").to_integer(), 1);
  EXPECT_EQ(result.at(1).at("bar").to_integer(), 2);
}

TEST(object_without_trailing_line_feed) {
  const std::string input{"\x1E"
                          "{ \"foo\": 1 }"};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream, MODE, FRAMING}) {
    result.push_back(document);
  }

  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.at(0).is_object());
  EXPECT_EQ(result.at(0).size(), 1);
  EXPECT_TRUE(result.at(0).defines("foo"));
  EXPECT_EQ(result.at(0).at("foo").to_integer(), 1);
}

TEST(array_without_trailing_line_feed) {
  const std::string input{"\x1E"
                          "[ 1, 2 ]"};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream, MODE, FRAMING}) {
    result.push_back(document);
  }

  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.at(0).is_array());
  EXPECT_EQ(result.at(0).size(), 2);
  EXPECT_EQ(result.at(0).at(0).to_integer(), 1);
  EXPECT_EQ(result.at(0).at(1).to_integer(), 2);
}

TEST(string_without_trailing_line_feed) {
  const std::string input{"\x1E"
                          "\"foo\""};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream, MODE, FRAMING}) {
    result.push_back(document);
  }

  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.at(0).is_string());
  EXPECT_EQ(result.at(0).to_string(), "foo");
}

TEST(truncated_number_dropped) {
  const std::string input{"\x1E"
                          "123"};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream, MODE, FRAMING}) {
    result.push_back(document);
  }

  EXPECT_TRUE(result.empty());
}

TEST(truncated_boolean_dropped) {
  const std::string input{"\x1E"
                          "true"};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream, MODE, FRAMING}) {
    result.push_back(document);
  }

  EXPECT_TRUE(result.empty());
}

TEST(truncated_null_dropped) {
  const std::string input{"\x1E"
                          "null"};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream, MODE, FRAMING}) {
    result.push_back(document);
  }

  EXPECT_TRUE(result.empty());
}

TEST(truncated_number_between_separators) {
  const std::string input{"\x1E"
                          "123"
                          "\x1E"
                          "456\n"};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream, MODE, FRAMING}) {
    result.push_back(document);
  }

  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.at(0).is_integer());
  EXPECT_EQ(result.at(0).to_integer(), 456);
}

TEST(number_delimited_by_space) {
  const std::string input{"\x1E"
                          "123 "
                          "\x1E"
                          "456\n"};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream, MODE, FRAMING}) {
    result.push_back(document);
  }

  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.at(0).is_integer());
  EXPECT_TRUE(result.at(1).is_integer());
  EXPECT_EQ(result.at(0).to_integer(), 123);
  EXPECT_EQ(result.at(1).to_integer(), 456);
}

TEST(consecutive_separators) {
  const std::string input{"\x1E"
                          "\x1E"
                          "\x1E"
                          "1\n"};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream, MODE, FRAMING}) {
    result.push_back(document);
  }

  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.at(0).is_integer());
  EXPECT_EQ(result.at(0).to_integer(), 1);
}

TEST(whitespace_only_record) {
  const std::string input{"\x1E"
                          " \n"
                          "\x1E"
                          "1\n"};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream, MODE, FRAMING}) {
    result.push_back(document);
  }

  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.at(0).is_integer());
  EXPECT_EQ(result.at(0).to_integer(), 1);
}

TEST(whitespace_before_first_separator) {
  const std::string input{"  \n"
                          "\x1E"
                          "1\n"};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream, MODE, FRAMING}) {
    result.push_back(document);
  }

  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.at(0).is_integer());
  EXPECT_EQ(result.at(0).to_integer(), 1);
}

TEST(multi_line_value) {
  const std::string input{"\x1E"
                          "{\n  \"foo\": 1\n}\n"
                          "\x1E"
                          "2\n"};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream, MODE, FRAMING}) {
    result.push_back(document);
  }

  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.at(0).is_object());
  EXPECT_EQ(result.at(0).size(), 1);
  EXPECT_TRUE(result.at(0).defines("foo"));
  EXPECT_EQ(result.at(0).at("foo").to_integer(), 1);
  EXPECT_TRUE(result.at(1).is_integer());
  EXPECT_EQ(result.at(1).to_integer(), 2);
}

TEST(carriage_return_line_endings) {
  const std::string input{"\x1E"
                          "1\r\n"
                          "\x1E"
                          "2\r\n"};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream, MODE, FRAMING}) {
    result.push_back(document);
  }

  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(result.at(0).is_integer());
  EXPECT_TRUE(result.at(1).is_integer());
  EXPECT_EQ(result.at(0).to_integer(), 1);
  EXPECT_EQ(result.at(1).to_integer(), 2);
}

TEST(mixed_types) {
  const std::string input{"\x1E"
                          "{ \"foo\": 1 }\n"
                          "\x1E"
                          "[ 1, 2 ]\n"
                          "\x1E"
                          "\"bar\"\n"
                          "\x1E"
                          "3\n"
                          "\x1E"
                          "true\n"
                          "\x1E"
                          "null\n"};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream, MODE, FRAMING}) {
    result.push_back(document);
  }

  EXPECT_EQ(result.size(), 6);
  EXPECT_TRUE(result.at(0).is_object());
  EXPECT_TRUE(result.at(1).is_array());
  EXPECT_TRUE(result.at(2).is_string());
  EXPECT_TRUE(result.at(3).is_integer());
  EXPECT_TRUE(result.at(4).is_boolean());
  EXPECT_TRUE(result.at(5).is_null());
  EXPECT_EQ(result.at(0).size(), 1);
  EXPECT_TRUE(result.at(0).defines("foo"));
  EXPECT_EQ(result.at(0).at("foo").to_integer(), 1);
  EXPECT_EQ(result.at(1).size(), 2);
  EXPECT_EQ(result.at(1).at(0).to_integer(), 1);
  EXPECT_EQ(result.at(1).at(1).to_integer(), 2);
  EXPECT_EQ(result.at(2).to_string(), "bar");
  EXPECT_EQ(result.at(3).to_integer(), 3);
  EXPECT_TRUE(result.at(4).to_boolean());
}

TEST(explicit_line_feed_framing) {
  const std::string input{"1\n2\n3\n"};
  std::istringstream stream{input};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{
           stream, MODE, sourcemeta::core::JSONLFraming::LineFeed}) {
    result.push_back(document);
  }

  EXPECT_EQ(result.size(), 3);
  EXPECT_TRUE(result.at(0).is_integer());
  EXPECT_TRUE(result.at(1).is_integer());
  EXPECT_TRUE(result.at(2).is_integer());
  EXPECT_EQ(result.at(0).to_integer(), 1);
  EXPECT_EQ(result.at(1).to_integer(), 2);
  EXPECT_EQ(result.at(2).to_integer(), 3);
}
