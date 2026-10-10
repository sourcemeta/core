#include <sourcemeta/core/test.h>
#include <sourcemeta/core/uri.h>

TEST(empty) { EXPECT_EQ(sourcemeta::core::URI::normalize_path(""), ""); }

TEST(root) { EXPECT_EQ(sourcemeta::core::URI::normalize_path("/"), "/"); }

TEST(no_dot_segments) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("/foo/bar"), "/foo/bar");
}

TEST(trailing_slash_is_preserved) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("/foo/bar/"), "/foo/bar/");
}

TEST(single_dot_slash) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("/./"), "/");
}

TEST(single_dot_at_end) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("/."), "/");
}

TEST(interior_single_dot) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("/foo/./bar"), "/foo/bar");
}

TEST(interior_single_dot_with_trailing) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("/foo/bar/./"), "/foo/bar/");
}

TEST(double_dot) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("/foo/bar/../baz"),
            "/foo/baz");
}

TEST(double_dot_at_end) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("/foo/bar/.."), "/foo/");
}

TEST(double_dot_at_end_exact) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("/.."), "/");
}

TEST(interleaved_dot_and_double_dot) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("/a/./b/../c"), "/a/c");
}

TEST(collapse_to_root) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("/example1/example2/../.."),
            "/");
}

TEST(double_dot_past_root_stays_at_root) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("/../"), "/");
}

// Only "." and ".." segments are removed, so sequential slashes are preserved
TEST(sequential_slashes_are_not_collapsed) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("/a//b"), "/a//b");
}

TEST(double_slash_is_not_collapsed) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("//"), "//");
}

TEST(relative_no_dot_segments) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("foo"), "foo");
}

TEST(relative_multi_segment) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("foo/bar"), "foo/bar");
}

TEST(relative_leading_single_dot) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("./foo"), "foo");
}

TEST(relative_interior_single_dot) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("a/./b"), "a/b");
}

TEST(relative_interior_double_dot) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("a/b/../c"), "a/c");
}

TEST(relative_double_dot_removes_prior_segment) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("a/../b"), "b");
}

TEST(relative_double_dot_consumes_segment) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("a/.."), "");
}

TEST(relative_single_dot_only) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("."), "");
}

// Leading ".." blocks are preserved for stand-alone relative paths, since the
// specification assumes a path already merged with an absolute base
TEST(relative_leading_double_dot_is_preserved) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("../foo"), "../foo");
}

TEST(relative_multiple_leading_double_dots_are_preserved) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("../../foo"), "../../foo");
}

// RFC 3986 Section 5.2.4 removes a dot-dot segment together with the one
// before it, and an absolute path that walks past its own root stays rooted
TEST(absolute_double_dot_only_keeps_the_root) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("/.."), "/");
}

TEST(absolute_walking_past_the_root_keeps_the_root) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("/../.."), "/");
}

// A relative path walking past its own start keeps the segments it could not
// resolve, which is what distinguishes it from a rooted one
TEST(relative_double_dot_only_is_preserved) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path(".."), "../");
}

TEST(relative_dot_dot_followed_by_a_rooted_remainder) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("../foo"), "../foo");
}

TEST(relative_dot_dot_segment_with_nothing_after_it) {
  EXPECT_EQ(sourcemeta::core::URI::normalize_path("/../"), "/");
}
