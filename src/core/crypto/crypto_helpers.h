#ifndef SOURCEMETA_CORE_CRYPTO_HELPERS_H_
#define SOURCEMETA_CORE_CRYPTO_HELPERS_H_

#include <sourcemeta/core/crypto_sha256.h>
#include <sourcemeta/core/crypto_sha384.h>
#include <sourcemeta/core/crypto_sha512.h>
#include <sourcemeta/core/crypto_verify.h>
#include <sourcemeta/core/text.h>

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::unreachable

namespace sourcemeta::core {

// The largest RSA key any backend accepts, so that every backend agrees on
// the range of valid key sizes
inline constexpr std::size_t MAXIMUM_KEY_BYTES{512};

// Overwrite a buffer that held secret material so it does not linger in freed
// memory. The volatile access stops the compiler from eliding the write as a
// dead store
inline auto secure_zero(void *const data, const std::size_t size) noexcept
    -> void {
  if (data == nullptr) {
    return;
  }

  auto *pointer{static_cast<volatile unsigned char *>(data)};
  for (std::size_t index{0}; index < size; index += 1) {
    pointer[index] = 0;
  }
}

inline auto secure_zero(std::string &value) noexcept -> void {
  secure_zero(value.data(), value.size());
}

// Overwrite the referenced buffer when leaving the current scope, so secret
// material a local holds is wiped across every return path without threading a
// manual call through each one. It clears the buffer the string owns at scope
// exit, so a reassignment or a growth that reallocates before then can still
// leave an earlier copy in freed memory, a residual that a wiping allocator
// would be needed to close
struct SecureScope {
  explicit SecureScope(std::string &value) noexcept : target{value} {}
  SecureScope(const SecureScope &) = delete;
  auto operator=(const SecureScope &) -> SecureScope & = delete;
  SecureScope(SecureScope &&) = delete;
  auto operator=(SecureScope &&) -> SecureScope & = delete;
  ~SecureScope() { secure_zero(this->target); }
  std::string &target;
};

// The same guard for a raw buffer, so a secret that a fixed-size digest array
// holds is wiped when leaving the current scope
struct SecureBufferScope {
  SecureBufferScope(void *const buffer, const std::size_t length) noexcept
      : data{buffer}, size{length} {}
  SecureBufferScope(const SecureBufferScope &) = delete;
  auto operator=(const SecureBufferScope &) -> SecureBufferScope & = delete;
  SecureBufferScope(SecureBufferScope &&) = delete;
  auto operator=(SecureBufferScope &&) -> SecureBufferScope & = delete;
  ~SecureBufferScope() { secure_zero(this->data, this->size); }
  void *const data;
  const std::size_t size;
};

// Whether one big-endian integer is strictly less than another, comparing by
// significant length first and then lexicographically once the leading zeros
// are stripped
inline auto octets_below(const std::string_view value,
                         const std::string_view bound) noexcept -> bool {
  const auto stripped_value{strip_left(value, '\x00')};
  const auto stripped_bound{strip_left(bound, '\x00')};
  if (stripped_value.size() != stripped_bound.size()) {
    return stripped_value.size() < stripped_bound.size();
  }

  return stripped_value < stripped_bound;
}

// Whether a signature representative, as a big-endian integer, is strictly
// less than the modulus. RFC 8017 Section 5.2.2 requires this range check, so
// that an unreduced signature, which an attacker forges by adding the modulus
// without changing the modular exponentiation result, is rejected
inline auto rsa_signature_in_range(const std::string_view signature,
                                   const std::string_view modulus) noexcept
    -> bool {
  return octets_below(signature, modulus);
}

// Whether an RSA public exponent is acceptable for the modulus: odd and in the
// range [3, n) (RFC 8017 Section 3.1). An even exponent is not invertible
// modulo the totient, e = 1 leaves the signature equal to the padded message
// and forges trivially, and an exponent at or above the modulus is not a valid
// key. Shared so every backend rejects such keys, not just the reference one
inline auto
rsa_public_exponent_acceptable(const std::string_view exponent,
                               const std::string_view modulus) noexcept
    -> bool {
  const auto stripped{strip_left(exponent, '\x00')};
  if (stripped.empty() ||
      (static_cast<std::uint8_t>(stripped.back()) & 1U) == 0 ||
      (stripped.size() == 1 &&
       static_cast<std::uint8_t>(stripped.back()) < 3)) {
    return false;
  }

  return octets_below(exponent, modulus);
}

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

// The group order of each NIST prime curve as big-endian octets (FIPS 186-4
// Appendix D.1.2), so a private scalar can be range-checked against it
inline auto curve_order_bytes(const EllipticCurve curve) -> std::string {
  switch (curve) {
    case EllipticCurve::P256:
      return hex_to_bytes("ffffffff00000000ffffffffffffffffbce6faada7179e84"
                          "f3b9cac2fc632551")
          .value();
    case EllipticCurve::P384:
      return hex_to_bytes("ffffffffffffffffffffffffffffffffffffffffffffffff"
                          "c7634d81f4372ddf581a0db248b0a77aecec196accc52973")
          .value();
    case EllipticCurve::P521:
      return hex_to_bytes("01fffffffffffffffffffffffffffffffffffffffffffffff"
                          "ffffffffffffffffffa51868783bf2f966b7fcc0148f709a5"
                          "d03bb5c9b8899c47aebb6fb71e91386409")
          .value();
  }

  std::unreachable();
}

// Whether an elliptic curve private scalar lies in the valid range [1, n)
// (SEC 1 Section 3.2.1). Shared so every backend rejects an out-of-range scalar
inline auto ec_private_scalar_in_range(const std::string_view scalar,
                                       const EllipticCurve curve) -> bool {
  return !strip_left(scalar, '\x00').empty() &&
         octets_below(scalar, curve_order_bytes(curve));
}

// The public key and signature octet lengths are fixed per curve (RFC 8032
// Section 5.1.2 and Section 5.1.6)
inline auto eddsa_public_key_bytes(const EdwardsCurve curve) noexcept
    -> std::size_t {
  switch (curve) {
    case EdwardsCurve::Ed25519:
      return 32;
    case EdwardsCurve::Ed448:
      return 57;
  }

  std::unreachable();
}

inline auto eddsa_signature_bytes(const EdwardsCurve curve) noexcept
    -> std::size_t {
  switch (curve) {
    case EdwardsCurve::Ed25519:
      return 64;
    case EdwardsCurve::Ed448:
      return 114;
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
