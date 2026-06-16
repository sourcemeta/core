#include <sourcemeta/core/crypto_verify.h>

#include "crypto_eddsa.h"

#include <string_view> // std::string_view
#include <utility>     // std::unreachable

namespace sourcemeta::core {

auto eddsa_verify(const EdwardsCurve curve, const std::string_view public_key,
                  const std::string_view message,
                  const std::string_view signature) -> bool {
  switch (curve) {
    case EdwardsCurve::Ed25519:
      return edwards25519_verify(public_key, message, signature);
    case EdwardsCurve::Ed448:
      return edwards448_verify(public_key, message, signature);
  }

  std::unreachable();
}

} // namespace sourcemeta::core
