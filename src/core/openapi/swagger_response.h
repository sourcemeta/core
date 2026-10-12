#ifndef SOURCEMETA_CORE_OPENAPI_SWAGGER_RESPONSE_H_
#define SOURCEMETA_CORE_OPENAPI_SWAGGER_RESPONSE_H_

#include <sourcemeta/core/openapi.h>

#include <sourcemeta/core/http.h>

#include "helpers.h"
#include "reference.h"
#include "swagger_items.h"

#include <array>       // std::array
#include <string_view> // std::string_view

namespace sourcemeta::core {

constexpr auto SWAGGER_HASH_RESPONSES{JSON::Object::hash("responses"sv)};
constexpr auto SWAGGER_HASH_HEADERS{JSON::Object::hash("headers"sv)};
constexpr auto SWAGGER_HASH_EXAMPLES{JSON::Object::hash("examples"sv)};

// Swagger 2.0, Header Object: a description beside the seventeen an Items
// Object tables
constexpr std::array<JSON::StringView, 18> SWAGGER_HEADER_FIELDS{
    {"description"sv, "type"sv, "format"sv, "items"sv, "collectionFormat"sv,
     "default"sv, "maximum"sv, "exclusiveMaximum"sv, "minimum"sv,
     "exclusiveMinimum"sv, "maxLength"sv, "minLength"sv, "pattern"sv,
     "maxItems"sv, "minItems"sv, "uniqueItems"sv, "enum"sv, "multipleOf"sv}};

// Swagger 2.0, Response Object
constexpr std::array<JSON::StringView, 4> SWAGGER_RESPONSE_FIELDS{
    {"description"sv, "schema"sv, "headers"sv, "examples"sv}};

// Swagger 2.0, Header Object. It describes what a header holds the way an
// Items Object does rather than by nesting a schema, so it is read the same
// way and is no more a Schema Object than one
inline auto swagger_check_header(const JSON &value, const Pointer &base,
                                 OpenAPIWalk &walk) -> void {
  openapi_record(walk, base, OpenAPIObjectKind::Header);
  openapi_expect_object(value, base, "The Header Object must be an object");

  openapi_reject_unknown_fields(value, SWAGGER_HEADER_FIELDS, base,
                                "The Header Object does not define this field");

  openapi_check_optional_string(value, base, "description"sv,
                                OPENAPI_HASH_DESCRIPTION,
                                "The Header Object description must be a "
                                "string");

  // Header Object: "type | string | **Required.** The type of the object"
  openapi_require(value, "type"sv, SWAGGER_HASH_TYPE, base,
                  "The Header Object must declare a type");

  // A header is sent once with its value, so it separates rather than repeats,
  // and the format that stands for repeating is not one it admits
  swagger_check_typed_value(value, base, SWAGGER_SIMPLE_TYPES,
                            SWAGGER_COLLECTION_FORMATS,
                            "The Header Object type must be a string",
                            "The Header Object type must name a simple type",
                            walk);
}

// Swagger 2.0, Headers Object: "Lists the headers that can be sent as part of
// a response". Every member is a header name, there being no carve-out for an
// extension in what it tables
inline auto swagger_check_headers(const JSON &value, const Pointer &base,
                                  OpenAPIWalk &walk) -> void {
  openapi_record(walk, base, OpenAPIObjectKind::Headers);
  openapi_expect_object(value, base, "The Headers Object must be an object");

  for (const auto &entry : value.as_object()) {
    swagger_check_header(entry.second, openapi_child(base, entry.first), walk);
  }
}

// Swagger 2.0, Example Object: "Allows sharing examples for operation
// responses". Its members are media types and its values are free form, so
// nothing is read inside one. It shares its name with the Object of the later
// revisions and nothing else
inline auto swagger_check_example(const JSON &value, const Pointer &base,
                                  OpenAPIWalk &walk) -> void {
  openapi_record(walk, base, OpenAPIObjectKind::Example);
  openapi_expect_object(value, base, "The Example Object must be an object");

  // Example Object: "The value of the property MUST be of the type the
  // operation describes", which is a rule about the value rather than about
  // its shape, and nothing here reads far enough into one to hold it
}

// Swagger 2.0, Response Object: "Describes a single response from an API
// Operation"
inline auto swagger_check_response(const JSON &value, const Pointer &base,
                                   OpenAPIWalk &walk) -> void {
  openapi_record(walk, base, OpenAPIObjectKind::Response);
  openapi_expect_object(value, base, "The Response Object must be an object");

  openapi_reject_unknown_fields(
      value, SWAGGER_RESPONSE_FIELDS, base,
      "The Response Object does not define this field");

  // Response Object: "description | string | **Required.** A short description
  // of the response"
  const auto &description{openapi_require(
      value, "description"sv, OPENAPI_HASH_DESCRIPTION, base,
      "The Response Object must declare a description")};
  openapi_expect_string(description, base, "description"sv,
                        "The Response Object description must be a string");

  // Response Object: "schema | Schema Object | A definition of the response
  // structure". A response body is described by one schema rather than by a
  // map keyed on media type, which is what the next revision brings
  const auto *schema{value.try_at("schema"sv, SWAGGER_HASH_SCHEMA)};
  if (schema != nullptr) {
    openapi_expect_schema(*schema, openapi_child(base, "schema"sv), walk);
  }

  const auto *headers{value.try_at("headers"sv, SWAGGER_HASH_HEADERS)};
  if (headers != nullptr) {
    swagger_check_headers(*headers, openapi_child(base, "headers"sv), walk);
  }

  const auto *examples{value.try_at("examples"sv, SWAGGER_HASH_EXAMPLES)};
  if (examples != nullptr) {
    swagger_check_example(*examples, openapi_child(base, "examples"sv), walk);
  }
}

inline auto swagger_check_response_or_reference(const JSON &value,
                                                const Pointer &base,
                                                OpenAPIWalk &walk) -> void {
  openapi_check_or_reference<OpenAPIObjectKind::Response,
                             swagger_check_response>(value, base, walk);
}

// Swagger 2.0, Responses Object: "A container for the expected responses of an
// operation [...] It is not expected from the documentation to necessarily
// cover all possible HTTP response codes"
inline auto swagger_check_responses(const JSON &value, const Pointer &base,
                                    OpenAPIWalk &walk) -> void {
  openapi_record(walk, base, OpenAPIObjectKind::Responses);
  openapi_expect_object(value, base, "The Responses Object must be an object");

  // Responses Object: "The Responses Object MUST contain at least one response
  // code". Which keys count as one goes unsaid here, exactly as it does in the
  // revisions after it, and the same two answers settle it: the specification's
  // own schema holds this Object to carrying a member that is not an
  // extension, by refusing one whose every member is, and nothing there asks
  // for a status code in particular. So a default answers for a response, and
  // only an Object answering for none at all is turned down, which is the
  // reading the revisions after it already go by
  if (value.empty()) {
    throw OpenAPIError{base, "The Responses Object must not be empty"};
  }

  bool answers_for_a_response{false};
  for (const auto &entry : value.as_object()) {
    if (entry.first.starts_with(OPENAPI_EXTENSION_PREFIX)) {
      continue;
    }

    answers_for_a_response = true;
    const auto location{openapi_child(base, entry.first)};

    // Responses Object: "default | Response Object | Reference Object | The
    // documentation of responses other than the ones declared for specific
    // HTTP response codes", and beside it "{HTTP Status Code} | Response
    // Object | Reference Object".
    //
    // Of what a status code is, this revision says "The available status codes
    // are described by RFC 7231 [...] and in the IANA Status Code Registry",
    // and nothing more. The revisions after it add a range form of their own,
    // writing a class as a digit and two literal characters, so that form is
    // one this revision has no room for and is not admitted here
    if (entry.first != "default"sv && !http_is_status_code(entry.first)) {
      throw OpenAPIError{location,
                         "The Responses Object keys must name a status code "
                         "or the default response"};
    }

    swagger_check_response_or_reference(entry.second, location, walk);
  }

  if (!answers_for_a_response) {
    throw OpenAPIError{
        base, "The Responses Object must answer for at least one response"};
  }
}

// Swagger 2.0, Responses Definitions Object: "An object to hold responses to
// be reused across operations"
inline auto swagger_check_responses_definitions(const JSON &document,
                                                OpenAPIWalk &walk) -> void {
  const auto *definitions{
      document.try_at("responses"sv, SWAGGER_HASH_RESPONSES)};
  if (definitions == nullptr) {
    return;
  }

  const Pointer base{"responses"};
  openapi_record(walk, base, OpenAPIObjectKind::ResponsesDefinitions);
  openapi_expect_object(*definitions, base,
                        "The Responses Definitions Object must be an object");

  for (const auto &entry : definitions->as_object()) {
    swagger_check_response(entry.second, openapi_child(base, entry.first),
                           walk);
  }
}

} // namespace sourcemeta::core

#endif
