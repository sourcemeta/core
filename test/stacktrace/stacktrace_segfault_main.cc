#include <sourcemeta/core/stacktrace.h>

namespace sourcemeta_core_stacktrace_test {

volatile int sink{0};

auto crash_deepest() -> void;
auto crash_middle() -> void;

__attribute__((noinline)) auto crash_deepest() -> void {
  volatile int *null_pointer{nullptr};
  *null_pointer = 42;
}

__attribute__((noinline)) auto crash_middle() -> void {
  crash_deepest();
  sink = sink + 1;
}

} // namespace sourcemeta_core_stacktrace_test

auto main() -> int {
  sourcemeta::core::install_crash_handler();
  sourcemeta_core_stacktrace_test::crash_middle();
  return 0;
}
