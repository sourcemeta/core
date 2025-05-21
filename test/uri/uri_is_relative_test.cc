#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_is_relative, dot_slash) {
  const sourcemeta::core::URI uri{"./foo"};
  EXPECT_TRUE(uri.is_relative());
}

TEST(URI_is_relative, dot_dot_slash) {
  const sourcemeta::core::URI uri{"../foo"};
  EXPECT_TRUE(uri.is_relative());
}

TEST(URI_is_relative, without_dot_and_slash) {
  const sourcemeta::core::URI uri{"foo"};
  EXPECT_TRUE(uri.is_relative());
}

TEST(URI_is_relative, unique_slash) {
  // The is a relative URI, even if the path is absolute.
  const sourcemeta::core::URI uri{"/foo"};
  EXPECT_TRUE(uri.is_relative());
}
