#include <sourcemeta/core/io.h>
#include <sourcemeta/core/test.h>

#include <filesystem>   // std::filesystem::path
#include <system_error> // std::error_code, std::errc,
                        // std::generic_category

TEST(fileview_error_message) {
  const sourcemeta::core::FileViewError error{"Failed to memory-map the file",
                                              "/foo/bar.json"};
  EXPECT_STREQ(error.what(), "Failed to memory-map the file");
}

TEST(fileview_error_path) {
  const sourcemeta::core::FileViewError error{"Failed to memory-map the file",
                                              "/foo/bar.json"};
  EXPECT_EQ(error.path(), std::filesystem::path{"/foo/bar.json"});
}

TEST(file_not_found_error_message) {
  const sourcemeta::core::IOFileNotFoundError error{"/foo/bar.json"};
  EXPECT_STREQ(error.what(), "File not found");
}

TEST(file_not_found_error_path) {
  const sourcemeta::core::IOFileNotFoundError error{"/foo/bar.json"};
  EXPECT_EQ(error.path(), std::filesystem::path{"/foo/bar.json"});
}

TEST(file_permission_error_message) {
  const sourcemeta::core::IOFilePermissionError error{"/foo/bar.json"};
  EXPECT_STREQ(error.what(), "Permission denied");
}

TEST(file_permission_error_path) {
  const sourcemeta::core::IOFilePermissionError error{"/foo/bar.json"};
  EXPECT_EQ(error.path(), std::filesystem::path{"/foo/bar.json"});
}

TEST(is_a_directory_error_message) {
  const sourcemeta::core::IOIsADirectoryError error{"/foo"};
  EXPECT_STREQ(error.what(), "Expected a file but got a directory");
}

TEST(is_a_directory_error_path) {
  const sourcemeta::core::IOIsADirectoryError error{"/foo"};
  EXPECT_EQ(error.path(), std::filesystem::path{"/foo"});
}

TEST(not_a_directory_error_message) {
  const sourcemeta::core::IONotADirectoryError error{"/foo/bar.json"};
  EXPECT_STREQ(error.what(), "Expected a directory but got a file");
}

TEST(not_a_directory_error_path) {
  const sourcemeta::core::IONotADirectoryError error{"/foo/bar.json"};
  EXPECT_EQ(error.path(), std::filesystem::path{"/foo/bar.json"});
}

TEST(file_already_exists_error_message) {
  const sourcemeta::core::IOFileAlreadyExistsError error{"/foo/bar.json"};
  EXPECT_STREQ(error.what(), "File already exists");
}

TEST(file_already_exists_error_path) {
  const sourcemeta::core::IOFileAlreadyExistsError error{"/foo/bar.json"};
  EXPECT_EQ(error.path(), std::filesystem::path{"/foo/bar.json"});
}

TEST(read_out_of_bounds_error_message) {
  const sourcemeta::core::IOReadOutOfBoundsError error{};
  EXPECT_STREQ(error.what(), "Read past the end of the underlying data");
}

TEST(stream_write_error_message) {
  const sourcemeta::core::IOStreamWriteError error{};
  EXPECT_STREQ(error.what(), "Failed to write to stream");
}

TEST(filesystem_error_message) {
  const sourcemeta::core::IOFilesystemError error{
      "Failed to rename the directory", "/foo/bar",
      std::error_code{static_cast<int>(std::errc::permission_denied),
                      std::generic_category()}};
  EXPECT_STREQ(error.what(), "Failed to rename the directory");
}

TEST(filesystem_error_path) {
  const sourcemeta::core::IOFilesystemError error{
      "Failed to rename the directory", "/foo/bar",
      std::error_code{static_cast<int>(std::errc::permission_denied),
                      std::generic_category()}};
  EXPECT_EQ(error.path(), std::filesystem::path{"/foo/bar"});
}

TEST(filesystem_error_code) {
  const sourcemeta::core::IOFilesystemError error{
      "Failed to rename the directory", "/foo/bar",
      std::error_code{static_cast<int>(std::errc::permission_denied),
                      std::generic_category()}};
  EXPECT_EQ(error.code(), std::errc::permission_denied);
}

TEST(filesystem_error_without_another_path) {
  const sourcemeta::core::IOFilesystemError error{
      "Failed to rename the directory", "/foo/bar",
      std::error_code{static_cast<int>(std::errc::permission_denied),
                      std::generic_category()}};
  EXPECT_EQ(error.other(), std::filesystem::path{});
}

TEST(filesystem_error_with_two_paths) {
  const sourcemeta::core::IOFilesystemError error{
      "Failed to atomically swap the directories", "/foo/bar", "/foo/baz",
      std::error_code{static_cast<int>(std::errc::cross_device_link),
                      std::generic_category()}};
  EXPECT_STREQ(error.what(), "Failed to atomically swap the directories");
  EXPECT_EQ(error.path(), std::filesystem::path{"/foo/bar"});
  EXPECT_EQ(error.other(), std::filesystem::path{"/foo/baz"});
  EXPECT_EQ(error.code(), std::errc::cross_device_link);
}
