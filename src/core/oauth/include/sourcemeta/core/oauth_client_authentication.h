#ifndef SOURCEMETA_CORE_OAUTH_CLIENT_AUTHENTICATION_H_
#define SOURCEMETA_CORE_OAUTH_CLIENT_AUTHENTICATION_H_

#ifndef SOURCEMETA_CORE_OAUTH_EXPORT
#include <sourcemeta/core/oauth_export.h>
#endif

#include <sourcemeta/core/crypto.h>

#include <string_view> // std::string_view

namespace sourcemeta::core {

/// @ingroup oauth
/// Append an HTTP `Basic` authentication credential (RFC 6749 Section 2.3.1) to
/// the sink, for use as an `Authorization` header value. The client identifier
/// and secret are each percent-encoded, joined with a colon, and Base64
/// encoded. The credential is secret, so the sink is a wiping string, and it is
/// appended to and never cleared. For example:
///
/// ```cpp
/// #include <sourcemeta/core/oauth.h>
/// #include <cassert>
///
/// sourcemeta::core::SecureString header;
/// sourcemeta::core::oauth_client_secret_basic("s6BhdRkqt3", "gX1fBat3bV",
///                                             header);
/// assert(header == "Basic czZCaGRSa3F0MzpnWDFmQmF0M2JW");
/// ```
SOURCEMETA_CORE_OAUTH_EXPORT
auto oauth_client_secret_basic(const std::string_view client_id,
                               const std::string_view client_secret,
                               SecureString &sink) -> void;

/// @ingroup oauth
/// Append the `client_secret_post` client authentication parameters (RFC 6749
/// Section 2.3.1) to a token request body. Both the identifier and the secret
/// are emitted, so this composes into the same sink as a grant builder. The
/// body carries a secret, so the sink is a wiping string, and it is appended to
/// and never cleared. For example:
///
/// ```cpp
/// #include <sourcemeta/core/oauth.h>
/// #include <cassert>
///
/// sourcemeta::core::SecureString body;
/// sourcemeta::core::oauth_client_secret_post("id", "secret", body);
/// assert(body == "client_id=id&client_secret=secret");
/// ```
SOURCEMETA_CORE_OAUTH_EXPORT
auto oauth_client_secret_post(const std::string_view client_id,
                              const std::string_view client_secret,
                              SecureString &sink) -> void;

/// @ingroup oauth
/// Append a public client identification parameter (RFC 6749 Section 3.2.1) to
/// a token request body, emitting the identifier alone with no secret. This
/// composes into the same sink as a grant builder. The sink is appended to and
/// never cleared. For example:
///
/// ```cpp
/// #include <sourcemeta/core/oauth.h>
/// #include <cassert>
///
/// sourcemeta::core::SecureString body;
/// sourcemeta::core::oauth_client_id_only("s6BhdRkqt3", body);
/// assert(body == "client_id=s6BhdRkqt3");
/// ```
SOURCEMETA_CORE_OAUTH_EXPORT
auto oauth_client_id_only(const std::string_view client_id, SecureString &sink)
    -> void;

} // namespace sourcemeta::core

#endif
