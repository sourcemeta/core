#include <gtest/gtest.h>

#include <sourcemeta/core/gzip.h>

#include <array>    // std::array
#include <cstdint>  // std::uint8_t
#include <cstring>  // std::memcmp
#include <istream>  // std::istream
#include <iterator> // std::istreambuf_iterator
#include <sstream>  // std::istringstream
#include <string>   // std::string

TEST(GZIP_stream_buffer, empty) {
  const std::string input;
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  std::string result;
  result.assign(std::istreambuf_iterator<char>(decompressed),
                std::istreambuf_iterator<char>());
  EXPECT_TRUE(result.empty());
}

TEST(GZIP_stream_buffer, hello_world) {
  const std::string input{"hello world"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  std::string result;
  result.assign(std::istreambuf_iterator<char>(decompressed),
                std::istreambuf_iterator<char>());
  EXPECT_EQ(result, input);
}

TEST(GZIP_stream_buffer, large_input) {
  const std::string pattern{"The quick brown fox jumps over the lazy dog. "};
  std::string input;
  for (int index = 0; index < 1000; ++index) {
    input += pattern;
  }

  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  std::string result;
  result.assign(std::istreambuf_iterator<char>(decompressed),
                std::istreambuf_iterator<char>());
  EXPECT_EQ(result, input);
}

TEST(GZIP_stream_buffer, binary_data) {
  std::string input;
  input.resize(256);
  for (unsigned int index = 0; index < 256; ++index) {
    input[index] = static_cast<char>(index);
  }

  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  std::string result;
  result.assign(std::istreambuf_iterator<char>(decompressed),
                std::istreambuf_iterator<char>());
  EXPECT_EQ(result.size(), input.size());
  EXPECT_EQ(std::memcmp(result.data(), input.data(), input.size()), 0);
}

TEST(GZIP_stream_buffer, read_char_by_char) {
  const std::string input{"hello world"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  std::string result;
  char character{};
  while (decompressed.get(character)) {
    result += character;
  }

  EXPECT_EQ(result, input);
}

TEST(GZIP_stream_buffer, multiple_small_reads) {
  const std::string pattern{"abcdefghij"};
  std::string input;
  for (int index = 0; index < 500; ++index) {
    input += pattern;
  }

  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  std::string result;
  std::array<char, 7> chunk{};
  while (decompressed.read(chunk.data(),
                           static_cast<std::streamsize>(chunk.size()))) {
    result.append(chunk.data(),
                  static_cast<std::size_t>(decompressed.gcount()));
  }

  result.append(chunk.data(), static_cast<std::size_t>(decompressed.gcount()));
  EXPECT_EQ(result, input);
}

TEST(GZIP_stream_buffer, invalid_input_throws) {
  const std::string garbage{"this is not gzip data"};
  std::istringstream stream{garbage};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  std::string result;
  EXPECT_THROW(result.assign(std::istreambuf_iterator<char>(decompressed),
                             std::istreambuf_iterator<char>()),
               sourcemeta::core::GZIPError);
}

TEST(GZIP_stream_buffer, truncated_input_throws) {
  const std::string input{"hello world, this needs to be long enough"};
  auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  compressed.resize(compressed.size() / 2);

  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  std::string result;
  EXPECT_THROW(result.assign(std::istreambuf_iterator<char>(decompressed),
                             std::istreambuf_iterator<char>()),
               sourcemeta::core::GZIPError);
}
