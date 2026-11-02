#include <sourcemeta/core/benchmark.h>

#include <sourcemeta/core/jsonl.h>

#include <cassert>    // assert
#include <cstddef>    // std::size_t
#include <filesystem> // std::filesystem
#include <fstream>    // std::ifstream
#include <ios>        // std::ios

BENCHMARK(JSONL_Parse_Large) {
  const std::filesystem::path filepath{std::string{CURRENT_DIRECTORY} +
                                       "/files/large.jsonl"};

  for (auto iteration : state) {
    std::ifstream stream{filepath};
    assert(stream.is_open());
    std::size_t count{0};
    for ([[maybe_unused]] const auto &entry : sourcemeta::core::JSONL{stream}) {
      count += 1;
    }

    assert(count == 10000);
    sourcemeta::core::benchmark_do_not_optimize(count);
  }
}

BENCHMARK(JSONL_Parse_Large_GZIP) {
  const std::filesystem::path filepath{std::string{CURRENT_DIRECTORY} +
                                       "/files/large.jsonl.gz"};

  for (auto iteration : state) {
    std::ifstream stream{filepath, std::ios::binary};
    assert(stream.is_open());
    std::size_t count{0};
    for ([[maybe_unused]] const auto &entry :
         sourcemeta::core::JSONL{stream, sourcemeta::core::JSONL::Mode::GZIP}) {
      count += 1;
    }

    assert(count == 10000);
    sourcemeta::core::benchmark_do_not_optimize(count);
  }
}
