#include <gtest/gtest.h>

#include <sourcemeta/core/gzip.h>

#include <sstream>

TEST(GZIP, compress_string_1) {
  const auto result{sourcemeta::core::gzip("Hello World")};
  EXPECT_EQ(result.size(), 31);
}

TEST(GZIP, decompress_string_stream_1) {
  const auto value{"Hello World"};
  const auto input{sourcemeta::core::gzip(value)};
  std::istringstream stream{input};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_EQ(result, value);
}

TEST(GZIP, decompress_string_stream_error_1) {
  std::istringstream stream{"not-zlib-content"};
  EXPECT_THROW(sourcemeta::core::gunzip(stream), sourcemeta::core::GZIPError);
}
