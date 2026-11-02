#include <sourcemeta/core/benchmark.h>

#include <sourcemeta/core/gzip.h>
#include <sourcemeta/core/io.h>

#include <cstdint>    // std::uint8_t
#include <filesystem> // std::filesystem
#include <ios>        // std::streamsize
#include <istream>    // std::istream
#include <sstream>    // std::istringstream
#include <string>     // std::string

BENCHMARK(GZIP_Compress_ISO_Language_Set_3_Locations) {
  const sourcemeta::core::FileView view{
      std::filesystem::path{CURRENT_DIRECTORY} / "files" /
      "iso_language_2023_set_3_locations.json.gz"};
  const auto contents{
      sourcemeta::core::gunzip(view.as<std::uint8_t>(), view.size())};

  for (auto iteration : state) {
    auto result{sourcemeta::core::gzip(
        reinterpret_cast<const std::uint8_t *>(contents.data()),
        contents.size())};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

BENCHMARK(GZIP_Decompress_ISO_Language_Set_3_Locations) {
  const sourcemeta::core::FileView view{
      std::filesystem::path{CURRENT_DIRECTORY} / "files" /
      "iso_language_2023_set_3_locations.json.gz"};
  const auto output_hint{
      sourcemeta::core::gunzip(view.as<std::uint8_t>(), view.size()).size()};

  for (auto iteration : state) {
    auto result{sourcemeta::core::gunzip(view.as<std::uint8_t>(), view.size(),
                                         output_hint)};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

BENCHMARK(GZIP_Decompress_Default_Level_ISO_Language_Set_3_Locations) {
  const sourcemeta::core::FileView view{
      std::filesystem::path{CURRENT_DIRECTORY} / "files" /
      "iso_language_2023_set_3_locations.json.gz"};
  const auto contents{
      sourcemeta::core::gunzip(view.as<std::uint8_t>(), view.size())};
  const auto compressed{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(contents.data()),
      contents.size())};

  for (auto iteration : state) {
    auto result{sourcemeta::core::gunzip(
        reinterpret_cast<const std::uint8_t *>(compressed.data()),
        compressed.size(), contents.size())};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

BENCHMARK(GZIP_Decompress_Stream_Default_Level_ISO_Language_Set_3_Locations) {
  const sourcemeta::core::FileView view{
      std::filesystem::path{CURRENT_DIRECTORY} / "files" /
      "iso_language_2023_set_3_locations.json.gz"};
  const auto contents{
      sourcemeta::core::gunzip(view.as<std::uint8_t>(), view.size())};
  std::istringstream source{sourcemeta::core::gzip(
      reinterpret_cast<const std::uint8_t *>(contents.data()),
      contents.size())};

  for (auto iteration : state) {
    source.clear();
    source.seekg(0);
    sourcemeta::core::GZIPStreamBuffer buffer{source};
    std::istream decompressed{&buffer};
    std::string result;
    result.resize(contents.size());
    decompressed.read(result.data(),
                      static_cast<std::streamsize>(contents.size()));
    // Reaching the end of the stream validates the member trailer
    sourcemeta::core::benchmark_do_not_optimize(decompressed.peek());
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

BENCHMARK(GZIP_Compress_ISO_Language_Set_3_Schema) {
  const sourcemeta::core::FileView view{
      std::filesystem::path{CURRENT_DIRECTORY} / "files" /
      "iso_language_2023_set_3_schema.json.gz"};
  const auto contents{
      sourcemeta::core::gunzip(view.as<std::uint8_t>(), view.size())};

  for (auto iteration : state) {
    auto result{sourcemeta::core::gzip(
        reinterpret_cast<const std::uint8_t *>(contents.data()),
        contents.size())};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

BENCHMARK(GZIP_Decompress_ISO_Language_Set_3_Schema) {
  const sourcemeta::core::FileView view{
      std::filesystem::path{CURRENT_DIRECTORY} / "files" /
      "iso_language_2023_set_3_schema.json.gz"};
  const auto output_hint{
      sourcemeta::core::gunzip(view.as<std::uint8_t>(), view.size()).size()};

  for (auto iteration : state) {
    auto result{sourcemeta::core::gunzip(view.as<std::uint8_t>(), view.size(),
                                         output_hint)};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}
