#ifndef SOURCEMETA_CORE_CRYPTO_APPLE_H_
#define SOURCEMETA_CORE_CRYPTO_APPLE_H_

// Security-framework key export helpers shared by the Apple signing and
// verification backends, so the curve mapping and the external representation
// handling stay single-sourced

#include <sourcemeta/core/crypto_verify.h>

#include <CoreFoundation/CoreFoundation.h> // CF*, kCF*
#include <Security/Security.h>             // Sec*, kSec*

#include <cstddef>  // std::size_t
#include <optional> // std::optional, std::nullopt
#include <string>   // std::string

namespace sourcemeta::core {

inline auto ec_curve_from_field_bytes(const std::size_t field_bytes) noexcept
    -> std::optional<EllipticCurve> {
  switch (field_bytes) {
    case 32:
      return EllipticCurve::P256;
    case 48:
      return EllipticCurve::P384;
    case 66:
      return EllipticCurve::P521;
    default:
      return std::nullopt;
  }
}

// Copy the platform's external representation of a key, the PKCS#1 structure
// for RSA and the X9.63 uncompressed point for elliptic curve keys
inline auto copy_external_representation(SecKeyRef key)
    -> std::optional<std::string> {
  CFErrorRef error{nullptr};
  auto data{SecKeyCopyExternalRepresentation(key, &error)};
  if (data == nullptr) {
    if (error != nullptr) {
      CFRelease(error);
    }

    return std::nullopt;
  }

  std::string result{reinterpret_cast<const char *>(CFDataGetBytePtr(data)),
                     static_cast<std::size_t>(CFDataGetLength(data))};
  CFRelease(data);
  return result;
}

} // namespace sourcemeta::core

#endif
