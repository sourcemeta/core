#include <sourcemeta/core/crypto_ecdh.h>

#include <CoreFoundation/CoreFoundation.h> // CF*, kCF*
#include <Security/Security.h>             // Sec*, kSec*

#include <cstddef>  // std::size_t
#include <optional> // std::optional, std::nullopt
#include <string>   // std::string

namespace sourcemeta::core {

// The layout matches the definitions in the sibling Apple key backends, since
// each translation unit that reads a key redeclares its file-private members
struct PublicKey::Internal {
  PublicKey::Type kind;
  SecKeyRef key;
  std::string modulus;
  std::size_t field_bytes;
  std::string edwards_point;
  EdwardsCurve edwards_curve;
};

struct PrivateKey::Internal {
  PrivateKey::Type kind;
  SecKeyRef key;
  std::size_t field_bytes;
  std::string edwards_seed;
  EdwardsCurve edwards_curve;
  bool rsa_pss_restricted{false};
};

auto ecdh_derive(const PrivateKey &private_key, const PublicKey &public_key)
    -> std::optional<std::string> {
  const auto *const private_internal{private_key.internal()};
  const auto *const public_internal{public_key.internal()};
  if (private_internal == nullptr || public_internal == nullptr ||
      private_internal->kind != PrivateKey::Type::EllipticCurve ||
      public_internal->kind != PublicKey::Type::EllipticCurve ||
      private_internal->field_bytes != public_internal->field_bytes) {
    return std::nullopt;
  }

  auto parameters{CFDictionaryCreate(kCFAllocatorDefault, nullptr, nullptr, 0,
                                     &kCFTypeDictionaryKeyCallBacks,
                                     &kCFTypeDictionaryValueCallBacks)};
  if (parameters == nullptr) {
    return std::nullopt;
  }

  // The standard exchange returns the agreed point x coordinate, and the peer
  // key was validated on the curve when it was parsed
  auto shared{SecKeyCopyKeyExchangeResult(
      private_internal->key, kSecKeyAlgorithmECDHKeyExchangeStandard,
      public_internal->key, parameters, nullptr)};
  CFRelease(parameters);
  if (shared == nullptr) {
    return std::nullopt;
  }

  std::string result{reinterpret_cast<const char *>(CFDataGetBytePtr(shared)),
                     static_cast<std::size_t>(CFDataGetLength(shared))};
  CFRelease(shared);
  return result;
}

} // namespace sourcemeta::core
