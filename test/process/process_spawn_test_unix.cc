#include <gtest/gtest.h>

#include <sourcemeta/core/process.h>

#include <filesystem> // std::filesystem::path

TEST(Process_spawn, usr_bin_true_returns_zero) {
  const int exit_code{sourcemeta::core::spawn("/usr/bin/true", {})};
  EXPECT_EQ(exit_code, 0);
}

TEST(Process_spawn, usr_bin_false_returns_one) {
  const int exit_code{sourcemeta::core::spawn("/usr/bin/false", {})};
  EXPECT_EQ(exit_code, 1);
}

TEST(Process_spawn, true_without_path_returns_zero) {
  const int exit_code{sourcemeta::core::spawn("true", {})};
  EXPECT_EQ(exit_code, 0);
}

TEST(Process_spawn, sh_exit_with_specific_code) {
  const int exit_code{sourcemeta::core::spawn("/bin/sh", {"-c", "exit 42"})};
  EXPECT_EQ(exit_code, 42);
}

TEST(Process_spawn, sh_exit_with_different_code) {
  const int exit_code{sourcemeta::core::spawn("/bin/sh", {"-c", "exit 7"})};
  EXPECT_EQ(exit_code, 7);
}

TEST(Process_spawn, test_command_failing_condition) {
  const int exit_code{sourcemeta::core::spawn("/bin/test", {"1", "-eq", "2"})};
  EXPECT_EQ(exit_code, 1);
}

TEST(Process_spawn, test_command_passing_condition) {
  const int exit_code{sourcemeta::core::spawn("/bin/test", {"1", "-eq", "1"})};
  EXPECT_EQ(exit_code, 0);
}

TEST(Process_spawn, nonexistent_program_throws_exception) {
  const auto program{"/bin/this_program_definitely_does_not_exist"};
  EXPECT_THROW(
      {
        try {
          sourcemeta::core::spawn(program, {});
        } catch (
            const sourcemeta::core::ProcessProgramNotNotFoundError &error) {
          EXPECT_EQ(error.program(), program);
          throw;
        }
      },
      sourcemeta::core::ProcessProgramNotNotFoundError);
}

TEST(Process_spawn, echo_with_arguments) {
  const int exit_code{sourcemeta::core::spawn("/bin/echo", {"hello", "world"})};
  EXPECT_EQ(exit_code, 0);
}

TEST(Process_spawn, pwd_with_custom_directory) {
  const int exit_code{
      sourcemeta::core::spawn("pwd", {}, std::filesystem::path{"/tmp"})};
  EXPECT_EQ(exit_code, 0);
}

TEST(Process_spawn, pwd_with_current_directory) {
  const int exit_code{sourcemeta::core::spawn("pwd", {})};
  EXPECT_EQ(exit_code, 0);
}
