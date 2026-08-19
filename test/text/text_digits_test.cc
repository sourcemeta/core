#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <array>   // std::array
#include <cstdint> // std::int64_t, std::uint64_t
#include <sstream> // std::ostringstream
#include <string>  // std::string
#include <tuple>   // std::tuple_size_v

namespace {

// Each capacity is exactly the length of the longest spelling of its type:
// "255", "-128", "4294967295", "-2147483648", "18446744073709551615" and
// "-9223372036854775808" respectively
static_assert(sourcemeta::core::DIGITS_CAPACITY<std::uint8_t> == 3);
static_assert(sourcemeta::core::DIGITS_CAPACITY<std::int8_t> == 4);
static_assert(sourcemeta::core::DIGITS_CAPACITY<std::uint32_t> == 10);
static_assert(sourcemeta::core::DIGITS_CAPACITY<std::int32_t> == 11);
static_assert(sourcemeta::core::DIGITS_CAPACITY<std::uint64_t> == 20);
static_assert(sourcemeta::core::DIGITS_CAPACITY<std::int64_t> == 20);
static_assert(std::tuple_size_v<sourcemeta::core::DigitsBuffer> == 20);

} // namespace

TEST(digits_view_accepts_a_buffer_sized_for_a_narrower_type) {
  std::array<char, sourcemeta::core::DIGITS_CAPACITY<std::uint8_t>> buffer;
  EXPECT_EQ(sourcemeta::core::digits_view(std::uint8_t{255}, buffer), "255");
}

TEST(digits_view_zero) {
  sourcemeta::core::DigitsBuffer buffer;
  EXPECT_EQ(sourcemeta::core::digits_view(0, buffer), "0");
}

TEST(digits_view_positive) {
  sourcemeta::core::DigitsBuffer buffer;
  EXPECT_EQ(sourcemeta::core::digits_view(1234, buffer), "1234");
}

TEST(digits_view_negative) {
  sourcemeta::core::DigitsBuffer buffer;
  EXPECT_EQ(sourcemeta::core::digits_view(-1234, buffer), "-1234");
}

TEST(digits_view_signed_minimum) {
  sourcemeta::core::DigitsBuffer buffer;
  EXPECT_EQ(sourcemeta::core::digits_view(
                std::int64_t{-9223372036854775807LL - 1}, buffer),
            "-9223372036854775808");
}

TEST(digits_view_unsigned_maximum) {
  sourcemeta::core::DigitsBuffer buffer;
  EXPECT_EQ(sourcemeta::core::digits_view(
                std::uint64_t{18446744073709551615ULL}, buffer),
            "18446744073709551615");
}

TEST(digits_view_reuses_the_buffer) {
  sourcemeta::core::DigitsBuffer buffer;
  EXPECT_EQ(sourcemeta::core::digits_view(999, buffer), "999");
  EXPECT_EQ(sourcemeta::core::digits_view(11, buffer), "11");
}

TEST(digits_append_to_an_empty_string) {
  std::string result;
  sourcemeta::core::digits_append(result, 42);
  EXPECT_EQ(result, "42");
}

TEST(digits_append_preserves_existing_content) {
  std::string result{"port="};
  sourcemeta::core::digits_append(result, 8080);
  EXPECT_EQ(result, "port=8080");
}

TEST(digits_append_negative) {
  std::string result{"offset="};
  sourcemeta::core::digits_append(result, -7);
  EXPECT_EQ(result, "offset=-7");
}

TEST(digits_write_to_a_stream) {
  std::ostringstream stream;
  sourcemeta::core::digits_write(stream, 1234);
  EXPECT_EQ(stream.str(), "1234");
}

TEST(digits_write_negative) {
  std::ostringstream stream;
  sourcemeta::core::digits_write(stream, -1234);
  EXPECT_EQ(stream.str(), "-1234");
}

// The stream insertion operator pads to the stream width using its fill
// character and then clears the width. An unformatted write does neither, so a
// stream left with a width is enough to tell the two paths apart without
// depending on any locale
TEST(digits_write_bypasses_stream_formatting) {
  std::ostringstream probe;
  probe.width(10);
  probe.fill('*');
  probe << 42;
  EXPECT_EQ(probe.str(), "********42");

  std::ostringstream stream;
  stream.width(10);
  stream.fill('*');
  sourcemeta::core::digits_write(stream, 42);
  EXPECT_EQ(stream.str(), "42");
  EXPECT_EQ(stream.width(), 10);
}
