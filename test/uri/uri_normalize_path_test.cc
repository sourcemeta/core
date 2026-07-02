#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

TEST(empty) { EXPECT_EQ(sourcemeta::core::URI::normalize_path(""), ""); }

TEST(root) { EXPECT_EQ(sourcemeta::core::URI::normalize_path("/"), "/"); }

TEST(no_dot_segments) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("/foo/bar"), "/foo/bar");
}

TEST(single_dot) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("/./"), "/");
}

TEST(interior_single_dot) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("/foo/./bar"), "/foo/bar");
}

TEST(double_dot) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("/foo/bar/../baz"),
            "/foo/baz");
}

TEST(trailing_double_dot) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("/foo/bar/.."), "/foo/");
}

TEST(collapse_to_root) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("/example1/example2/../.."),
            "/");
}

TEST(double_dot_past_root_stays_at_root) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("/../"), "/");
}
