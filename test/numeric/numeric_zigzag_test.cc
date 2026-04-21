#include <gtest/gtest.h>

#include <sourcemeta/core/numeric.h>

#include <cstdint> // std::int64_t, std::uint64_t
#include <limits>  // std::numeric_limits

TEST(Numeric_zigzag, encode_int_0_to_0) {
  EXPECT_EQ(sourcemeta::core::zigzag_encode(0), std::uint64_t{0});
}

TEST(Numeric_zigzag, encode_int_minus_1_to_1) {
  EXPECT_EQ(sourcemeta::core::zigzag_encode(-1), std::uint64_t{1});
}

TEST(Numeric_zigzag, encode_int_1_to_2) {
  EXPECT_EQ(sourcemeta::core::zigzag_encode(1), std::uint64_t{2});
}

TEST(Numeric_zigzag, encode_int_minus_2_to_3) {
  EXPECT_EQ(sourcemeta::core::zigzag_encode(-2), std::uint64_t{3});
}

TEST(Numeric_zigzag, encode_int64_0_to_0) {
  EXPECT_EQ(sourcemeta::core::zigzag_encode(std::int64_t{0}), std::uint64_t{0});
}

TEST(Numeric_zigzag, encode_int64_minus_1_to_1) {
  EXPECT_EQ(sourcemeta::core::zigzag_encode(std::int64_t{-1}),
            std::uint64_t{1});
}

TEST(Numeric_zigzag, encode_int64_1_to_2) {
  EXPECT_EQ(sourcemeta::core::zigzag_encode(std::int64_t{1}), std::uint64_t{2});
}

TEST(Numeric_zigzag, encode_int64_minus_2_to_3) {
  EXPECT_EQ(sourcemeta::core::zigzag_encode(std::int64_t{-2}),
            std::uint64_t{3});
}

TEST(Numeric_zigzag, encode_int64_max) {
  const std::int64_t value{std::numeric_limits<std::int64_t>::max()};
  EXPECT_EQ(sourcemeta::core::zigzag_encode(value),
            std::uint64_t{18446744073709551614U});
}

TEST(Numeric_zigzag, encode_int64_min_plus_one) {
  const std::int64_t value{std::numeric_limits<std::int64_t>::min() + 1};
  EXPECT_EQ(sourcemeta::core::zigzag_encode(value),
            std::uint64_t{18446744073709551613U});
}

TEST(Numeric_zigzag, encode_int64_min) {
  EXPECT_EQ(
      sourcemeta::core::zigzag_encode(std::numeric_limits<std::int64_t>::min()),
      std::numeric_limits<std::uint64_t>::max());
}

TEST(Numeric_zigzag, decode_int_0_to_0) {
  EXPECT_EQ(sourcemeta::core::zigzag_decode(0U), std::int64_t{0});
}

TEST(Numeric_zigzag, decode_int_1_to_minus_1) {
  EXPECT_EQ(sourcemeta::core::zigzag_decode(1U), std::int64_t{-1});
}

TEST(Numeric_zigzag, decode_int_2_to_1) {
  EXPECT_EQ(sourcemeta::core::zigzag_decode(2U), std::int64_t{1});
}

TEST(Numeric_zigzag, decode_int_3_to_minus_2) {
  EXPECT_EQ(sourcemeta::core::zigzag_decode(3U), std::int64_t{-2});
}

TEST(Numeric_zigzag, decode_int64_0_to_0) {
  EXPECT_EQ(sourcemeta::core::zigzag_decode(std::uint64_t{0}), std::int64_t{0});
}

TEST(Numeric_zigzag, decode_int64_1_to_minus_1) {
  EXPECT_EQ(sourcemeta::core::zigzag_decode(std::uint64_t{1}),
            std::int64_t{-1});
}

TEST(Numeric_zigzag, decode_int64_2_to_1) {
  EXPECT_EQ(sourcemeta::core::zigzag_decode(std::uint64_t{2}), std::int64_t{1});
}

TEST(Numeric_zigzag, decode_int64_3_to_minus_2) {
  EXPECT_EQ(sourcemeta::core::zigzag_decode(std::uint64_t{3}),
            std::int64_t{-2});
}

TEST(Numeric_zigzag, decode_int64_max_encoded) {
  EXPECT_EQ(
      sourcemeta::core::zigzag_decode(std::uint64_t{18446744073709551614U}),
      std::numeric_limits<std::int64_t>::max());
}

TEST(Numeric_zigzag, decode_int64_min_plus_one_encoded) {
  const std::int64_t expected{std::numeric_limits<std::int64_t>::min() + 1};
  EXPECT_EQ(
      sourcemeta::core::zigzag_decode(std::uint64_t{18446744073709551613U}),
      expected);
}

TEST(Numeric_zigzag, decode_uint64_max_to_int64_min) {
  EXPECT_EQ(sourcemeta::core::zigzag_decode(
                std::numeric_limits<std::uint64_t>::max()),
            std::numeric_limits<std::int64_t>::min());
}

TEST(Numeric_zigzag, encode_decimal_0_to_0) {
  EXPECT_EQ(sourcemeta::core::zigzag_encode(sourcemeta::core::Decimal{0}),
            sourcemeta::core::Decimal{0});
}

TEST(Numeric_zigzag, encode_decimal_minus_1_to_1) {
  EXPECT_EQ(sourcemeta::core::zigzag_encode(sourcemeta::core::Decimal{-1}),
            sourcemeta::core::Decimal{1});
}

TEST(Numeric_zigzag, encode_decimal_1_to_2) {
  EXPECT_EQ(sourcemeta::core::zigzag_encode(sourcemeta::core::Decimal{1}),
            sourcemeta::core::Decimal{2});
}

TEST(Numeric_zigzag, encode_decimal_minus_2_to_3) {
  EXPECT_EQ(sourcemeta::core::zigzag_encode(sourcemeta::core::Decimal{-2}),
            sourcemeta::core::Decimal{3});
}

TEST(Numeric_zigzag, encode_decimal_large_positive) {
  EXPECT_EQ(sourcemeta::core::zigzag_encode(sourcemeta::core::Decimal{1000000}),
            sourcemeta::core::Decimal{2000000});
}

TEST(Numeric_zigzag, decode_decimal_0_to_0) {
  EXPECT_EQ(sourcemeta::core::zigzag_decode(sourcemeta::core::Decimal{0}),
            sourcemeta::core::Decimal{0});
}

TEST(Numeric_zigzag, decode_decimal_1_to_minus_1) {
  EXPECT_EQ(sourcemeta::core::zigzag_decode(sourcemeta::core::Decimal{1}),
            sourcemeta::core::Decimal{-1});
}

TEST(Numeric_zigzag, decode_decimal_2_to_1) {
  EXPECT_EQ(sourcemeta::core::zigzag_decode(sourcemeta::core::Decimal{2}),
            sourcemeta::core::Decimal{1});
}

TEST(Numeric_zigzag, decode_decimal_3_to_minus_2) {
  EXPECT_EQ(sourcemeta::core::zigzag_decode(sourcemeta::core::Decimal{3}),
            sourcemeta::core::Decimal{-2});
}

TEST(Numeric_zigzag, decode_decimal_large_even) {
  EXPECT_EQ(sourcemeta::core::zigzag_decode(sourcemeta::core::Decimal{2000000}),
            sourcemeta::core::Decimal{1000000});
}
