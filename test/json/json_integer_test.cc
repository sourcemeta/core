#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

TEST(JSON_integer, positive) {
  const sourcemeta::core::JSON document{10};
  EXPECT_TRUE(document.is_integer());
  EXPECT_EQ(document.to_integer(), 10);
}

TEST(JSON_integer, type) {
  const sourcemeta::core::JSON document{5};
  EXPECT_EQ(document.type(), sourcemeta::core::JSON::Type::Integer);
}

TEST(JSON_integer, negative) {
  const sourcemeta::core::JSON document{-10};
  EXPECT_TRUE(document.is_integer());
  EXPECT_EQ(document.to_integer(), -10);
}

TEST(JSON_integer, zero) {
  const sourcemeta::core::JSON document{0};
  EXPECT_TRUE(document.is_integer());
  EXPECT_EQ(document.to_integer(), 0);
}

// We have seen GCC getting confused about this one,
// creating an array with a single integer item instead.
TEST(JSON_integer, copy_constructor) {
  const sourcemeta::core::JSON integer{5};
  const sourcemeta::core::JSON document{integer};
  EXPECT_TRUE(document.is_integer());
  EXPECT_EQ(document.to_integer(), 5);
}

TEST(JSON_integer, estimated_byte_size_5) {
  const sourcemeta::core::JSON document{5};
  EXPECT_EQ(document.estimated_byte_size(), 8);
}

TEST(JSON_integer, estimated_byte_size_minus_5) {
  const sourcemeta::core::JSON document{-5};
  EXPECT_EQ(document.estimated_byte_size(), 8);
}

TEST(JSON_integer, estimated_byte_size_0) {
  const sourcemeta::core::JSON document{0};
  EXPECT_EQ(document.estimated_byte_size(), 8);
}

TEST(JSON_integer, estimated_byte_size_1234567) {
  const sourcemeta::core::JSON document{1234567};
  EXPECT_EQ(document.estimated_byte_size(), 8);
}

TEST(JSON_integer, fast_hash_5) {
  const sourcemeta::core::JSON document{5};
  EXPECT_EQ(document.fast_hash(), 9);
}

TEST(JSON_integer, fast_hash_minus_5) {
  const sourcemeta::core::JSON document{-5};
  EXPECT_EQ(document.fast_hash(), 255);
}

TEST(JSON_integer, fast_hash_0) {
  const sourcemeta::core::JSON document{0};
  EXPECT_EQ(document.fast_hash(), 4);
}

TEST(JSON_integer, fast_hash_1234567) {
  const sourcemeta::core::JSON document{1234567};
  EXPECT_EQ(document.fast_hash(), 139);
}

TEST(JSON_integer, less_than_integer_decimal) {
  const sourcemeta::core::JSON integer{3};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  EXPECT_TRUE(integer < decimal);
  EXPECT_FALSE(decimal < integer);
}

TEST(JSON_integer, less_than_or_equal_integer_decimal) {
  const sourcemeta::core::JSON integer{3};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON equal{sourcemeta::core::Decimal{"3"}};
  EXPECT_TRUE(integer <= decimal);
  EXPECT_FALSE(decimal <= integer);
  EXPECT_TRUE(integer <= equal);
}

TEST(JSON_integer, greater_than_integer_decimal) {
  const sourcemeta::core::JSON integer{5};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  EXPECT_TRUE(integer > decimal);
  EXPECT_FALSE(decimal > integer);
}

TEST(JSON_integer, greater_than_or_equal_integer_decimal) {
  const sourcemeta::core::JSON integer{5};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON equal{sourcemeta::core::Decimal{"5"}};
  EXPECT_TRUE(integer >= decimal);
  EXPECT_FALSE(decimal >= integer);
  EXPECT_TRUE(integer >= equal);
}

TEST(JSON_integer, addition_integer_decimal) {
  const sourcemeta::core::JSON integer{3};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  const sourcemeta::core::JSON result{integer + decimal};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal().to_string(), "8.7");
}

TEST(JSON_integer, subtraction_integer_decimal) {
  const sourcemeta::core::JSON integer{5};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  const sourcemeta::core::JSON result{integer - decimal};
  EXPECT_TRUE(result.is_decimal());
  EXPECT_EQ(result.to_decimal().to_string(), "1.8");
}

TEST(JSON_integer, addition_assignment_integer_decimal) {
  sourcemeta::core::JSON integer{3};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"5.7"}};
  integer += decimal;
  EXPECT_TRUE(integer.is_decimal());
  EXPECT_EQ(integer.to_decimal().to_string(), "8.7");
}

TEST(JSON_integer, subtraction_assignment_integer_decimal) {
  sourcemeta::core::JSON integer{5};
  const sourcemeta::core::JSON decimal{sourcemeta::core::Decimal{"3.2"}};
  integer -= decimal;
  EXPECT_TRUE(integer.is_decimal());
  EXPECT_EQ(integer.to_decimal().to_string(), "1.8");
}
