#include <gtest/gtest.h>

#include <sourcemeta/core/io.h>

TEST(IO_flush, test_txt) {
  const auto path{std::filesystem::path{TEST_DIRECTORY} / "test.txt"};
  sourcemeta::core::flush(path);
  SUCCEED();
}

TEST(IO_flush, not_exists) {
  const auto path{std::filesystem::path{TEST_DIRECTORY} / "foo.txt"};

  try {
    sourcemeta::core::flush(path);
    FAIL();
  } catch (const std::filesystem::filesystem_error &error) {
    EXPECT_EQ(error.path1(), path);
    EXPECT_EQ(error.code().default_error_condition(),
              std::errc::no_such_file_or_directory);

#if defined(_WIN32)
    // Windows uses system_category()
    EXPECT_EQ(&error.code().category(), &std::system_category());
#else
    // POSIX uses generic_category()
    EXPECT_EQ(&error.code().category(), &std::generic_category());
#endif
  } catch (...) {
    FAIL();
  }
}
