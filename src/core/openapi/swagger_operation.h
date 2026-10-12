#ifndef SOURCEMETA_CORE_OPENAPI_SWAGGER_OPERATION_H_
#define SOURCEMETA_CORE_OPENAPI_SWAGGER_OPERATION_H_

#include <sourcemeta/core/openapi.h>

#include "external_documentation.h"
#include "helpers.h"
#include "reference.h"
#include "security.h"
#include "swagger_parameter.h"
#include "swagger_response.h"

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <set>         // std::set
#include <string_view> // std::string_view

namespace sourcemeta::core {

constexpr auto SWAGGER_HASH_CONSUMES{JSON::Object::hash("consumes"sv)};
constexpr auto SWAGGER_HASH_PRODUCES{JSON::Object::hash("produces"sv)};
constexpr auto SWAGGER_HASH_SCHEMES{JSON::Object::hash("schemes"sv)};
constexpr auto SWAGGER_HASH_DEPRECATED{JSON::Object::hash("deprecated"sv)};
constexpr auto SWAGGER_HASH_OPERATION_ID{JSON::Object::hash("operationId"sv)};
constexpr auto SWAGGER_HASH_PATHS{JSON::Object::hash("paths"sv)};

// Swagger 2.0, Path Item Object: a reference, the seven methods it names, and
// the parameters every one of them inherits. There is no trace method here,
// and no summary, description or server of its own
constexpr std::array<JSON::StringView, 9> SWAGGER_PATH_ITEM_FIELDS{
    {"$ref"sv, "get"sv, "put"sv, "post"sv, "delete"sv, "options"sv, "head"sv,
     "patch"sv, "parameters"sv}};

// The seven in the order the specification tables them
constexpr std::array<JSON::StringView, 7> SWAGGER_METHODS{
    {"get"sv, "put"sv, "post"sv, "delete"sv, "options"sv, "head"sv,
     "patch"sv}};

// Swagger 2.0, Operation Object
constexpr std::array<JSON::StringView, 12> SWAGGER_OPERATION_FIELDS{
    {"tags"sv, "summary"sv, "description"sv, "externalDocs"sv, "operationId"sv,
     "consumes"sv, "produces"sv, "parameters"sv, "responses"sv, "schemes"sv,
     "deprecated"sv, "security"sv}};

// Swagger Object: "schemes | [string] | The transfer protocol of the API.
// Values MUST be from the list: `"http"`, `"https"`, `"ws"`, `"wss"`", which
// its Operation Object repeats as an override
constexpr std::initializer_list<JSON::StringView> SWAGGER_SCHEMES{
    "http"sv, "https"sv, "ws"sv, "wss"sv};

// The media type lists an operation declares, and the ones the document
// declares for every operation to fall back on. Both are checked for shape
// alone, as what a media type means is settled by its own grammar rather than
// by this specification
inline auto swagger_check_media_type_list(const JSON &value,
                                          const Pointer &base,
                                          const char *type_message,
                                          const char *entry_message) -> void {
  openapi_check_array_of_strings(value, base, type_message, entry_message);
}

inline auto swagger_check_schemes(const JSON &value, const Pointer &base,
                                  const char *type_message) -> void {
  openapi_expect_array(value, base, type_message);
  std::size_t index{0};
  for (const auto &entry : value.as_array()) {
    openapi_expect_enumeration(
        entry, base, JSON::String{std::to_string(index)},
        SWAGGER_SCHEMES, "A scheme must be a string",
        "A scheme must name a transfer protocol this revision admits");
    index += 1;
  }
}

// The parameters a Path Item Object or an Operation Object declares. Swagger
// 2.0, Operation Object: "The list MUST NOT include duplicated parameters. A
// unique parameter is defined by a combination of a `name` and `location`"
inline auto swagger_check_parameter_list(const JSON &value,
                                         const Pointer &base,
                                         const char *type_message,
                                         OpenAPIWalk &walk)
    -> std::vector<JSON::String> {
  openapi_expect_array(value, base, type_message);
  std::vector<JSON::String> result;
  result.reserve(value.size());
  std::size_t index{0};
  for (const auto &entry : value.as_array()) {
    const auto location{openapi_child(base, index)};
    swagger_check_parameter_or_reference(entry, location, walk);
    result.emplace_back(openapi_location_uri(walk.base, location));
    index += 1;
  }

  return result;
}

// Swagger 2.0, Operation Object: "Describes a single API operation on a path"
inline auto swagger_check_operation(const JSON &value, const Pointer &base,
                                    OpenAPIWalk &walk) -> void {
  openapi_record(walk, base, OpenAPIObjectKind::Operation);
  openapi_expect_object(value, base, "The Operation Object must be an object");

  openapi_reject_unknown_fields(
      value, SWAGGER_OPERATION_FIELDS, base,
      "The Operation Object does not define this field");

  OpenAPIOperationRecord record;

  const auto *tags{value.try_at("tags"sv, OPENAPI_HASH_TAGS)};
  if (tags != nullptr) {
    openapi_check_array_of_strings(*tags, openapi_child(base, "tags"sv),
                                   "The Operation Object tags must be an array",
                                   "The Operation Object tags must be strings");
    for (const auto &entry : tags->as_array()) {
      record.tags.emplace_back(entry.to_string());
    }
  }

  openapi_check_optional_string(value, base, "summary"sv, OPENAPI_HASH_SUMMARY,
                                "The Operation Object summary must be a "
                                "string");
  openapi_check_optional_string(value, base, "description"sv,
                                OPENAPI_HASH_DESCRIPTION,
                                "The Operation Object description must be a "
                                "string");

  const auto *external_documentation{
      value.try_at("externalDocs"sv, OPENAPI_HASH_EXTERNAL_DOCS)};
  if (external_documentation != nullptr) {
    openapi_check_external_documentation(
        *external_documentation, openapi_child(base, "externalDocs"sv), walk);
  }

  // Operation Object: "operationId | string | Unique string used to identify
  // the operation. The id MUST be unique among all operations described in the
  // API"
  const auto *identifier{
      value.try_at("operationId"sv, SWAGGER_HASH_OPERATION_ID)};
  if (identifier != nullptr) {
    const auto named{openapi_expect_string(
        *identifier, base, "operationId"sv,
        "The Operation Object identifier must be a string")};
    const auto where{openapi_location_uri(walk.base, base)};
    const auto known{walk.operation_ids.find(JSON::String{named})};
    if (known != walk.operation_ids.cend() && known->second != where) {
      throw OpenAPIError{
          openapi_child(base, "operationId"sv),
          "The Operation Object identifier must not repeat another one"};
    }

    walk.operation_ids.insert_or_assign(JSON::String{named}, where);
  }

  const auto *consumes{value.try_at("consumes"sv, SWAGGER_HASH_CONSUMES)};
  if (consumes != nullptr) {
    swagger_check_media_type_list(
        *consumes, openapi_child(base, "consumes"sv),
        "The Operation Object consumed media types must be an array",
        "The Operation Object consumed media types must be strings");
  }

  const auto *produces{value.try_at("produces"sv, SWAGGER_HASH_PRODUCES)};
  if (produces != nullptr) {
    swagger_check_media_type_list(
        *produces, openapi_child(base, "produces"sv),
        "The Operation Object produced media types must be an array",
        "The Operation Object produced media types must be strings");
  }

  const auto *parameters{
      value.try_at("parameters"sv, SWAGGER_HASH_PARAMETERS)};
  if (parameters != nullptr) {
    record.parameters = swagger_check_parameter_list(
        *parameters, openapi_child(base, "parameters"sv),
        "The Operation Object parameters must be an array", walk);
  }

  const auto *schemes{value.try_at("schemes"sv, SWAGGER_HASH_SCHEMES)};
  if (schemes != nullptr) {
    swagger_check_schemes(*schemes, openapi_child(base, "schemes"sv),
                          "The Operation Object schemes must be an array");
  }

  const auto *deprecated{value.try_at("deprecated"sv, SWAGGER_HASH_DEPRECATED)};
  if (deprecated != nullptr) {
    openapi_expect_boolean(*deprecated, base, "deprecated"sv,
                           "The Operation Object deprecation must be a "
                           "boolean");
  }

  const auto *security{value.try_at("security"sv, OPENAPI_HASH_SECURITY)};
  if (security != nullptr) {
    record.security = openapi_check_security(
        *security, openapi_child(base, "security"sv),
        "The Operation Object security must be an array", walk);
  }

  // Operation Object: "responses | Responses Object | **Required.** The list
  // of possible responses as they are returned from executing this operation"
  const auto &responses{openapi_require(
      value, "responses"sv, SWAGGER_HASH_RESPONSES, base,
      "The Operation Object must declare its responses")};
  swagger_check_responses(responses, openapi_child(base, "responses"sv), walk);

  walk.operation_records.insert_or_assign(openapi_location_uri(walk.base, base),
                                          std::move(record));
}

// Swagger 2.0, Path Item Object: "Describes the operations available on a
// single path. A Path Item may be empty, due to ACL constraints"
inline auto swagger_check_path_item(const JSON &value, const Pointer &base,
                                    OpenAPIWalk &walk) -> void {
  openapi_record(walk, base, OpenAPIObjectKind::PathItem);
  openapi_expect_object(value, base, "The Path Item Object must be an object");

  openapi_reject_unknown_fields(
      value, SWAGGER_PATH_ITEM_FIELDS, base,
      "The Path Item Object does not define this field");

  OpenAPIPathItemRecord record;

  const auto *parameters{
      value.try_at("parameters"sv, SWAGGER_HASH_PARAMETERS)};
  if (parameters != nullptr) {
    record.parameters = swagger_check_parameter_list(
        *parameters, openapi_child(base, "parameters"sv),
        "The Path Item Object parameters must be an array", walk);
  }

  for (const auto &method : SWAGGER_METHODS) {
    const auto *operation{value.try_at(JSON::String{method})};
    if (operation == nullptr) {
      continue;
    }

    const auto location{openapi_child(base, method)};
    swagger_check_operation(*operation, location, walk);
    record.operations.emplace_back(JSON::String{method},
                                   openapi_location_uri(walk.base, location));
  }

  // Path Item Object: "$ref | string | Allows for an external definition of
  // this path item. The referenced structure MUST be in the format of a Path
  // Item Object. If there are conflicts between the referenced definition and
  // this Path Item's definition, the behavior is undefined". So the reference
  // is followed last, what sits beside it having been read already
  const auto *reference{value.try_at("$ref"sv, OPENAPI_HASH_REF)};
  if (reference != nullptr) {
    const auto target{openapi_expect_uri_reference(
        *reference, base, "$ref"sv,
        "The Path Item Object reference must be a string",
        "The Path Item Object reference must be a URI reference")};
    openapi_follow_reference(target, openapi_child(base, "$ref"sv),
                             OpenAPIObjectKind::PathItem, walk);
  }

  walk.path_items.insert_or_assign(openapi_location_uri(walk.base, base),
                                   std::move(record));
}

} // namespace sourcemeta::core

#endif
