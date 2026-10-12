#ifndef SOURCEMETA_CORE_OPENAPI_SWAGGER_ITEMS_H_
#define SOURCEMETA_CORE_OPENAPI_SWAGGER_ITEMS_H_

#include <sourcemeta/core/openapi.h>

#include "helpers.h"

#include <array>       // std::array
#include <string_view> // std::string_view

namespace sourcemeta::core {

constexpr auto SWAGGER_HASH_TYPE{JSON::Object::hash("type"sv)};
constexpr auto SWAGGER_HASH_FORMAT{JSON::Object::hash("format"sv)};
constexpr auto SWAGGER_HASH_ITEMS{JSON::Object::hash("items"sv)};
constexpr auto SWAGGER_HASH_COLLECTION_FORMAT{
    JSON::Object::hash("collectionFormat"sv)};
constexpr auto SWAGGER_HASH_DEFAULT{JSON::Object::hash("default"sv)};
constexpr auto SWAGGER_HASH_ENUM{JSON::Object::hash("enum"sv)};
constexpr auto SWAGGER_HASH_SCHEMA{JSON::Object::hash("schema"sv)};
constexpr auto SWAGGER_HASH_REQUIRED{JSON::Object::hash("required"sv)};
constexpr auto SWAGGER_HASH_ALLOW_EMPTY_VALUE{
    JSON::Object::hash("allowEmptyValue"sv)};

// Swagger 2.0, Items Object: the twelve fields it takes from the validation
// draft it cites, which every position that describes a value without nesting
// a schema carries alongside the rest
constexpr std::array<JSON::StringView, 12> SWAGGER_VALIDATION_FIELDS{
    {"maximum"sv, "exclusiveMaximum"sv, "minimum"sv, "exclusiveMinimum"sv,
     "maxLength"sv, "minLength"sv, "pattern"sv, "maxItems"sv, "minItems"sv,
     "uniqueItems"sv, "enum"sv, "multipleOf"sv}};

// The Items Object tables those twelve and five of its own
constexpr std::array<JSON::StringView, 17> SWAGGER_ITEMS_FIELDS{
    {"type"sv, "format"sv, "items"sv, "collectionFormat"sv, "default"sv,
     "maximum"sv, "exclusiveMaximum"sv, "minimum"sv, "exclusiveMinimum"sv,
     "maxLength"sv, "minLength"sv, "pattern"sv, "maxItems"sv, "minItems"sv,
     "uniqueItems"sv, "enum"sv, "multipleOf"sv}};

// Swagger 2.0, Items Object: "The value MUST be one of `"string"`, `"number"`,
// `"integer"`, `"boolean"`, or `"array"`. Files and models are not allowed"
constexpr std::initializer_list<JSON::StringView> SWAGGER_SIMPLE_TYPES{
    "string"sv, "number"sv, "integer"sv, "boolean"sv, "array"sv};

// The same five, and the one a form may add. Swagger 2.0, Parameter Object:
// "The value MUST be one of `"string"`, `"number"`, `"integer"`, `"boolean"`,
// `"array"` or `"file"`. If `type` is `"file"`, the `consumes` MUST be either
// `"multipart/form-data"`, `"application/x-www-form-urlencoded"` or both"
constexpr std::initializer_list<JSON::StringView> SWAGGER_FORM_TYPES{
    "string"sv, "number"sv, "integer"sv, "boolean"sv, "array"sv, "file"sv};

// Swagger 2.0, Items Object, of the four separators it names: comma, space,
// tab and pipe. Its Parameter Object adds a fifth for the positions that may
// repeat instead of separating, which is why this list is asked for rather
// than assumed
constexpr std::initializer_list<JSON::StringView> SWAGGER_COLLECTION_FORMATS{
    "csv"sv, "ssv"sv, "tsv"sv, "pipes"sv};

// Swagger 2.0, Parameter Object, of the fifth: "`multi` - corresponds to
// multiple parameter instances instead of multiple values for a single
// instance [...] This is valid only for parameters `in` "query" or "formData""
constexpr std::initializer_list<JSON::StringView>
    SWAGGER_COLLECTION_FORMATS_WITH_MULTI{"csv"sv, "ssv"sv, "tsv"sv, "pipes"sv,
                                          "multi"sv};

// The twelve validation fields hold what the draft they come from holds, and
// nothing here reads further into them than the type each one takes. What a
// value means is the business of whatever validates against it, and what a
// position does with it is settled by the type beside it
inline auto swagger_check_validation_fields(const JSON &value,
                                            const Pointer &base) -> void {
  for (const auto &field :
       {"maximum"sv, "minimum"sv, "multipleOf"sv, "maxLength"sv, "minLength"sv,
        "maxItems"sv, "minItems"sv}) {
    const auto *member{value.try_at(field)};
    if (member != nullptr && !member->is_number()) {
      throw OpenAPIError{openapi_child(base, field),
                         "This validation field must be a number"};
    }
  }

  for (const auto &field :
       {"exclusiveMaximum"sv, "exclusiveMinimum"sv, "uniqueItems"sv}) {
    const auto *member{value.try_at(field)};
    if (member != nullptr && !member->is_boolean()) {
      throw OpenAPIError{openapi_child(base, field),
                         "This validation field must be a boolean"};
    }
  }

  openapi_check_optional_string(value, base, "pattern"sv,
                                JSON::Object::hash("pattern"sv),
                                "This validation field must be a string");

  const auto *enumeration{value.try_at("enum"sv, SWAGGER_HASH_ENUM)};
  if (enumeration != nullptr) {
    openapi_expect_array(*enumeration, openapi_child(base, "enum"sv),
                         "This validation field must be an array");
  }
}

// What a position that names a type carries beside it, which is the type
// itself, the format extending it, and the array fields that only a type of
// array gives meaning to. The types and the collection formats admitted are
// handed in, as they differ by where the position sits
inline auto swagger_check_typed_value(
    const JSON &value, const Pointer &base,
    const std::initializer_list<JSON::StringView> types,
    const std::initializer_list<JSON::StringView> collection_formats,
    const char *type_message, const char *type_value_message,
    OpenAPIWalk &walk) -> void;

// Swagger 2.0, Items Object: "A limited subset of JSON-Schema's items object.
// It is used by parameter definitions that are not located in `"body"`".
//
// It is not a Schema Object and must never be read as one. Its own array field
// holds another Items Object rather than a schema, and of its seventeen fields
// the only one no Schema Object of this revision also names is the collection
// format. So nothing but where it sits tells the two apart
inline auto swagger_check_items(const JSON &value, const Pointer &base,
                                OpenAPIWalk &walk) -> void {
  openapi_record(walk, base, OpenAPIObjectKind::Items);
  openapi_expect_object(value, base, "The Items Object must be an object");

  openapi_reject_unknown_fields(value, SWAGGER_ITEMS_FIELDS, base,
                                "The Items Object does not define this field");

  // Items Object: "type | string | **Required.** The internal type of the
  // array"
  openapi_require(value, "type"sv, SWAGGER_HASH_TYPE, base,
                  "The Items Object must declare a type");

  swagger_check_typed_value(value, base, SWAGGER_SIMPLE_TYPES,
                            SWAGGER_COLLECTION_FORMATS,
                            "The Items Object type must be a string",
                            "The Items Object type must name a simple type",
                            walk);
}

inline auto swagger_check_typed_value(
    const JSON &value, const Pointer &base,
    const std::initializer_list<JSON::StringView> types,
    const std::initializer_list<JSON::StringView> collection_formats,
    const char *type_message, const char *type_value_message,
    OpenAPIWalk &walk) -> void {
  const auto *type{value.try_at("type"sv, SWAGGER_HASH_TYPE)};
  JSON::StringView named;
  if (type != nullptr) {
    named = openapi_expect_enumeration(*type, base, "type"sv, types,
                                       type_message, type_value_message);
  }

  openapi_check_optional_string(value, base, "format"sv, SWAGGER_HASH_FORMAT,
                                "The format must be a string");

  const auto *collection_format{
      value.try_at("collectionFormat"sv, SWAGGER_HASH_COLLECTION_FORMAT)};
  if (collection_format != nullptr) {
    openapi_expect_enumeration(
        *collection_format, base, "collectionFormat"sv, collection_formats,
        "The collection format must be a string",
        "The collection format must name a format this position admits");
  }

  swagger_check_validation_fields(value, base);

  // Items Object: "items | Items Object | **Required if `type` is "array".**
  // Describes the type of items in the array". So the array field is asked for
  // where the type is an array, and means nothing anywhere else
  const auto *items{value.try_at("items"sv, SWAGGER_HASH_ITEMS)};
  if (named == "array"sv) {
    if (items == nullptr) {
      throw OpenAPIError{
          base, "This position must declare items when its type is an array"};
    }
  }

  if (items != nullptr) {
    swagger_check_items(*items, openapi_child(base, "items"sv), walk);
  }
}

} // namespace sourcemeta::core

#endif
