#ifndef SOURCEMETA_CORE_OPENAPI_SWAGGER_OPERATION_H_
#define SOURCEMETA_CORE_OPENAPI_SWAGGER_OPERATION_H_

#include <sourcemeta/core/openapi.h>

#include "external_documentation.h"
#include "helpers.h"
#include "reference.h"
#include "security.h"
#include "swagger_parameter.h"
#include "swagger_response.h"

#include <algorithm>   // std::max, std::ranges::all_of, std::ranges::find
#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <map>         // std::map
#include <optional>    // std::optional
#include <set>         // std::set
#include <string_view> // std::string_view
#include <utility>     // std::pair

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
    {"get"sv, "put"sv, "post"sv, "delete"sv, "options"sv, "head"sv, "patch"sv}};

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
        entry, base, JSON::String{std::to_string(index)}, SWAGGER_SCHEMES,
        "A scheme must be a string",
        "A scheme must name a transfer protocol this revision admits");
    index += 1;
  }
}

// The parameters a Path Item Object or an Operation Object declares. Swagger
// 2.0 states three rules of such a list, and its Path Item Object and its
// Operation Object state them in the same words as each other.
//
// Of repetition: "The list MUST NOT include duplicated parameters. A unique
// parameter is defined by a combination of a `name` and `location`". A
// Reference Object names neither until it is followed, and following is eager,
// so what one leads to is compared alongside the parameters written out in
// place. One that could not be followed names nothing and is left out.
//
// Of the body: "There can be one `"body"` parameter at most", which its
// Parameter Object gives the reason for: "Since there can only be one payload,
// there can only be *one* body parameter".
//
// And of a body beside a form, the same paragraph: "Since Form parameters are
// also in the payload, body and form parameters cannot exist together for the
// same operation", which its Form bullet repeats from the other side: "Since
// form parameters are sent in the payload, they cannot be declared together
// with a body parameter for the same operation"
inline auto swagger_check_parameter_list(const JSON &value, const Pointer &base,
                                         const char *type_message,
                                         OpenAPIWalk &walk)
    -> std::vector<JSON::String> {
  openapi_expect_array(value, base, type_message);
  std::vector<JSON::String> result;
  result.reserve(value.size());
  // These own their strings, as an identity read back through a reference
  // borrows from a map the walk keeps writing to
  std::set<std::pair<JSON::String, JSON::String>> seen;
  std::optional<std::size_t> body;
  std::optional<std::size_t> form;
  std::size_t index{0};
  for (const auto &entry : value.as_array()) {
    const auto location{openapi_child(base, index)};
    std::optional<std::pair<JSON::String, JSON::String>> identity;
    if (openapi_is_reference(entry)) {
      swagger_check_parameter_or_reference(entry, location, walk);
      const auto *named{openapi_parameter_identity(
          walk, openapi_location_uri(walk.base, location))};
      if (named != nullptr) {
        identity = *named;
      }
    } else {
      const auto named{swagger_check_parameter(entry, location, walk)};
      identity =
          std::pair{JSON::String{named.first}, JSON::String{named.second}};
    }

    if (identity.has_value()) {
      if (!seen.insert(identity.value()).second) {
        throw OpenAPIError{location,
                           "The parameters must not repeat another one"};
      }

      if (identity->second == "body") {
        if (body.has_value()) {
          throw OpenAPIError{location,
                             "The parameters must declare at most one body"};
        }

        body = index;
      } else if (identity->second == "formData") {
        form = index;
      }
    }

    result.emplace_back(openapi_location_uri(walk.base, location));
    index += 1;
  }

  if (body.has_value() && form.has_value()) {
    // Whichever of the two comes second is the one this names, a list holding
    // only one of them being no trouble at all
    throw OpenAPIError{
        openapi_child(base, std::max(body.value(), form.value())),
        "The parameters must not carry both a body and a form"};
  }

  return result;
}

// Swagger 2.0 states the rules about a payload of an operation rather than of
// a list, its Parameter Object reading "body and form parameters cannot exist
// together for the same operation" and "there can only be *one* body
// parameter". An operation carries what it declares and what its Path Item
// Object declares for every operation under it, so the two lists are folded
// before the rules are put to them.
//
// Operation Object, of how they fold: "If a parameter is already defined at
// the Path Item, the new definition will override it, but can never remove
// it". A parameter is identified by its name and its location, so one the
// operation writes under an identity the Path Item already used replaces it
// rather than joining it
inline auto swagger_check_what_an_operation_carries(
    const Pointer &base, const std::vector<JSON::String> &own,
    const std::vector<JSON::String> &inherited, OpenAPIWalk &walk) -> void {
  std::map<std::pair<JSON::String, JSON::String>, JSON::String> effective;
  // The inherited list goes in first so that what the operation declares
  // overrides it rather than the other way round
  for (const auto *list : {&inherited, &own}) {
    for (const auto &position : *list) {
      const auto *identity{openapi_parameter_identity(walk, position)};
      if (identity != nullptr) {
        effective.insert_or_assign(*identity, position);
      }
    }
  }

  std::size_t bodies{0};
  bool form{false};
  for (const auto &entry : effective) {
    if (entry.first.second == "body") {
      bodies += 1;
    } else if (entry.first.second == "formData") {
      form = true;
    }
  }

  if (bodies > 1) {
    throw OpenAPIError{
        base, "The parameters of an operation must declare at most one body"};
  }

  if (bodies == 1 && form) {
    throw OpenAPIError{base,
                       "The parameters of an operation must not carry both a "
                       "body and a form"};
  }
}

// Swagger 2.0, Parameter Object, of the two media types a payload carrying a
// file may be sent as. The second is written with a leading space where the
// specification states the rule, which the Mime Types section it points at
// does not carry, so the name is read without it
constexpr std::initializer_list<JSON::StringView> SWAGGER_FORM_MEDIA_TYPES{
    "multipart/form-data"sv, "application/x-www-form-urlencoded"sv};

// Parameter Object: "If `type` is `"file"`, the `consumes` MUST be either
// `"multipart/form-data"`, `" application/x-www-form-urlencoded"` or both and
// the parameter MUST be `in` `"formData"`".
//
// What `consumes` names is the Operation Object's, and the Swagger Object
// states its own for every operation to fall back on: "A list of MIME types
// the APIs can consume. This is global to all APIs but can be overridden on
// specific API calls". So the list held against the rule is the operation's
// own where it declares one, and the document's otherwise.
//
// The parameters reaching an operation are the ones it declares and the ones
// its Path Item Object declares for every operation under it, so both lists
// are asked
inline auto
swagger_check_what_a_file_requires(const JSON &value, const Pointer &base,
                                   const std::vector<JSON::String> &own,
                                   const std::vector<JSON::String> &inherited,
                                   OpenAPIWalk &walk) -> void {
  const auto *declared{value.try_at("consumes"sv, SWAGGER_HASH_CONSUMES)};
  std::vector<JSON::StringView> effective;
  if (declared != nullptr && declared->is_array()) {
    for (const auto &entry : declared->as_array()) {
      if (entry.is_string()) {
        effective.emplace_back(entry.to_string());
      }
    }
  } else {
    for (const auto &entry : walk.consumes) {
      effective.emplace_back(entry);
    }
  }

  // A list naming nothing but those two satisfies the rule, and one naming
  // none of them does not, an operation declaring no media type at all having
  // no `consumes` that is either of them
  const auto consumes_a_form{
      !effective.empty() &&
      std::ranges::all_of(effective, [](const auto media_type) {
        return std::ranges::find(SWAGGER_FORM_MEDIA_TYPES, media_type) !=
               SWAGGER_FORM_MEDIA_TYPES.end();
      })};
  if (consumes_a_form) {
    return;
  }

  for (const auto *list : {&own, &inherited}) {
    for (const auto &position : *list) {
      if (walk.file_parameters.contains(
              openapi_resolve_position(walk, position))) {
        throw OpenAPIError{
            base, "An operation taking a file must consume a form media type"};
      }
    }
  }
}

// Swagger 2.0, Operation Object: "Describes a single API operation on a path"
inline auto swagger_check_operation(const JSON &value, const Pointer &base,
                                    const std::vector<JSON::String> &inherited,
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

  const auto *parameters{value.try_at("parameters"sv, SWAGGER_HASH_PARAMETERS)};
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
  const auto &responses{
      openapi_require(value, "responses"sv, SWAGGER_HASH_RESPONSES, base,
                      "The Operation Object must declare its responses")};
  swagger_check_responses(responses, openapi_child(base, "responses"sv), walk);

  swagger_check_what_an_operation_carries(base, record.parameters, inherited,
                                          walk);
  swagger_check_what_a_file_requires(value, base, record.parameters, inherited,
                                     walk);

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

  const auto *parameters{value.try_at("parameters"sv, SWAGGER_HASH_PARAMETERS)};
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
    swagger_check_operation(*operation, location, record.parameters, walk);
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
