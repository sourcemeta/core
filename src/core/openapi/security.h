#ifndef SOURCEMETA_CORE_OPENAPI_SECURITY_H_
#define SOURCEMETA_CORE_OPENAPI_SECURITY_H_

#include <sourcemeta/core/openapi.h>

#include "helpers.h"
#include "reference.h"

#include <sourcemeta/core/text.h>

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <string_view> // std::string_view
#include <vector>      // std::vector

namespace sourcemeta::core {

constexpr auto OPENAPI_HASH_SECURITY{JSON::Object::hash("security"sv)};
constexpr auto OPENAPI_HASH_TYPE{JSON::Object::hash("type"sv)};
constexpr auto OPENAPI_HASH_SCHEME{JSON::Object::hash("scheme"sv)};
constexpr auto OPENAPI_HASH_BEARER_FORMAT{JSON::Object::hash("bearerFormat"sv)};
constexpr auto OPENAPI_HASH_FLOWS{JSON::Object::hash("flows"sv)};
constexpr auto OPENAPI_HASH_OPENID_CONNECT_URL{
    JSON::Object::hash("openIdConnectUrl"sv)};
constexpr auto OPENAPI_HASH_SCOPES{JSON::Object::hash("scopes"sv)};
constexpr auto OPENAPI_HASH_AUTHORIZATION_URL{
    JSON::Object::hash("authorizationUrl"sv)};
constexpr auto OPENAPI_HASH_TOKEN_URL{JSON::Object::hash("tokenUrl"sv)};
constexpr auto OPENAPI_HASH_REFRESH_URL{JSON::Object::hash("refreshUrl"sv)};

constexpr std::array<JSON::StringView, 4> OPENAPI_SECURITY_SCHEME_APIKEY_FIELDS{
    {"type"sv, "description"sv, "name"sv, "in"sv}};
// OpenAPI Specification 3.1.1, Section 4.8.27 scopes `bearerFormat` to `http
// ("bearer")` in its "Applies To" column, so an HTTP scheme that is not bearer
// does not define it. The published meta-schema draws the same line, admitting
// the field only under a `scheme` matching `^[Bb][Ee][Aa][Rr][Ee][Rr]$`
constexpr std::array<JSON::StringView, 3> OPENAPI_SECURITY_SCHEME_HTTP_FIELDS{
    {"type"sv, "description"sv, "scheme"sv}};
constexpr std::array<JSON::StringView, 4>
    OPENAPI_SECURITY_SCHEME_HTTP_BEARER_FIELDS{
        {"type"sv, "description"sv, "scheme"sv, "bearerFormat"sv}};
constexpr std::array<JSON::StringView, 3> OPENAPI_SECURITY_SCHEME_OAUTH2_FIELDS{
    {"type"sv, "description"sv, "flows"sv}};
constexpr std::array<JSON::StringView, 3> OPENAPI_SECURITY_SCHEME_OIDC_FIELDS{
    {"type"sv, "description"sv, "openIdConnectUrl"sv}};
constexpr std::array<JSON::StringView, 2> OPENAPI_SECURITY_SCHEME_COMMON_FIELDS{
    {"type"sv, "description"sv}};

constexpr std::array<JSON::StringView, 4> OPENAPI_OAUTH_FLOWS_FIELDS{
    {"implicit"sv, "password"sv, "clientCredentials"sv, "authorizationCode"sv}};

constexpr std::array<JSON::StringView, 4> OPENAPI_OAUTH_FLOW_FIELDS{
    {"authorizationUrl"sv, "tokenUrl"sv, "refreshUrl"sv, "scopes"sv}};

// OpenAPI Specification 3.1.1, Section 4.8.29: "Configuration details for a
// supported OAuth Flow". Which of the two URLs a flow must carry depends on
// which flow it is, and `scopes` is required by every one of them
inline auto openapi_check_oauth_flow(const JSON &value, const Pointer &base,
                                     const bool needs_authorization_url,
                                     const bool needs_token_url,
                                     OpenAPIWalk &walk) -> void {
  openapi_record(walk, base, OpenAPIObjectKind::OAuthFlow);
  openapi_expect_object(value, base, "The OAuth Flow Object must be an object");
  openapi_reject_unknown_fields(
      value, OPENAPI_OAUTH_FLOW_FIELDS, base,
      "The OAuth Flow Object does not define this field");

  // Section 4.8.29 scopes each URL to the flows it names in its "Applies To"
  // column, and it names exactly the flows that require it. So a flow does not
  // define the URL it does not require, which the published meta-schema draws
  // the same way, giving each flow its own property set
  if (!needs_authorization_url &&
      value.try_at("authorizationUrl", OPENAPI_HASH_AUTHORIZATION_URL) !=
          nullptr) {
    throw OpenAPIError{
        openapi_child(base, "authorizationUrl"sv),
        "This OAuth Flow Object does not define an authorization URI"};
  }

  if (!needs_token_url &&
      value.try_at("tokenUrl", OPENAPI_HASH_TOKEN_URL) != nullptr) {
    throw OpenAPIError{openapi_child(base, "tokenUrl"sv),
                       "This OAuth Flow Object does not define a token URI"};
  }

  const auto *authorization{
      value.try_at("authorizationUrl", OPENAPI_HASH_AUTHORIZATION_URL)};
  if (needs_authorization_url && authorization == nullptr) {
    throw OpenAPIError{
        base, "This OAuth Flow Object must declare an authorization URI"};
  }

  if (authorization != nullptr) {
    openapi_expect_uri_reference(
        *authorization, base, "authorizationUrl"sv,
        "The OAuth Flow Object authorization URI must be a string",
        "The OAuth Flow Object authorization URI must be a URI reference");
  }

  const auto *token{value.try_at("tokenUrl", OPENAPI_HASH_TOKEN_URL)};
  if (needs_token_url && token == nullptr) {
    throw OpenAPIError{base, "This OAuth Flow Object must declare a token URI"};
  }

  if (token != nullptr) {
    openapi_expect_uri_reference(
        *token, base, "tokenUrl"sv,
        "The OAuth Flow Object token URI must be a string",
        "The OAuth Flow Object token URI must be a URI reference");
  }

  const auto *refresh{value.try_at("refreshUrl", OPENAPI_HASH_REFRESH_URL)};
  if (refresh != nullptr) {
    openapi_expect_uri_reference(
        *refresh, base, "refreshUrl"sv,
        "The OAuth Flow Object refresh URI must be a string",
        "The OAuth Flow Object refresh URI must be a URI reference");
  }

  // OpenAPI Specification 3.1.1, Section 4.8.29: "scopes | Map[string, string]
  // | REQUIRED. The available scopes for the OAuth2 security scheme"
  const auto *scopes{value.try_at("scopes", OPENAPI_HASH_SCOPES)};
  if (scopes == nullptr) {
    throw OpenAPIError{base, "The OAuth Flow Object must declare its scopes"};
  }

  openapi_check_map_of_strings(
      *scopes, openapi_child(base, "scopes"sv),
      "The OAuth Flow Object scopes must be an object",
      "The OAuth Flow Object scopes must hold strings");
}

// OpenAPI Specification 3.1.1, Section 4.8.28: "Allows configuration of the
// supported OAuth Flows"
inline auto openapi_check_oauth_flows(const JSON &value, const Pointer &base,
                                      OpenAPIWalk &walk) -> void {
  openapi_record(walk, base, OpenAPIObjectKind::OAuthFlows);
  openapi_expect_object(value, base,
                        "The OAuth Flows Object must be an object");
  openapi_reject_unknown_fields(
      value, OPENAPI_OAUTH_FLOWS_FIELDS, base,
      "The OAuth Flows Object does not define this field");

  const auto *implicit{
      value.try_at("implicit", JSON::Object::hash("implicit"sv))};
  if (implicit != nullptr) {
    openapi_check_oauth_flow(*implicit, openapi_child(base, "implicit"sv), true,
                             false, walk);
  }

  const auto *password{
      value.try_at("password", JSON::Object::hash("password"sv))};
  if (password != nullptr) {
    openapi_check_oauth_flow(*password, openapi_child(base, "password"sv),
                             false, true, walk);
  }

  const auto *client_credentials{value.try_at(
      "clientCredentials", JSON::Object::hash("clientCredentials"sv))};
  if (client_credentials != nullptr) {
    openapi_check_oauth_flow(*client_credentials,
                             openapi_child(base, "clientCredentials"sv), false,
                             true, walk);
  }

  const auto *authorization_code{value.try_at(
      "authorizationCode", JSON::Object::hash("authorizationCode"sv))};
  if (authorization_code != nullptr) {
    openapi_check_oauth_flow(*authorization_code,
                             openapi_child(base, "authorizationCode"sv), true,
                             true, walk);
  }
}

// OpenAPI Specification 3.1.1, Section 4.8.27: "Defines a security scheme that
// can be used by the operations". Its field table carries an "Applies To"
// column, so which fields are required depends on the type it declares
inline auto openapi_check_security_scheme(const JSON &value,
                                          const Pointer &base,
                                          OpenAPIWalk &walk) -> void {
  openapi_record(walk, base, OpenAPIObjectKind::SecurityScheme);
  openapi_expect_object(value, base,
                        "The Security Scheme Object must be an object");

  // OpenAPI Specification 3.1.1, Section 4.8.27: "type | string | Any |
  // REQUIRED. The type of the security scheme"
  const auto *type{value.try_at("type", OPENAPI_HASH_TYPE)};
  if (type == nullptr) {
    throw OpenAPIError{base,
                       "The Security Scheme Object must declare its type"};
  }

  const auto scheme_type{openapi_expect_enumeration(
      *type, base, "type"sv,
      {"apiKey"sv, "http"sv, "mutualTLS"sv, "oauth2"sv, "openIdConnect"sv},
      "The Security Scheme Object type must be a string",
      "The Security Scheme Object type is not one this specification "
      "defines")};

  const auto *description{
      value.try_at("description", OPENAPI_HASH_DESCRIPTION)};
  if (description != nullptr) {
    openapi_expect_string(
        *description, base, "description"sv,
        "The Security Scheme Object description must be a string");
  }

  if (scheme_type == "apiKey"sv) {
    openapi_reject_unknown_fields(
        value, OPENAPI_SECURITY_SCHEME_APIKEY_FIELDS, base,
        "The Security Scheme Object does not define this field");

    const auto *name{value.try_at("name", OPENAPI_HASH_NAME)};
    if (name == nullptr) {
      throw OpenAPIError{
          base, "An apiKey Security Scheme Object must declare a name"};
    }

    openapi_expect_string(*name, base, "name"sv,
                          "The Security Scheme Object name must be a string");

    const auto *location{value.try_at("in", OPENAPI_HASH_IN)};
    if (location == nullptr) {
      throw OpenAPIError{
          base, "An apiKey Security Scheme Object must declare a location"};
    }

    openapi_expect_enumeration(
        *location, base, "in"sv, {"query"sv, "header"sv, "cookie"sv},
        "The Security Scheme Object location must be a string",
        "The Security Scheme Object location is not one an apiKey admits");
    return;
  }

  if (scheme_type == "http"sv) {
    // RFC 7235 Section 2.1 makes an authentication scheme name
    // case-insensitive, which is why the meta-schema spells bearer as a
    // pattern rather than a constant
    const auto *scheme{value.try_at("scheme", OPENAPI_HASH_SCHEME)};
    if (scheme != nullptr && scheme->is_string() &&
        equals_ignore_case(scheme->to_string(), "bearer"sv)) {
      openapi_reject_unknown_fields(
          value, OPENAPI_SECURITY_SCHEME_HTTP_BEARER_FIELDS, base,
          "The Security Scheme Object does not define this field");
    } else {
      openapi_reject_unknown_fields(
          value, OPENAPI_SECURITY_SCHEME_HTTP_FIELDS, base,
          "The Security Scheme Object does not define this field");
    }

    if (scheme == nullptr) {
      throw OpenAPIError{
          base, "An http Security Scheme Object must declare a scheme"};
    }

    openapi_expect_string(*scheme, base, "scheme"sv,
                          "The Security Scheme Object scheme must be a string");

    const auto *bearer{
        value.try_at("bearerFormat", OPENAPI_HASH_BEARER_FORMAT)};
    if (bearer != nullptr) {
      openapi_expect_string(
          *bearer, base, "bearerFormat"sv,
          "The Security Scheme Object bearer format must be a string");
    }

    return;
  }

  if (scheme_type == "oauth2"sv) {
    openapi_reject_unknown_fields(
        value, OPENAPI_SECURITY_SCHEME_OAUTH2_FIELDS, base,
        "The Security Scheme Object does not define this field");

    const auto *flows{value.try_at("flows", OPENAPI_HASH_FLOWS)};
    if (flows == nullptr) {
      throw OpenAPIError{
          base, "An oauth2 Security Scheme Object must declare its flows"};
    }

    openapi_check_oauth_flows(*flows, openapi_child(base, "flows"sv), walk);
    return;
  }

  if (scheme_type == "openIdConnect"sv) {
    openapi_reject_unknown_fields(
        value, OPENAPI_SECURITY_SCHEME_OIDC_FIELDS, base,
        "The Security Scheme Object does not define this field");

    const auto *url{
        value.try_at("openIdConnectUrl", OPENAPI_HASH_OPENID_CONNECT_URL)};
    if (url == nullptr) {
      throw OpenAPIError{base, "An openIdConnect Security Scheme Object must "
                               "declare its discovery URI"};
    }

    openapi_expect_uri_reference(
        *url, base, "openIdConnectUrl"sv,
        "The Security Scheme Object discovery URI must be a string",
        "The Security Scheme Object discovery URI must be a URI reference");
    return;
  }

  // A mutualTLS scheme carries nothing beyond the two fields every type has
  openapi_reject_unknown_fields(
      value, OPENAPI_SECURITY_SCHEME_COMMON_FIELDS, base,
      "The Security Scheme Object does not define this field");
}

inline auto openapi_check_security_scheme_or_reference(const JSON &value,
                                                       const Pointer &base,
                                                       OpenAPIWalk &walk)
    -> void {
  if (openapi_is_reference(value)) {
    openapi_check_reference(value, base, OpenAPIObjectKind::SecurityScheme,
                            walk);
    return;
  }

  openapi_check_security_scheme(value, base, walk);
}

inline auto openapi_check_security_requirement(const JSON &value,
                                               const Pointer &base,
                                               OpenAPIWalk &walk) -> void {
  openapi_record(walk, base, OpenAPIObjectKind::SecurityRequirement);
  openapi_expect_object(value, base,
                        "The Security Requirement Object must be an object");
  for (const auto &entry : value.as_object()) {
    // Section 4.8.30: "Each name MUST correspond to a security scheme which is
    // declared in the Security Schemes under the Components Object". This
    // Object declares no pattern but its names, so a member called `x-` is a
    // scheme name and is held to the same requirement
    if (!walk.security_schemes.contains(entry.first)) {
      throw OpenAPIError{
          openapi_child(base, entry.first),
          "The Security Requirement Object must name a declared security "
          "scheme"};
    }

    openapi_check_array_of_strings(
        entry.second, openapi_child(base, entry.first),
        "The Security Requirement Object entries must be arrays",
        "The Security Requirement Object entries must hold strings");
  }
}

// What comes back is where each Security Requirement Object was recorded, so
// that an operation can name the requirements in force without repeating them
inline auto openapi_check_security(const JSON &value, const Pointer &base,
                                   const char *type_message, OpenAPIWalk &walk)
    -> std::vector<JSON::String> {
  openapi_expect_array(value, base, type_message);
  std::vector<JSON::String> result;
  result.reserve(value.size());
  std::size_t index{0};
  for (const auto &requirement : value.as_array()) {
    const auto location{openapi_child(base, index)};
    openapi_check_security_requirement(requirement, location, walk);
    result.push_back(openapi_location_uri(walk.base, location));
    index += 1;
  }

  return result;
}

} // namespace sourcemeta::core

#endif
