#include <benchmark/benchmark.h>

#include <cassert>    // assert
#include <filesystem> // std::filesystem

#include <sourcemeta/core/alterschema.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

static void
Alterschema_Check_Readibility_ISO_Country_Set_3(benchmark::State &state) {
  const auto schema{sourcemeta::core::read_json(
      std::filesystem::path{CURRENT_DIRECTORY} / "schemas" /
      "2020_12_iso_country_2023_set_3.json")};

  sourcemeta::core::SchemaTransformer bundle;
  sourcemeta::core::add(bundle, sourcemeta::core::AlterSchemaMode::Readability);

  for (auto _ : state) {
    auto result = bundle.check(
        schema, sourcemeta::core::schema_official_walker,
        sourcemeta::core::schema_official_resolver,
        [](const auto &, const auto &, const auto &, const auto &) {});
    assert(result.first);
    assert(result.second == 100);
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK(Alterschema_Check_Readibility_ISO_Country_Set_3);
