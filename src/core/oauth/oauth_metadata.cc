#include <sourcemeta/core/oauth_metadata.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth_error.h>

#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

namespace sourcemeta::core {

namespace {

using namespace std::literals::string_view_literals;

const auto HASH_ISSUER{JSON::Object::hash("issuer"sv)};
const auto HASH_AUTHORIZATION_ENDPOINT{
    JSON::Object::hash("authorization_endpoint"sv)};
const auto HASH_TOKEN_ENDPOINT{JSON::Object::hash("token_endpoint"sv)};
const auto HASH_REGISTRATION_ENDPOINT{
    JSON::Object::hash("registration_endpoint"sv)};
const auto HASH_REVOCATION_ENDPOINT{
    JSON::Object::hash("revocation_endpoint"sv)};
const auto HASH_INTROSPECTION_ENDPOINT{
    JSON::Object::hash("introspection_endpoint"sv)};
const auto HASH_JWKS_URI{JSON::Object::hash("jwks_uri"sv)};
const auto HASH_RESPONSE_TYPES{
    JSON::Object::hash("response_types_supported"sv)};
const auto HASH_GRANT_TYPES{JSON::Object::hash("grant_types_supported"sv)};
const auto HASH_CODE_CHALLENGE_METHODS{
    JSON::Object::hash("code_challenge_methods_supported"sv)};
const auto HASH_TOKEN_AUTH_METHODS{
    JSON::Object::hash("token_endpoint_auth_methods_supported"sv)};
const auto HASH_TOKEN_AUTH_ALGS{
    JSON::Object::hash("token_endpoint_auth_signing_alg_values_supported"sv)};
const auto HASH_ISS_SUPPORTED{
    JSON::Object::hash("authorization_response_iss_parameter_supported"sv)};

auto is_valid_issuer(const std::string_view issuer) -> bool {
  // RFC 8414 Section 2: the issuer is an https URL with no query or fragment
  static constexpr std::string_view scheme{"https://"};
  return issuer.starts_with(scheme) && issuer.size() > scheme.size() &&
         issuer.find('#') == std::string_view::npos &&
         issuer.find('?') == std::string_view::npos;
}

auto string_member(const JSON &data, const JSON::StringView name,
                   const JSON::Object::hash_type hash)
    -> std::optional<std::string_view> {
  if (!data.is_object()) {
    return std::nullopt;
  }

  const auto *member{data.try_at(name, hash)};
  if (member == nullptr || !member->is_string()) {
    return std::nullopt;
  }

  return std::string_view{member->to_string()};
}

auto array_member_contains(const JSON &data, const JSON::StringView name,
                           const JSON::Object::hash_type hash,
                           const JSON::StringView value) -> bool {
  if (!data.is_object()) {
    return false;
  }

  const auto *member{data.try_at(name, hash)};
  return member != nullptr && member->is_array() && member->contains(value);
}

auto validated_server_metadata(JSON &&data, const std::string_view issuer)
    -> JSON {
  if (!data.is_object()) {
    throw OAuthMetadataParseError{};
  }

  const auto *issuer_member{data.try_at("issuer"sv, HASH_ISSUER)};
  if (issuer_member == nullptr || !issuer_member->is_string()) {
    throw OAuthMetadataParseError{};
  }

  // RFC 8414 Section 3.3: the issuer in the document must be identical by code
  // points to the one the document was retrieved for, the impersonation defense
  const auto issuer_value{std::string_view{issuer_member->to_string()}};
  if (issuer_value != issuer || !is_valid_issuer(issuer_value)) {
    throw OAuthMetadataParseError{};
  }

  // RFC 8414 Section 2: response_types_supported is REQUIRED, and Section 3.2:
  // "Claims with zero elements MUST be omitted from the response", so a
  // present but empty array is a malformed document
  const auto *response_types{
      data.try_at("response_types_supported"sv, HASH_RESPONSE_TYPES)};
  if (response_types == nullptr || !response_types->is_array() ||
      response_types->empty()) {
    throw OAuthMetadataParseError{};
  }

  // RFC 8414 Section 2: a signing algorithm list is REQUIRED alongside the JWT
  // authentication methods, and "none" MUST NOT appear in it
  if (array_member_contains(data, "token_endpoint_auth_methods_supported"sv,
                            HASH_TOKEN_AUTH_METHODS, "private_key_jwt") ||
      array_member_contains(data, "token_endpoint_auth_methods_supported"sv,
                            HASH_TOKEN_AUTH_METHODS, "client_secret_jwt")) {
    const auto *algorithms{
        data.try_at("token_endpoint_auth_signing_alg_values_supported"sv,
                    HASH_TOKEN_AUTH_ALGS)};
    if (algorithms == nullptr || !algorithms->is_array() ||
        algorithms->contains("none")) {
      throw OAuthMetadataParseError{};
    }
  }

  return std::move(data);
}

} // namespace

auto oauth_well_known_url(const std::string_view identifier,
                          const OAuthWellKnownKind kind, std::string &sink)
    -> bool {
  static constexpr std::string_view scheme{"https://"};
  // Reject a missing scheme, a fragment, or an empty authority, where the
  // character after the scheme already ends the host (RFC 3986 Section 3.2)
  if (!identifier.starts_with(scheme) || identifier.size() == scheme.size() ||
      identifier[scheme.size()] == '/' || identifier[scheme.size()] == '?' ||
      identifier.find('#') != std::string_view::npos) {
    return false;
  }

  const bool protected_resource{kind == OAuthWellKnownKind::ProtectedResource};
  const auto query_position{identifier.find('?')};
  // RFC 8414 Section 2: an issuer carries no query, unlike a protected resource
  if (query_position != std::string_view::npos && !protected_resource) {
    return false;
  }

  std::string_view suffix;
  switch (kind) {
    case OAuthWellKnownKind::AuthorizationServer:
      suffix = "oauth-authorization-server";
      break;
    case OAuthWellKnownKind::ProtectedResource:
      suffix = "oauth-protected-resource";
      break;
    case OAuthWellKnownKind::OpenIDConfigurationInserted:
    case OAuthWellKnownKind::OpenIDConfigurationAppended:
      suffix = "openid-configuration";
      break;
  }

  static constexpr std::string_view infix{"/.well-known/"};
  if (kind == OAuthWellKnownKind::OpenIDConfigurationAppended) {
    // RFC 8414 Section 5: the legacy OpenID Connect form appends the well-known
    // string after the path rather than inserting it
    auto base{identifier};
    if (base.ends_with('/')) {
      base.remove_suffix(1);
    }

    sink.reserve(sink.size() + base.size() + infix.size() + suffix.size());
    sink.append(base);
    sink.append(infix);
    sink.append(suffix);
    return true;
  }

  std::string_view query;
  auto without_query{identifier};
  if (query_position != std::string_view::npos) {
    query = identifier.substr(query_position);
    without_query = identifier.substr(0, query_position);
  }

  // RFC 8414 Section 3.1: the well-known string is inserted between the host
  // and the path, with any terminating slash on the path removed first
  auto authority{without_query};
  std::string_view path;
  const auto path_position{without_query.find('/', scheme.size())};
  if (path_position != std::string_view::npos) {
    authority = without_query.substr(0, path_position);
    path = without_query.substr(path_position);
  }

  if (path.ends_with('/')) {
    path.remove_suffix(1);
  }

  sink.reserve(sink.size() + authority.size() + infix.size() + suffix.size() +
               path.size() + query.size());
  sink.append(authority);
  sink.append(infix);
  sink.append(suffix);
  sink.append(path);
  sink.append(query);
  return true;
}

OAuthServerMetadata::OAuthServerMetadata(JSON &&data,
                                         const std::string_view issuer)
    : data_{validated_server_metadata(std::move(data), issuer)} {}

auto OAuthServerMetadata::from(JSON &&data, const std::string_view issuer)
    -> std::optional<OAuthServerMetadata> {
  try {
    return OAuthServerMetadata{std::move(data), issuer};
  } catch (const OAuthMetadataParseError &) {
    return std::nullopt;
  }
}

auto OAuthServerMetadata::issuer() const -> std::string_view {
  return string_member(this->data_, "issuer"sv, HASH_ISSUER).value();
}

auto OAuthServerMetadata::authorization_endpoint() const
    -> std::optional<std::string_view> {
  return string_member(this->data_, "authorization_endpoint"sv,
                       HASH_AUTHORIZATION_ENDPOINT);
}

auto OAuthServerMetadata::token_endpoint() const
    -> std::optional<std::string_view> {
  return string_member(this->data_, "token_endpoint"sv, HASH_TOKEN_ENDPOINT);
}

auto OAuthServerMetadata::registration_endpoint() const
    -> std::optional<std::string_view> {
  return string_member(this->data_, "registration_endpoint"sv,
                       HASH_REGISTRATION_ENDPOINT);
}

auto OAuthServerMetadata::revocation_endpoint() const
    -> std::optional<std::string_view> {
  return string_member(this->data_, "revocation_endpoint"sv,
                       HASH_REVOCATION_ENDPOINT);
}

auto OAuthServerMetadata::introspection_endpoint() const
    -> std::optional<std::string_view> {
  return string_member(this->data_, "introspection_endpoint"sv,
                       HASH_INTROSPECTION_ENDPOINT);
}

auto OAuthServerMetadata::jwks_uri() const -> std::optional<std::string_view> {
  return string_member(this->data_, "jwks_uri"sv, HASH_JWKS_URI);
}

auto OAuthServerMetadata::authorization_response_iss_parameter_supported() const
    -> bool {
  if (!this->data_.is_object()) {
    return false;
  }

  const auto *member{this->data_.try_at(
      "authorization_response_iss_parameter_supported"sv, HASH_ISS_SUPPORTED)};
  return member != nullptr && member->is_boolean() && member->to_boolean();
}

auto OAuthServerMetadata::supports_response_type(
    const std::string_view value) const -> bool {
  return array_member_contains(this->data_, "response_types_supported"sv,
                               HASH_RESPONSE_TYPES, value);
}

auto OAuthServerMetadata::supports_grant_type(
    const std::string_view value) const -> bool {
  const auto *member{
      this->data_.is_object()
          ? this->data_.try_at("grant_types_supported"sv, HASH_GRANT_TYPES)
          : nullptr};
  if (member == nullptr || !member->is_array()) {
    // RFC 8414 Section 2: the default is the authorization code and implicit
    // grants
    return value == "authorization_code" || value == "implicit";
  }

  return member->contains(value);
}

auto OAuthServerMetadata::supports_code_challenge_method(
    const std::string_view value) const -> bool {
  // RFC 8414 Section 2: an omitted list means PKCE is not supported, so there
  // is no default to fall back to
  return array_member_contains(this->data_,
                               "code_challenge_methods_supported"sv,
                               HASH_CODE_CHALLENGE_METHODS, value);
}

auto OAuthServerMetadata::supports_token_endpoint_auth_method(
    const std::string_view value) const -> bool {
  const auto *member{
      this->data_.is_object()
          ? this->data_.try_at("token_endpoint_auth_methods_supported"sv,
                               HASH_TOKEN_AUTH_METHODS)
          : nullptr};
  if (member == nullptr || !member->is_array()) {
    // RFC 8414 Section 2: the default is client_secret_basic
    return value == "client_secret_basic";
  }

  return member->contains(value);
}

auto OAuthServerMetadata::data() const -> const JSON & { return this->data_; }

} // namespace sourcemeta::core
