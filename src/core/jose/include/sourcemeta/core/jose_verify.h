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
#include <optional>    // std::optional
#include <string_view> // std::string_view

namespace sourcemeta::core {

/// @ingroup jose
/// The claim validation errors that `jwt_check_claims` can return, one per
/// check it performs rather than an exhaustive list of registered claims.
enum class JWTClaimError : std::uint8_t {
  Issuer,
  Subject,
  Audience,
  Expiration,
  NotBefore,
  IssuedAt
};

/// @ingroup jose
/// Validate the registered claims of a JSON Web Token against the expected
/// issuer and audience at a given time, returning the first failing check or no
/// value when every check passes. The expiration claim is required (RFC 9068
/// Section 2.2), and the subject is checked only when an expected value is
/// supplied. For example:
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
/// const auto error{sourcemeta::core::jwt_check_claims(
///     token.value(), "acme", "client",
///     std::chrono::system_clock::from_time_t(1500000000))};
/// assert(!error.has_value());
/// ```
SOURCEMETA_CORE_JOSE_EXPORT
auto jwt_check_claims(
    const JWT &token, const std::string_view expected_issuer,
    const std::string_view expected_audience,
    const std::chrono::system_clock::time_point now,
    const std::chrono::seconds clock_skew = std::chrono::seconds{0},
    const std::optional<std::string_view> expected_subject = std::nullopt)
    -> std::optional<JWTClaimError>;

} // namespace sourcemeta::core

#endif
