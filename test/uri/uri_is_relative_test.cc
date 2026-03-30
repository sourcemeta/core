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

// RFC 3986 Section 4: a URI with a scheme is never a relative reference,
// regardless of path content
TEST(URI_is_relative, scheme_with_dot_path_is_not_relative) {
  const sourcemeta::core::URI uri{"a:./foo"};
  EXPECT_TRUE(uri.is_absolute());
  EXPECT_FALSE(uri.is_relative());
}

TEST(URI_is_relative, scheme_with_dotdot_path_is_not_relative) {
  const sourcemeta::core::URI uri{"a:../foo"};
  EXPECT_TRUE(uri.is_absolute());
  EXPECT_FALSE(uri.is_relative());
}

TEST(URI_is_relative, scheme_with_dot_only_is_not_relative) {
  const sourcemeta::core::URI uri{"a:."};
  EXPECT_TRUE(uri.is_absolute());
  EXPECT_FALSE(uri.is_relative());
}
