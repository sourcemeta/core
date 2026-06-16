#include <sourcemeta/core/jose_verify.h>

#include <chrono>      // std::chrono::seconds, std::chrono::system_clock
#include <expected>    // std::expected, std::unexpected
#include <string_view> // std::string_view

namespace sourcemeta::core {

auto jwt_check_claims(const JWT &token, const std::string_view expected_issuer,
                      const std::string_view expected_audience,
                      const std::chrono::system_clock::time_point now,
                      const std::chrono::seconds clock_skew)
    -> std::expected<void, JWTClaimError> {
  // The issuer must be present and match the expected value (RFC 7519 Section
  // 4.1.1)
  const auto issuer{token.issuer()};
  if (!issuer.has_value() || issuer.value() != expected_issuer) {
    return std::unexpected{JWTClaimError::Issuer};
  }

  // The audience must be present and contain the expected value (RFC 7519
  // Section 4.1.3)
  if (!token.has_audience(expected_audience)) {
    return std::unexpected{JWTClaimError::Audience};
  }

  // A bearer credential without an expiry is not acceptable for authentication,
  // so the claim is required here even though RFC 7519 makes it optional in
  // general (RFC 9068 Section 2.2)
  const auto expires_at{token.expires_at()};
  if (!expires_at.has_value() || now >= expires_at.value() + clock_skew) {
    return std::unexpected{JWTClaimError::Expiration};
  }

  // The not-before time, when present, must not be in the future (RFC 7519
  // Section 4.1.5)
  const auto not_before{token.not_before()};
  if (not_before.has_value() && now < not_before.value() - clock_skew) {
    return std::unexpected{JWTClaimError::NotBefore};
  }

  // The issued-at time, when present, must not be in the future (RFC 7519
  // Section 4.1.6)
  const auto issued_at{token.issued_at()};
  if (issued_at.has_value() && now < issued_at.value() - clock_skew) {
    return std::unexpected{JWTClaimError::IssuedAt};
  }

  return {};
}

} // namespace sourcemeta::core
