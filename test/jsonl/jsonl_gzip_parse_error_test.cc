#include <sourcemeta/core/test.h>

#include <sourcemeta/core/gzip.h>
#include <sourcemeta/core/jsonl.h>

#include <cstdint>   // std::uint8_t
#include <exception> // std::exception
#include <sstream>   // std::istringstream
#include <string>    // std::string

TEST(invalid_compressed_data) {
  const std::string garbage{"this is not gzip data"};
  std::istringstream stream{garbage};
  try {
    sourcemeta::core::JSONL parser{stream, sourcemeta::core::JSONL::Mode::GZIP};
    sourcemeta::core::ConstJSONLIterator iterator{parser.cbegin()};
    while (iterator != parser.cend()) {
      ++iterator;
    }
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_STREQ(error.what(), "Invalid gzip magic bytes");
  } catch (const std::exception &) {
    FAIL();
  }
}

TEST(invalid_json_in_compressed_data) {
  const std::string input{"trrue"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  try {
    sourcemeta::core::JSONL parser{stream, sourcemeta::core::JSONL::Mode::GZIP};
    sourcemeta::core::ConstJSONLIterator iterator{parser.cbegin()};
    while (iterator != parser.cend()) {
      ++iterator;
    }
    FAIL();
  } catch (const sourcemeta::core::JSONParseError &error) {
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 3);
  } catch (const std::exception &) {
    FAIL();
  }
}

TEST(invalid_delimiter_in_compressed_data) {
  const std::string input{"false true"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  try {
    sourcemeta::core::JSONL parser{stream, sourcemeta::core::JSONL::Mode::GZIP};
    sourcemeta::core::ConstJSONLIterator iterator{parser.cbegin()};
    while (iterator != parser.cend()) {
      ++iterator;
    }
    FAIL();
  } catch (const sourcemeta::core::JSONParseError &error) {
    EXPECT_EQ(error.line(), 1);
    EXPECT_EQ(error.column(), 7);
  } catch (const std::exception &) {
    FAIL();
  }
}
