#include <sourcemeta/core/crypto_sha384.h>

#include <array>   // std::array
#include <cstdint> // std::uint8_t

#if defined(SOURCEMETA_CORE_CRYPTO_USE_SYSTEM_OPENSSL)
#include <openssl/evp.h> // EVP_*
#include <stdexcept>     // std::runtime_error
#elif defined(__APPLE__)
#include <CommonCrypto/CommonDigest.h> // CC_SHA384*, CC_SHA512_CTX, CC_LONG

#include <cstddef> // std::size_t
#include <limits>  // std::numeric_limits
#elif defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h> // ULONG

#include <bcrypt.h> // BCrypt*, BCRYPT_*

#include <cstddef>   // std::size_t
#include <limits>    // std::numeric_limits
#include <stdexcept> // std::runtime_error
#else
#include "crypto_sha2_64.h"

#include <cstddef> // std::size_t
#include <cstdint> // std::uint64_t
#endif

namespace {
constexpr std::array<char, 17> HEX_DIGITS{{'0', '1', '2', '3', '4', '5', '6',
                                           '7', '8', '9', 'a', 'b', 'c', 'd',
                                           'e', 'f', '\0'}};
} // namespace

#if defined(SOURCEMETA_CORE_CRYPTO_USE_SYSTEM_OPENSSL)

namespace sourcemeta::core {

auto sha384_digest(const std::string_view input)
    -> std::array<std::uint8_t, 48> {
  auto *context = EVP_MD_CTX_new();
  if (context == nullptr) {
    throw std::runtime_error("Could not allocate OpenSSL digest context");
  }

  if (EVP_DigestInit_ex(context, EVP_sha384(), nullptr) != 1 ||
      EVP_DigestUpdate(context, input.data(), input.size()) != 1) {
    EVP_MD_CTX_free(context);
    throw std::runtime_error("Could not compute SHA-384 digest");
  }

  std::array<std::uint8_t, 48> digest{};
  unsigned int length = 0;
  if (EVP_DigestFinal_ex(context, digest.data(), &length) != 1) {
    EVP_MD_CTX_free(context);
    throw std::runtime_error("Could not finalize SHA-384 digest");
  }

  EVP_MD_CTX_free(context);
  return digest;
}

} // namespace sourcemeta::core

#elif defined(__APPLE__)

namespace sourcemeta::core {

auto sha384_digest(const std::string_view input)
    -> std::array<std::uint8_t, 48> {
  CC_SHA512_CTX context;
  CC_SHA384_Init(&context);

  // The platform update interface takes a 32-bit length, so larger
  // inputs must be fed in chunks
  const auto *remaining_data{input.data()};
  auto remaining_size{input.size()};
  constexpr std::size_t maximum_chunk{std::numeric_limits<CC_LONG>::max()};
  while (remaining_size > 0) {
    const auto chunk_size{remaining_size > maximum_chunk ? maximum_chunk
                                                         : remaining_size};
    CC_SHA384_Update(&context, remaining_data,
                     static_cast<CC_LONG>(chunk_size));
    remaining_data += chunk_size;
    remaining_size -= chunk_size;
  }

  std::array<std::uint8_t, 48> digest{};
  CC_SHA384_Final(digest.data(), &context);
  return digest;
}

} // namespace sourcemeta::core

#elif defined(_WIN32)

namespace sourcemeta::core {

auto sha384_digest(const std::string_view input)
    -> std::array<std::uint8_t, 48> {
  BCRYPT_ALG_HANDLE algorithm{nullptr};
  if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(
          &algorithm, BCRYPT_SHA384_ALGORITHM, nullptr, 0))) {
    throw std::runtime_error("Could not open the CNG SHA-384 provider");
  }

  BCRYPT_HASH_HANDLE hash{nullptr};
  if (!BCRYPT_SUCCESS(
          BCryptCreateHash(algorithm, &hash, nullptr, 0, nullptr, 0, 0))) {
    BCryptCloseAlgorithmProvider(algorithm, 0);
    throw std::runtime_error("Could not create the CNG SHA-384 hash");
  }

  // The data interface is not const-qualified but never writes through
  // the pointer, and it takes a 32-bit length, so larger inputs must be
  // fed in chunks
  auto *remaining_data{
      reinterpret_cast<unsigned char *>(const_cast<char *>(input.data()))};
  auto remaining_size{input.size()};
  constexpr std::size_t maximum_chunk{std::numeric_limits<ULONG>::max()};
  auto success{true};
  while (remaining_size > 0 && success) {
    const auto chunk_size{remaining_size > maximum_chunk ? maximum_chunk
                                                         : remaining_size};
    success = BCRYPT_SUCCESS(BCryptHashData(hash, remaining_data,
                                            static_cast<ULONG>(chunk_size), 0));
    remaining_data += chunk_size;
    remaining_size -= chunk_size;
  }

  std::array<std::uint8_t, 48> digest{};
  if (success) {
    success = BCRYPT_SUCCESS(BCryptFinishHash(
        hash, digest.data(), static_cast<ULONG>(digest.size()), 0));
  }

  BCryptDestroyHash(hash);
  BCryptCloseAlgorithmProvider(algorithm, 0);
  if (!success) {
    throw std::runtime_error("Could not compute the CNG SHA-384 digest");
  }

  return digest;
}

} // namespace sourcemeta::core

#else

namespace sourcemeta::core {

auto sha384_digest(const std::string_view input)
    -> std::array<std::uint8_t, 48> {
  // Initial hash values: first 64 bits of the fractional parts of the
  // square roots of the 9th through 16th primes (FIPS 180-4 Section 5.3.4)
  std::array<std::uint64_t, 8> state{
      {0xcbbb9d5dc1059ed8U, 0x629a292a367cd507U, 0x9159015a3070dd17U,
       0x152fecd8f70e5939U, 0x67332667ffc00b31U, 0x8eb44a8768581511U,
       0xdb0c2e0d64f98fa7U, 0x47b5481dbefa4fa4U}};

  sha2_64_hash(input, state);

  // The digest is the leftmost 384 bits of the final state
  // (FIPS 180-4 Section 6.5)
  std::array<std::uint8_t, 48> digest{};
  for (std::size_t word_index = 0u; word_index < 6u; ++word_index) {
    for (std::size_t byte_index = 0u; byte_index < 8u; ++byte_index) {
      digest[(word_index * 8u) + byte_index] = static_cast<std::uint8_t>(
          (state[word_index] >> (8u * (7u - byte_index))) & 0xffu);
    }
  }

  return digest;
}

} // namespace sourcemeta::core

#endif

namespace sourcemeta::core {

auto sha384(const std::string_view input) -> std::string {
  const auto digest{sha384_digest(input)};
  std::string result;
  result.reserve(96);
  for (const auto byte : digest) {
    result.push_back(HEX_DIGITS[(byte >> 4u) & 0x0fu]);
    result.push_back(HEX_DIGITS[byte & 0x0fu]);
  }

  return result;
}

auto sha384(const std::string_view input, std::ostream &output) -> void {
  const auto result = sha384(input);
  output.write(result.data(), static_cast<std::streamsize>(result.size()));
}

} // namespace sourcemeta::core
