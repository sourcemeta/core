#include <sourcemeta/core/crypto_verify.h>
#include <sourcemeta/core/text.h>

#include "crypto_verify_util.h"

#include <CoreFoundation/CoreFoundation.h> // CF*, kCF*
#include <Security/Security.h>             // Sec*, kSec*

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::unreachable

namespace {

auto der_append_length(std::string &output, const std::size_t length) -> void {
  if (length < 128) {
    output.push_back(static_cast<char>(length));
  } else if (length < 256) {
    output.push_back('\x81');
    output.push_back(static_cast<char>(length));
  } else {
    output.push_back('\x82');
    output.push_back(static_cast<char>((length >> 8u) & 0xffu));
    output.push_back(static_cast<char>(length & 0xffu));
  }
}

auto der_append_unsigned_integer(std::string &output, std::string_view value)
    -> void {
  while (!value.empty() && value.front() == '\x00') {
    value.remove_prefix(1);
  }

  // A leading zero byte keeps the value positive when its high bit is set,
  // and represents the value zero when nothing remains
  const auto needs_zero_prefix{
      value.empty() ||
      (static_cast<unsigned char>(value.front()) & 0x80u) != 0};
  output.push_back('\x02');
  der_append_length(output, value.size() + (needs_zero_prefix ? 1 : 0));
  if (needs_zero_prefix) {
    output.push_back('\x00');
  }

  output.append(value);
}

auto to_sec_key_pkcs1_v15_algorithm(
    const sourcemeta::core::SignatureHashFunction hash) noexcept
    -> SecKeyAlgorithm {
  switch (hash) {
    case sourcemeta::core::SignatureHashFunction::SHA256:
      return kSecKeyAlgorithmRSASignatureMessagePKCS1v15SHA256;
    case sourcemeta::core::SignatureHashFunction::SHA384:
      return kSecKeyAlgorithmRSASignatureMessagePKCS1v15SHA384;
    case sourcemeta::core::SignatureHashFunction::SHA512:
      return kSecKeyAlgorithmRSASignatureMessagePKCS1v15SHA512;
  }

  std::unreachable();
}

// These algorithm variants fix the salt length to the hash function
// output, which is exactly what RFC 7518 Section 3.5 requires
auto to_sec_key_pss_algorithm(
    const sourcemeta::core::SignatureHashFunction hash) noexcept
    -> SecKeyAlgorithm {
  switch (hash) {
    case sourcemeta::core::SignatureHashFunction::SHA256:
      return kSecKeyAlgorithmRSASignatureMessagePSSSHA256;
    case sourcemeta::core::SignatureHashFunction::SHA384:
      return kSecKeyAlgorithmRSASignatureMessagePSSSHA384;
    case sourcemeta::core::SignatureHashFunction::SHA512:
      return kSecKeyAlgorithmRSASignatureMessagePSSSHA512;
  }

  std::unreachable();
}

auto make_data(const std::string_view value) -> CFDataRef {
  return CFDataCreate(kCFAllocatorDefault,
                      reinterpret_cast<const UInt8 *>(value.data()),
                      static_cast<CFIndex>(value.size()));
}

auto make_rsa_public_key(const std::string_view modulus,
                         const std::string_view exponent) -> SecKeyRef {
  // The platform expects the PKCS#1 RSAPublicKey structure, a DER sequence
  // of the modulus and exponent integers (RFC 8017 Appendix A.1.1)
  std::string body;
  der_append_unsigned_integer(body, modulus);
  der_append_unsigned_integer(body, exponent);
  std::string der;
  der.push_back('\x30');
  der_append_length(der, body.size());
  der.append(body);

  auto key_data{make_data(der)};
  if (key_data == nullptr) {
    return nullptr;
  }

  std::array<const void *, 2> attribute_keys{
      {kSecAttrKeyType, kSecAttrKeyClass}};
  std::array<const void *, 2> attribute_values{
      {kSecAttrKeyTypeRSA, kSecAttrKeyClassPublic}};
  auto attributes{CFDictionaryCreate(
      kCFAllocatorDefault, attribute_keys.data(), attribute_values.data(),
      static_cast<CFIndex>(attribute_keys.size()),
      &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks)};
  if (attributes == nullptr) {
    CFRelease(key_data);
    return nullptr;
  }

  auto key{SecKeyCreateWithData(key_data, attributes, nullptr)};
  CFRelease(attributes);
  CFRelease(key_data);
  return key;
}

auto verify_rsa_signature(const SecKeyAlgorithm algorithm,
                          const std::string_view modulus,
                          const std::string_view exponent,
                          const std::string_view message,
                          const std::string_view signature) -> bool {
  const auto stripped_modulus{sourcemeta::core::strip_left(modulus, '\x00')};
  const auto stripped_exponent{sourcemeta::core::strip_left(exponent, '\x00')};
  if (stripped_modulus.empty() || stripped_exponent.empty() ||
      stripped_modulus.size() > sourcemeta::core::MAXIMUM_KEY_BYTES ||
      stripped_exponent.size() > sourcemeta::core::MAXIMUM_KEY_BYTES) {
    return false;
  }

  auto key{make_rsa_public_key(stripped_modulus, stripped_exponent)};
  if (key == nullptr) {
    return false;
  }

  auto message_data{make_data(message)};
  auto signature_data{make_data(signature)};
  auto result{false};
  if (message_data != nullptr && signature_data != nullptr) {
    result = SecKeyVerifySignature(key, algorithm, message_data, signature_data,
                                   nullptr) == true;
  }

  if (signature_data != nullptr) {
    CFRelease(signature_data);
  }

  if (message_data != nullptr) {
    CFRelease(message_data);
  }

  CFRelease(key);
  return result;
}

} // namespace

namespace sourcemeta::core {

auto rsassa_pkcs1_v15_verify(const SignatureHashFunction hash,
                             const std::string_view modulus,
                             const std::string_view exponent,
                             const std::string_view message,
                             const std::string_view signature) -> bool {
  return verify_rsa_signature(to_sec_key_pkcs1_v15_algorithm(hash), modulus,
                              exponent, message, signature);
}

auto rsassa_pss_verify(const SignatureHashFunction hash,
                       const std::string_view modulus,
                       const std::string_view exponent,
                       const std::string_view message,
                       const std::string_view signature) -> bool {
  return verify_rsa_signature(to_sec_key_pss_algorithm(hash), modulus, exponent,
                              message, signature);
}

} // namespace sourcemeta::core
