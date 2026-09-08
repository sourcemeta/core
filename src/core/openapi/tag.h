#ifndef SOURCEMETA_CORE_OPENAPI_TAG_H_
#define SOURCEMETA_CORE_OPENAPI_TAG_H_

#include <sourcemeta/core/openapi.h>

#include "external_documentation.h"
#include "helpers.h"

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <set>         // std::set
#include <string_view> // std::string_view

namespace sourcemeta::core {

constexpr std::array<JSON::StringView, 3> OPENAPI_TAG_FIELDS{
    {"name"sv, "description"sv, "externalDocs"sv}};

// OpenAPI Specification 3.1.1, Section 4.8.22: "Adds metadata to a single tag
// that is used by the Operation Object"
inline auto openapi_check_tag(const JSON &value, const Pointer &base,
                              OpenAPIWalk &walk) -> JSON::StringView {
  openapi_record(walk, base, OpenAPIObjectKind::Tag);
  if (!value.is_object()) {
    throw OpenAPIError{base, "The Tag Object must be an object"};
  }

  openapi_reject_unknown_fields(value, OPENAPI_TAG_FIELDS, base,
                                "The Tag Object does not define this field");

  // OpenAPI Specification 3.1.1, Section 4.8.22: "name | string | REQUIRED.
  // The name of the tag"
  const auto *name{value.try_at("name", OPENAPI_HASH_NAME)};
  if (name == nullptr) {
    throw OpenAPIError{base, "The Tag Object must declare a name"};
  }

  const auto result{openapi_expect_string(
      *name, base, "name"sv, "The Tag Object name must be a string")};

  const auto *description{
      value.try_at("description", OPENAPI_HASH_DESCRIPTION)};
  if (description != nullptr) {
    openapi_expect_string(*description, base, "description"sv,
                          "The Tag Object description must be a string");
  }

  const auto *external_documentation{
      value.try_at("externalDocs", OPENAPI_HASH_EXTERNAL_DOCS)};
  if (external_documentation != nullptr) {
    openapi_check_external_documentation(
        *external_documentation, openapi_child(base, "externalDocs"sv), walk);
  }

  return result;
}

// OpenAPI Specification 3.1.1, Section 4.8.1: "tags | [Tag Object] | A list of
// tags used by the OpenAPI Description with additional metadata [...] Each tag
// name in the list MUST be unique"
// The tags the entry document declares, read before the walk goes anywhere so
// that the order documents are read in cannot decide what an Operation Object
// resolves against. Like the security schemes, this runs before the tags
// themselves have been checked, so it takes what is there and leaves being
// strict about the shape to the check that owns it
inline auto openapi_collect_tags(const JSON &document, OpenAPIWalk &walk)
    -> void {
  const auto *tags{document.try_at("tags", OPENAPI_HASH_TAGS)};
  if (tags == nullptr || !tags->is_array()) {
    return;
  }

  const Pointer base{"tags"};
  std::size_t index{0};
  for (const auto &tag : tags->as_array()) {
    const auto *name{tag.is_object() ? tag.try_at("name", OPENAPI_HASH_NAME)
                                     : nullptr};
    if (name != nullptr && name->is_string()) {
      walk.tags.emplace(
          name->to_string(),
          openapi_location_uri(walk.base, openapi_child(base, index)));
    }

    index += 1;
  }
}

inline auto openapi_check_tags(const JSON &document, OpenAPIWalk &walk)
    -> void {
  const auto *tags{document.try_at("tags", OPENAPI_HASH_TAGS)};
  if (tags == nullptr) {
    return;
  }

  const Pointer base{"tags"};
  if (!tags->is_array()) {
    throw OpenAPIError{base, "The OpenAPI Description tags must be an array"};
  }

  // Uniqueness is stated in the prose alone. The published meta-schema says
  // nothing about it, as no keyword there can compare a property across the
  // members of an array
  std::set<JSON::StringView> names;
  std::size_t index{0};
  for (const auto &tag : tags->as_array()) {
    const auto location{openapi_child(base, index)};
    const auto name{openapi_check_tag(tag, location, walk)};
    if (!names.insert(name).second) {
      throw OpenAPIError{openapi_child(location, "name"sv),
                         "The OpenAPI Description tag names must be unique"};
    }

    index += 1;
  }
}

} // namespace sourcemeta::core

#endif
