#include <sourcemeta/core/benchmark.h>

#include <cstdint>  // std::uint64_t
#include <iostream> // std::cout

BENCHMARK(counted_first) {
  std::uint64_t count{0};
  for (auto iteration : state) {
    ++count;
    sourcemeta::core::benchmark_do_not_optimize(count);
  }
  std::cout << "Observed iterations: " << count << "\n";
}

BENCHMARK(counted_other) {
  std::uint64_t count{0};
  for (auto iteration : state) {
    ++count;
    sourcemeta::core::benchmark_do_not_optimize(count);
  }
  std::cout << "Other iterations: " << count << "\n";
}
