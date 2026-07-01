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
