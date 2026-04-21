#include <gtest/gtest.h>

#include <sourcemeta/core/numeric.h>

#include <cstdint> // std::int64_t, std::uint64_t, std::uint8_t
#include <limits>  // std::numeric_limits

TEST(Numeric_util, is_byte_zero) { EXPECT_TRUE(sourcemeta::core::is_byte(0)); }

TEST(Numeric_util, is_byte_255) { EXPECT_TRUE(sourcemeta::core::is_byte(255)); }

TEST(Numeric_util, is_byte_256) {
  EXPECT_FALSE(sourcemeta::core::is_byte(256));
}

TEST(Numeric_util, is_byte_negative) {
  EXPECT_FALSE(sourcemeta::core::is_byte(-1));
}

TEST(Numeric_util, is_byte_decimal_zero) {
  EXPECT_TRUE(sourcemeta::core::is_byte(sourcemeta::core::Decimal{0}));
}

TEST(Numeric_util, is_byte_decimal_255) {
  EXPECT_TRUE(sourcemeta::core::is_byte(sourcemeta::core::Decimal{255}));
}

TEST(Numeric_util, is_byte_decimal_256) {
  EXPECT_FALSE(sourcemeta::core::is_byte(sourcemeta::core::Decimal{256}));
}

TEST(Numeric_util, is_byte_decimal_non_integral) {
  EXPECT_FALSE(
      sourcemeta::core::is_byte(sourcemeta::core::Decimal{std::string{"1.5"}}));
}

TEST(Numeric_util, uint_max_2) { EXPECT_EQ(sourcemeta::core::uint_max<2>, 3); }

TEST(Numeric_util, uint_max_3) { EXPECT_EQ(sourcemeta::core::uint_max<3>, 7); }

TEST(Numeric_util, uint_max_5) { EXPECT_EQ(sourcemeta::core::uint_max<5>, 31); }

TEST(Numeric_util, uint_max_8) {
  EXPECT_EQ(sourcemeta::core::uint_max<8>,
            std::numeric_limits<std::uint8_t>::max());
}

TEST(Numeric_util, uint_max_16) {
  EXPECT_EQ(sourcemeta::core::uint_max<16>,
            std::numeric_limits<std::uint16_t>::max());
}

TEST(Numeric_util, uint_max_32) {
  EXPECT_EQ(sourcemeta::core::uint_max<32>,
            std::numeric_limits<std::uint32_t>::max());
}

TEST(Numeric_util, is_within_in_range) {
  EXPECT_TRUE(
      sourcemeta::core::is_within(5, std::int64_t{1}, std::int64_t{10}));
}

TEST(Numeric_util, is_within_at_lower_bound) {
  EXPECT_TRUE(
      sourcemeta::core::is_within(1, std::int64_t{1}, std::int64_t{10}));
}

TEST(Numeric_util, is_within_at_upper_bound) {
  EXPECT_TRUE(
      sourcemeta::core::is_within(10, std::int64_t{1}, std::int64_t{10}));
}

TEST(Numeric_util, is_within_below_range) {
  EXPECT_FALSE(
      sourcemeta::core::is_within(0, std::int64_t{1}, std::int64_t{10}));
}

TEST(Numeric_util, is_within_above_range) {
  EXPECT_FALSE(
      sourcemeta::core::is_within(11, std::int64_t{1}, std::int64_t{10}));
}

TEST(Numeric_util, is_within_unsigned_in_range) {
  EXPECT_TRUE(
      sourcemeta::core::is_within(5, std::uint64_t{1}, std::uint64_t{10}));
}

TEST(Numeric_util, is_within_unsigned_negative_value) {
  EXPECT_FALSE(
      sourcemeta::core::is_within(-1, std::uint64_t{0}, std::uint64_t{10}));
}

TEST(Numeric_util, is_within_decimal_in_range) {
  EXPECT_TRUE(sourcemeta::core::is_within(sourcemeta::core::Decimal{5},
                                          sourcemeta::core::Decimal{1},
                                          sourcemeta::core::Decimal{10}));
}

TEST(Numeric_util, is_within_decimal_below) {
  EXPECT_FALSE(sourcemeta::core::is_within(sourcemeta::core::Decimal{0},
                                           sourcemeta::core::Decimal{1},
                                           sourcemeta::core::Decimal{10}));
}

TEST(Numeric_util, is_within_decimal_above) {
  EXPECT_FALSE(sourcemeta::core::is_within(sourcemeta::core::Decimal{11},
                                           sourcemeta::core::Decimal{1},
                                           sourcemeta::core::Decimal{10}));
}

TEST(Numeric_util, abs_positive) {
  EXPECT_EQ(sourcemeta::core::abs(std::int64_t{42}), std::uint64_t{42});
}

TEST(Numeric_util, abs_zero) {
  EXPECT_EQ(sourcemeta::core::abs(std::int64_t{0}), std::uint64_t{0});
}

TEST(Numeric_util, abs_negative) {
  EXPECT_EQ(sourcemeta::core::abs(std::int64_t{-42}), std::uint64_t{42});
}

TEST(Numeric_util, abs_int64_min_plus_one) {
  EXPECT_EQ(
      sourcemeta::core::abs(std::numeric_limits<std::int64_t>::min() + 1),
      static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()));
}

TEST(Numeric_util, abs_int64_min) {
  EXPECT_EQ(sourcemeta::core::abs(std::numeric_limits<std::int64_t>::min()),
            static_cast<std::uint64_t>(
                std::numeric_limits<std::int64_t>::max()) +
                1);
}

TEST(Numeric_util, abs_decimal_positive) {
  EXPECT_EQ(sourcemeta::core::abs(sourcemeta::core::Decimal{42}),
            sourcemeta::core::Decimal{42});
}

TEST(Numeric_util, abs_decimal_zero) {
  EXPECT_EQ(sourcemeta::core::abs(sourcemeta::core::Decimal{0}),
            sourcemeta::core::Decimal{0});
}

TEST(Numeric_util, abs_decimal_negative) {
  EXPECT_EQ(sourcemeta::core::abs(sourcemeta::core::Decimal{-42}),
            sourcemeta::core::Decimal{42});
}

TEST(Numeric_util, divide_ceil_simple_positive) {
  EXPECT_EQ(sourcemeta::core::divide_ceil(std::int64_t{10}, std::uint64_t{5}),
            std::int64_t{2});
}

TEST(Numeric_util, divide_ceil_simple_negative) {
  EXPECT_EQ(sourcemeta::core::divide_ceil(std::int64_t{-10}, std::uint64_t{5}),
            std::int64_t{-2});
}

TEST(Numeric_util, divide_ceil_small_positive) {
  EXPECT_EQ(sourcemeta::core::divide_ceil(std::int64_t{11}, std::uint64_t{5}),
            std::int64_t{3});
}

TEST(Numeric_util, divide_ceil_small_negative) {
  EXPECT_EQ(sourcemeta::core::divide_ceil(std::int64_t{-11}, std::uint64_t{5}),
            std::int64_t{-2});
}

TEST(Numeric_util, divide_ceil_positive_large_divisor) {
  EXPECT_EQ(sourcemeta::core::divide_ceil(std::int64_t{11}, std::uint64_t{20}),
            std::int64_t{1});
}

TEST(Numeric_util, divide_ceil_negative_large_divisor) {
  EXPECT_EQ(sourcemeta::core::divide_ceil(std::int64_t{-11}, std::uint64_t{20}),
            std::int64_t{0});
}

TEST(Numeric_util, divide_ceil_max_dividend_min_divisor) {
  EXPECT_EQ(sourcemeta::core::divide_ceil(
                std::numeric_limits<std::int64_t>::max(), std::uint64_t{1}),
            std::numeric_limits<std::int64_t>::max());
}

TEST(Numeric_util, divide_ceil_min_dividend_min_divisor) {
  EXPECT_EQ(sourcemeta::core::divide_ceil(
                std::numeric_limits<std::int64_t>::min(), std::uint64_t{1}),
            std::numeric_limits<std::int64_t>::min());
}

TEST(Numeric_util, divide_ceil_int64_min_by_2) {
  EXPECT_EQ(sourcemeta::core::divide_ceil(
                std::numeric_limits<std::int64_t>::min(), std::uint64_t{2}),
            std::int64_t{-4611686018427387904});
}

TEST(Numeric_util, divide_ceil_max_dividend_max_divisor) {
  EXPECT_EQ(
      sourcemeta::core::divide_ceil(std::numeric_limits<std::int64_t>::max(),
                                    std::numeric_limits<std::uint64_t>::max()),
      std::int64_t{1});
}

TEST(Numeric_util, divide_ceil_min_dividend_max_divisor) {
  EXPECT_EQ(
      sourcemeta::core::divide_ceil(std::numeric_limits<std::int64_t>::min(),
                                    std::numeric_limits<std::uint64_t>::max()),
      std::int64_t{0});
}

TEST(Numeric_util, divide_ceil_decimal_simple_positive) {
  EXPECT_EQ(sourcemeta::core::divide_ceil(sourcemeta::core::Decimal{10},
                                          sourcemeta::core::Decimal{5}),
            sourcemeta::core::Decimal{2});
}

TEST(Numeric_util, divide_ceil_decimal_simple_negative) {
  EXPECT_EQ(sourcemeta::core::divide_ceil(sourcemeta::core::Decimal{-10},
                                          sourcemeta::core::Decimal{5}),
            sourcemeta::core::Decimal{-2});
}

TEST(Numeric_util, divide_ceil_decimal_inexact_positive) {
  EXPECT_EQ(sourcemeta::core::divide_ceil(sourcemeta::core::Decimal{11},
                                          sourcemeta::core::Decimal{5}),
            sourcemeta::core::Decimal{3});
}

TEST(Numeric_util, divide_ceil_decimal_inexact_negative) {
  EXPECT_EQ(sourcemeta::core::divide_ceil(sourcemeta::core::Decimal{-11},
                                          sourcemeta::core::Decimal{5}),
            sourcemeta::core::Decimal{-2});
}

TEST(Numeric_util, divide_ceil_mixed_decimal_and_int) {
  EXPECT_EQ(sourcemeta::core::divide_ceil(sourcemeta::core::Decimal{11},
                                          std::uint64_t{5}),
            sourcemeta::core::Decimal{3});
}

TEST(Numeric_util, divide_ceil_mixed_int_and_decimal) {
  EXPECT_EQ(sourcemeta::core::divide_ceil(std::int64_t{11},
                                          sourcemeta::core::Decimal{5}),
            sourcemeta::core::Decimal{3});
}

TEST(Numeric_util, divide_floor_simple_positive) {
  EXPECT_EQ(sourcemeta::core::divide_floor(std::int64_t{10}, std::uint64_t{5}),
            std::int64_t{2});
}

TEST(Numeric_util, divide_floor_simple_negative) {
  EXPECT_EQ(sourcemeta::core::divide_floor(std::int64_t{-10}, std::uint64_t{5}),
            std::int64_t{-2});
}

TEST(Numeric_util, divide_floor_small_positive) {
  EXPECT_EQ(sourcemeta::core::divide_floor(std::int64_t{11}, std::uint64_t{5}),
            std::int64_t{2});
}

TEST(Numeric_util, divide_floor_small_negative) {
  EXPECT_EQ(sourcemeta::core::divide_floor(std::int64_t{-11}, std::uint64_t{5}),
            std::int64_t{-3});
}

TEST(Numeric_util, divide_floor_positive_large_divisor) {
  EXPECT_EQ(sourcemeta::core::divide_floor(std::int64_t{11}, std::uint64_t{20}),
            std::int64_t{0});
}

TEST(Numeric_util, divide_floor_negative_large_divisor) {
  EXPECT_EQ(
      sourcemeta::core::divide_floor(std::int64_t{-11}, std::uint64_t{20}),
      std::int64_t{-1});
}

TEST(Numeric_util, divide_floor_max_dividend_min_divisor) {
  EXPECT_EQ(sourcemeta::core::divide_floor(
                std::numeric_limits<std::int64_t>::max(), std::uint64_t{1}),
            std::numeric_limits<std::int64_t>::max());
}

TEST(Numeric_util, divide_floor_min_dividend_min_divisor) {
  EXPECT_EQ(sourcemeta::core::divide_floor(
                std::numeric_limits<std::int64_t>::min(), std::uint64_t{1}),
            std::numeric_limits<std::int64_t>::min());
}

TEST(Numeric_util, divide_floor_int64_min_by_2) {
  EXPECT_EQ(sourcemeta::core::divide_floor(
                std::numeric_limits<std::int64_t>::min(), std::uint64_t{2}),
            std::int64_t{-4611686018427387904});
}

TEST(Numeric_util, divide_floor_max_dividend_max_divisor) {
  EXPECT_EQ(
      sourcemeta::core::divide_floor(std::numeric_limits<std::int64_t>::max(),
                                     std::numeric_limits<std::uint64_t>::max()),
      std::int64_t{0});
}

TEST(Numeric_util, divide_floor_min_dividend_max_divisor) {
  EXPECT_EQ(
      sourcemeta::core::divide_floor(std::numeric_limits<std::int64_t>::min(),
                                     std::numeric_limits<std::uint64_t>::max()),
      std::int64_t{-1});
}

TEST(Numeric_util, divide_floor_decimal_simple_positive) {
  EXPECT_EQ(sourcemeta::core::divide_floor(sourcemeta::core::Decimal{10},
                                           sourcemeta::core::Decimal{5}),
            sourcemeta::core::Decimal{2});
}

TEST(Numeric_util, divide_floor_decimal_simple_negative) {
  EXPECT_EQ(sourcemeta::core::divide_floor(sourcemeta::core::Decimal{-10},
                                           sourcemeta::core::Decimal{5}),
            sourcemeta::core::Decimal{-2});
}

TEST(Numeric_util, divide_floor_decimal_inexact_positive) {
  EXPECT_EQ(sourcemeta::core::divide_floor(sourcemeta::core::Decimal{11},
                                           sourcemeta::core::Decimal{5}),
            sourcemeta::core::Decimal{2});
}

TEST(Numeric_util, divide_floor_decimal_inexact_negative) {
  EXPECT_EQ(sourcemeta::core::divide_floor(sourcemeta::core::Decimal{-11},
                                           sourcemeta::core::Decimal{5}),
            sourcemeta::core::Decimal{-3});
}

TEST(Numeric_util, divide_floor_mixed_decimal_and_int) {
  EXPECT_EQ(sourcemeta::core::divide_floor(sourcemeta::core::Decimal{11},
                                           std::uint64_t{5}),
            sourcemeta::core::Decimal{2});
}

TEST(Numeric_util, count_multiples_positive_range) {
  EXPECT_EQ(sourcemeta::core::count_multiples(std::int64_t{1}, std::int64_t{10},
                                              std::int64_t{3}),
            std::uint64_t{3});
}

TEST(Numeric_util, count_multiples_single_element) {
  EXPECT_EQ(sourcemeta::core::count_multiples(std::int64_t{3}, std::int64_t{3},
                                              std::int64_t{3}),
            std::uint64_t{1});
}

TEST(Numeric_util, count_multiples_negative_range) {
  // Multiples of 3 in [-9, -1]: -9, -6, -3 = 3
  EXPECT_EQ(sourcemeta::core::count_multiples(
                std::int64_t{-9}, std::int64_t{-1}, std::int64_t{3}),
            std::uint64_t{3});
}

TEST(Numeric_util, count_multiples_mixed_range) {
  // Multiples of 3 in [-7, 5]: -6, -3, 0, 3 = 4
  EXPECT_EQ(sourcemeta::core::count_multiples(std::int64_t{-7}, std::int64_t{5},
                                              std::int64_t{3}),
            std::uint64_t{4});
}

TEST(Numeric_util, count_multiples_decimal_positive_range) {
  EXPECT_EQ(sourcemeta::core::count_multiples(sourcemeta::core::Decimal{1},
                                              sourcemeta::core::Decimal{10},
                                              sourcemeta::core::Decimal{3}),
            sourcemeta::core::Decimal{3});
}

TEST(Numeric_util, count_multiples_decimal_negative_range) {
  EXPECT_EQ(sourcemeta::core::count_multiples(sourcemeta::core::Decimal{-9},
                                              sourcemeta::core::Decimal{-1},
                                              sourcemeta::core::Decimal{3}),
            sourcemeta::core::Decimal{3});
}

TEST(Numeric_util, count_multiples_decimal_mixed_range) {
  EXPECT_EQ(sourcemeta::core::count_multiples(sourcemeta::core::Decimal{-7},
                                              sourcemeta::core::Decimal{5},
                                              sourcemeta::core::Decimal{3}),
            sourcemeta::core::Decimal{4});
}

TEST(Numeric_util, closest_smallest_exponent_2_2_1_2) {
  EXPECT_EQ(sourcemeta::core::closest_smallest_exponent(2, 2, 1, 2), 1);
}

TEST(Numeric_util, closest_smallest_exponent_20_2_1_6) {
  EXPECT_EQ(sourcemeta::core::closest_smallest_exponent(20, 2, 1, 6), 4);
}

TEST(Numeric_util, closest_smallest_exponent_20_3_1_6) {
  EXPECT_EQ(sourcemeta::core::closest_smallest_exponent(20, 3, 1, 6), 2);
}

TEST(Numeric_util, correct_ieee754_snap_up_double) {
  EXPECT_DOUBLE_EQ(sourcemeta::core::correct_ieee754(2.9999999999), 3.0);
}

TEST(Numeric_util, correct_ieee754_snap_down_double) {
  EXPECT_DOUBLE_EQ(sourcemeta::core::correct_ieee754(3.0000000001), 3.0);
}

TEST(Numeric_util, correct_ieee754_no_correction_double) {
  EXPECT_DOUBLE_EQ(sourcemeta::core::correct_ieee754(2.5), 2.5);
}

TEST(Numeric_util, correct_ieee754_snap_up_float) {
  EXPECT_FLOAT_EQ(sourcemeta::core::correct_ieee754(2.9999999999f), 3.0f);
}

TEST(Numeric_util, correct_ieee754_snap_down_float) {
  EXPECT_FLOAT_EQ(sourcemeta::core::correct_ieee754(3.0000000001f), 3.0f);
}

TEST(Numeric_util, correct_ieee754_no_correction_float) {
  EXPECT_FLOAT_EQ(sourcemeta::core::correct_ieee754(2.5f), 2.5f);
}

TEST(Numeric_util, real_digits_whole_number) {
  std::uint64_t position{0};
  const auto digits{sourcemeta::core::real_digits<std::int64_t>(5.0, position)};
  EXPECT_EQ(digits, 5);
  EXPECT_EQ(position, 0);
}

TEST(Numeric_util, real_digits_simple_decimal) {
  std::uint64_t position{0};
  const auto digits{
      sourcemeta::core::real_digits<std::int64_t>(3.14, position)};
  EXPECT_EQ(digits, 314);
  EXPECT_EQ(position, 2);
}

TEST(Numeric_util, real_digits_small_fractional_part) {
  std::uint64_t position{0};
  const auto digits{sourcemeta::core::real_digits<std::int64_t>(1.5, position)};
  EXPECT_EQ(digits, 15);
  EXPECT_EQ(position, 1);
}
