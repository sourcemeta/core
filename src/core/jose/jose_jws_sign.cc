#include <sourcemeta/core/jose_sign.h>

#include <sourcemeta/core/crypto.h>

#include "jose_key.h"

#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::unreachable

namespace sourcemeta::core {

auto jws_sign(const JWSAlgorithm algorithm,
              const std::string_view signing_input, const JWKPrivate &key)
    -> std::optional<std::string> {
  // A key that names an algorithm must not contradict the one in use (RFC 7517
  // Section 4.4)
  if (key.algorithm().has_value() && key.algorithm().value() != algorithm) {
    return std::nullopt;
  }

  // The key material is parsed into a reusable platform key when the key is
  // constructed, so an absent one is material that never formed a valid key
  const auto *private_key{key.private_key()};
  if (private_key == nullptr) {
    return std::nullopt;
  }

  switch (algorithm) {
    case JWSAlgorithm::RS256:
    case JWSAlgorithm::RS384:
    case JWSAlgorithm::RS512:
      if (key.type() != JWKPrivate::Type::RSA) {
        return std::nullopt;
      }

      return rsassa_pkcs1_v15_sign(*private_key, jws_hash_for(algorithm),
                                   signing_input);
    case JWSAlgorithm::PS256:
    case JWSAlgorithm::PS384:
    case JWSAlgorithm::PS512:
      if (key.type() != JWKPrivate::Type::RSA) {
        return std::nullopt;
      }

      return rsassa_pss_sign(*private_key, jws_hash_for(algorithm),
                             signing_input);
    // Each ECDSA algorithm is pinned to exactly one curve (RFC 7518 Section
    // 3.4), so the key's curve is checked independently of any algorithm it
    // declares
    case JWSAlgorithm::ES256:
      if (key.type() != JWKPrivate::Type::EllipticCurve ||
          key.curve() != "P-256") {
        return std::nullopt;
      }

      return ecdsa_sign(*private_key, SignatureHashFunction::SHA256,
                        signing_input);
    case JWSAlgorithm::ES384:
      if (key.type() != JWKPrivate::Type::EllipticCurve ||
          key.curve() != "P-384") {
        return std::nullopt;
      }

      return ecdsa_sign(*private_key, SignatureHashFunction::SHA384,
                        signing_input);
    case JWSAlgorithm::ES512:
      if (key.type() != JWKPrivate::Type::EllipticCurve ||
          key.curve() != "P-521") {
        return std::nullopt;
      }

      return ecdsa_sign(*private_key, SignatureHashFunction::SHA512,
                        signing_input);
    // The Edwards-curve algorithm names one of two curves through the key
    // rather than the algorithm (RFC 8037 Section 3.1), and the key fixes the
    // curve when it is parsed
    case JWSAlgorithm::EdDSA:
      if (key.type() != JWKPrivate::Type::OctetKeyPair) {
        return std::nullopt;
      }

      return eddsa_sign(*private_key, signing_input);
  }

  std::unreachable();
}

} // namespace sourcemeta::core
