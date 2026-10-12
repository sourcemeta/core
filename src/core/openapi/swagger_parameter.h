#ifndef SOURCEMETA_CORE_OPENAPI_SWAGGER_PARAMETER_H_
#define SOURCEMETA_CORE_OPENAPI_SWAGGER_PARAMETER_H_

#include <sourcemeta/core/openapi.h>

#include "helpers.h"
#include "reference.h"
#include "swagger_items.h"

#include <array>       // std::array
#include <string_view> // std::string_view

namespace sourcemeta::core {

// Swagger 2.0, Parameter Object: the four fields every form of it carries,
// then the one the body form takes and the seventeen every other form does
constexpr auto SWAGGER_HASH_PARAMETERS{JSON::Object::hash("parameters"sv)};

constexpr std::array<JSON::StringView, 23> SWAGGER_PARAMETER_FIELDS{
    {"name"sv, "in"sv, "description"sv, "required"sv, "schema"sv, "type"sv,
     "format"sv, "allowEmptyValue"sv, "items"sv, "collectionFormat"sv,
     "default"sv, "maximum"sv, "exclusiveMaximum"sv, "minimum"sv,
     "exclusiveMinimum"sv, "maxLength"sv, "minLength"sv, "pattern"sv,
     "maxItems"sv, "minItems"sv, "uniqueItems"sv, "enum"sv, "multipleOf"sv}};

// Parameter Object: "in | string | **Required.** The location of the
// parameter. Possible values are "query", "header", "path", "formData" or
// "body""
constexpr std::initializer_list<JSON::StringView> SWAGGER_PARAMETER_LOCATIONS{
    "query"sv, "header"sv, "path"sv, "formData"sv, "body"sv};

// Of the five, the two that may repeat rather than separate, and the two that
// may be sent with a name and no value. Parameter Object, of the latter: "This
// is valid only for either `query` or `formData` parameters"
inline auto swagger_parameter_may_repeat(const JSON::StringView where) noexcept
    -> bool {
  return where == "query"sv || where == "formData"sv;
}

// Swagger 2.0, Parameter Object: "Describes a single operation parameter. A
// unique parameter is defined by a combination of a `name` and `location`".
//
// Which fields it carries is decided by where it sits rather than by the
// revision, which is what sets this Object apart from every later one. The
// body form nests a schema. Every other form describes what it holds on
// itself, the way an Items Object does, and so is not a Schema Object and must
// never be handed to one
inline auto swagger_check_parameter(const JSON &value, const Pointer &base,
                                    OpenAPIWalk &walk) -> void {
  openapi_record(walk, base, OpenAPIObjectKind::Parameter);
  openapi_expect_object(value, base, "The Parameter Object must be an object");

  openapi_reject_unknown_fields(
      value, SWAGGER_PARAMETER_FIELDS, base,
      "The Parameter Object does not define this field");

  // Parameter Object: "name | string | **Required.**"
  const auto &name{openapi_require(value, "name"sv, OPENAPI_HASH_NAME, base,
                                   "The Parameter Object must declare a name")};
  const auto parameter{openapi_expect_string(
      name, base, "name"sv, "The Parameter Object name must be a string")};

  // Parameter Object: "in | string | **Required.**"
  const auto &location{
      openapi_require(value, "in"sv, OPENAPI_HASH_IN, base,
                      "The Parameter Object must declare a location")};
  const auto where{openapi_expect_enumeration(
      location, base, "in"sv, SWAGGER_PARAMETER_LOCATIONS,
      "The Parameter Object location must be a string",
      "The Parameter Object location must name a place a parameter may sit")};

  openapi_check_optional_string(
      value, base, "description"sv, OPENAPI_HASH_DESCRIPTION,
      "The Parameter Object description must be a string");

  // Parameter Object: "required | boolean | Determines whether this parameter
  // is mandatory. If the parameter is `in` "path", this property is
  // **required** and its value MUST be `true`"
  const auto *required{value.try_at("required"sv, SWAGGER_HASH_REQUIRED)};
  if (required != nullptr) {
    openapi_expect_boolean(*required, base, "required"sv,
                           "The Parameter Object requiredness must be a "
                           "boolean");
  }

  if (where == "path"sv &&
      (required == nullptr || !required->to_boolean())) {
    throw OpenAPIError{base,
                       "A Parameter Object of a path must be declared required"};
  }

  // Parameter Object, of the field that lets a name be sent without a value:
  // "This is valid only for either `query` or `formData` parameters"
  const auto *allow_empty{
      value.try_at("allowEmptyValue"sv, SWAGGER_HASH_ALLOW_EMPTY_VALUE)};
  if (allow_empty != nullptr) {
    if (!swagger_parameter_may_repeat(where)) {
      throw OpenAPIError{
          openapi_child(base, "allowEmptyValue"sv),
          "Only a Parameter Object of a query or a form may allow an empty "
          "value"};
    }

    openapi_expect_boolean(*allow_empty, base, "allowEmptyValue"sv,
                           "The Parameter Object empty value allowance must be "
                           "a boolean");
  }

  // Parameter Object: "If `in` is "body": schema | Schema Object |
  // **Required.** The schema defining the type used for the body parameter"
  if (where == "body"sv) {
    const auto &schema{openapi_require(
        value, "schema"sv, SWAGGER_HASH_SCHEMA, base,
        "A Parameter Object of a body must declare a schema")};

    for (const auto &field : SWAGGER_ITEMS_FIELDS) {
      if (value.defines(JSON::String{field})) {
        throw OpenAPIError{
            openapi_child(base, field),
            "A Parameter Object of a body describes what it holds through its "
            "schema alone"};
      }
    }

    openapi_expect_schema(schema, openapi_child(base, "schema"sv), walk);
    walk.parameters.insert_or_assign(
        openapi_location_uri(walk.base, base),
        std::pair{JSON::String{parameter}, JSON::String{where}});
    return;
  }

  // "If `in` is any value other than "body": type | string | **Required.**"
  if (value.defines("schema"sv, SWAGGER_HASH_SCHEMA)) {
    throw OpenAPIError{
        openapi_child(base, "schema"sv),
        "Only a Parameter Object of a body describes what it holds through a "
        "schema"};
  }

  openapi_require(value, "type"sv, SWAGGER_HASH_TYPE, base,
                  "The Parameter Object must declare a type");

  // Only a form may name the file type, the same paragraph binding what the
  // operation then has to consume, and only a query or a form may repeat
  swagger_check_typed_value(
      value, base,
      where == "formData"sv ? SWAGGER_FORM_TYPES : SWAGGER_SIMPLE_TYPES,
      swagger_parameter_may_repeat(where)
          ? SWAGGER_COLLECTION_FORMATS_WITH_MULTI
          : SWAGGER_COLLECTION_FORMATS,
      "The Parameter Object type must be a string",
      "The Parameter Object type must name a type this location admits", walk);

  walk.parameters.insert_or_assign(
      openapi_location_uri(walk.base, base),
      std::pair{JSON::String{parameter}, JSON::String{where}});
}

inline auto swagger_check_parameter_or_reference(const JSON &value,
                                                 const Pointer &base,
                                                 OpenAPIWalk &walk) -> void {
  openapi_check_or_reference<OpenAPIObjectKind::Parameter,
                             swagger_check_parameter>(value, base, walk);
}

// Swagger 2.0, Parameters Definitions Object: "An object to hold parameters to
// be reused across operations. Parameter definitions can be referenced to the
// ones defined here. This does *not* define global operation parameters"
inline auto swagger_check_parameters_definitions(const JSON &document,
                                                 OpenAPIWalk &walk) -> void {
  const auto *definitions{
      document.try_at("parameters"sv, SWAGGER_HASH_PARAMETERS)};
  if (definitions == nullptr) {
    return;
  }

  const Pointer base{"parameters"};
  openapi_record(walk, base, OpenAPIObjectKind::ParametersDefinitions);
  openapi_expect_object(
      *definitions, base,
      "The Parameters Definitions Object must be an object");

  for (const auto &entry : definitions->as_object()) {
    swagger_check_parameter(entry.second, openapi_child(base, entry.first),
                            walk);
  }
}

} // namespace sourcemeta::core

#endif
