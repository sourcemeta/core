#include <sourcemeta/core/stacktrace.h>

#if defined(_MSC_VER)
#define STACKTRACE_TEST_NOINLINE __declspec(noinline)
#else
#define STACKTRACE_TEST_NOINLINE __attribute__((noinline))
#endif

namespace sourcemeta_core_stacktrace_test {

volatile int sink{0};

auto print_deepest() -> void;
auto print_middle() -> void;

STACKTRACE_TEST_NOINLINE auto print_deepest() -> void {
  sourcemeta::core::stacktrace();
  sink = sink + 1;
}

STACKTRACE_TEST_NOINLINE auto print_middle() -> void {
  print_deepest();
  sink = sink + 1;
}

} // namespace sourcemeta_core_stacktrace_test

auto main() -> int {
  sourcemeta_core_stacktrace_test::print_middle();
  return 0;
}
