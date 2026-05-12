#include <gtest/gtest.h>

#include <sourcemeta/core/io.h>

TEST(IO_flush, test_txt) {
  const auto path{std::filesystem::path{STUBS_DIRECTORY} / "test.txt"};
  sourcemeta::core::flush(path);
  SUCCEED();
}

TEST(IO_flush, not_exists) {
  const auto path{std::filesystem::path{STUBS_DIRECTORY} / "foo.txt"};

  try {
    sourcemeta::core::flush(path);
    FAIL();
  } catch (const sourcemeta::core::IOFileNotFoundError &error) {
    EXPECT_EQ(error.path(), path);
  } catch (...) {
    FAIL();
  }
}
