#include <sourcemeta/core/crypto_sha256.h>
#include <sourcemeta/core/crypto_sha384.h>
#include <sourcemeta/core/crypto_sha512.h>
#include <sourcemeta/core/crypto_verify.h>

#include "crypto_bignum.h"

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t
#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::unreachable

namespace {

constexpr std::size_t MAXIMUM_KEY_BYTES{512};

auto strip_leading_zeros(std::string_view value) -> std::string_view {
  while (!value.empty() && value.front() == '\x00') {
    value.remove_prefix(1);
  }

  return value;
}

// The DigestInfo prefixes for the EMSA-PKCS1-v1_5 encoding, taken verbatim
// from RFC 8017 Section 9.2 Note 1
constexpr std::array<std::uint8_t, 19> DIGEST_INFO_SHA256{
    {0x30, 0x31, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03,
     0x04, 0x02, 0x01, 0x05, 0x00, 0x04, 0x20}};
constexpr std::array<std::uint8_t, 19> DIGEST_INFO_SHA384{
    {0x30, 0x41, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03,
     0x04, 0x02, 0x02, 0x05, 0x00, 0x04, 0x30}};
constexpr std::array<std::uint8_t, 19> DIGEST_INFO_SHA512{
    {0x30, 0x51, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03,
     0x04, 0x02, 0x03, 0x05, 0x00, 0x04, 0x40}};

auto digest_info_prefix(const sourcemeta::core::SignatureHashFunction hash)
    -> std::string_view {
  switch (hash) {
    case sourcemeta::core::SignatureHashFunction::SHA256:
      return {reinterpret_cast<const char *>(DIGEST_INFO_SHA256.data()),
              DIGEST_INFO_SHA256.size()};
    case sourcemeta::core::SignatureHashFunction::SHA384:
      return {reinterpret_cast<const char *>(DIGEST_INFO_SHA384.data()),
              DIGEST_INFO_SHA384.size()};
    case sourcemeta::core::SignatureHashFunction::SHA512:
      return {reinterpret_cast<const char *>(DIGEST_INFO_SHA512.data()),
              DIGEST_INFO_SHA512.size()};
  }

  std::unreachable();
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

// EMSA-PKCS1-v1_5 encoding (RFC 8017 Section 9.2)
auto build_encoded_message(const sourcemeta::core::SignatureHashFunction hash,
                           const std::string_view message,
                           const std::size_t key_length)
    -> std::optional<std::string> {
  const auto prefix{digest_info_prefix(hash)};
  const auto digest{digest_message(hash, message)};
  const auto encoded_length{prefix.size() + digest.size()};

  // RFC 8017 Section 9.2 step 3: "If emLen < tLen + 11, output 'intended
  // encoded message length too short'"
  if (key_length < encoded_length + 11) {
    return std::nullopt;
  }

  std::string result;
  result.reserve(key_length);
  result.push_back('\x00');
  result.push_back('\x01');
  result.append(key_length - encoded_length - 3, '\xff');
  result.push_back('\x00');
  result.append(prefix);
  result.append(digest);
  return result;
}

} // namespace

namespace sourcemeta::core {

auto rsassa_pkcs1_v15_verify(const SignatureHashFunction hash,
                             const std::string_view modulus,
                             const std::string_view exponent,
                             const std::string_view message,
                             const std::string_view signature) -> bool {
  const auto stripped_modulus{strip_leading_zeros(modulus)};
  const auto stripped_exponent{strip_leading_zeros(exponent)};
  if (stripped_modulus.empty() || stripped_exponent.empty() ||
      stripped_modulus.size() > MAXIMUM_KEY_BYTES ||
      stripped_exponent.size() > MAXIMUM_KEY_BYTES) {
    return false;
  }

  // RFC 8017 Section 8.2.2 step 1: "If the length of S is not k octets,
  // output 'invalid signature'"
  const auto key_length{stripped_modulus.size()};
  if (signature.size() != key_length) {
    return false;
  }

  const auto modulus_number{bignum_from_bytes(stripped_modulus)};
  const auto signature_number{bignum_from_bytes(signature)};

  // RFC 8017 Section 5.2.2: "If the signature representative s is not
  // between 0 and n - 1, output 'signature representative out of range'"
  if (bignum_compare(signature_number, modulus_number) >= 0) {
    return false;
  }

  const auto exponent_number{bignum_from_bytes(stripped_exponent)};
  const auto message_representative{
      bignum_mod_exp(signature_number, exponent_number, modulus_number)};
  const auto encoded_message{
      bignum_to_bytes(message_representative, key_length)};
  const auto expected{build_encoded_message(hash, message, key_length)};
  return expected.has_value() && encoded_message == expected.value();
}

} // namespace sourcemeta::core
