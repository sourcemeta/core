#include <sourcemeta/core/crypto_verify.h>

#include "crypto_eddsa.h"
#include "crypto_eddsa_apple.h"
#include "crypto_helpers.h"

#include <string_view> // std::string_view
#include <utility>     // std::unreachable

// The Apple Security framework exposes no EdDSA primitive through its C API.
// Ed25519 is verified through CryptoKit by way of an Objective-C++ and Swift
// bridge, while Ed448, which CryptoKit does not provide, falls back to the
// reference implementation

namespace sourcemeta::core {

auto eddsa_verify(const EdwardsCurve curve, const std::string_view public_key,
                  const std::string_view message,
                  const std::string_view signature) -> bool {
  // Reject a key or signature of the wrong length before touching the backend,
  // as the other backends do
  if (public_key.size() != eddsa_public_key_bytes(curve) ||
      signature.size() != eddsa_signature_bytes(curve)) {
    return false;
  }

  switch (curve) {
    case EdwardsCurve::Ed25519:
      return sourcemeta_core_eddsa_ed25519_verify_cryptokit(
          reinterpret_cast<const unsigned char *>(public_key.data()),
          public_key.size(),
          reinterpret_cast<const unsigned char *>(message.data()),
          message.size(),
          reinterpret_cast<const unsigned char *>(signature.data()),
          signature.size());
    case EdwardsCurve::Ed448:
      return edwards448_verify(public_key, message, signature);
  }

  std::unreachable();
}

} // namespace sourcemeta::core
