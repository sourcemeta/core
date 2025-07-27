#include <gtest/gtest.h>

#include <sourcemeta/core/gzip.h>

#include <sstream>

TEST(GZIP, compress_stream_1) {
  const auto value{"Hello World"};
  std::istringstream input{value};
  std::ostringstream output;
  sourcemeta::core::gzip(input, output);
  EXPECT_EQ(output.str().size(), 31);
}

TEST(GZIP, compress_stream_to_string_1) {
  const auto value{"Hello World"};
  std::istringstream input{value};
  const auto result{sourcemeta::core::gzip(input)};
  EXPECT_EQ(result.size(), 31);
}

TEST(GZIP, compress_string_to_string_1) {
  const auto result{sourcemeta::core::gzip("Hello World")};
  EXPECT_EQ(result.size(), 31);
}

TEST(GZIP, decompress_stream_1) {
  const auto value{"Hello World"};
  std::istringstream input{sourcemeta::core::gzip(value)};
  std::ostringstream output;
  sourcemeta::core::gunzip(input, output);
  EXPECT_EQ(output.str(), value);
}

TEST(GZIP, decompress_stream_error_1) {
  std::istringstream input{"not-zlib-content"};
  std::ostringstream output;
  EXPECT_THROW(sourcemeta::core::gunzip(input, output),
               sourcemeta::core::GZIPError);
}

TEST(GZIP, decompress_stream_to_string_1) {
  const auto value{"Hello World"};
  std::istringstream input{sourcemeta::core::gzip(value)};
  const auto result{sourcemeta::core::gunzip(input)};
  EXPECT_EQ(result, value);
}

TEST(GZIP, decompress_string_to_string_1) {
  const auto value{"Hello World"};
  const auto result{sourcemeta::core::gunzip(sourcemeta::core::gzip(value))};
  EXPECT_EQ(result, value);
}
