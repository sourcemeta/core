#include <sourcemeta/core/jose_encrypt.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/json.h>

#include "jose_jwe_internal.h"
#include "jose_key.h"
#include "jose_switch.h"

#include <optional>    // std::optional, std::nullopt
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

namespace {
using namespace std::string_view_literals;

constexpr auto HASH_ALG{sourcemeta::core::JSON::Object::hash("alg"sv)};
constexpr auto HASH_ENC{sourcemeta::core::JSON::Object::hash("enc"sv)};
constexpr auto HASH_CRIT{sourcemeta::core::JSON::Object::hash("crit"sv)};
constexpr auto HASH_ZIP{sourcemeta::core::JSON::Object::hash("zip"sv)};
constexpr auto HASH_APU{sourcemeta::core::JSON::Object::hash("apu"sv)};
constexpr auto HASH_APV{sourcemeta::core::JSON::Object::hash("apv"sv)};
constexpr auto HASH_KTY{sourcemeta::core::JSON::Object::hash("kty"sv)};
constexpr auto HASH_CRV{sourcemeta::core::JSON::Object::hash("crv"sv)};
constexpr auto HASH_X{sourcemeta::core::JSON::Object::hash("x"sv)};
constexpr auto HASH_Y{sourcemeta::core::JSON::Object::hash("y"sv)};

auto compact(const sourcemeta::core::JSON &document) -> std::string {
  std::ostringstream stream;
  sourcemeta::core::stringify(document, stream);
  return stream.str();
}

} // namespace

namespace sourcemeta::core {

auto jwe_encrypt(const JSON &header, const std::string_view plaintext,
                 const JWK &key, const RandomGenerator &generator)
    -> std::optional<std::string> {
  if (!header.is_object() || !header.unique_keys()) {
    return std::nullopt;
  }

  // Critical header extensions are not understood, so an object carrying them
  // is not emitted (RFC 7516 Section 4.1.13)
  if (header.try_at("crit", HASH_CRIT) != nullptr) {
    return std::nullopt;
  }

  // Compression is not implemented, so a request to compress is refused rather
  // than emitting an object that could not be decrypted here (RFC 7516 Section
  // 4.1.3)
  if (header.try_at("zip", HASH_ZIP) != nullptr) {
    return std::nullopt;
  }

  // The algorithm and content encryption are taken from the header (RFC 7516
  // Sections 4.1.1 and 4.1.2)
  const auto *algorithm_member{header.try_at("alg", HASH_ALG)};
  const auto *encryption_member{header.try_at("enc", HASH_ENC)};
  if (algorithm_member == nullptr || !algorithm_member->is_string() ||
      encryption_member == nullptr || !encryption_member->is_string()) {
    return std::nullopt;
  }

  const auto algorithm{to_jwe_algorithm(algorithm_member->to_string())};
  const auto encryption{to_jwe_encryption(encryption_member->to_string())};
  if (!algorithm.has_value() || !encryption.has_value()) {
    return std::nullopt;
  }

  const auto content_key_bytes{jwe_encryption_key_bytes(encryption.value())};

  // The protected header that is emitted and authenticated. Only ECDH-ES needs
  // to augment it (with the ephemeral public key), so the deep copy is deferred
  // to that branch and every other algorithm authenticates the caller header
  // directly
  const JSON *emitted_header{&header};
  JSON ecdh_header{nullptr};

  std::string content_key;
  std::string encrypted_key;

  // Asked of every algorithm, since the absence of a length is what tells an
  // algorithm that wraps a fresh content key from one that does not
  const auto key_wrap_bytes{jwe_key_wrap_bytes(algorithm.value())};

  switch (algorithm.value()) {
    SOURCEMETA_CORE_JOSE_FIRST_CASE(JWEAlgorithm::RSA_OAEP)
    case JWEAlgorithm::RSA_OAEP_256: {
      if (key.type() != JWK::Type::RSA || key.public_key() == nullptr) {
        return std::nullopt;
      }

      // The two RSA algorithms differ only in the OAEP hash (RFC 7518
      // Section 4.3)
      const auto oaep_hash{algorithm.value() == JWEAlgorithm::RSA_OAEP
                               ? RSAOAEPHash::SHA1
                               : RSAOAEPHash::SHA256};
      content_key = random_bytes(content_key_bytes, generator);
      // A content key is at most sixty four octets while the smallest modulus
      // this module accepts is two hundred and fifty six, so the padding
      // always has the room RFC 8017 Section 7.1.1 asks of it
      encrypted_key =
          rsa_oaep_encrypt(*key.public_key(), oaep_hash, content_key, generator)
              .value();
      break;
    }
    case JWEAlgorithm::A128KW:
    case JWEAlgorithm::A192KW:
    case JWEAlgorithm::A256KW: {
      // The shared secret is the key-encryption key, of the exact AES Key Wrap
      // size (RFC 7518 Section 4.4)
      if (key.type() != JWK::Type::Octet ||
          key.secret().size() != key_wrap_bytes.value()) {
        return std::nullopt;
      }

      content_key = random_bytes(content_key_bytes, generator);
      // The key-encryption key was held to an AES width just above and a
      // content key is a whole number of semiblocks of at least two, which is
      // everything RFC 3394 Section 2 asks of the pair
      encrypted_key = aes_key_wrap(key.secret(), content_key).value();
      break;
    }
    case JWEAlgorithm::DIR: {
      // The shared secret is used directly as the CEK (RFC 7518 Section 4.5)
      if (key.type() != JWK::Type::Octet ||
          key.secret().size() != content_key_bytes) {
        return std::nullopt;
      }

      content_key = std::string{key.secret()};
      break;
    }
    case JWEAlgorithm::ECDH_ES:
    case JWEAlgorithm::ECDH_ES_A128KW:
    case JWEAlgorithm::ECDH_ES_A192KW:
    case JWEAlgorithm::ECDH_ES_A256KW: {
      if (key.type() != JWK::Type::EllipticCurve ||
          key.public_key() == nullptr) {
        return std::nullopt;
      }

      // The key was held to its curve when it was built, so its components
      // read back
      const auto recipient{ec_public_components(*key.public_key()).value()};

      // A fresh ephemeral key on the recipient curve (RFC 7518 Section 4.6.1)
      const auto ephemeral{generate_ec_private_key(recipient.curve, generator)};
      if (!ephemeral.has_value()) {
        return std::nullopt;
      }

      // The public half of a key just built follows from it, its components
      // read back the same way the recipient's do, and the agreement pairs two
      // keys of the one curve, which is the only thing it turns away
      const auto ephemeral_public{derive_public_key(ephemeral.value()).value()};
      const auto ephemeral_components{
          ec_public_components(ephemeral_public).value()};
      const auto shared{
          ecdh_derive(ephemeral.value(), *key.public_key()).value()};

      std::string party_u;
      std::string party_v;
      if (!jwe_read_party_info(header, "apu", HASH_APU, party_u) ||
          !jwe_read_party_info(header, "apv", HASH_APV, party_v)) {
        return std::nullopt;
      }

      if (!key_wrap_bytes.has_value()) {
        // Direct Key Agreement: AlgorithmID is the "enc" value and the derived
        // key is the CEK (RFC 7518 Section 4.6.2)
        // Each length the derivation encodes is a field of thirty two bits,
        // and an identifier, party information and key width of that size are
        // all far beyond what a header carries (RFC 7518 Section 4.6.2)
        content_key =
            kdf_concat(shared, jwe_encryption_name(encryption.value()), party_u,
                       party_v, content_key_bytes)
                .value();
      } else {
        // Key Agreement with Key Wrapping: AlgorithmID is the "alg" value and
        // the derived key wraps a fresh CEK (RFC 7518 Section 4.6.2)
        // As above, and the wrapping is handed a key of the width it asked
        // for alongside a content key of whole semiblocks
        const auto key_encryption_key{
            kdf_concat(shared, jwe_algorithm_name(algorithm.value()), party_u,
                       party_v, key_wrap_bytes.value())
                .value()};
        content_key = random_bytes(content_key_bytes, generator);
        encrypted_key = aes_key_wrap(key_encryption_key, content_key).value();
      }

      // The ephemeral public key is a fresh object with known-unique members,
      // so each is inserted with its precomputed hash and no duplicate check
      auto ephemeral_key{JSON::make_object()};
      ephemeral_key.assign_if_nonempty("kty", HASH_KTY, "EC");
      ephemeral_key.assign_if_nonempty("crv", HASH_CRV,
                                       elliptic_curve_to_jwk(recipient.curve));
      ephemeral_key.assign_if_nonempty(
          "x", HASH_X, base64url_encode(ephemeral_components.x));
      ephemeral_key.assign_if_nonempty(
          "y", HASH_Y, base64url_encode(ephemeral_components.y));
      ecdh_header = header;
      ecdh_header.assign("epk", std::move(ephemeral_key));
      emitted_header = &ecdh_header;
      break;
    }
  }

  // The Additional Authenticated Data is the encoded protected header, which is
  // also the first compact segment (RFC 7516 Section 5.1 steps 13 and 14)
  const auto encoded_header{base64url_encode(compact(*emitted_header))};

  const auto initialization_vector{
      random_bytes(jwe_encryption_iv_bytes(encryption.value()), generator)};
  const auto content{jwe_content_encrypt(encryption.value(), content_key,
                                         initialization_vector, encoded_header,
                                         plaintext)};

  // Assemble the five compact segments (RFC 7516 Section 7.1), reserving the
  // exact length so the growing result never reallocates
  const auto encoded_key{base64url_encode(encrypted_key)};
  const auto encoded_iv{base64url_encode(initialization_vector)};
  const auto encoded_ciphertext{base64url_encode(content.ciphertext())};
  const auto encoded_tag{base64url_encode(content.tag())};

  std::string result;
  result.reserve(encoded_header.size() + encoded_key.size() +
                 encoded_iv.size() + encoded_ciphertext.size() +
                 encoded_tag.size() + 4);
  result.append(encoded_header);
  result.push_back('.');
  result.append(encoded_key);
  result.push_back('.');
  result.append(encoded_iv);
  result.push_back('.');
  result.append(encoded_ciphertext);
  result.push_back('.');
  result.append(encoded_tag);
  return result;
}

} // namespace sourcemeta::core
