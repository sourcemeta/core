#ifndef SOURCEMETA_CORE_JOSE_VERIFY_H_
#define SOURCEMETA_CORE_JOSE_VERIFY_H_

#ifndef SOURCEMETA_CORE_JOSE_EXPORT
#include <sourcemeta/core/jose_export.h>
#endif

// NOLINTBEGIN(misc-include-cleaner)
#include <sourcemeta/core/jose_jwt.h>
// NOLINTEND(misc-include-cleaner)

#include <chrono>      // std::chrono::seconds, std::chrono::system_clock
#include <cstdint>     // std::uint8_t
#include <expected>    // std::expected
#include <string_view> // std::string_view

namespace sourcemeta::core {

/// @ingroup jose
/// The registered claim checks that a JSON Web Token can fail (RFC 7519 Section
/// 4.1).
enum class JWTClaimError : std::uint8_t {
  Issuer,
  Audience,
  Expiration,
  NotBefore,
  IssuedAt
};

/// @ingroup jose
/// Validate the registered claims of a JSON Web Token against the expected
/// issuer and audience at a given time, returning the first failing check. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/jose.h>
/// #include <cassert>
/// #include <chrono>
/// #include <string>
///
/// const std::string input{
///     "eyJhbGciOiJSUzI1NiJ9."
///     "eyJpc3MiOiJhY21lIiwiYXVkIjoiY2xpZW50IiwiZXhwIjoyMDAwMDAwMDAwfQ.c2ln"};
/// const auto token{sourcemeta::core::JWT::from(input)};
/// assert(token.has_value());
/// const auto result{sourcemeta::core::jwt_check_claims(
///     token.value(), "acme", "client",
///     std::chrono::system_clock::from_time_t(1500000000))};
/// assert(result.has_value());
/// ```
SOURCEMETA_CORE_JOSE_EXPORT
auto jwt_check_claims(
    const JWT &token, const std::string_view expected_issuer,
    const std::string_view expected_audience,
    const std::chrono::system_clock::time_point now,
    const std::chrono::seconds clock_skew = std::chrono::seconds{0})
    -> std::expected<void, JWTClaimError>;

} // namespace sourcemeta::core

#endif
