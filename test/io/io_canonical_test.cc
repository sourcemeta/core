#include <gtest/gtest.h>

#include <sourcemeta/core/io.h>

TEST(IO_canonical, test_txt) {
  const auto path{sourcemeta::core::canonical(
      std::filesystem::path{TEST_DIRECTORY} / ".." / "io" / "test.txt")};
  EXPECT_EQ(path, std::filesystem::path{TEST_DIRECTORY} / "test.txt");
}

TEST(IO_canonical, not_exists) {
  EXPECT_THROW(sourcemeta::core::canonical(
                   std::filesystem::path{TEST_DIRECTORY} / "foo.txt"),
               std::filesystem::filesystem_error);
}
