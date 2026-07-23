#include <sourcemeta/core/crypto_ecdh.h>

#include <openssl/evp.h> // EVP_*

#include <cstddef>  // std::size_t
#include <optional> // std::optional, std::nullopt
#include <string>   // std::string
#include <utility>  // std::move

namespace sourcemeta::core {

// The layout matches the definitions in the sibling OpenSSL key backends, since
// each translation unit that reads a key redeclares its file-private members
struct PublicKey::Internal {
  PublicKey::Type kind;
  EVP_PKEY *key;
  std::string modulus;
  std::size_t field_bytes;
  std::size_t signature_bytes;
};

struct PrivateKey::Internal {
  PrivateKey::Type kind;
  EVP_PKEY *key;
  std::size_t field_bytes;
  bool rsa_pss_restricted{false};
};

auto ecdh_derive(const PrivateKey &private_key, const PublicKey &public_key)
    -> std::optional<std::string> {
  const auto *const private_internal{private_key.internal()};
  const auto *const public_internal{public_key.internal()};
  if (private_internal == nullptr || public_internal == nullptr ||
      private_internal->kind != PrivateKey::Type::EllipticCurve ||
      public_internal->kind != PublicKey::Type::EllipticCurve ||
      private_internal->field_bytes != public_internal->field_bytes) {
    return std::nullopt;
  }

  auto *context{EVP_PKEY_CTX_new(private_internal->key, nullptr)};
  if (context == nullptr) {
    return std::nullopt;
  }

  std::optional<std::string> result;
  std::size_t length{0};
  // The peer key was validated on the curve when it was parsed, and setting it
  // as the derivation peer checks it again
  if (EVP_PKEY_derive_init(context) == 1 &&
      EVP_PKEY_derive_set_peer(context, public_internal->key) == 1 &&
      EVP_PKEY_derive(context, nullptr, &length) == 1) {
    std::string secret(length, '\x00');
    if (EVP_PKEY_derive(context,
                        reinterpret_cast<unsigned char *>(secret.data()),
                        &length) == 1) {
      secret.resize(length);
      result = std::move(secret);
    }
  }

  EVP_PKEY_CTX_free(context);
  return result;
}

} // namespace sourcemeta::core
