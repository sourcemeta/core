#include <sourcemeta/core/io.h>
#include <sourcemeta/core/test.h>

#ifndef _WIN32

TEST(posix_nested_dir) {
  const std::filesystem::path path{"/foo/bar/baz"};
  const std::filesystem::path prefix{"/foo"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix),
            std::filesystem::path{"bar/baz"});
}

TEST(posix_single_component_suffix) {
  const std::filesystem::path path{"/foo/bar"};
  const std::filesystem::path prefix{"/foo"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix),
            std::filesystem::path{"bar"});
}

TEST(posix_exact_match) {
  const std::filesystem::path path{"/foo/bar"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix),
            std::filesystem::path{});
}

TEST(posix_not_under_same_chars) {
  const std::filesystem::path path{"/foo/barbaz"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix), path);
}

TEST(posix_not_under_unrelated) {
  const std::filesystem::path path{"/other/place"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix), path);
}

TEST(posix_reverse_substring_returns_original) {
  const std::filesystem::path path{"/foo"};
  const std::filesystem::path prefix{"/foobar"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix), path);
}

TEST(posix_partial_component_with_dot_returns_original) {
  const std::filesystem::path path{"/foo.bar/baz"};
  const std::filesystem::path prefix{"/foo"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix), path);
}

TEST(posix_dot_normalization) {
  const std::filesystem::path path{"/foo/./bar/baz"};
  const std::filesystem::path prefix{"/foo"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix),
            std::filesystem::path{"bar/baz"});
}

TEST(posix_dotdot_normalization) {
  const std::filesystem::path path{"/foo/x/../bar/baz"};
  const std::filesystem::path prefix{"/foo"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix),
            std::filesystem::path{"bar/baz"});
}

TEST(posix_dotdot_suffix_is_clean) {
  const std::filesystem::path path{"/foo/x/../bar/y"};
  const std::filesystem::path prefix{"/foo/bar"};
  const auto result{sourcemeta::core::strip_path_prefix(path, prefix)};
  EXPECT_EQ(result, std::filesystem::path{"y"});
  EXPECT_EQ(result.string().find(".."), std::string::npos);
}

TEST(posix_dotdot_escapes_prefix_returns_original) {
  const std::filesystem::path path{"/foo/bar/../../etc/passwd"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix), path);
}

TEST(posix_dotdot_smuggled_returns_original) {
  const std::filesystem::path path{"/foo/bar/x/../../baz"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix), path);
}

TEST(posix_dotdot_escape_then_return) {
  const std::filesystem::path path{"/foo/bar/../bar/baz"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix),
            std::filesystem::path{"baz"});
}

TEST(posix_dotdot_escape_to_sibling_returns_original) {
  const std::filesystem::path path{"/foo/bar/../bar2/baz"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix), path);
}

TEST(posix_dotdot_at_path_start_clamps_at_root) {
  const std::filesystem::path path{"/../../foo/bar"};
  const std::filesystem::path prefix{"/foo"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix),
            std::filesystem::path{"bar"});
}

TEST(posix_dotdot_in_prefix_resolved_in_place) {
  const std::filesystem::path path{"/foo/bar/baz"};
  const std::filesystem::path prefix{"/foo/bar/../bar"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix),
            std::filesystem::path{"baz"});
}

TEST(posix_dotdot_in_prefix_escapes_to_unrelated) {
  const std::filesystem::path path{"/baz/qux"};
  const std::filesystem::path prefix{"/foo/bar/../../../baz"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix),
            std::filesystem::path{"qux"});
}

TEST(posix_traversal_into_etc_returns_original) {
  const std::filesystem::path path{"/foo/../../etc/passwd"};
  const std::filesystem::path prefix{"/foo"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix), path);
}

TEST(posix_complex_traversal_smuggle_returns_original) {
  const std::filesystem::path path{"/foo/bar/zzz/../../../etc/foo/bar"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix), path);
}

TEST(posix_double_slash_path) {
  const std::filesystem::path path{"//foo//bar//baz"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix),
            std::filesystem::path{"baz"});
}

TEST(posix_double_slash_prefix) {
  const std::filesystem::path path{"/foo/bar/baz"};
  const std::filesystem::path prefix{"//foo//bar"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix),
            std::filesystem::path{"baz"});
}

TEST(posix_trailing_slash_on_prefix) {
  const std::filesystem::path path{"/foo/bar/baz"};
  const std::filesystem::path prefix{"/foo/bar/"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix),
            std::filesystem::path{"baz"});
}

TEST(posix_trailing_dot_on_path) {
  const std::filesystem::path path{"/foo/bar/."};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix),
            std::filesystem::path{});
}

TEST(posix_trailing_dotdot_cancels_last) {
  const std::filesystem::path path{"/foo/bar/baz/.."};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix),
            std::filesystem::path{});
}

TEST(posix_root_prefix_returns_full_path_components) {
  const std::filesystem::path path{"/foo/bar/baz"};
  const std::filesystem::path prefix{"/"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix),
            std::filesystem::path{"foo/bar/baz"});
}

TEST(posix_root_path) {
  const std::filesystem::path path{"/"};
  const std::filesystem::path prefix{"/"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix),
            std::filesystem::path{});
}

TEST(posix_empty_path) {
  const std::filesystem::path path{};
  const std::filesystem::path prefix{"/foo"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix), path);
}

TEST(posix_empty_empty) {
  const std::filesystem::path path{};
  const std::filesystem::path prefix{};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix),
            std::filesystem::path{});
}

TEST(posix_prefix_longer_than_path) {
  const std::filesystem::path path{"/foo"};
  const std::filesystem::path prefix{"/foo/bar/baz"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix), path);
}

TEST(posix_unrelated_branches) {
  const std::filesystem::path path{"/foo/bar"};
  const std::filesystem::path prefix{"/baz"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix), path);
}

TEST(posix_returned_suffix_has_no_dotdot) {
  const std::filesystem::path path{"/foo/bar/zzz/../baz"};
  const std::filesystem::path prefix{"/foo/bar"};
  const auto result{sourcemeta::core::strip_path_prefix(path, prefix)};
  EXPECT_EQ(result, std::filesystem::path{"baz"});
  EXPECT_EQ(result.string().find(".."), std::string::npos);
}

TEST(posix_returned_suffix_has_no_leading_separator) {
  const std::filesystem::path path{"/foo/bar/baz"};
  const std::filesystem::path prefix{"/foo/bar"};
  const auto result{sourcemeta::core::strip_path_prefix(path, prefix)};
  EXPECT_FALSE(result.is_absolute());
}

TEST(posix_only_slashes_under_root) {
  const std::filesystem::path path{"////"};
  const std::filesystem::path prefix{"/"};
  EXPECT_EQ(sourcemeta::core::strip_path_prefix(path, prefix),
            std::filesystem::path{});
}

#endif
