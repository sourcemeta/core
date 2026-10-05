#include <sourcemeta/core/benchmark.h>

#include <filesystem> // std::filesystem

#include <sourcemeta/core/openapi.h>

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

BENCHMARK(OpenAPI_Format_Synthetic_3_1) {
  const auto document{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "files" / "openapi_3_1_synthetic.json")};

  for (auto iteration : state) {
    auto description{document};
    // Formatting spends the frame it is given, so framing cannot be hoisted out
    const sourcemeta::core::OpenAPIFrame frame{
        sourcemeta::core::OpenAPIFrame::Mode::Everything, description,
        sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};
    sourcemeta::core::openapi_format(description, frame);
    sourcemeta::core::benchmark_do_not_optimize(description);
  }
}

BENCHMARK(OpenAPI_Frame_Schemas_Synthetic_3_1) {
  const auto document{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "files" / "openapi_3_1_synthetic.json")};

  for (auto iteration : state) {
    const sourcemeta::core::OpenAPIFrame frame{
        sourcemeta::core::OpenAPIFrame::Mode::Schemas, document,
        sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};
    sourcemeta::core::benchmark_do_not_optimize(frame);
  }
}

BENCHMARK(OpenAPI_Frame_Everything_Synthetic_3_1) {
  const auto document{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "files" / "openapi_3_1_synthetic.json")};

  for (auto iteration : state) {
    const sourcemeta::core::OpenAPIFrame frame{
        sourcemeta::core::OpenAPIFrame::Mode::Everything, document,
        sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};
    sourcemeta::core::benchmark_do_not_optimize(frame);
  }
}
