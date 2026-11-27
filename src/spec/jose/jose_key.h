#ifndef SOURCEMETA_CORE_JOSE_KEY_H_
#define SOURCEMETA_CORE_JOSE_KEY_H_

// Curve and algorithm mappings shared by the public JWK and the private
// JWKPrivate parsing paths

#include "jose_switch.h"

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/jose_algorithm.h>
#include <sourcemeta/core/jose_jwk.h>
#include <sourcemeta/core/jose_jwk_private.h>

#include <bit>         // std::countl_zero
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t
#include <optional>    // std::optional, std::nullopt
#include <string_view> // std::string_view
#include <utility>     // std::unreachable

namespace sourcemeta::core {

// The kind of key, shared by the public and private parsing paths so that the
// algorithm matching does not need to be written twice
enum class JWKKind : std::uint8_t { RSA, EllipticCurve, OctetKeyPair, Octet };

// The kind a key of either flavour stands for, so that what depends on the
// kind alone is written once rather than per flavour
inline auto to_jwk_kind(const JWK::Type type) noexcept -> JWKKind {
  switch (type) {
    SOURCEMETA_CORE_JOSE_FIRST_CASE(JWK::Type::RSA)
    return JWKKind::RSA;
    case JWK::Type::EllipticCurve:
      return JWKKind::EllipticCurve;
    case JWK::Type::OctetKeyPair:
      return JWKKind::OctetKeyPair;
    case JWK::Type::Octet:
      return JWKKind::Octet;
  }

  SOURCEMETA_CORE_JOSE_NO_OTHER_VALUE
}

inline auto to_jwk_kind(const JWKPrivate::Type type) noexcept -> JWKKind {
  switch (type) {
    SOURCEMETA_CORE_JOSE_FIRST_CASE(JWKPrivate::Type::RSA)
    return JWKKind::RSA;
    case JWKPrivate::Type::EllipticCurve:
      return JWKKind::EllipticCurve;
    case JWKPrivate::Type::OctetKeyPair:
      return JWKKind::OctetKeyPair;
    case JWKPrivate::Type::Octet:
      return JWKKind::Octet;
  }

  SOURCEMETA_CORE_JOSE_NO_OTHER_VALUE
}

// RFC 7518 Section 3.3: "A key of size 2048 bits or larger MUST be used with"
// the RSASSA algorithms, so a smaller modulus is refused before a key is built
inline constexpr std::size_t MINIMUM_RSA_MODULUS_BITS{2048};

inline auto jwk_rsa_modulus_is_allowed(const std::string_view modulus) noexcept
    -> bool {
  std::size_t offset{0};
  while (offset < modulus.size() &&
         static_cast<std::uint8_t>(modulus[offset]) == 0) {
    offset += 1;
  }

  if (offset >= modulus.size()) {
    return false;
  }

  const auto leading{static_cast<std::uint8_t>(modulus[offset])};
  const auto bits{((modulus.size() - offset - 1) * 8) +
                  (8 - static_cast<std::size_t>(std::countl_zero(leading)))};
  return bits >= MINIMUM_RSA_MODULUS_BITS;
}

// The coordinate octet length is fixed per curve (RFC 7518 Section 6.2.1.2)
inline auto jwk_ec_coordinate_bytes(const std::string_view curve)
    -> std::optional<std::size_t> {
  if (curve == "P-256") {
    return 32;
  }
  if (curve == "P-384") {
    return 48;
  }
  if (curve == "P-521") {
    return 66;
  }
  return std::nullopt;
}

// The key octet length is fixed per Edwards curve (RFC 8032 Sections 5.1.5 and
// 5.2.5), and the private seed shares it (RFC 8037 Section 2)
inline auto jwk_okp_key_bytes(const std::string_view curve)
    -> std::optional<std::size_t> {
  if (curve == "Ed25519") {
    return 32;
  }
  if (curve == "Ed448") {
    return 57;
  }
  return std::nullopt;
}

// Both mappings are only reached after the curve has been validated above
inline auto jwk_to_elliptic_curve(const std::string_view curve) noexcept
    -> EllipticCurve {
  if (curve == "P-256") {
    return EllipticCurve::P256;
  }
  if (curve == "P-384") {
    return EllipticCurve::P384;
  }
  return EllipticCurve::P521;
}

inline auto jwk_to_edwards_curve(const std::string_view curve) noexcept
    -> EdwardsCurve {
  if (curve == "Ed25519") {
    return EdwardsCurve::Ed25519;
  }
  return EdwardsCurve::Ed448;
}

// The JWK curve name each elliptic curve carries (RFC 7518 Section 6.2.1.1),
// the reverse of the parsing mapping, so a key built without a curve name can
// recover it from the parsed material
inline auto elliptic_curve_to_jwk(const EllipticCurve curve) noexcept
    -> std::string_view {
  switch (curve) {
    SOURCEMETA_CORE_JOSE_FIRST_CASE(EllipticCurve::P256)
    return "P-256";
    case EllipticCurve::P384:
      return "P-384";
    case EllipticCurve::P521:
      return "P-521";
  }

  SOURCEMETA_CORE_JOSE_NO_OTHER_VALUE
}

// The JWK curve name each Edwards curve carries (RFC 8037 Section 2), the
// reverse of the parsing mapping
inline auto edwards_curve_to_jwk(const EdwardsCurve curve) noexcept
    -> std::string_view {
  switch (curve) {
    SOURCEMETA_CORE_JOSE_FIRST_CASE(EdwardsCurve::Ed25519)
    return "Ed25519";
    case EdwardsCurve::Ed448:
      return "Ed448";
  }

  SOURCEMETA_CORE_JOSE_NO_OTHER_VALUE
}

// The RSA algorithms only require an RSA key, each ECDSA algorithm is tied to a
// specific curve (RFC 7518 Section 3.1), and the Edwards-curve algorithm
// requires an octet key pair of either curve (RFC 8037 Section 3.1)
inline auto jwk_algorithm_matches_key(const JWSAlgorithm algorithm,
                                      const JWKKind kind,
                                      const std::string_view curve) -> bool {
  switch (algorithm) {
    SOURCEMETA_CORE_JOSE_FIRST_CASE(JWSAlgorithm::RS256)
    case JWSAlgorithm::RS384:
    case JWSAlgorithm::RS512:
    case JWSAlgorithm::PS256:
    case JWSAlgorithm::PS384:
    case JWSAlgorithm::PS512:
      return kind == JWKKind::RSA;
    case JWSAlgorithm::ES256:
      return kind == JWKKind::EllipticCurve && curve == "P-256";
    case JWSAlgorithm::ES384:
      return kind == JWKKind::EllipticCurve && curve == "P-384";
    case JWSAlgorithm::ES512:
      return kind == JWKKind::EllipticCurve && curve == "P-521";
    case JWSAlgorithm::EdDSA:
      return kind == JWKKind::OctetKeyPair;
    case JWSAlgorithm::HS256:
    case JWSAlgorithm::HS384:
    case JWSAlgorithm::HS512:
      break;
  }

  return kind == JWKKind::Octet;
}

// The hash an algorithm names over the signing input (RFC 7518 Sections 3.1
// and 3.2). The Edwards-curve algorithm computes its own internally (RFC 8032
// Sections 5.1.6 and 5.2.6) and so names none, which is what asking every
// algorithm here reports rather than leaving to a caller to know
inline auto jws_hash_for(const JWSAlgorithm algorithm) noexcept
    -> std::optional<SignatureHashFunction> {
  switch (algorithm) {
    SOURCEMETA_CORE_JOSE_FIRST_CASE(JWSAlgorithm::RS256)
    case JWSAlgorithm::PS256:
    case JWSAlgorithm::ES256:
    case JWSAlgorithm::HS256:
      return SignatureHashFunction::SHA256;
    case JWSAlgorithm::RS384:
    case JWSAlgorithm::PS384:
    case JWSAlgorithm::ES384:
    case JWSAlgorithm::HS384:
      return SignatureHashFunction::SHA384;
    case JWSAlgorithm::RS512:
    case JWSAlgorithm::PS512:
    case JWSAlgorithm::ES512:
    case JWSAlgorithm::HS512:
      return SignatureHashFunction::SHA512;
    case JWSAlgorithm::EdDSA:
      return std::nullopt;
  }

  SOURCEMETA_CORE_JOSE_NO_OTHER_VALUE
}

// The curve each ECDSA algorithm is pinned to (RFC 7518 Section 3.4). Every
// other algorithm names none, which is what separates the ECDSA family from
// the rest without a second question being asked of it
inline auto jws_curve_for(const JWSAlgorithm algorithm) noexcept
    -> std::optional<EllipticCurve> {
  switch (algorithm) {
    SOURCEMETA_CORE_JOSE_FIRST_CASE(JWSAlgorithm::ES256)
    return EllipticCurve::P256;
    case JWSAlgorithm::ES384:
      return EllipticCurve::P384;
    case JWSAlgorithm::ES512:
      return EllipticCurve::P521;
    case JWSAlgorithm::RS256:
    case JWSAlgorithm::RS384:
    case JWSAlgorithm::RS512:
    case JWSAlgorithm::PS256:
    case JWSAlgorithm::PS384:
    case JWSAlgorithm::PS512:
    case JWSAlgorithm::EdDSA:
    case JWSAlgorithm::HS256:
    case JWSAlgorithm::HS384:
    case JWSAlgorithm::HS512:
      return std::nullopt;
  }

  SOURCEMETA_CORE_JOSE_NO_OTHER_VALUE
}

// RFC 7518 Section 3.2 states the bound in terms of the hash rather than the
// algorithm: "A key of the same size as the hash output (for instance, 256
// bits for HS256) or larger MUST be used with this algorithm". Taking the hash
// is what lets every size it names be asked for, since an algorithm carries
// one of three hashes and the caller already knows which
inline auto jws_hmac_minimum_secret_bytes(const SignatureHashFunction hash)
    -> std::size_t {
  switch (hash) {
    SOURCEMETA_CORE_JOSE_FIRST_CASE(SignatureHashFunction::SHA256)
    return 32;
    case SignatureHashFunction::SHA384:
      return 48;
    case SignatureHashFunction::SHA512:
      return 64;
  }

  SOURCEMETA_CORE_JOSE_NO_OTHER_VALUE
}

// The raw ECDSA signature is twice the curve field width (RFC 7518 Section
// 3.4), so the curve is what the width is asked of. An algorithm is pinned to
// exactly one curve, which the caller names alongside the hash
inline auto jws_ecdsa_signature_bytes(const EllipticCurve curve)
    -> std::size_t {
  switch (curve) {
    SOURCEMETA_CORE_JOSE_FIRST_CASE(EllipticCurve::P256)
    return 64;
    case EllipticCurve::P384:
      return 96;
    case EllipticCurve::P521:
      return 132;
  }

  SOURCEMETA_CORE_JOSE_NO_OTHER_VALUE
}

} // namespace sourcemeta::core

#endif
