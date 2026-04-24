#include <gtest/gtest.h>

#include <sourcemeta/core/gzip.h>

#include <cstdint> // std::uint8_t
#include <cstring> // std::memcmp
#include <string>  // std::string

TEST(GZIP_Compress, empty_input) {
  const std::string input;
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  EXPECT_FALSE(compressed.empty());

  const auto decompressed{sourcemeta::core::gunzip(
      reinterpret_cast<const std::uint8_t *>(compressed.data()),
      compressed.size())};
  EXPECT_EQ(decompressed, input);
}

TEST(GZIP_Compress, hello_world) {
  const std::string input{"hello world"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  EXPECT_FALSE(compressed.empty());
  EXPECT_LT(compressed.size(), input.size() + 30);

  const auto decompressed{sourcemeta::core::gunzip(
      reinterpret_cast<const std::uint8_t *>(compressed.data()),
      compressed.size())};
  EXPECT_EQ(decompressed, input);
}

TEST(GZIP_Compress, round_trip_binary_data) {
  std::string input;
  input.resize(256);
  for (unsigned int index = 0; index < 256; ++index) {
    input[index] = static_cast<char>(index);
  }

  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  EXPECT_FALSE(compressed.empty());

  const auto decompressed{sourcemeta::core::gunzip(
      reinterpret_cast<const std::uint8_t *>(compressed.data()),
      compressed.size())};
  EXPECT_EQ(decompressed.size(), input.size());
  EXPECT_EQ(std::memcmp(decompressed.data(), input.data(), input.size()), 0);
}

TEST(GZIP_Compress, round_trip_large_input) {
  const std::string pattern{"The quick brown fox jumps over the lazy dog. "};
  std::string input;
  for (int index = 0; index < 1000; ++index) {
    input += pattern;
  }

  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  EXPECT_FALSE(compressed.empty());
  EXPECT_LT(compressed.size(), input.size());

  const auto decompressed{sourcemeta::core::gunzip(
      reinterpret_cast<const std::uint8_t *>(compressed.data()),
      compressed.size())};
  EXPECT_EQ(decompressed, input);
}

TEST(GZIP_Decompress, with_output_hint) {
  const std::string input{"hello world"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};

  const auto decompressed{sourcemeta::core::gunzip(
      reinterpret_cast<const std::uint8_t *>(compressed.data()),
      compressed.size(), input.size())};
  EXPECT_EQ(decompressed, input);
}

TEST(GZIP_Decompress, invalid_input_throws) {
  const std::string garbage{"this is not gzip data"};
  EXPECT_THROW(sourcemeta::core::gunzip(
                   reinterpret_cast<const std::uint8_t *>(garbage.data()),
                   garbage.size()),
               sourcemeta::core::GZIPError);
}
