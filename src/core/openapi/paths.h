#ifndef SOURCEMETA_CORE_OPENAPI_PATHS_H_
#define SOURCEMETA_CORE_OPENAPI_PATHS_H_

#include <sourcemeta/core/openapi.h>

#include "helpers.h"
#include "path_item.h"

#include <cstddef>     // std::size_t
#include <set>         // std::set
#include <string_view> // std::string_view
#include <vector>      // std::vector

namespace sourcemeta::core {

constexpr auto OPENAPI_HASH_PATHS{JSON::Object::hash("paths"sv)};
constexpr auto OPENAPI_HASH_WEBHOOKS{JSON::Object::hash("webhooks"sv)};

// Two paths are "identical" when they differ only in what their template
// expressions are called, so comparing them means dropping those names while
// leaving every other byte in place
inline auto openapi_path_shape(const JSON::StringView path) -> JSON::String {
  JSON::String result;
  result.reserve(path.size());
  std::size_t cursor{0};
  while (cursor < path.size()) {
    const auto open{path.find('{', cursor)};
    if (open == JSON::StringView::npos) {
      break;
    }

    // A brace that never closes opens no expression, so what follows it is
    // ordinary text. Dropping it instead would make two paths that differ only
    // after an unclosed brace compare as one, and 3.1 constrains neither
    const auto close{path.find('}', open)};
    if (close == JSON::StringView::npos) {
      break;
    }

    result.append(path.substr(cursor, open - cursor)).append("{}");
    cursor = close + 1;
  }

  result.append(path.substr(cursor));
  return result;
}

// The template expressions a path declares. OpenAPI Specification 3.1.1,
// Section 4.3: "Path templating refers to the usage of template expressions,
// delimited by curly braces (`{}`), to mark a section of a URL path as
// replaceable using path parameters". Nothing there says what an unbalanced
// brace means, so a run that never closes is no expression
inline auto openapi_path_templates(const JSON::StringView path)
    -> std::vector<JSON::StringView> {
  std::vector<JSON::StringView> result;
  std::size_t cursor{0};
  while (cursor < path.size()) {
    const auto open{path.find('{', cursor)};
    if (open == JSON::StringView::npos) {
      break;
    }

    const auto close{path.find('}', open)};
    if (close == JSON::StringView::npos) {
      break;
    }

    result.push_back(path.substr(open + 1, close - open - 1));
    cursor = close + 1;
  }

  return result;
}

// OpenAPI Specification 3.1.1, Section 4.8.8: "Holds the relative paths to the
// individual endpoints and their operations"
inline auto openapi_check_paths(const JSON &document, OpenAPIWalk &walk)
    -> void {
  const auto *paths{document.try_at("paths", OPENAPI_HASH_PATHS)};
  if (paths == nullptr) {
    return;
  }

  const Pointer base{"paths"};
  openapi_record(walk, base, OpenAPIObjectKind::Paths);
  openapi_expect_object(*paths, base, "The Paths Object must be an object");

  std::set<JSON::String> shapes;
  for (const auto &entry : paths->as_object()) {
    if (entry.first.starts_with(OPENAPI_EXTENSION_PREFIX)) {
      continue;
    }

    const auto location{openapi_child(base, entry.first)};

    // OpenAPI Specification 3.1.1, Section 4.8.8: "The field name MUST begin
    // with a forward slash (`/`)"
    if (!entry.first.starts_with('/')) {
      throw OpenAPIError{location,
                         "The Paths Object keys must begin with a slash"};
    }

    // OpenAPI Specification 3.1.1, Section 4.8.8: "Templated paths with the
    // same hierarchy but different templated names MUST NOT exist as they are
    // identical". The published meta-schema cannot state this, as no keyword
    // there compares one property name against another
    if (!shapes.insert(openapi_path_shape(entry.first)).second) {
      throw OpenAPIError{
          location, "The Paths Object keys must not repeat a templated path"};
    }

    openapi_check_path_item(entry.second, location, walk);

    // Section 3: "only the entry document's Paths Object contributes URLs to
    // the described API", so a Paths Object in a document a reference brought
    // in describes nothing
    if (walk.entry) {
      walk.endpoints.push_back(
          {.kind = OpenAPIOperationKind::Path,
           .path = entry.first,
           .path_item = openapi_location_uri(walk.base, location)});
    }
  }
}

// OpenAPI Specification 3.1.1, Section 4.8.1: "webhooks | Map[string, Path
// Item Object] | The incoming webhooks that MAY be received as part of this
// API". Its keys are names rather than paths, and unlike the Paths Object this
// map carries no extension carve-out, so a member named `x-` is a webhook
inline auto openapi_check_webhooks(const JSON &document, OpenAPIWalk &walk)
    -> void {
  const auto *webhooks{document.try_at("webhooks", OPENAPI_HASH_WEBHOOKS)};
  if (webhooks == nullptr) {
    return;
  }

  const Pointer base{"webhooks"};
  openapi_expect_object(*webhooks, base,
                        "The OpenAPI Description webhooks must be an object");

  for (const auto &entry : webhooks->as_object()) {
    const auto location{openapi_child(base, entry.first)};
    openapi_check_path_item(entry.second, location, walk);
    if (walk.entry) {
      walk.endpoints.push_back(
          {.kind = OpenAPIOperationKind::Webhook,
           .path = entry.first,
           .path_item = openapi_location_uri(walk.base, location)});
    }
  }
}

} // namespace sourcemeta::core

#endif
