#include <sourcemeta/core/stacktrace.h>

namespace sourcemeta_core_stacktrace_test {

volatile int sink{0};

auto print_deepest() -> void;
auto print_middle() -> void;

__attribute__((noinline)) auto print_deepest() -> void {
  sourcemeta::core::stacktrace();
}

__attribute__((noinline)) auto print_middle() -> void {
  print_deepest();
  sink = sink + 1;
}

} // namespace sourcemeta_core_stacktrace_test

auto main() -> int {
  sourcemeta_core_stacktrace_test::print_middle();
  return 0;
}
