#include <benchmark/benchmark.h>

#include <sourcemeta/core/jsonl.h>

#include <cassert>    // assert
#include <cstddef>    // std::size_t
#include <filesystem> // std::filesystem
#include <fstream>    // std::ifstream
#include <string>     // std::string

static void JSONL_Parse_Large(benchmark::State &state) {
  const std::filesystem::path filepath{std::string{CURRENT_DIRECTORY} +
                                       "/files/large.jsonl"};

  for (auto _ : state) {
    std::ifstream stream{filepath};
    assert(stream.is_open());
    std::size_t count{0};
    for (const auto &document : sourcemeta::core::JSONL{stream}) {
      assert(document.is_object());
      count += 1;
    }

    assert(count == 10000);
    benchmark::DoNotOptimize(count);
  }
}

BENCHMARK(JSONL_Parse_Large);
