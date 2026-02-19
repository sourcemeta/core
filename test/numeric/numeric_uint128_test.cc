#include <gtest/gtest.h>

#include <sourcemeta/core/numeric.h>

#include <cstdint> // std::uint64_t, std::int64_t

TEST(Numeric_uint128, default_construction) {
  const sourcemeta::core::uint128_t value{};
  EXPECT_EQ(static_cast<std::uint64_t>(value), 0);
}

TEST(Numeric_uint128, construct_from_int) {
  const sourcemeta::core::uint128_t value{42};
  EXPECT_EQ(static_cast<std::uint64_t>(value), 42);
}

TEST(Numeric_uint128, construct_from_unsigned_int) {
  const sourcemeta::core::uint128_t value{42u};
  EXPECT_EQ(static_cast<std::uint64_t>(value), 42);
}

TEST(Numeric_uint128, construct_from_uint64) {
  const sourcemeta::core::uint128_t value{
      static_cast<std::uint64_t>(1000000000000ULL)};
  EXPECT_EQ(static_cast<std::uint64_t>(value), 1000000000000ULL);
}

TEST(Numeric_uint128, construct_from_int64) {
  const sourcemeta::core::uint128_t value{static_cast<std::int64_t>(99)};
  EXPECT_EQ(static_cast<std::int64_t>(value), 99);
}

TEST(Numeric_uint128, explicit_cast_to_uint64) {
  const sourcemeta::core::uint128_t value{12345ULL};
  EXPECT_EQ(static_cast<std::uint64_t>(value), 12345ULL);
}

TEST(Numeric_uint128, explicit_cast_to_int64) {
  const sourcemeta::core::uint128_t value{789};
  EXPECT_EQ(static_cast<std::int64_t>(value), 789);
}

TEST(Numeric_uint128, explicit_cast_to_bool_zero) {
  const sourcemeta::core::uint128_t value{0};
  EXPECT_FALSE(static_cast<bool>(value));
}

TEST(Numeric_uint128, explicit_cast_to_bool_nonzero) {
  const sourcemeta::core::uint128_t value{1};
  EXPECT_TRUE(static_cast<bool>(value));
}

TEST(Numeric_uint128, addition) {
  const sourcemeta::core::uint128_t left{100};
  const sourcemeta::core::uint128_t right{200};
  const auto result = left + right;
  EXPECT_EQ(static_cast<std::uint64_t>(result), 300);
}

TEST(Numeric_uint128, addition_assignment) {
  sourcemeta::core::uint128_t value{100};
  value += sourcemeta::core::uint128_t{50};
  EXPECT_EQ(static_cast<std::uint64_t>(value), 150);
}

TEST(Numeric_uint128, multiplication) {
  const sourcemeta::core::uint128_t left{1000};
  const sourcemeta::core::uint128_t right{2000};
  const auto result = left * right;
  EXPECT_EQ(static_cast<std::uint64_t>(result), 2000000);
}

TEST(Numeric_uint128, multiplication_assignment) {
  sourcemeta::core::uint128_t value{500};
  value *= sourcemeta::core::uint128_t{3};
  EXPECT_EQ(static_cast<std::uint64_t>(value), 1500);
}

TEST(Numeric_uint128, division_by_uint64) {
  const sourcemeta::core::uint128_t dividend{100};
  const auto result = dividend / static_cast<std::uint64_t>(10);
  EXPECT_EQ(static_cast<std::uint64_t>(result), 10);
}

TEST(Numeric_uint128, modulo_by_uint64) {
  const sourcemeta::core::uint128_t dividend{17};
  const auto result = dividend % static_cast<std::uint64_t>(5);
  EXPECT_EQ(static_cast<std::uint64_t>(result), 2);
}

TEST(Numeric_uint128, equal) {
  const sourcemeta::core::uint128_t left{42};
  const sourcemeta::core::uint128_t right{42};
  EXPECT_TRUE(left == right);
}

TEST(Numeric_uint128, not_equal) {
  const sourcemeta::core::uint128_t left{42};
  const sourcemeta::core::uint128_t right{43};
  EXPECT_TRUE(left != right);
}

TEST(Numeric_uint128, less_than) {
  const sourcemeta::core::uint128_t left{10};
  const sourcemeta::core::uint128_t right{20};
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(Numeric_uint128, greater_than) {
  const sourcemeta::core::uint128_t left{20};
  const sourcemeta::core::uint128_t right{10};
  EXPECT_TRUE(left > right);
  EXPECT_FALSE(right > left);
}

TEST(Numeric_uint128, less_than_or_equal) {
  const sourcemeta::core::uint128_t left{10};
  const sourcemeta::core::uint128_t right{10};
  const sourcemeta::core::uint128_t bigger{20};
  EXPECT_TRUE(left <= right);
  EXPECT_TRUE(left <= bigger);
  EXPECT_FALSE(bigger <= left);
}

TEST(Numeric_uint128, greater_than_or_equal) {
  const sourcemeta::core::uint128_t left{10};
  const sourcemeta::core::uint128_t right{10};
  const sourcemeta::core::uint128_t smaller{5};
  EXPECT_TRUE(left >= right);
  EXPECT_TRUE(left >= smaller);
  EXPECT_FALSE(smaller >= left);
}

TEST(Numeric_uint128, addition_overflow_into_high_bits) {
  const sourcemeta::core::uint128_t max_low{UINT64_MAX};
  const sourcemeta::core::uint128_t one{1};
  const auto result = max_low + one;
  // UINT64_MAX + 1 should overflow low and set high to 1
  EXPECT_EQ(static_cast<std::uint64_t>(result), 0);
  EXPECT_TRUE(static_cast<bool>(result));
}

TEST(Numeric_uint128, large_multiplication_roundtrip) {
  // 10^18 * 5 = 5 * 10^18, which fits in 64 bits
  const sourcemeta::core::uint128_t base{1000000000000000000ULL};
  const sourcemeta::core::uint128_t factor{5};
  const auto product = base * factor;
  EXPECT_EQ(static_cast<std::uint64_t>(product), 5000000000000000000ULL);
}

TEST(Numeric_uint128, large_multiply_then_divide) {
  // (10^18 * 7) / 7 should give back 10^18
  const sourcemeta::core::uint128_t base{1000000000000000000ULL};
  const sourcemeta::core::uint128_t factor{7};
  const auto product = base * factor;
  const auto back = product / static_cast<std::uint64_t>(7);
  EXPECT_EQ(static_cast<std::uint64_t>(back), 1000000000000000000ULL);
}

TEST(Numeric_uint128, modulo_large_value) {
  // 10^18 % 7
  const sourcemeta::core::uint128_t value{1000000000000000000ULL};
  const auto result = value % static_cast<std::uint64_t>(7);
  EXPECT_EQ(static_cast<std::uint64_t>(result), 1000000000000000000ULL % 7ULL);
}

TEST(Numeric_uint128, overflow_multiply_then_divide) {
  // UINT64_MAX * 3 overflows 64 bits, then divide by 3 should recover
  const sourcemeta::core::uint128_t large{UINT64_MAX};
  const sourcemeta::core::uint128_t three{3};
  const auto product = large * three;
  const auto back = product / static_cast<std::uint64_t>(3);
  EXPECT_EQ(static_cast<std::uint64_t>(back), UINT64_MAX);
}

TEST(Numeric_uint128, addition_commutativity) {
  const sourcemeta::core::uint128_t left{123456789ULL};
  const sourcemeta::core::uint128_t right{987654321ULL};
  EXPECT_TRUE((left + right) == (right + left));
}

TEST(Numeric_uint128, multiplication_commutativity) {
  const sourcemeta::core::uint128_t left{12345ULL};
  const sourcemeta::core::uint128_t right{67890ULL};
  EXPECT_TRUE((left * right) == (right * left));
}

TEST(Numeric_uint128, multiply_by_zero) {
  const sourcemeta::core::uint128_t value{999999ULL};
  const sourcemeta::core::uint128_t zero{0};
  const auto result = value * zero;
  EXPECT_EQ(static_cast<std::uint64_t>(result), 0);
  EXPECT_FALSE(static_cast<bool>(result));
}

TEST(Numeric_uint128, add_zero) {
  const sourcemeta::core::uint128_t value{42};
  const sourcemeta::core::uint128_t zero{0};
  EXPECT_TRUE((value + zero) == value);
}

TEST(Numeric_uint128, division_by_uint128) {
  const sourcemeta::core::uint128_t dividend{100};
  const sourcemeta::core::uint128_t divisor{10};
  const auto result = dividend / divisor;
  EXPECT_EQ(static_cast<std::uint64_t>(result), 10);
}

TEST(Numeric_uint128, modulo_by_uint128) {
  const sourcemeta::core::uint128_t dividend{17};
  const sourcemeta::core::uint128_t divisor{5};
  const auto result = dividend % divisor;
  EXPECT_EQ(static_cast<std::uint64_t>(result), 2);
}
