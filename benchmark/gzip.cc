#include <benchmark/benchmark.h>

#include <sourcemeta/core/gzip.h>
#include <sourcemeta/core/io.h>

#include <cstdint>    // std::uint8_t
#include <filesystem> // std::filesystem

static void
GZIP_Compress_ISO_Language_Set_3_Locations(benchmark::State &state) {
  const sourcemeta::core::FileView view{
      std::filesystem::path{CURRENT_DIRECTORY} / "files" /
      "iso_language_2023_set_3_locations.json.gz"};
  const auto contents{
      sourcemeta::core::gunzip(view.as<std::uint8_t>(), view.size())};

  for (auto _ : state) {
    auto result{sourcemeta::core::gzip(
        reinterpret_cast<const std::uint8_t *>(contents.data()),
        contents.size())};
    benchmark::DoNotOptimize(result);
  }
}

static void
GZIP_Decompress_ISO_Language_Set_3_Locations(benchmark::State &state) {
  const sourcemeta::core::FileView view{
      std::filesystem::path{CURRENT_DIRECTORY} / "files" /
      "iso_language_2023_set_3_locations.json.gz"};
  const auto output_hint{
      sourcemeta::core::gunzip(view.as<std::uint8_t>(), view.size()).size()};

  for (auto _ : state) {
    auto result{sourcemeta::core::gunzip(view.as<std::uint8_t>(), view.size(),
                                         output_hint)};
    benchmark::DoNotOptimize(result);
  }
}

static void GZIP_Compress_ISO_Language_Set_3_Schema(benchmark::State &state) {
  const sourcemeta::core::FileView view{
      std::filesystem::path{CURRENT_DIRECTORY} / "files" /
      "iso_language_2023_set_3_schema.json.gz"};
  const auto contents{
      sourcemeta::core::gunzip(view.as<std::uint8_t>(), view.size())};

  for (auto _ : state) {
    auto result{sourcemeta::core::gzip(
        reinterpret_cast<const std::uint8_t *>(contents.data()),
        contents.size())};
    benchmark::DoNotOptimize(result);
  }
}

static void GZIP_Decompress_ISO_Language_Set_3_Schema(benchmark::State &state) {
  const sourcemeta::core::FileView view{
      std::filesystem::path{CURRENT_DIRECTORY} / "files" /
      "iso_language_2023_set_3_schema.json.gz"};
  const auto output_hint{
      sourcemeta::core::gunzip(view.as<std::uint8_t>(), view.size()).size()};

  for (auto _ : state) {
    auto result{sourcemeta::core::gunzip(view.as<std::uint8_t>(), view.size(),
                                         output_hint)};
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK(GZIP_Compress_ISO_Language_Set_3_Locations);
BENCHMARK(GZIP_Decompress_ISO_Language_Set_3_Locations);
BENCHMARK(GZIP_Compress_ISO_Language_Set_3_Schema);
BENCHMARK(GZIP_Decompress_ISO_Language_Set_3_Schema);
