#ifndef SOURCEMETA_CORE_OPENAPI_SWAGGER_SECURITY_H_
#define SOURCEMETA_CORE_OPENAPI_SWAGGER_SECURITY_H_

#include <sourcemeta/core/openapi.h>

#include "helpers.h"

#include <array>       // std::array
#include <string_view> // std::string_view

namespace sourcemeta::core {

constexpr auto SWAGGER_HASH_SECURITY_DEFINITIONS{
    JSON::Object::hash("securityDefinitions"sv)};
constexpr auto SWAGGER_HASH_FLOW{JSON::Object::hash("flow"sv)};
constexpr auto SWAGGER_HASH_SCOPES{JSON::Object::hash("scopes"sv)};
constexpr auto SWAGGER_HASH_AUTHORIZATION_URL{
    JSON::Object::hash("authorizationUrl"sv)};
constexpr auto SWAGGER_HASH_TOKEN_URL{JSON::Object::hash("tokenUrl"sv)};

// Swagger 2.0, Security Scheme Object. Every field sits on the Object itself,
// with a Validity column saying which scheme types give it meaning, where the
// next revision gathers the flows into an Object of their own
constexpr std::array<JSON::StringView, 8> SWAGGER_SECURITY_SCHEME_FIELDS{
    {"type"sv, "description"sv, "name"sv, "in"sv, "flow"sv,
     "authorizationUrl"sv, "tokenUrl"sv, "scopes"sv}};

// Security Scheme Object: "type | string | Any | **Required.** The type of the
// security scheme. Valid values are `"basic"`, `"apiKey"` or `"oauth2"`"
constexpr std::initializer_list<JSON::StringView> SWAGGER_SCHEME_TYPES{
    "basic"sv, "apiKey"sv, "oauth2"sv};

// "in | string | `apiKey` | **Required** The location of the API key. Valid
// values are `"query"` or `"header"`"
constexpr std::initializer_list<JSON::StringView> SWAGGER_KEY_LOCATIONS{
    "query"sv, "header"sv};

// "flow | string | `oauth2` | **Required.** The flow used by the OAuth2
// security scheme. Valid values are `"implicit"`, `"password"`,
// `"application"` or `"accessCode"`"
constexpr std::initializer_list<JSON::StringView> SWAGGER_SCHEME_FLOWS{
    "implicit"sv, "password"sv, "application"sv, "accessCode"sv};

// Swagger 2.0, Scopes Object: "Lists the available scopes for an OAuth2
// security scheme". Every member names a scope against its description, and
// unlike the maps beside it this one does carve out an extension
inline auto swagger_check_scopes(const JSON &value, const Pointer &base,
                                 OpenAPIWalk &walk) -> void {
  openapi_record(walk, base, OpenAPIObjectKind::Scopes);
  openapi_expect_object(value, base, "The Scopes Object must be an object");

  for (const auto &entry : value.as_object()) {
    if (entry.first.starts_with(OPENAPI_EXTENSION_PREFIX)) {
      continue;
    }

    if (!entry.second.is_string()) {
      throw OpenAPIError{openapi_child(base, entry.first),
                         "The Scopes Object descriptions must be strings"};
    }
  }
}

// Swagger 2.0, Security Scheme Object: "Allows the definition of a security
// scheme that can be used by the operations. Supported schemes are basic
// authentication, an API key (either as a header or as a query parameter) and
// OAuth2's common flows"
inline auto swagger_check_security_scheme(const JSON &value,
                                          const Pointer &base,
                                          OpenAPIWalk &walk) -> void {
  openapi_record(walk, base, OpenAPIObjectKind::SecurityScheme);
  openapi_expect_object(value, base,
                        "The Security Scheme Object must be an object");

  openapi_reject_unknown_fields(
      value, SWAGGER_SECURITY_SCHEME_FIELDS, base,
      "The Security Scheme Object does not define this field");

  const auto &declared{
      openapi_require(value, "type"sv, SWAGGER_HASH_TYPE, base,
                      "The Security Scheme Object must declare a type")};
  const auto type{openapi_expect_enumeration(
      declared, base, "type"sv, SWAGGER_SCHEME_TYPES,
      "The Security Scheme Object type must be a string",
      "The Security Scheme Object type must name a scheme this revision "
      "supports")};

  openapi_check_optional_string(
      value, base, "description"sv, OPENAPI_HASH_DESCRIPTION,
      "The Security Scheme Object description must be a string");

  // The Validity column of each field below says which type gives it meaning.
  // A field written against a type it says nothing about is a field the Object
  // does not define there, which is why each is asked for where it applies and
  // turned down where it does not
  if (type == "apiKey"sv) {
    const auto &name{openapi_require(
        value, "name"sv, OPENAPI_HASH_NAME, base,
        "A Security Scheme Object of an API key must declare a name")};
    openapi_expect_string(name, base, "name"sv,
                          "The Security Scheme Object name must be a string");

    const auto &location{openapi_require(
        value, "in"sv, OPENAPI_HASH_IN, base,
        "A Security Scheme Object of an API key must declare a location")};
    openapi_expect_enumeration(
        location, base, "in"sv, SWAGGER_KEY_LOCATIONS,
        "The Security Scheme Object location must be a string",
        "The Security Scheme Object location must name a place an API key may "
        "sit");
  } else {
    for (const auto &field : {"name"sv, "in"sv}) {
      if (value.defines(JSON::String{field})) {
        throw OpenAPIError{
            openapi_child(base, field),
            "Only a Security Scheme Object of an API key names where the key "
            "sits"};
      }
    }
  }

  if (type != "oauth2"sv) {
    for (const auto &field :
         {"flow"sv, "authorizationUrl"sv, "tokenUrl"sv, "scopes"sv}) {
      if (value.defines(JSON::String{field})) {
        throw OpenAPIError{
            openapi_child(base, field),
            "Only a Security Scheme Object of OAuth2 describes a flow"};
      }
    }

    return;
  }

  const auto &declared_flow{openapi_require(
      value, "flow"sv, SWAGGER_HASH_FLOW, base,
      "A Security Scheme Object of OAuth2 must declare a flow")};
  const auto flow{openapi_expect_enumeration(
      declared_flow, base, "flow"sv, SWAGGER_SCHEME_FLOWS,
      "The Security Scheme Object flow must be a string",
      "The Security Scheme Object flow must name a flow this revision "
      "supports")};

  // "authorizationUrl | string | `oauth2` (`"implicit"`, `"accessCode"`) |
  // **Required.**", and "tokenUrl | string | `oauth2` (`"password"`,
  // `"application"`, `"accessCode"`) | **Required.**". So which of the two a
  // scheme must declare is settled by the flow rather than by the type
  const auto wants_authorization{flow == "implicit"sv ||
                                 flow == "accessCode"sv};
  const auto wants_token{flow == "password"sv || flow == "application"sv ||
                         flow == "accessCode"sv};

  const auto *authorization{
      value.try_at("authorizationUrl"sv, SWAGGER_HASH_AUTHORIZATION_URL)};
  if (wants_authorization) {
    if (authorization == nullptr) {
      throw OpenAPIError{base,
                         "This OAuth2 flow must declare an authorization URL"};
    }

    openapi_expect_string(*authorization, base, "authorizationUrl"sv,
                          "The Security Scheme Object authorization URL must "
                          "be a string");
  } else if (authorization != nullptr) {
    throw OpenAPIError{openapi_child(base, "authorizationUrl"sv),
                       "This OAuth2 flow does not declare an authorization "
                       "URL"};
  }

  const auto *token{value.try_at("tokenUrl"sv, SWAGGER_HASH_TOKEN_URL)};
  if (wants_token) {
    if (token == nullptr) {
      throw OpenAPIError{base, "This OAuth2 flow must declare a token URL"};
    }

    openapi_expect_string(*token, base, "tokenUrl"sv,
                          "The Security Scheme Object token URL must be a "
                          "string");
  } else if (token != nullptr) {
    throw OpenAPIError{openapi_child(base, "tokenUrl"sv),
                       "This OAuth2 flow does not declare a token URL"};
  }

  // "scopes | Scopes Object | `oauth2` | **Required.** The available scopes
  // for the OAuth2 security scheme"
  const auto &scopes{openapi_require(
      value, "scopes"sv, SWAGGER_HASH_SCOPES, base,
      "A Security Scheme Object of OAuth2 must declare its scopes")};
  swagger_check_scopes(scopes, openapi_child(base, "scopes"sv), walk);
}

// Every name the entry document declares a security scheme under, which is
// what a Security Requirement Object anywhere in the description may draw on.
// This revision holds them in a container of the root rather than in a
// Components Object, and it has no Reference Object in that position, so the
// type of each is read where it sits.
//
// This runs before the container itself is checked, so it takes what is there
// and leaves being strict about the shape to that check
inline auto swagger_collect_security_schemes(const JSON &document,
                                             OpenAPIWalk &walk) -> void {
  const auto *schemes{document.try_at("securityDefinitions"sv,
                                      SWAGGER_HASH_SECURITY_DEFINITIONS)};
  if (schemes == nullptr || !schemes->is_object()) {
    return;
  }

  for (const auto &entry : schemes->as_object()) {
    walk.security_schemes.insert(entry.first, entry.hash);

    if (!entry.second.is_object()) {
      continue;
    }

    // Only an OAuth2 scheme of this revision offers scopes, its Security
    // Requirement Object reading "If the security scheme is of type
    // `"oauth2"`, then the value is a list of scope names required for the
    // execution. For other security scheme types, the array MUST be empty"
    const auto *type{entry.second.try_at("type"sv, SWAGGER_HASH_TYPE)};
    if (type != nullptr && type->is_string() && type->to_string() != "oauth2") {
      walk.security_schemes_without_scopes.insert(entry.first, entry.hash);
    }
  }
}

// Swagger 2.0, Security Definitions Object: "A declaration of the security
// schemes available to be executed in the specification [...] It does not
// enforce the security schemes on the operations"
inline auto swagger_check_security_definitions(const JSON &document,
                                               OpenAPIWalk &walk) -> void {
  const auto *definitions{document.try_at("securityDefinitions"sv,
                                          SWAGGER_HASH_SECURITY_DEFINITIONS)};
  if (definitions == nullptr) {
    return;
  }

  const Pointer base{"securityDefinitions"};
  openapi_record(walk, base, OpenAPIObjectKind::SecurityDefinitions);
  openapi_expect_object(*definitions, base,
                        "The Security Definitions Object must be an object");

  for (const auto &entry : definitions->as_object()) {
    swagger_check_security_scheme(entry.second,
                                  openapi_child(base, entry.first), walk);
  }
}

} // namespace sourcemeta::core

#endif
