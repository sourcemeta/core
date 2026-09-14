#include <benchmark/benchmark.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/io.h>

#include <cstdint>     // std::uint8_t
#include <filesystem>  // std::filesystem
#include <string_view> // std::string_view

// NOLINTNEXTLINE(readability-identifier-naming)
static void CRYPTO_CRC32_Large_JSONL(benchmark::State &state) {
  const sourcemeta::core::FileView view{
      std::filesystem::path{CURRENT_DIRECTORY} / "files" / "large.jsonl"};
  const std::string_view contents{
      reinterpret_cast<const char *>(view.as<std::uint8_t>()), view.size()};

  for (auto iteration : state) {
    auto result{sourcemeta::core::crc32(contents)};
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK(CRYPTO_CRC32_Large_JSONL);
