#ifndef SOURCEMETA_CORE_OPENAPI_DISCRIMINATOR_H_
#define SOURCEMETA_CORE_OPENAPI_DISCRIMINATOR_H_

#include <sourcemeta/core/openapi.h>

#include "components.h"
#include "helpers.h"

#include <set>         // std::set
#include <string_view> // std::string_view
#include <utility>     // std::move
#include <vector>      // std::vector

namespace sourcemeta::core {

constexpr auto OPENAPI_HASH_DISCRIMINATOR{
    JSON::Object::hash("discriminator"sv)};
constexpr auto OPENAPI_HASH_MAPPING{JSON::Object::hash("mapping"sv)};
constexpr auto OPENAPI_HASH_DEFAULT_MAPPING{
    JSON::Object::hash("defaultMapping"sv)};

/// Where a Discriminator Object names a schema by URI. OpenAPI Specification
/// 3.1.1, Section 4.3.1 lists the URI form of a `mapping` among the fields
/// that connect the documents of a description, which makes one of these a
/// reference like any other
struct OpenAPIDiscriminator {
  /// Where the mapping value sits, as a pointer from the root of the document
  Pointer origin;
  /// Where it points, resolved and canonicalised
  JSON::String destination;
  /// What the schema holding it resolves against, which is the nearest
  /// identifier an enclosing schema declares
  JSON::String scope;
};

// OpenAPI Specification 3.1.1, Section 4.8.25: a `mapping` entry "maps a
// specific property value to either a different schema component name, or to a
// schema identified by a URI". Only the latter is a reference, as Section
// 4.3.3 lists the name form among the connections a description makes by name,
// and Section 4.8.25 settles which of the two a value is:
//
//   The behavior of a `mapping` value that is both a valid schema name and a
//   valid relative URI reference is implementation-defined, but it is
//   RECOMMENDED that it be treated as a schema name. To ensure that an
//   ambiguous value (e.g. `"foo"`) is treated as a relative URI reference by
//   all implementations, authors MUST prefix it with the `"."` path segment
//
// A schema name is what the Components Object takes as a key, and the path
// segment an author writes to force the other reading is no such key
inline auto openapi_is_discriminator_reference(const JSON &value) -> bool {
  return value.is_string() && !openapi_is_component_key(value.to_string());
}

// Keep what one of those names, where it names a schema by URI rather than by
// the name a component goes by
inline auto
openapi_record_discriminator(std::vector<OpenAPIDiscriminator> &result,
                             Pointer origin, const JSON &value,
                             const JSON::String &scope) -> void {
  if (!openapi_is_discriminator_reference(value)) {
    return;
  }

  auto destination{openapi_resolve_reference(value.to_string(), scope)};
  if (!destination.has_value()) {
    return;
  }

  result.push_back({.origin = std::move(origin),
                    .destination = std::move(destination.value()),
                    .scope = scope});
}

// Every schema that a Discriminator Object of the document names by URI.
// Section 4.3 has a relative reference of a Schema Object resolve against
// "the nearest parent `$id`", which is the base that framing the schemas
// settles for wherever the Discriminator Object sits
inline auto openapi_discriminators(const JSON &document,
                                   const SchemaFrame &schemas,
                                   const OpenAPIVersion version)
    -> std::vector<OpenAPIDiscriminator> {
  std::vector<OpenAPIDiscriminator> result;
  // A schema that declares an identifier of its own is registered under every
  // base it can be reached by, and what it holds is the one thing whichever
  // way it is reached
  std::set<JSON::String> seen;
  schemas.for_each_subschema([&document, &result, &seen,
                              version](const auto &location) -> void {
    const auto *schema{try_get(document, location.pointer)};
    if (schema == nullptr || !schema->is_object()) {
      return;
    }

    const auto *discriminator{
        schema->try_at("discriminator", OPENAPI_HASH_DISCRIMINATOR)};
    if (discriminator == nullptr || !discriminator->is_object()) {
      return;
    }

    auto base{to_pointer(location.pointer)};
    if (!seen.insert(to_string(base)).second) {
      return;
    }

    const JSON::String scope{location.base};
    base.push_back(JSON::String{"discriminator"});

    const auto *mapping{discriminator->try_at("mapping", OPENAPI_HASH_MAPPING)};
    if (mapping != nullptr && mapping->is_object()) {
      const auto entries{base.concat(JSON::String{"mapping"})};
      for (const auto &entry : mapping->as_object()) {
        openapi_record_discriminator(result, entries.concat(entry.first),
                                     entry.second, scope);
      }
    }

    // OpenAPI Specification 3.2.1, Section 4.26 gives a Discriminator Object a
    // default of its own, which is "the schema name or URI reference to a
    // schema" just as every entry of the map beside it is. Only that revision
    // defines the field, and the dialect of the one before it turns down
    // anything it does not name
    if (version != OpenAPIVersion::OPENAPI_3_2) {
      return;
    }

    const auto *fallback{
        discriminator->try_at("defaultMapping", OPENAPI_HASH_DEFAULT_MAPPING)};
    if (fallback != nullptr) {
      openapi_record_discriminator(result,
                                   base.concat(JSON::String{"defaultMapping"}),
                                   *fallback, scope);
    }
  });

  return result;
}

} // namespace sourcemeta::core

#endif
