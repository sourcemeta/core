#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>

#include <cstdint> // std::int64_t
#include <limits>  // std::numeric_limits

TEST(positive) {
  const sourcemeta::core::JSON document{10};
  EXPECT_TRUE(document.is_integer());
  EXPECT_EQ(document.to_integer(), 10);
}

TEST(type) {
  const sourcemeta::core::JSON document{5};
  EXPECT_EQ(document.type(), sourcemeta::core::JSON::Type::Integer);
}

TEST(negative) {
  const sourcemeta::core::JSON document{-10};
  EXPECT_TRUE(document.is_integer());
  EXPECT_EQ(document.to_integer(), -10);
}

TEST(zero) {
  const sourcemeta::core::JSON document{0};
  EXPECT_TRUE(document.is_integer());
  EXPECT_EQ(document.to_integer(), 0);
}

// We have seen GCC getting confused about this one,
// creating an array with a single integer item instead.
TEST(copy_constructor) {
  const sourcemeta::core::JSON integer{5};
  const sourcemeta::core::JSON document{integer};
  EXPECT_TRUE(document.is_integer());
  EXPECT_EQ(document.to_integer(), 5);
}

TEST(estimated_byte_size_5) {
  const sourcemeta::core::JSON document{5};
  EXPECT_EQ(document.estimated_byte_size(), 8);
}

TEST(estimated_byte_size_minus_5) {
  const sourcemeta::core::JSON document{-5};
  EXPECT_EQ(document.estimated_byte_size(), 8);
}

TEST(estimated_byte_size_0) {
  const sourcemeta::core::JSON document{0};
  EXPECT_EQ(document.estimated_byte_size(), 8);
}

TEST(estimated_byte_size_1234567) {
  const sourcemeta::core::JSON document{1234567};
  EXPECT_EQ(document.estimated_byte_size(), 8);
}

TEST(fast_hash_5) {
  const sourcemeta::core::JSON document{5};
  EXPECT_EQ(document.fast_hash(), 9);
}

TEST(fast_hash_minus_5) {
  const sourcemeta::core::JSON document{-5};
  EXPECT_EQ(document.fast_hash(), 255);
}

TEST(fast_hash_0) {
  const sourcemeta::core::JSON document{0};
  EXPECT_EQ(document.fast_hash(), 4);
}

TEST(fast_hash_1234567) {
  const sourcemeta::core::JSON document{1234567};
  EXPECT_EQ(document.fast_hash(), 139);
}

TEST(less_than_integer_decimal) {
  const sourcemeta::core::JSON integer{3};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  EXPECT_TRUE(integer < decimal);
  EXPECT_FALSE(decimal < integer);
}

TEST(less_than_or_equal_integer_decimal) {
  const sourcemeta::core::JSON integer{3};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON equal{sourcemeta::core::Decimal{"3"}};
  EXPECT_TRUE(integer <= decimal);
  EXPECT_FALSE(decimal <= integer);
  EXPECT_TRUE(integer <= equal);
}

TEST(greater_than_integer_decimal) {
  const sourcemeta::core::JSON integer{5};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  EXPECT_TRUE(integer > decimal);
  EXPECT_FALSE(decimal > integer);
}

TEST(greater_than_or_equal_integer_decimal) {
  const sourcemeta::core::JSON integer{5};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON equal{sourcemeta::core::Decimal{"5"}};
  EXPECT_TRUE(integer >= decimal);
  EXPECT_FALSE(decimal >= integer);
  EXPECT_TRUE(integer >= equal);
}

TEST(addition_integer_decimal) {
  const sourcemeta::core::JSON integer{3};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON result{integer + decimal};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal().to_string(), "8.7");
}

TEST(subtraction_integer_decimal) {
  const sourcemeta::core::JSON integer{5};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON result{integer - decimal};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal().to_string(), "1.8");
}

TEST(addition_assignment_integer_decimal) {
  sourcemeta::core::JSON integer{3};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  integer += decimal;
  EXPECT_TRUE(integer.is_decimal());
  EXPECT_EQ(integer.to_decimal().to_string(), "8.7");
}

TEST(subtraction_assignment_integer_decimal) {
  sourcemeta::core::JSON integer{5};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  integer -= decimal;
  EXPECT_TRUE(integer.is_decimal());
  EXPECT_EQ(integer.to_decimal().to_string(), "1.8");
}

// Adding or subtracting past the integer range promotes to arbitrary precision
// rather than wrapping, since signed integer overflow is undefined behavior.
TEST(addition_overflow_promotes_to_decimal) {
  const sourcemeta::core::JSON maximum{
      std::numeric_limits<std::int64_t>::max()};
  const sourcemeta::core::JSON one{static_cast<std::int64_t>(1)};
  const auto result{maximum + one};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_FALSE(result.is_integer());
}

TEST(subtraction_overflow_promotes_to_decimal) {
  const sourcemeta::core::JSON minimum{
      std::numeric_limits<std::int64_t>::min()};
  const sourcemeta::core::JSON one{static_cast<std::int64_t>(1)};
  const auto result{minimum - one};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_FALSE(result.is_integer());
}

// Numeric equality is exact and transitive across the integer, real and
// decimal representations.
TEST(equal_across_numeric_representations) {
  const sourcemeta::core::JSON as_integer{static_cast<std::int64_t>(1000)};
  const sourcemeta::core::JSON as_real{1000.0};
  const auto as_decimal{sourcemeta::core::parse_json("1000e0")};
  EXPECT_TRUE(as_integer == as_real);
  EXPECT_TRUE(as_real == as_decimal);
  EXPECT_TRUE(as_integer == as_decimal);
}

// The comparison is exact, so INT64_MAX and 2^63 (which differ by one) are not
// treated as equal even though they round to the same double.
TEST(equal_integer_real_is_exact_not_lossy) {
  const sourcemeta::core::JSON real_two_pow_63{9223372036854775808.0};
  const sourcemeta::core::JSON integer_maximum{
      std::numeric_limits<std::int64_t>::max()};
  EXPECT_FALSE(real_two_pow_63 == integer_maximum);
  const sourcemeta::core::JSON two{static_cast<std::int64_t>(2)};
  const sourcemeta::core::JSON two_and_a_half{2.5};
  EXPECT_FALSE(two == two_and_a_half);
}

// Values that compare equal must produce the same fast hash. Equality checks
// that use the hash as a prefilter skip the comparison when the hashes differ,
// so an equal pair across numeric representations would otherwise be missed.
TEST(fast_hash_matches_equal_real) {
  const sourcemeta::core::JSON as_integer{static_cast<std::int64_t>(1000)};
  const sourcemeta::core::JSON as_real{1000.0};
  EXPECT_TRUE(as_integer == as_real);
  EXPECT_EQ(as_integer.fast_hash(), as_real.fast_hash());
}
