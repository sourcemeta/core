#ifndef SOURCEMETA_CORE_OAUTH_TOKEN_H_
#define SOURCEMETA_CORE_OAUTH_TOKEN_H_

#ifndef SOURCEMETA_CORE_OAUTH_EXPORT
#include <sourcemeta/core/oauth_export.h>
#endif

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth_authorization.h>

#include <chrono>      // std::chrono::seconds
#include <optional>    // std::optional
#include <span>        // std::span
#include <string_view> // std::string_view

namespace sourcemeta::core {

/// @ingroup oauth
/// Append an authorization code grant token request body (RFC 6749
/// Section 4.1.3) to the sink. The `code` is required, the `redirect_uri` is
/// emitted only when the authorization request carried one, and the
/// `code_verifier` is emitted only when PKCE is in use (RFC 7636 Section 4.5).
/// No `client_id` is emitted, so the caller composes a client authentication
/// builder into the same sink. The body carries secrets, so the sink is a
/// wiping string, and it is appended to and never cleared. For example:
///
/// ```cpp
/// #include <sourcemeta/core/oauth.h>
/// #include <cassert>
///
/// sourcemeta::core::SecureString body;
/// sourcemeta::core::oauth_build_token_request_code(
///     "SplxlOBeZQQYbYS6WxSbIA", "https://client.example/cb", "", {}, body);
/// assert(body == "grant_type=authorization_code&code=SplxlOBeZQQYbYS6WxSbIA"
///                "&redirect_uri=https%3A%2F%2Fclient.example%2Fcb");
/// ```
SOURCEMETA_CORE_OAUTH_EXPORT
auto oauth_build_token_request_code(
    const std::string_view code, const std::string_view redirect_uri,
    const std::string_view code_verifier,
    const std::span<const OAuthParameter> resources, SecureString &sink)
    -> void;

/// @ingroup oauth
/// Append a refresh token grant token request body (RFC 6749 Section 6) to the
/// sink. The `refresh_token` is required, and the requested `scope` is emitted
/// only when present and must not exceed the original grant. No `client_id` is
/// emitted, so the caller composes a client authentication builder into the
/// same sink. The body carries secrets, so the sink is a wiping string, and it
/// is appended to and never cleared. For example:
///
/// ```cpp
/// #include <sourcemeta/core/oauth.h>
/// #include <cassert>
///
/// sourcemeta::core::SecureString body;
/// sourcemeta::core::oauth_build_token_request_refresh("tGzv3JOkF0XG5Qx2TlKWIA",
///                                                     "read", {}, body);
/// assert(body == "grant_type=refresh_token"
///                "&refresh_token=tGzv3JOkF0XG5Qx2TlKWIA&scope=read");
/// ```
SOURCEMETA_CORE_OAUTH_EXPORT
auto oauth_build_token_request_refresh(
    const std::string_view refresh_token, const std::string_view scope,
    const std::span<const OAuthParameter> resources, SecureString &sink)
    -> void;

/// @ingroup oauth
/// Append a client credentials grant token request body (RFC 6749
/// Section 4.4.2) to the sink. The requested `scope` is emitted only when
/// present. No `client_id` is emitted, so the caller composes a client
/// authentication builder into the same sink. The sink is appended to and never
/// cleared. For example:
///
/// ```cpp
/// #include <sourcemeta/core/oauth.h>
/// #include <cassert>
///
/// sourcemeta::core::SecureString body;
/// sourcemeta::core::oauth_build_token_request_client_credentials("read", {},
///                                                                body);
/// assert(body == "grant_type=client_credentials&scope=read");
/// ```
SOURCEMETA_CORE_OAUTH_EXPORT
auto oauth_build_token_request_client_credentials(
    const std::string_view scope,
    const std::span<const OAuthParameter> resources, SecureString &sink)
    -> void;

/// @ingroup oauth
/// A non-owning view over a token endpoint success response (RFC 6749
/// Section 5.1) held in a caller-owned JSON value. The value must outlive the
/// view, and the token accessors return views into it, which are secret and
/// must not be logged. Extension members such as an OpenID Connect `id_token`
/// are reached through the underlying document. For example:
///
/// ```cpp
/// #include <sourcemeta/core/oauth.h>
/// #include <sourcemeta/core/json.h>
/// #include <cassert>
///
/// const auto document{sourcemeta::core::parse_json(
///     R"JSON({"access_token":"2YotnFZFEjr1zCsicMWpAA","token_type":"Bearer",
///            "expires_in":3600})JSON")};
/// const sourcemeta::core::OAuthTokenResponse response{document};
/// assert(response.is_bearer_token_type());
/// assert(response.access_token().value() == "2YotnFZFEjr1zCsicMWpAA");
/// ```
class SOURCEMETA_CORE_OAUTH_EXPORT OAuthTokenResponse {
public:
  /// Construct a view over a token response document, which is borrowed.
  explicit OAuthTokenResponse(const JSON &data);

  /// The issued access token (RFC 6749 Section 5.1), or no value when absent.
  [[nodiscard]] auto access_token() const -> std::optional<std::string_view>;

  /// The access token type (RFC 6749 Section 5.1), or no value when absent.
  [[nodiscard]] auto token_type() const -> std::optional<std::string_view>;

  /// Whether the token type is `Bearer`, matched case insensitively (RFC 6749
  /// Section 5.1).
  [[nodiscard]] auto is_bearer_token_type() const -> bool;

  /// The access token lifetime in seconds (RFC 6749 Section 5.1), or no value
  /// when absent or not a non-negative integer.
  [[nodiscard]] auto expires_in() const -> std::optional<std::chrono::seconds>;

  /// The issued refresh token (RFC 6749 Section 5.1), or no value when absent.
  [[nodiscard]] auto refresh_token() const -> std::optional<std::string_view>;

  /// The granted scope (RFC 6749 Section 5.1), or no value when absent.
  [[nodiscard]] auto scope() const -> std::optional<std::string_view>;

  /// Whether the granted scope contains a value, comparing the space-delimited
  /// unordered set (RFC 6749 Section 3.3).
  [[nodiscard]] auto has_scope(const std::string_view value) const -> bool;

  /// The underlying document, for reaching extension members such as an OpenID
  /// Connect `id_token`.
  [[nodiscard]] auto data() const -> const JSON &;

private:
  const JSON *data_;
};

} // namespace sourcemeta::core

#endif
