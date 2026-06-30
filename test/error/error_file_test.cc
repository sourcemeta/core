#include <sourcemeta/core/test.h>

#include <sourcemeta/core/error.h>

#include <filesystem>
#include <stdexcept>
#include <string>

TEST(stores_path) {
  const std::filesystem::path path{std::filesystem::temp_directory_path()};
  try {
    throw sourcemeta::core::FileError<std::runtime_error>(path, "test message");
  } catch (const sourcemeta::core::FileError<std::runtime_error> &error) {
    EXPECT_EQ(error.path(), path);
  }
}

TEST(inherits_what) {
  const std::filesystem::path path{std::filesystem::temp_directory_path()};
  try {
    throw sourcemeta::core::FileError<std::runtime_error>(path, "test message");
  } catch (const sourcemeta::core::FileError<std::runtime_error> &error) {
    EXPECT_STREQ(error.what(), "test message");
  }
}

TEST(catchable_as_base) {
  const std::filesystem::path path{std::filesystem::temp_directory_path()};
  try {
    throw sourcemeta::core::FileError<std::runtime_error>(path, "test message");
  } catch (const std::runtime_error &error) {
    EXPECT_STREQ(error.what(), "test message");
  }
}

TEST(catchable_as_exception) {
  const std::filesystem::path path{std::filesystem::temp_directory_path()};
  try {
    throw sourcemeta::core::FileError<std::runtime_error>(path, "test message");
  } catch (const std::exception &error) {
    EXPECT_STREQ(error.what(), "test message");
  }
}

TEST(works_with_logic_error) {
  const std::filesystem::path path{std::filesystem::temp_directory_path()};
  try {
    throw sourcemeta::core::FileError<std::logic_error>(path, "logic failure");
  } catch (const sourcemeta::core::FileError<std::logic_error> &error) {
    EXPECT_EQ(error.path(), path);
    EXPECT_STREQ(error.what(), "logic failure");
  }
}
