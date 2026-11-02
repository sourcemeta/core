#include <sourcemeta/core/benchmark.h>

BENCHMARK(never_iterates) {
  [[maybe_unused]] const auto iterations{state.iterations()};
}
