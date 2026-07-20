#ifndef SOURCEMETA_CORE_OAUTH_AUTHORIZATION_H_
#define SOURCEMETA_CORE_OAUTH_AUTHORIZATION_H_

#ifndef SOURCEMETA_CORE_OAUTH_EXPORT
#include <sourcemeta/core/oauth_export.h>
#endif

#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::core {

/// @ingroup oauth
/// A single query or form parameter as a name and value pair. Both members are
/// non-owning views that must outlive any use of this parameter. For example:
///
/// ```cpp
/// #include <sourcemeta/core/oauth.h>
///
/// const sourcemeta::core::OAuthParameter parameter{"resource",
///                                                  "https://api.example"};
/// ```
struct OAuthParameter {
  /// The parameter name.
  std::string_view name;
  /// The parameter value, still in its raw unescaped form.
  std::string_view value;
};

/// @ingroup oauth
/// A non-owning view of the parameters of an authorization request (RFC 6749
/// Section 4.1.1). Every field borrows from the caller and must outlive any use
/// of this struct. An empty scalar field is treated as absent and is not
/// emitted, and the spans carry the repeatable and extension parameters. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/oauth.h>
/// #include <cassert>
/// #include <string>
///
/// sourcemeta::core::OAuthAuthorizationRequest request;
/// request.client_id = "s6BhdRkqt3";
/// request.state = "xyz";
/// std::string url;
/// sourcemeta::core::oauth_build_authorization_url(
///     "https://server.example/authorize", request, url);
/// assert(url ==
///        "https://server.example/authorize?response_type=code"
///        "&client_id=s6BhdRkqt3&state=xyz");
/// ```
struct OAuthAuthorizationRequest {
  /// The client identifier (RFC 6749 Section 2.2).
  std::string_view client_id;
  /// The redirection endpoint the response is returned to (RFC 6749
  /// Section 3.1.2).
  std::string_view redirect_uri;
  /// The space-delimited requested scope (RFC 6749 Section 3.3).
  std::string_view scope;
  /// The opaque cross-site request forgery token echoed back on the response
  /// (RFC 6749 Section 10.12).
  std::string_view state;
  /// The PKCE code challenge (RFC 7636 Section 4.3).
  std::string_view code_challenge;
  /// The PKCE code challenge method, emitted only alongside a challenge
  /// (RFC 7636 Section 4.3).
  std::string_view code_challenge_method;
  /// The reference to a pushed authorization request (RFC 9126 Section 4).
  std::string_view request_uri;
  /// The JWK thumbprint of the DPoP proof public key (RFC 9449 Section 10).
  std::string_view dpop_jkt;
  /// The repeatable resource indicators, each an absolute URI without a
  /// fragment (RFC 8707 Section 2).
  std::span<const OAuthParameter> resources;
  /// The extension parameters, such as an OpenID Connect `nonce`, emitted
  /// verbatim after the known parameters.
  std::span<const OAuthParameter> extra;
};

/// @ingroup oauth
/// Build an authorization request URL by appending the query to the endpoint
/// (RFC 6749 Section 4.1.1). The `response_type` is always `code`, since the
/// implicit grant is not represented, every value is percent-escaped, an
/// existing query on the endpoint is honored, and the code challenge method is
/// emitted only when a challenge is present. The sink is appended to and never
/// cleared. For example:
///
/// ```cpp
/// #include <sourcemeta/core/oauth.h>
/// #include <cassert>
/// #include <string>
///
/// sourcemeta::core::OAuthAuthorizationRequest request;
/// request.client_id = "s6BhdRkqt3";
/// request.redirect_uri = "https://client.example/cb";
/// std::string url;
/// sourcemeta::core::oauth_build_authorization_url(
///     "https://server.example/authorize", request, url);
/// assert(url ==
///        "https://server.example/authorize?response_type=code"
///        "&client_id=s6BhdRkqt3&redirect_uri=https%3A%2F%2Fclient.example%2Fcb");
/// ```
SOURCEMETA_CORE_OAUTH_EXPORT
auto oauth_build_authorization_url(const std::string_view endpoint,
                                   const OAuthAuthorizationRequest &request,
                                   std::string &sink) -> void;

/// @ingroup oauth
/// A non-owning view of an authorization response returned on the redirection
/// endpoint (RFC 6749 Section 4.1.2). Each field borrows from the parsed query
/// or the decode arena, so both must outlive the view, and an absent parameter
/// is an empty view. A present `error` marks a failure response (RFC 6749
/// Section 4.1.2.1).
struct OAuthAuthorizationResponse {
  /// The authorization code (RFC 6749 Section 4.1.2).
  std::string_view code;
  /// The state value echoed from the request (RFC 6749 Section 4.1.2).
  std::string_view state;
  /// The issuer identifier of the authorization server (RFC 9207 Section 2).
  std::string_view iss;
  /// The error code of a failure response (RFC 6749 Section 4.1.2.1).
  std::string_view error;
  /// The human-readable error description of a failure response (RFC 6749
  /// Section 4.1.2.1).
  std::string_view error_description;
  /// The URI of a human-readable error page for a failure response (RFC 6749
  /// Section 4.1.2.1).
  std::string_view error_uri;
};

/// @ingroup oauth
/// Parse the query of an authorization response (RFC 6749 Section 4.1.2) into
/// the result, returning whether the query is well formed. Each recognized
/// value is form-decoded (RFC 6749 Appendix B), borrowing from the input when
/// it carries no escape and otherwise from the storage arena, which the caller
/// owns and which is reused across parses. A duplicated recognized parameter is
/// a failure, and an unrecognized parameter is ignored. For example:
///
/// ```cpp
/// #include <sourcemeta/core/oauth.h>
/// #include <cassert>
/// #include <string>
///
/// std::string storage;
/// sourcemeta::core::OAuthAuthorizationResponse response;
/// assert(sourcemeta::core::oauth_parse_authorization_response(
///     "code=SplxlOBeZQQYbYS6WxSbIA&state=xyz", storage, response));
/// assert(response.code == "SplxlOBeZQQYbYS6WxSbIA");
/// assert(response.state == "xyz");
/// ```
SOURCEMETA_CORE_OAUTH_EXPORT
auto oauth_parse_authorization_response(const std::string_view query,
                                        std::string &storage,
                                        OAuthAuthorizationResponse &result)
    -> bool;

} // namespace sourcemeta::core

#endif
