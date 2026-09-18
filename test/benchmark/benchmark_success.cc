#include <sourcemeta/core/benchmark.h>

BENCHMARK(addition) {
  for (auto iteration : state) {
    auto result{1 + 1};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

BENCHMARK(multiplication) {
  for (auto iteration : state) {
    auto result{2 * 3};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

BENCHMARK(subtraction) {
  const auto left{9};
  for (auto iteration : state) {
    sourcemeta::core::benchmark_do_not_optimize(left - 1);
  }
}
