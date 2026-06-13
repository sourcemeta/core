#include <sourcemeta/core/crypto_sha256.h>
#include <sourcemeta/core/crypto_sha384.h>
#include <sourcemeta/core/crypto_sha512.h>
#include <sourcemeta/core/crypto_verify.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h> // ULONG, LPCWSTR

#include <bcrypt.h> // BCrypt*, BCRYPT_*

#include <bit>         // std::countl_zero
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t
#include <cstring>     // std::memcpy
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::unreachable

namespace {

// Matching the reference backend so that every backend accepts the same
// key sizes
constexpr std::size_t MAXIMUM_KEY_BYTES{512};

auto to_cng_algorithm(
    const sourcemeta::core::SignatureHashFunction hash) noexcept -> LPCWSTR {
  switch (hash) {
    case sourcemeta::core::SignatureHashFunction::SHA256:
      return BCRYPT_SHA256_ALGORITHM;
    case sourcemeta::core::SignatureHashFunction::SHA384:
      return BCRYPT_SHA384_ALGORITHM;
    case sourcemeta::core::SignatureHashFunction::SHA512:
      return BCRYPT_SHA512_ALGORITHM;
  }

  std::unreachable();
}

auto strip_leading_zeros(std::string_view value) -> std::string_view {
  while (!value.empty() && value.front() == '\x00') {
    value.remove_prefix(1);
  }

  return value;
}

auto to_ecdsa_algorithm(const sourcemeta::core::EllipticCurve curve) noexcept
    -> LPCWSTR {
  switch (curve) {
    case sourcemeta::core::EllipticCurve::P256:
      return BCRYPT_ECDSA_P256_ALGORITHM;
    case sourcemeta::core::EllipticCurve::P384:
      return BCRYPT_ECDSA_P384_ALGORITHM;
    case sourcemeta::core::EllipticCurve::P521:
      return BCRYPT_ECDSA_P521_ALGORITHM;
  }

  std::unreachable();
}

auto to_ecc_public_magic(const sourcemeta::core::EllipticCurve curve) noexcept
    -> ULONG {
  switch (curve) {
    case sourcemeta::core::EllipticCurve::P256:
      return BCRYPT_ECDSA_PUBLIC_P256_MAGIC;
    case sourcemeta::core::EllipticCurve::P384:
      return BCRYPT_ECDSA_PUBLIC_P384_MAGIC;
    case sourcemeta::core::EllipticCurve::P521:
      return BCRYPT_ECDSA_PUBLIC_P521_MAGIC;
  }

  std::unreachable();
}

auto curve_field_bytes(const sourcemeta::core::EllipticCurve curve) noexcept
    -> std::size_t {
  switch (curve) {
    case sourcemeta::core::EllipticCurve::P256:
      return 32;
    case sourcemeta::core::EllipticCurve::P384:
      return 48;
    case sourcemeta::core::EllipticCurve::P521:
      return 66;
  }

  std::unreachable();
}

auto left_pad(const std::string_view value, const std::size_t width)
    -> std::string {
  if (value.size() >= width) {
    return std::string{value};
  }

  return std::string(width - value.size(), '\x00') + std::string{value};
}

auto digest_message(const sourcemeta::core::SignatureHashFunction hash,
                    const std::string_view message) -> std::string {
  switch (hash) {
    case sourcemeta::core::SignatureHashFunction::SHA256: {
      const auto digest{sourcemeta::core::sha256_digest(message)};
      return {reinterpret_cast<const char *>(digest.data()), digest.size()};
    }
    case sourcemeta::core::SignatureHashFunction::SHA384: {
      const auto digest{sourcemeta::core::sha384_digest(message)};
      return {reinterpret_cast<const char *>(digest.data()), digest.size()};
    }
    case sourcemeta::core::SignatureHashFunction::SHA512: {
      const auto digest{sourcemeta::core::sha512_digest(message)};
      return {reinterpret_cast<const char *>(digest.data()), digest.size()};
    }
  }

  std::unreachable();
}

auto verify_rsa_signature(const sourcemeta::core::SignatureHashFunction hash,
                          const std::string_view modulus,
                          const std::string_view exponent,
                          const std::string_view message,
                          const std::string_view signature,
                          const bool probabilistic) -> bool {
  const auto stripped_modulus{strip_leading_zeros(modulus)};
  const auto stripped_exponent{strip_leading_zeros(exponent)};
  if (stripped_modulus.empty() || stripped_exponent.empty() ||
      stripped_modulus.size() > MAXIMUM_KEY_BYTES ||
      stripped_exponent.size() > MAXIMUM_KEY_BYTES) {
    return false;
  }

  const auto modulus_bit_length{
      (stripped_modulus.size() * 8u) -
      static_cast<std::size_t>(std::countl_zero(
          static_cast<std::uint8_t>(stripped_modulus.front())))};

  BCRYPT_RSAKEY_BLOB header{};
  header.Magic = BCRYPT_RSAPUBLIC_MAGIC;
  header.BitLength = static_cast<ULONG>(modulus_bit_length);
  header.cbPublicExp = static_cast<ULONG>(stripped_exponent.size());
  header.cbModulus = static_cast<ULONG>(stripped_modulus.size());
  header.cbPrime1 = 0;
  header.cbPrime2 = 0;

  std::string blob;
  blob.resize(sizeof(header));
  std::memcpy(blob.data(), &header, sizeof(header));
  blob.append(stripped_exponent);
  blob.append(stripped_modulus);

  BCRYPT_ALG_HANDLE algorithm{nullptr};
  if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(
          &algorithm, BCRYPT_RSA_ALGORITHM, nullptr, 0))) {
    return false;
  }

  BCRYPT_KEY_HANDLE key{nullptr};
  if (!BCRYPT_SUCCESS(
          BCryptImportKeyPair(algorithm, nullptr, BCRYPT_RSAPUBLIC_BLOB, &key,
                              reinterpret_cast<unsigned char *>(blob.data()),
                              static_cast<ULONG>(blob.size()), 0))) {
    BCryptCloseAlgorithmProvider(algorithm, 0);
    return false;
  }

  const auto digest{digest_message(hash, message)};

  // The signature parameter is not const-qualified but is input only
  auto result{false};
  if (probabilistic) {
    // The digest-length salt is what RFC 7518 Section 3.5 requires
    BCRYPT_PSS_PADDING_INFO padding{};
    padding.pszAlgId = to_cng_algorithm(hash);
    padding.cbSalt = static_cast<ULONG>(digest.size());
    result = BCRYPT_SUCCESS(BCryptVerifySignature(
        key, &padding,
        reinterpret_cast<unsigned char *>(const_cast<char *>(digest.data())),
        static_cast<ULONG>(digest.size()),
        reinterpret_cast<unsigned char *>(const_cast<char *>(signature.data())),
        static_cast<ULONG>(signature.size()), BCRYPT_PAD_PSS));
  } else {
    BCRYPT_PKCS1_PADDING_INFO padding{};
    padding.pszAlgId = to_cng_algorithm(hash);
    result = BCRYPT_SUCCESS(BCryptVerifySignature(
        key, &padding,
        reinterpret_cast<unsigned char *>(const_cast<char *>(digest.data())),
        static_cast<ULONG>(digest.size()),
        reinterpret_cast<unsigned char *>(const_cast<char *>(signature.data())),
        static_cast<ULONG>(signature.size()), BCRYPT_PAD_PKCS1));
  }

  BCryptDestroyKey(key);
  BCryptCloseAlgorithmProvider(algorithm, 0);
  return result;
}

} // namespace

namespace sourcemeta::core {

auto rsassa_pkcs1_v15_verify(const SignatureHashFunction hash,
                             const std::string_view modulus,
                             const std::string_view exponent,
                             const std::string_view message,
                             const std::string_view signature) -> bool {
  return verify_rsa_signature(hash, modulus, exponent, message, signature,
                              false);
}

auto rsassa_pss_verify(const SignatureHashFunction hash,
                       const std::string_view modulus,
                       const std::string_view exponent,
                       const std::string_view message,
                       const std::string_view signature) -> bool {
  return verify_rsa_signature(hash, modulus, exponent, message, signature,
                              true);
}

auto ecdsa_verify(const EllipticCurve curve, const SignatureHashFunction hash,
                  const std::string_view coordinate_x,
                  const std::string_view coordinate_y,
                  const std::string_view message,
                  const std::string_view signature) -> bool {
  const auto width{curve_field_bytes(curve)};
  if (signature.size() != width * 2 || coordinate_x.size() > width ||
      coordinate_y.size() > width) {
    return false;
  }

  // The public key blob is the header followed by the two fixed-width
  // coordinates
  BCRYPT_ECCKEY_BLOB header{};
  header.dwMagic = to_ecc_public_magic(curve);
  header.cbKey = static_cast<ULONG>(width);

  std::string blob;
  blob.resize(sizeof(header));
  std::memcpy(blob.data(), &header, sizeof(header));
  blob.append(left_pad(coordinate_x, width));
  blob.append(left_pad(coordinate_y, width));

  BCRYPT_ALG_HANDLE algorithm{nullptr};
  if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(
          &algorithm, to_ecdsa_algorithm(curve), nullptr, 0))) {
    return false;
  }

  BCRYPT_KEY_HANDLE key{nullptr};
  if (!BCRYPT_SUCCESS(
          BCryptImportKeyPair(algorithm, nullptr, BCRYPT_ECCPUBLIC_BLOB, &key,
                              reinterpret_cast<unsigned char *>(blob.data()),
                              static_cast<ULONG>(blob.size()), 0))) {
    BCryptCloseAlgorithmProvider(algorithm, 0);
    return false;
  }

  const auto digest{digest_message(hash, message)};

  // The CNG signature format is the raw fixed-width R || S concatenation, so
  // the input passes through unchanged
  const auto result{BCRYPT_SUCCESS(BCryptVerifySignature(
      key, nullptr,
      reinterpret_cast<unsigned char *>(const_cast<char *>(digest.data())),
      static_cast<ULONG>(digest.size()),
      reinterpret_cast<unsigned char *>(const_cast<char *>(signature.data())),
      static_cast<ULONG>(signature.size()), 0))};

  BCryptDestroyKey(key);
  BCryptCloseAlgorithmProvider(algorithm, 0);
  return result;
}

} // namespace sourcemeta::core
