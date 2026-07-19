#ifndef SOURCEMETA_CORE_OAUTH_BEARER_H_
#define SOURCEMETA_CORE_OAUTH_BEARER_H_

#ifndef SOURCEMETA_CORE_OAUTH_EXPORT
#include <sourcemeta/core/oauth_export.h>
#endif

#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::core {

/// @ingroup oauth
/// Append a `Bearer` credential (RFC 6750 Section 2.1) for an access token to
/// the sink, returning whether the token is a well-formed `b64token`. Nothing
/// is appended when it is not. The result carries the access token, which is
/// secret, so a caller that keeps it should hold it in wiping storage. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/oauth.h>
/// #include <cassert>
/// #include <string>
///
/// std::string header;
/// assert(sourcemeta::core::oauth_bearer_header("mF_9.B5f-4.1JqM", header));
/// assert(header == "Bearer mF_9.B5f-4.1JqM");
/// ```
SOURCEMETA_CORE_OAUTH_EXPORT
auto oauth_bearer_header(const std::string_view token, std::string &sink)
    -> bool;

/// @ingroup oauth
/// Find the value of one authentication parameter within the challenge for a
/// scheme in a `WWW-Authenticate` header value (RFC 7235 Section 4.1),
/// returning no value when the scheme or parameter is absent, or the header is
/// malformed before the parameter is reached. The scheme and parameter name are
/// matched case insensitively. A
/// quoted-string value is unescaped (RFC 9110 Section 5.6.4) into the storage
/// arena and the result borrows from it, otherwise the result borrows from the
/// input. The full grammar is parsed so that adjacent challenges and parameters
/// are told apart correctly. For example:
///
/// ```cpp
/// #include <sourcemeta/core/oauth.h>
/// #include <cassert>
/// #include <string>
///
/// std::string storage;
/// const auto realm{sourcemeta::core::oauth_challenge_parameter(
///     R"(Bearer realm="example", error="invalid_token")", "Bearer", "realm",
///     storage)};
/// assert(realm.has_value());
/// assert(realm.value() == "example");
/// ```
SOURCEMETA_CORE_OAUTH_EXPORT
auto oauth_challenge_parameter(const std::string_view header,
                               const std::string_view scheme,
                               const std::string_view name,
                               std::string &storage)
    -> std::optional<std::string_view>;

} // namespace sourcemeta::core

#endif
