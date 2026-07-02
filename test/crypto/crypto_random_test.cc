#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/test.h>

#include <array>   // std::array
#include <cstdint> // std::uint8_t
#include <span>    // std::span
#include <string>  // std::string

TEST(random_bytes_returns_requested_length) {
  EXPECT_EQ(sourcemeta::core::random_bytes(32).size(), 32);
}

TEST(random_bytes_returns_empty_for_zero_length) {
  EXPECT_TRUE(sourcemeta::core::random_bytes(0).empty());
}

TEST(random_bytes_returns_large_length) {
  EXPECT_EQ(sourcemeta::core::random_bytes(4096).size(), 4096);
}

TEST(random_bytes_single_byte) {
  EXPECT_EQ(sourcemeta::core::random_bytes(1).size(), 1);
}

TEST(random_bytes_two_calls_differ) {
  const auto first{sourcemeta::core::random_bytes(32)};
  const auto second{sourcemeta::core::random_bytes(32)};
  EXPECT_NE(first, second);
}

TEST(random_bytes_not_all_zero) {
  const auto bytes{sourcemeta::core::random_bytes(32)};
  const std::string zeroes(32, '\0');
  EXPECT_NE(bytes, zeroes);
}

TEST(fill_populates_buffer) {
  std::array<std::uint8_t, 32> buffer{};
  sourcemeta::core::random_bytes(buffer);
  std::array<std::uint8_t, 32> zeroes{};
  EXPECT_NE(buffer, zeroes);
}

TEST(fill_two_calls_differ) {
  std::array<std::uint8_t, 32> first{};
  std::array<std::uint8_t, 32> second{};
  sourcemeta::core::random_bytes(first);
  sourcemeta::core::random_bytes(second);
  EXPECT_NE(first, second);
}

TEST(fill_empty_span_is_noop) {
  sourcemeta::core::random_bytes(std::span<std::uint8_t>{});
  EXPECT_TRUE(true);
}

TEST(fill_respects_span_bounds) {
  std::array<std::uint8_t, 8> buffer{};
  sourcemeta::core::random_bytes(std::span<std::uint8_t>{buffer.data(), 4});
  // The bytes past the requested span must remain untouched
  EXPECT_EQ(buffer[4], 0);
  EXPECT_EQ(buffer[5], 0);
  EXPECT_EQ(buffer[6], 0);
  EXPECT_EQ(buffer[7], 0);
}
