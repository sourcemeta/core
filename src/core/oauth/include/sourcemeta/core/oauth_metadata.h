#ifndef SOURCEMETA_CORE_OAUTH_METADATA_H_
#define SOURCEMETA_CORE_OAUTH_METADATA_H_

#ifndef SOURCEMETA_CORE_OAUTH_EXPORT
#include <sourcemeta/core/oauth_export.h>
#endif

#include <sourcemeta/core/json.h>

#include <cstdint>     // std::uint8_t
#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::core {

/// @ingroup oauth
/// The kind of metadata document a well-known URL points at, which selects the
/// suffix and whether it is inserted before the path or appended after it.
enum class OAuthWellKnownKind : std::uint8_t {
  /// The `oauth-authorization-server` document, inserted before the path
  /// (RFC 8414 Section 3).
  AuthorizationServer,
  /// The `oauth-protected-resource` document, inserted before the path
  /// (RFC 9728 Section 3).
  ProtectedResource,
  /// The `openid-configuration` document, inserted before the path (RFC 8414
  /// Section 5).
  OpenIDConfigurationInserted,
  /// The `openid-configuration` document, appended after the path, the legacy
  /// OpenID Connect Discovery form (RFC 8414 Section 5).
  OpenIDConfigurationAppended
};

/// @ingroup oauth
/// Derive a metadata well-known URL from an identifier and append it to the
/// sink, returning whether the identifier is well formed (RFC 8414 Section 3,
/// RFC 9728 Section 3). The identifier must use the `https` scheme and carry no
/// fragment, and no query unless it is a protected resource. A terminating
/// slash on the path is removed, and for the inserted kinds the well-known
/// string is placed between the host and the path, preserving a protected
/// resource query after it. The sink is appended to and never cleared. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/oauth.h>
/// #include <cassert>
/// #include <string>
///
/// std::string url;
/// assert(sourcemeta::core::oauth_well_known_url(
///     "https://example.com/issuer1",
///     sourcemeta::core::OAuthWellKnownKind::AuthorizationServer, url));
/// assert(url ==
///        "https://example.com/.well-known/oauth-authorization-server/issuer1");
/// ```
SOURCEMETA_CORE_OAUTH_EXPORT
auto oauth_well_known_url(const std::string_view identifier,
                          const OAuthWellKnownKind kind, std::string &sink)
    -> bool;

#if defined(_MSC_VER)
#pragma warning(disable : 4251)
#endif

/// @ingroup oauth
/// An authorization server metadata document (RFC 8414), owning its JSON. The
/// document is validated on construction against the issuer it was retrieved
/// for: the issuer must match by exact code points and be a valid issuer
/// identifier, `response_types_supported` must be present and non-empty, and an
/// authentication method that needs a signing algorithm list must carry a
/// non-empty one without `none`. Accessors apply the specification defaults,
/// and any member without a typed accessor is reachable through the underlying
/// document. A string accessor returns a view into the owned document, valid
/// for the lifetime of this object, so a view taken before the object is moved
/// from must not be used afterward. For example:
///
/// ```cpp
/// #include <sourcemeta/core/oauth.h>
/// #include <sourcemeta/core/json.h>
/// #include <cassert>
///
/// auto document{sourcemeta::core::parse_json(
///     R"JSON({"issuer":"https://example.com",
///            "response_types_supported":["code"],
///            "authorization_endpoint":"https://example.com/authorize",
///            "token_endpoint":"https://example.com/token"})JSON")};
/// const auto metadata{sourcemeta::core::OAuthServerMetadata::from(
///     std::move(document), "https://example.com")};
/// assert(metadata.has_value());
/// assert(metadata.value().token_endpoint().value() ==
///        "https://example.com/token");
/// ```
class SOURCEMETA_CORE_OAUTH_EXPORT OAuthServerMetadata {
public:
  /// Construct and validate a metadata document for an expected issuer,
  /// throwing when it is invalid. The document is moved in.
  OAuthServerMetadata(JSON &&data, const std::string_view issuer);

  /// Construct and validate a metadata document for an expected issuer,
  /// returning no value when it is invalid. The document is moved in.
  [[nodiscard]] static auto from(JSON &&data, const std::string_view issuer)
      -> std::optional<OAuthServerMetadata>;

  /// The issuer identifier (RFC 8414 Section 2).
  [[nodiscard]] auto issuer() const -> std::string_view;

  /// The authorization endpoint (RFC 8414 Section 2).
  [[nodiscard]] auto authorization_endpoint() const
      -> std::optional<std::string_view>;

  /// The token endpoint (RFC 8414 Section 2).
  [[nodiscard]] auto token_endpoint() const -> std::optional<std::string_view>;

  /// The dynamic client registration endpoint (RFC 8414 Section 2).
  [[nodiscard]] auto registration_endpoint() const
      -> std::optional<std::string_view>;

  /// The token revocation endpoint (RFC 8414 Section 2).
  [[nodiscard]] auto revocation_endpoint() const
      -> std::optional<std::string_view>;

  /// The token introspection endpoint (RFC 8414 Section 2).
  [[nodiscard]] auto introspection_endpoint() const
      -> std::optional<std::string_view>;

  /// The JWK Set document location (RFC 8414 Section 2).
  [[nodiscard]] auto jwks_uri() const -> std::optional<std::string_view>;

  /// Whether the server signs authorization responses with an `iss` parameter,
  /// defaulting to false when absent (RFC 9207 Section 3).
  [[nodiscard]] auto authorization_response_iss_parameter_supported() const
      -> bool;

  /// Whether a response type is supported (RFC 8414 Section 2).
  [[nodiscard]] auto supports_response_type(const std::string_view value) const
      -> bool;

  /// Whether a grant type is supported, defaulting to the authorization code
  /// and implicit grants when absent (RFC 8414 Section 2).
  [[nodiscard]] auto supports_grant_type(const std::string_view value) const
      -> bool;

  /// Whether a PKCE code challenge method is supported, defaulting to none when
  /// absent since an omitted list means PKCE is unsupported (RFC 8414
  /// Section 2).
  [[nodiscard]] auto
  supports_code_challenge_method(const std::string_view value) const -> bool;

  /// Whether a token endpoint authentication method is supported, defaulting to
  /// `client_secret_basic` when absent (RFC 8414 Section 2).
  [[nodiscard]] auto
  supports_token_endpoint_auth_method(const std::string_view value) const
      -> bool;

  /// The underlying document, for reaching members without a typed accessor.
  [[nodiscard]] auto data() const -> const JSON &;

private:
  JSON data_;
};

#if defined(_MSC_VER)
#pragma warning(default : 4251)
#endif

} // namespace sourcemeta::core

#endif
