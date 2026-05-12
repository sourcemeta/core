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
class IOCanonicalTest : public ::testing::Test {
protected:
  void SetUp() override {
    std::filesystem::create_directories(this->workspace);
  }

  void TearDown() override { std::filesystem::remove_all(this->workspace); }

  // The tests are always sequential, so using the same path is safe
  std::filesystem::path workspace{std::filesystem::path{BUILD_DIRECTORY} /
                                  "sourcemeta_core_io_canonical_test"};
};

TEST_F(IOCanonicalTest, unmapped_error_surfaces_as_filesystem_error) {
  const auto loop_path{this->workspace / "loop"};
  // A symlink pointing at itself produces a loop error which is neither
  // no_such_file_or_directory nor any of the other mapped categories.
  std::filesystem::create_symlink(loop_path, loop_path);

  EXPECT_THROW(sourcemeta::core::canonical(loop_path),
               std::filesystem::filesystem_error);
}
#endif
