#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/test.h>

TEST(property_prefix_true) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer prefix{"foo", "bar"};
  EXPECT_TRUE(pointer.starts_with_strict(prefix));
}

TEST(property_prefix_false) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer prefix{"bar", "baz"};
  EXPECT_FALSE(pointer.starts_with_strict(prefix));
}

TEST(index_prefix_true) {
  const sourcemeta::core::Pointer pointer{0, 1, 2};
  const sourcemeta::core::Pointer prefix{0, 1};
  EXPECT_TRUE(pointer.starts_with_strict(prefix));
}

TEST(index_prefix_false) {
  const sourcemeta::core::Pointer pointer{0, 1, 2};
  const sourcemeta::core::Pointer prefix{1, 2};
  EXPECT_FALSE(pointer.starts_with_strict(prefix));
}

TEST(property_equal) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::Pointer prefix{"foo", "bar"};
  EXPECT_FALSE(pointer.starts_with_strict(prefix));
}

TEST(index_equal) {
  const sourcemeta::core::Pointer pointer{0, 1};
  const sourcemeta::core::Pointer prefix{0, 1};
  EXPECT_FALSE(pointer.starts_with_strict(prefix));
}

TEST(same_pointer) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  EXPECT_FALSE(pointer.starts_with_strict(pointer));
}

TEST(longer_prefix) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::Pointer prefix{"foo", "bar", "baz"};
  EXPECT_FALSE(pointer.starts_with_strict(prefix));
}

TEST(non_empty_empty) {
  const sourcemeta::core::Pointer pointer{"foo"};
  const sourcemeta::core::Pointer prefix;
  EXPECT_TRUE(pointer.starts_with_strict(prefix));
}

TEST(empty_non_empty) {
  const sourcemeta::core::Pointer pointer;
  const sourcemeta::core::Pointer prefix{"foo"};
  EXPECT_FALSE(pointer.starts_with_strict(prefix));
}

TEST(empty_empty) {
  const sourcemeta::core::Pointer pointer;
  const sourcemeta::core::Pointer prefix;
  EXPECT_FALSE(pointer.starts_with_strict(prefix));
}

TEST(divergent_last_prefix_token) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer prefix{"foo", "qux"};
  EXPECT_FALSE(pointer.starts_with_strict(prefix));
}

TEST(property_and_index_mixed_true) {
  const sourcemeta::core::Pointer pointer{"foo", 0, "bar"};
  const sourcemeta::core::Pointer prefix{"foo", 0};
  EXPECT_TRUE(pointer.starts_with_strict(prefix));
}

TEST(property_and_index_mixed_false) {
  const sourcemeta::core::Pointer pointer{"foo", 0, "bar"};
  const sourcemeta::core::Pointer prefix{"foo", 1};
  EXPECT_FALSE(pointer.starts_with_strict(prefix));
}

TEST(property_token_against_index_token) {
  const sourcemeta::core::Pointer pointer{"foo", "0", "bar"};
  const sourcemeta::core::Pointer prefix{"foo", 0};
  EXPECT_FALSE(pointer.starts_with_strict(prefix));
}
