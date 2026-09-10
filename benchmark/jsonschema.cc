#include <benchmark/benchmark.h>

#include <cassert>    // assert
#include <filesystem> // std::filesystem
#include <functional> // std::ref
#include <optional>   // std::optional

#include <sourcemeta/core/jsonschema.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

// Google Benchmark reports these names as the benchmark labels, so they
// have to stay comparable against previously recorded runs
// NOLINTBEGIN(readability-identifier-naming)
static void Schema_Frame_WoT_References(benchmark::State &state) {
  const auto schema{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "files" / "draft7_w3c_wot_td_v1_1.json")};

  for (auto iteration : state) {
    sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::References, schema,
        sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};
    benchmark::DoNotOptimize(frame);
  }
}

static void Schema_Frame_OMC_References(benchmark::State &state) {
  const auto schema{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "files" / "2019_09_omc_json_v2.json")};

  for (auto iteration : state) {
    sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::References, schema,
        sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};
    benchmark::DoNotOptimize(frame);
  }
}

static void Schema_Frame_OMC_Pointers(benchmark::State &state) {
  const auto schema{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "files" / "2019_09_omc_json_v2.json")};

  for (auto iteration : state) {
    sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::Pointers, schema,
        sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};
    benchmark::DoNotOptimize(frame);
  }
}

static void Schema_Frame_OMC_Locations(benchmark::State &state) {
  const auto schema{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "files" / "2019_09_omc_json_v2.json")};

  for (auto iteration : state) {
    sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::Locations, schema,
        sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};
    benchmark::DoNotOptimize(frame);
  }
}

static void Schema_Frame_ISO_Language_Locations(benchmark::State &state) {
  const auto schema{sourcemeta::core::read_json(
      std::filesystem::path{CURRENT_DIRECTORY} / "files" /
      "2020_12_iso_language_2023_set_3.json")};

  for (auto iteration : state) {
    sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::Locations, schema,
        sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};
    benchmark::DoNotOptimize(frame);
  }
}

static void Schema_Frame_ISO_Language_Root(benchmark::State &state) {
  const auto schema{sourcemeta::core::read_json(
      std::filesystem::path{CURRENT_DIRECTORY} / "files" /
      "2020_12_iso_language_2023_set_3.json")};

  for (auto iteration : state) {
    sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::Root, schema,
        sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};
    benchmark::DoNotOptimize(frame);
  }
}

static void Schema_Frame_KrakenD_References(benchmark::State &state) {
  const auto schema{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "files" / "2019_09_krakend.json")};

  for (auto iteration : state) {
    sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::References, schema,
        sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};
    benchmark::DoNotOptimize(frame);
  }
}

static void Schema_Frame_KrakenD_Reachable(benchmark::State &state) {
  const auto schema{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "files" / "2019_09_krakend.json")};

  // Kept out of the timed region so that neither building nor discarding the
  // frame counts towards the reachability measurement
  std::optional<sourcemeta::core::SchemaFrame> frame;

  for (auto iteration : state) {
    state.PauseTiming();
    frame.emplace(sourcemeta::core::SchemaFrame::Mode::References, schema,
                  sourcemeta::core::schema_walker,
                  sourcemeta::core::schema_resolver);
    state.ResumeTiming();

    frame->for_each_location(
        [&frame](const sourcemeta::core::SchemaReferenceType,
                 const std::string_view,
                 const sourcemeta::core::SchemaFrame::Location &entry) -> void {
          if (entry.type ==
              sourcemeta::core::SchemaFrame::LocationType::Pointer) {
            return;
          }

          frame->for_each_subschema(
              [&frame,
               &entry](const sourcemeta::core::SchemaFrame::Location &subentry)
                  -> void {
                auto result{frame->is_reachable(
                    subentry, entry, sourcemeta::core::schema_walker,
                    sourcemeta::core::schema_resolver)};
                benchmark::DoNotOptimize(result);
              });
        });
  }
}

static void
Schema_Frame_ISO_Language_Locations_To_JSON(benchmark::State &state) {
  sourcemeta::core::PointerPositionTracker tracker;
  sourcemeta::core::JSON schema{nullptr};
  sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "files" /
                                  "2020_12_iso_language_2023_set_3.json",
                              schema, std::ref(tracker));

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations, schema,
      sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};

  for (auto iteration : state) {
    auto result{frame.to_json(sourcemeta::core::schema_resolver, tracker)};
    assert(result.is_object());
    benchmark::DoNotOptimize(result);
  }
}

static void Schema_Frame_Many_Resources_References(benchmark::State &state) {
  const auto schema{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "files" / "2020_12_many_resources.json")};

  for (auto iteration : state) {
    sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::References, schema,
        sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};
    benchmark::DoNotOptimize(frame);
  }
}

static void Schema_Frame_Deeply_Nested_References(benchmark::State &state) {
  const auto schema{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "files" / "2020_12_deeply_nested.json")};

  for (auto iteration : state) {
    sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::References, schema,
        sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};
    benchmark::DoNotOptimize(frame);
  }
}

BENCHMARK(Schema_Frame_WoT_References);
BENCHMARK(Schema_Frame_OMC_References);
BENCHMARK(Schema_Frame_OMC_Pointers);
BENCHMARK(Schema_Frame_OMC_Locations);
BENCHMARK(Schema_Frame_ISO_Language_Locations);
BENCHMARK(Schema_Frame_ISO_Language_Root);
BENCHMARK(Schema_Frame_KrakenD_References);
BENCHMARK(Schema_Frame_KrakenD_Reachable);
BENCHMARK(Schema_Frame_ISO_Language_Locations_To_JSON);
BENCHMARK(Schema_Frame_Many_Resources_References);
BENCHMARK(Schema_Frame_Deeply_Nested_References);
// NOLINTEND(readability-identifier-naming)
