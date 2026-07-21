#include <sourcemeta/core/io.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/core/text.h>

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <filesystem>  // std::filesystem::path
#include <set>         // std::set
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

// Each value the module emits or recognizes on the wire is asserted to be an
// entry in the pinned IANA registry snapshot (RFC 6749 Section 11 and the
// registries its extensions establish), so a typo in a constant or a drift from
// the registry is caught. Values the registries do not carry, such as the
// device authorization response fields and the revocation `token` and
// `token_type_hint` parameters, are described normatively rather than
// registered here, so they are not asserted against these registries. Keys the
// module uses only as JSON Web Token or introspection claims, such as `aud`,
// `cnf`, and the DPoP proof `nonce`, are likewise not OAuth parameters and
// belong to their own registries rather than these.

namespace {

// The registered values live in the first column of each IANA CSV, one per row
// after the header. A registered name carries no comma, so it is the part
// before the first one
auto load_registry(const std::filesystem::path &path) -> std::set<std::string> {
  std::set<std::string> names;
  auto stream{sourcemeta::core::read_file(path)};
  bool header{true};
  sourcemeta::core::for_each_line(
      stream, [&names, &header](const std::string_view line) -> void {
        if (header) {
          header = false;
          return;
        }

        if (line.empty()) {
          return;
        }

        const auto columns{sourcemeta::core::split_once(line, ',')};
        names.emplace(columns.has_value() ? columns.value().first : line);
      });

  return names;
}

// The membership is resolved at registration time and the outcome carried into
// the closure, matching the vendored suite runners
auto register_membership(const std::string &suite, const std::string_view value,
                         const std::set<std::string> &registry) -> void {
  const std::string name{value};
  const bool registered{registry.contains(name)};
  sourcemeta::core::test_register(
      suite + "." + name, [registered]() -> void { EXPECT_TRUE(registered); });
}

template <typename Error, std::size_t Size>
auto register_errors(const std::string &suite,
                     const std::array<Error, Size> &errors,
                     const std::set<std::string> &registry) -> void {
  for (const auto error : errors) {
    register_membership(suite, sourcemeta::core::oauth_error_code(error),
                        registry);
  }
}

auto register_all(const std::string &suite,
                  const std::span<const std::string_view> values,
                  const std::set<std::string> &registry) -> void {
  for (const auto value : values) {
    register_membership(suite, value, registry);
  }
}

} // namespace

auto main(int argc, char **argv) -> int {
  const std::filesystem::path base{IANA_OAUTH_PATH};

  // OAuth Extensions Error Registry (RFC 6749 Section 11.4): every wire code
  // the error mappings emit, per endpoint family
  const auto errors{load_registry(base / "extensions-error.csv")};
  register_errors(
      "OAuth_IANA_AuthorizationError",
      std::array<sourcemeta::core::OAuthAuthorizationError, 7>{
          {sourcemeta::core::OAuthAuthorizationError::InvalidRequest,
           sourcemeta::core::OAuthAuthorizationError::UnauthorizedClient,
           sourcemeta::core::OAuthAuthorizationError::AccessDenied,
           sourcemeta::core::OAuthAuthorizationError::UnsupportedResponseType,
           sourcemeta::core::OAuthAuthorizationError::InvalidScope,
           sourcemeta::core::OAuthAuthorizationError::ServerError,
           sourcemeta::core::OAuthAuthorizationError::TemporarilyUnavailable}},
      errors);
  register_errors(
      "OAuth_IANA_TokenError",
      std::array<sourcemeta::core::OAuthTokenError, 14>{
          {sourcemeta::core::OAuthTokenError::InvalidRequest,
           sourcemeta::core::OAuthTokenError::InvalidClient,
           sourcemeta::core::OAuthTokenError::InvalidGrant,
           sourcemeta::core::OAuthTokenError::UnauthorizedClient,
           sourcemeta::core::OAuthTokenError::UnsupportedGrantType,
           sourcemeta::core::OAuthTokenError::InvalidScope,
           sourcemeta::core::OAuthTokenError::AuthorizationPending,
           sourcemeta::core::OAuthTokenError::SlowDown,
           sourcemeta::core::OAuthTokenError::AccessDenied,
           sourcemeta::core::OAuthTokenError::ExpiredToken,
           sourcemeta::core::OAuthTokenError::InvalidTarget,
           sourcemeta::core::OAuthTokenError::InvalidDPoPProof,
           sourcemeta::core::OAuthTokenError::UseDPoPNonce,
           sourcemeta::core::OAuthTokenError::UnsupportedTokenType}},
      errors);
  register_errors("OAuth_IANA_BearerError",
                  std::array<sourcemeta::core::OAuthBearerError, 5>{
                      {sourcemeta::core::OAuthBearerError::InvalidRequest,
                       sourcemeta::core::OAuthBearerError::InvalidToken,
                       sourcemeta::core::OAuthBearerError::InsufficientScope,
                       sourcemeta::core::OAuthBearerError::InvalidDPoPProof,
                       sourcemeta::core::OAuthBearerError::UseDPoPNonce}},
                  errors);
  register_errors(
      "OAuth_IANA_RegistrationError",
      std::array<sourcemeta::core::OAuthRegistrationError, 4>{
          {sourcemeta::core::OAuthRegistrationError::InvalidRedirectURI,
           sourcemeta::core::OAuthRegistrationError::InvalidClientMetadata,
           sourcemeta::core::OAuthRegistrationError::InvalidSoftwareStatement,
           sourcemeta::core::OAuthRegistrationError::
               UnapprovedSoftwareStatement}},
      errors);

  // OAuth Access Token Types (RFC 6749 Section 11.1)
  const auto token_types{load_registry(base / "token-types.csv")};
  const std::array<std::string_view, 2> access_token_types{
      {"Bearer", sourcemeta::core::OAUTH_TOKEN_TYPE_DPOP}};
  register_all("OAuth_IANA_TokenType", access_token_types, token_types);

  // OAuth Token Type Hints (RFC 7009 Section 4.1.2)
  const auto hints{load_registry(base / "token-type-hint.csv")};
  const std::array<std::string_view, 2> token_type_hints{
      {sourcemeta::core::OAUTH_TOKEN_TYPE_HINT_ACCESS_TOKEN,
       sourcemeta::core::OAUTH_TOKEN_TYPE_HINT_REFRESH_TOKEN}};
  register_all("OAuth_IANA_TokenTypeHint", token_type_hints, hints);

  // PKCE Code Challenge Methods (RFC 7636 Section 6.2.2)
  const auto pkce_methods{
      load_registry(base / "pkce-code-challenge-method.csv")};
  const std::array<std::string_view, 2> code_challenge_methods{
      {"plain", "S256"}};
  register_all("OAuth_IANA_PKCE", code_challenge_methods, pkce_methods);

  // OAuth Authorization Endpoint Response Types (RFC 6749 Section 11.3)
  const auto response_types{load_registry(base / "endpoint.csv")};
  const std::array<std::string_view, 2> supported_response_types{
      {"code", "token"}};
  register_all("OAuth_IANA_ResponseType", supported_response_types,
               response_types);

  // OAuth URI (RFC 6755): the module's grant type, client assertion type, and
  // token type identifiers, plus the request URI reference
  const auto uris{load_registry(base / "uri.csv")};
  const std::array<std::string_view, 11> identifiers{
      {sourcemeta::core::OAUTH_GRANT_TYPE_JWT_BEARER,
       sourcemeta::core::OAUTH_CLIENT_ASSERTION_TYPE_JWT_BEARER,
       sourcemeta::core::OAUTH_TOKEN_TYPE_ACCESS_TOKEN,
       sourcemeta::core::OAUTH_TOKEN_TYPE_REFRESH_TOKEN,
       sourcemeta::core::OAUTH_TOKEN_TYPE_ID_TOKEN,
       sourcemeta::core::OAUTH_TOKEN_TYPE_JWT,
       sourcemeta::core::OAUTH_TOKEN_TYPE_SAML1,
       sourcemeta::core::OAUTH_TOKEN_TYPE_SAML2,
       "urn:ietf:params:oauth:grant-type:device_code",
       "urn:ietf:params:oauth:grant-type:token-exchange",
       "urn:ietf:params:oauth:request_uri"}};
  register_all("OAuth_IANA_URI", identifiers, uris);

  // OAuth Parameters (RFC 6749 Section 11.2): the request and response
  // parameters the builders and parsers use
  const auto parameters{load_registry(base / "parameters.csv")};
  const std::array<std::string_view, 33> parameter_names{
      {"client_id",
       "client_secret",
       "response_type",
       "redirect_uri",
       "scope",
       "state",
       "code",
       "grant_type",
       "error",
       "error_description",
       "error_uri",
       "code_challenge",
       "code_challenge_method",
       "code_verifier",
       "refresh_token",
       "access_token",
       "token_type",
       "device_code",
       "request_uri",
       "resource",
       "audience",
       "dpop_jkt",
       "assertion",
       "client_assertion",
       "client_assertion_type",
       "subject_token",
       "subject_token_type",
       "actor_token",
       "actor_token_type",
       "requested_token_type",
       "issued_token_type",
       "iss",
       "expires_in"}};
  register_all("OAuth_IANA_Parameter", parameter_names, parameters);

  // OAuth Authorization Server Metadata (RFC 8414 Section 7.1)
  const auto server_metadata{
      load_registry(base / "authorization-server-metadata.csv")};
  const std::array<std::string_view, 16> server_metadata_names{
      {"issuer", "authorization_endpoint", "token_endpoint",
       "registration_endpoint", "jwks_uri", "response_types_supported",
       "grant_types_supported", "code_challenge_methods_supported",
       "token_endpoint_auth_methods_supported",
       "token_endpoint_auth_signing_alg_values_supported", "scopes_supported",
       "revocation_endpoint", "introspection_endpoint",
       "pushed_authorization_request_endpoint",
       "require_pushed_authorization_requests",
       "authorization_response_iss_parameter_supported"}};
  register_all("OAuth_IANA_ServerMetadata", server_metadata_names,
               server_metadata);

  // OAuth Protected Resource Metadata (RFC 9728 Section 6.1)
  const auto resource_metadata{
      load_registry(base / "protected-resource-metadata.csv")};
  const std::array<std::string_view, 7> resource_metadata_names{
      {"resource", "authorization_servers", "jwks_uri",
       "bearer_methods_supported", "resource_signing_alg_values_supported",
       "scopes_supported", "dpop_bound_access_tokens_required"}};
  register_all("OAuth_IANA_ResourceMetadata", resource_metadata_names,
               resource_metadata);

  // OAuth Dynamic Client Registration Metadata (RFC 7591 Section 4.2, RFC 7592)
  const auto client_metadata{load_registry(base / "client-metadata.csv")};
  const std::array<std::string_view, 22> client_metadata_names{
      {"redirect_uris",
       "token_endpoint_auth_method",
       "grant_types",
       "response_types",
       "client_name",
       "client_uri",
       "logo_uri",
       "scope",
       "contacts",
       "tos_uri",
       "policy_uri",
       "jwks_uri",
       "jwks",
       "software_id",
       "software_version",
       "software_statement",
       "client_id",
       "client_secret",
       "client_id_issued_at",
       "client_secret_expires_at",
       "registration_access_token",
       "registration_client_uri"}};
  register_all("OAuth_IANA_ClientMetadata", client_metadata_names,
               client_metadata);

  // OAuth Token Introspection Response (RFC 7662 Section 3.1)
  const auto introspection{
      load_registry(base / "token-introspection-response.csv")};
  const std::array<std::string_view, 11> introspection_names{
      {"active", "client_id", "exp", "iat", "iss", "jti", "nbf", "scope", "sub",
       "token_type", "username"}};
  register_all("OAuth_IANA_IntrospectionResponse", introspection_names,
               introspection);

  return sourcemeta::core::test_run(argc, argv);
}
