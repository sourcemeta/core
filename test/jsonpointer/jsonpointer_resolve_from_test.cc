#include <sourcemeta/core/test.h>

#include <sourcemeta/core/jsonpointer.h>

TEST(mismatch_base_shorter) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::Pointer base{"foo", "bar", "baz"};
  EXPECT_EQ(pointer.resolve_from(base), pointer);
}

TEST(mismatch_pointer_empty) {
  const sourcemeta::core::Pointer pointer;
  const sourcemeta::core::Pointer base{"foo", "bar", "baz"};
  EXPECT_EQ(pointer.resolve_from(base), pointer);
}

TEST(mismatch_base_empty) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::Pointer base;
  EXPECT_EQ(pointer.resolve_from(base), pointer);
}

TEST(equal_empty) {
  const sourcemeta::core::Pointer pointer;
  const sourcemeta::core::Pointer base;
  EXPECT_EQ(pointer.resolve_from(base), pointer);
}

TEST(equal_non_empty) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::Pointer base{"foo", "bar"};
  EXPECT_EQ(pointer.resolve_from(base), sourcemeta::core::Pointer{});
}

TEST(match_1) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz", "qux"};
  const sourcemeta::core::Pointer base{"foo", "bar"};
  EXPECT_EQ(pointer.resolve_from(base),
            sourcemeta::core::Pointer({"baz", "qux"}));
}

TEST(match_2) {
  const sourcemeta::core::Pointer pointer{0, 1, 2};
  const sourcemeta::core::Pointer base{0, 1};
  EXPECT_EQ(pointer.resolve_from(base), sourcemeta::core::Pointer({2}));
}
