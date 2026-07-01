#include <sourcemeta/core/test.h>

#include <sourcemeta/core/gzip.h>

#include <cstdint> // std::uint8_t
#include <cstring> // std::memcmp
#include <string>  // std::string

TEST(compress_empty_input) {
  const std::string input;
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  EXPECT_FALSE(compressed.empty());

  const auto decompressed{sourcemeta::core::gunzip(
      reinterpret_cast<const std::uint8_t *>(compressed.data()),
      compressed.size())};
  EXPECT_EQ(decompressed, input);
}

TEST(compress_hello_world) {
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

TEST(compress_round_trip_binary_data) {
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

TEST(compress_round_trip_large_input) {
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

TEST(decompress_with_output_hint) {
  const std::string input{"hello world"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};

  const auto decompressed{sourcemeta::core::gunzip(
      reinterpret_cast<const std::uint8_t *>(compressed.data()),
      compressed.size(), input.size())};
  EXPECT_EQ(decompressed, input);
}

TEST(compress_with_explicit_level_round_trips) {
  const std::string pattern{"The quick brown fox jumps over the lazy dog. "};
  std::string input;
  for (int index = 0; index < 1000; ++index) {
    input += pattern;
  }

  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size(), 9)};
  EXPECT_FALSE(compressed.empty());

  const auto decompressed{sourcemeta::core::gunzip(
      reinterpret_cast<const std::uint8_t *>(compressed.data()),
      compressed.size())};
  EXPECT_EQ(decompressed, input);
}

TEST(compress_higher_level_is_not_larger) {
  const std::string pattern{"The quick brown fox jumps over the lazy dog. "};
  std::string input;
  for (int index = 0; index < 1000; ++index) {
    input += pattern;
  }

  const auto fastest{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size(), 1)};
  const auto smallest{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size(), 12)};
  EXPECT_LE(smallest.size(), fastest.size());
}

TEST(decompress_invalid_input_throws) {
  const std::string garbage{"this is not gzip data"};
  try {
    sourcemeta::core::gunzip(
        reinterpret_cast<const std::uint8_t *>(garbage.data()), garbage.size());
    FAIL();
  } catch (const sourcemeta::core::GZIPError &error) {
    EXPECT_EQ(std::string{error.what()}, "Could not decompress input");
  }
}
