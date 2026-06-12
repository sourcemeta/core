#include <sourcemeta/core/crypto_sha1.h>

#include <array>   // std::array
#include <cstdint> // std::uint32_t, std::uint64_t

#if defined(SOURCEMETA_CORE_CRYPTO_USE_SYSTEM_OPENSSL)
#include <openssl/evp.h> // EVP_MD_CTX_new, EVP_DigestInit_ex, EVP_sha1, EVP_DigestUpdate, EVP_DigestFinal_ex, EVP_MD_CTX_free
#include <stdexcept>     // std::runtime_error
#elif defined(__APPLE__)
#include <CommonCrypto/CommonDigest.h> // CC_SHA1*, CC_LONG

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
#include <cstring> // std::memcpy
#endif

namespace {
constexpr std::array<char, 17> HEX_DIGITS{{'0', '1', '2', '3', '4', '5', '6',
                                           '7', '8', '9', 'a', 'b', 'c', 'd',
                                           'e', 'f', '\0'}};
} // namespace

#if defined(SOURCEMETA_CORE_CRYPTO_USE_SYSTEM_OPENSSL)

namespace sourcemeta::core {

auto sha1(const std::string_view input) -> std::string {
  auto *context = EVP_MD_CTX_new();
  if (context == nullptr) {
    throw std::runtime_error("Could not allocate OpenSSL digest context");
  }

  if (EVP_DigestInit_ex(context, EVP_sha1(), nullptr) != 1 ||
      EVP_DigestUpdate(context, input.data(), input.size()) != 1) {
    EVP_MD_CTX_free(context);
    throw std::runtime_error("Could not compute SHA-1 digest");
  }

  std::array<unsigned char, 20> digest{};
  unsigned int length = 0;
  if (EVP_DigestFinal_ex(context, digest.data(), &length) != 1) {
    EVP_MD_CTX_free(context);
    throw std::runtime_error("Could not finalize SHA-1 digest");
  }

  EVP_MD_CTX_free(context);

  std::string result;
  result.reserve(40);
  for (std::uint64_t index = 0; index < 20u; ++index) {
    result.push_back(HEX_DIGITS[(digest[index] >> 4u) & 0x0fu]);
    result.push_back(HEX_DIGITS[digest[index] & 0x0fu]);
  }

  return result;
}

} // namespace sourcemeta::core

#elif defined(__APPLE__)

namespace sourcemeta::core {

auto sha1(const std::string_view input) -> std::string {
  // The platform marks its SHA-1 interfaces as deprecated because the
  // algorithm is cryptographically broken, but this module keeps exposing
  // SHA-1 for non-security use cases
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
  CC_SHA1_CTX context;
  CC_SHA1_Init(&context);

  // The platform update interface takes a 32-bit length, so larger
  // inputs must be fed in chunks
  const auto *remaining_data{input.data()};
  auto remaining_size{input.size()};
  constexpr std::size_t maximum_chunk{std::numeric_limits<CC_LONG>::max()};
  while (remaining_size > 0) {
    const auto chunk_size{remaining_size > maximum_chunk ? maximum_chunk
                                                         : remaining_size};
    CC_SHA1_Update(&context, remaining_data, static_cast<CC_LONG>(chunk_size));
    remaining_data += chunk_size;
    remaining_size -= chunk_size;
  }

  std::array<unsigned char, CC_SHA1_DIGEST_LENGTH> digest{};
  CC_SHA1_Final(digest.data(), &context);
#pragma clang diagnostic pop

  std::string result;
  result.reserve(40);
  for (std::uint64_t index = 0; index < 20u; ++index) {
    result.push_back(HEX_DIGITS[(digest[index] >> 4u) & 0x0fu]);
    result.push_back(HEX_DIGITS[digest[index] & 0x0fu]);
  }

  return result;
}

} // namespace sourcemeta::core

#elif defined(_WIN32)

namespace sourcemeta::core {

auto sha1(const std::string_view input) -> std::string {
  BCRYPT_ALG_HANDLE algorithm{nullptr};
  if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(
          &algorithm, BCRYPT_SHA1_ALGORITHM, nullptr, 0))) {
    throw std::runtime_error("Could not open the CNG SHA-1 provider");
  }

  BCRYPT_HASH_HANDLE hash{nullptr};
  if (!BCRYPT_SUCCESS(
          BCryptCreateHash(algorithm, &hash, nullptr, 0, nullptr, 0, 0))) {
    BCryptCloseAlgorithmProvider(algorithm, 0);
    throw std::runtime_error("Could not create the CNG SHA-1 hash");
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

  std::array<unsigned char, 20> digest{};
  if (success) {
    success = BCRYPT_SUCCESS(BCryptFinishHash(
        hash, digest.data(), static_cast<ULONG>(digest.size()), 0));
  }

  BCryptDestroyHash(hash);
  BCryptCloseAlgorithmProvider(algorithm, 0);
  if (!success) {
    throw std::runtime_error("Could not compute the CNG SHA-1 digest");
  }

  std::string result;
  result.reserve(40);
  for (std::uint64_t index = 0; index < 20u; ++index) {
    result.push_back(HEX_DIGITS[(digest[index] >> 4u) & 0x0fu]);
    result.push_back(HEX_DIGITS[digest[index] & 0x0fu]);
  }

  return result;
}

} // namespace sourcemeta::core

#else

namespace {

inline constexpr auto rotate_left(std::uint32_t value,
                                  std::uint64_t count) noexcept
    -> std::uint32_t {
  return (value << count) | (value >> (32u - count));
}

// Equivalent to (x & y) ^ (~x & z) but avoids a bitwise NOT
// (RFC 3174 Section 5, rounds 0 to 19)
inline constexpr auto choice(std::uint32_t x, std::uint32_t y,
                             std::uint32_t z) noexcept -> std::uint32_t {
  return z ^ (x & (y ^ z));
}

// RFC 3174 Section 5, rounds 20 to 39 and 60 to 79
inline constexpr auto parity(std::uint32_t x, std::uint32_t y,
                             std::uint32_t z) noexcept -> std::uint32_t {
  return x ^ y ^ z;
}

// RFC 3174 Section 5, rounds 40 to 59
inline constexpr auto majority(std::uint32_t x, std::uint32_t y,
                               std::uint32_t z) noexcept -> std::uint32_t {
  return (x & y) ^ (x & z) ^ (y & z);
}

inline auto sha1_process_block(const unsigned char *block,
                               std::array<std::uint32_t, 5> &state) noexcept
    -> void {
  // Decode 16 big-endian 32-bit words from the block
  std::array<std::uint32_t, 80> schedule;
  for (std::uint64_t word_index = 0; word_index < 16u; ++word_index) {
    const std::uint64_t byte_index = word_index * 4u;
    schedule[word_index] =
        (static_cast<std::uint32_t>(block[byte_index]) << 24u) |
        (static_cast<std::uint32_t>(block[byte_index + 1u]) << 16u) |
        (static_cast<std::uint32_t>(block[byte_index + 2u]) << 8u) |
        static_cast<std::uint32_t>(block[byte_index + 3u]);
  }

  // Extend the message schedule (RFC 3174 Section 6.1 step b)
  for (std::uint64_t index = 16u; index < 80u; ++index) {
    schedule[index] =
        rotate_left(schedule[index - 3u] ^ schedule[index - 8u] ^
                        schedule[index - 14u] ^ schedule[index - 16u],
                    1u);
  }

  auto working = state;

  // Compression function (RFC 3174 Section 6.1 step d), with the round
  // constants of RFC 3174 Section 5
  for (std::uint64_t round_index = 0u; round_index < 80u; ++round_index) {
    std::uint32_t function_value;
    std::uint32_t round_constant;
    if (round_index < 20u) {
      function_value = choice(working[1], working[2], working[3]);
      round_constant = 0x5a827999U;
    } else if (round_index < 40u) {
      function_value = parity(working[1], working[2], working[3]);
      round_constant = 0x6ed9eba1U;
    } else if (round_index < 60u) {
      function_value = majority(working[1], working[2], working[3]);
      round_constant = 0x8f1bbcdcU;
    } else {
      function_value = parity(working[1], working[2], working[3]);
      round_constant = 0xca62c1d6U;
    }

    const auto temporary = rotate_left(working[0], 5u) + function_value +
                           working[4] + schedule[round_index] + round_constant;

    working[4] = working[3];
    working[3] = working[2];
    working[2] = rotate_left(working[1], 30u);
    working[1] = working[0];
    working[0] = temporary;
  }

  for (std::uint64_t index = 0u; index < 5u; ++index) {
    state[index] += working[index];
  }
}

} // namespace

namespace sourcemeta::core {

auto sha1(const std::string_view input) -> std::string {
  // Initial hash values (RFC 3174 Section 6.1)
  std::array<std::uint32_t, 5> state{};
  state[0] = 0x67452301U;
  state[1] = 0xefcdab89U;
  state[2] = 0x98badcfeU;
  state[3] = 0x10325476U;
  state[4] = 0xc3d2e1f0U;

  const auto *const input_bytes =
      reinterpret_cast<const unsigned char *>(input.data());
  const std::size_t input_length = input.size();

  // Process all full 64-byte blocks directly from the input (streaming)
  std::size_t processed_bytes = 0u;
  while (input_length - processed_bytes >= 64u) {
    sha1_process_block(input_bytes + processed_bytes, state);
    processed_bytes += 64u;
  }

  // Prepare the final block(s) (one or two 64-byte blocks)
  std::array<unsigned char, 128> final_block{};
  const std::size_t remaining_bytes = input_length - processed_bytes;
  if (remaining_bytes > 0u) {
    std::memcpy(final_block.data(), input_bytes + processed_bytes,
                remaining_bytes);
  }

  // Append the 0x80 byte after the message data (RFC 3174 Section 4)
  final_block[remaining_bytes] = 0x80u;

  // Append length in bits as big-endian 64-bit at the end of the padding
  const std::uint64_t message_length_bits =
      static_cast<std::uint64_t>(input_length) * 8ull;

  if (remaining_bytes < 56u) {
    for (std::uint64_t index = 0u; index < 8u; ++index) {
      final_block[56u + index] = static_cast<unsigned char>(
          (message_length_bits >> (8u * (7u - index))) & 0xffu);
    }
    sha1_process_block(final_block.data(), state);
  } else {
    for (std::uint64_t index = 0u; index < 8u; ++index) {
      final_block[64u + 56u + index] = static_cast<unsigned char>(
          (message_length_bits >> (8u * (7u - index))) & 0xffu);
    }

    sha1_process_block(final_block.data(), state);
    sha1_process_block(final_block.data() + 64u, state);
  }

  std::string result;
  result.reserve(40);
  for (std::uint64_t state_index = 0u; state_index < 5u; ++state_index) {
    const auto value = state[state_index];
    for (std::uint64_t nibble = 0u; nibble < 8u; ++nibble) {
      const auto shift = 28u - nibble * 4u;
      result.push_back(HEX_DIGITS[(value >> shift) & 0x0fu]);
    }
  }

  return result;
}

} // namespace sourcemeta::core

#endif

namespace sourcemeta::core {

auto sha1(const std::string_view input, std::ostream &output) -> void {
  const auto result = sha1(input);
  output.write(result.data(), static_cast<std::streamsize>(result.size()));
}

} // namespace sourcemeta::core
