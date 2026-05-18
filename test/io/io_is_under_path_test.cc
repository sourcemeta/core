#include <gtest/gtest.h>

#include <sourcemeta/core/io.h>

#ifdef _WIN32

TEST(IO_is_under_path, windows_case_insensitive) {
  const std::filesystem::path path{"C:\\Foo\\Bar"};
  const std::filesystem::path prefix{"C:\\foo"};
  EXPECT_FALSE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, windows_different_drive) {
  const std::filesystem::path path{"D:\\Foo\\Bar"};
  const std::filesystem::path prefix{"C:\\Foo"};
  EXPECT_FALSE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, windows_drive_letter_no_slash) {
  const std::filesystem::path path{"C:folder\\sub"};
  const std::filesystem::path prefix{"C:folder"};
  EXPECT_TRUE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, windows_unc_path) {
  const std::filesystem::path path{"\\\\server\\share\\folder\\file.txt"};
  const std::filesystem::path prefix{"\\\\server\\share\\folder"};
  EXPECT_TRUE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, windows_same_chars_not_component) {
  const std::filesystem::path path{"C:\\foo\\barbaz"};
  const std::filesystem::path prefix{"C:\\foo\\bar"};
  EXPECT_FALSE(sourcemeta::core::is_under_path(path, prefix));
}

#else

TEST(IO_is_under_path, posix_exact_match) {
  const std::filesystem::path path{"/foo/bar"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_TRUE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_nested_dir) {
  const std::filesystem::path path{"/foo/bar/baz"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_TRUE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_same_chars_not_component) {
  const std::filesystem::path path{"/foo/barbaz"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_FALSE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_reverse_substring_attack) {
  const std::filesystem::path path{"/foo"};
  const std::filesystem::path prefix{"/foobar"};
  EXPECT_FALSE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_partial_component_with_dot) {
  const std::filesystem::path path{"/foo.bar/baz"};
  const std::filesystem::path prefix{"/foo"};
  EXPECT_FALSE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_trailing_slash) {
  const std::filesystem::path path{"/foo/bar"};
  const std::filesystem::path prefix{"/foo/bar/"};
  EXPECT_TRUE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_relative_vs_absolute) {
  const std::filesystem::path path{"foo/bar"};
  const std::filesystem::path prefix{"/foo"};
  EXPECT_FALSE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_dot_normalization) {
  const std::filesystem::path path{"/foo/./bar"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_TRUE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_multiple_dot_segments) {
  const std::filesystem::path path{"/foo/./bar/./baz"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_TRUE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_dotdot_normalization) {
  const std::filesystem::path path{"/foo/baz/../bar"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_TRUE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_dotdot_net_under_prefix) {
  const std::filesystem::path path{"/foo/x/../bar/y"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_TRUE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_dotdot_escapes_prefix) {
  const std::filesystem::path path{"/foo/bar/../../etc/passwd"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_FALSE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_dotdot_smuggled_inside_after_match) {
  const std::filesystem::path path{"/foo/bar/x/../../baz"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_FALSE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_dotdot_escape_then_return) {
  const std::filesystem::path path{"/foo/bar/../bar/baz"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_TRUE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_dotdot_escape_then_return_sibling) {
  const std::filesystem::path path{"/foo/bar/../bar2/baz"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_FALSE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_dotdot_at_path_start_clamps_at_root) {
  const std::filesystem::path path{"/../../etc"};
  const std::filesystem::path prefix{"/etc"};
  EXPECT_TRUE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_dotdot_in_prefix_resolved_in_place) {
  const std::filesystem::path path{"/foo/bar/baz"};
  const std::filesystem::path prefix{"/foo/bar/../bar"};
  EXPECT_TRUE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_dotdot_in_prefix_escapes_to_unrelated) {
  const std::filesystem::path path{"/baz/qux"};
  const std::filesystem::path prefix{"/foo/bar/../../../baz"};
  EXPECT_TRUE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_path_traversal_into_etc) {
  const std::filesystem::path path{"/foo/../../etc/passwd"};
  const std::filesystem::path prefix{"/foo"};
  EXPECT_FALSE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_double_slash_path) {
  const std::filesystem::path path{"//foo//bar//baz"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_TRUE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_double_slash_prefix) {
  const std::filesystem::path path{"/foo/bar/baz"};
  const std::filesystem::path prefix{"//foo//bar"};
  EXPECT_TRUE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_only_slashes_path) {
  const std::filesystem::path path{"////"};
  const std::filesystem::path prefix{"/"};
  EXPECT_TRUE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_root_prefix_matches_everything) {
  const std::filesystem::path path{"/foo/bar/baz/qux"};
  const std::filesystem::path prefix{"/"};
  EXPECT_TRUE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_trailing_dot_on_path) {
  const std::filesystem::path path{"/foo/bar/."};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_TRUE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_trailing_dotdot_cancels_last) {
  const std::filesystem::path path{"/foo/bar/baz/.."};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_TRUE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_dot_only_path) {
  const std::filesystem::path path{"/."};
  const std::filesystem::path prefix{"/"};
  EXPECT_TRUE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_complex_traversal_smuggle) {
  const std::filesystem::path path{"/foo/bar/zzz/../../../etc/foo/bar"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_FALSE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_root_path) {
  const std::filesystem::path path{"/"};
  const std::filesystem::path prefix{"/"};
  EXPECT_TRUE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_empty_prefix) {
  const std::filesystem::path path{"/foo/bar"};
  const std::filesystem::path prefix{};
  EXPECT_TRUE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_empty_path) {
  const std::filesystem::path path{};
  const std::filesystem::path prefix{"/foo"};
  EXPECT_FALSE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_empty_empty) {
  const std::filesystem::path path{};
  const std::filesystem::path prefix{};
  EXPECT_TRUE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_prefix_longer_than_path) {
  const std::filesystem::path path{"/foo"};
  const std::filesystem::path prefix{"/foo/bar/baz"};
  EXPECT_FALSE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_prefix_one_component_longer) {
  const std::filesystem::path path{"/foo/bar"};
  const std::filesystem::path prefix{"/foo/bar/baz"};
  EXPECT_FALSE(sourcemeta::core::is_under_path(path, prefix));
}

TEST(IO_is_under_path, posix_unrelated_branches) {
  const std::filesystem::path path{"/foo/bar"};
  const std::filesystem::path prefix{"/baz"};
  EXPECT_FALSE(sourcemeta::core::is_under_path(path, prefix));
}

#endif
