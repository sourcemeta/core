#ifndef SOURCEMETA_CORE_OPENAPI_SWAGGER_H_
#define SOURCEMETA_CORE_OPENAPI_SWAGGER_H_

#include <sourcemeta/core/openapi.h>

#include "external_documentation.h"
#include "helpers.h"
#include "info.h"
#include "paths.h"
#include "security.h"
#include "swagger_operation.h"
#include "swagger_parameter.h"
#include "swagger_response.h"
#include "swagger_security.h"
#include "tag.h"

#include <array>       // std::array
#include <cassert>     // assert
#include <set>         // std::set
#include <string_view> // std::string_view

namespace sourcemeta::core {

constexpr auto SWAGGER_HASH_HOST{JSON::Object::hash("host"sv)};
constexpr auto SWAGGER_HASH_BASE_PATH{JSON::Object::hash("basePath"sv)};
constexpr auto SWAGGER_HASH_DEFINITIONS{JSON::Object::hash("definitions"sv)};

// Swagger 2.0, Swagger Object: the fifteen fields the root of a document of
// this revision tables. Four of them are the containers that the Components
// Object gathers into one from the next revision onwards, and three describe
// where the API is served in place of the Server Objects that replace them
constexpr std::array<JSON::StringView, 15> SWAGGER_ROOT_FIELDS{
    {"swagger"sv, "info"sv, "host"sv, "basePath"sv, "schemes"sv, "consumes"sv,
     "produces"sv, "paths"sv, "definitions"sv, "parameters"sv, "responses"sv,
     "securityDefinitions"sv, "security"sv, "tags"sv, "externalDocs"sv}};

// Swagger 2.0, Definitions Object: "An object to hold data types that can be
// consumed and produced by operations. These data types can be primitives,
// arrays or models". Every member is a Schema Object of this revision, and
// this is the location that bundling reaches for, the way the Components
// Object is from the next revision onwards
inline auto swagger_check_definitions(const JSON &document, OpenAPIWalk &walk)
    -> void {
  const auto *definitions{
      document.try_at("definitions"sv, SWAGGER_HASH_DEFINITIONS)};
  if (definitions == nullptr) {
    return;
  }

  const Pointer base{"definitions"};
  openapi_record(walk, base, OpenAPIObjectKind::Definitions);
  openapi_expect_object(*definitions, base,
                        "The Definitions Object must be an object");

  for (const auto &entry : definitions->as_object()) {
    openapi_expect_schema(entry.second, openapi_child(base, entry.first),
                          walk);
  }
}

// Swagger 2.0, Paths Object: "Holds the relative paths to the individual
// endpoints". Its shape is the one every later revision keeps, so what differs
// is only the Path Item Object each of its members holds
inline auto swagger_check_paths(const JSON &document, OpenAPIWalk &walk)
    -> void {
  const auto *paths{document.try_at("paths"sv, SWAGGER_HASH_PATHS)};
  // The Swagger Object marks this required, so nothing reaches here without it
  assert(paths != nullptr);

  const Pointer base{"paths"};
  openapi_record(walk, base, OpenAPIObjectKind::Paths);
  openapi_expect_object(*paths, base, "The Paths Object must be an object");

  std::set<JSON::String> shapes;
  for (const auto &entry : paths->as_object()) {
    if (entry.first.starts_with(OPENAPI_EXTENSION_PREFIX)) {
      continue;
    }

    const auto location{openapi_child(base, entry.first)};

    // Paths Object: "A relative path to an individual endpoint. The field name
    // MUST begin with a slash"
    if (!entry.first.starts_with('/')) {
      throw OpenAPIError{location,
                         "The Paths Object keys must begin with a slash"};
    }

    // Paths Object: "Templated paths with the same hierarchy but different
    // templated names MUST NOT exist as they are identical". The shape a path
    // reduces to is what tells two of those apart, and this revision says it
    // in the same words every later one does
    if (!shapes.insert(openapi_path_shape(entry.first)).second) {
      throw OpenAPIError{
          location, "The Paths Object keys must not repeat a templated path"};
    }

    swagger_check_path_item(entry.second, location, walk);

    walk.endpoints.push_back(
        {.kind = OpenAPIOperationKind::Path,
         .path = entry.first,
         .path_item = openapi_location_uri(walk.base, location)});
  }
}

// Swagger 2.0, Swagger Object: "This is the root document object for the API
// specification. It combines what previously was the Resource Listing and API
// Declaration (version 1.2 and earlier) together into one document"
inline auto swagger_check_root(const JSON &document, OpenAPIWalk &walk)
    -> void {
  openapi_reject_unknown_fields(
      document, SWAGGER_ROOT_FIELDS, EMPTY_POINTER,
      "The Swagger Object does not define this field");

  // The names a Security Requirement Object anywhere in the description may
  // draw on come from the entry document, the same way they do in every later
  // revision, and are taken before anything else is read
  if (!walk.referenced) {
    swagger_collect_security_schemes(document, walk);
    openapi_collect_tags(document, walk);
  }

  // Swagger Object: "paths | Paths Object | **Required.** The available paths
  // and operations for the API"
  if (document.try_at("paths"sv, SWAGGER_HASH_PATHS) == nullptr) {
    throw OpenAPIError{EMPTY_POINTER, "The Swagger Object must declare paths"};
  }

  walk.info = openapi_parse_info(document, walk);

  // This revision publishes no field for naming a dialect, so the one its
  // Schema Objects fall back on is the only one ever in force
  JSON::String dialect{openapi_dialect(walk.version)};
  walk.dialect = dialect;
  openapi_record(walk, EMPTY_POINTER, OpenAPIObjectKind::Document,
                 std::move(dialect));

  // Swagger Object: "host | string | The host (name or ip) serving the API.
  // This MUST be the host only and does not include the scheme nor sub-paths"
  openapi_check_optional_string(
      document, EMPTY_POINTER, "host"sv, SWAGGER_HASH_HOST,
      "The Swagger Object host must be a string");

  // "basePath | string | The base path on which the API is served [...] The
  // value MUST start with a leading slash (`/`)"
  const auto *base_path{
      document.try_at("basePath"sv, SWAGGER_HASH_BASE_PATH)};
  if (base_path != nullptr) {
    const auto value{openapi_expect_string(
        *base_path, EMPTY_POINTER, "basePath"sv,
        "The Swagger Object base path must be a string")};
    if (!value.starts_with('/')) {
      throw OpenAPIError{Pointer{"basePath"},
                         "The Swagger Object base path must begin with a "
                         "slash"};
    }
  }

  const auto *schemes{document.try_at("schemes"sv, SWAGGER_HASH_SCHEMES)};
  if (schemes != nullptr) {
    swagger_check_schemes(*schemes, Pointer{"schemes"},
                          "The Swagger Object schemes must be an array");
  }

  const auto *consumes{document.try_at("consumes"sv, SWAGGER_HASH_CONSUMES)};
  if (consumes != nullptr) {
    swagger_check_media_type_list(
        *consumes, Pointer{"consumes"},
        "The Swagger Object consumed media types must be an array",
        "The Swagger Object consumed media types must be strings");
  }

  const auto *produces{document.try_at("produces"sv, SWAGGER_HASH_PRODUCES)};
  if (produces != nullptr) {
    swagger_check_media_type_list(
        *produces, Pointer{"produces"},
        "The Swagger Object produced media types must be an array",
        "The Swagger Object produced media types must be strings");
  }

  openapi_check_root_external_documentation(document, walk);
  openapi_check_tags(document, walk);

  // The four containers this revision spreads across its root, each read
  // before the paths that reference into them
  swagger_check_definitions(document, walk);
  swagger_check_parameters_definitions(document, walk);
  swagger_check_responses_definitions(document, walk);
  swagger_check_security_definitions(document, walk);

  swagger_check_paths(document, walk);

  const auto *security{document.try_at("security"sv, OPENAPI_HASH_SECURITY)};
  if (security != nullptr) {
    walk.security = openapi_check_security(
        *security, Pointer{"security"},
        "The Swagger Object security must be an array", walk);
  }
}

} // namespace sourcemeta::core

#endif
