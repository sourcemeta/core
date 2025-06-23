#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

TEST(JSONPointer_to_json, foo_bar_baz) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const auto result{sourcemeta::core::to_json(pointer)};
  const sourcemeta::core::JSON expected{"/foo/bar/baz"};
  EXPECT_EQ(result, expected);
}

TEST(JSONWeakPointer_to_json, foo_bar_baz) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const std::string baz{"baz"};

  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar),
                                              std::cref(baz)};

  const auto result{sourcemeta::core::to_json(pointer)};
  const sourcemeta::core::JSON expected{"/foo/bar/baz"};
  EXPECT_EQ(result, expected);
}
