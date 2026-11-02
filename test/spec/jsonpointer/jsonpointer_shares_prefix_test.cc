#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/test.h>

TEST(property_common_prefix_true) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer other{"foo", "bar", "qux"};
  EXPECT_TRUE(pointer.shares_prefix(other, 2));
}

TEST(property_common_prefix_false) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer other{"foo", "bar", "qux"};
  EXPECT_FALSE(pointer.shares_prefix(other, 3));
}

TEST(index_common_prefix_true) {
  const sourcemeta::core::Pointer pointer{0, 1, 2};
  const sourcemeta::core::Pointer other{0, 1, 3};
  EXPECT_TRUE(pointer.shares_prefix(other, 2));
}

TEST(index_common_prefix_false) {
  const sourcemeta::core::Pointer pointer{0, 1, 2};
  const sourcemeta::core::Pointer other{0, 1, 3};
  EXPECT_FALSE(pointer.shares_prefix(other, 3));
}

TEST(equal_pointers_full_size) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::Pointer other{"foo", "bar"};
  EXPECT_TRUE(pointer.shares_prefix(other, 2));
}

TEST(different_first_token) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::Pointer other{"baz", "bar"};
  EXPECT_FALSE(pointer.shares_prefix(other, 1));
}

TEST(zero_size_non_empty) {
  const sourcemeta::core::Pointer pointer{"foo"};
  const sourcemeta::core::Pointer other{"bar"};
  EXPECT_TRUE(pointer.shares_prefix(other, 0));
}

TEST(zero_size_empty) {
  const sourcemeta::core::Pointer pointer;
  const sourcemeta::core::Pointer other;
  EXPECT_TRUE(pointer.shares_prefix(other, 0));
}

TEST(size_beyond_left) {
  const sourcemeta::core::Pointer pointer{"foo"};
  const sourcemeta::core::Pointer other{"foo", "bar"};
  EXPECT_FALSE(pointer.shares_prefix(other, 2));
}

TEST(size_beyond_right) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::Pointer other{"foo"};
  EXPECT_FALSE(pointer.shares_prefix(other, 2));
}

TEST(size_beyond_both) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::Pointer other{"foo", "bar"};
  EXPECT_FALSE(pointer.shares_prefix(other, 3));
}

TEST(commutative_true) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer other{"foo", "bar"};
  EXPECT_TRUE(pointer.shares_prefix(other, 2));
  EXPECT_TRUE(other.shares_prefix(pointer, 2));
}

TEST(property_and_index_mixed_true) {
  const sourcemeta::core::Pointer pointer{"foo", 0, "bar"};
  const sourcemeta::core::Pointer other{"foo", 0, "baz"};
  EXPECT_TRUE(pointer.shares_prefix(other, 2));
}

TEST(property_and_index_mixed_false) {
  const sourcemeta::core::Pointer pointer{"foo", 0, "bar"};
  const sourcemeta::core::Pointer other{"foo", 1, "bar"};
  EXPECT_FALSE(pointer.shares_prefix(other, 2));
}

TEST(property_token_against_index_token) {
  const sourcemeta::core::Pointer pointer{"foo", "0"};
  const sourcemeta::core::Pointer other{"foo", 0};
  EXPECT_FALSE(pointer.shares_prefix(other, 2));
}
