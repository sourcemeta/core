#include <sourcemeta/core/benchmark.h>

#include <filesystem> // std::filesystem

#include <sourcemeta/core/openapi.h>

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

// A description of this size takes milliseconds to read and seconds to frame,
// so what it measures is the walk rather than the parser. It comes from the
// OpenAPI Initiative's own directory of public descriptions, at
// https://github.com/APIs-guru/openapi-directory (CC0 1.0), under
// APIs/gsmtasks.com/2.4.13/openapi.yaml at commit f04b8d0, converted from YAML
// to JSON. Of its hundred and fifty-six paths, ninety-one carry a template
// expression, which is the shape the checks around those are read against
BENCHMARK(OpenAPI_Frame_GSMTasks_3_0) {
  const auto document{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "files" / "openapi_3_0_gsmtasks.json")};

  for (auto iteration : state) {
    const sourcemeta::core::OpenAPIFrame frame{
        document, sourcemeta::core::schema_walker,
        sourcemeta::core::schema_resolver};
    sourcemeta::core::benchmark_do_not_optimize(frame);
  }
}

BENCHMARK(OpenAPI_Format_Synthetic_3_1) {
  const auto document{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "files" / "openapi_3_1_synthetic.json")};

  for (auto iteration : state) {
    auto description{document};
    // Formatting spends the frame it is given, so framing cannot be hoisted out
    const sourcemeta::core::OpenAPIFrame frame{
        description, sourcemeta::core::schema_walker,
        sourcemeta::core::schema_resolver};
    sourcemeta::core::openapi_format(description, frame);
    sourcemeta::core::benchmark_do_not_optimize(description);
  }
}
