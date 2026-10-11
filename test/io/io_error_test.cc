#include <sourcemeta/core/io.h>
#include <sourcemeta/core/test.h>

#include <filesystem> // std::filesystem::path

TEST(is_a_directory_error_message) {
  const sourcemeta::core::IOIsADirectoryError error{"/foo"};
  EXPECT_STREQ(error.what(), "Expected a file but got a directory");
}

TEST(is_a_directory_error_path) {
  const sourcemeta::core::IOIsADirectoryError error{"/foo"};
  EXPECT_EQ(error.path(), std::filesystem::path{"/foo"});
}

TEST(read_out_of_bounds_error_message) {
  const sourcemeta::core::IOReadOutOfBoundsError error{};
  EXPECT_STREQ(error.what(), "Read past the end of the underlying data");
}

TEST(stream_write_error_message) {
  const sourcemeta::core::IOStreamWriteError error{};
  EXPECT_STREQ(error.what(), "Failed to write to stream");
}
