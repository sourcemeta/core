#include <gtest/gtest.h>

#include <sourcemeta/core/io.h>

#include <algorithm>
#include <sstream>

TEST(IO_read_file, text_file) {
  auto stream{sourcemeta::core::read_file(
      std::filesystem::path{TEST_DIRECTORY} / "test.txt")};
  std::ostringstream contents;
  contents << stream.rdbuf();
  auto result{contents.str()};
  // Strip `\r` in the tests, as there is no portable way of normalising this at
  // the library level
  result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
  EXPECT_EQ(result, "Hello World\n");
}

TEST(IO_read_file, directory) {
  try {
    sourcemeta::core::read_file(std::filesystem::path{TEST_DIRECTORY});
  } catch (const std::filesystem::filesystem_error &error) {
    EXPECT_EQ(error.code(), std::errc::is_a_directory);
    EXPECT_EQ(error.path1(), std::filesystem::path{TEST_DIRECTORY});
  } catch (...) {
    FAIL() << "The parse function was expected to throw a filesystem error";
  }
}
