#include <sourcemeta/core/test.h>

#include <sourcemeta/core/json.h>

TEST(true_value) {
  const sourcemeta::core::JSON document{true};
  EXPECT_TRUE(document.is_boolean());
  EXPECT_TRUE(document.to_boolean());
}

TEST(false_value) {
  const sourcemeta::core::JSON document{false};
  EXPECT_TRUE(document.is_boolean());
  EXPECT_FALSE(document.to_boolean());
}

TEST(type_false) {
  const sourcemeta::core::JSON document{false};
  EXPECT_EQ(document.type(), sourcemeta::core::JSON::Type::Boolean);
}

TEST(type_true) {
  const sourcemeta::core::JSON document{true};
  EXPECT_EQ(document.type(), sourcemeta::core::JSON::Type::Boolean);
}

TEST(literal_equality_false) {
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

TEST(literal_equality_true) {
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

TEST(estimated_byte_size_false) {
  const sourcemeta::core::JSON document{false};
  EXPECT_EQ(document.estimated_byte_size(), 1);
}

TEST(estimated_byte_size_true) {
  const sourcemeta::core::JSON document{true};
  EXPECT_EQ(document.estimated_byte_size(), 1);
}

TEST(fast_hash_false) {
  const sourcemeta::core::JSON document{false};
  EXPECT_EQ(document.fast_hash(), 0);
}

TEST(fast_hash_true) {
  const sourcemeta::core::JSON document{true};
  EXPECT_EQ(document.fast_hash(), 1);
}
