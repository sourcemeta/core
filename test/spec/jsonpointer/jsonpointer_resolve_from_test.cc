#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/test.h>

#include <functional>
#include <string>

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

// A base the pointer disagrees with at a token it shares a position with is no
// base of it, so the pointer comes back whole
TEST(base_that_diverges_from_the_pointer) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::Pointer base{"qux"};
  EXPECT_EQ(pointer.resolve_from(base), pointer);
}

TEST(weak_mismatch_base_shorter) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const std::string baz{"baz"};
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer base{std::cref(foo), std::cref(bar),
                                           std::cref(baz)};
  EXPECT_EQ(pointer.resolve_from(base), pointer);
}

TEST(weak_match) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const std::string baz{"baz"};
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar),
                                              std::cref(baz)};
  const sourcemeta::core::WeakPointer base{std::cref(foo), std::cref(bar)};
  EXPECT_EQ(pointer.resolve_from(base),
            sourcemeta::core::WeakPointer{std::cref(baz)});
}

TEST(weak_base_that_diverges_from_the_pointer) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const std::string qux{"qux"};
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer base{std::cref(qux)};
  EXPECT_EQ(pointer.resolve_from(base), pointer);
}
