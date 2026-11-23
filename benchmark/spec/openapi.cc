#include <sourcemeta/core/benchmark.h>

#include <cassert>     // assert
#include <cstddef>     // std::size_t
#include <filesystem>  // std::filesystem
#include <optional>    // std::nullopt
#include <string>      // std::string, std::to_string
#include <string_view> // std::string_view
#include <utility>     // std::move, std::pair
#include <vector>      // std::vector

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

// The path count sets how many references bundling has to gather for, and the
// shared count how many distinct documents it reaches, so they are the knobs
// to turn when stress testing this functionality
static constexpr std::size_t PATH_COUNT{64};
static constexpr std::size_t SHARED_COUNT{8};
static constexpr std::size_t FIELDS_PER_SCHEMA{4};

static auto shared_identifier(const std::size_t index) -> std::string {
  return "https://example.com/shared-" + std::to_string(index) + ".json";
}

static auto make_shared_schema(const std::size_t index)
    -> sourcemeta::core::JSON {
  auto properties{sourcemeta::core::JSON::make_object()};
  for (std::size_t field = 0; field < FIELDS_PER_SCHEMA; field += 1) {
    auto property{sourcemeta::core::JSON::make_object()};
    property.assign("type", sourcemeta::core::JSON{"string"});
    property.assign("title",
                    sourcemeta::core::JSON{"Field " + std::to_string(field) +
                                           " of " + std::to_string(index)});
    properties.assign("field-" + std::to_string(field), std::move(property));
  }

  auto schema{sourcemeta::core::JSON::make_object()};
  schema.assign("type", sourcemeta::core::JSON{"object"});
  schema.assign("properties", std::move(properties));
  return schema;
}

static auto make_shared_document(const std::size_t index)
    -> sourcemeta::core::JSON {
  const auto number{std::to_string(index)};
  auto schema{make_shared_schema(index)};

  auto media{sourcemeta::core::JSON::make_object()};
  media.assign("schema", schema);
  auto content{sourcemeta::core::JSON::make_object()};
  content.assign("application/json", std::move(media));
  auto response{sourcemeta::core::JSON::make_object()};
  response.assign("description", sourcemeta::core::JSON{"Shared " + number});
  response.assign("content", std::move(content));

  auto responses{sourcemeta::core::JSON::make_object()};
  responses.assign("Thing", std::move(response));
  auto schemas{sourcemeta::core::JSON::make_object()};
  schemas.assign("Thing", std::move(schema));
  auto components{sourcemeta::core::JSON::make_object()};
  components.assign("responses", std::move(responses));
  components.assign("schemas", std::move(schemas));

  auto info{sourcemeta::core::JSON::make_object()};
  info.assign("title", sourcemeta::core::JSON{"Shared " + number});
  info.assign("version", sourcemeta::core::JSON{"1.0.0"});

  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("openapi", sourcemeta::core::JSON{"3.1.1"});
  document.assign("info", std::move(info));
  document.assign("components", std::move(components));
  return document;
}

using SharedDocument = std::pair<std::string, sourcemeta::core::JSON>;

static auto make_shared_documents() -> std::vector<SharedDocument> {
  std::vector<SharedDocument> result;
  result.reserve(SHARED_COUNT);
  for (std::size_t index = 0; index < SHARED_COUNT; index += 1) {
    result.emplace_back(shared_identifier(index), make_shared_document(index));
  }
  return result;
}

// The resolver below runs inside the timed loop, so each document is stored
// against the identifier it answers to rather than having one built per
// candidate on every lookup
static auto shared_documents() -> const std::vector<SharedDocument> & {
  static const std::vector<SharedDocument> INSTANCE{make_shared_documents()};
  return INSTANCE;
}

static auto bundle_resolver(const std::string_view identifier)
    -> sourcemeta::core::OpenAPIResolverResult {
  for (const auto &entry : shared_documents()) {
    if (identifier == entry.first) {
      return entry.second;
    }
  }

  return std::nullopt;
}

static auto make_split_description() -> sourcemeta::core::JSON {
  auto paths{sourcemeta::core::JSON::make_object()};
  for (std::size_t index = 0; index < PATH_COUNT; index += 1) {
    const auto shared{shared_identifier(index % SHARED_COUNT)};

    auto response{sourcemeta::core::JSON::make_object()};
    response.assign("$ref", sourcemeta::core::JSON{
                                shared + "#/components/responses/Thing"});
    auto responses{sourcemeta::core::JSON::make_object()};
    responses.assign("200", std::move(response));

    auto schema{sourcemeta::core::JSON::make_object()};
    schema.assign(
        "$ref", sourcemeta::core::JSON{shared + "#/components/schemas/Thing"});
    auto media{sourcemeta::core::JSON::make_object()};
    media.assign("schema", std::move(schema));
    auto content{sourcemeta::core::JSON::make_object()};
    content.assign("application/json", std::move(media));
    auto body{sourcemeta::core::JSON::make_object()};
    body.assign("content", std::move(content));

    auto operation{sourcemeta::core::JSON::make_object()};
    operation.assign("operationId",
                     sourcemeta::core::JSON{"thing" + std::to_string(index)});
    operation.assign("requestBody", std::move(body));
    operation.assign("responses", std::move(responses));

    auto item{sourcemeta::core::JSON::make_object()};
    item.assign("post", std::move(operation));
    paths.assign("/thing-" + std::to_string(index), std::move(item));
  }

  auto info{sourcemeta::core::JSON::make_object()};
  info.assign("title", sourcemeta::core::JSON{"Split"});
  info.assign("version", sourcemeta::core::JSON{"1.0.0"});

  auto document{sourcemeta::core::JSON::make_object()};
  document.assign("openapi", sourcemeta::core::JSON{"3.1.1"});
  document.assign("info", std::move(info));
  document.assign("paths", std::move(paths));
  return document;
}

BENCHMARK(OpenAPI_Bundle_Split_3_1) {
  const auto document{make_split_description()};

  for (auto iteration : state) {
    auto result{sourcemeta::core::openapi_bundle(
        document, sourcemeta::core::schema_walker,
        sourcemeta::core::schema_resolver, bundle_resolver)};
    assert(result.is_object());
    assert(result.at("paths").size() == PATH_COUNT);
    assert(result.at("components").at("responses").size() == SHARED_COUNT);
    assert(result.at("components").at("schemas").size() == SHARED_COUNT);
    sourcemeta::core::benchmark_do_not_optimize(result);
  }
}
