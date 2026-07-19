#ifndef SOURCEMETA_CORE_OAUTH_PROFILE_H_
#define SOURCEMETA_CORE_OAUTH_PROFILE_H_

#ifndef SOURCEMETA_CORE_OAUTH_EXPORT
#include <sourcemeta/core/oauth_export.h>
#endif

#include <cstdint> // std::uint8_t

namespace sourcemeta::core {

/// @ingroup oauth
/// The behavioural profile a parser or validator runs under. `Strict` applies
/// the OAuth 2.1 and RFC 9700 hardening, and `Compatible` relaxes it for
/// RFC 6749 interoperability where a specification still permits the older
/// behaviour. For example:
///
/// ```cpp
/// #include <sourcemeta/core/oauth.h>
///
/// const auto profile{sourcemeta::core::OAuthProfile::Strict};
/// ```
enum class OAuthProfile : std::uint8_t {
  /// The default. Rejects the `plain` PKCE method, requires exact redirect URI
  /// matching, and refuses bearer tokens outside the `Authorization` header.
  Strict,
  /// Accepts the RFC 6749 behaviours the strict profile refuses, for
  /// interoperability with deployments that predate the hardening.
  Compatible
};

} // namespace sourcemeta::core

#endif
