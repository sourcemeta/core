#include <sourcemeta/core/test.h>

#include <sourcemeta/core/io.h>

#include <algorithm>
#include <sstream>

TEST(text_file) {
  auto stream{sourcemeta::core::read_file(
      std::filesystem::path{STUBS_DIRECTORY} / "test.txt")};
  std::ostringstream contents;
  contents << stream.rdbuf();
  auto result{contents.str()};
  // Strip `\r` in the tests, as there is no portable way of normalising this at
  // the library level
  result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
  EXPECT_EQ(result, "Hello World\n");
}

TEST(directory) {
  const std::filesystem::path path{STUBS_DIRECTORY};
  try {
    sourcemeta::core::read_file(path);
    FAIL();
  } catch (const sourcemeta::core::IOIsADirectoryError &error) {
    EXPECT_EQ(error.path(), path);
  } catch (...) {
    FAIL();
  }
}

TEST(not_exists) {
  const auto path{std::filesystem::path{STUBS_DIRECTORY} / "missing.txt"};
  try {
    sourcemeta::core::read_file(path);
    FAIL();
  } catch (const sourcemeta::core::IOFileNotFoundError &error) {
    EXPECT_EQ(error.path(), path);
  } catch (...) {
    FAIL();
  }
}
