#include <gtest/gtest.h>

#include <sourcemeta/core/argv.h>

TEST(ARGV_executable_path, relative_argc_argv) {
  const char *const argv[] = {"foo"};
  const auto &program{sourcemeta::core::program_path(1, argv)};
  EXPECT_TRUE(program.is_absolute());
  EXPECT_TRUE(std::filesystem::exists(program));
  EXPECT_TRUE(std::filesystem::is_regular_file(program));
  EXPECT_EQ(program, std::filesystem::path{EXECUTABLE_PATH});
  EXPECT_EQ(program, std::filesystem::canonical(program));
}

TEST(ARGV_executable_path, absolute_argc_argv) {
  const char *const argv[] = {EXECUTABLE_PATH};
  const auto &program{sourcemeta::core::program_path(1, argv)};
  EXPECT_TRUE(program.is_absolute());
  EXPECT_TRUE(std::filesystem::exists(program));
  EXPECT_TRUE(std::filesystem::is_regular_file(program));
  EXPECT_EQ(program, std::filesystem::path{EXECUTABLE_PATH});
  EXPECT_EQ(program, std::filesystem::canonical(program));
}

TEST(ARGV_executable_path, idempotency_relative_argc_argv) {
  const char *const argv[] = {"foo"};
  const auto &program_1{sourcemeta::core::program_path(1, argv)};
  const auto &program_2{sourcemeta::core::program_path(1, argv)};
  const auto &program_3{sourcemeta::core::program_path(1, argv)};
  EXPECT_EQ(program_1, program_2);
  EXPECT_EQ(program_2, program_3);
  EXPECT_EQ(program_3, program_1);
}
