#include <benchmark/benchmark.h>

#include <filesystem> // std::filesystem

#include <sourcemeta/core/editorschema.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

static void EditorSchema_ForEditor_EmbeddedResources(benchmark::State &state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto schema{sourcemeta::core::read_json(
        std::filesystem::path{CURRENT_DIRECTORY} / "schemas" /
        "2020_12_embedded_resources.json")};
    state.ResumeTiming();
    sourcemeta::core::for_editor(schema, sourcemeta::core::schema_walker,
                                 sourcemeta::core::schema_resolver);
    benchmark::DoNotOptimize(schema);
  }
}

BENCHMARK(EditorSchema_ForEditor_EmbeddedResources);
