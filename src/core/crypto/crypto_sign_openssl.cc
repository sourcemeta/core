#include <sourcemeta/core/crypto_sign.h>

#include <openssl/bn.h>  // BN_bn2binpad
#include <openssl/ec.h>  // ECDSA_SIG, d2i_ECDSA_SIG, ECDSA_SIG_get0_*
#include <openssl/evp.h> // EVP_*
#include <openssl/pem.h> // PEM_read_bio_PrivateKey, BIO_*
#include <openssl/rsa.h> // RSA_PKCS1_PSS_PADDING, RSA_PSS_SALTLEN_DIGEST

#include <cstddef>     // std::size_t
#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move, std::unreachable

namespace sourcemeta::core {

// The parsed key keeps the native handle alive so that many signatures are
// produced without rebuilding it
struct PrivateKey::Internal {
  PrivateKey::Type kind;
  EVP_PKEY *key;
  // The field width for the elliptic curve raw signature encoding
  std::size_t field_bytes;
};

} // namespace sourcemeta::core

namespace {

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

// Refuse encrypted documents by offering no password, rather than letting the
// default callback prompt on the terminal
auto no_password(char *, int, int, void *) -> int { return 0; }

auto sign_digest(EVP_MD_CTX *context, const std::string_view message)
    -> std::optional<std::string> {
  std::size_t length{0};
  if (EVP_DigestSign(context, nullptr, &length,
                     reinterpret_cast<const unsigned char *>(message.data()),
                     message.size()) != 1) {
    return std::nullopt;
  }

  std::string signature(length, '\x00');
  if (EVP_DigestSign(
          context, reinterpret_cast<unsigned char *>(signature.data()), &length,
          reinterpret_cast<const unsigned char *>(message.data()),
          message.size()) != 1) {
    return std::nullopt;
  }

  signature.resize(length);
  return signature;
}

auto sign_rsa(EVP_PKEY *key, const sourcemeta::core::SignatureHashFunction hash,
              const std::string_view message, const bool probabilistic)
    -> std::optional<std::string> {
  std::optional<std::string> result;
  auto *context{EVP_MD_CTX_new()};
  if (context != nullptr) {
    EVP_PKEY_CTX *key_context{nullptr};
    auto ready{EVP_DigestSignInit(context, &key_context,
                                  to_message_digest(hash), nullptr, key) == 1};
    if (ready && probabilistic) {
      ready = EVP_PKEY_CTX_set_rsa_padding(key_context,
                                           RSA_PKCS1_PSS_PADDING) == 1 &&
              EVP_PKEY_CTX_set_rsa_pss_saltlen(key_context,
                                               RSA_PSS_SALTLEN_DIGEST) == 1;
    }

    if (ready) {
      result = sign_digest(context, message);
    }

    EVP_MD_CTX_free(context);
  }

  return result;
}

// Convert the DER ECDSA signature that the platform produces into the raw
// fixed-width R || S concatenation that JWS mandates (RFC 7518 Section 3.4)
auto encode_ecdsa_signature(const std::string_view der,
                            const std::size_t field_bytes)
    -> std::optional<std::string> {
  const auto *pointer{reinterpret_cast<const unsigned char *>(der.data())};
  auto *signature{
      d2i_ECDSA_SIG(nullptr, &pointer, static_cast<long>(der.size()))};
  if (signature == nullptr) {
    return std::nullopt;
  }

  const auto width{static_cast<int>(field_bytes)};
  std::string raw(field_bytes * 2, '\x00');
  auto *raw_data{reinterpret_cast<unsigned char *>(raw.data())};
  const auto ok{
      BN_bn2binpad(ECDSA_SIG_get0_r(signature), raw_data, width) == width &&
      BN_bn2binpad(ECDSA_SIG_get0_s(signature), raw_data + width, width) ==
          width};
  ECDSA_SIG_free(signature);
  if (!ok) {
    return std::nullopt;
  }

  return raw;
}

} // namespace

namespace sourcemeta::core {

PrivateKey::PrivateKey(Internal *internal) noexcept : internal_{internal} {}

PrivateKey::~PrivateKey() {
  if (internal_ != nullptr) {
    EVP_PKEY_free(internal_->key);
    delete internal_;
  }
}

PrivateKey::PrivateKey(PrivateKey &&other) noexcept
    : internal_{other.internal_} {
  other.internal_ = nullptr;
}

auto PrivateKey::operator=(PrivateKey &&other) noexcept -> PrivateKey & {
  if (this != &other) {
    if (internal_ != nullptr) {
      EVP_PKEY_free(internal_->key);
      delete internal_;
    }

    internal_ = other.internal_;
    other.internal_ = nullptr;
  }

  return *this;
}

auto PrivateKey::type() const noexcept -> Type { return internal_->kind; }

auto make_private_key(const std::string_view pem) -> std::optional<PrivateKey> {
  auto *bio{BIO_new_mem_buf(pem.data(), static_cast<int>(pem.size()))};
  if (bio == nullptr) {
    return std::nullopt;
  }

  auto *key{PEM_read_bio_PrivateKey(bio, nullptr, no_password, nullptr)};
  BIO_free(bio);
  if (key == nullptr) {
    return std::nullopt;
  }

  PrivateKey::Type kind{};
  std::size_t field_bytes{0};
  switch (EVP_PKEY_get_base_id(key)) {
    case EVP_PKEY_RSA:
    case EVP_PKEY_RSA_PSS:
      kind = PrivateKey::Type::RSA;
      break;
    case EVP_PKEY_EC:
      kind = PrivateKey::Type::EllipticCurve;
      field_bytes = static_cast<std::size_t>((EVP_PKEY_get_bits(key) + 7) / 8);
      break;
    case EVP_PKEY_ED25519:
    case EVP_PKEY_ED448:
      kind = PrivateKey::Type::Edwards;
      break;
    default:
      EVP_PKEY_free(key);
      return std::nullopt;
  }

  return PrivateKey{new PrivateKey::Internal{
      .kind = kind, .key = key, .field_bytes = field_bytes}};
}

auto rsassa_pkcs1_v15_sign(const PrivateKey &key,
                           const SignatureHashFunction hash,
                           const std::string_view message)
    -> std::optional<std::string> {
  const auto *internal{key.internal()};
  if (internal == nullptr || internal->kind != PrivateKey::Type::RSA) {
    return std::nullopt;
  }

  return sign_rsa(internal->key, hash, message, false);
}

auto rsassa_pss_sign(const PrivateKey &key, const SignatureHashFunction hash,
                     const std::string_view message)
    -> std::optional<std::string> {
  const auto *internal{key.internal()};
  if (internal == nullptr || internal->kind != PrivateKey::Type::RSA) {
    return std::nullopt;
  }

  return sign_rsa(internal->key, hash, message, true);
}

auto ecdsa_sign(const PrivateKey &key, const SignatureHashFunction hash,
                const std::string_view message) -> std::optional<std::string> {
  const auto *internal{key.internal()};
  if (internal == nullptr ||
      internal->kind != PrivateKey::Type::EllipticCurve) {
    return std::nullopt;
  }

  std::optional<std::string> der;
  auto *context{EVP_MD_CTX_new()};
  if (context != nullptr) {
    if (EVP_DigestSignInit(context, nullptr, to_message_digest(hash), nullptr,
                           internal->key) == 1) {
      der = sign_digest(context, message);
    }

    EVP_MD_CTX_free(context);
  }

  if (!der.has_value()) {
    return std::nullopt;
  }

  return encode_ecdsa_signature(der.value(), internal->field_bytes);
}

auto eddsa_sign(const PrivateKey &key, const std::string_view message)
    -> std::optional<std::string> {
  const auto *internal{key.internal()};
  if (internal == nullptr || internal->kind != PrivateKey::Type::Edwards) {
    return std::nullopt;
  }

  std::optional<std::string> result;
  auto *context{EVP_MD_CTX_new()};
  if (context != nullptr) {
    // EdDSA is a one-shot signature with a null digest, since the curve fixes
    // the hash function internally
    if (EVP_DigestSignInit(context, nullptr, nullptr, nullptr, internal->key) ==
        1) {
      result = sign_digest(context, message);
    }

    EVP_MD_CTX_free(context);
  }

  return result;
}

} // namespace sourcemeta::core
