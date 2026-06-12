#include <sourcemeta/core/crypto_sha512.h>

#include <array>   // std::array
#include <cstdint> // std::uint8_t

#if defined(SOURCEMETA_CORE_CRYPTO_USE_SYSTEM_OPENSSL)
#include <openssl/evp.h> // EVP_*
#include <stdexcept>     // std::runtime_error
#elif defined(__APPLE__)
#include <CommonCrypto/CommonDigest.h> // CC_SHA512*, CC_LONG

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

auto sha512_digest(const std::string_view input)
    -> std::array<std::uint8_t, 64> {
  auto *context = EVP_MD_CTX_new();
  if (context == nullptr) {
    throw std::runtime_error("Could not allocate OpenSSL digest context");
  }

  if (EVP_DigestInit_ex(context, EVP_sha512(), nullptr) != 1 ||
      EVP_DigestUpdate(context, input.data(), input.size()) != 1) {
    EVP_MD_CTX_free(context);
    throw std::runtime_error("Could not compute SHA-512 digest");
  }

  std::array<std::uint8_t, 64> digest{};
  unsigned int length = 0;
  if (EVP_DigestFinal_ex(context, digest.data(), &length) != 1) {
    EVP_MD_CTX_free(context);
    throw std::runtime_error("Could not finalize SHA-512 digest");
  }

  EVP_MD_CTX_free(context);
  return digest;
}

} // namespace sourcemeta::core

#elif defined(__APPLE__)

namespace sourcemeta::core {

auto sha512_digest(const std::string_view input)
    -> std::array<std::uint8_t, 64> {
  CC_SHA512_CTX context;
  CC_SHA512_Init(&context);

  // The platform update interface takes a 32-bit length, so larger
  // inputs must be fed in chunks
  const auto *remaining_data{input.data()};
  auto remaining_size{input.size()};
  constexpr std::size_t maximum_chunk{std::numeric_limits<CC_LONG>::max()};
  while (remaining_size > 0) {
    const auto chunk_size{remaining_size > maximum_chunk ? maximum_chunk
                                                         : remaining_size};
    CC_SHA512_Update(&context, remaining_data,
                     static_cast<CC_LONG>(chunk_size));
    remaining_data += chunk_size;
    remaining_size -= chunk_size;
  }

  std::array<std::uint8_t, 64> digest{};
  CC_SHA512_Final(digest.data(), &context);
  return digest;
}

} // namespace sourcemeta::core

#elif defined(_WIN32)

namespace sourcemeta::core {

auto sha512_digest(const std::string_view input)
    -> std::array<std::uint8_t, 64> {
  BCRYPT_ALG_HANDLE algorithm{nullptr};
  if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(
          &algorithm, BCRYPT_SHA512_ALGORITHM, nullptr, 0))) {
    throw std::runtime_error("Could not open the CNG SHA-512 provider");
  }

  BCRYPT_HASH_HANDLE hash{nullptr};
  if (!BCRYPT_SUCCESS(
          BCryptCreateHash(algorithm, &hash, nullptr, 0, nullptr, 0, 0))) {
    BCryptCloseAlgorithmProvider(algorithm, 0);
    throw std::runtime_error("Could not create the CNG SHA-512 hash");
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

  std::array<std::uint8_t, 64> digest{};
  if (success) {
    success = BCRYPT_SUCCESS(BCryptFinishHash(
        hash, digest.data(), static_cast<ULONG>(digest.size()), 0));
  }

  BCryptDestroyHash(hash);
  BCryptCloseAlgorithmProvider(algorithm, 0);
  if (!success) {
    throw std::runtime_error("Could not compute the CNG SHA-512 digest");
  }

  return digest;
}

} // namespace sourcemeta::core

#else

namespace sourcemeta::core {

auto sha512_digest(const std::string_view input)
    -> std::array<std::uint8_t, 64> {
  // Initial hash values: first 64 bits of the fractional parts of the
  // square roots of the first 8 primes (FIPS 180-4 Section 5.3.5)
  std::array<std::uint64_t, 8> state{
      {0x6a09e667f3bcc908U, 0xbb67ae8584caa73bU, 0x3c6ef372fe94f82bU,
       0xa54ff53a5f1d36f1U, 0x510e527fade682d1U, 0x9b05688c2b3e6c1fU,
       0x1f83d9abfb41bd6bU, 0x5be0cd19137e2179U}};

  sha2_64_hash(input, state);

  std::array<std::uint8_t, 64> digest{};
  for (std::size_t word_index = 0u; word_index < 8u; ++word_index) {
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

auto sha512(const std::string_view input) -> std::string {
  const auto digest{sha512_digest(input)};
  std::string result;
  result.reserve(128);
  for (const auto byte : digest) {
    result.push_back(HEX_DIGITS[(byte >> 4u) & 0x0fu]);
    result.push_back(HEX_DIGITS[byte & 0x0fu]);
  }

  return result;
}

auto sha512(const std::string_view input, std::ostream &output) -> void {
  const auto result = sha512(input);
  output.write(result.data(), static_cast<std::streamsize>(result.size()));
}

} // namespace sourcemeta::core
