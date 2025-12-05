#include <benchmark/benchmark.h>

#include <cassert>    // assert
#include <filesystem> // std::filesystem

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

static void Schema_Frame_OMC_Instances(benchmark::State &state) {
  const auto schema{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "schemas" / "2019_09_omc_json_v2.json")};

  for (auto _ : state) {
    sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::Instances};
    frame.analyse(schema, sourcemeta::core::schema_official_walker,
                  sourcemeta::core::schema_official_resolver);
    benchmark::DoNotOptimize(frame);
  }
}

static void Schema_Frame_OMC_References(benchmark::State &state) {
  const auto schema{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "schemas" / "2019_09_omc_json_v2.json")};

  for (auto _ : state) {
    sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::References};
    frame.analyse(schema, sourcemeta::core::schema_official_walker,
                  sourcemeta::core::schema_official_resolver);
    benchmark::DoNotOptimize(frame);
  }
}

static void Schema_Frame_OMC_Locations(benchmark::State &state) {
  const auto schema{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "schemas" / "2019_09_omc_json_v2.json")};

  for (auto _ : state) {
    sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::Locations};
    frame.analyse(schema, sourcemeta::core::schema_official_walker,
                  sourcemeta::core::schema_official_resolver);
    benchmark::DoNotOptimize(frame);
  }
}

static void Schema_Frame_ISO_Country_Locations(benchmark::State &state) {
  const auto schema{sourcemeta::core::read_json(
      std::filesystem::path{CURRENT_DIRECTORY} / "schemas" /
      "2020_12_iso_country_2023_set_3.json")};

  for (auto _ : state) {
    sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::Locations};
    frame.analyse(schema, sourcemeta::core::schema_official_walker,
                  sourcemeta::core::schema_official_resolver);
    benchmark::DoNotOptimize(frame);
  }
}

static void Schema_Bundle_Meta_2020_12(benchmark::State &state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto schema{sourcemeta::core::schema_official_resolver(
                    "https://json-schema.org/draft/2020-12/schema")
                    .value()};
    state.ResumeTiming();
    sourcemeta::core::bundle(schema, sourcemeta::core::schema_official_walker,
                             sourcemeta::core::schema_official_resolver);
    benchmark::DoNotOptimize(schema);
  }
}

BENCHMARK(Schema_Frame_OMC_Instances);
BENCHMARK(Schema_Frame_OMC_References);
BENCHMARK(Schema_Frame_OMC_Locations);
BENCHMARK(Schema_Frame_ISO_Country_Locations);
BENCHMARK(Schema_Bundle_Meta_2020_12);
