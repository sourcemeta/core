#include <sourcemeta/core/benchmark.h>

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/metapack.h>

#include <filesystem> // std::filesystem

static auto benchmark_document() -> sourcemeta::core::JSON {
  return sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                     "files" / "2019_09_omc_json_v2.json");
}

BENCHMARK(Metapack_Parse_OMC_JSON_V2) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "benchmark.metapack"};
  sourcemeta::core::metapack_write_json(
      path, benchmark_document(), "application/schema+json",
      sourcemeta::core::MetapackEncoding::GZIP);
  const sourcemeta::core::FileView view{path};

  for (auto iteration : state) {
    auto result{sourcemeta::core::metapack_parse(view)};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

BENCHMARK(Metapack_Read_OMC_JSON_V2) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "benchmark.metapack"};
  sourcemeta::core::metapack_write_json(
      path, benchmark_document(), "application/schema+json",
      sourcemeta::core::MetapackEncoding::GZIP);
  const sourcemeta::core::FileView view{path};

  for (auto iteration : state) {
    auto result{sourcemeta::core::metapack_read_json(view)};
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

BENCHMARK(Metapack_Write_OMC_JSON_V2) {
  const sourcemeta::core::TemporaryDirectory directory{
      std::filesystem::temp_directory_path(), ".metapack-"};
  const auto path{directory.path() / "benchmark.metapack"};
  const auto document{benchmark_document()};

  for (auto iteration : state) {
    sourcemeta::core::metapack_write_json(
        path, document, "application/schema+json",
        sourcemeta::core::MetapackEncoding::GZIP);
    sourcemeta::core::benchmark_do_not_optimize(path);
  }
}
