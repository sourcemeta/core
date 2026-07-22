#ifndef SOURCEMETA_CORE_JOSE_ALGORITHM_H_
#define SOURCEMETA_CORE_JOSE_ALGORITHM_H_

#ifndef SOURCEMETA_CORE_JOSE_EXPORT
#include <sourcemeta/core/jose_export.h>
#endif

#include <cstdint>     // std::uint8_t, std::uint16_t
#include <optional>    // std::optional
#include <string_view> // std::string_view

namespace sourcemeta::core {

/// @ingroup jose
/// The JSON Web Signature algorithms from RFC 7518 Section 3.1 and the
/// Edwards-curve algorithm from RFC 8037 Section 3.1. The null algorithm is
/// intentionally absent. Each algorithm demands a key of exactly one family,
/// the symmetric algorithms an octet sequence and the asymmetric ones their own
/// key type, which is what keeps algorithm confusion attacks unexploitable.
enum class JWSAlgorithm : std::uint8_t {
  /// RSASSA-PKCS1-v1_5 using SHA-256.
  RS256,
  /// RSASSA-PKCS1-v1_5 using SHA-384.
  RS384,
  /// RSASSA-PKCS1-v1_5 using SHA-512.
  RS512,
  /// RSASSA-PSS using SHA-256 and MGF1 with SHA-256.
  PS256,
  /// RSASSA-PSS using SHA-384 and MGF1 with SHA-384.
  PS384,
  /// RSASSA-PSS using SHA-512 and MGF1 with SHA-512.
  PS512,
  /// ECDSA using the NIST P-256 curve and SHA-256.
  ES256,
  /// ECDSA using the NIST P-384 curve and SHA-384.
  ES384,
  /// ECDSA using the NIST P-521 curve and SHA-512.
  ES512,
  /// Edwards-curve Digital Signature Algorithm.
  EdDSA,
  /// HMAC using SHA-256.
  HS256,
  /// HMAC using SHA-384.
  HS384,
  /// HMAC using SHA-512.
  HS512
};

/// @ingroup jose
/// Map a JSON Web Signature `alg` value to its algorithm, returning no value
/// for any unrecognized name. For example:
///
/// ```cpp
/// #include <sourcemeta/core/jose.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::to_jws_algorithm("RS256").has_value());
/// assert(!sourcemeta::core::to_jws_algorithm("none").has_value());
/// ```
SOURCEMETA_CORE_JOSE_EXPORT
auto to_jws_algorithm(const std::string_view value) noexcept
    -> std::optional<JWSAlgorithm>;

/// @ingroup jose
/// Map a JSON Web Signature algorithm to its `alg` value, the inverse of
/// parsing (RFC 7515 Section 4.1.1). For example:
///
/// ```cpp
/// #include <sourcemeta/core/jose.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::jws_algorithm_name(
///            sourcemeta::core::JWSAlgorithm::ES256) == "ES256");
/// ```
SOURCEMETA_CORE_JOSE_EXPORT
auto jws_algorithm_name(const JWSAlgorithm algorithm) noexcept
    -> std::string_view;

/// @ingroup jose
/// Whether an algorithm is an asymmetric digital signature algorithm rather
/// than a symmetric message authentication code (RFC 7518 Section 3.1). For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/jose.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::jws_algorithm_is_asymmetric(
///            sourcemeta::core::JWSAlgorithm::ES256));
/// assert(!sourcemeta::core::jws_algorithm_is_asymmetric(
///            sourcemeta::core::JWSAlgorithm::HS256));
/// ```
SOURCEMETA_CORE_JOSE_EXPORT
auto jws_algorithm_is_asymmetric(const JWSAlgorithm algorithm) noexcept -> bool;

/// @ingroup jose
/// The size, in bits, of the digest each signature algorithm is defined over
/// (RFC 7518 Section 3.1), so that derived hash claims such as OpenID
/// Connect's `at_hash` and `c_hash` select their digest by table rather than
/// by slicing the algorithm name. The Edwards-curve algorithm names no digest
/// and OpenID Connect leaves the choice unspecified, so it is pinned to
/// SHA-512, the convention deployed implementations follow for Ed25519, whose
/// signature scheme is internally defined over that hash (RFC 8032 Section
/// 5.1). Hash claims minted over Ed448 by a SHAKE256-based implementation
/// need curve-specific handling that no algorithm-only table can provide. For
/// example:
///
/// ```cpp
/// #include <sourcemeta/core/jose.h>
/// #include <cassert>
///
/// assert(sourcemeta::core::jws_algorithm_digest_bits(
///            sourcemeta::core::JWSAlgorithm::ES384) == 384);
/// ```
SOURCEMETA_CORE_JOSE_EXPORT
auto jws_algorithm_digest_bits(const JWSAlgorithm algorithm) noexcept
    -> std::uint16_t;

} // namespace sourcemeta::core

#endif
