#include <sourcemeta/core/test.h>

#include <sourcemeta/core/jsonpointer.h>

TEST(first_property_for_one) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::Pointer prefix{"foo"};
  const sourcemeta::core::Pointer replacement{"baz"};
  const sourcemeta::core::Pointer expected{"baz", "bar"};
  EXPECT_EQ(pointer.rebase(prefix, replacement), expected);
}

TEST(first_property_for_two) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::Pointer prefix{"foo"};
  const sourcemeta::core::Pointer replacement{"baz", "qux"};
  const sourcemeta::core::Pointer expected{"baz", "qux", "bar"};
  EXPECT_EQ(pointer.rebase(prefix, replacement), expected);
}

TEST(first_property_for_empty) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::Pointer prefix{"foo"};
  const sourcemeta::core::Pointer replacement;
  const sourcemeta::core::Pointer expected{"bar"};
  EXPECT_EQ(pointer.rebase(prefix, replacement), expected);
}

TEST(empty_for_single_property) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::Pointer prefix;
  const sourcemeta::core::Pointer replacement{"baz"};
  const sourcemeta::core::Pointer expected{"baz", "foo", "bar"};
  EXPECT_EQ(pointer.rebase(prefix, replacement), expected);
}

TEST(empty_for_multi_property) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::Pointer prefix;
  const sourcemeta::core::Pointer replacement{"baz", "qux"};
  const sourcemeta::core::Pointer expected{"baz", "qux", "foo", "bar"};
  EXPECT_EQ(pointer.rebase(prefix, replacement), expected);
}

TEST(prefix_equal_pointer) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::Pointer prefix{"foo", "bar"};
  const sourcemeta::core::Pointer replacement{"baz", "qux"};
  const sourcemeta::core::Pointer expected{"baz", "qux"};
  EXPECT_EQ(pointer.rebase(prefix, replacement), expected);
}

TEST(empty_replacement) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer prefix{"foo"};
  const sourcemeta::core::Pointer replacement;
  const sourcemeta::core::Pointer expected{"bar", "baz"};
  EXPECT_EQ(pointer.rebase(prefix, replacement), expected);
}

TEST(empty_empty_empty) {
  const sourcemeta::core::Pointer pointer;
  const sourcemeta::core::Pointer prefix;
  const sourcemeta::core::Pointer replacement;
  const sourcemeta::core::Pointer expected;
  EXPECT_EQ(pointer.rebase(prefix, replacement), expected);
}

TEST(no_prefix_match_1) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::Pointer prefix{"bar"};
  const sourcemeta::core::Pointer replacement{"baz"};
  const sourcemeta::core::Pointer expected{"foo", "bar"};
  EXPECT_EQ(pointer.rebase(prefix, replacement), expected);
}
