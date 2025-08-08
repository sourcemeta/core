#include <gtest/gtest.h>

#include <sourcemeta/core/io.h>

TEST(IO_weakly_canonical, test_txt) {
  const auto path{sourcemeta::core::weakly_canonical(
      std::filesystem::path{TEST_DIRECTORY} / ".." / "io" / "test.txt")};
  EXPECT_EQ(path, std::filesystem::path{TEST_DIRECTORY} / "test.txt");
}

TEST(IO_weakly_canonical, not_exists) {
  const auto path{sourcemeta::core::weakly_canonical(
      std::filesystem::path{TEST_DIRECTORY} / "foo.txt")};
  EXPECT_EQ(path, std::filesystem::path{TEST_DIRECTORY} / "foo.txt");
}
