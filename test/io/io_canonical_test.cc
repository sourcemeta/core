#include <gtest/gtest.h>

#include <sourcemeta/core/io.h>

TEST(IO_canonical, test_txt) {
  const auto path{sourcemeta::core::canonical(
      std::filesystem::path{STUBS_DIRECTORY} / "test.txt")};
  EXPECT_EQ(path, std::filesystem::path{STUBS_DIRECTORY} / "test.txt");
}

TEST(IO_canonical, not_exists) {
  const auto path{std::filesystem::path{STUBS_DIRECTORY} / "foo.txt"};
  try {
    sourcemeta::core::canonical(path);
    FAIL();
  } catch (const sourcemeta::core::IOFileNotFoundError &error) {
    EXPECT_EQ(error.path(), path);
  } catch (...) {
    FAIL();
  }
}

#if !defined(_WIN32)
TEST(IO_canonical, unmapped_error_surfaces_as_filesystem_error) {
  const sourcemeta::core::TemporaryDirectory workspace{
      std::filesystem::path{BUILD_DIRECTORY}, ".test-canonical-"};
  const auto loop_path{workspace.path() / "loop"};
  // A symlink pointing at itself produces a loop error which is neither
  // no_such_file_or_directory nor any of the other mapped categories.
  std::filesystem::create_symlink(loop_path, loop_path);

  EXPECT_THROW(sourcemeta::core::canonical(loop_path),
               std::filesystem::filesystem_error);
}
#endif
