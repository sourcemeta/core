#include <sourcemeta/core/io.h>
#include <sourcemeta/core/test.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <system_error>

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

class IOReadFileTest {
protected:
  IOReadFileTest() { std::filesystem::create_directories(this->workspace_); }

  ~IOReadFileTest() {
    std::error_code error;
    std::filesystem::permissions(this->workspace_ / "locked.txt",
                                 std::filesystem::perms::owner_all,
                                 std::filesystem::perm_options::replace, error);
    std::filesystem::remove_all(this->workspace_, error);
  }

  // The tests are always sequential, so using the same path is safe
  std::filesystem::path workspace_{std::filesystem::path{BUILD_DIRECTORY} /
                                   "sourcemeta_core_io_read_file_test"};
};

// A file that exists but cannot be opened is a different refusal from one that
// is missing or is a directory
TEST_F(IOReadFileTest, unreadable_file) {
  const auto path{this->workspace_ / "locked.txt"};
  {
    std::ofstream handle{path};
    handle << "secret";
  }
  std::filesystem::permissions(path, std::filesystem::perms::none,
                               std::filesystem::perm_options::replace);

  try {
    sourcemeta::core::read_file(path);
    FAIL();
  } catch (const sourcemeta::core::IOFilePermissionError &error) {
    EXPECT_EQ(error.path(), path);
  } catch (...) {
    FAIL();
  }
}
