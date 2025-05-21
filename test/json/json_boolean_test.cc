#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

TEST(JSON_boolean, true_value) {
  const sourcemeta::core::JSON document{true};
  EXPECT_TRUE(document.is_boolean());
  EXPECT_TRUE(document.to_boolean());
}

TEST(JSON_boolean, false_value) {
  const sourcemeta::core::JSON document{false};
  EXPECT_TRUE(document.is_boolean());
  EXPECT_FALSE(document.to_boolean());
}

TEST(JSON_boolean, type_false) {
  const sourcemeta::core::JSON document{false};
  EXPECT_EQ(document.type(), sourcemeta::core::JSON::Type::Boolean);
}

TEST(JSON_boolean, type_true) {
  const sourcemeta::core::JSON document{true};
  EXPECT_EQ(document.type(), sourcemeta::core::JSON::Type::Boolean);
}

TEST(JSON_boolean, literal_equality_false) {
  const sourcemeta::core::JSON left{false};
  const sourcemeta::core::JSON right{false};
  const sourcemeta::core::JSON extra{true};
  EXPECT_EQ(left, left);
  EXPECT_EQ(right, right);
  EXPECT_EQ(left, right);
  EXPECT_EQ(right, left);
  EXPECT_FALSE(left == extra);
  EXPECT_FALSE(right == extra);
}

TEST(JSON_boolean, literal_equality_true) {
  const sourcemeta::core::JSON left{true};
  const sourcemeta::core::JSON right{true};
  const sourcemeta::core::JSON extra{false};
  EXPECT_EQ(left, left);
  EXPECT_EQ(right, right);
  EXPECT_EQ(left, right);
  EXPECT_EQ(right, left);
  EXPECT_FALSE(left == extra);
  EXPECT_FALSE(right == extra);
}

TEST(JSON_boolean, estimated_byte_size_false) {
  const sourcemeta::core::JSON document{false};
  EXPECT_EQ(document.estimated_byte_size(), 1);
}

TEST(JSON_boolean, estimated_byte_size_true) {
  const sourcemeta::core::JSON document{true};
  EXPECT_EQ(document.estimated_byte_size(), 1);
}

TEST(JSON_boolean, fast_hash_false) {
  const sourcemeta::core::JSON document{false};
  EXPECT_EQ(document.fast_hash(), 0);
}

TEST(JSON_boolean, fast_hash_true) {
  const sourcemeta::core::JSON document{true};
  EXPECT_EQ(document.fast_hash(), 1);
}
