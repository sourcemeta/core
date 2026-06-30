#include <sourcemeta/core/test.h>

#include <sourcemeta/core/jsonpointer.h>

TEST(empty) {
  const sourcemeta::core::Pointer pointer;
  EXPECT_EQ(sourcemeta::core::to_string(pointer), "");
}

TEST(empty_property) {
  const sourcemeta::core::Pointer pointer{""};
  EXPECT_EQ(sourcemeta::core::to_string(pointer), "/");
}

TEST(foo_bar_baz) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  EXPECT_EQ(sourcemeta::core::to_string(pointer), "/foo/bar/baz");
}

TEST(foo_1_2) {
  const sourcemeta::core::Pointer pointer{"foo", 1, 2};
  EXPECT_EQ(sourcemeta::core::to_string(pointer), "/foo/1/2");
}

TEST(weak_empty) {
  const sourcemeta::core::WeakPointer pointer;
  EXPECT_EQ(sourcemeta::core::to_string(pointer), "");
}

TEST(weak_empty_property) {
  const std::string empty = "";
  const sourcemeta::core::WeakPointer pointer{std::cref(empty)};
  EXPECT_EQ(sourcemeta::core::to_string(pointer), "/");
}

TEST(weak_foo_bar_baz) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar),
                                              std::cref(baz)};
  EXPECT_EQ(sourcemeta::core::to_string(pointer), "/foo/bar/baz");
}

TEST(weak_foo_1_2) {
  const std::string foo = "foo";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), 1, 2};
  EXPECT_EQ(sourcemeta::core::to_string(pointer), "/foo/1/2");
}
