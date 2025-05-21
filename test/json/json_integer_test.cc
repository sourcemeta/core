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
