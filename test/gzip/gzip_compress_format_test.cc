#include <sourcemeta/core/gzip.h>
#include <sourcemeta/core/test.h>

#include <cstdint> // std::uint8_t
#include <string>  // std::string
#include <vector>  // std::vector

namespace {

auto compress(const std::string &input, const int level)
    -> std::vector<std::uint8_t> {
  const auto output{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(input.data()), input.size(),
      level)};
  return {output.cbegin(), output.cend()};
}

} // namespace

TEST(compress_output_header_identifies_deflate_member) {
  const auto compressed{compress("hello world", 6)};
  EXPECT_GE(compressed.size(), 18);
  EXPECT_EQ(compressed.at(0), 0x1f);
  EXPECT_EQ(compressed.at(1), 0x8b);
  EXPECT_EQ(compressed.at(2), 0x08);
  EXPECT_EQ(compressed.at(3) & 0xe0, 0);
}

TEST(compress_output_trailer_of_empty_input) {
  const auto compressed{compress("", 1)};
  const std::vector<std::uint8_t> trailer{compressed.cend() - 8,
                                          compressed.cend()};
  const std::vector<std::uint8_t> expected{0x00, 0x00, 0x00, 0x00,
                                           0x00, 0x00, 0x00, 0x00};
  EXPECT_EQ(trailer, expected);
}

TEST(compress_output_trailer_of_hello_world_at_level_0) {
  const auto compressed{compress("hello world", 0)};
  const std::vector<std::uint8_t> trailer{compressed.cend() - 8,
                                          compressed.cend()};
  const std::vector<std::uint8_t> expected{0x85, 0x11, 0x4a, 0x0d,
                                           0x0b, 0x00, 0x00, 0x00};
  EXPECT_EQ(trailer, expected);
}

TEST(compress_output_trailer_of_hello_world_at_level_1) {
  const auto compressed{compress("hello world", 1)};
  const std::vector<std::uint8_t> trailer{compressed.cend() - 8,
                                          compressed.cend()};
  const std::vector<std::uint8_t> expected{0x85, 0x11, 0x4a, 0x0d,
                                           0x0b, 0x00, 0x00, 0x00};
  EXPECT_EQ(trailer, expected);
}

TEST(compress_output_trailer_of_hello_world_at_level_12) {
  const auto compressed{compress("hello world", 12)};
  const std::vector<std::uint8_t> trailer{compressed.cend() - 8,
                                          compressed.cend()};
  const std::vector<std::uint8_t> expected{0x85, 0x11, 0x4a, 0x0d,
                                           0x0b, 0x00, 0x00, 0x00};
  EXPECT_EQ(trailer, expected);
}

TEST(compress_output_trailer_of_crc32_check_string) {
  const auto compressed{compress("123456789", 6)};
  const std::vector<std::uint8_t> trailer{compressed.cend() - 8,
                                          compressed.cend()};
  const std::vector<std::uint8_t> expected{0x26, 0x39, 0xf4, 0xcb,
                                           0x09, 0x00, 0x00, 0x00};
  EXPECT_EQ(trailer, expected);
}

TEST(compress_output_trailer_of_pangram) {
  const auto compressed{
      compress("The quick brown fox jumps over the lazy dog", 9)};
  const std::vector<std::uint8_t> trailer{compressed.cend() - 8,
                                          compressed.cend()};
  const std::vector<std::uint8_t> expected{0x39, 0xa3, 0x4f, 0x41,
                                           0x2b, 0x00, 0x00, 0x00};
  EXPECT_EQ(trailer, expected);
}

TEST(compress_output_trailer_size_spans_multiple_bytes) {
  const auto compressed{compress(std::string(70000, 'z'), 1)};
  const std::vector<std::uint8_t> size{compressed.cend() - 4,
                                       compressed.cend()};
  const std::vector<std::uint8_t> expected{0x70, 0x11, 0x01, 0x00};
  EXPECT_EQ(size, expected);
}
