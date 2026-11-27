#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/test.h>

#include <algorithm> // std::ranges::fill
#include <array>     // std::array
#include <cstdint>   // std::uint8_t
#include <span>      // std::span
#include <string>    // std::string

namespace {
auto counting_generator(const std::span<std::uint8_t> buffer) -> void {
  std::uint8_t next{0};
  for (auto &byte : buffer) {
    byte = next;
    next += 1;
  }
}

auto saturated_generator(const std::span<std::uint8_t> buffer) -> void {
  std::ranges::fill(buffer, std::uint8_t{0xff});
}

auto refusing_generator(const std::span<std::uint8_t>) -> void {
  throw sourcemeta::core::CryptoError{"Refusing to draw random bytes"};
}
} // namespace

// The bytes come from the given generator and from nowhere else, so a
// generator that answers the same way every time fixes the whole result
TEST(random_bytes_takes_the_bytes_from_the_generator) {
  const auto bytes{sourcemeta::core::random_bytes(4, counting_generator)};
  EXPECT_EQ(bytes.size(), 4);
  EXPECT_EQ(static_cast<std::uint8_t>(bytes[0]), 0x00);
  EXPECT_EQ(static_cast<std::uint8_t>(bytes[1]), 0x01);
  EXPECT_EQ(static_cast<std::uint8_t>(bytes[2]), 0x02);
  EXPECT_EQ(static_cast<std::uint8_t>(bytes[3]), 0x03);
}

TEST(random_bytes_fills_a_buffer_from_the_generator) {
  std::array<std::uint8_t, 3> buffer{};
  sourcemeta::core::random_bytes(buffer, saturated_generator);
  EXPECT_EQ(buffer[0], 0xff);
  EXPECT_EQ(buffer[1], 0xff);
  EXPECT_EQ(buffer[2], 0xff);
}

// A refusal has no shorter answer than throwing, so it leaves the call the
// same way the system provider's own refusal does
TEST(random_bytes_raises_a_refusal_from_the_generator) {
  bool raised{false};
  try {
    [[maybe_unused]] const auto bytes{
        sourcemeta::core::random_bytes(4, refusing_generator)};
  } catch (const sourcemeta::core::CryptoError &error) {
    raised = true;
    EXPECT_STREQ(error.what(), "Refusing to draw random bytes");
  }

  EXPECT_TRUE(raised);
}

// An empty request never reaches the generator, so one that would refuse is
// never asked
TEST(random_bytes_does_not_draw_for_an_empty_request) {
  EXPECT_TRUE(sourcemeta::core::random_bytes(0, refusing_generator).empty());
}

TEST(random_bytes_returns_requested_length) {
  EXPECT_EQ(sourcemeta::core::random_bytes(
                32, sourcemeta::core::system_random_generator)
                .size(),
            32);
}

TEST(random_bytes_returns_empty_for_zero_length) {
  EXPECT_TRUE(sourcemeta::core::random_bytes(
                  0, sourcemeta::core::system_random_generator)
                  .empty());
}

TEST(random_bytes_returns_large_length) {
  EXPECT_EQ(sourcemeta::core::random_bytes(
                4096, sourcemeta::core::system_random_generator)
                .size(),
            4096);
}

TEST(random_bytes_single_byte) {
  EXPECT_EQ(sourcemeta::core::random_bytes(
                1, sourcemeta::core::system_random_generator)
                .size(),
            1);
}

TEST(random_bytes_two_calls_differ) {
  const auto first{sourcemeta::core::random_bytes(
      32, sourcemeta::core::system_random_generator)};
  const auto second{sourcemeta::core::random_bytes(
      32, sourcemeta::core::system_random_generator)};
  EXPECT_NE(first, second);
}

TEST(random_bytes_not_all_zero) {
  const auto bytes{sourcemeta::core::random_bytes(
      32, sourcemeta::core::system_random_generator)};
  const std::string zeroes(32, '\0');
  EXPECT_NE(bytes, zeroes);
}

TEST(fill_populates_buffer) {
  std::array<std::uint8_t, 32> buffer{};
  sourcemeta::core::random_bytes(buffer,
                                 sourcemeta::core::system_random_generator);
  std::array<std::uint8_t, 32> zeroes{};
  EXPECT_NE(buffer, zeroes);
}

TEST(fill_two_calls_differ) {
  std::array<std::uint8_t, 32> first{};
  std::array<std::uint8_t, 32> second{};
  sourcemeta::core::random_bytes(first,
                                 sourcemeta::core::system_random_generator);
  sourcemeta::core::random_bytes(second,
                                 sourcemeta::core::system_random_generator);
  EXPECT_NE(first, second);
}

TEST(fill_empty_span_is_noop) {
  sourcemeta::core::random_bytes(std::span<std::uint8_t>{},
                                 sourcemeta::core::system_random_generator);
  EXPECT_TRUE(true);
}

TEST(fill_respects_span_bounds) {
  std::array<std::uint8_t, 8> buffer{};
  sourcemeta::core::random_bytes(std::span<std::uint8_t>{buffer.data(), 4},
                                 sourcemeta::core::system_random_generator);
  // The bytes past the requested span must remain untouched
  EXPECT_EQ(buffer[4], 0);
  EXPECT_EQ(buffer[5], 0);
  EXPECT_EQ(buffer[6], 0);
  EXPECT_EQ(buffer[7], 0);
}
