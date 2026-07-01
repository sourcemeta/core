#include <sourcemeta/core/test.h>

#include <sourcemeta/core/io.h>

TEST(test_txt) {
  const auto path{std::filesystem::path{STUBS_DIRECTORY} / "test.txt"};
  sourcemeta::core::flush(path);
}

TEST(not_exists) {
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
