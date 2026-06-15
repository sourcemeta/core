#ifndef SOURCEMETA_CORE_CRYPTO_VERIFY_UTIL_H_
#define SOURCEMETA_CORE_CRYPTO_VERIFY_UTIL_H_

// Helpers shared by the signature verification backends, exactly one of
// which is compiled per build

#include <sourcemeta/core/crypto_sha256.h>
#include <sourcemeta/core/crypto_sha384.h>
#include <sourcemeta/core/crypto_sha512.h>
#include <sourcemeta/core/crypto_verify.h>

#include <cstddef>     // std::size_t
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::unreachable

namespace sourcemeta::core {

// The largest RSA key any backend accepts, so that every backend agrees on
// the range of valid key sizes
inline constexpr std::size_t MAXIMUM_KEY_BYTES{512};

inline auto curve_field_bytes(const EllipticCurve curve) noexcept
    -> std::size_t {
  switch (curve) {
    case EllipticCurve::P256:
      return 32;
    case EllipticCurve::P384:
      return 48;
    case EllipticCurve::P521:
      return 66;
  }

  std::unreachable();
}

inline auto digest_message(const SignatureHashFunction hash,
                           const std::string_view message) -> std::string {
  switch (hash) {
    case SignatureHashFunction::SHA256: {
      const auto digest{sha256_digest(message)};
      return {reinterpret_cast<const char *>(digest.data()), digest.size()};
    }
    case SignatureHashFunction::SHA384: {
      const auto digest{sha384_digest(message)};
      return {reinterpret_cast<const char *>(digest.data()), digest.size()};
    }
    case SignatureHashFunction::SHA512: {
      const auto digest{sha512_digest(message)};
      return {reinterpret_cast<const char *>(digest.data()), digest.size()};
    }
  }

  std::unreachable();
}

} // namespace sourcemeta::core

#endif
