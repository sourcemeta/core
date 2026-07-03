#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/text.h>

#include "crypto_bignum.h"
#include "crypto_ecc.h"
#include "crypto_eddsa.h"
#include "crypto_helpers.h"
#include "crypto_pkcs8.h"

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t, std::uint32_t
#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move, std::unreachable

namespace sourcemeta::core {
namespace {

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

auto digest_info_prefix(const SignatureHashFunction hash) -> std::string_view {
  switch (hash) {
    case SignatureHashFunction::SHA256:
      return {reinterpret_cast<const char *>(DIGEST_INFO_SHA256.data()),
              DIGEST_INFO_SHA256.size()};
    case SignatureHashFunction::SHA384:
      return {reinterpret_cast<const char *>(DIGEST_INFO_SHA384.data()),
              DIGEST_INFO_SHA384.size()};
    case SignatureHashFunction::SHA512:
      return {reinterpret_cast<const char *>(DIGEST_INFO_SHA512.data()),
              DIGEST_INFO_SHA512.size()};
  }

  std::unreachable();
}

// EMSA-PKCS1-v1_5 encoding (RFC 8017 Section 9.2)
auto build_encoded_message(const SignatureHashFunction hash,
                           const std::string_view message,
                           const std::size_t key_length)
    -> std::optional<std::string> {
  const auto prefix{digest_info_prefix(hash)};
  const auto digest{digest_message(hash, message)};
  const auto encoded_length{prefix.size() + digest.size()};
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

// MGF1 mask generation (RFC 8017 Appendix B.2.1)
auto mask_generation(const SignatureHashFunction hash,
                     const std::string_view seed, const std::size_t length)
    -> std::string {
  std::string result;
  result.reserve(length + 64);
  std::uint32_t counter{0};
  while (result.size() < length) {
    std::string block{seed};
    block.push_back(static_cast<char>((counter >> 24u) & 0xffu));
    block.push_back(static_cast<char>((counter >> 16u) & 0xffu));
    block.push_back(static_cast<char>((counter >> 8u) & 0xffu));
    block.push_back(static_cast<char>(counter & 0xffu));
    result.append(digest_message(hash, block));
    counter += 1;
  }

  result.resize(length);
  return result;
}

// EMSA-PSS encoding (RFC 8017 Section 9.1.1), with the salt length fixed to the
// hash function output as RFC 7518 Section 3.5 requires
auto emsa_pss_encode(const SignatureHashFunction hash,
                     const std::string_view message,
                     const std::size_t encoded_bits)
    -> std::optional<std::string> {
  const auto message_digest{digest_message(hash, message)};
  const auto hash_length{message_digest.size()};
  const auto salt_length{hash_length};
  const auto encoded_length{(encoded_bits + 7) / 8};
  if (encoded_length < hash_length + salt_length + 2) {
    return std::nullopt;
  }

  const auto salt{random_bytes(salt_length)};
  std::string primed(8, '\x00');
  primed.append(message_digest);
  primed.append(salt);
  const auto hash_value{digest_message(hash, primed)};

  const auto database_length{encoded_length - hash_length - 1};
  std::string database(database_length - salt_length - 1, '\x00');
  database.push_back('\x01');
  database.append(salt);
  const auto mask{mask_generation(hash, hash_value, database_length)};
  for (std::size_t index = 0; index < database_length; ++index) {
    database[index] = static_cast<char>(database[index] ^ mask[index]);
  }

  // RFC 8017 Section 9.1.1 step 11: clear the leftmost 8emLen - emBits bits
  const auto unused_bits{(8 * encoded_length) - encoded_bits};
  const auto unused_mask{
      static_cast<std::uint8_t>((0xff00u >> unused_bits) & 0xffu)};
  database[0] = static_cast<char>(static_cast<std::uint8_t>(database[0]) &
                                  static_cast<std::uint8_t>(~unused_mask));

  std::string result{std::move(database)};
  result.append(hash_value);
  result.push_back('\xbc');
  return result;
}

auto to_curve_parameters(const EllipticCurve curve) -> EllipticCurveParameters {
  switch (curve) {
    case EllipticCurve::P256:
      return curve_p256();
    case EllipticCurve::P384:
      return curve_p384();
    case EllipticCurve::P521:
      return curve_p521();
  }

  std::unreachable();
}

// FIPS 186-4 Section 6.4 step 2, deriving the integer from the leftmost bits of
// the message digest, truncated to the bit length of the order
auto digest_to_integer(const SignatureHashFunction hash,
                       const std::string_view message,
                       const std::size_t order_bits) -> Bignum {
  const auto digest{digest_message(hash, message)};
  auto value{bignum_from_bytes(digest)};
  const auto digest_bits{digest.size() * 8};
  if (digest_bits > order_bits) {
    value = bignum_shift_right(value, digest_bits - order_bits);
  }

  return value;
}

auto sign_rsa(const std::string_view modulus,
              const std::string_view private_exponent,
              const std::string_view encoded_message) -> std::string {
  const auto representative{bignum_mod_exp(bignum_from_bytes(encoded_message),
                                           bignum_from_bytes(private_exponent),
                                           bignum_from_bytes(modulus))};
  return bignum_to_bytes(representative, modulus.size());
}

// ECDSA signature generation (FIPS 186-4 Section 6.4.1)
auto sign_ecdsa(const EllipticCurve curve, const SignatureHashFunction hash,
                const std::string_view scalar, const std::string_view message)
    -> std::optional<std::string> {
  const auto parameters{to_curve_parameters(curve)};
  const auto field_bytes{parameters.field_bytes};
  const auto order_bits{bignum_bit_length(parameters.order)};
  const auto digest_integer{digest_to_integer(hash, message, order_bits)};
  const auto private_scalar{bignum_from_bytes(scalar)};
  const JacobianPoint generator{.x = parameters.generator_x,
                                .y = parameters.generator_y,
                                .z = bignum_from_u64(1)};

  for (std::size_t attempt = 0; attempt < 256; ++attempt) {
    const auto nonce{bignum_random_scalar(parameters.order)};
    const auto point{point_double_scalar_multiply(
        nonce, generator, bignum_from_u64(0), generator, parameters)};
    if (point_is_infinity(point)) {
      continue;
    }

    auto r{point_affine_x(point, parameters)};
    bignum_reduce(r, parameters.order);
    if (bignum_is_zero(r)) {
      continue;
    }

    // s = k^-1 (z + r * d) mod n
    const auto s{bignum_mod_multiply(
        bignum_mod_inverse(nonce, parameters.order),
        bignum_mod_add(digest_integer,
                       bignum_mod_multiply(r, private_scalar, parameters.order),
                       parameters.order),
        parameters.order)};
    if (bignum_is_zero(s)) {
      continue;
    }

    std::string signature{bignum_to_bytes(r, field_bytes)};
    signature.append(bignum_to_bytes(s, field_bytes));
    return signature;
  }

  return std::nullopt;
}

} // namespace

// The reference backend parses the key material into big integers inside each
// signature, so the parsed key simply holds the raw material
struct PrivateKey::Internal {
  PrivateKey::Type kind;
  std::string modulus;
  std::string public_exponent;
  std::string private_exponent;
  std::string scalar;
  EllipticCurve elliptic_curve;
  std::string edwards_seed;
  EdwardsCurve edwards_curve;
};

PrivateKey::PrivateKey(Internal *internal) noexcept : internal_{internal} {}

PrivateKey::~PrivateKey() { delete internal_; }

PrivateKey::PrivateKey(PrivateKey &&other) noexcept
    : internal_{other.internal_} {
  other.internal_ = nullptr;
}

auto PrivateKey::operator=(PrivateKey &&other) noexcept -> PrivateKey & {
  if (this != &other) {
    delete internal_;
    internal_ = other.internal_;
    other.internal_ = nullptr;
  }

  return *this;
}

auto PrivateKey::type() const noexcept -> Type { return internal_->kind; }

auto make_private_key(const std::string_view pem) -> std::optional<PrivateKey> {
  const auto der{pem_to_der(pem)};
  if (!der.has_value()) {
    return std::nullopt;
  }

  const auto parsed{parse_pkcs8(der.value())};
  if (!parsed.has_value()) {
    return std::nullopt;
  }

  switch (parsed->kind) {
    case PKCS8KeyKind::RSA: {
      // RFC 8017 Appendix A.1.2: RSAPrivateKey is a SEQUENCE of version,
      // modulus, publicExponent, privateExponent, and the further primes
      const auto sequence{der_read(parsed->key)};
      if (!sequence.has_value() || sequence->tag != 0x30) {
        return std::nullopt;
      }

      const auto version{der_read(sequence->content)};
      if (!version.has_value() || version->tag != 0x02) {
        return std::nullopt;
      }

      const auto modulus{der_read(version->rest)};
      if (!modulus.has_value() || modulus->tag != 0x02) {
        return std::nullopt;
      }

      const auto public_exponent{der_read(modulus->rest)};
      if (!public_exponent.has_value() || public_exponent->tag != 0x02) {
        return std::nullopt;
      }

      const auto private_exponent{der_read(public_exponent->rest)};
      if (!private_exponent.has_value() || private_exponent->tag != 0x02) {
        return std::nullopt;
      }

      const auto stripped_modulus{strip_left(modulus->content, '\x00')};
      if (stripped_modulus.empty() ||
          stripped_modulus.size() > MAXIMUM_KEY_BYTES) {
        return std::nullopt;
      }

      return PrivateKey{new PrivateKey::Internal{
          .kind = PrivateKey::Type::RSA,
          .modulus = std::string{stripped_modulus},
          .public_exponent =
              std::string{strip_left(public_exponent->content, '\x00')},
          .private_exponent =
              std::string{strip_left(private_exponent->content, '\x00')},
          .scalar = {},
          .elliptic_curve = {},
          .edwards_seed = {},
          .edwards_curve = {}}};
    }
    case PKCS8KeyKind::EllipticCurve: {
      // RFC 5915 Section 3: ECPrivateKey is a SEQUENCE whose second field is
      // the private scalar as an OCTET STRING
      const auto sequence{der_read(parsed->key)};
      if (!sequence.has_value() || sequence->tag != 0x30) {
        return std::nullopt;
      }

      const auto version{der_read(sequence->content)};
      if (!version.has_value() || version->tag != 0x02) {
        return std::nullopt;
      }

      const auto scalar{der_read(version->rest)};
      if (!scalar.has_value() || scalar->tag != 0x04) {
        return std::nullopt;
      }

      return PrivateKey{new PrivateKey::Internal{
          .kind = PrivateKey::Type::EllipticCurve,
          .modulus = {},
          .public_exponent = {},
          .private_exponent = {},
          .scalar =
              std::string{pad_left(strip_left(scalar->content, '\x00'),
                                   curve_field_bytes(parsed->curve), '\x00')},
          .elliptic_curve = parsed->curve,
          .edwards_seed = {},
          .edwards_curve = {}}};
    }
    case PKCS8KeyKind::Edwards: {
      const auto seed{der_read(parsed->key)};
      if (!seed.has_value() || seed->tag != 0x04 ||
          seed->content.size() !=
              eddsa_public_key_bytes(parsed->edwards_curve)) {
        return std::nullopt;
      }

      return PrivateKey{
          new PrivateKey::Internal{.kind = PrivateKey::Type::Edwards,
                                   .modulus = {},
                                   .public_exponent = {},
                                   .private_exponent = {},
                                   .scalar = {},
                                   .elliptic_curve = {},
                                   .edwards_seed = std::string{seed->content},
                                   .edwards_curve = parsed->edwards_curve}};
    }
  }

  std::unreachable();
}

auto rsassa_pkcs1_v15_sign(const PrivateKey &key,
                           const SignatureHashFunction hash,
                           const std::string_view message)
    -> std::optional<std::string> {
  const auto *internal{key.internal()};
  if (internal == nullptr || internal->kind != PrivateKey::Type::RSA) {
    return std::nullopt;
  }

  const auto encoded{
      build_encoded_message(hash, message, internal->modulus.size())};
  if (!encoded.has_value()) {
    return std::nullopt;
  }

  return sign_rsa(internal->modulus, internal->private_exponent,
                  encoded.value());
}

auto rsassa_pss_sign(const PrivateKey &key, const SignatureHashFunction hash,
                     const std::string_view message)
    -> std::optional<std::string> {
  const auto *internal{key.internal()};
  if (internal == nullptr || internal->kind != PrivateKey::Type::RSA) {
    return std::nullopt;
  }

  const auto encoded_bits{
      bignum_bit_length(bignum_from_bytes(internal->modulus)) - 1};
  const auto encoded{emsa_pss_encode(hash, message, encoded_bits)};
  if (!encoded.has_value()) {
    return std::nullopt;
  }

  return sign_rsa(internal->modulus, internal->private_exponent,
                  encoded.value());
}

auto ecdsa_sign(const PrivateKey &key, const SignatureHashFunction hash,
                const std::string_view message) -> std::optional<std::string> {
  const auto *internal{key.internal()};
  if (internal == nullptr ||
      internal->kind != PrivateKey::Type::EllipticCurve) {
    return std::nullopt;
  }

  return sign_ecdsa(internal->elliptic_curve, hash, internal->scalar, message);
}

auto eddsa_sign(const PrivateKey &key, const std::string_view message)
    -> std::optional<std::string> {
  const auto *internal{key.internal()};
  if (internal == nullptr || internal->kind != PrivateKey::Type::Edwards) {
    return std::nullopt;
  }

  switch (internal->edwards_curve) {
    case EdwardsCurve::Ed25519:
      return edwards25519_sign(internal->edwards_seed, message);
    case EdwardsCurve::Ed448:
      return edwards448_sign(internal->edwards_seed, message);
  }

  std::unreachable();
}

} // namespace sourcemeta::core
