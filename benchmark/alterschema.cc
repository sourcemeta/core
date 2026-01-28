#include <benchmark/benchmark.h>

#include <cassert>    // assert
#include <filesystem> // std::filesystem

#include <sourcemeta/core/alterschema.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

static void
Alterschema_Check_Readibility_ISO_Language_Set_3(benchmark::State &state) {
  const auto schema{sourcemeta::core::read_json(
      std::filesystem::path{CURRENT_DIRECTORY} / "schemas" /
      "2020_12_iso_language_2023_set_3.json")};

  sourcemeta::core::SchemaTransformer bundle;
  sourcemeta::core::add(bundle, sourcemeta::core::AlterSchemaMode::Linter);

  for (auto _ : state) {
    auto result = bundle.check(
        schema, sourcemeta::core::schema_walker,
        sourcemeta::core::schema_resolver,
        [](const auto &, const auto &, const auto &, const auto &) {});
    assert(result.first);
    assert(result.second == 100);
    benchmark::DoNotOptimize(result);
  }
}

static void Alterschema_Check_Readibility_OMC(benchmark::State &state) {
  const auto schema{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "schemas" / "2019_09_omc_json_v2.json")};

  sourcemeta::core::SchemaTransformer bundle;
  sourcemeta::core::add(bundle, sourcemeta::core::AlterSchemaMode::Linter);

  for (auto _ : state) {
    auto result = bundle.check(
        schema, sourcemeta::core::schema_walker,
        sourcemeta::core::schema_resolver,
        [](const auto &, const auto &, const auto &, const auto &) {});
    assert(result.first);
    benchmark::DoNotOptimize(result);
  }
}

static void Alterschema_Apply_Readibility_KrakenD(benchmark::State &state) {
  sourcemeta::core::SchemaTransformer bundle;
  sourcemeta::core::add(bundle, sourcemeta::core::AlterSchemaMode::Linter);

  const auto schema{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "schemas" / "2019_09_krakend.json")};

  for (auto _ : state) {
    state.PauseTiming();
    auto copy = schema;
    state.ResumeTiming();
    auto result = bundle.apply(
        copy, sourcemeta::core::schema_walker,
        sourcemeta::core::schema_resolver,
        [](const auto &, const auto &, const auto &, const auto &) {});
    assert(!result.first);
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK(Alterschema_Check_Readibility_ISO_Language_Set_3);
BENCHMARK(Alterschema_Check_Readibility_OMC);
BENCHMARK(Alterschema_Apply_Readibility_KrakenD);
