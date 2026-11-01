#include <sourcemeta/core/benchmark.h>

#include <cstdint>  // std::uint64_t
#include <iostream> // std::cout

BENCHMARK(counted) {
  std::uint64_t count{0};
  for (auto iteration : state) {
    ++count;
    sourcemeta::core::benchmark_do_not_optimize(count);
  }
  std::cout << "Observed iterations: " << count << "\n";
}
