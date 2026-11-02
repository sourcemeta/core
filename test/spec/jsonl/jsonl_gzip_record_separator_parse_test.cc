#include <sourcemeta/core/gzip.h>
#include <sourcemeta/core/jsonl.h>
#include <sourcemeta/core/test.h>

#include <cstdint> // std::uint8_t
#include <sstream> // std::istringstream
#include <string>  // std::string
#include <vector>  // std::vector

// A hexadecimal escape consumes every hexadecimal digit that follows it, so
// the record separator always closes the string literal it appears in
static constexpr auto FRAMING{sourcemeta::core::JSONLFraming::RecordSeparator};
static constexpr auto MODE{sourcemeta::core::JSONL::Mode::GZIP};

// The decompressed data is read through a buffer of this many bytes, so a
// record that exceeds it spans more than one refill
static constexpr std::size_t VALUE_SIZE{10000};

TEST(integers) {
  const std::string input{"\x1E"
                          "1\n"
                          "\x1E"
                          "2\n"
                          "\x1E"
                          "3\n"
                          "\x1E"
                          "4\n"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
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
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
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

TEST(truncated_number_dropped) {
  const std::string input{"\x1E"
                          "123"
                          "\x1E"
                          "456\n"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream, MODE, FRAMING}) {
    result.push_back(document);
  }

  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(result.at(0).is_integer());
  EXPECT_EQ(result.at(0).to_integer(), 456);
}

TEST(records_across_buffer_refills) {
  const std::string first(VALUE_SIZE, 'a');
  const std::string second(VALUE_SIZE, 'b');
  const std::string third(VALUE_SIZE, 'c');
  const std::string input{"\x1E"
                          "{ \"value\": \"" +
                          first + "\" }\n" +
                          "\x1E"
                          "{ \"value\": \"" +
                          second + "\" }\n" +
                          "\x1E"
                          "{ \"value\": \"" +
                          third + "\" }\n"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  std::vector<sourcemeta::core::JSON> result;
  for (const auto &document : sourcemeta::core::JSONL{stream, MODE, FRAMING}) {
    result.push_back(document);
  }

  EXPECT_EQ(result.size(), 3);
  EXPECT_TRUE(result.at(0).is_object());
  EXPECT_TRUE(result.at(1).is_object());
  EXPECT_TRUE(result.at(2).is_object());
  EXPECT_TRUE(result.at(0).defines("value"));
  EXPECT_TRUE(result.at(1).defines("value"));
  EXPECT_TRUE(result.at(2).defines("value"));
  EXPECT_EQ(result.at(0).at("value").to_string(), first);
  EXPECT_EQ(result.at(1).at("value").to_string(), second);
  EXPECT_EQ(result.at(2).at("value").to_string(), third);
}
