#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

TEST(JSON_null, nullptr_value) {
  const sourcemeta::core::JSON document{nullptr};
  EXPECT_TRUE(document.is_null());
}

TEST(JSON_null, type) {
  const sourcemeta::core::JSON document{nullptr};
  EXPECT_EQ(document.type(), sourcemeta::core::JSON::Type::Null);
}

TEST(JSON_null, literal_equality) {
  const sourcemeta::core::JSON left{nullptr};
  const sourcemeta::core::JSON right{false};
  EXPECT_EQ(left, left);
  EXPECT_FALSE(left == right);
  EXPECT_FALSE(right == left);
}

TEST(JSON_null, estimated_byte_size) {
  const sourcemeta::core::JSON document{nullptr};
  EXPECT_EQ(document.estimated_byte_size(), 8);
}

TEST(JSON_null, fast_hash) {
  const sourcemeta::core::JSON document{nullptr};
  EXPECT_EQ(document.fast_hash(), 2);
}
