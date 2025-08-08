#include <gtest/gtest.h>

#include <sourcemeta/core/io.h>

#ifdef _WIN32

TEST(IO_starts_with, windows_case_insensitive) {
  const std::filesystem::path path{"C:\\Foo\\Bar"};
  const std::filesystem::path prefix{"C:\\foo"};
  EXPECT_FALSE(sourcemeta::core::starts_with(path, prefix));
}

TEST(IO_starts_with, windows_different_drive) {
  const std::filesystem::path path{"D:\\Foo\\Bar"};
  const std::filesystem::path prefix{"C:\\Foo"};
  EXPECT_FALSE(sourcemeta::core::starts_with(path, prefix));
}

TEST(IO_starts_with, windows_drive_letter_no_slash) {
  const std::filesystem::path path{"C:folder\\sub"};
  const std::filesystem::path prefix{"C:folder"};
  EXPECT_TRUE(sourcemeta::core::starts_with(path, prefix));
}

TEST(IO_starts_with, windows_unc_path) {
  const std::filesystem::path path{"\\\\server\\share\\folder\\file.txt"};
  const std::filesystem::path prefix{"\\\\server\\share\\folder"};
  EXPECT_TRUE(sourcemeta::core::starts_with(path, prefix));
}

TEST(IO_starts_with, windows_same_chars_not_component) {
  const std::filesystem::path path{"C:\\foo\\barbaz"};
  const std::filesystem::path prefix{"C:\\foo\\bar"};
  EXPECT_FALSE(sourcemeta::core::starts_with(path, prefix));
}

#else

TEST(IO_starts_with, posix_exact_match) {
  const std::filesystem::path path{"/foo/bar"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_TRUE(sourcemeta::core::starts_with(path, prefix));
}

TEST(IO_starts_with, posix_nested_dir) {
  const std::filesystem::path path{"/foo/bar/baz"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_TRUE(sourcemeta::core::starts_with(path, prefix));
}

TEST(IO_starts_with, posix_same_chars_not_component) {
  const std::filesystem::path path{"/foo/barbaz"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_FALSE(sourcemeta::core::starts_with(path, prefix));
}

TEST(IO_starts_with, posix_trailing_slash) {
  const std::filesystem::path path{"/foo/bar"};
  const std::filesystem::path prefix{"/foo/bar/"};
  EXPECT_FALSE(sourcemeta::core::starts_with(path, prefix));
}

TEST(IO_starts_with, posix_relative_vs_absolute) {
  const std::filesystem::path path{"foo/bar"};
  const std::filesystem::path prefix{"/foo"};
  EXPECT_FALSE(sourcemeta::core::starts_with(path, prefix));
}

TEST(IO_starts_with, posix_dot_normalization) {
  const std::filesystem::path path{"/foo/./bar"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_FALSE(sourcemeta::core::starts_with(path, prefix));
}

TEST(IO_starts_with, posix_dotdot_normalization) {
  const std::filesystem::path path{"/foo/baz/../bar"};
  const std::filesystem::path prefix{"/foo/bar"};
  EXPECT_FALSE(sourcemeta::core::starts_with(path, prefix));
}

TEST(IO_starts_with, posix_root_path) {
  const std::filesystem::path path{"/"};
  const std::filesystem::path prefix{"/"};
  EXPECT_TRUE(sourcemeta::core::starts_with(path, prefix));
}

TEST(IO_starts_with, posix_empty_prefix) {
  const std::filesystem::path path{"/foo/bar"};
  const std::filesystem::path prefix{};
  EXPECT_TRUE(sourcemeta::core::starts_with(path, prefix));
}

TEST(IO_starts_with, posix_empty_path) {
  const std::filesystem::path path{};
  const std::filesystem::path prefix{"/foo"};
  EXPECT_FALSE(sourcemeta::core::starts_with(path, prefix));
}

TEST(IO_starts_with, posix_empty_empty) {
  const std::filesystem::path path{};
  const std::filesystem::path prefix{};
  EXPECT_TRUE(sourcemeta::core::starts_with(path, prefix));
}

#endif
