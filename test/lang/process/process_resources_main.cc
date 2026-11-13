#include <sourcemeta/core/process.h>

#include <array>  // std::array
#include <cerrno> // errno, EBADF, ECHILD
#include <csignal> // SIGCHLD, SIG_IGN, SIGPIPE, SIG_BLOCK, sigaction, sigpending, sigwait
#include <cstddef>        // std::size_t
#include <fcntl.h>        // fcntl, F_DUPFD_CLOEXEC, F_GETFD
#include <iostream>       // std::cerr
#include <pthread.h>      // pthread_sigmask
#include <string>         // std::string
#include <string_view>    // std::string_view
#include <sys/resource.h> // getrlimit, setrlimit, RLIMIT_NOFILE
#include <sys/wait.h>     // waitpid, WNOHANG
#include <unistd.h> // alarm, close, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO

namespace {

auto closed(const int descriptor) -> bool {
  return fcntl(descriptor, F_GETFD) == -1 && errno == EBADF;
}

auto capture(const std::string &program) -> bool {
  const auto output{sourcemeta::core::spawn_and_capture(
      program, {"both", "the output", "the error"})};
  return output.exit_code == 0 && output.standard_output == "the output" &&
         output.standard_error == "the error";
}

auto exhausted(const std::string &program, const std::size_t available)
    -> bool {
  rlimit original{};
  if (getrlimit(RLIMIT_NOFILE, &original) != 0) {
    return false;
  }

  // Find five free slots without assuming the helper inherited only stdio.
  // The selected ceiling leaves exactly the requested number above stdio.
  std::array<int, 5> slots{{-1, -1, -1, -1, -1}};
  for (auto &slot : slots) {
    slot = fcntl(STDERR_FILENO, F_DUPFD_CLOEXEC, STDERR_FILENO + 1);
    if (slot == -1) {
      for (const auto acquired : slots) {
        if (acquired != -1) {
          close(acquired);
        }
      }

      return false;
    }
  }

  for (const auto slot : slots) {
    close(slot);
  }

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
  rlimit restricted{original};
  restricted.rlim_cur = static_cast<rlim_t>(slots[available]);
  if (setrlimit(RLIMIT_NOFILE, &restricted) != 0) {
    return false;
  }

  bool correct{true};
  for (unsigned int attempt = 0; attempt < 3; ++attempt) {
    bool refused{false};
    try {
      static_cast<void>(sourcemeta::core::spawn_and_capture(
          program, {"both", "the output", "the error"}));
    } catch (const sourcemeta::core::ProcessSpawnError &error) {
      refused = error.program() == program && error.arguments().size() == 3 &&
                error.arguments()[0] == "both" &&
                error.arguments()[1] == "the output" &&
                error.arguments()[2] == "the error";
    }

    correct = refused && correct;
    for (const auto slot : slots) {
      correct = closed(slot) && correct;
    }

    correct = closed(STDIN_FILENO) && closed(STDOUT_FILENO) &&
              closed(STDERR_FILENO) && correct;
  }

  const bool restored{setrlimit(RLIMIT_NOFILE, &original) == 0};
  return correct && restored && capture(program);
}

auto failed_spawns(const std::string &program) -> bool {
  // Error construction and the platform loader may initialize process-global
  // state on first use. Measure steady-state calls after the same operation.
  const auto missing{program + ".does-not-exist"};
  auto attempt = [&]() -> bool {
    try {
      const auto output{sourcemeta::core::spawn_and_capture(
          missing, {"an argument"}, {.standard_input = "input"})};
      std::cerr << "Missing executable returned a result (exit "
                << output.exit_code.value_or(-1) << ")\n";
      return false;
    } catch (const sourcemeta::core::ProcessProgramNotFoundError &error) {
      return error.program() == missing;
    }
  };
  if (!attempt()) {
    return false;
  }

  const auto before{sourcemeta::core::process_descriptors()};
  for (unsigned int index = 0; index < 3; ++index) {
    if (!attempt()) {
      return false;
    }
  }

  const auto after{sourcemeta::core::process_descriptors()};
  if (before.open != after.open) {
    std::cerr << "Failed-exec descriptor count changed: "
              << before.open.value_or(0) << " -> " << after.open.value_or(0)
              << '\n';
    return false;
  }

  int status{0};
  const auto child{waitpid(-1, &status, WNOHANG)};
  if (child != -1 || errno != ECHILD) {
    std::cerr << "Failed exec left a child: waitpid=" << child
              << " errno=" << errno << '\n';
    return false;
  }

  return capture(program);
}

auto pending_signal(const std::string &program) -> bool {
  sigset_t blocked;
  sigemptyset(&blocked);
  sigaddset(&blocked, SIGPIPE);
  sigset_t previous;
  if (pthread_sigmask(SIG_BLOCK, &blocked, &previous) != 0 ||
      std::raise(SIGPIPE) != 0) {
    return false;
  }

  const bool captured{capture(program)};
  sigset_t pending;
  sigemptyset(&pending);
  const bool preserved{sigpending(&pending) == 0 &&
                       sigismember(&pending, SIGPIPE) == 1};
  if (preserved) {
    int received{0};
    if (sigwait(&blocked, &received) != 0 || received != SIGPIPE) {
      return false;
    }
  }

  const bool restored{pthread_sigmask(SIG_SETMASK, &previous, nullptr) == 0};
  return captured && preserved && restored;
}

auto ignored_child(const std::string &program) -> bool {
  struct sigaction ignored{};
  ignored.sa_handler = SIG_IGN;
  sigemptyset(&ignored.sa_mask);
  struct sigaction previous{};
  if (sigaction(SIGCHLD, &ignored, &previous) != 0) {
    return false;
  }

  const auto before{sourcemeta::core::process_descriptors()};
  bool refused{false};
  try {
    static_cast<void>(sourcemeta::core::spawn_and_capture(program, {"silent"}));
  } catch (const sourcemeta::core::ProcessSpawnError &error) {
    refused = error.program() == program && error.arguments().size() == 1 &&
              error.arguments()[0] == "silent";
  }

  const auto after{sourcemeta::core::process_descriptors()};
  const bool restored{sigaction(SIGCHLD, &previous, nullptr) == 0};
  return refused && before.open == after.open && restored && capture(program);
}

#if defined(__linux__)
auto telemetry() -> bool {
  rlimit original{};
  if (getrlimit(RLIMIT_NOFILE, &original) != 0) {
    return false;
  }

  rlimit restricted{original};
  restricted.rlim_cur = 0;
  if (setrlimit(RLIMIT_NOFILE, &restricted) != 0) {
    return false;
  }

  const auto usage{sourcemeta::core::process_usage()};
  const auto descriptors{sourcemeta::core::process_descriptors()};
  const auto started{sourcemeta::core::process_start_time()};
  const bool restored{setrlimit(RLIMIT_NOFILE, &original) == 0};
  const auto recovered{sourcemeta::core::process_usage()};
  return restored && !usage.cpu_time.has_value() &&
         !usage.resident_bytes.has_value() &&
         !usage.virtual_bytes.has_value() && !descriptors.open.has_value() &&
         descriptors.maximum == 0 && !started.has_value() &&
         recovered.cpu_time.has_value() &&
         recovered.resident_bytes.has_value() &&
         recovered.virtual_bytes.has_value() &&
         sourcemeta::core::process_descriptors().open.has_value() &&
         sourcemeta::core::process_start_time().has_value();
}
#endif

} // namespace

auto main(int argc, char *argv[]) -> int {
  if (argc != 3) {
    return 2;
  }

  alarm(15);
  const std::string_view scenario{argv[1]};
  const std::string program{argv[2]};
  if (scenario == "closed-input") {
    close(STDIN_FILENO);
    return sourcemeta::core::spawn(program, {"expect-stdin", "the input"},
                                   {.standard_input = "the input"});
  }

  if (scenario == "closed-output" || scenario == "closed-error" ||
      scenario == "closed-all") {
    if (scenario != "closed-error") {
      close(STDOUT_FILENO);
    }

    if (scenario != "closed-output") {
      close(STDERR_FILENO);
    }

    if (scenario == "closed-all") {
      close(STDIN_FILENO);
      const std::string payload{"before\0after", 12};
      const auto output{sourcemeta::core::spawn_and_capture(
          program, {"cat"}, {.standard_input = payload})};
      if (output.exit_code != 0 || output.standard_output != payload ||
          !output.standard_error.empty()) {
        return 1;
      }
    }

    return capture(program) ? 0 : 1;
  }

  if (scenario.starts_with("exhaust-")) {
    const std::size_t available{scenario == "exhaust-first"    ? 0U
                                : scenario == "exhaust-second" ? 1U
                                : scenario == "exhaust-output" ? 2U
                                                               : 4U};
    return exhausted(program, available) ? 0 : 1;
  }

  if (scenario == "failed-spawns") {
    return failed_spawns(program) ? 0 : 1;
  }

  if (scenario == "pending-signal") {
    return pending_signal(program) ? 0 : 1;
  }

  if (scenario == "ignored-child") {
    return ignored_child(program) ? 0 : 1;
  }

#if defined(__linux__)
  if (scenario == "telemetry") {
    return telemetry() ? 0 : 1;
  }
#endif

  return 2;
}
