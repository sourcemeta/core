#include <sourcemeta/core/benchmark.h>

#include <cassert>     // assert
#include <filesystem>  // std::filesystem
#include <functional>  // std::ref
#include <map>         // std::map
#include <string>      // std::string, std::to_string
#include <string_view> // std::string_view
#include <utility>     // std::move

#include <sourcemeta/core/jsonschema.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

// Google Benchmark reports these names as the benchmark labels, so they
// have to stay comparable against previously recorded runs
BENCHMARK(Schema_Frame_WoT_References) {
  const auto schema{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "files" / "draft7_w3c_wot_td_v1_1.json")};

  for (auto iteration : state) {
    sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::References, schema,
        sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};
    sourcemeta::core::benchmark_do_not_optimize(frame);
  }
}

BENCHMARK(Schema_Frame_OMC_References) {
  const auto schema{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "files" / "2019_09_omc_json_v2.json")};

  for (auto iteration : state) {
    sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::References, schema,
        sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};
    sourcemeta::core::benchmark_do_not_optimize(frame);
  }
}

BENCHMARK(Schema_Frame_OMC_Pointers) {
  const auto schema{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "files" / "2019_09_omc_json_v2.json")};

  for (auto iteration : state) {
    sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::Pointers, schema,
        sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};
    sourcemeta::core::benchmark_do_not_optimize(frame);
  }
}

BENCHMARK(Schema_Frame_OMC_Locations) {
  const auto schema{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "files" / "2019_09_omc_json_v2.json")};

  for (auto iteration : state) {
    sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::Locations, schema,
        sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};
    sourcemeta::core::benchmark_do_not_optimize(frame);
  }
}

BENCHMARK(Schema_Frame_ISO_Language_Locations) {
  const auto schema{sourcemeta::core::read_json(
      std::filesystem::path{CURRENT_DIRECTORY} / "files" /
      "2020_12_iso_language_2023_set_3.json")};

  for (auto iteration : state) {
    sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::Locations, schema,
        sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};
    sourcemeta::core::benchmark_do_not_optimize(frame);
  }
}

BENCHMARK(Schema_Frame_ISO_Language_Root) {
  const auto schema{sourcemeta::core::read_json(
      std::filesystem::path{CURRENT_DIRECTORY} / "files" /
      "2020_12_iso_language_2023_set_3.json")};

  for (auto iteration : state) {
    sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::Root, schema,
        sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};
    sourcemeta::core::benchmark_do_not_optimize(frame);
  }
}

BENCHMARK(Schema_Frame_KrakenD_References) {
  const auto schema{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "files" / "2019_09_krakend.json")};

  for (auto iteration : state) {
    sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::References, schema,
        sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};
    sourcemeta::core::benchmark_do_not_optimize(frame);
  }
}

BENCHMARK(Schema_Frame_KrakenD_Reachable) {
  const auto schema{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "files" / "2019_09_krakend.json")};

  // Reachability is memoised on the frame, so every iteration has to start
  // from a frame that has not answered a query yet. Framing is therefore part
  // of what this measures, and the framing benchmark above accounts for it
  for (auto iteration : state) {
    const sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::References, schema,
        sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};

    frame.for_each_location(
        [&frame](const sourcemeta::core::SchemaReferenceType,
                 const std::string_view,
                 const sourcemeta::core::SchemaFrame::Location &entry) -> void {
          if (entry.type ==
              sourcemeta::core::SchemaFrame::LocationType::Pointer) {
            return;
          }

          frame.for_each_subschema(
              [&frame,
               &entry](const sourcemeta::core::SchemaFrame::Location &subentry)
                  -> void {
                auto result{frame.is_reachable(
                    subentry, entry, sourcemeta::core::schema_walker,
                    sourcemeta::core::schema_resolver)};
                sourcemeta::core::benchmark_do_not_optimize(result);
              });
        });
  }
}

BENCHMARK(Schema_Frame_ISO_Language_Locations_To_JSON) {
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
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}

BENCHMARK(Schema_Frame_Many_Resources_References) {
  const auto schema{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "files" / "2020_12_many_resources.json")};

  for (auto iteration : state) {
    sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::References, schema,
        sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};
    sourcemeta::core::benchmark_do_not_optimize(frame);
  }
}

BENCHMARK(Schema_Frame_Deeply_Nested_References) {
  const auto schema{
      sourcemeta::core::read_json(std::filesystem::path{CURRENT_DIRECTORY} /
                                  "files" / "2020_12_deeply_nested.json")};

  for (auto iteration : state) {
    sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::References, schema,
        sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};
    sourcemeta::core::benchmark_do_not_optimize(frame);
  }
}

BENCHMARK(Schema_Format_ISO_Language) {
  const auto document{sourcemeta::core::read_json(
      std::filesystem::path{CURRENT_DIRECTORY} / "files" /
      "2020_12_iso_language_2023_set_3.json")};

  for (auto iteration : state) {
    auto schema{document};
    const sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::Locations, schema,
        sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};
    sourcemeta::core::schema_format(schema, frame);
    sourcemeta::core::benchmark_do_not_optimize(schema);
  }
}

BENCHMARK(Schema_Bundle_Meta_2020_12) {
  const auto document{sourcemeta::core::schema_resolver(
                          "https://json-schema.org/draft/2020-12/schema")
                          .value()};

  for (auto iteration : state) {
    auto schema{document};
    sourcemeta::core::schema_bundle(schema, sourcemeta::core::schema_walker,
                                    sourcemeta::core::schema_resolver);
    sourcemeta::core::benchmark_do_not_optimize(schema);
  }
}

// Bundling checks that the fragment of a reference names something the remote
// actually has, once per distinct remote that a fragment reaches into
BENCHMARK(Schema_Bundle_Many_Remotes_With_Fragments) {
  static constexpr auto REMOTES{10};
  static constexpr auto SUBSCHEMAS{200};

  auto body{sourcemeta::core::JSON::make_object()};
  for (auto index = 0; index < SUBSCHEMAS; index++) {
    auto subschema{sourcemeta::core::JSON::make_object()};
    subschema.assign("type", sourcemeta::core::JSON{"string"});
    subschema.assign("minLength", sourcemeta::core::JSON{index});
    body.assign("property-" + std::to_string(index), std::move(subschema));
  }

  std::map<std::string, sourcemeta::core::JSON> registry;
  auto properties{sourcemeta::core::JSON::make_object()};
  for (auto index = 0; index < REMOTES; index++) {
    const auto identifier{"https://example.com/remote-" +
                          std::to_string(index)};
    auto remote{sourcemeta::core::JSON::make_object()};
    remote.assign(
        "$schema",
        sourcemeta::core::JSON{"https://json-schema.org/draft/2020-12/schema"});
    remote.assign("$id", sourcemeta::core::JSON{identifier});
    remote.assign("properties", body);
    registry.emplace(identifier, std::move(remote));

    auto property{sourcemeta::core::JSON::make_object()};
    property.assign(
        "$ref", sourcemeta::core::JSON{identifier + "#/properties/property-0"});
    properties.assign("from-" + std::to_string(index), std::move(property));
  }

  auto document{sourcemeta::core::JSON::make_object()};
  document.assign(
      "$schema",
      sourcemeta::core::JSON{"https://json-schema.org/draft/2020-12/schema"});
  document.assign("$id", sourcemeta::core::JSON{"https://example.com/main"});
  document.assign("properties", std::move(properties));

  const auto resolver{[&registry](const std::string_view identifier)
                          -> sourcemeta::core::SchemaResolverResult {
    const auto match{registry.find(std::string{identifier})};
    if (match != registry.cend()) {
      return match->second;
    }

    return sourcemeta::core::schema_resolver(identifier);
  }};

  for (auto iteration : state) {
    auto schema{document};
    sourcemeta::core::schema_bundle(schema, sourcemeta::core::schema_walker,
                                    resolver);
    sourcemeta::core::benchmark_do_not_optimize(schema);
  }
}
