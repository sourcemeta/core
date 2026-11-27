#include <sourcemeta/core/jose_sign.h>

#include <sourcemeta/core/crypto.h>

#include "jose_key.h"
#include "jose_switch.h"

#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view

namespace {

// The octets of a signature, with none where the backend declined to make one.
// Holding that refusal here is what keeps it out of the width check below,
// which every backend can be shown reaching. A backend with no system provider
// derives its nonce from the key and the message (RFC 6979 Section 3.2) rather
// than drawing one, so it refuses only where the key is of the wrong kind,
// which is settled before the call, or where two hundred and fifty six
// successive derived nonces all fall outside the curve order, which no input
// can be built to arrange
auto ecdsa_signature_octets(const std::optional<std::string> &signature)
    -> std::string_view {
  if (!signature.has_value()) {
    return {};
  }

  return signature.value();
}

} // namespace

namespace sourcemeta::core {

auto jws_sign(const JWSAlgorithm algorithm,
              const std::string_view signing_input, const JWKPrivate &key,
              const RandomGenerator &generator) -> std::optional<std::string> {
  // A key that names an algorithm must not contradict the one in use (RFC 7517
  // Section 4.4)
  if (key.algorithm().has_value() && key.algorithm().value() != algorithm) {
    return std::nullopt;
  }

  const auto *private_key{key.private_key()};

  // Asked of every algorithm, since the absence of a hash marks the one that
  // computes its own, and the absence of a curve the families that name none
  const auto hash{jws_hash_for(algorithm)};
  const auto curve{jws_curve_for(algorithm)};

  switch (algorithm) {
    // The asymmetric key material is parsed into a reusable platform key when
    // the key is constructed, so an absent one is material that never formed a
    // valid key
    SOURCEMETA_CORE_JOSE_FIRST_CASE(JWSAlgorithm::RS256)
    case JWSAlgorithm::RS384:
    case JWSAlgorithm::RS512:
      if (private_key == nullptr || key.type() != JWKPrivate::Type::RSA) {
        return std::nullopt;
      }

      return rsassa_pkcs1_v15_sign(*private_key, hash.value(), signing_input);
    case JWSAlgorithm::PS256:
    case JWSAlgorithm::PS384:
    case JWSAlgorithm::PS512:
      if (private_key == nullptr || key.type() != JWKPrivate::Type::RSA) {
        return std::nullopt;
      }

      return rsassa_pss_sign(*private_key, hash.value(), signing_input,
                             generator);
    // Each ECDSA algorithm is pinned to exactly one curve (RFC 7518 Section
    // 3.4). The raw signature width is twice the curve field width, so it pins
    // the key's curve to the algorithm, including for a key parsed from PEM
    // which carries no curve name
    case JWSAlgorithm::ES256:
    case JWSAlgorithm::ES384:
    case JWSAlgorithm::ES512: {
      if (private_key == nullptr ||
          key.type() != JWKPrivate::Type::EllipticCurve) {
        return std::nullopt;
      }

      auto signature{ecdsa_sign(*private_key, hash.value(), signing_input)};
      // A refusal carries no octets, so the width the algorithm pins turns away
      // both that and a signature the curve does not match (RFC 7518 Section
      // 3.4)
      if (ecdsa_signature_octets(signature).size() !=
          jws_ecdsa_signature_bytes(curve.value())) {
        return std::nullopt;
      }

      return signature;
    }
    // The Edwards-curve algorithm names one of two curves through the key
    // rather than the algorithm (RFC 8037 Section 3.1), and the key fixes the
    // curve when it is parsed
    case JWSAlgorithm::EdDSA:
      if (private_key == nullptr ||
          key.type() != JWKPrivate::Type::OctetKeyPair) {
        return std::nullopt;
      }

      return eddsa_sign(*private_key, signing_input);
    // The symmetric algorithms authenticate with the raw secret rather than a
    // parsed platform key, and the secret must be at least as large as the hash
    // output (RFC 7518 Section 3.2)
    case JWSAlgorithm::HS256: {
      if (key.type() != JWKPrivate::Type::Octet ||
          key.secret().size() < jws_hmac_minimum_secret_bytes(hash.value())) {
        return std::nullopt;
      }

      const auto digest{hmac_sha256_digest(key.secret(), signing_input)};
      return std::string{reinterpret_cast<const char *>(digest.data()),
                         digest.size()};
    }
    case JWSAlgorithm::HS384: {
      if (key.type() != JWKPrivate::Type::Octet ||
          key.secret().size() < jws_hmac_minimum_secret_bytes(hash.value())) {
        return std::nullopt;
      }

      const auto digest{hmac_sha384_digest(key.secret(), signing_input)};
      return std::string{reinterpret_cast<const char *>(digest.data()),
                         digest.size()};
    }
    case JWSAlgorithm::HS512:
      break;
  }

  if (key.type() != JWKPrivate::Type::Octet ||
      key.secret().size() < jws_hmac_minimum_secret_bytes(hash.value())) {
    return std::nullopt;
  }

  const auto digest{hmac_sha512_digest(key.secret(), signing_input)};
  return std::string{reinterpret_cast<const char *>(digest.data()),
                     digest.size()};
}

} // namespace sourcemeta::core
