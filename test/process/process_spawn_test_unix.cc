#include <sourcemeta/core/test.h>

#include <sourcemeta/core/process.h>

#include <filesystem> // std::filesystem::path

TEST(usr_bin_true_returns_zero) {
  const int exit_code{sourcemeta::core::spawn("/usr/bin/true", {})};
  EXPECT_EQ(exit_code, 0);
}

TEST(usr_bin_false_returns_one) {
  const int exit_code{sourcemeta::core::spawn("/usr/bin/false", {})};
  EXPECT_EQ(exit_code, 1);
}

TEST(true_without_path_returns_zero) {
  const int exit_code{sourcemeta::core::spawn("true", {})};
  EXPECT_EQ(exit_code, 0);
}

TEST(sh_exit_with_specific_code) {
  const int exit_code{sourcemeta::core::spawn("/bin/sh", {"-c", "exit 42"})};
  EXPECT_EQ(exit_code, 42);
}

TEST(sh_exit_with_different_code) {
  const int exit_code{sourcemeta::core::spawn("/bin/sh", {"-c", "exit 7"})};
  EXPECT_EQ(exit_code, 7);
}

TEST(test_command_failing_condition) {
  const int exit_code{sourcemeta::core::spawn("/bin/test", {"1", "-eq", "2"})};
  EXPECT_EQ(exit_code, 1);
}

TEST(test_command_passing_condition) {
  const int exit_code{sourcemeta::core::spawn("/bin/test", {"1", "-eq", "1"})};
  EXPECT_EQ(exit_code, 0);
}

TEST(nonexistent_program_throws_exception) {
  const auto program{"/bin/this_program_definitely_does_not_exist"};
  try {
    sourcemeta::core::spawn(program, {});
    FAIL();
  } catch (const sourcemeta::core::ProcessProgramNotFoundError &error) {
    EXPECT_EQ(error.program(), program);
  }
}

TEST(echo_with_arguments) {
  const int exit_code{sourcemeta::core::spawn("/bin/echo", {"hello", "world"})};
  EXPECT_EQ(exit_code, 0);
}

TEST(argument_with_spaces_is_a_single_argument) {
  const int exit_code{sourcemeta::core::spawn(
      "/bin/sh",
      {"-c", "test \"$#\" -eq 1 && test \"$1\" = \"alpha beta gamma\"", "sh",
       "alpha beta gamma"})};
  EXPECT_EQ(exit_code, 0);
}

TEST(empty_argument_is_preserved) {
  const int exit_code{sourcemeta::core::spawn(
      "/bin/sh",
      {"-c", "test \"$#\" -eq 2 && test -z \"$1\"", "sh", "", "second"})};
  EXPECT_EQ(exit_code, 0);
}

TEST(pwd_with_custom_directory) {
  const int exit_code{
      sourcemeta::core::spawn("pwd", {}, std::filesystem::path{"/tmp"})};
  EXPECT_EQ(exit_code, 0);
}

TEST(pwd_with_current_directory) {
  const int exit_code{sourcemeta::core::spawn("pwd", {})};
  EXPECT_EQ(exit_code, 0);
}
