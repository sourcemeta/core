#include <sourcemeta/core/crypto_verify.h>

#include <openssl/bn.h>          // BN_*
#include <openssl/core_names.h>  // OSSL_PKEY_PARAM_*
#include <openssl/ec.h>          // ECDSA_SIG_*, i2d_ECDSA_SIG
#include <openssl/evp.h>         // EVP_*
#include <openssl/param_build.h> // OSSL_PARAM_*
#include <openssl/rsa.h> // RSA_PKCS1_PSS_PADDING, RSA_PSS_SALTLEN_DIGEST

#include <cstddef>     // std::size_t
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::unreachable

namespace {

// Matching the reference backend so that every backend accepts the same
// key sizes
constexpr std::size_t MAXIMUM_KEY_BYTES{512};

auto strip_leading_zeros(std::string_view value) -> std::string_view {
  while (!value.empty() && value.front() == '\x00') {
    value.remove_prefix(1);
  }

  return value;
}

auto to_message_digest(
    const sourcemeta::core::SignatureHashFunction hash) noexcept
    -> const EVP_MD * {
  switch (hash) {
    case sourcemeta::core::SignatureHashFunction::SHA256:
      return EVP_sha256();
    case sourcemeta::core::SignatureHashFunction::SHA384:
      return EVP_sha384();
    case sourcemeta::core::SignatureHashFunction::SHA512:
      return EVP_sha512();
  }

  std::unreachable();
}

auto make_rsa_public_key(const std::string_view modulus,
                         const std::string_view exponent) -> EVP_PKEY * {
  EVP_PKEY *result{nullptr};
  auto *modulus_number{
      BN_bin2bn(reinterpret_cast<const unsigned char *>(modulus.data()),
                static_cast<int>(modulus.size()), nullptr)};
  auto *exponent_number{
      BN_bin2bn(reinterpret_cast<const unsigned char *>(exponent.data()),
                static_cast<int>(exponent.size()), nullptr)};
  auto *builder{OSSL_PARAM_BLD_new()};

  if (modulus_number != nullptr && exponent_number != nullptr &&
      builder != nullptr &&
      OSSL_PARAM_BLD_push_BN(builder, OSSL_PKEY_PARAM_RSA_N, modulus_number) ==
          1 &&
      OSSL_PARAM_BLD_push_BN(builder, OSSL_PKEY_PARAM_RSA_E, exponent_number) ==
          1) {
    auto *parameters{OSSL_PARAM_BLD_to_param(builder)};
    if (parameters != nullptr) {
      auto *context{EVP_PKEY_CTX_new_from_name(nullptr, "RSA", nullptr)};
      if (context != nullptr) {
        if (EVP_PKEY_fromdata_init(context) == 1) {
          EVP_PKEY_fromdata(context, &result, EVP_PKEY_PUBLIC_KEY, parameters);
        }

        EVP_PKEY_CTX_free(context);
      }

      OSSL_PARAM_free(parameters);
    }
  }

  OSSL_PARAM_BLD_free(builder);
  BN_free(exponent_number);
  BN_free(modulus_number);
  return result;
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

  auto *key{make_rsa_public_key(stripped_modulus, stripped_exponent)};
  if (key == nullptr) {
    return false;
  }

  auto result{false};
  auto *context{EVP_MD_CTX_new()};
  if (context != nullptr) {
    EVP_PKEY_CTX *key_context{nullptr};
    auto ready{EVP_DigestVerifyInit(context, &key_context,
                                    to_message_digest(hash), nullptr,
                                    key) == 1};
    if (ready && probabilistic) {
      // Requesting the digest-length salt that RFC 7518 Section 3.5
      // requires makes verification reject signatures carrying any other
      // salt length
      ready = EVP_PKEY_CTX_set_rsa_padding(key_context,
                                           RSA_PKCS1_PSS_PADDING) == 1 &&
              EVP_PKEY_CTX_set_rsa_pss_saltlen(key_context,
                                               RSA_PSS_SALTLEN_DIGEST) == 1;
    }

    if (ready) {
      result = EVP_DigestVerify(
                   context,
                   reinterpret_cast<const unsigned char *>(signature.data()),
                   signature.size(),
                   reinterpret_cast<const unsigned char *>(message.data()),
                   message.size()) == 1;
    }

    EVP_MD_CTX_free(context);
  }

  EVP_PKEY_free(key);
  return result;
}

auto to_group_name(const sourcemeta::core::EllipticCurve curve) noexcept
    -> const char * {
  switch (curve) {
    case sourcemeta::core::EllipticCurve::P256:
      return "P-256";
    case sourcemeta::core::EllipticCurve::P384:
      return "P-384";
    case sourcemeta::core::EllipticCurve::P521:
      return "P-521";
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

auto make_ec_public_key(const sourcemeta::core::EllipticCurve curve,
                        const std::string_view coordinate_x,
                        const std::string_view coordinate_y) -> EVP_PKEY * {
  const auto width{curve_field_bytes(curve)};
  if (coordinate_x.size() > width || coordinate_y.size() > width) {
    return nullptr;
  }

  std::string point;
  point.push_back('\x04');
  point.append(left_pad(coordinate_x, width));
  point.append(left_pad(coordinate_y, width));

  EVP_PKEY *result{nullptr};
  auto *builder{OSSL_PARAM_BLD_new()};
  if (builder != nullptr &&
      OSSL_PARAM_BLD_push_utf8_string(builder, OSSL_PKEY_PARAM_GROUP_NAME,
                                      to_group_name(curve), 0) == 1 &&
      OSSL_PARAM_BLD_push_octet_string(
          builder, OSSL_PKEY_PARAM_PUB_KEY,
          reinterpret_cast<const unsigned char *>(point.data()),
          point.size()) == 1) {
    auto *parameters{OSSL_PARAM_BLD_to_param(builder)};
    if (parameters != nullptr) {
      auto *context{EVP_PKEY_CTX_new_from_name(nullptr, "EC", nullptr)};
      if (context != nullptr) {
        if (EVP_PKEY_fromdata_init(context) == 1) {
          EVP_PKEY_fromdata(context, &result, EVP_PKEY_PUBLIC_KEY, parameters);
        }

        EVP_PKEY_CTX_free(context);
      }

      OSSL_PARAM_free(parameters);
    }
  }

  OSSL_PARAM_BLD_free(builder);
  return result;
}

// Convert the raw fixed-width R || S concatenation into the DER signature
// that the verification interface expects
auto encode_ecdsa_signature(const std::string_view raw_signature,
                            unsigned char **output) -> int {
  const auto half{raw_signature.size() / 2};
  auto *signature{ECDSA_SIG_new()};
  if (signature == nullptr) {
    return -1;
  }

  auto *r{
      BN_bin2bn(reinterpret_cast<const unsigned char *>(raw_signature.data()),
                static_cast<int>(half), nullptr)};
  auto *s{BN_bin2bn(
      reinterpret_cast<const unsigned char *>(raw_signature.data() + half),
      static_cast<int>(half), nullptr)};
  if (r == nullptr || s == nullptr || ECDSA_SIG_set0(signature, r, s) != 1) {
    BN_free(r);
    BN_free(s);
    ECDSA_SIG_free(signature);
    return -1;
  }

  const auto length{i2d_ECDSA_SIG(signature, output)};
  ECDSA_SIG_free(signature);
  return length;
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
  if (signature.size() != curve_field_bytes(curve) * 2) {
    return false;
  }

  auto *key{make_ec_public_key(curve, coordinate_x, coordinate_y)};
  if (key == nullptr) {
    return false;
  }

  unsigned char *der_signature{nullptr};
  const auto der_length{encode_ecdsa_signature(signature, &der_signature)};
  auto result{false};
  if (der_length > 0) {
    auto *context{EVP_MD_CTX_new()};
    if (context != nullptr) {
      if (EVP_DigestVerifyInit(context, nullptr, to_message_digest(hash),
                               nullptr, key) == 1) {
        result =
            EVP_DigestVerify(
                context, der_signature, static_cast<std::size_t>(der_length),
                reinterpret_cast<const unsigned char *>(message.data()),
                message.size()) == 1;
      }

      EVP_MD_CTX_free(context);
    }
  }

  OPENSSL_free(der_signature);
  EVP_PKEY_free(key);
  return result;
}

} // namespace sourcemeta::core
