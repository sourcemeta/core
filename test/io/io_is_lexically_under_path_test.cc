#include <gtest/gtest.h>

#include <sourcemeta/core/io.h>

#include <string_view> // std::string_view

TEST(IO_is_lexically_under_path, relative_exact_match) {
  const std::filesystem::path path{"foo/bar"};
  const std::filesystem::path prefix{"foo/bar"};
  EXPECT_TRUE(sourcemeta::core::is_lexically_under_path(path, prefix));
}

TEST(IO_is_lexically_under_path, relative_nested) {
  const std::filesystem::path path{"foo/bar/baz"};
  const std::filesystem::path prefix{"foo/bar"};
  EXPECT_TRUE(sourcemeta::core::is_lexically_under_path(path, prefix));
}

TEST(IO_is_lexically_under_path, relative_sibling) {
  const std::filesystem::path path{"foo/bar"};
  const std::filesystem::path prefix{"foo/baz"};
  EXPECT_FALSE(sourcemeta::core::is_lexically_under_path(path, prefix));
}

TEST(IO_is_lexically_under_path, relative_unrelated_branches) {
  const std::filesystem::path path{"foo/bar"};
  const std::filesystem::path prefix{"baz"};
  EXPECT_FALSE(sourcemeta::core::is_lexically_under_path(path, prefix));
}

TEST(IO_is_lexically_under_path, relative_same_chars_not_component) {
  const std::filesystem::path path{"foo/barbaz"};
  const std::filesystem::path prefix{"foo/bar"};
  EXPECT_FALSE(sourcemeta::core::is_lexically_under_path(path, prefix));
}

TEST(IO_is_lexically_under_path, relative_prefix_longer_than_path) {
  const std::filesystem::path path{"foo"};
  const std::filesystem::path prefix{"foo/bar"};
  EXPECT_FALSE(sourcemeta::core::is_lexically_under_path(path, prefix));
}

TEST(IO_is_lexically_under_path, relative_trailing_slash_prefix) {
  const std::filesystem::path path{"foo/bar"};
  const std::filesystem::path prefix{"foo/bar/"};
  EXPECT_TRUE(sourcemeta::core::is_lexically_under_path(path, prefix));
}

TEST(IO_is_lexically_under_path, relative_repeated_slashes) {
  const std::filesystem::path path{"foo//bar//baz"};
  const std::filesystem::path prefix{"foo/bar"};
  EXPECT_TRUE(sourcemeta::core::is_lexically_under_path(path, prefix));
}

TEST(IO_is_lexically_under_path, relative_dot_normalization) {
  const std::filesystem::path path{"foo/./bar"};
  const std::filesystem::path prefix{"foo/bar"};
  EXPECT_TRUE(sourcemeta::core::is_lexically_under_path(path, prefix));
}

TEST(IO_is_lexically_under_path, relative_dotdot_net_under_prefix) {
  const std::filesystem::path path{"foo/x/../bar"};
  const std::filesystem::path prefix{"foo/bar"};
  EXPECT_TRUE(sourcemeta::core::is_lexically_under_path(path, prefix));
}

TEST(IO_is_lexically_under_path, relative_dotdot_escapes_prefix) {
  const std::filesystem::path path{"foo/bar/../../etc"};
  const std::filesystem::path prefix{"foo/bar"};
  EXPECT_FALSE(sourcemeta::core::is_lexically_under_path(path, prefix));
}

TEST(IO_is_lexically_under_path, relative_dotdot_smuggled_inside_after_match) {
  const std::filesystem::path path{"foo/bar/x/../../baz"};
  const std::filesystem::path prefix{"foo/bar"};
  EXPECT_FALSE(sourcemeta::core::is_lexically_under_path(path, prefix));
}

TEST(IO_is_lexically_under_path, relative_empty_prefix) {
  const std::filesystem::path path{"foo/bar"};
  const std::filesystem::path prefix{};
  EXPECT_TRUE(sourcemeta::core::is_lexically_under_path(path, prefix));
}

TEST(IO_is_lexically_under_path, relative_string_view_arguments) {
  const std::string_view path{"foo/bar/baz"};
  const std::string_view prefix{"foo/bar"};
  EXPECT_TRUE(sourcemeta::core::is_lexically_under_path(path, prefix));
}

TEST(IO_is_lexically_under_path, relative_string_view_not_under) {
  const std::string_view path{"foo/barbaz"};
  const std::string_view prefix{"foo/bar"};
  EXPECT_FALSE(sourcemeta::core::is_lexically_under_path(path, prefix));
}

#ifndef _WIN32

TEST(IO_is_lexically_under_path, posix_absolute_nested) {
  const std::filesystem::path path{"/foo/bar/baz"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_TRUE(sourcemeta::core::is_lexically_under_path(path, prefix));
}

TEST(IO_is_lexically_under_path, posix_absolute_traversal_into_etc) {
  const std::filesystem::path path{"/foo/../../etc/passwd"};
  const std::filesystem::path prefix{"/foo"};
  EXPECT_FALSE(sourcemeta::core::is_lexically_under_path(path, prefix));
}

TEST(IO_is_lexically_under_path, posix_root_prefix_matches_everything) {
  const std::filesystem::path path{"/foo/bar/baz"};
  const std::filesystem::path prefix{"/"};
  EXPECT_TRUE(sourcemeta::core::is_lexically_under_path(path, prefix));
}

TEST(IO_is_lexically_under_path, posix_relative_vs_absolute) {
  const std::filesystem::path path{"foo/bar"};
  const std::filesystem::path prefix{"/foo"};
  EXPECT_FALSE(sourcemeta::core::is_lexically_under_path(path, prefix));
}

#endif
