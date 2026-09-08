#ifndef SOURCEMETA_CORE_OPENAPI_CONTENT_H_
#define SOURCEMETA_CORE_OPENAPI_CONTENT_H_

#include <sourcemeta/core/openapi.h>

#include "example.h"
#include "helpers.h"
#include "reference.h"

#include <array>       // std::array
#include <string_view> // std::string_view

namespace sourcemeta::core {

constexpr auto OPENAPI_HASH_SCHEMA{JSON::Object::hash("schema"sv)};
constexpr auto OPENAPI_HASH_CONTENT{JSON::Object::hash("content"sv)};
constexpr auto OPENAPI_HASH_ENCODING{JSON::Object::hash("encoding"sv)};
constexpr auto OPENAPI_HASH_CONTENT_TYPE{JSON::Object::hash("contentType"sv)};
constexpr auto OPENAPI_HASH_HEADERS{JSON::Object::hash("headers"sv)};
constexpr auto OPENAPI_HASH_STYLE{JSON::Object::hash("style"sv)};
constexpr auto OPENAPI_HASH_EXPLODE{JSON::Object::hash("explode"sv)};
constexpr auto OPENAPI_HASH_ALLOW_RESERVED{
    JSON::Object::hash("allowReserved"sv)};
constexpr auto OPENAPI_HASH_REQUIRED{JSON::Object::hash("required"sv)};
constexpr auto OPENAPI_HASH_DEPRECATED{JSON::Object::hash("deprecated"sv)};

constexpr std::array<JSON::StringView, 5> OPENAPI_ENCODING_FIELDS{
    {"contentType"sv, "headers"sv, "style"sv, "explode"sv, "allowReserved"sv}};

constexpr std::array<JSON::StringView, 4> OPENAPI_MEDIA_TYPE_FIELDS{
    {"schema"sv, "example"sv, "examples"sv, "encoding"sv}};

// A Header Object only admits the serialisation fields alongside a `schema`,
// which is how the meta-schema reads it, holding them behind a dependent
// schema that a `content` form never reaches
constexpr std::array<JSON::StringView, 3> OPENAPI_HEADER_FIELDS{
    {"description"sv, "required"sv, "deprecated"sv}};

constexpr std::array<JSON::StringView, 8> OPENAPI_HEADER_SCHEMA_FIELDS{
    {"description"sv, "required"sv, "deprecated"sv, "schema"sv, "style"sv,
     "explode"sv, "example"sv, "examples"sv}};

constexpr std::array<JSON::StringView, 4> OPENAPI_HEADER_CONTENT_FIELDS{
    {"description"sv, "required"sv, "deprecated"sv, "content"sv}};

inline auto openapi_check_header_or_reference(const JSON &value,
                                              const Pointer &base,
                                              OpenAPIWalk &walk) -> void;

// OpenAPI Specification 3.1.1, Section 4.8.15: "A single encoding definition
// applied to a single schema property"
inline auto openapi_check_encoding(const JSON &value, const Pointer &base,
                                   OpenAPIWalk &walk) -> void {
  openapi_record(walk, base, OpenAPIObjectKind::Encoding);
  openapi_expect_object(value, base, "The Encoding Object must be an object");
  openapi_reject_unknown_fields(
      value, OPENAPI_ENCODING_FIELDS, base,
      "The Encoding Object does not define this field");

  // The specification says media type definitions "SHOULD be in compliance
  // with RFC6838", which is not a requirement, so only the type is checked
  const auto *content_type{
      value.try_at("contentType", OPENAPI_HASH_CONTENT_TYPE)};
  if (content_type != nullptr) {
    openapi_expect_string(*content_type, base, "contentType"sv,
                          "The Encoding Object content type must be a string");
  }

  const auto *headers{value.try_at("headers", OPENAPI_HASH_HEADERS)};
  if (headers != nullptr) {
    const auto location{openapi_child(base, "headers"sv)};
    openapi_expect_object(*headers, location,
                          "The Encoding Object headers must be an object");
    for (const auto &entry : headers->as_object()) {
      openapi_check_header_or_reference(
          entry.second, openapi_child(location, entry.first), walk);
    }
  }

  const auto *style{value.try_at("style", OPENAPI_HASH_STYLE)};
  if (style != nullptr) {
    openapi_expect_enumeration(
        *style, base, "style"sv,
        {"form"sv, "spaceDelimited"sv, "pipeDelimited"sv, "deepObject"sv},
        "The Encoding Object style must be a string",
        "The Encoding Object style is not one this specification defines");
  }

  const auto *explode{value.try_at("explode", OPENAPI_HASH_EXPLODE)};
  if (explode != nullptr) {
    openapi_expect_boolean(*explode, base, "explode"sv,
                           "The Encoding Object explode must be a boolean");
  }

  const auto *allow_reserved{
      value.try_at("allowReserved", OPENAPI_HASH_ALLOW_RESERVED)};
  if (allow_reserved != nullptr) {
    openapi_expect_boolean(
        *allow_reserved, base, "allowReserved"sv,
        "The Encoding Object allowReserved must be a boolean");
  }
}

// OpenAPI Specification 3.1.1, Section 4.8.14: "Each Media Type Object
// provides schema and examples for the media type identified by its key"
inline auto openapi_check_media_type(const JSON &value, const Pointer &base,
                                     OpenAPIWalk &walk) -> void {
  openapi_record(walk, base, OpenAPIObjectKind::MediaType);
  openapi_expect_object(value, base, "The Media Type Object must be an object");
  openapi_reject_unknown_fields(
      value, OPENAPI_MEDIA_TYPE_FIELDS, base,
      "The Media Type Object does not define this field");

  const auto *schema{value.try_at("schema", OPENAPI_HASH_SCHEMA)};
  if (schema != nullptr) {
    openapi_expect_schema(*schema, openapi_child(base, "schema"sv),
                          "A Schema Object must be an object or a boolean",
                          walk);
  }

  openapi_check_examples(
      value, base,
      "The Media Type Object example and examples are mutually exclusive",
      "The Media Type Object examples must be an object", walk);

  const auto *encoding{value.try_at("encoding", OPENAPI_HASH_ENCODING)};
  if (encoding != nullptr) {
    const auto location{openapi_child(base, "encoding"sv)};
    openapi_expect_object(*encoding, location,
                          "The Media Type Object encoding must be an object");
    for (const auto &entry : encoding->as_object()) {
      openapi_check_encoding(entry.second, openapi_child(location, entry.first),
                             walk);
    }
  }
}

// The map that the Request Body, Response, Parameter and Header Objects all
// key by media type. Section 4.5 says those definitions "SHOULD be in
// compliance with RFC6838", so the keys carry no requirement to enforce
inline auto openapi_check_content(const JSON &value, const Pointer &location,
                                  const char *type_message, OpenAPIWalk &walk)
    -> void {
  openapi_expect_object(value, location, type_message);
  for (const auto &entry : value.as_object()) {
    openapi_check_media_type(entry.second, openapi_child(location, entry.first),
                             walk);
  }
}

// OpenAPI Specification 3.1.1, Section 4.8.21: "Describes a single header for
// HTTP responses and for individual parts in `multipart` representations"
inline auto openapi_check_header(const JSON &value, const Pointer &base,
                                 OpenAPIWalk &walk) -> void {
  openapi_record(walk, base, OpenAPIObjectKind::Header);
  openapi_expect_object(value, base, "The Header Object must be an object");

  const auto *schema{value.try_at("schema", OPENAPI_HASH_SCHEMA)};
  const auto *content{value.try_at("content", OPENAPI_HASH_CONTENT)};

  // OpenAPI Specification 3.1.1, Section 4.8.21: "The `schema` field and
  // `content` field are mutually exclusive", and one of them has to be there
  if (schema != nullptr && content != nullptr) {
    throw OpenAPIError{
        base, "The Header Object schema and content are mutually exclusive"};
  }

  if (schema == nullptr && content == nullptr) {
    throw OpenAPIError{base,
                       "The Header Object must declare a schema or a content"};
  }

  if (schema == nullptr) {
    openapi_reject_unknown_fields(
        value, OPENAPI_HEADER_CONTENT_FIELDS, base,
        "The Header Object does not define this field");
  } else {
    openapi_reject_unknown_fields(
        value, OPENAPI_HEADER_SCHEMA_FIELDS, base,
        "The Header Object does not define this field");
  }

  const auto *description{
      value.try_at("description", OPENAPI_HASH_DESCRIPTION)};
  if (description != nullptr) {
    openapi_expect_string(*description, base, "description"sv,
                          "The Header Object description must be a string");
  }

  const auto *required{value.try_at("required", OPENAPI_HASH_REQUIRED)};
  if (required != nullptr) {
    openapi_expect_boolean(*required, base, "required"sv,
                           "The Header Object required must be a boolean");
  }

  const auto *deprecated{value.try_at("deprecated", OPENAPI_HASH_DEPRECATED)};
  if (deprecated != nullptr) {
    openapi_expect_boolean(*deprecated, base, "deprecated"sv,
                           "The Header Object deprecated must be a boolean");
  }

  if (content != nullptr) {
    const auto location{openapi_child(base, "content"sv)};
    openapi_check_content(*content, location,
                          "The Header Object content must be an object", walk);
    // The meta-schema bounds this map at one entry in both directions
    if (content->size() != 1) {
      throw OpenAPIError{
          location, "The Header Object content must hold exactly one entry"};
    }

    return;
  }

  openapi_expect_schema(*schema, openapi_child(base, "schema"sv),
                        "A Schema Object must be an object or a boolean", walk);

  // OpenAPI Specification 3.1.1, Section 4.8.21 fixes the only style a Header
  // Object may name
  const auto *style{value.try_at("style", OPENAPI_HASH_STYLE)};
  if (style != nullptr) {
    openapi_expect_enumeration(*style, base, "style"sv, {"simple"sv},
                               "The Header Object style must be a string",
                               "The Header Object style must be simple");
  }

  const auto *explode{value.try_at("explode", OPENAPI_HASH_EXPLODE)};
  if (explode != nullptr) {
    openapi_expect_boolean(*explode, base, "explode"sv,
                           "The Header Object explode must be a boolean");
  }

  openapi_check_examples(
      value, base,
      "The Header Object example and examples are mutually exclusive",
      "The Header Object examples must be an object", walk);
}

inline auto openapi_check_header_or_reference(const JSON &value,
                                              const Pointer &base,
                                              OpenAPIWalk &walk) -> void {
  if (openapi_is_reference(value)) {
    openapi_check_reference(value, base, OpenAPIObjectKind::Header, walk);
    return;
  }

  openapi_check_header(value, base, walk);
}

} // namespace sourcemeta::core

#endif
