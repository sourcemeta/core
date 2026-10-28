#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/test.h>

TEST(multi_multi) {
  const sourcemeta::core::Pointer left{"foo", "bar"};
  const sourcemeta::core::Pointer right{"baz", "qux"};
  const sourcemeta::core::Pointer result{"foo", "bar", "baz", "qux"};
  EXPECT_EQ(left.concat(right), result);
}

TEST(single_single) {
  const sourcemeta::core::Pointer left{"foo"};
  const sourcemeta::core::Pointer right{"bar"};
  const sourcemeta::core::Pointer result{"foo", "bar"};
  EXPECT_EQ(left.concat(right), result);
}

TEST(multi_single) {
  const sourcemeta::core::Pointer left{"foo", "bar"};
  const sourcemeta::core::Pointer right{"baz"};
  const sourcemeta::core::Pointer result{"foo", "bar", "baz"};
  EXPECT_EQ(left.concat(right), result);
}

TEST(single_multi) {
  const sourcemeta::core::Pointer left{"foo"};
  const sourcemeta::core::Pointer right{"bar", "baz"};
  const sourcemeta::core::Pointer result{"foo", "bar", "baz"};
  EXPECT_EQ(left.concat(right), result);
}

TEST(empty_single) {
  const sourcemeta::core::Pointer left;
  const sourcemeta::core::Pointer right{"foo"};
  const sourcemeta::core::Pointer result{"foo"};
  EXPECT_EQ(left.concat(right), result);
}

TEST(single_empty) {
  const sourcemeta::core::Pointer left{"foo"};
  const sourcemeta::core::Pointer right;
  const sourcemeta::core::Pointer result{"foo"};
  EXPECT_EQ(left.concat(right), result);
}

TEST(empty_multi) {
  const sourcemeta::core::Pointer left;
  const sourcemeta::core::Pointer right{"foo", "bar"};
  const sourcemeta::core::Pointer result{"foo", "bar"};
  EXPECT_EQ(left.concat(right), result);
}

TEST(multi_empty) {
  const sourcemeta::core::Pointer left{"foo", "bar"};
  const sourcemeta::core::Pointer right;
  const sourcemeta::core::Pointer result{"foo", "bar"};
  EXPECT_EQ(left.concat(right), result);
}

TEST(empty_empty) {
  const sourcemeta::core::Pointer left;
  const sourcemeta::core::Pointer right;
  const sourcemeta::core::Pointer result;
  EXPECT_EQ(left.concat(right), result);
}

TEST(itself) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::Pointer result{"foo", "bar", "foo", "bar"};
  EXPECT_EQ(pointer.concat(pointer), result);
}

TEST(single_property_token) {
  const sourcemeta::core::Pointer pointer{"foo"};
  const sourcemeta::core::Pointer result{"foo", "bar"};
  EXPECT_EQ(pointer.concat("bar"), result);
}

TEST(single_index_token) {
  const sourcemeta::core::Pointer pointer{"foo"};
  const sourcemeta::core::Pointer result{"foo", 0};
  EXPECT_EQ(pointer.concat(0), result);
}

TEST(multi_property_token) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::Pointer result{"foo", "bar", "baz"};
  EXPECT_EQ(pointer.concat("baz"), result);
}

TEST(multi_index_token) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::Pointer result{"foo", "bar", 0};
  EXPECT_EQ(pointer.concat(0), result);
}

TEST(empty_property_token) {
  const sourcemeta::core::Pointer pointer;
  const sourcemeta::core::Pointer result{"foo"};
  EXPECT_EQ(pointer.concat("foo"), result);
}

TEST(empty_index_token) {
  const sourcemeta::core::Pointer pointer;
  const sourcemeta::core::Pointer result{0};
  EXPECT_EQ(pointer.concat(0), result);
}
