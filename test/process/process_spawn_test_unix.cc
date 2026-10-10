#include <sourcemeta/core/process.h>
#include <sourcemeta/core/test.h>

#include <csignal>    // SIGPIPE, sigaddset, sigemptyset, sigset_t
#include <filesystem> // std::filesystem::path
#include <pthread.h>  // pthread_sigmask

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
  const auto *const program{"/bin/this_program_definitely_does_not_exist"};
  try {
    sourcemeta::core::spawn(program, {});
    FAIL();
  } catch (const sourcemeta::core::ProcessProgramNotFoundError &error) {
    EXPECT_EQ(error.program(), program);
  }
}

// A path that names something present but not executable is a different
// failure from a path that names nothing, and only the latter reports a
// missing program
TEST(non_executable_program_throws_a_spawn_error) {
  const auto *const program{"/etc/hosts"};
  try {
    sourcemeta::core::spawn(program, {});
    FAIL();
  } catch (const sourcemeta::core::ProcessSpawnError &error) {
    EXPECT_EQ(error.program(), program);
    EXPECT_TRUE(error.arguments().empty());
  }
}

// A program cut short by a signal reports no exit code of its own
TEST(signalled_program_throws_a_spawn_error) {
  try {
    sourcemeta::core::spawn("/bin/sh", {"-c", "kill -9 $$"});
    FAIL();
  } catch (const sourcemeta::core::ProcessSpawnError &error) {
    EXPECT_EQ(error.program(), "/bin/sh");
  }
}

// Spawning blocks the broken-pipe signal for the calling thread and consumes
// any instance raised meanwhile, but an instance raised under a mask the caller
// established belongs to whoever established it, so the mask comes back as it
// was and the signal is left alone
TEST(a_caller_already_blocking_the_broken_pipe_signal_keeps_its_mask) {
  sigset_t blocked;
  sigemptyset(&blocked);
  sigaddset(&blocked, SIGPIPE);
  sigset_t previous;
  sigemptyset(&previous);
  EXPECT_EQ(pthread_sigmask(SIG_BLOCK, &blocked, &previous), 0);

  const int exit_code{sourcemeta::core::spawn("/usr/bin/true", {})};

  sigset_t current;
  sigemptyset(&current);
  EXPECT_EQ(pthread_sigmask(SIG_BLOCK, nullptr, &current), 0);
  EXPECT_EQ(pthread_sigmask(SIG_SETMASK, &previous, nullptr), 0);

  EXPECT_EQ(exit_code, 0);
  EXPECT_EQ(sigismember(&current, SIGPIPE), 1);
}

TEST(echo_with_arguments) {
  const int exit_code{sourcemeta::core::spawn("/bin/echo", {"hello", "world"})};
  EXPECT_EQ(exit_code, 0);
}

TEST(argument_with_spaces_is_a_single_argument) {
  const int exit_code{sourcemeta::core::spawn(
      "/bin/sh", {"-c", R"(test "$#" -eq 1 && test "$1" = "alpha beta gamma")",
                  "sh", "alpha beta gamma"})};
  EXPECT_EQ(exit_code, 0);
}

TEST(empty_argument_is_preserved) {
  const int exit_code{sourcemeta::core::spawn(
      "/bin/sh",
      {"-c", R"(test "$#" -eq 2 && test -z "$1")", "sh", "", "second"})};
  EXPECT_EQ(exit_code, 0);
}

TEST(pwd_with_custom_directory) {
  const int exit_code{sourcemeta::core::spawn(
      "pwd", {}, {.directory = std::filesystem::path{"/tmp"}})};
  EXPECT_EQ(exit_code, 0);
}

TEST(pwd_with_current_directory) {
  const int exit_code{sourcemeta::core::spawn("pwd", {})};
  EXPECT_EQ(exit_code, 0);
}
