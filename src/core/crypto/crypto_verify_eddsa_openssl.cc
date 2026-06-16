#include <sourcemeta/core/crypto_verify.h>

#include "crypto_helpers.h"

#include <openssl/evp.h> // EVP_*

#include <string_view> // std::string_view
#include <utility>     // std::unreachable

namespace {

auto to_pkey_id(const sourcemeta::core::EdwardsCurve curve) noexcept -> int {
  switch (curve) {
    case sourcemeta::core::EdwardsCurve::Ed25519:
      return EVP_PKEY_ED25519;
    case sourcemeta::core::EdwardsCurve::Ed448:
      return EVP_PKEY_ED448;
  }

  std::unreachable();
}

} // namespace

namespace sourcemeta::core {

auto eddsa_verify(const EdwardsCurve curve, const std::string_view public_key,
                  const std::string_view message,
                  const std::string_view signature) -> bool {
  if (public_key.size() != eddsa_public_key_bytes(curve) ||
      signature.size() != eddsa_signature_bytes(curve)) {
    return false;
  }

  auto *key{EVP_PKEY_new_raw_public_key(
      to_pkey_id(curve), nullptr,
      reinterpret_cast<const unsigned char *>(public_key.data()),
      public_key.size())};
  if (key == nullptr) {
    return false;
  }

  auto result{false};
  auto *context{EVP_MD_CTX_new()};
  if (context != nullptr) {
    // EdDSA is a one-shot verification with a null digest, since the curve
    // fixes the hash function internally
    if (EVP_DigestVerifyInit(context, nullptr, nullptr, nullptr, key) == 1) {
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
