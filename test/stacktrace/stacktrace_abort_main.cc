#include <sourcemeta/core/stacktrace.h>

#include <csignal> // raise, SIGABRT

#if defined(_MSC_VER)
#define STACKTRACE_TEST_NOINLINE __declspec(noinline)
#else
#define STACKTRACE_TEST_NOINLINE __attribute__((noinline))
#endif

namespace sourcemeta_core_stacktrace_test {

volatile int sink{0};

auto crash_deepest() -> void;
auto crash_middle() -> void;

STACKTRACE_TEST_NOINLINE auto crash_deepest() -> void {
  ::raise(SIGABRT);
  sink = sink + 1;
}

STACKTRACE_TEST_NOINLINE auto crash_middle() -> void {
  crash_deepest();
  sink = sink + 1;
}

} // namespace sourcemeta_core_stacktrace_test

auto main() -> int {
  sourcemeta::core::install_crash_handler();
  sourcemeta_core_stacktrace_test::crash_middle();
  return 0;
}
