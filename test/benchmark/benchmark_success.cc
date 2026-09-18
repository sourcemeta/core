#include <sourcemeta/core/benchmark.h>

#include <cstdint> // std::uint64_t
#include <limits>  // std::numeric_limits

BENCHMARK(addition) {
  std::uint64_t accumulator{0};
  for (auto iteration : state) {
    accumulator += 1;
    sourcemeta::core::benchmark_do_not_optimize(accumulator);
  }
}

BENCHMARK(multiplication) {
  std::uint64_t accumulator{1};
  for (auto iteration : state) {
    accumulator *= 3;
    sourcemeta::core::benchmark_do_not_optimize(accumulator);
  }
}

BENCHMARK(subtraction) {
  std::uint64_t accumulator{std::numeric_limits<std::uint64_t>::max()};
  for (auto iteration : state) {
    accumulator -= 1;
    sourcemeta::core::benchmark_do_not_optimize(accumulator);
  }
}
