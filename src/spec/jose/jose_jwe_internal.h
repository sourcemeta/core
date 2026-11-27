#ifndef SOURCEMETA_CORE_JOSE_JWE_INTERNAL_H_
#define SOURCEMETA_CORE_JOSE_JWE_INTERNAL_H_

// The content encryption and key-management dispatch shared by the JWE
// encryption and decryption paths (RFC 7516, RFC 7518)

#include "jose_switch.h"

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/jose_algorithm.h>

#include <sourcemeta/core/json.h>

#include <cstddef>     // std::size_t
#include <optional>    // std::optional, std::nullopt
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move, std::unreachable

namespace sourcemeta::core {

// The agreement party information is carried base64url-encoded in the header,
// but the Concat KDF consumes the raw octets (RFC 7518 Section 4.6.2). An
// absent parameter is the empty octet string; a present but malformed one is an
// error
inline auto jwe_read_party_info(const JSON &header, const JSON::StringView name,
                                const JSON::Object::hash_type &hash,
                                std::string &output) -> bool {
  const auto *member{header.try_at(name, hash)};
  if (member == nullptr) {
    output.clear();
    return true;
  }

  if (!member->is_string()) {
    return false;
  }

  auto decoded{base64url_decode(member->to_string())};
  if (!decoded.has_value()) {
    return false;
  }

  output = std::move(decoded).value();
  return true;
}

// The output of content encryption, holding the ciphertext followed by the
// authentication tag in one buffer as the AEAD primitives produce it, so the
// compact serialization can slice out each segment without a copy (RFC 7516
// Section 7.1)
struct JWEContent {
  std::string data;
  std::string::size_type tag_bytes;

  [[nodiscard]] auto ciphertext() const -> std::string_view {
    return std::string_view{this->data}.substr(0, this->data.size() -
                                                      this->tag_bytes);
  }

  [[nodiscard]] auto tag() const -> std::string_view {
    return std::string_view{this->data}.substr(this->data.size() -
                                               this->tag_bytes);
  }
};

// AES GCM takes a 96-bit initialization vector (RFC 7518 Section 5.3) and AES
// CBC a 128-bit one (RFC 7518 Section 5.2.2.1)
inline auto jwe_encryption_iv_bytes(const JWEEncryption encryption) noexcept
    -> std::size_t {
  switch (encryption) {
    SOURCEMETA_CORE_JOSE_FIRST_CASE(JWEEncryption::A128GCM)
    case JWEEncryption::A192GCM:
    case JWEEncryption::A256GCM:
      return 12;
    case JWEEncryption::A128CBC_HS256:
    case JWEEncryption::A192CBC_HS384:
    case JWEEncryption::A256CBC_HS512:
      return 16;
  }

  SOURCEMETA_CORE_JOSE_NO_OTHER_VALUE
}

// The AES Key Wrap key length for the wrapping algorithms, both the standalone
// A*KW and the key-wrapping variant of ECDH-ES (RFC 7518 Sections 4.4 and 4.6).
// An algorithm that wraps nothing answers with no length, which is what tells
// a direct key agreement from one that wraps
inline auto jwe_key_wrap_bytes(const JWEAlgorithm algorithm) noexcept
    -> std::optional<std::size_t> {
  switch (algorithm) {
    SOURCEMETA_CORE_JOSE_FIRST_CASE(JWEAlgorithm::A128KW)
    case JWEAlgorithm::ECDH_ES_A128KW:
      return 16;
    case JWEAlgorithm::A192KW:
    case JWEAlgorithm::ECDH_ES_A192KW:
      return 24;
    case JWEAlgorithm::A256KW:
    case JWEAlgorithm::ECDH_ES_A256KW:
      return 32;
    case JWEAlgorithm::RSA_OAEP:
    case JWEAlgorithm::RSA_OAEP_256:
    case JWEAlgorithm::ECDH_ES:
    case JWEAlgorithm::DIR:
      break;
  }

  return std::nullopt;
}

// Seal the plaintext with the content encryption key, dispatching to the AEAD
// scheme the "enc" algorithm names (RFC 7518 Sections 5.2 and 5.3)
inline auto jwe_content_encrypt(const JWEEncryption encryption,
                                const std::string_view key,
                                const std::string_view iv,
                                const std::string_view associated_data,
                                const std::string_view plaintext)
    -> JWEContent {
  switch (encryption) {
    SOURCEMETA_CORE_JOSE_FIRST_CASE(JWEEncryption::A128GCM)
    case JWEEncryption::A192GCM:
    case JWEEncryption::A256GCM: {
      // The key is the width the scheme names and the initialization vector
      // the twelve octets it asks for, which is all the sealing turns away
      auto result{aes_gcm_encrypt(key, iv, associated_data, plaintext).value()};
      // The GCM tag is always 16 octets (RFC 7518 Section 5.3)
      return JWEContent{.data = std::move(result).data, .tag_bytes = 16};
    }
    case JWEEncryption::A128CBC_HS256:
    case JWEEncryption::A192CBC_HS384:
    case JWEEncryption::A256CBC_HS512:
      break;
  }

  // As above, with the sixteen octet initialization vector this scheme asks
  // for instead (RFC 7518 Section 5.2.2.1)
  auto result{
      aes_cbc_hmac_encrypt(key, iv, associated_data, plaintext).value()};
  const auto tag_bytes{result.tag_length};
  return JWEContent{.data = std::move(result).data, .tag_bytes = tag_bytes};
}

// Open the ciphertext with the content encryption key, verifying the tag before
// returning the plaintext (RFC 7518 Sections 5.2 and 5.3)
inline auto jwe_content_decrypt(const JWEEncryption encryption,
                                const std::string_view key,
                                const std::string_view iv,
                                const std::string_view associated_data,
                                const std::string_view ciphertext,
                                const std::string_view tag)
    -> std::optional<std::string> {
  switch (encryption) {
    SOURCEMETA_CORE_JOSE_FIRST_CASE(JWEEncryption::A128GCM)
    case JWEEncryption::A192GCM:
    case JWEEncryption::A256GCM:
      return aes_gcm_decrypt(key, iv, associated_data, ciphertext, tag);
    case JWEEncryption::A128CBC_HS256:
    case JWEEncryption::A192CBC_HS384:
    case JWEEncryption::A256CBC_HS512:
      break;
  }

  return aes_cbc_hmac_decrypt(key, iv, associated_data, ciphertext, tag);
}

} // namespace sourcemeta::core

#endif
