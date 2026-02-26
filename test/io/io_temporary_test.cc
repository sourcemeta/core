#include <gtest/gtest.h>

#include <sourcemeta/core/io.h>

#include <filesystem> // std::filesystem
#include <fstream>    // std::ofstream
#include <string>     // std::string

TEST(IO_TemporaryDirectory, creates_directory) {
  const auto parent{std::filesystem::temp_directory_path()};
  const sourcemeta::core::TemporaryDirectory temporary{parent, ".test-"};
  EXPECT_TRUE(std::filesystem::exists(temporary.path()));
  EXPECT_TRUE(std::filesystem::is_directory(temporary.path()));
}

TEST(IO_TemporaryDirectory, unique_names) {
  const auto parent{std::filesystem::temp_directory_path()};
  const sourcemeta::core::TemporaryDirectory first{parent, ".test-"};
  const sourcemeta::core::TemporaryDirectory second{parent, ".test-"};
  EXPECT_NE(first.path(), second.path());
}

TEST(IO_TemporaryDirectory, custom_prefix) {
  const auto parent{std::filesystem::temp_directory_path()};
  const sourcemeta::core::TemporaryDirectory temporary{parent,
                                                       ".my-test-prefix-"};
  const auto filename{temporary.path().filename().string()};
  EXPECT_TRUE(filename.starts_with(".my-test-prefix-"));
}

TEST(IO_TemporaryDirectory, removed_on_destruction) {
  std::filesystem::path path_copy;
  {
    const sourcemeta::core::TemporaryDirectory temporary{
        std::filesystem::temp_directory_path(), ".test-"};
    path_copy = temporary.path();
    EXPECT_TRUE(std::filesystem::exists(path_copy));
  }
  EXPECT_FALSE(std::filesystem::exists(path_copy));
}

TEST(IO_TemporaryDirectory, removed_on_destruction_non_empty) {
  std::filesystem::path path_copy;
  {
    const sourcemeta::core::TemporaryDirectory temporary{
        std::filesystem::temp_directory_path(), ".test-"};
    path_copy = temporary.path();
    std::filesystem::create_directory(path_copy / "subdir");
    std::ofstream{path_copy / "file.txt"} << "hello";
    std::ofstream{path_copy / "subdir" / "nested.txt"} << "world";
    EXPECT_TRUE(std::filesystem::exists(path_copy / "subdir" / "nested.txt"));
  }
  EXPECT_FALSE(std::filesystem::exists(path_copy));
}

TEST(IO_TemporaryDirectory, inside_parent) {
  const auto parent{
      std::filesystem::canonical(std::filesystem::temp_directory_path())};
  const sourcemeta::core::TemporaryDirectory temporary{parent, ".test-"};
  EXPECT_EQ(temporary.path().parent_path(), parent);
}

TEST(IO_TemporaryDirectory, creates_nonexistent_parent) {
  const sourcemeta::core::TemporaryDirectory unique_parent{
      std::filesystem::temp_directory_path(), ".test-parent-"};
  const auto parent{unique_parent.path() / "nonexistent"};
  EXPECT_FALSE(std::filesystem::exists(parent));
  const sourcemeta::core::TemporaryDirectory temporary{parent, ".test-"};
  EXPECT_TRUE(std::filesystem::exists(temporary.path()));
  EXPECT_TRUE(std::filesystem::is_directory(temporary.path()));
}

TEST(IO_TemporaryDirectory, creates_nested_nonexistent_parents) {
  const sourcemeta::core::TemporaryDirectory unique_parent{
      std::filesystem::temp_directory_path(), ".test-parent-"};
  const auto parent{unique_parent.path() / "nested_a" / "nested_b"};
  EXPECT_FALSE(std::filesystem::exists(parent));
  const sourcemeta::core::TemporaryDirectory temporary{parent, ".test-"};
  EXPECT_TRUE(std::filesystem::exists(temporary.path()));
  EXPECT_TRUE(std::filesystem::is_directory(temporary.path()));
}

TEST(IO_TemporaryDirectory, parent_is_a_file) {
  const sourcemeta::core::TemporaryDirectory unique_parent{
      std::filesystem::temp_directory_path(), ".test-parent-"};
  const auto file_path{unique_parent.path() / "test_parent_file"};
  std::ofstream{file_path};
  EXPECT_TRUE(std::filesystem::exists(file_path));
  EXPECT_FALSE(std::filesystem::is_directory(file_path));

  try {
    sourcemeta::core::TemporaryDirectory(file_path, ".test-");
    FAIL();
  } catch (const std::filesystem::filesystem_error &error) {
    EXPECT_EQ(error.path1(), file_path);
    EXPECT_EQ(error.code(), std::make_error_code(std::errc::not_a_directory));
  }
}
