#include <gtest/gtest.h>

#include <sourcemeta/core/process.h>

#include <filesystem> // std::filesystem::path

TEST(Process_spawn, cmd_exit_zero_returns_zero) {
  const int exit_code{sourcemeta::core::spawn("cmd.exe", {"/c", "exit 0"})};
  EXPECT_EQ(exit_code, 0);
}

TEST(Process_spawn, cmd_exit_one_returns_one) {
  const int exit_code{sourcemeta::core::spawn("cmd.exe", {"/c", "exit 1"})};
  EXPECT_EQ(exit_code, 1);
}

TEST(Process_spawn, cmd_exit_with_specific_code) {
  const int exit_code{sourcemeta::core::spawn("cmd.exe", {"/c", "exit 42"})};
  EXPECT_EQ(exit_code, 42);
}

TEST(Process_spawn, cmd_exit_with_different_code) {
  const int exit_code{sourcemeta::core::spawn("cmd.exe", {"/c", "exit 7"})};
  EXPECT_EQ(exit_code, 7);
}

TEST(Process_spawn, where_command_success) {
  // where.exe should successfully find cmd.exe
  const int exit_code{sourcemeta::core::spawn("where.exe", {"cmd.exe"})};
  EXPECT_EQ(exit_code, 0);
}

TEST(Process_spawn, where_command_not_found) {
  // where.exe should fail to find a nonexistent program
  const int exit_code{sourcemeta::core::spawn(
      "where.exe", {"this_program_definitely_does_not_exist"})};
  EXPECT_NE(exit_code, 0);
}

TEST(Process_spawn, nonexistent_program_throws_exception) {
  const auto program{"this_program_definitely_does_not_exist.exe"};
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

TEST(Process_spawn, cmd_echo_with_arguments) {
  const int exit_code{
      sourcemeta::core::spawn("cmd.exe", {"/c", "echo hello world"})};
  EXPECT_EQ(exit_code, 0);
}

TEST(Process_spawn, cmd_cd_with_custom_directory) {
  // Get the Windows temp directory
  const auto temp_dir = std::filesystem::temp_directory_path();
  const int exit_code{
      sourcemeta::core::spawn("cmd.exe", {"/c", "cd"}, temp_dir)};
  EXPECT_EQ(exit_code, 0);
}

TEST(Process_spawn, cmd_cd_with_current_directory) {
  const int exit_code{sourcemeta::core::spawn("cmd.exe", {"/c", "cd"})};
  EXPECT_EQ(exit_code, 0);
}
