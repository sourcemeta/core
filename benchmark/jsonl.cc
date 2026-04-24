#include <benchmark/benchmark.h>

#include <sourcemeta/core/jsonl.h>

#include <cassert>    // assert
#include <cstddef>    // std::size_t
#include <filesystem> // std::filesystem
#include <fstream>    // std::ifstream
#include <ios>        // std::ios

static void JSONL_Parse_Large(benchmark::State &state) {
  const std::filesystem::path filepath{std::string{CURRENT_DIRECTORY} +
                                       "/files/large.jsonl"};

  for (auto _ : state) {
    std::ifstream stream{filepath};
    assert(stream.is_open());
    std::size_t count{0};
    for ([[maybe_unused]] const auto &entry : sourcemeta::core::JSONL{stream}) {
      count += 1;
    }

    assert(count == 10000);
    benchmark::DoNotOptimize(count);
  }
}

static void JSONL_Parse_Large_GZIP(benchmark::State &state) {
  const std::filesystem::path filepath{std::string{CURRENT_DIRECTORY} +
                                       "/files/large.jsonl.gz"};

  for (auto _ : state) {
    std::ifstream stream{filepath, std::ios::binary};
    assert(stream.is_open());
    std::size_t count{0};
    for ([[maybe_unused]] const auto &entry :
         sourcemeta::core::JSONL{stream, sourcemeta::core::JSONL::Mode::GZIP}) {
      count += 1;
    }

    assert(count == 10000);
    benchmark::DoNotOptimize(count);
  }
}

BENCHMARK(JSONL_Parse_Large);
BENCHMARK(JSONL_Parse_Large_GZIP);
