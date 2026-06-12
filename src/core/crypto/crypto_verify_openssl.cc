#include <sourcemeta/core/crypto_verify.h>

#include <openssl/bn.h>          // BN_*
#include <openssl/core_names.h>  // OSSL_PKEY_PARAM_*
#include <openssl/evp.h>         // EVP_*
#include <openssl/param_build.h> // OSSL_PARAM_*

#include <cstddef>     // std::size_t
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

  auto *key{make_rsa_public_key(stripped_modulus, stripped_exponent)};
  if (key == nullptr) {
    return false;
  }

  auto result{false};
  auto *context{EVP_MD_CTX_new()};
  if (context != nullptr) {
    if (EVP_DigestVerifyInit(context, nullptr, to_message_digest(hash), nullptr,
                             key) == 1) {
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

} // namespace sourcemeta::core
