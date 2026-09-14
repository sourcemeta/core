#include <sourcemeta/core/gzip.h>
#include <sourcemeta/core/test.h>

#include <array>    // std::array
#include <cstddef>  // std::size_t
#include <cstdint>  // std::uint8_t
#include <cstring>  // std::memcmp
#include <istream>  // std::istream
#include <iterator> // std::istreambuf_iterator
#include <sstream>  // std::istringstream
#include <string>   // std::string

TEST(read_char_by_char) {
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

TEST(multiple_small_reads) {
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

TEST(read_after_end_returns_eof) {
  const std::string input{"hello"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  std::string result;
  result.assign(std::istreambuf_iterator<char>(decompressed),
                std::istreambuf_iterator<char>());
  EXPECT_EQ(result, input);
  EXPECT_EQ(decompressed.get(), std::char_traits<char>::eof());
  EXPECT_EQ(decompressed.get(), std::char_traits<char>::eof());
}

TEST(peek_returns_first_byte) {
  const std::string input{"hello"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  EXPECT_EQ(decompressed.peek(), 'h');
  EXPECT_EQ(decompressed.peek(), 'h');
  EXPECT_EQ(decompressed.get(), 'h');
  EXPECT_EQ(decompressed.peek(), 'e');
}

TEST(mixed_peek_and_get) {
  const std::string input{"abcdef"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  EXPECT_EQ(decompressed.peek(), 'a');
  EXPECT_EQ(decompressed.get(), 'a');
  EXPECT_EQ(decompressed.peek(), 'b');
  EXPECT_EQ(decompressed.get(), 'b');
  EXPECT_EQ(decompressed.get(), 'c');
  EXPECT_EQ(decompressed.peek(), 'd');
  EXPECT_EQ(decompressed.get(), 'd');
  EXPECT_EQ(decompressed.get(), 'e');
  EXPECT_EQ(decompressed.get(), 'f');
  EXPECT_EQ(decompressed.peek(), std::char_traits<char>::eof());
}

TEST(read_chunk_equal_to_internal_buffer) {
  std::string input;
  input.resize(50000);
  for (std::size_t index = 0; index < input.size(); ++index) {
    input[index] = static_cast<char>(index & 0xff);
  }
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  std::string result;
  std::array<char, 16384> chunk{};
  while (decompressed.read(chunk.data(),
                           static_cast<std::streamsize>(chunk.size()))) {
    result.append(chunk.data(),
                  static_cast<std::size_t>(decompressed.gcount()));
  }
  result.append(chunk.data(), static_cast<std::size_t>(decompressed.gcount()));
  EXPECT_EQ(result.size(), input.size());
  EXPECT_EQ(std::memcmp(result.data(), input.data(), input.size()), 0);
}

TEST(read_chunk_larger_than_internal_buffer) {
  std::string input;
  input.resize(50000);
  for (std::size_t index = 0; index < input.size(); ++index) {
    input[index] = static_cast<char>(index & 0xff);
  }
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  std::string result;
  std::array<char, 40000> chunk{};
  while (decompressed.read(chunk.data(),
                           static_cast<std::streamsize>(chunk.size()))) {
    result.append(chunk.data(),
                  static_cast<std::size_t>(decompressed.gcount()));
  }
  result.append(chunk.data(), static_cast<std::size_t>(decompressed.gcount()));
  EXPECT_EQ(result.size(), input.size());
  EXPECT_EQ(std::memcmp(result.data(), input.data(), input.size()), 0);
}

TEST(read_via_getline) {
  const std::string input{"line one\nline two\nline three"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  std::string first_line;
  std::getline(decompressed, first_line);
  EXPECT_EQ(first_line, "line one");
  std::string second_line;
  std::getline(decompressed, second_line);
  EXPECT_EQ(second_line, "line two");
  std::string third_line;
  std::getline(decompressed, third_line);
  EXPECT_EQ(third_line, "line three");
}

TEST(ignore_skips_bytes) {
  const std::string input{"hello world"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  decompressed.ignore(6);
  std::string remaining;
  remaining.assign(std::istreambuf_iterator<char>(decompressed),
                   std::istreambuf_iterator<char>());
  EXPECT_EQ(remaining, "world");
}

TEST(operator_in_extracts_tokens) {
  const std::string input{"hello world"};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size())};
  std::istringstream stream{compressed};
  sourcemeta::core::GZIPStreamBuffer buffer{stream};
  std::istream decompressed{&buffer};

  std::string first;
  std::string second;
  decompressed >> first >> second;
  EXPECT_EQ(first, "hello");
  EXPECT_EQ(second, "world");
}
